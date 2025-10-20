target_sources(taller_server
    PRIVATE
    # .cpp files
    main.cpp
        server.cpp
        server_protocol.cpp
        client_acceptor.cpp
        client_handler.cpp
        client_receiver.cpp
        client_sender.cpp
    PUBLIC
        server.h
        server_protocol.h
        client_acceptor.h
        client_handler.h
        client_receiver.h
        client_sender.h
    # .h files
    )
