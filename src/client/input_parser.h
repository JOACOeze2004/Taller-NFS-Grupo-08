#ifndef TALLER_TP_INPUT_PARSER_H
#define TALLER_TP_INPUT_PARSER_H

#include "client_sender.h"
#include "../common/queue.h"
#include "../common/command.h"
#include <string>

class InputParser {
    ClientSender& sender; 
    Queue<Command>& command_queue;

public:
    explicit InputParser(ClientSender& _sender, Queue<Command>& _command_queue);
    void parse_command(int command); 
};

#endif  // TALLER_TP_INPUT_PARSER_H