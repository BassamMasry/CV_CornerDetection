cmake_minimum_required(VERSION 3.0.0)
project(CVProject VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 17)

include(CTest)
enable_testing()

find_package(OpenCV REQUIRED)
FIND_PACKAGE (PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTKMM gtkmm-3.0)

include_directories(include)
include_directories(res)
include_directories(${OpenCV_INCLUDE_DIRS} ${GTKMM_INCLUDE_DIRS})

set(SOURCES main.cpp
    src/GUI/GUI_init.cpp
    src/GUI/GUI_signals.cpp
    src/matching/SSD.cpp
    src/matching/NCC.cpp
    src/greyscale.cpp
    src/Tree.cpp
    src/SIFT.cpp
    src/harris.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

link_directories(${GTKMM_LIBRARY_DIRS})

target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS} ${GTKMM_LIBRARIES})
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
