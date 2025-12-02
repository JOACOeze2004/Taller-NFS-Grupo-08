#include <gtest/gtest.h>
#include "../src/common/mocked_socket.h"
#include "../src/common/protocol.h"
#include "../src/client/client_protocol.h"

TEST(ClientProtocolTest, ReceiveGamesList) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data;

    data.push_back(0x00);
    data.push_back(0x03);
    
    data.push_back(0x00);
    data.push_back(0x01);
    data.insert(data.end(), {'1'});
    
    data.push_back(0x00);
    data.push_back(0x01);
    data.insert(data.end(), {'2'});
    
    data.push_back(0x00);
    data.push_back(0x01);
    data.insert(data.end(), {'3'});
    
    socket.inject_bytes(data);

    auto games = client_proto.receive_games_list();
    
    ASSERT_EQ(games.size(), 3);
    EXPECT_EQ(games[0], "1");
    EXPECT_EQ(games[1], "2");
    EXPECT_EQ(games[2], "3");
}

TEST(ClientProtocolTest, ReceiveGamesListEmpty) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data = {0x00, 0x00};
    socket.inject_bytes(data);

    auto games = client_proto.receive_games_list();
    EXPECT_EQ(games.size(), 0);
}

TEST(ClientProtocolTest, SendLobbyActionCreate) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    client_proto.send_lobby_action(SEND_CREATE_GAME, "");
    
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 1);
    EXPECT_EQ(written[0], SEND_CREATE_GAME);
}

TEST(ClientProtocolTest, SendLobbyActionJoin) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::string game_id = "1";
    client_proto.send_lobby_action(SEND_JOIN_GAME, game_id);
    
    auto written = socket.get_written_bytes();
    
    ASSERT_EQ(written.size(), 1 + 2 + game_id.size());
    EXPECT_EQ(written[0], SEND_JOIN_GAME);
    EXPECT_EQ(written[1], 0x00);
    EXPECT_EQ(written[2], 0x01);
    
    std::string received_id(written.begin() + 3, written.end());
    EXPECT_EQ(received_id, game_id);
}

TEST(ClientProtocolTest, SendPlayerConfig) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::string name = "Joaco";
    uint8_t car_id = 3;
    std::string map_name = "VICE CITY";

    client_proto.send_player_config(name, car_id, map_name);
    
    auto written = socket.get_written_bytes();
    
    size_t expected_size = 2 + name.size() + 1 + 2 + map_name.size();
    ASSERT_EQ(written.size(), expected_size);
    
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x05);
    
    size_t car_id_pos = 2 + name.size();
    EXPECT_EQ(written[car_id_pos], car_id);
}

TEST(ClientProtocolTest, ReceiveGameInitData) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::string expected_path = "VICE CITY";
    float expected_x = 100.5f;
    float expected_y = 200.75f;

    std::vector<uint8_t> data;
    
    uint16_t path_len = expected_path.size();
    data.push_back((path_len >> 8) & 0xFF);
    data.push_back(path_len & 0xFF);
    data.insert(data.end(), expected_path.begin(), expected_path.end());
    
    uint32_t x_bytes;
    std::memcpy(&x_bytes, &expected_x, sizeof(float));
    data.push_back((x_bytes >> 24) & 0xFF);
    data.push_back((x_bytes >> 16) & 0xFF);
    data.push_back((x_bytes >> 8) & 0xFF);
    data.push_back(x_bytes & 0xFF);
    
    uint32_t y_bytes;
    std::memcpy(&y_bytes, &expected_y, sizeof(float));
    data.push_back((y_bytes >> 24) & 0xFF);
    data.push_back((y_bytes >> 16) & 0xFF);
    data.push_back((y_bytes >> 8) & 0xFF);
    data.push_back(y_bytes & 0xFF);
    
    socket.inject_bytes(data);

    std::string map_path;
    float spawn_x, spawn_y;
    client_proto.receive_game_init_data(map_path, spawn_x, spawn_y);
    
    EXPECT_EQ(map_path, expected_path);
    EXPECT_FLOAT_EQ(spawn_x, expected_x);
    EXPECT_FLOAT_EQ(spawn_y, expected_y);
}

TEST(ClientProtocolTest, ReceiveErrorMessage) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::string error_msg = "Connection failed";
    
    std::vector<uint8_t> data;
    uint16_t msg_len = error_msg.size();
    data.push_back((msg_len >> 8) & 0xFF);
    data.push_back(msg_len & 0xFF);
    data.insert(data.end(), error_msg.begin(), error_msg.end());
    
    socket.inject_bytes(data);

    std::string received = client_proto.receive_error_message();
    EXPECT_EQ(received, error_msg);
}

TEST(ClientProtocolTest, ReceiveErrorMessageEmpty) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data = {0x00, 0x00};
    socket.inject_bytes(data);

    std::string received = client_proto.receive_error_message();
    EXPECT_EQ(received, "");
}

TEST(ClientProtocolTest, ReceiveGameStateMinimal) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data;

    data.push_back(0x01);

    data.push_back(0x00);
    data.push_back(0x00);

    data.push_back(0x05);
    data.push_back(0x01);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    
    for (int i = 0; i < 5; ++i) {
        float f = 0.0f;
        uint32_t fb;
        std::memcpy(&fb, &f, sizeof(float));
        uint32_t fb_net = htonl(fb);
        data.push_back((fb_net >> 24) & 0xFF);
        data.push_back((fb_net >> 16) & 0xFF);
        data.push_back((fb_net >> 8) & 0xFF);
        data.push_back(fb_net & 0xFF);
    }
    data.push_back(0x00);

    uint16_t total = htons(25);
    data.push_back(total >> 8);
    data.push_back(total & 0xFF);

    uint16_t current = htons(3);
    data.push_back(current >> 8);
    data.push_back(current & 0xFF);

    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    
    data.push_back(0x00);
    data.push_back(0x00);
    
    data.push_back(0x00);
    data.push_back(0x00);
    
    data.push_back(0x00);
    data.push_back(0x00);

    data.push_back(0x00);
    data.push_back(0x00);
    
    data.push_back(0x00);
    data.push_back(0x00);

    socket.inject_bytes(data);

    Snapshot snapshot = client_proto.receive_game_state();
    
    EXPECT_EQ(snapshot.id, 0x01);
    EXPECT_EQ(snapshot.cars.size(), 0);
    EXPECT_EQ(snapshot.game_id, 0x05);
    EXPECT_EQ(snapshot.position, 0x01);
    
    EXPECT_EQ(snapshot.cars_finished.size(), 0);
    EXPECT_EQ(snapshot.player_total_times.size(), 0);
    EXPECT_EQ(snapshot.upgrades.size(), 0);
}

TEST(ClientProtocolTest, ReceiveFinalResultsEmpty) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data = {0x00, 0x00};
    socket.inject_bytes(data);

    FinalScoreList results = client_proto.receive_final_results();
    EXPECT_EQ(results.size(), 0);
}

TEST(ClientProtocolTest, ReceiveFinalResults) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    std::vector<uint8_t> data;
    
    data.push_back(0x00);
    data.push_back(0x02);
    
    std::string name1 = "Joaco";
    data.push_back(0x00);
    data.push_back(name1.size());
    data.insert(data.end(), name1.begin(), name1.end());
    
    float time1 = 125.5f;
    uint32_t time1_as_int;
    std::memcpy(&time1_as_int, &time1, sizeof(float));
    data.push_back((time1_as_int >> 24) & 0xFF);
    data.push_back((time1_as_int >> 16) & 0xFF);
    data.push_back((time1_as_int >> 8) & 0xFF);
    data.push_back(time1_as_int & 0xFF);
    data.push_back(1);

    std::string name2 = "Pepe";
    data.push_back(0x00);
    data.push_back(name2.size());
    data.insert(data.end(), name2.begin(), name2.end());
    
    float time2 = 130.2f;
    uint32_t time2_as_int;
    std::memcpy(&time2_as_int, &time2, sizeof(float));
    data.push_back((time2_as_int >> 24) & 0xFF);
    data.push_back((time2_as_int >> 16) & 0xFF);
    data.push_back((time2_as_int >> 8) & 0xFF);
    data.push_back(time2_as_int & 0xFF);
    data.push_back(2);
    
    socket.inject_bytes(data);

    FinalScoreList results = client_proto.receive_final_results();
    
    ASSERT_EQ(results.size(), 2);
    EXPECT_EQ(results[0].name, "Joaco");
    EXPECT_FLOAT_EQ(results[0].time, 125.5f);
    EXPECT_EQ(results[0].position, 1);
    
    EXPECT_EQ(results[1].name, "Pepe");
    EXPECT_FLOAT_EQ(results[1].time, 130.2f);
    EXPECT_EQ(results[1].position, 2);
}

TEST(ClientProtocolTest, CloseSocket) {
    MockedSocket socket;
    ClientProtocol client_proto(socket);

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    client_proto.close();

    EXPECT_TRUE(socket.is_stream_send_closed());
    EXPECT_TRUE(socket.is_stream_recv_closed());
}