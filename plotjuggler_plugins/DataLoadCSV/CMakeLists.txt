include_directories(../)

qt5_wrap_ui(UI_SRC dataload_csv.ui datetimehelp.ui)

set(SRC dataload_csv.cpp datetimehelp.cpp)

add_library(DataLoadCSV SHARED ${SRC} ${UI_SRC})
target_link_libraries(DataLoadCSV PRIVATE Qt5::Widgets Qt5::Xml
                                          plotjuggler_base QCodeEditor)

target_compile_definitions(DataLoadCSV PRIVATE QT_PLUGIN)

install(TARGETS DataLoadCSV DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY})
