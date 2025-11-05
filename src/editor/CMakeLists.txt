target_sources(taller_editor PRIVATE
        main.cpp
        city_config.cpp
        city_config.h
        city_selec.cpp
        city_selec.h
        editor_window.cpp
        editor_window.h
        editor_toolbar.cpp
        editor_toolbar.h
        editor_input_handler.cpp
        editor_input_handler.h
        track_file_manager.cpp
        track_file_manager.h
        map_renderer.cpp
        map_renderer.h
        checkpoint_manager.cpp
        checkpoint_manager.h
        hint_manager.cpp
        hint_manager.h
        track_serializer.cpp
        track_serializer.h
        yaml_parser.cpp
        yaml_parser.h
        race_track_data.h
)

target_link_libraries(taller_editor Qt5::Widgets)

install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/maps/
        DESTINATION /var/taller-nfs/maps
        FILES_MATCHING PATTERN "*.png")

install(DIRECTORY ${CMAKE_SOURCE_DIR}/tracks/
        DESTINATION /var/taller-nfs/tracks
        FILES_MATCHING PATTERN "*.yaml" PATTERN "*.yml")