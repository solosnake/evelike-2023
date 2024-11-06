# the name of the target operating system
set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_CROSSCOMPILING True)
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_C_COMPILER     "i686-w64-mingw32-gcc")
SET(CMAKE_CXX_COMPILER   "i686-w64-mingw32-g++")
SET(CMAKE_RC_COMPILER    "i686-w64-mingw32-windres")
SET(CMAKE_RANLIB         "i686-w64-mingw32-ranlib")

# Configure the behaviour of the find commands
SET(CMAKE_FIND_ROOT_PATH
    "/usr/bin/i686-w64-mingw32-gcc"
    "/usr/bin/i686-w64-mingw32-g++"
    "/usr/bin/i686-w64-mingw32-windres"
    "/usr/bin/i686-w64-mingw32-ranlib"
    "/usr/bin")

SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# Try to force static linking or else .exe depends on e.g. libgcc_s_seh-1.dll
set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static")
