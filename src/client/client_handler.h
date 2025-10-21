#ifndef TALLER_TP_CLIENT_HANDLER_H
#define TALLER_TP_CLIENT_HANDLER_H

#include "input_parser.h"

class ClientHandler {
    InputParser& parser;

public:
    explicit ClientHandler(InputParser& _parser);
    void handle_event();
};


#endif  // TALLER_TP_CLIENT_HANDLER_H
