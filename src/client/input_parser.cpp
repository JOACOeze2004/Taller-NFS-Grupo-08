#include "input_parser.h"
#include "../common/constants.h"

InputParser::InputParser(ClientSender& _sender, Queue<Command>& _command_queue)
    : sender(_sender), command_queue(_command_queue) {}


void InputParser::parse_command(int command) {
    Command cmd;
    cmd.cmd = static_cast<uint8_t>(command);

    command_queue.push(cmd);
}