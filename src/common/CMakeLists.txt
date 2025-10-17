target_sources(taller_common
    PRIVATE
    # .cpp files
    foo.cpp
        protocol.cpp
        socket.cpp
        resolver.cpp
        resolvererror.cpp
        liberror.cpp
    PUBLIC
    # .h files
    foo.h
        constants.h
        protocol.h
        socket.h
        thread.h
        queue.h
        resolver.h
        resolvererror.h
        liberror.h
    )
