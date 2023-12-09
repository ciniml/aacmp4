cmake_minimum_required(VERSION 3.16)
project(examples)

find_package(PkgConfig)

pkg_check_modules(LIBFDKAAC REQUIRED fdk-aac)
include_directories(${LIBFDKAAC_INCLUDE_DIRS})

include_directories(../src)

add_executable(aacmp4test
    ./aacmp4test.cpp
)

target_link_libraries(aacmp4test
    ${LIBFDKAAC_LIBRARIES}
)
