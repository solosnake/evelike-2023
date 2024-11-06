cmake_minimum_required(VERSION 3.1)
cmake_policy(VERSION 3.1)

project(libpng C)

set(CMAKE_C_STANDARD 99)
set(PNGLIB_MAJOR 1)
set(PNGLIB_MINOR 6)
set(PNGLIB_REVISION 39)
set(PNGLIB_SUBREVISION 0)
set(PNGLIB_NAME libpng${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_VERSION ${PNGLIB_MAJOR}.${PNGLIB_MINOR}.${PNGLIB_REVISION})
set(PNGLIB_SHARED_SOVERSION ${PNGLIB_MAJOR}${PNGLIB_MINOR})
set(PNGLIB_SHARED_VERSION ${PNGLIB_SHARED_SOVERSION}.${PNGLIB_REVISION}.${PNGLIB_SUBREVISION})

include(GNUInstallDirs)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/../zlib-1.2.13" "${CMAKE_CURRENT_BINARY_DIR}/build_zlib")

# No PNG_HARDWARE_OPTIMIZATIONS:

# Set definitions and sources for ARM.
if(TARGET_ARCH MATCHES "^arm" OR
   TARGET_ARCH MATCHES "^aarch64")
  add_definitions(-DPNG_ARM_NEON_OPT=0)
endif()

# Set definitions and sources for PowerPC.
if(TARGET_ARCH MATCHES "^powerpc*" OR
   TARGET_ARCH MATCHES "^ppc64*")
  add_definitions(-DPNG_POWERPC_VSX_OPT=0)
endif()

# Set definitions and sources for Intel.
if(TARGET_ARCH MATCHES "^i?86" OR
   TARGET_ARCH MATCHES "^x86_64*")
  add_definitions(-DPNG_INTEL_SSE_OPT=0)
endif()

# Set definitions and sources for MIPS.
if(TARGET_ARCH MATCHES "mipsel*" OR
   TARGET_ARCH MATCHES "mips64el*")
  add_definitions(-DPNG_MIPS_MSA_OPT=0)
endif()


# Set PNG_LIB_NAME.
set(PNG_LIB_NAME png${PNGLIB_MAJOR}${PNGLIB_MINOR})

# Distinguish between debug and release builds.
set(CMAKE_DEBUG_POSTFIX "d")

include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories("${CMAKE_CURRENT_LIST_DIR}/../zlib-1.2.13")
include_directories("${CMAKE_CURRENT_BINARY_DIR}/build_zlib")

# List the source code files.
set(libpng_public_hdrs
    png.h
    pngconf.h
    pnglibconf.h
)

set(libpng_private_hdrs
    pngpriv.h
    pngdebug.h
    pnginfo.h
    pngstruct.h
)

set(libpng_sources
    ${libpng_public_hdrs}
    ${libpng_private_hdrs}
    png.c
    pngerror.c
    pngget.c
    pngmem.c
    pngpread.c
    pngread.c
    pngrio.c
    pngrtran.c
    pngrutil.c
    pngset.c
    pngtrans.c
    pngwio.c
    pngwrite.c
    pngwtran.c
    pngwutil.c
)

set(pngtest_sources
    pngtest.c
)

set(pngvalid_sources
    contrib/libtests/pngvalid.c
)

set(pngstest_sources
    contrib/libtests/pngstest.c
)

set(pngunknown_sources
    contrib/libtests/pngunknown.c
)

set(pngimage_sources
    contrib/libtests/pngimage.c
)

set(pngfix_sources
    contrib/tools/pngfix.c
)

set(png_fix_itxt_sources
    contrib/tools/png-fix-itxt.c
)

if(MSVC)
  add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif()

if(PNG_DEBUG)
  add_definitions(-DPNG_DEBUG)
endif()

# Now build our target.
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ZLIB_INCLUDE_DIRS})

# does not work without changing name
set(PNG_LIB_NAME_STATIC png_static)
add_library(png_static STATIC ${libpng_sources})

# MSVC doesn't use a different file extension for shared vs. static
# libs. We are able to change OUTPUT_NAME to remove the _static
# for all other platforms.
if(NOT MSVC)
    set_target_properties(png_static PROPERTIES
                          OUTPUT_NAME "${PNG_LIB_NAME}"
                          CLEAN_DIRECT_OUTPUT 1)
else()
    set_target_properties(png_static PROPERTIES
                          OUTPUT_NAME "${PNG_LIB_NAME}_static"
                          CLEAN_DIRECT_OUTPUT 1)
endif()

list(APPEND PNG_LIB_TARGETS png_static)

if(MSVC)
    # MSVC does not append 'lib'. Do it here, to have consistent name.
    set_target_properties(png_static PROPERTIES PREFIX "lib")
	target_link_libraries(png_static PUBLIC zlibstatic)
else()
	# m is maths lib on Linux
	target_link_libraries(png_static PUBLIC zlibstatic m)
endif()


# Test the library works.
add_executable(png_test pngtest.c)
target_link_libraries(png_test PUBLIC png_static)
