cmake_minimum_required (VERSION 2.8)
project (project_1)
find_package( OpenCV REQUIRED)
add_executable (polygons polygons.cpp)
target_link_libraries(polygons ${OpenCV_LIBS})
target_link_libraries(polygons m)

