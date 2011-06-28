cmake_minimum_required(VERSION 2.6)

PROJECT(VoronoiNeighbors)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

ADD_EXECUTABLE(VoronoiNeighbors VoronoiNeighbors.cpp)
TARGET_LINK_LIBRARIES(VoronoiNeighbors ${ITK_LIBRARIES} ${VTK_LIBRARIES})
