
cmake_minimum_required(VERSION 2.8)

PROJECT(dataToPCDs)

find_package(PCL 1.3 REQUIRED)
FIND_PACKAGE(OpenCV REQUIRED)
include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable(dataToPCDs dataToPCDs.cpp ../kinect.h ../kinect.cpp Background.h Background.cpp)

target_link_libraries( dataToPCDs ${PCL_LIBRARIES}) 
TARGET_LINK_LIBRARIES( dataToPCDs ${OpenCV_LIBS} )