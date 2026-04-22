
function(download_wasmer)

  if(TARGET wasmer::wasmer)
    return()
  endif()

  set(WASMER_VERSION "v7.0.1")
  set(WASMER_BASE_URL "https://github.com/wasmerio/wasmer/releases/download/${WASMER_VERSION}")

  if(WIN32)
    if(MINGW)
      message(STATUS "Downloading wasmer for MinGW")
      set(WASMER_URL "${WASMER_BASE_URL}/wasmer-windows-gnu64.tar.gz")
      set(WASMER_URL_HASH "4b7be5b79a127edf45bb79639f2747021363efb78f3e9698c9b052a4fbc3a4be")
    else()
      message(STATUS "Downloading wasmer for MSVC")
      set(WASMER_URL "${WASMER_BASE_URL}/wasmer-windows-amd64.tar.gz")
      set(WASMER_URL_HASH "7d76d4ce7e3ed40366bbd2b76c9039b78843a905df34c89fa5d6ffb227d78c77")
    endif()
  elseif(UNIX)
    if(APPLE)
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        message(STATUS "Downloading wasmer for macOS x86_64")
        set(WASMER_URL "${WASMER_BASE_URL}/wasmer-darwin-amd64.tar.gz")
        set(WASMER_URL_HASH "3a0f44a3aae570b0870d4573fa663c7f0c96a2f9550e38eb22c3be7c77658a1e")
      elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        message(STATUS "Downloading wasmer for macOS arm64")
        set(WASMER_URL "${WASMER_BASE_URL}/wasmer-darwin-arm64.tar.gz")
        set(WASMER_URL_HASH "3eff017389fb838b0b5af607a4d392edc6039e76343984fcd24307aa027d67ee")
      endif()
    else()
      if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
        message(STATUS "Downloading wasmer for Linux x86_64")
        set(WASMER_URL "${WASMER_BASE_URL}/wasmer-linux-amd64.tar.gz")
        set(WASMER_URL_HASH "10a55885b11eb51b06bb24ff184facde8c2a83c252782a0c04e7a46926630d72")
      elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "^(aarch64|arm64)$")
        message(STATUS "Downloading wasmer for Linux aarch64")
        set(WASMER_URL "${WASMER_BASE_URL}/wasmer-linux-aarch64.tar.gz")
        set(WASMER_URL_HASH "21d6968d33defa4a31d878022d261667a8fa8abbfe96007d5f4f28564b7fa372")
      endif()
    endif()
  endif()

  if(NOT WASMER_URL)
    message(ERROR "Unsupported platform for wasmer: ${CMAKE_SYSTEM_PROCESSOR}")
    return()
  endif()

  set(WASMER_STATIC_LIBRARY_NAME ${CMAKE_STATIC_LIBRARY_PREFIX}wasmer${CMAKE_STATIC_LIBRARY_SUFFIX})
  message(STATUS "WASMER_URL: ${WASMER_URL}")

  cpmaddpackage(NAME wasmer
      URL ${WASMER_URL}
      URL_HASH SHA256=${WASMER_URL_HASH}
      DOWNLOAD_ONLY YES)

  if(WIN32 AND NOT MINGW)
    # MSVC: wasmer ships as DLL + import library; DLL lives in lib/, not bin/
    if(NOT EXISTS "${wasmer_SOURCE_DIR}/lib/wasmer.dll.lib")
      message(FATAL_ERROR "wasmer import library not found: ${wasmer_SOURCE_DIR}/lib/wasmer.dll.lib")
    endif()
    if(NOT EXISTS "${wasmer_SOURCE_DIR}/lib/wasmer.dll")
      message(FATAL_ERROR "wasmer DLL not found: ${wasmer_SOURCE_DIR}/lib/wasmer.dll")
    endif()
    add_library(wasmer::wasmer SHARED IMPORTED GLOBAL)
    set_target_properties(wasmer::wasmer PROPERTIES
        IMPORTED_IMPLIB   "${wasmer_SOURCE_DIR}/lib/wasmer.dll.lib"
        IMPORTED_LOCATION "${wasmer_SOURCE_DIR}/lib/wasmer.dll"
        INTERFACE_INCLUDE_DIRECTORIES "${wasmer_SOURCE_DIR}/include"
        INTERFACE_LINK_LIBRARIES "ws2_32;advapi32;userenv;ntdll;bcrypt")
    # Headers default to __declspec(dllimport) on Windows — do not override WASM_API_EXTERN
  else()
    # Linux, macOS, MinGW: static library
    if(NOT EXISTS "${wasmer_SOURCE_DIR}/lib/${WASMER_STATIC_LIBRARY_NAME}")
      message(FATAL_ERROR "wasmer library not found: ${wasmer_SOURCE_DIR}/lib/${WASMER_STATIC_LIBRARY_NAME}")
    endif()
    add_library(wasmer::wasmer UNKNOWN IMPORTED GLOBAL)
    set_target_properties(wasmer::wasmer PROPERTIES
        IMPORTED_LOCATION "${wasmer_SOURCE_DIR}/lib/${WASMER_STATIC_LIBRARY_NAME}"
        INTERFACE_INCLUDE_DIRECTORIES "${wasmer_SOURCE_DIR}/include")
    if(WIN32)
      # MinGW static lib — suppress dllimport annotations
      set_property(TARGET wasmer::wasmer APPEND PROPERTY
          INTERFACE_LINK_LIBRARIES "ws2_32;advapi32;userenv;ntdll;bcrypt")
      set_property(TARGET wasmer::wasmer APPEND PROPERTY
          INTERFACE_COMPILE_DEFINITIONS "WASM_API_EXTERN=;WASI_API_EXTERN=")
    else()
      set_property(TARGET wasmer::wasmer APPEND PROPERTY
          INTERFACE_LINK_LIBRARIES "pthread;dl;m")
    endif()
  endif()

  set(wasmer_FOUND TRUE CACHE BOOL "Whether wasmer was found or downloaded")

endfunction()
