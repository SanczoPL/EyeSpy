cmake_minimum_required(VERSION 3.7.2 FATAL_ERROR)
project(MotorApp
  VERSION 0.0.1
  DESCRIPTION "MotorApp"
  LANGUAGES C CXX)

set(DEFAULT_BUILD_TYPE "Release")
set(TARGET_NAME "MotorApp")

#STNDARD STUFF
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAGS_COMMON} -std=c++17 -pthread -lwiringPi -fPIC")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/../raspicam-0.1.8/src)
find_package(Qt5Core REQUIRED)
find_package(Qt5Network REQUIRED)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(OpenCV REQUIRED)
include_directories(
    ${OpenCV_INCLUDE_DIRS})
find_package(raspicam)
LINK_LIBRARIES( raspicam )



if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(SRC
  src/main.cpp
  src/Threads/motor.cpp
  src/tools.cpp
  src/mainloop.cpp
  src/Motor/DEV_Config.cpp
  src/Motor/MotorDriver.cpp
  src/Motor/PCA9685_Motor.cpp
  src/Threads/adc.cpp
  src/PID/MiniPID.cpp
  src/Threads/pid.cpp
  src/ImageProcessing/acquisition.cpp
  src/ImageProcessing/adaptivethreshold.cpp
  src/ImageProcessing/boundariesfinder.cpp

  RTIMULib2/RTIMULib/RTFusion.cpp
  RTIMULib2/RTIMULib/RTFusionRTQF.cpp
  RTIMULib2/RTIMULib/RTMath.cpp
  RTIMULib2/RTIMULib/RTFusionKalman4.cpp
  RTIMULib2/RTIMULib/RTIMUAccelCal.cpp
  RTIMULib2/RTIMULib/RTIMUHal.cpp
  RTIMULib2/RTIMULib/RTIMUMagCal.cpp
  RTIMULib2/RTIMULib/RTIMUSettings.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMU.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUGD20M303DLHC.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUGD20HM303DLHC.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUGD20HM303D.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMULSM9DS0.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMULSM9DS1.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUMPU9150.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUMPU9250.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUBMX055.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUBNO055.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTIMUNull.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTPressure.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTPressureBMP180.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTPressureLPS25H.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTPressureMS5611.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTPressureMS5637.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTHumidity.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTHumidityHTS221.cpp
  RTIMULib2/RTIMULib/IMUDrivers/RTHumidityHTU21D.cpp

  src/Threads/rtimuloop.cpp
  src/Threads/serialport.cpp
  src/Threads/broadcaster.cpp
  src/ImageProcessing/capture.cpp
  src/Threads/epd.cpp
  src/EPD/EPD_2in13.cpp
  src/EPD/DEV_EPD_Config.cpp
  src/EPD/GUI_Paint.cpp
  src/EPD/Fonts/fonts.cpp
  src/Servo/DEV_Servo_Config.cpp
  src/Servo/PCA9685_Servo.cpp
)

set(HEADERS
  src/Threads/motor.h
  src/tools.h
  src/mainloop.h
  src/Motor/DEV_Config.h
  src/Motor/MotorDriver.h
  src/Motor/PCA9685_Motor.h
  src/spdlog/spdlog.h
  src/spdlog/sinks/sink.h
  src/spdlog/sinks/stdout_sinks.h
  src/Threads/adc.h
  src/PID/MiniPID.h
  src/Threads/pid.h
  src/ImageProcessing/acquisition.h
  src/ImageProcessing/adaptivethreshold.h
  src/ImageProcessing/boundariesfinder.h
  src/Threads/rtimuloop.h
  src/Threads/serialport.h
  src/Threads/broadcaster.h
  src/ImageProcessing/capture.h
  src/Threads/epd.h
  src/EPD/EPD_2in13.h
  src/EPD/DEV_EPD_Config.h
  src/EPD/GUI_Paint.h
  src/EPD/Fonts/fonts.h
  src/Servo/DEV_Servo_Config.h
  src/Servo/PCA9685_Servo.h
)

add_executable(${TARGET_NAME}
    ${SRC}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME}  Qt5::Core  ${OpenCV_LIBS}  Qt5SerialPort  Qt5::Network raspicam_cv)
#${OpenCV_LIBS}   ${OPENGL_LIBRARIES}  Threads::Threads   ${QT_LIBRARIES} Qt5::Core Threads::Threads
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/interface/vcos" "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies/interface" "${CMAKE_CURRENT_SOURCE_DIR}/../dependencies")
