cmake_minimum_required(VERSION 3.7)
project(gap-com C)
set(CMAKE_CXX_STANDARD GNU17)
find_package(Nanopb REQUIRED)

# Options
set(
    GAPCOM_LOGGING_BACKEND
    "console"
    CACHE
    STRING
    "Log messages on the given backend. Can be amongst: none (no logs), console"
)
# Options sanity checks
if (NOT (${GAPCOM_LOGGING_BACKEND} STREQUAL "none") AND NOT (${GAPCOM_LOGGING_BACKEND} STREQUAL "console"))
message(FATAL_ERROR "GAPCOM_LOGGING_BACKEND must be amongst: none (no logs), console")
endif()

set(GAPCOM_SRC_PATH ${PROJECT_SOURCE_DIR}/src/)
set(GAPCOM_INC_PATH ${PROJECT_SOURCE_DIR}/include/)
set(GAPCOM_PROTO_PATH ${PROJECT_SOURCE_DIR}/proto/)

# In addition to our source files, we must generate source files from our
# Protocol Buffer specification. This is done by invoking the nanopb code
# generator during build.
nanopb_generate_cpp(
    GAPCOM_PROTO_SRC
    GAPCOM_PROTO_HDR
    ${GAPCOM_PROTO_PATH}/gap.proto
)
set(
    GAPCOM_SRC
    ${GAPCOM_SRC_PATH}/gapcom.c
    ${GAPCOM_SRC_PATH}/gapcom_core.c
    ${GAPCOM_SRC_PATH}/gapcom_client.c
    ${GAPCOM_SRC_PATH}/gapcom_server.c
    ${GAPCOM_PROTO_SRC}
)
include_directories(
    ${GAPCOM_INC_PATH}
    ${CMAKE_CURRENT_BINARY_DIR} # Now contains ${GAPCOM_PROTO_HDR}
)

add_library(gapcom STATIC ${GAPCOM_SRC})
if(${GAPCOM_LOGGING_BACKEND} STREQUAL "none")
target_compile_definitions(gapcom PRIVATE GAPCOM_LOGGING_BACKEND_NONE)
else() # ${GAPCOM_LOGGING_BACKEND} STREQUAL "console"
target_compile_definitions(gapcom PRIVATE GAPCOM_LOGGING_BACKEND_CONSOLE)
if(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Linux")
add_compile_definitions(GAP_COM_EOL="\\n")
elseif(${CMAKE_HOST_SYSTEM_NAME} STREQUAL "Windows")
add_compile_definitions(GAP_COM_EOL="\\r\\n")
endif()
endif()
target_link_libraries(gapcom PUBLIC gaptinyframe)
target_include_directories(
    gapcom
    PUBLIC
    ${PROJECT_SOURCE_DIR}/../TinyFrame # gaptinyframe TinyFrame.h
    ${PROJECT_SOURCE_DIR}/../gap-tinyframe/include # gaptinyframe TF_Config.h
    ${NANOPB_INCLUDE_DIRS} # Exported by Nanopb package
)
