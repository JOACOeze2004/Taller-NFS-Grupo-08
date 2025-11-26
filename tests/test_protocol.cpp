#include <gtest/gtest.h>
#include "../src/common/mocked_socket.h"
#include "../src/common/protocol.h"


TEST(ProtocolTest, SendAndReceiveOneByte) {
    MockedSocket socket;
    Protocol proto(socket);

    proto.send_byte(0xAB);
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 1);
    EXPECT_EQ(written[0], 0xAB);

    socket.inject_bytes({0xCD});
    uint8_t b = proto.receive_byte();
    EXPECT_EQ(b, 0xCD);
}

TEST(ProtocolTest, SendAndReceiveBigEndian16) {
    MockedSocket socket;
    Protocol proto(socket);

    proto.send_big_endian_16(0x1234);
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 2);
    EXPECT_EQ(written[0], 0x12);
    EXPECT_EQ(written[1], 0x34);

    socket.inject_bytes({0x56, 0x78});
    uint16_t val = proto.receive_big_endian_16();
    EXPECT_EQ(val, 0x5678);
}

TEST(ProtocolTest, SendAndReceiveBigEndian32) {
    MockedSocket socket;
    Protocol proto(socket);

    proto.send_big_endian_32(0x11223344);
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 4);
    EXPECT_EQ(written[0], 0x11);
    EXPECT_EQ(written[1], 0x22);
    EXPECT_EQ(written[2], 0x33);
    EXPECT_EQ(written[3], 0x44);

    socket.inject_bytes({0x55, 0x66, 0x77, 0x88});
    uint32_t val = proto.receive_big_endian_32();
    EXPECT_EQ(val, 0x55667788);
}

TEST(ProtocolTest, SendAndReceiveBigEndian64) {
    MockedSocket socket;
    Protocol proto(socket);

    uint64_t number = 0x1122334455667788;
    proto.send_big_endian_64(number);
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 8);
    EXPECT_EQ(written[0], 0x11);
    EXPECT_EQ(written[7], 0x88);

    socket.inject_bytes({0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,0x00});
    uint64_t val = proto.receive_big_endian_64();
    EXPECT_EQ(val, 0x99AABBCCDDEEFF00);
}

TEST(ProtocolTest, SendAndReceiveFloat) {
    MockedSocket socket;
    Protocol proto(socket);

    float f = 3.14f;
    proto.send_float(f);
    ASSERT_EQ(socket.get_written_bytes().size(), 4);

    uint32_t raw;
    std::memcpy(&raw, socket.get_written_bytes().data(), sizeof(raw));

    uint32_t f_bits;
    std::memcpy(&f_bits, &f, sizeof(f));
    EXPECT_EQ(raw, htonl(f_bits));

    socket.inject_bytes(socket.get_written_bytes());
    float f2 = proto.receive_float();
    EXPECT_FLOAT_EQ(f, f2);
}

TEST(ProtocolTest, SendAndReceiveString) {
    MockedSocket socket;
    Protocol proto(socket);

    std::string msg = "Hello";
    proto.send_string(msg);
    auto written = socket.get_written_bytes();
    EXPECT_EQ(std::string(written.begin(), written.end()), msg);

    socket.inject_bytes(written);
    std::string recv_msg = proto.receive_string(written.size());
    EXPECT_EQ(recv_msg, msg);
}

TEST(ProtocolTest, SendAndReceiveBool) {
    MockedSocket socket;
    Protocol proto(socket);

    proto.send_bool(true);
    proto.send_bool(false);
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 2);
    EXPECT_EQ(written[0], 1);
    EXPECT_EQ(written[1], 0);

    socket.inject_bytes({1, 0});
    EXPECT_TRUE(proto.receive_bool());
    EXPECT_FALSE(proto.receive_bool());
}

TEST(ProtocolTest, CloseSocketClosesAllStreams) {
    MockedSocket socket;
    Protocol proto(socket);

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    proto.close_socket();

    EXPECT_TRUE(socket.is_stream_send_closed());
    EXPECT_TRUE(socket.is_stream_recv_closed());
}