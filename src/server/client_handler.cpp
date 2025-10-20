#include "client_handler.h"

ClientHandler::ClientHandler(Socket&& peer, Queue<std::string>& commands, Queue<std::string>& messages):
        peer(std::move(peer)),
        protocol(this->peer),
        command_queue(commands),
        client_queue(messages),
        receiver(protocol, command_queue),
        sender(protocol, command_queue) {}  //Cambiar a clientQueue cuando tengamos el broadcast.

void ClientHandler::run() {
    std::cout << "[HANDLER] receiver and sender threads have started" << std::endl;
    receiver.start();
    sender.start();
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

Queue<std::string>* ClientHandler::get_client_queue() const { return &client_queue; }

ClientHandler::~ClientHandler() {}
