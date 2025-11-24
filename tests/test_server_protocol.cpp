#include <gtest/gtest.h>
#include "../src/common/mocked_socket.h"
#include "../src/common/protocol.h"
#include "../src/server/server_protocol.h"

TEST(ServerProtocolTest, ReceiveStandardCommand) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    socket.inject_bytes({SEND_CREATE_GAME});
    EXPECT_EQ(server_proto.receive_standar_command(), SEND_CREATE_GAME);
    
    socket.inject_bytes({SEND_JOIN_GAME});
    EXPECT_EQ(server_proto.receive_standar_command(), SEND_JOIN_GAME);
}


TEST(ServerProtocolTest, SendGamesList) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    std::vector<std::string> games = {"1", "2"};
    server_proto.send_games_list(games);
    
    auto written = socket.get_written_bytes();
    
    ASSERT_GE(written.size(), 4); 
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x02);
    EXPECT_EQ(written[2], 0x00);
    EXPECT_EQ(written[3], 0x01);
}

TEST(ServerProtocolTest, SendGamesListEmpty) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    std::vector<std::string> games;
    server_proto.send_games_list(games);
    
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 2);
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x00);
}

TEST(ServerProtocolTest, ReceivePlayerConfig) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    std::vector<uint8_t> data;
    std::string name = "Joaco";
    data.push_back(0x00);
    data.push_back(name.size());
    data.insert(data.end(), name.begin(), name.end());
    data.push_back(0x05); 
    std::string map = "VICE CITY";
    data.push_back(0x00);
    data.push_back(map.size());
    data.insert(data.end(), map.begin(), map.end());
    
    socket.inject_bytes(data);

    std::string recv_name, recv_map;
    uint8_t recv_car_id;
    server_proto.receive_player_config(recv_name, recv_car_id, recv_map);
    
    ASSERT_FALSE(recv_name.empty());
    ASSERT_FALSE(recv_map.empty());
    EXPECT_EQ(recv_name, "Joaco");
    EXPECT_EQ(recv_car_id, 5);
    EXPECT_EQ(recv_map, "VICE CITY");
}

TEST(ServerProtocolTest, SendGameInitData) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    server_proto.send_game_init_data("VICE CITY", 50.0f, 100.0f);
    
    auto written = socket.get_written_bytes();
    
    ASSERT_EQ(written.size(), 19);
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x09); 
}

TEST(ServerProtocolTest, ReceiveLobbyActionCreate) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    socket.inject_bytes({SEND_CREATE_GAME});
    
    uint8_t action;
    std::string game_id;
    server_proto.receive_lobby_action(action, game_id);
    
    EXPECT_EQ(action, SEND_CREATE_GAME);
    EXPECT_TRUE(game_id.empty());
}

TEST(ServerProtocolTest, ReceiveLobbyActionJoin) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    std::vector<uint8_t> data;
    data.push_back(SEND_JOIN_GAME);
    std::string game_id = "123";
    data.push_back(0x00);
    data.push_back(game_id.size());
    data.insert(data.end(), game_id.begin(), game_id.end());
    
    socket.inject_bytes(data);
    
    uint8_t action;
    std::string recv_game_id;
    server_proto.receive_lobby_action(action, recv_game_id);
    
    EXPECT_EQ(action, SEND_JOIN_GAME);
    EXPECT_EQ(recv_game_id, "123");
}

TEST(ServerProtocolTest, SendGameStateMinimal) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    Snapshot snapshot;
    snapshot.id = 1;
    snapshot.game_id = 5;
    snapshot.position = 2;
    snapshot.map = static_cast<Map>(0);
    snapshot.upgrade = static_cast<Upgrades>(0);
    snapshot.upgradeable = false;
    snapshot.collision = static_cast<CollisionType>(0);
    snapshot.checkpoint = {0.0f, 0.0f};
    snapshot.hint = {0.0f, 0.0f, 0.0f};
    snapshot.type_checkpoint = static_cast<TypeCheckpoint>(0);
    snapshot.time_ms = 0;
    snapshot.state = static_cast<State>(0);
    snapshot.is_owner = false;

    server_proto.send_game_state(snapshot);
    
    auto written = socket.get_written_bytes();
    
    ASSERT_GE(written.size(), 3);
    EXPECT_EQ(written[0], 1); 
    EXPECT_EQ(written[1], 0x00);
    EXPECT_EQ(written[2], 0x00); 
}

TEST(ServerProtocolTest, SendErrorMessage) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    std::string error = "Error test";
    server_proto.send_error_message(error);
    
    auto written = socket.get_written_bytes();
    
    EXPECT_EQ(written[0], SEND_ERROR_MESSAGE);
    EXPECT_EQ(written[1], 0x00);
    EXPECT_EQ(written[2], error.size());
    
    std::string recv_msg(written.begin() + 3, written.end());
    EXPECT_EQ(recv_msg, error);
}

TEST(ServerProtocolTest, SendErrorMessageEmpty) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    server_proto.send_error_message("");
    
    auto written = socket.get_written_bytes();
    ASSERT_GE(written.size(), 3);
    EXPECT_EQ(written[2], 0x00);
}

TEST(ServerProtocolTest, SendOk) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    server_proto.send_ok();
    
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 1);
    EXPECT_EQ(written[0], SEND_OK_MESSAGE);
}

TEST(ServerProtocolTest, SendFinalResultsEmpty) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    FinalScoreList results;
    server_proto.send_final_results(results);
    
    auto written = socket.get_written_bytes();
    ASSERT_EQ(written.size(), 2);
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x00);
}

TEST(ServerProtocolTest, SendFinalResults) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    FinalScoreList results;
    CarRacingInfo result;
    result.name = "Pepe";
    result.time = 100.5f;
    result.position = 1;
    results.push_back(result);

    server_proto.send_final_results(results);
    
    auto written = socket.get_written_bytes();
    
    EXPECT_EQ(written[0], 0x00);
    EXPECT_EQ(written[1], 0x01); 
    EXPECT_EQ(written[2], 0x00);
    EXPECT_EQ(written[3], 0x04);
}

TEST(ServerProtocolTest, CloseSocket) {
    MockedSocket socket;
    ServerProtocol server_proto(socket);

    EXPECT_FALSE(socket.is_stream_send_closed());
    EXPECT_FALSE(socket.is_stream_recv_closed());

    server_proto.close();

    EXPECT_TRUE(socket.is_stream_send_closed());
    EXPECT_TRUE(socket.is_stream_recv_closed());
}