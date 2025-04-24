cmake_minimum_required(VERSION 3.7)
project(
    libgapcom
    VERSION 0.1.0
    DESCRIPTION "GISTRE ARM Project communication library & CLI"
    LANGUAGES C
)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_FLAGS ${CMAKE_C_FLAGS} "-Wall -Wextra -Wswitch-default -Wswitch-enum -Og -g3")

### Build libgapcom

add_subdirectory(nanopb)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/nanopb/extra)

add_subdirectory(gap-tinyframe)

add_subdirectory(gap-com)

### Build gapcli

add_subdirectory(gap-agents)

### Generate source ZIP archive including protobuf generated C code.
### This is useful to export libgapcom source code for integration
### into another build system, like STM32CubeIDE.

add_custom_target(
    full-src
    COMMAND /bin/bash ${CMAKE_CURRENT_SOURCE_DIR}/host-tools/generate_libgapcom_full_src_archive ${CMAKE_PROJECT_VERSION} ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR}
    VERBATIM
)
add_dependencies(full-src gapcom)

add_custom_target(
    format
    COMMAND clang-format -i ${CMAKE_CURRENT_SOURCE_DIR}/gap-com/**/*.c ${CMAKE_CURRENT_SOURCE_DIR}/gap-com/**/*.h ${CMAKE_CURRENT_SOURCE_DIR}/gap-agents/**/*.c ${CMAKE_CURRENT_SOURCE_DIR}/gap-agents/**/*.h
)
