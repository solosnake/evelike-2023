cmake_minimum_required(VERSION 3.1)

# To run just one test use command arg "--gtest_filter=TESTSUITE.TESTNAME"
project( test_solosnake )
set (CMAKE_CXX_STANDARD 17)

include_directories ("../../../external/${LUA_DIR}/src")
include_directories ("../../../solosnake/include")
include_directories ("../include")

if( WIN32 )
    # Windows specific files:
    set (
        os_files_list
		)
    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    source_group( win32 FILES ${os_files_list} )
    
    add_definitions(-DSS_OS_WINDOWS)

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

    # Enable Multiprocessor compilation.
    set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP" )
    set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP" )
    
    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )
    add_definitions(-D_VARIADIC_MAX=10)
endif( MSVC)

add_definitions(-DBLUEDATADIR="${CMAKE_SOURCE_DIR}/solosnake/test_lunar/data")
add_definitions(-DGLEW_STATIC)

if(UNIX)
    add_definitions(-std=c++1y)     
      
    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)    
        # Silence warnings due to Boost:
        add_definitions(-Wno-unused-local-typedefs)
        add_definitions(-Wno-unsafe-loop-optimizations)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        # Clang
    endif()
    
    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)

    add_definitions(-DSS_OS_LINUX)
    
endif(UNIX)

add_executable(
    test_lunar
    main.cpp
    ../../../solosnake/src/log_type.cpp
    ../../../solosnake/src/logging.cpp
    ../../../solosnake/src/external/lua.cpp	
    ../../../solosnake/src/external/lua2.cpp	
    ../../../solosnake/src/external/lua3.cpp	
    ../../../solosnake/src/external/lua4.cpp	
    ../../../solosnake/src/external/lua/lunar.cpp	
    ${os_files_list}
    test_lunar.cpp
	test_lua.cpp
    )

target_compile_definitions(test_lunar PRIVATE TESTING_NO_AUTOMATIC_MAIN )

if (UNIX)
    target_link_libraries( test_lunar "pthread" )   
    target_link_libraries( test_lunar "rt" )
endif (UNIX)

#enable_testing()
#--build-run-dir "${CMAKE_SOURCE_DIR}/solosnake/src/external/lua"
#add_test( testsolosnake test_solosnake --output-on-failure -V )
#set( CTEST_OUTPUT_ON_FAILURE 1 )
#include(CTest)
