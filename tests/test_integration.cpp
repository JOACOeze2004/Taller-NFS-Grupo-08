#include <gtest/gtest.h>
#include "../src/common/mocked_socket.h"
#include "../src/common/protocol.h"
#include "../src/client/client_protocol.h"
#include "../src/server/server_protocol.h"

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesGamesList) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    
    std::vector<std::string> games = {"1", "2", "3"};
    server_proto.send_games_list(games);
    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    ClientProtocol client_proto(client_socket);
    auto received_games = client_proto.receive_games_list();
    
    ASSERT_EQ(received_games.size(), 3);
    EXPECT_EQ(received_games[0], "1");
    EXPECT_EQ(received_games[1], "2");
    EXPECT_EQ(received_games[2], "3");
}

TEST(ProtocolIntegrationTest, ClientSendsServerReceivesPlayerConfig) {
    MockedSocket client_socket;
    MockedSocket server_socket;
    
    ClientProtocol client_proto(client_socket);
    std::string name = "Joaco";
    uint8_t car_id = 5;
    std::string map = "VICE CITY";
    
    client_proto.send_player_config(name, car_id, map);   
    auto data = client_socket.get_written_bytes();
    server_socket.inject_bytes(data);    
    ServerProtocol server_proto(server_socket);
    std::string recv_name, recv_map;
    uint8_t recv_car_id;
    
    server_proto.receive_player_config(recv_name, recv_car_id, recv_map);
    
    EXPECT_EQ(recv_name, name);
    EXPECT_EQ(recv_car_id, car_id);
    EXPECT_EQ(recv_map, map);
}

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesGameInitData) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);  
    std::string map_path = "VICE CITY";
    float spawn_x = 123.45f;
    float spawn_y = 678.90f;    
    server_proto.send_game_init_data(map_path, spawn_x, spawn_y);  
    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    
    ClientProtocol client_proto(client_socket);
    std::string recv_path;
    float recv_x, recv_y;
    
    client_proto.receive_game_init_data(recv_path, recv_x, recv_y);
    
    EXPECT_EQ(recv_path, map_path);
    EXPECT_FLOAT_EQ(recv_x, spawn_x);
    EXPECT_FLOAT_EQ(recv_y, spawn_y);
}

TEST(ProtocolIntegrationTest, ClientSendsServerReceivesLobbyAction) {
    MockedSocket client_socket;
    MockedSocket server_socket;
    
    ClientProtocol client_proto(client_socket);   
    std::string game_id = "1";
    client_proto.send_lobby_action(SEND_JOIN_GAME, game_id); 
    auto data = client_socket.get_written_bytes();
    server_socket.inject_bytes(data);
    
    ServerProtocol server_proto(server_socket);
    uint8_t action;
    std::string recv_game_id;
    
    server_proto.receive_lobby_action(action, recv_game_id);
    
    EXPECT_EQ(action, SEND_JOIN_GAME);
    EXPECT_EQ(recv_game_id, game_id);
}

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesErrorMessage) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    
    std::string error = "Game is full (8/8)";
    server_proto.send_error_message(error);
    
    auto data = server_socket.get_written_bytes();
    std::vector<uint8_t> msg_data(data.begin() + 1, data.end());
    client_socket.inject_bytes(msg_data);
    
    ClientProtocol client_proto(client_socket);
    std::string recv_error = client_proto.receive_error_message();
    
    EXPECT_EQ(recv_error, error);
}

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesOk) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    server_proto.send_ok();
    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    
    ClientProtocol client_proto(client_socket);
    uint8_t response = client_proto.receive_byte();
    
    EXPECT_EQ(response, SEND_OK_MESSAGE);
}

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesFinalResults) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    
    FinalScoreList results; 
    CarRacingInfo r1;
    r1.name = "Joaco";
    r1.time = 100.5f;
    r1.position = 1;
    results.push_back(r1);

    CarRacingInfo r2;
    r2.name = "Pepe";
    r2.time = 105.3f;
    r2.position = 2;
    results.push_back(r2);
    server_proto.send_final_results(results);

    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    
    ClientProtocol client_proto(client_socket);
    FinalScoreList recv_results = client_proto.receive_final_results();
    
    ASSERT_EQ(recv_results.size(), 2);
    EXPECT_EQ(recv_results[0].name, "Joaco");
    EXPECT_FLOAT_EQ(recv_results[0].time, 100.5f);
    EXPECT_EQ(recv_results[0].position, 1);
    EXPECT_EQ(recv_results[1].name, "Pepe");
    EXPECT_FLOAT_EQ(recv_results[1].time, 105.3f);
    EXPECT_EQ(recv_results[1].position, 2);
}

TEST(ProtocolIntegrationTest, ServerSendsClientReceivesGameState) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    
    Snapshot snapshot;
    snapshot.id = 42;
    snapshot.game_id = 7;
    snapshot.position = 3;
    snapshot.map = static_cast<Map>(1);
    snapshot.upgrade = static_cast<Upgrades>(0);
    snapshot.upgradeable = true;
    snapshot.collision = static_cast<CollisionType>(0);
    snapshot.checkpoint = {10.0f, 20.0f};
    snapshot.hint = {5.0f, 15.0f, 90.0f};
    snapshot.type_checkpoint = static_cast<TypeCheckpoint>(1);
    snapshot.time_ms = 12345;
    snapshot.state = static_cast<State>(1);
    snapshot.is_owner = true;
    
    server_proto.send_game_state(snapshot);
    
    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    
    ClientProtocol client_proto(client_socket);
    Snapshot recv_snapshot = client_proto.receive_game_state();
    
    EXPECT_EQ(recv_snapshot.id, 42);
    EXPECT_EQ(recv_snapshot.game_id, 7);
    EXPECT_EQ(recv_snapshot.position, 3);
    EXPECT_EQ(static_cast<int>(recv_snapshot.map), 1);
    EXPECT_TRUE(recv_snapshot.upgradeable);
    EXPECT_EQ(recv_snapshot.time_ms, 12345);
    EXPECT_TRUE(recv_snapshot.is_owner);
}

TEST(ProtocolIntegrationTest, CompleteFlowClientJoinsGame) {
    MockedSocket client_socket;
    MockedSocket server_socket;
    
    ClientProtocol client_proto(client_socket);

    client_proto.send_byte(SEND_AVAILABLE_GAMES);
    
    auto req = client_socket.get_written_bytes();
    server_socket.inject_bytes(req);
    
    ServerProtocol server_proto(server_socket);
    uint8_t cmd = server_proto.receive_standar_command();
    EXPECT_EQ(cmd, SEND_AVAILABLE_GAMES);

    std::vector<std::string> games = {"1", "2"};
    server_proto.send_games_list(games);
    
    auto resp = server_socket.get_written_bytes();
    client_socket.inject_bytes(resp);
    
    auto recv_games = client_proto.receive_games_list();
    ASSERT_EQ(recv_games.size(), 2);
    EXPECT_EQ(recv_games[0], "1");
}

TEST(ProtocolIntegrationTest, ServerRejectsWithError) {
    MockedSocket server_socket;
    MockedSocket client_socket;
    
    ServerProtocol server_proto(server_socket);
    
    std::string error = "Invalid game ID";
    server_proto.send_error_message(error);
    
    auto data = server_socket.get_written_bytes();
    client_socket.inject_bytes(data);
    
    ClientProtocol client_proto(client_socket);
    
    uint8_t signal = client_proto.receive_byte();
    EXPECT_EQ(signal, SEND_ERROR_MESSAGE);
    
    std::string recv_error = client_proto.receive_error_message();
    EXPECT_EQ(recv_error, error);
}