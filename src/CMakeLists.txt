# CMakeList.txt : CMake project for WoS, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

if (UNIX)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
endif (UNIX)

# include directories
include_directories(
					ext/eigen/
					ext/tinyobj/
					Core/
)

set(srcs_file
	"Core/ImageBuffer.h"
	"Core/Common.h"
	"Core/Common.cpp"
	"Core/Source.h"
	"Core/Boundary.h"
	"Core/PDE.h"
    "Core/SolverWoS.h" 
	"Core/Scene.h" "Core/Scene.cpp" 
	"Core/Boundary.cpp"  
 )
