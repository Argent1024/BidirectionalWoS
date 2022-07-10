# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("BidirectionalWoS")


cmake_minimum_required (VERSION 3.8)

if (UNIX)
	SET(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} "-std=c++11 -pthread")
endif (UNIX)

# include directories
include_directories(
					src/ext/eigen/
					src/ext/tinyobj/
					src/Core/
)

set(srcs_file
	"src/Core/ImageBuffer.h"
	"src/Core/Common.h"
	"src/Core/Common.cpp"
	"src/Core/Source.h"
	"src/Core/Boundary.h"
	"src/Core/PDE.h"
    "src/Core/SolverWoS.h" 
	"src/Core/Scene.h" "src/Core/Scene.cpp" 
	"src/Core/Boundary.cpp"  
 )

 
add_executable(ForwardWoS 
	"src/ForwardWoS.cpp"
	${srcs_file}  
)

add_executable(ReverseWoS
    "src/ReverseWoS.h"
	"src/ReverseWoS.cpp"
	${srcs_file}
)

# add_executable(FinalGather
# 	"FinalGather.cpp"
# 	${srcs_file}
# )


# Add fcpw to the project
add_subdirectory(src/ext/fcpw)

target_link_libraries(ForwardWoS fcpw)
target_include_directories(ForwardWoS PUBLIC ${FCPW_EIGEN_INCLUDES})
target_include_directories(ForwardWoS PUBLIC ${FCPW_ENOKI_INCLUDES})

target_link_libraries(ReverseWoS fcpw)
target_include_directories(ReverseWoS PUBLIC ${FCPW_EIGEN_INCLUDES})
target_include_directories(ReverseWoS PUBLIC ${FCPW_ENOKI_INCLUDES})
