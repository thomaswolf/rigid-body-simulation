
add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

### GLFW ###

add_subdirectory (glfw)

include_directories(
	glfw/include/GLFW/
	glew/include/
)

set(OPENGL_LIBRARY
	${OPENGL_LIBRARY}
	-lGL -lGLU -lXrandr -lXext -lX11 -lrt
	${CMAKE_DL_LIBS}
	${GLFW_LIBRARIES}
)

### GLEW ###

set(GLEW_SOURCE
	glew/src/glew.c
)

set(GLEW_HEADERS
)


add_library( GLEW_1130 STATIC
	${GLEW_SOURCE}
	${GLEW_INCLUDE}
)

target_link_libraries(GLEW_1130
	${OPENGL_LIBRARY}
	${EXTRA_LIBS}
)

### ooqp  ###

#add_subdirectory (OOQP)

### ooqp eigen interface ###

#add_subdirectory (ooqp_eigen_interface)

#include_directories(QuadProgpp/include/)

### ASSIMP ###
# AssImp already has a CMakeLists.txt so let's use these

# Compile built-in, modified version of Zlib
#include(CheckIncludeFile)
#include(CheckTypeSize)
#include(CheckFunctionExists)
#add_subdirectory( assimp-3.0.1270/contrib/zlib )

# Compile without Boost
#include_directories( assimp-3.0.1270/code/BoostWorkaround )
#add_definitions( -DASSIMP_BUILD_BOOST_WORKAROUND )

# Compile AssImp
#set( LIB_INSTALL_DIR "lib")
#set(LIBASSIMP_COMPONENT libassimp3.0-r1270-OGLtuts)
#set(ZLIB_LIBRARIES zlib)
#set(BUILD_STATIC_LIB ON)
#set(ZLIB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib)
#add_subdirectory( assimp-3.0.1270/code )
