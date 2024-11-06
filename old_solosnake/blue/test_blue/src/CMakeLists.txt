cmake_minimum_required(VERSION 3.1)

# To run just one test use command arg "--gtest_filter=TESTSUITE.TESTNAME"
project( test_blue )
set (CMAKE_CXX_STANDARD 17)

# The version number.
set (blue_version_major 0)
set (blue_version_minor 0)
set (blue_version_patch 0)
set (bluename "blue_test" )
set (companyname "solosnake")

# CMake will inject Major.Minor into the code.
configure_file( "../../include/solosnake/blue/blue_version.hpp.in" "blue_version_cmake_generated.hpp" )

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories ("../../../../external/${ALUT_DIR}/include")
include_directories ("../../../../external/${ALUT_DIR}/AL")
include_directories ("../../../../external/${FREETYPE_DIR}/include")
include_directories ("../../../../external/${LUA_DIR}/src")
include_directories ("../../../../external/${OPENAL_DIR}/openal/include")
include_directories ("../../../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../../../external/${UTF8_DIR}/source")
include_directories ("../../../../solosnake/include")
include_directories ("../../include")

# Add paths to x32 or x64 libs.
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib32" )
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win32" )
else ()
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win64" )
endif ()

if( WIN32 )
    # Windows specific files:
    set (
        os_files_list
        win32_test_blue_main.cpp
        )
    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32 -DNOMINMAX)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    source_group( win32 FILES ${os_files_list} )
endif( WIN32 )

if( MSVC )
    # Replace MDd and MD with MTd and MT (see http://www.cmake.org/Wiki/CMake_FAQ)
    foreach(flag_var
            CMAKE_C_FLAGS 
            CMAKE_C_FLAGS_DEBUG 
            CMAKE_C_FLAGS_RELEASE
            CMAKE_C_FLAGS_MINSIZEREL 
            CMAKE_C_FLAGS_RELWITHDEBINFO
            CMAKE_CXX_FLAGS 
            CMAKE_CXX_FLAGS_DEBUG 
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL 
            CMAKE_CXX_FLAGS_RELWITHDEBINFO )
       if(${flag_var} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
       endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)

    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )
    add_definitions(-D_VARIADIC_MAX=10)
endif( MSVC)

add_definitions(-DBLUEDATADIR="${CMAKE_SOURCE_DIR}/solosnake/test_blue/data")
add_definitions(-DGLEW_STATIC)

if(UNIX)
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )
    add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64")
    set_source_files_properties( ${glew_files_list} PROPERTIES LANGUAGE CXX )
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    add_definitions(-std=c++1y) 
    
    # OpenAL
    find_package(OpenAL REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR})
    message(${OPENAL_INCLUDE_DIR})
    
    # OpenGL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_LIBRARIES})
endif(UNIX)

set( blue_files
     ../../src/blue_hardpoint_angles.cpp
     ../../src/blue_asteroid.cpp
     ../../src/blue_compiler.cpp
     ../../src/blue_compilationresult.cpp
     ../../src/blue_gamehexgrid.cpp 
     ../../src/blue_hexgrid.cpp
     ../../src/blue_hex_steps.cpp   
     ../../src/blue_sun.cpp  
     ../../src/blue_tradables.cpp
     ../../src/blue_tradabletypes.cpp
     ../../src/blue_xml_board.cpp 
     )

source_group( blue FILES ${blue_files} )

add_executable( test_blue 
    main.cpp
    ${os_files_list}
    ${blue_files}
    test_hexgrid.cpp
    test_hardpoint_angles.cpp
    test_hex_steps_under_line.cpp
    test_blue_renderer_header.cpp
    test_compiler.cpp
    test_sphere_geometry.cpp    
    test_xml_board.cpp    
    )

target_compile_definitions( test_blue PRIVATE TESTING_NO_AUTOMATIC_MAIN )

target_link_libraries( test_blue solosnake )
target_link_libraries( test_blue "freetype${SOLOSNAKE_WORDSIZE}-lib" )

if (UNIX)
    target_link_libraries( test_blue ${OPENGL_LIBRARIES} )
    target_link_libraries( test_blue ${OPENAL_LIBRARIES} ) 
    target_link_libraries( test_blue "libboost_chrono-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_blue "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( test_blue "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_blue "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( test_blue "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_blue "libboost_regex-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_blue "pthread" )   
    target_link_libraries( test_blue "rt" )
endif (UNIX)

#enable_testing()
#--build-run-dir "${CMAKE_SOURCE_DIR}/solosnake/src/external/lua"
#add_test( testblue test_blue --output-on-failure -V )
#set( CTEST_OUTPUT_ON_FAILURE 1 )
#include(CTest)
