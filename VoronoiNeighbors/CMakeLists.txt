cmake_minimum_required(VERSION 2.6)

PROJECT(VoronoiNeighbors)

FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})

# ADD_EXECUTABLE(VoronoiNeighborsExample Example.cpp VoronoiNeighbors.cpp)
# TARGET_LINK_LIBRARIES(VoronoiNeighborsExample ${ITK_LIBRARIES} ${VTK_LIBRARIES})

ADD_EXECUTABLE(VoronoiNeighborsDemo Demo.cpp VoronoiNeighbors.cpp)
TARGET_LINK_LIBRARIES(VoronoiNeighborsDemo ${ITK_LIBRARIES} ${VTK_LIBRARIES})
