cmake_minimum_required(VERSION 3.16)
project(aacmp4)

find_package(PkgConfig)

pkg_check_modules(LIBFDKAAC REQUIRED fdk-aac)

include_directories(${LIBFDKAAC_INCLUDE_DIRS})

add_executable(aacmp4test
    src/aacmp4test.cpp
)

target_link_libraries(aacmp4test
    ${LIBFDKAAC_LIBRARIES}
)