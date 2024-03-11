if(BUILDING_WITH_VCPKG)
    message(STATUS "Finding Zcm with vcpkg")
    set(Zcm_LIBRARIES libzcm)
elseif(BUILDING_WITH_CONAN)
    message(STATUS "Finding Zcm with conan")
    set(Zcm_LIBRARIES libzcm)
else()
    message(STATUS "Finding Zcm without package managers")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")
endif()

find_package(Zcm QUIET)

if(Zcm_FOUND)

    message(STATUS "[Zcm] found")

    include_directories( ${Zcm_INCLUDE_DIR})

    add_definitions(${QT_DEFINITIONS})
    add_definitions(-DQT_PLUGIN)

    # Streaming plugin
    QT5_WRAP_UI(STREAM_UI datastream_zcm.ui config_zcm.ui )
    add_library(DataStreamZcm SHARED
        datastream_zcm.h
        datastream_zcm.cpp
        config_zcm.h
        config_zcm.cpp
        ${STREAM_UI})

    # log loading plugin
    target_link_libraries(DataStreamZcm
        ${Qt5Widgets_LIBRARIES}
        plotjuggler_base
        ${Zcm_LIBRARIES} )

    QT5_WRAP_UI(LOAD_UI dataload_zcm.ui config_zcm.ui)
    add_library(DataLoadZcm SHARED
        dataload_zcm.h
        dataload_zcm.cpp
        config_zcm.h
        config_zcm.cpp
        ${LOAD_UI})

    target_link_libraries(DataLoadZcm
        ${Qt5Widgets_LIBRARIES}
        plotjuggler_base
        ${Zcm_LIBRARIES} )

    install(TARGETS DataStreamZcm DataLoadZcm
        DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY}  )
else()
    message("[Zcm] not found. Skipping plugin DataStreamZcma and DataLoadZcm.")
endif()
