cmake_minimum_required(VERSION 3.1)

# To run just one test use command arg "--gtest_filter=TESTSUITE.TESTNAME"
project( test_gui )
set (CMAKE_CXX_STANDARD 17)

include_directories ("../../../external/${FREETYPE_DIR}/include")
include_directories ("../../../external/${LUA_DIR}/src")
include_directories ("../../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../../external/${UTF8_DIR}/source")
include_directories ("../../../solosnake/include")
include_directories ("../include")
include_directories ("./external")

# Add paths to x32 or x64 libs.
if (CMAKE_SIZEOF_VOID_P EQUAL 4)
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib32" )
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win32" )
else ()
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win64" )
endif ()

if( WIN32 )
    
    # Enable Multiprocessor compilation.
    set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP" )
    set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP" )
    
    set ( os_files_list ) #WIN32 )
    add_definitions(-D_WIN32)
    add_definitions(-D_WIN32_WINDOWS=0x0501)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    
    add_definitions(-DSS_OS_WINDOWS)

endif(WIN32)

# Replace MDd and MD with MTd and MT (see http://www.cmake.org/Wiki/CMake_FAQ)
if( MSVC )
    foreach(flag_var
            CMAKE_CXX_FLAGS 
            CMAKE_CXX_FLAGS_DEBUG 
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_CXX_FLAGS_MINSIZEREL 
            CMAKE_CXX_FLAGS_RELWITHDEBINFO )
       if(${flag_var} MATCHES "/MD")
          string(REGEX REPLACE "/MD" "/MT" ${flag_var} "${${flag_var}}")
       endif(${flag_var} MATCHES "/MD")
    endforeach(flag_var)
    add_definitions(-D_VARIADIC_MAX=10)
endif( MSVC )

add_definitions(-DGLEW_STATIC)

if(UNIX)
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )
    
    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)    
        # Silence warnings due to Boost:
        add_definitions(-Wno-unused-local-typedefs)
        add_definitions(-Wno-unsafe-loop-optimizations)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
        # Clang
    endif()
    
    add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64")
    list( APPEND CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS} ")    
    
    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)
    
    add_definitions(-DSS_OS_LINUX)
endif(UNIX)

set( tests_list test_gui.cpp test_layout.cpp )

source_group( tests FILES ${tests_list} )

add_executable(
    test_gui
    ${os_files_list}
    ${tests_list}
    main.cpp
    ../../../solosnake/src/bgr.cpp
    ../../../solosnake/src/bgra.cpp
    ../../../solosnake/src/gui.cpp
    ../../../solosnake/src/guishape.cpp
    ../../../solosnake/src/guiloader.cpp
    ../../../solosnake/src/layout.cpp
    ../../../solosnake/src/logging.cpp
    ../../../solosnake/src/iwidget.cpp
    ../../../solosnake/src/iwidgetname.cpp
    ../../../solosnake/src/inputs.cpp
    ../../../solosnake/src/iinput_events.cpp
    ../../../solosnake/src/iscreenstate.cpp
    ../../../solosnake/src/ishape.cpp
    ../../../solosnake/src/iwidgetcollection.cpp
    ../../../solosnake/src/log_type.cpp
    ../../../solosnake/src/nextscreen.cpp
    ../../../solosnake/src/unicode_to_utf8.cpp
    ../../../solosnake/src/utf8text.cpp
    ../../../solosnake/src/external/xml.cpp
    ../../../solosnake/src/ixmlelementreader.cpp
    )

target_compile_definitions( test_gui PRIVATE TESTING_NO_AUTOMATIC_MAIN )

if (UNIX)  
    target_link_libraries( test_gui "libboost_chrono-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_gui "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  ) 
    target_link_libraries( test_gui "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" ) 
    target_link_libraries( test_gui "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( test_gui "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )    
    target_link_libraries( test_gui "pthread" )
endif (UNIX)
