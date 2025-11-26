#include "mocked_socket.h"
#include <cstring>
#include <algorithm>

MockedSocket::MockedSocket() :
    write_buffer(),
    read_buffer(),
    read_pos(0),
    send_closed(false),
    recv_closed(false) {}

MockedSocket::~MockedSocket() { }

int MockedSocket::sendall(const void* data, unsigned size) {
    if (send_closed){
        return -1;
    } 
    const uint8_t* bytes = reinterpret_cast<const uint8_t*>(data);
    write_buffer.insert(write_buffer.end(), bytes, bytes + size);
    return size;
}

int MockedSocket::recvall(void* data, unsigned size) {
    if (recv_closed){
        return 0;
    } 
    size_t available = read_buffer.size() - read_pos;
    size_t to_read = std::min<size_t>(size, available);
    if (to_read == 0){
        return 0;
    } 
    std::memcpy(data, &read_buffer[read_pos], to_read);
    read_pos += to_read;
    return to_read;
}

void MockedSocket::shutdown(int how) {
    if (how == 0){
        recv_closed = true;
        return;
    } 
    if (how == 1){
        send_closed = true;
        return;
    }
    if (how == 2){
        send_closed = true;
        recv_closed = true;
        return;
    }
}

int MockedSocket::close() {
    send_closed = recv_closed = true;
    return 0;
}

bool MockedSocket::is_stream_send_closed() const { return send_closed; }
bool MockedSocket::is_stream_recv_closed() const { return recv_closed; }

void MockedSocket::inject_bytes(const std::vector<uint8_t>& bytes) { read_buffer.insert(read_buffer.end(), bytes.begin(), bytes.end()); }

std::vector<uint8_t> MockedSocket::get_written_bytes() { return write_buffer; }