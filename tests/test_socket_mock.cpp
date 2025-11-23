#include <gtest/gtest.h>
#include "../src/common/mocked_socket.h"

TEST(MockedSocketTest, SendStoresBytesInWriteBuffer) {
    MockedSocket socket;

    std::vector<uint8_t> data = {0x01, 0x02, 0x03};
    int sent = socket.sendall(data.data(), data.size());

    EXPECT_EQ(sent, 3);

    auto written = socket.get_written_bytes();
    EXPECT_EQ(written, data);
}

TEST(MockedSocketTest, RecvReadsInjectedBytes) {
    MockedSocket socket;

    socket.inject_bytes({0x0A, 0x0B, 0x0C});

    uint8_t buffer[3];
    int received = socket.recvall(buffer, 3);

    EXPECT_EQ(received, 3);
    EXPECT_EQ(std::vector<uint8_t>(buffer, buffer + 3), (std::vector<uint8_t>{0x0A, 0x0B, 0x0C}));
}

TEST(MockedSocketTest, RecvPartialWhenNotEnoughData) {
    MockedSocket socket;

    socket.inject_bytes({0xFF});

    uint8_t buffer[3] = {0};
    int received = socket.recvall(buffer, 3);

    EXPECT_EQ(received, 1);
    EXPECT_EQ(buffer[0], 0xFF);
    EXPECT_EQ(buffer[1], 0x00);
    EXPECT_EQ(buffer[2], 0x00);
}

TEST(MockedSocketTest, ShutdownClosesStreams) {
    MockedSocket socket;

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    socket.shutdown(2);

    EXPECT_TRUE(socket.is_stream_send_closed());
    EXPECT_TRUE(socket.is_stream_recv_closed());
}

TEST(MockedSocketTest, ShutdownClosesLectureStream) {
    MockedSocket socket;

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    socket.shutdown(0); 
    EXPECT_TRUE(socket.is_stream_recv_closed());
    EXPECT_FALSE(socket.is_stream_send_closed());
}

TEST(MockedSocketTest, ShutdownClosesWriteStream) {
    MockedSocket socket;

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    socket.shutdown(1); 
    EXPECT_FALSE(socket.is_stream_recv_closed());
    EXPECT_TRUE(socket.is_stream_send_closed());
}

TEST(MockedSocketTest, CloseClosesStreams) {
    MockedSocket socket;

    socket.close();

    EXPECT_TRUE(socket.is_stream_send_closed());
    EXPECT_TRUE(socket.is_stream_recv_closed());
}