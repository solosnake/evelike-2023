cmake_minimum_required(VERSION 3.1)

project(datamaker)
set (CMAKE_CXX_STANDARD 17)

set (datamakername  "datamaker" )
set (companyname "solosnake")

if(NOT DEFINED SOLOSNAKE_WORDSIZE)
  set(SOLOSNAKE_WORDSIZE "64")
endif()

if(SOLOSNAKE_WORDSIZE STREQUAL "32")
    message("${datamakername} 32 bit build")
elseif(SOLOSNAKE_WORDSIZE STREQUAL "64")
    message("${datamakername} 64 bit build")
endif()

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories ("../../solosnake/include")
include_directories ("../../solosnake/blue/include")
include_directories ("../../external/${ALUT_DIR}/include")
include_directories ("../../external/${ALUT_DIR}/AL")
include_directories ("../../external/${BOOST_DIR}")
include_directories ("../../external/${FREETYPE_DIR}/include")
include_directories ("../../external/${GLEW_DIR}/include")
include_directories ("../../external/${LUA_DIR}/src")
include_directories ("../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../external/${UTF8_DIR}/source")
include_directories ("../../external/${ASSIMP_DIR}/include")

set (datamakername "${datamakername}.x${SOLOSNAKE_WORDSIZE}" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${ASSIMP_DIR}/lib" )

# Replace MDd and MD with MTd and MT (see http://www.cmake.org/Wiki/CMake_FAQ)
if( WIN32 )
    include_directories ("../../external/${OPENAL_DIR}/openal/include")

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
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - 'Fibre Safe' optimisations.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GT" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )

    # Add machine type
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  /MACHINE:X86" )
    else ()
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}  /MACHINE:X64" )
    endif ()

    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    add_definitions(-D_VARIADIC_MAX=10)
        
    # 64-bit automatically has this enabled, so this will generate a warning.
    if(SOLOSNAKE_WORDSIZE STREQUAL "32")
        add_definitions(/arch:SSE2)
    endif()

    set( os_specific_files )

endif( WIN32 )

if( UNIX )
    list( APPEND CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS} ")

    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations)    
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    
    endif()
    
    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
      message("${datamakername} Debug build")
    else()
      add_definitions(-O3)
      message("${datamakername} 03 build")
    endif()

    add_definitions(-mfpmath=sse -msse -msse2 -msse3)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}")
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-Winit-self)
    add_definitions(-Wold-style-cast)
    add_definitions(-Woverloaded-virtual)
    add_definitions(-Wuninitialized)
    add_definitions(-Wmissing-declarations)
    add_definitions(-Winit-self)
    add_definitions(-pedantic)
    add_definitions(-pedantic-errors)
    
    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)

    # OpenGL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_LIBRARIES})

    set( os_specific_files )
endif(UNIX)

add_executable( ${datamakername} 
                ${os_specific_files}
                datamaker.cpp
                ../blue/src/blue_component.cpp
                ../blue/src/blue_softpoint.cpp 
                ../blue/src/blue_hardpoint.cpp
                ../blue/src/blue_hardpointshot.cpp
                ../blue/src/blue_hardpointshooter.cpp
                ../blue/src/blue_thruster.cpp 
                ../blue/src/blue_tradabletypes.cpp 
                ../blue/src/blue_hardwaremodifier.cpp 
                )

if (UNIX)
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
      set_target_properties(${datamakername} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif ()
    target_link_libraries( ${datamakername} ${OPENGL_LIBRARY} )
    target_link_libraries( ${datamakername} ${OPENAL_LIBRARY} )
    target_link_libraries( ${datamakername} "libboost_chrono-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( ${datamakername} "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( ${datamakername} "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( ${datamakername} "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( ${datamakername} "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( ${datamakername} "pthread" )
    target_link_libraries( ${datamakername} "X11" )
    target_link_libraries( ${datamakername} "GL" )
    target_link_libraries( ${datamakername} "dl" )
    target_link_libraries( ${datamakername} "rt" )
endif (UNIX)

target_link_libraries( ${datamakername} assimp )
target_link_libraries( ${datamakername} solosnake )

# From p.163 of Mastering Cmake...
include (InstallRequiredSystemLibraries)
install (TARGETS ${datamakername} DESTINATION bin)
