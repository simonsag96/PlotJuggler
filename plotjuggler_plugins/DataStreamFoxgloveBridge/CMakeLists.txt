cmake_minimum_required(VERSION 3.16)
project(DataStreamFoxgloveBridge)

find_package(Qt5 REQUIRED COMPONENTS Widgets WebSockets Xml)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(../)

qt5_wrap_ui(UI_SRC
  foxglove_client.ui
)

set(SRC
  foxglove_client.cpp
  foxglove_client_config.cpp
  foxglove_dialog.cpp
)

add_library(DataStreamFoxgloveBridge SHARED
  ${SRC}
  ${UI_SRC}
)

target_include_directories(DataStreamFoxgloveBridge PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
)

target_link_libraries(DataStreamFoxgloveBridge PRIVATE
  Qt5::Widgets
  Qt5::WebSockets
  Qt5::Xml
  plotjuggler_base
)

target_compile_definitions(DataStreamFoxgloveBridge PRIVATE
  QT_PLUGIN
  PJ_BUILD
)

install(TARGETS DataStreamFoxgloveBridge
        DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY})
