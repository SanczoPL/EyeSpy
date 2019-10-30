cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)
project(OpenCVexample
  VERSION 0.0.1
  DESCRIPTION "OpenCVexample"
  LANGUAGES C CXX)

set(DEFAULT_BUILD_TYPE "Release")
set(TARGET_NAME "OpenCVexample")
#STNDARD STUFF
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON} -std=c++11 -pthread ")


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)



# include QT
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS})


if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SRC
  src/main.cpp
  ../src/tools.cpp
  src/mainloop.cpp
  ../src/ImageProcessing/acquisition.cpp
  ../src/ImageProcessing/adaptivethreshold.cpp
  ../src/ImageProcessing/boundariesfinder.cpp
  ../src/broadcaster.cpp
)

set(HEADERS
  ../src/tools.h
  src/mainloop.h
  ../src/spdlog/spdlog.h
  ../src/spdlog/sinks/sink.h
  ../src/spdlog/sinks/stdout_sinks.h
  ../src/ImageProcessing/acquisition.h
  ../src/ImageProcessing/adaptivethreshold.h
  ../src/ImageProcessing/boundariesfinder.h
  ../src/broadcaster.h
)

add_executable(${TARGET_NAME}
    ${SRC}
    ${HEADERS}
)


#qt5_use_modules(${PROJECT_NAME} Core)

target_link_libraries(${PROJECT_NAME}  Qt5::Core  ${OpenCV_LIBS}  Qt5::Network)
#${OpenCV_LIBS}   ${OPENGL_LIBRARIES}  Threads::Threads   ${QT_LIBRARIES} Qt5::Core Threads::Threads
