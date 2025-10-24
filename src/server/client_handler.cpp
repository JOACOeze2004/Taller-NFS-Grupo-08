#include "client_handler.h"

ClientHandler::ClientHandler(Socket&& peer, Queue<ClientCommand>& commands, int _id):
        peer(std::move(peer)),
        protocol(this->peer),
        command_queue(commands),
        client_queue(CLIENT_QUEUE_LEN),
        id(_id),
        receiver(protocol, command_queue, this->id),
        sender(protocol, client_queue) {} 

void ClientHandler::run() {
    std::cout << "[HANDLER] receiver and sender threads have started" << std::endl;
    receiver.start();
    sender.start();
}

void ClientHandler::send_state(CarDTO state) {
    client_queue.push(state);
}

void ClientHandler::kill() {
    stop();

    client_queue.close();
    receiver.stop();
    sender.stop();
    kill_threads();
}

void ClientHandler::kill_threads() {
    receiver.join();
    sender.join();
}

bool ClientHandler::is_dead() const { return !sender.is_alive() && !receiver.is_alive(); }

ClientHandler::~ClientHandler() {}
