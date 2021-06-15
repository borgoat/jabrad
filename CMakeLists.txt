cmake_minimum_required(VERSION 3.17)
project(jabractl C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})
add_definitions(${GTK3_CFLAGS_OTHER})

add_executable(jabractl main.c jabra-generated.c)

target_link_libraries(jabractl ${GTK3_LIBRARIES} jabra)