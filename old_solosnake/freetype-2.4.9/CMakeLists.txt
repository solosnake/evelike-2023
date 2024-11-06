###############################################################################
# freetype lib
###############################################################################
cmake_minimum_required(VERSION 3.1)
project (lib-freetype)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED SOLOSNAKE_WORDSIZE)
  set(SOLOSNAKE_WORDSIZE "64")
endif()

if(SOLOSNAKE_WORDSIZE STREQUAL "32")
    message("${3dsconvertname} 32 bit build")
elseif(SOLOSNAKE_WORDSIZE STREQUAL "64")
    message("${3dsconvertname} 64 bit build")
endif()


# Add paths to x32 or x64 libs.
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win${SOLOSNAKE_WORDSIZE}" )

###############################################################################
# Build freetype (version 2.4.9 src listing)
###############################################################################

include_directories ("../../external/${FREETYPE_DIR}/include")
include_directories ("../../external/${FREETYPE_DIR}/src")

set( ftdir "../../external/${FREETYPE_DIR}" )
set( ftsrc "${ftdir}/src" )
set( freetypename "freetype${SOLOSNAKE_WORDSIZE}-lib" )

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
    
    # Release - Add whole program optimisation & link-time code generation.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Release - Add intrinsics
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oi" )
    
    # Release - 'Fibre Safe' optimisations.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GT" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GT" )

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
    add_definitions(/W2) 

endif( MSVC )

if( WIN32 )
    set ( ft_os_files_list ${ftdir}/builds/win32/ftdebug.c )
endif( WIN32 )

if( UNIX )    
    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
      message("${freetypename} Debug build")
    else()
      add_definitions(-O3)
      message("${freetypename} Optimised (-O3) build")
    endif()
        
    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations)    
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    
    endif()
    
    add_definitions(-mfpmath=sse -msse -msse2 -msse3)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}")

    add_definitions(-ansi)    
    
    # Silence warnings:
    add_definitions(-Wno-variadic-macros)
    add_definitions(-Wno-old-style-cast)
    add_definitions(-Wno-strict-aliasing)
 
    # Linux specific files:
    set ( ft_os_files_list  )
    source_group( x11 FILES ${os_files_list} )    
endif( UNIX )

add_definitions(-DFT2_BUILD_LIBRARY)

add_library( 
    ${freetypename}
    STATIC  
    ${ft_os_files_list}
    ${ftsrc}/base/ftbbox.c
    ${ftsrc}/base/ftgxval.c
    ${ftsrc}/base/ftlcdfil.c
    ${ftsrc}/base/ftmm.c
    ${ftsrc}/base/ftotval.c
    ${ftsrc}/base/ftpatent.c
    ${ftsrc}/base/ftpfr.c
    ${ftsrc}/base/ftsynth.c
    ${ftsrc}/base/fttype1.c
    ${ftsrc}/base/ftwinfnt.c
    ${ftsrc}/base/ftxf86.c
    ${ftsrc}/pcf/pcf.c
    ${ftsrc}/pfr/pfr.c
    ${ftsrc}/psaux/psaux.c
    ${ftsrc}/pshinter/pshinter.c
    ${ftsrc}/psnames/psmodule.c
    ${ftsrc}/raster/raster.c
    ${ftsrc}/sfnt/sfnt.c
    ${ftsrc}/truetype/truetype.c
    ${ftsrc}/type1/type1.c
    ${ftsrc}/cid/type1cid.c
    ${ftsrc}/type42/type42.c
    ${ftsrc}/winfonts/winfnt.c
    ${ftsrc}/autofit/autofit.c
    ${ftsrc}/bdf/bdf.c
    ${ftsrc}/cff/cff.c
    ${ftsrc}/base/ftbase.c
    ${ftsrc}/base/ftbitmap.c
    ${ftsrc}/cache/ftcache.c
    ${ftsrc}/base/ftfstype.c
    ${ftsrc}/base/ftgasp.c
    ${ftsrc}/base/ftglyph.c
    ${ftsrc}/gzip/ftgzip.c
    ${ftsrc}/base/ftinit.c
    ${ftsrc}/lzw/ftlzw.c
    ${ftsrc}/base/ftstroke.c
    ${ftsrc}/base/ftsystem.c
    ${ftsrc}/smooth/smooth.c
    )
    
if( UNIX )   
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
      set_target_properties(${freetypename} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif ()
endif( UNIX )
