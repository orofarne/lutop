#include "Encode.hpp"

#include <msgpack.h>
#include "msglen.h"

#include <string.h>

#include <stdexcept>

#define ASSERTPP(expr) \
    if (!(expr)) { \
        std::string msg = "Assertion failed: "; \
        msg += #expr; \
        throw std::runtime_error(msg); \
    }

namespace lutop {

std::string encodeRequest(const Request &r) {
    std::shared_ptr<msgpack_sbuffer> buffer{msgpack_sbuffer_new(), &msgpack_sbuffer_free};
    std::shared_ptr<msgpack_packer> pk{msgpack_packer_new(buffer.get(), msgpack_sbuffer_write), &msgpack_packer_free};

    msgpack_pack_map(pk.get(), 5);
    msgpack_pack_raw(pk.get(), 4);
    msgpack_pack_raw_body(pk.get(), "func", 4);
    msgpack_pack_raw(pk.get(), r.func.size());
    msgpack_pack_raw_body(pk.get(), r.func.data(), r.func.size());
    msgpack_pack_raw(pk.get(), 6);
    msgpack_pack_raw_body(pk.get(), "metric", 6);
    msgpack_pack_raw(pk.get(), r.metric.size());
    msgpack_pack_raw_body(pk.get(), r.metric.data(), r.metric.size());
    msgpack_pack_raw(pk.get(), 9);
    msgpack_pack_raw_body(pk.get(), "timestamp", 9);
    msgpack_pack_uint64(pk.get(), r.timestamp);

    msgpack_pack_raw(pk.get(), 5);
    msgpack_pack_raw_body(pk.get(), "value", 5);
    switch(r.value.type()) {
        case Value::Type::Nil:
            msgpack_pack_nil(pk.get());
            break;
        case Value::Type::Boolean:
            if(r.value.as<bool>())
                msgpack_pack_true(pk.get());
            else
                msgpack_pack_false(pk.get());
            break;
        case Value::Type::Numeric:
            msgpack_pack_double(pk.get(), r.value.as<double>());
            break;
        case Value::Type::String:
            {
                std::string b = r.value.as<std::string>();
                msgpack_pack_raw(pk.get(), b.size());
                msgpack_pack_raw_body(pk.get(), b.data(), b.size());
            }
            break;
        case Value::Type::Table:
            {
                // Hack
                std::string b = r.value.as<std::string>();
                if(b.empty())
                    msgpack_pack_nil(pk.get());
                else
                    msgpack_sbuffer_write(reinterpret_cast<void *>(buffer.get()), b.data(), b.size());
            }
            break;
    }

    msgpack_pack_raw(pk.get(), 5);
    msgpack_pack_raw_body(pk.get(), "state", 5);
    if(r.state.empty())
        msgpack_pack_nil(pk.get());
    else
        msgpack_sbuffer_write(reinterpret_cast<void *>(buffer.get()), r.state.data(), r.state.size());

    return std::string(buffer->data, buffer->size);
}

std::shared_ptr<Response> decodeResponse(std::string const &buf) {
    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    ASSERTPP( msgpack_unpack_next(&msg, buf.data(), buf.size(), nullptr) );
    msgpack_object obj = msg.data;
    ASSERTPP(obj.type == MSGPACK_OBJECT_MAP);

    std::shared_ptr<Response> r{new Response};

    for(int i = 0; i < obj.via.map.size; ++i) {
        msgpack_object key_obj = obj.via.map.ptr[i].key;
        msgpack_object val = obj.via.map.ptr[i].val;

        ASSERTPP(key_obj.type == MSGPACK_OBJECT_RAW);
        msgpack_object_raw key = key_obj.via.raw;

        if(key.size == 5 && 0 == strncmp("state", key.ptr, 5)) {
            if(val.type == MSGPACK_OBJECT_NIL) {
                // do nosing
            }
            else {
                const char *ptr = key.ptr + key.size;
                char *err = nullptr;
                size_t max_len = buf.size() -
                    (reinterpret_cast<const char *>(ptr) - reinterpret_cast<const char *>(buf.data()));
                void *void_ptr = const_cast<void *>(reinterpret_cast<const void *>(ptr));
                size_t m_len = ::msgpackclen_buf_read(void_ptr, max_len, &err);
                if(err)
                    throw std::runtime_error(std::string("msgpackclen_buf_read error: ") + err);

                r->state.assign(ptr, m_len);
            }
        }
        else if(key.size == 5 && 0 == strncmp("value", key.ptr, 5)) {
            switch(val.type) {
                case MSGPACK_OBJECT_NIL:
                    r->value = Value();
                    break;
                case MSGPACK_OBJECT_BOOLEAN:
                    r->value = Value(Value::Type::Boolean, (bool)val.via.boolean);
                    break;
                case MSGPACK_OBJECT_POSITIVE_INTEGER:
                    r->value = Value(Value::Type::Numeric, val.via.u64);
                    break;
                case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                    r->value = Value(Value::Type::Numeric, val.via.i64);
                    break;
                case MSGPACK_OBJECT_DOUBLE:
                    r->value = Value(Value::Type::Numeric, val.via.dec);
                    break;
                case MSGPACK_OBJECT_RAW:
                    r->value = Value(Value::Type::String, std::string(val.via.raw.ptr, val.via.raw.size));
                    break;
                case MSGPACK_OBJECT_ARRAY:
                case MSGPACK_OBJECT_MAP:
                    {
                        const char *ptr = key.ptr + key.size;
                        char *err = nullptr;
                        size_t max_len = buf.size() -
                            (reinterpret_cast<const char *>(ptr) - reinterpret_cast<const char *>(buf.data()));
                        void *void_ptr = const_cast<void *>(reinterpret_cast<const void *>(ptr));
                        size_t m_len = ::msgpackclen_buf_read(void_ptr, max_len, &err);
                        if(err)
                            throw std::runtime_error(std::string("msgpackclen_buf_read error: ") + err);

                        r->value = Value(Value::Type::Table, std::string(ptr, m_len));
                    }
                    break;

            }
        }
        else if(key.size == 9 && 0 == strncmp("timestamp", key.ptr, 9)) {
            switch(val.type) {
                case MSGPACK_OBJECT_POSITIVE_INTEGER:
                    r->timestamp = val.via.u64;
                    break;
                case MSGPACK_OBJECT_NEGATIVE_INTEGER:
                    r->timestamp = val.via.i64;
                    break;
                case MSGPACK_OBJECT_DOUBLE:
                    r->timestamp = val.via.dec;
                    break;
                default:
                    throw std::runtime_error("Invalid type for timestamp");
            }
        }
        else if(key.size == 5 && 0 == strncmp("error", key.ptr, 5)) {
            ASSERTPP(val.type == MSGPACK_OBJECT_RAW);
            r->error.assign(val.via.raw.ptr, val.via.raw.size);
        }

    }

    return r;
}

}
