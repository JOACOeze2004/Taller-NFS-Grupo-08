#include "input_parser.h"
#include "../common/constants.h"

InputParser::InputParser(ClientSender& _sender, Queue<Command>& _command_queue)
    : sender(_sender), command_queue(_command_queue) {}

void InputParser::parse(std::string& input) {
    //no se sabe si se usa
    input = input; //para q no se queje el compilador
}

void InputParser::parse_command(int command) {
    // Crear el comando y pushearlo a la cola // ver como manejar esto seguro cambia el comando
    Command cmd;
    cmd.cmd = static_cast<uint8_t>(command);

    command_queue.push(cmd);
}