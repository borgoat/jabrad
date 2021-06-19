cmake_minimum_required(VERSION 3.17)
project(jabrad C)

set(CMAKE_C_STANDARD 11)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GIO REQUIRED gio-unix-2.0)  # just reference the entire GTK if this causes issues

include_directories(${GIO_INCLUDE_DIRS})
link_directories(${GIO_LIBRARY_DIRS})
add_definitions(${GIO_CFLAGS_OTHER})

add_executable(jabractl main.c dbus-gen.c handler-dbus.c handler-jabra.c)

target_link_libraries(jabractl ${GIO_LIBRARIES} jabra)