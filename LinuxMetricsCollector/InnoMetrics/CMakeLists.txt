cmake_minimum_required(VERSION 3.6)
project(InnoMetrics)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -lcurl")

### GTK
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})


### X11
include(CMake/ImportX11.cmake)
import_x11()

### SQLite
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/)
set(sqlite3_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/sqlite3.c
        ${CMAKE_CURRENT_SOURCE_DIR}/sqlite3/sqlite3.h
        )
add_library(sqlite3lib SHARED ${sqlite3_srcs})

### Main program
set(MAIN_SOURCES main.cpp)

### Collecting data program
set(METRIC_SOURCES Sources/measurements_main.h Sources/Helpers/XInputListener.cpp Sources/Helpers/XInputListener.h Sources/BrowserFile.cpp Sources/BrowserFile.h Sources/Helpers/Helper.cpp Sources/Helpers/Helper.h Sources/Helpers/SystemInfoHelper.cpp Sources/Helpers/SystemInfoHelper.h Sources/DbQueries.h Sources/ApplicationType.h Sources/Helpers/WindowHelper.h Sources/Helpers/WindowHelper.cpp Sources/WindowInfo.h Sources/WindowInfo.cpp send_demon_main.h)

add_executable(InnoMetrics ${MAIN_SOURCES} ${METRIC_SOURCES})

# Link the target to the GTK+ libraries
target_link_libraries(InnoMetrics ${GTK3_LIBRARIES})

# Link the target to the SQLlite libararies
target_link_libraries(InnoMetrics sqlite3lib)

# Link the target to the CURL libararies
target_link_libraries(InnoMetrics curl)
