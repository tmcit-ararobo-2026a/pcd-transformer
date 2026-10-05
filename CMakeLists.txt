cmake_minimum_required(VERSION 3.10)
project(pcd_transformer)

find_package(PCL 1.7 REQUIRED)

include_directories(${PCL_INCLUDE_DIRS})
link_directories(${PCL_LIBRARY_DIRS})
add_definitions(${PCL_DEFINITIONS})

add_executable(pcd_transformer pcd_transformer.cpp)
target_link_libraries(pcd_transformer ${PCL_LIBRARIES})