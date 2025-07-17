cmake_minimum_required(VERSION 3.20)
project(PlateRecognizeSystem)

set(CMAKE_CXX_STANDARD 17)

# 查找OpenCV库
find_package(OpenCV 4.11 REQUIRED)

add_executable(PlateRecognizeSystem main.cpp)

# 链接OpenCV库
target_include_directories(PlateRecognizeSystem PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(PlateRecognizeSystem PRIVATE ${OpenCV_LIBS})
