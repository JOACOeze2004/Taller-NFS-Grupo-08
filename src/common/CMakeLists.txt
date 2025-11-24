target_sources(taller_common
    PRIVATE
    # .cpp files
        protocol.cpp
        socket.cpp
        resolver.cpp
        resolvererror.cpp
        liberror.cpp
        mocked_socket.cpp
    PUBLIC
    # .h files
        constants.h
        protocol.h
        socket.h
        thread.h
        queue.h
        resolver.h
        resolvererror.h
        liberror.h
        mocked_socket.h
    )
