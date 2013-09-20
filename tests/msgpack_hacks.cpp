#include <gtest/gtest.h>

#include <string.h>

#include <memory>

#include <msgpack.h>
#include "msglen.h"

TEST(msgpack_hacks, pack_buffer) {
    std::shared_ptr<msgpack_sbuffer> buffer{msgpack_sbuffer_new(), &msgpack_sbuffer_free};
    std::shared_ptr<msgpack_packer> pk{msgpack_packer_new(buffer.get(), msgpack_sbuffer_write), &msgpack_packer_free};

    msgpack_pack_array(pk.get(), 3);
    msgpack_pack_raw(pk.get(), 5);
    msgpack_pack_raw_body(pk.get(), "Hello", 5);
    msgpack_pack_raw(pk.get(), 11);
    msgpack_pack_raw_body(pk.get(), "MessagePack", 11);

    // Hack!
    char buf[] = {'\xa5', 'H', 'e', 'l', 'l', 'o'};
    size_t n = 6;
    msgpack_sbuffer_write(reinterpret_cast<void *>(buffer.get()), buf, n);

    // Check it!
    // array: 10010011 -> 0x93
    // raw: 10100101 -> 0xa5
    //      10101011 -> 0xab
    char res[] = "\x93\xa5Hello\xabMessagePack\xa5Hello";
    size_t res_len = strlen(res);
    ASSERT_EQ(res_len, buffer->size);
    ASSERT_EQ(0, strncmp(res, buffer->data, res_len));
}

TEST(msgpack_hacks, unpack_buffer) {
    char res[] = "\x93\xa5Hello\xabMessagePack\xa5Hello";
    size_t res_len = strlen(res);

    msgpack_unpacked msg;
    msgpack_unpacked_init(&msg);
    bool success = msgpack_unpack_next(&msg, res, res_len, nullptr);
    ASSERT_TRUE(success);

    msgpack_object obj = msg.data;
    ASSERT_EQ(MSGPACK_OBJECT_ARRAY, obj.type);
    msgpack_object_array arr = obj.via.array;
    ASSERT_EQ(3, arr.size);

    // Hack!
    ASSERT_EQ(MSGPACK_OBJECT_RAW, arr.ptr[0].type);
    void *ptr = const_cast<void *>(reinterpret_cast<const void *>(arr.ptr[0].via.raw.ptr));
    ASSERT_TRUE(reinterpret_cast<size_t>(ptr) > reinterpret_cast<size_t>(res));
    ASSERT_TRUE(reinterpret_cast<size_t>(ptr) < reinterpret_cast<size_t>(res) + reinterpret_cast<size_t>(res_len));

    size_t max_len = res_len - reinterpret_cast<size_t>(res) + reinterpret_cast<size_t>(ptr);
    size_t offset = arr.ptr[0].via.raw.size;
    ASSERT_NE(0, offset);
    ASSERT_TRUE(offset < max_len);

    ptr = reinterpret_cast<void *>(reinterpret_cast<char *>(ptr) + offset);
    max_len -= offset;

    ASSERT_TRUE(reinterpret_cast<size_t>(ptr) < reinterpret_cast<size_t>(res) + reinterpret_cast<size_t>(res_len));
    char *err = nullptr;
    size_t part_size = msgpackclen_buf_read(ptr, max_len, &err);
    ASSERT_TRUE(err == nullptr);
    ASSERT_NE(0, offset);
    ASSERT_TRUE(offset < max_len);

    char res_part[] = "\xabMessagePack";
    size_t res_part_len = strlen(res_part);
    ASSERT_EQ(res_part_len, part_size);
    ASSERT_EQ(0, strncmp(res_part, reinterpret_cast<char *>(ptr), res_part_len));
}
