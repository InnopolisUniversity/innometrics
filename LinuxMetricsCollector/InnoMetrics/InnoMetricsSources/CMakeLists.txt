cmake_minimum_required(VERSION 3.6)
project(InnoMetrics)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "/home/alon/innometrics_build/InnoMetrics/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread -lcurl -I/usr/include/libappindicator-0.1/ -lappindicator")

### GTK
# Use the package PkgConfig to detect GTK+ headers/library files
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

# Setup CMake to use GTK+, tell the compiler where to look for headers
# and to the linker where to look for libraries
include_directories(${GTK3_INCLUDE_DIRS})
link_directories(${GTK3_LIBRARY_DIRS})

# Link the target to the  libararies
configure_file(Sources/inno.cfg ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/inno.cfg COPYONLY)

# Add other flags to the compiler
add_definitions(${GTK3_CFLAGS_OTHER})

### X11
include(CMake/ImportX11.cmake)
import_x11()

### SQLite
include_directories(sqlite3)
set(sqlite3_srcs
        sqlite3/sqlite3.c
        sqlite3/sqlite3.h
        )
add_library(sqlite3lib SHARED ${sqlite3_srcs})

### Collecting data program
set(METRIC_SOURCES Sources/measurements_main.h Sources/Helpers/XInputListener.cpp Sources/Helpers/XInputListener.h Sources/BrowserFile.cpp Sources/BrowserFile.h Sources/Helpers/Helper.cpp Sources/Helpers/Helper.h Sources/Helpers/SystemInfoHelper.cpp Sources/Helpers/SystemInfoHelper.h Sources/DbQueries.h Sources/ApplicationType.h Sources/Helpers/WindowHelper.h Sources/Helpers/WindowHelper.cpp Sources/WindowInfo.h Sources/WindowInfo.cpp Sources/send_demon_main.h Sources/updater_demon_main.h Sources/Helpers/ConfigParser.h Sources/Authorization.h Sources/Helpers/HelpFunctions.h Sources/SettingDialog.h Sources/AddTimeFilterDialog.h Sources/MainWindow.h)

add_library(innometrics_core ${METRIC_SOURCES})

add_executable(InnoMetrics main.cpp)

# Link the target to the core file libararies
target_link_libraries(InnoMetrics innometrics_core)

# Link the target to the GTK+ libraries
target_link_libraries(InnoMetrics ${GTK3_LIBRARIES})

# Link the target to the SQLlite libararies
target_link_libraries(InnoMetrics sqlite3lib)

# Link the target to the CURL libararies
target_link_libraries(InnoMetrics curl)

###############################################################################
## packaging ##################################################################
###############################################################################

install(TARGETS InnoMetrics DESTINATION bin)
#install(DIRECTORY resources DESTINATION example_destination)

set(CPACK_PACKAGE_NAME "InnoMetrics Linux")
set(CPACK_PACKAGE_VERSION "1.0.0.0")

set(CPACK_MONOLITHIC_INSTALL 1)

include(CPack)
