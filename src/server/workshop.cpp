#include "workshop.h"
#include <iostream>

Workshop::Workshop(Gameloop* _gameloop, float _duration)
    : Phase(_gameloop, _duration) {
}

void Workshop::run() { }

void Workshop::execute() {
    //std::cout << "[Workshop] Ejecutando comando" << command.id << std::endl;
}


