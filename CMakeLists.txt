cmake_minimum_required(VERSION 3.16)

if (ESP_PLATFORM)

# As IDF component

idf_component_register(INCLUDE_DIRS src)

else()

# Process as a normal CMakke library
project(aacmp4)

add_library(aacmp4 INTERFACE)
target_include_directories(aacmp4 INTERFACE src)

endif()
