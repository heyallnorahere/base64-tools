cmake_minimum_required(VERSION 3.10)
project(base64-tools)
option(BASE64_TOOLS_INSTALL "Installs b64-decode and b64-encode to a directory on the PATH." OFF)
add_subdirectory("deps")
add_subdirectory("decoder")
add_subdirectory("encoder")
if(BASE64_TOOLS_INSTALL)
    install(TARGETS decoder CONFIGURATIONS Release)
    install(TARGETS encoder CONFIGURATIONS Release)
endif()