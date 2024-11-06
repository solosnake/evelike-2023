cmake_minimum_required(VERSION 3.24)
set(CMAKE_DISABLE_SOURCE_CHANGES  ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

project(Solosnake VERSION 1.0.0 LANGUAGES CXX)
enable_testing()
include(GNUInstallDirs)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/libpng-1.6.39")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/SDL2-2.26.3")
add_subdirectory("${CMAKE_CURRENT_LIST_DIR}/external/glew-2.1.0/build/cmake")

# Default to Release build
if( NOT CMAKE_BUILD_TYPE )
   message(STATUS "${PROJECT_NAME}: Setting build type to Release")
   set( CMAKE_BUILD_TYPE Release )
endif()
message(STATUS "${PROJECT_NAME}: Build type is ${CMAKE_BUILD_TYPE}")
message(STATUS "${PROJECT_NAME}: Version is ${CMAKE_PROJECT_VERSION}")

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Emit `compile_commands.json` (useful for lint etc)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Where is the code expected to be found (headers and sources)
set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src/solosnake")
set(INCLUDE_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# Globs up all the named hpp, inline and cpp files. Remove main.cpp
file(GLOB src_hpps ${SRC_DIR}/*hpp)
file(GLOB src_inls ${SRC_DIR}/*inl)
file(GLOB src_cpps ${SRC_DIR}/*cpp)
list(REMOVE_ITEM "${src_cpps}" ${SRC_DIR}/main.cpp)
file(GLOB nlohmann_hpps  ${SRC_DIR}/nlohmann/*hpp)
file(GLOB universe_hpps  ${SRC_DIR}/universe/*hpp)
file(GLOB universe_inls  ${SRC_DIR}/universe/*inl)
file(GLOB universe_cpps  ${SRC_DIR}/universe/*cpp)

# Make source code list.
set(all_headers ${src_hpps} ${nlohmann_hpps} ${universe_hpps} ${src_inls})
set(all_cpp ${src_cpps} ${universe_cpps})
set(all_code ${all_headers} ${all_cpp} )

# Dependencies
set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(GLEW REQUIRED)
find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)

# Make EveLike library target.
add_library(EveLikeLib STATIC ${all_code})
target_compile_options(EveLikeLib
    PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic>
)
set_target_properties(EveLikeLib
    PROPERTIES DEBUG_POSTFIX "d"
    VERSION ${CMAKE_PROJECT_VERSION}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<0:>
)
set_target_properties(EveLikeLib
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<0:>
)
target_include_directories(EveLikeLib PUBLIC ${INCLUDE_DIR})
target_include_directories(EveLikeLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/external/libpng-1.6.39")
target_include_directories(EveLikeLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/external/SDL2-2.26.3/include")
target_include_directories(EveLikeLib PUBLIC "${CMAKE_CURRENT_LIST_DIR}/external/glew-2.1.0/include")

target_link_libraries(EveLikeLib      PUBLIC OpenGL::GL)
target_link_libraries(EveLikeLib      PUBLIC glew_s)
target_link_libraries(EveLikeLib      PUBLIC png_static)
target_link_libraries(EveLikeLib      PUBLIC SDL2-static)

# Make EveLike executable.
add_executable(EveLike ${SRC_DIR}/main.cpp)
set_target_properties(EveLike
    PROPERTIES DEBUG_POSTFIX "d"
    VERSION ${CMAKE_PROJECT_VERSION}
	RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<0:>
)
set_target_properties(EveLike
	PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<0:>
)
target_link_libraries(EveLike PUBLIC EveLikeLib)

# Make the target tests: each cpp in `tests` makes a test.
file(GLOB tests ${SRC_DIR}/tests/*.cpp)
foreach(test_filepath ${tests})
    string(REPLACE ".cpp" "" test_file ${test_filepath})
    get_filename_component(test_name ${test_file} NAME)
    message(STATUS "${PROJECT_NAME}: Building ${target} test '${test_name}'")
    add_executable(${test_name} ${test_filepath} )
    set_target_properties(${test_name}
		PROPERTIES DEBUG_POSTFIX "_d")
	set_target_properties(${test_name}
		PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/$<0:>)
    target_include_directories(${test_name} PRIVATE ${SRC_DIR})
    target_link_libraries(${test_name} EveLikeLib ${extra_deps})
    add_test(NAME ${test_name}
             COMMAND $<TARGET_FILE:${test_name}>
             WORKING_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endforeach()
