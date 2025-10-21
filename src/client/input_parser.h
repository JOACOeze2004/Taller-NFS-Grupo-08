#ifndef TALLER_TP_INPUT_PARSER_H
#define TALLER_TP_INPUT_PARSER_H

#include "client_sender.h"
#include <string>

class InputParser {
    ClientSender& sender;

public:
    explicit InputParser(ClientSender& _sender);
    void parse(std::string& input);

};

#endif  // TALLER_TP_INPUT_PARSER_H