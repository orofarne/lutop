#include "Encode.hpp"

#include <msgpack.h>

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

    std::shared_ptr<Response> r{new Response};

    return r;
}

}
