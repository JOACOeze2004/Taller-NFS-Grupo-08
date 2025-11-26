target_sources(taller_client
    PRIVATE
    # .cpp files
    audio_manager.cpp
    client.cpp
    client_protocol.cpp
    main.cpp
    text_renderer.cpp
    PUBLIC
    # .h files
    audio_manager.h
    client.h
    client_protocol.h
    text_renderer.h
    )
