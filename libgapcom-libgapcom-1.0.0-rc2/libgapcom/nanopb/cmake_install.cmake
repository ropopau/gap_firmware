# Install script for directory: /home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3.12/site-packages/proto" TYPE FILE FILES
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/nanopb_pb2.py"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/generator/proto/nanopb.proto"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/python3.12/site-packages/proto" TYPE FILE FILES "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/generator/proto/_utils.py")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/bin" TYPE PROGRAM FILES
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/generator/nanopb_generator.py"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/generator/protoc-gen-nanopb"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/libprotobuf-nanopb.a")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb/nanopb-targets.cmake")
    file(DIFFERENT _cmake_export_file_changed FILES
         "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb/nanopb-targets.cmake"
         "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/CMakeFiles/Export/856b60d80594378b4f0a78e696a1c3d7/nanopb-targets.cmake")
    if(_cmake_export_file_changed)
      file(GLOB _cmake_old_config_files "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb/nanopb-targets-*.cmake")
      if(_cmake_old_config_files)
        string(REPLACE ";" ", " _cmake_old_config_files_text "${_cmake_old_config_files}")
        message(STATUS "Old export file \"$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb/nanopb-targets.cmake\" will be replaced.  Removing files [${_cmake_old_config_files_text}].")
        unset(_cmake_old_config_files_text)
        file(REMOVE ${_cmake_old_config_files})
      endif()
      unset(_cmake_old_config_files)
    endif()
    unset(_cmake_export_file_changed)
  endif()
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb" TYPE FILE FILES "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/CMakeFiles/Export/856b60d80594378b4f0a78e696a1c3d7/nanopb-targets.cmake")
  if(CMAKE_INSTALL_CONFIG_NAME MATCHES "^()$")
    file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb" TYPE FILE FILES "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/CMakeFiles/Export/856b60d80594378b4f0a78e696a1c3d7/nanopb-targets-noconfig.cmake")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/cmake/nanopb" TYPE FILE FILES
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/extra/nanopb-config.cmake"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/nanopb-config-version.cmake"
    )
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include" TYPE FILE FILES
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/pb.h"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/pb_common.h"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/pb_encode.h"
    "/home/valentin/GISTRE/2025/ARM/armproject/libgapcom-libgapcom-1.0.0-rc2/libgapcom/nanopb/pb_decode.h"
    )
endif()

