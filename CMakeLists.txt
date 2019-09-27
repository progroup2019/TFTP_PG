cmake_minimum_required(VERSION 3.10)
project(TFTP_PG)

set(CMAKE_CXX_STANDARD 14)

SET(BUILD_SHARED_LIBS OFF)
SET(CMAKE_EXE_LINKER_FLAGS "-static")

add_library(libs STATIC libs/checksum.h libs/checksum.cpp libs/header.cpp libs/header.h libs/helpers.cpp libs/helpers.h libs/server.cpp libs/server.h libs/client.cpp libs/client.h)

add_executable(TFTP_PG_SERVER server_main.cpp)

add_executable(TFTP_PG_CLIENT client_main.cpp)


target_link_libraries(TFTP_PG_SERVER libs)

target_link_libraries(TFTP_PG_CLIENT libs)

install(TARGETS TFTP_PG_SERVER
        COMPONENT linapp
        RUNTIME DESTINATION "$ENV{HOME}/TFTP_PG/server/"
        LIBRARY DESTINATION "$ENV{HOME}/TFTP_PG/server/"
        DESTINATION "$ENV{HOME}/TFTP_PG/server/"
        )

install(TARGETS TFTP_PG_CLIENT
        COMPONENT linapp
        RUNTIME DESTINATION "$ENV{HOME}/TFTP_PG/client/"
        LIBRARY DESTINATION "$ENV{HOME}/TFTP_PG/client/"
        DESTINATION "$ENV{HOME}/TFTP_PG/client/"
        )

SET(MAJOR_VERSION 1)
SET(MINOR_VERSION 0)
SET(PATCH_VERSION 0)
SET(CPACK_GENERATOR "DEB")
SET(CPACK_PACKAGE_DESCRIPTION "TFTP Protocol project")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Project TFTP Protocol builded by https://github.com/progroup2019")
SET(CPACK_PACKAGE_CONTACT "https://github.com/progroup2019")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${CPACK_PACKAGE_VERSION_PATCH}-${CPACK_DEBIAN_ARCHITECTURE}")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "KK") #required
INCLUDE(CPack)