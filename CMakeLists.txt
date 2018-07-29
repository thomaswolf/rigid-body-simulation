cmake_minimum_required (VERSION 3.0)
project (PhysEngine)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(OpenGL REQUIRED)
#find_package(Eigen3 REQUIRED)
#find_package(CGAL REQUIRED)
find_package(Boost REQUIRED)
find_package(OpenMP REQUIRED)
find_package(Boost COMPONENTS regex)

#include_directories(${EIGEN3_INCLUDE_DIR})
include_directories(${Boost_INCLUDE_DIRS}) 
#include(${CGAL_USE_FILE})
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")

# Compile external dependencies 
add_subdirectory (external)

include_directories(
	external/glfw/include/GLFW/
	external/glm/
	external/glew/include/
	common/
	#external/ooqp_eigen_interface/include/
	#external/assimp/include/
	.
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	#${CGAL_LIBRARIES}
	#${CGAL_3RD_PARTY_LIBRARIES}
	${Boost_REGEX_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
	-lcgal
	-lcgal_core
	-lmpfr
	-lgmp
	-fopenmp
	-frounding-math
	-DPLATFORM_DESKTOP
)

macro(add_resource path)
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/resources/${path} .
)
endmacro(add_resource)

add_executable(main platform/desktop/main.cpp)
target_link_libraries(main ${ALL_LIBS})
set_property(TARGET main PROPERTY CXX_STANDARD 11)


### add all resources ###
file(GLOB files "resources/*")
foreach(file ${files})
	MESSAGE(STATUS ${file})
	configure_file(
		${file} .
	)
endforeach()
