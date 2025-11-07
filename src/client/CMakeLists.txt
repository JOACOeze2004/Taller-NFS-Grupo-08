target_sources(taller_client
    PRIVATE
    # .cpp files
    client.cpp
    client_protocol.cpp
    main.cpp
    text_renderer.cpp
    PUBLIC
    # .h files
    client.h
    client_protocol.h
    text_renderer.h
    )
