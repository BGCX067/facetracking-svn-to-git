cmake_minimum_required(VERSION 2.8) 


project(btracker)

add_definitions(-D__LINUX__)
add_definitions(-DNDEBUG)
set(CMAKE_CXX_FLAGS "-O2 -msse3")

include_directories(/res/users/acohen/subsystem/include)
link_directories(/res/users/acohen/subsystem/lib)
include_directories(BTrackerCore BTrackerCore/video BTrackerCore/warp BTrackerCore/algorithms)

aux_source_directory(BTrackerBatch Batch)
aux_source_directory(BTrackerCore Core)
aux_source_directory(BTrackerCore/video Video)
aux_source_directory(BTrackerCore/warp Warp)
aux_source_directory(BTrackerCore/algorithms Algorithms)

LIST(REMOVE_ITEM Video BTrackerCore/video/FFVideo.cpp)
LIST(REMOVE_ITEM Warp BTrackerCore/warp/CLController.cpp)
LIST(REMOVE_ITEM Warp BTrackerCore/warp/CLWarp.cpp)

add_executable(btracker ${Batch} ${Core} ${Video} ${Warp} ${Algorithms})

target_link_libraries(btracker boost_system boost_filesystem highgui cv)
