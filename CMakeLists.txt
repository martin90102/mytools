cmake_minimum_required(VERSION 3.0.0)

project(test02)

find_package(OpenCV REQUIRED)
find_package(Pangolin REQUIRED)

add_compile_options(-std=c++11)

# include_directories("include")
add_executable(xxx plotTrajectory.cpp)
target_link_libraries(xxx
    ${CMAKE_THREAD_LIBS_INIT}
    ${Pangolin_LIBRARIES}
    ${OpenCV_LIBS})
