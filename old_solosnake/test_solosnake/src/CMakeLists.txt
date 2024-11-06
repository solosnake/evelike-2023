cmake_minimum_required(VERSION 3.1)

# To run just one test use command arg "--gtest_filter=TESTSUITE.TESTNAME"
project( test_solosnake )
set (CMAKE_CXX_STANDARD 17)

include_directories ("../../../external/${ALUT_DIR}/include")
include_directories ("../../../external/${ALUT_DIR}/AL")
include_directories ("../../../external/${FREETYPE_DIR}/include")
include_directories ("../../../external/${GLEW_DIR}/include")
include_directories ("../../../external/${LUA_DIR}/src")
include_directories ("../../../external/${OPENAL_DIR}/openal/include")
include_directories ("../../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../../external/${UTF8_DIR}/source")
include_directories ("../../../solosnake/include")
include_directories ("../include")

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
        test_win32_window_opengl.cpp 
        test_glut_window.cpp
        test_openal.cpp
        test_sound_renderer.cpp
        )
    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    
    add_definitions(-DSS_OS_WINDOWS)

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

    # Enable Multiprocessor compilation.
    set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP" )
    set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP" )
    
    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )
    add_definitions(-D_VARIADIC_MAX=10)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
endif( MSVC)

add_definitions(-DBLUEDATADIR="${CMAKE_SOURCE_DIR}/solosnake/test_solosnake/data")
add_definitions(-DGLEW_STATIC)

if(UNIX)
    # Linux specific files:
    set (
        os_files_list
        test_openal.cpp
        test_sound_renderer.cpp
        )
    link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64" )
    add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib64")
    set( glew_files_list "${CMAKE_SOURCE_DIR}/external/${GLEW_DIR}/src/glew.c" )
    set_source_files_properties( ${glew_files_list} PROPERTIES LANGUAGE CXX )
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    add_definitions(-std=c++1y)     
    
    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations)    
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    
    endif()
    
    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)
    
    # OpenAL
    find_package(OpenAL REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR})
    message(${OPENAL_INCLUDE_DIR})
    
    # OpenGL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_LIBRARIES})

    add_definitions(-DSS_OS_LINUX)

endif(UNIX)

add_executable(
    test_solosnake 
    ${os_files_list}
    ${glew_files_list}
    "${CMAKE_SOURCE_DIR}/external/${GLEW_DIR}/src/glew.c"
    ../include/quickloader.hpp
    ../include/meshsrc.hpp        
    ../../../solosnake/include/solosnake/quickmodel.hpp
    ../../../solosnake/include/solosnake/testing/testing.hpp
    ../../../solosnake/src/quickmodel.cpp
    generate_model_normal_tex.cpp
    Noise.cpp
    quickloader.cpp
    test_ascii.cpp
    test_attributereaders.cpp
    test_barytriangle.cpp
    test_bestfitrectangle.cpp
    test_bytebuffer.cpp
    test_directoryfiles.cpp
    test_case_insensitive_compare.cpp
    test_colour.cpp
    test_colourtriangle.cpp
    test_deferred_renderer.cpp
    test_fontletters.cpp
    test_fontprinter.cpp
    test_fontprinter2.cpp
    test_fontprinter3.cpp
    test_fontprinter4.cpp
    test_fontprinter5.cpp
    test_fontprinter6.cpp
    test_icosahedron.cpp
    test_image.cpp
    test_img_cache.cpp
    test_intersection2d.cpp
    test_iworldobject.cpp
    test_linearregression3d.cpp
    test_line2d.cpp
    test_line_circle_intersection.cpp
    test_linesegment.cpp
    test_loadedmodel.cpp
    test_load_models.cpp
    #test_lockstepserver.cpp
    test_lua.cpp
    test_makefont.cpp
    test_make_normalmap.cpp
    test_make_noise_texture_3d.cpp
    test_matrix3d.cpp
    test_mesh_cache.cpp
    test_opengl_blendrenderbuffers.cpp
    test_opengl_buffers.cpp
    test_opengl_renderbuffer.cpp
    test_opengl_renderbuffers.cpp
    test_persistance.cpp
    test_point.cpp
    test_radians.cpp
    test_rectangle_hittest.cpp
    test_renderer_header.cpp
    test_screenxy.cpp
    test_styledwidgetrenderer.cpp
    test_throw.cpp
    test_utf8text.cpp
    main.cpp
    )

target_compile_definitions( test_solosnake PRIVATE TESTING_NO_AUTOMATIC_MAIN )
target_link_libraries( test_solosnake solosnake )
target_link_libraries( test_solosnake "freetype${SOLOSNAKE_WORDSIZE}-lib" )

if (UNIX)
    target_link_libraries( test_solosnake ${OPENGL_LIBRARIES} )
    target_link_libraries( test_solosnake ${OPENAL_LIBRARIES} ) 
    target_link_libraries( test_solosnake "libboost_chrono-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_solosnake "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( test_solosnake "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_solosnake "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    target_link_libraries( test_solosnake "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( test_solosnake "pthread" )   
    target_link_libraries( test_solosnake "rt" )
endif (UNIX)

#enable_testing()
#--build-run-dir "${CMAKE_SOURCE_DIR}/solosnake/src/external/lua"
#add_test( testsolosnake test_solosnake --output-on-failure -V )
#set( CTEST_OUTPUT_ON_FAILURE 1 )
#include(CTest)
