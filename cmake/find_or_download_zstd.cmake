function(find_or_download_zstd)

  if(NOT TARGET zstd::libzstd_static)

    message(STATUS "Downloading and compiling ZSTD")

    # zstd ###
    cpmaddpackage(
      NAME zstd
      URL https://github.com/facebook/zstd/archive/refs/tags/v1.5.7.zip
      DOWNLOAD_ONLY YES)

    set(ZSTD_BUILD_STATIC ON CACHE BOOL " " FORCE)
    set(ZSTD_BUILD_SHARED OFF CACHE BOOL " " FORCE)
    set(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_BUILD_PROGRAMS OFF CACHE BOOL " " FORCE)
    set(ZSTD_BUILD_TESTS OFF CACHE BOOL " " FORCE)
    set(ZSTD_BUILD_CONTRIB OFF CACHE BOOL " " FORCE)
    set(ZSTD_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
    set(ZSTD_MULTITHREAD_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_LEGACY_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_ZLIB_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_LZ4_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_LZMA_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_ZDICT_SUPPORT OFF CACHE BOOL " " FORCE)
    set(ZSTD_PROGRAMS "" CACHE STRING " " FORCE)

    add_subdirectory(${zstd_SOURCE_DIR}/build/cmake ${zstd_BINARY_DIR})
    set(ZSTD_FOUND TRUE)

    add_library(zstd::libzstd_static INTERFACE IMPORTED)
    set_target_properties(zstd::libzstd_static PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${zstd_SOURCE_DIR}/lib
        INTERFACE_LINK_LIBRARIES libzstd_static)

  endif()

endfunction()
