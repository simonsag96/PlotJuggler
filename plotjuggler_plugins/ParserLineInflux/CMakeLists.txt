
add_definitions(${QT_DEFINITIONS})
add_definitions(-DQT_PLUGIN)


add_library(ParserLineInflux SHARED
    line_parser.cpp
    line_parser.h )

target_link_libraries(ParserLineInflux
    ${Qt5Widgets_LIBRARIES}
    plotjuggler_base)


install(TARGETS ParserLineInflux
    DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
