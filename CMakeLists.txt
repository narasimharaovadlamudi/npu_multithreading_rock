cmake_minimum_required(VERSION 3.10 FATAL_ERROR)

project(YoloV5_NPU_multi)

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CheckIncludeFile)

# Set compiler options
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -fexceptions -pthread -O3")

find_package(OpenCV REQUIRED)   

list(APPEND CMAKE_LIBRARY_PATH "/usr/local/lib/")

set(SOURCES src/main.cc src/postprocess.cc src/rkYolov5s.cc )


include_directories(include ${OpenCV_INCLUDE_DIRS} ./include )

set(LINK_OPT -fopenmp -s -ldl -lpthread -pthread -O3 -lgomp -DNDEBUG -rdynamic -s)

add_executable(YoloV5_NPU_multi ${SOURCES})

set(EXECUTABLE_OUTPUT_PATH "../")

target_link_libraries(YoloV5_NPU_multi ${OpenCV_LIBS} /usr/local/lib/librknnrt.so)

