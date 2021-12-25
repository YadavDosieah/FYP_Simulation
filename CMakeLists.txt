cmake_minimum_required(VERSION 3.16)

project(shepherding)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(enki_DIR third_party/enki)

execute_process (
    COMMAND bash -c "cmake . && make enki enkiviewer"
    WORKING_DIRECTORY ${enki_DIR}
)

find_package(enki REQUIRED)
find_package (Eigen3 3.3 REQUIRED NO_MODULE)
include(FindOpenGL)

include_directories(${enki_INCLUDE_DIR})
include_directories(/usr/local/include/libcmaes)
include_directories(/usr/local/lib)
include_directories(Main)

set(project_SRCS Main/source/Main.cpp)

add_executable(shepherding ${project_SRCS})
target_link_libraries(shepherding ${enki_VIEWER_LIBRARY} ${enki_LIBRARY} Eigen3::Eigen -lcmaes -lconfig++ Qt5::OpenGL Qt5::Widgets ${OPENGL_LIBRARIES})


set(CMAKE_BINARY_DIR .)
set_property(DIRECTORY APPEND PROPERTY ADDITIONAL_MAKE_CLEAN_FILES
  "${CMAKE_BINARY_DIR}/CMakeCache.txt"
  "${CMAKE_BINARY_DIR}/cmake_install.cmake"
  "${CMAKE_BINARY_DIR}/Makefile"
  "${CMAKE_BINARY_DIR}/CMakeFiles"
  "${CMAKE_BINARY_DIR}/CMakeDoxyfile.in"
  "${CMAKE_BINARY_DIR}/CMakeDoxygenDefaults.cmake"
)
