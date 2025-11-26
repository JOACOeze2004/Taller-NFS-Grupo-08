#pragma once
#include <vector>
#include <cstdint>
#include <cstring>

class MockedSocket {
private:
    std::vector<uint8_t> write_buffer;
    std::vector<uint8_t> read_buffer;
    size_t read_pos = 0;
    bool send_closed;
    bool recv_closed;

public:
    MockedSocket();
    
    int sendall(const void* data, unsigned size);
    int recvall(void* data, unsigned size);

    void shutdown(int how);
    int close();
    bool is_stream_send_closed() const;
    bool is_stream_recv_closed() const;
    void inject_bytes(const std::vector<uint8_t>& bytes);
    std::vector<uint8_t> get_written_bytes();  
    ~MockedSocket();
};

