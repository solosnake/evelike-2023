cmake_minimum_required(VERSION 3.1)

project(fontmaker)
set (CMAKE_CXX_STANDARD 17)

set( fontmakername "fontmaker" )

if(NOT DEFINED SOLOSNAKE_WORDSIZE)
  set(SOLOSNAKE_WORDSIZE "64")
endif()

if(SOLOSNAKE_WORDSIZE STREQUAL "32")
    message("${fontmakername} 32 bit build")
elseif(SOLOSNAKE_WORDSIZE STREQUAL "64")
    message("${fontmakername} 64 bit build")
endif()

include_directories ("../../solosnake/include")

# Add paths to x32 or x64 libs.
set( fontmakername "${fontmakername}.x${SOLOSNAKE_WORDSIZE}" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}" )

# Replace MDd and MD with MTd and MT (see http://www.cmake.org/Wiki/CMake_FAQ)
if( MSVC )
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

    # Release - Add whole program optimisation & link-time code generation.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - Add intrinsics
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - 'Fibre Safe' optimisations.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )

    # Add machine type
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  /MACHINE:X86" )
    else ()
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  /MACHINE:X64" )
    endif ()

    # 64-bit automatically has this enabled, so this will generate a warning.
    if(SOLOSNAKE_WORDSIZE STREQUAL "32")
        add_definitions(/arch:SSE2)
    endif()

    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    add_definitions(-D_VARIADIC_MAX=10)

endif( MSVC )

if( UNIX )
    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
    else()
      add_definitions(-O3)
    endif()

    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations)
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")

    endif()

    add_definitions(-Wno-variadic-macros)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    set( glew_files_list "${CMAKE_SOURCE_DIR}/external/${GLEW_DIR}/src/glew.c" )
    set_source_files_properties( ${glew_files_list} PROPERTIES LANGUAGE CXX )

    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)

    # OpenGL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_LIBRARIES})

endif( UNIX )

add_definitions(-DFT2_BUILD_LIBRARY)

add_executable( ${fontmakername} main.cpp )

target_link_libraries( ${fontmakername} solosnake )

if( UNIX )
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
      set_target_properties(${fontmakername} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif ()

    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
      message("${fontmakername} Debug build")
    else()
      add_definitions(-O3)
      message("${fontmakername} Optimised (-O3) build")
    endif()

    target_link_libraries( ${fontmakername} "pthread" )
endif( UNIX )

# From p.163 of Mastering Cmake...
include (InstallRequiredSystemLibraries)
install (TARGETS ${fontmakername} DESTINATION bin)



