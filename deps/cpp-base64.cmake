cmake_minimum_required(VERSION 3.10)
include(FetchContent)
FetchContent_Declare(cpp-base64 GIT_REPOSITORY https://github.com/ReneNyffenegger/cpp-base64.git)
FetchContent_GetProperties(cpp-base64)
if(NOT cpp-base64_POPULATED)
    FetchContent_Populate(cpp-base64)
    file(GLOB CPP_BASE64_SOURCE "${cpp-base64_SOURCE_DIR}/base64.*")
    add_library(cpp-base64 STATIC ${CPP_BASE64_SOURCE})
    set_target_properties(cpp-base64 PROPERTIES
        CXX_STANDARD 17
        LIBRARY_OUTPUT_DIRECTORY "${cpp-base64_BINARY_DIR}"
        ARCHIVE_OUTPUT_DIRECTORY "${cpp-base64_BINARY_DIR}")
    target_include_directories(cpp-base64 PUBLIC "${cpp-base64_SOURCE_DIR}")
endif()