target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
        server.cpp
        server_protocol.cpp
        acceptor.cpp
        client_handler.cpp
        receiver.cpp
        sender.cpp
        config.cpp
    PUBLIC
        server.h
        server_protocol.h
        acceptor.h
        client_handler.h
        receiver.h
        sender.h
        config.h
    # .h files
    )
