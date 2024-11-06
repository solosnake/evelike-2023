cmake_minimum_required(VERSION 3.1)

project( test_x11 )
set (CMAKE_CXX_STANDARD 17)
set( test_x11name "test_x11" )

include_directories ("../../../external/${BOOST_DIR}")
include_directories ("../../../solosnake/include")
include_directories ("../include")
include_directories ("./external")

link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )


add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DGLEW_STATIC)
add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64")    

if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)    
    # Silence warnings due to Boost:
    add_definitions(-Wno-unused-local-typedefs))
    add_definitions(-Wno-unsafe-loop-optimizations)
elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    # Clang
endif()

# Silence warnings:
add_definitions(-Wno-variadic-macros)
add_definitions(-Wno-old-style-cast)
add_definitions(-Wno-strict-aliasing)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
message(${OPENGL_INCLUDE_DIR})
message(${OPENGL_LIBRARIES})

list( APPEND CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS} -g ")
    
add_executable(
  ${test_x11name}
  main.cpp
  ../../../solosnake/src/x11/window_opengl.cpp
  ../../../solosnake/src/x11/x11.cpp
  ../../../solosnake/src/ioswindow.cpp
  ../../../solosnake/src/get_opengl_version.cpp
  ../../../solosnake/src/logging.cpp
  ../../../solosnake/src/log_type.cpp
  ../../../solosnake/src/inputs.cpp
  ../../../solosnake/src/iinput_events.cpp
  )
  
target_link_libraries( ${test_x11name} "libboost_chrono-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
target_link_libraries( ${test_x11name} "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
target_link_libraries( ${test_x11name} "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
target_link_libraries( ${test_x11name} "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
target_link_libraries( ${test_x11name} "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
target_link_libraries( ${test_x11name} "pthread" )
target_link_libraries( ${test_x11name} "GL" )
target_link_libraries( ${test_x11name} "X11" )
