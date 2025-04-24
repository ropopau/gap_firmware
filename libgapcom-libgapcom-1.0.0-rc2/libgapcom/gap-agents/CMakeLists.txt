cmake_minimum_required(VERSION 3.7)
project(gap-agents C)

# Options
set(
    GAP_AGENTS_BUILD_GAPCLI
    "TRUE"
    CACHE
    STRING
    "Build gapcli client agent"
)
set(
    GAP_AGENTS_COM_BACKEND
    "UART"
    CACHE
    STRING
    "Communication back-end used for sending/receiving messages"
)
# Options sanity checks
if(NOT ((${GAP_AGENTS_COM_BACKEND} STREQUAL "UART")))
message(FATAL_ERROR "GAP_AGENTS_COM_BACKEND must be UART (TCP backend unsupported for now)")
endif()

set(GAP_AGENTS_SRC_PATH ${PROJECT_SOURCE_DIR}/src)

######################
### Helper library ###
######################

set(
    GAPCOM_AGENTS_COMMON_SRC
    ${GAP_AGENTS_SRC_PATH}/options.c
)
add_library(gap-agents-common STATIC ${GAPCOM_AGENTS_COMMON_SRC})

######################
### Client program ###
######################

set(GAP_AGENTS_GAPCLI_SRC ${GAP_AGENTS_SRC_PATH}/gapcli.c)
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
add_compile_definitions(GAP_AGENTS_EOL="\\n")
set(
    GAP_AGENTS_GAPCLI_SRC
    ${GAP_AGENTS_GAPCLI_SRC}
    ${GAP_AGENTS_SRC_PATH}/communicator_impl_uart_client_linux.c
)
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
add_compile_definitions(GAP_AGENTS_EOL="\\r\\n")
set(
    GAP_AGENTS_GAPCLI_SRC
    ${GAP_AGENTS_GAPCLI_SRC}
    ${GAP_AGENTS_SRC_PATH}/communicator_impl_uart_client_win32.c
)
endif()

add_executable(gapcli ${GAP_AGENTS_GAPCLI_SRC})
target_link_libraries(gapcli PUBLIC "-Wl,-whole-archive" gapcom "-Wl,-no-whole-archive")
target_link_libraries(gapcli PRIVATE gap-agents-common)
target_include_directories(
    gapcli
    PUBLIC
    ${PROJECT_SOURCE_DIR}/../gap-com/include/ # gapcom.h
    ${CMAKE_BINARY_DIR}/gap-com/ # Contains generated Protocol Buffer C header
)
