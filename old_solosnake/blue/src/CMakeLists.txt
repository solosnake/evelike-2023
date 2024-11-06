cmake_minimum_required(VERSION 3.1)

project(blue)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED SOLOSNAKE_WORDSIZE)
  set(SOLOSNAKE_WORDSIZE "64")
endif()

if(SOLOSNAKE_WORDSIZE STREQUAL "32")
    message("blue 32 bit build")
elseif(SOLOSNAKE_WORDSIZE STREQUAL "64")
    message("blue 64 bit build")
endif()

# The version number.
set (blue_version_major 0)
set (blue_version_minor 3)
set (blue_version_patch 8)
set (bluename "blue" )
set (companyname "solosnakesoftware")

include_directories("${CMAKE_CURRENT_BINARY_DIR}")
include_directories ("../../../solosnake/include")
include_directories ("../../../solosnake/blue/include")
include_directories ("../../../external/${ALUT_DIR}/include")
#include_directories ("../../../external/${BOOST_DIR}")
#include_directories ("../../../external/${FREETYPE_DIR}/include")
include_directories ("../../../external/${GLEW_DIR}/include")
include_directories ("../../../external/${LUA_DIR}/src")
include_directories ("../../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../../external/${UTF8_DIR}/source")

# Paths which depend on word size (32 / 64 bit versions).
set (bluefullname "${bluename}.${blue_version_major}.${blue_version_minor}.${blue_version_patch}.x${SOLOSNAKE_WORDSIZE}" )
set( dllsdir "../dlls/win${SOLOSNAKE_WORDSIZE}/" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}" )
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win${SOLOSNAKE_WORDSIZE}" )

# CMake will inject Major.Minor into the code.
configure_file( "../include/solosnake/blue/blue_version.hpp.in" "blue_version_cmake_generated.hpp" )

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

    # CMake will create the Lint options file for this project and it will contain the
    # contents of the blue.lnt.in file.
    configure_file( "../include/solosnake/blue/blue.lnt.in" "../../../../${bluefullname}.vcxproj.lnt" )

    set( CMAKE_EXE_LINKER_FLAGS "/VERSION:${blue_version_major}.${blue_version_minor}" )

    # Release - Add whole program optimisation & link-time code generation.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )
    set( CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /GL" )

    # Enable Multiprocessor compilation.
    set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP" )
    set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP" )
    
    # Additional checks. Debug only as these have run-time costs.
    if(MSVC AND MSVC_VERSION GREATER 1600)
        set( CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG}          /sdl" )
    endif()
    
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
    add_definitions(-DVC_EXTRALEAN)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DWIN32_EXTRA_LEAN)

    add_definitions(-DSS_OS_WINDOWS)

endif( MSVC )

if( WIN32 )
    include_directories ("../../../external/${OPENAL_DIR}/openal/include")
    # Generate windows .rc file
    configure_file( "../include/solosnake/blue/blue_win32.rc.in" "blue_win32.rc" )
    # Generate old MFC afxres.h
    configure_file( "../include/solosnake/blue/afxres.h.in" "afxres.h" )

    set(
        os_specific_files
        ./win32/blue_winmain.cpp
        ./win32/blue_win32_link.cpp
        ./win32/blue_win32_application_paths.cpp
        ../include/solosnake/blue/blue_win32.rc.in
        blue_win32.rc
        afxres.h
        )
endif( WIN32 )

if( UNIX )
    list( APPEND CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS} ")

    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations) 
        add_definitions(-Wunsafe-loop-optimizations)
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)    
        # Silence warnings only in Boost:
        add_definitions(-Wno-unsafe-loop-optimizations)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")
    
    endif()

    # Optimised / debug builds:
    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
      message("${bluefullname} Debug build")
    else()
      add_definitions(-O3)
      message("${bluefullname} Optimised (-O3) build")
    endif()

    add_definitions(-mfpmath=sse -msse -msse2 -msse3)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    #add_definitions(-DBOOST_LIBRARYDIR="${CMAKE_SOURCE_DIR}/external/${BOOST_DIR}/lib${SOLOSNAKE_WORDSIZE}")
    add_definitions(-Wall)
    add_definitions(-Wextra)
    add_definitions(-Winit-self)
    add_definitions(-Woverloaded-virtual)
    add_definitions(-Wuninitialized)
    add_definitions(-Wmissing-declarations)
    add_definitions(-Winit-self)
    #add_definitions(-Wold-style-cast)
    #add_definitions(-Wpadded)
    #add_definitions(-Wsign-conversion)
    #add_definitions(-Wswitch-enum)
    #add_definitions(-Wlogical-op)
    #add_definitions(-Wdouble-promotion)
    #add_definitions(-Weffc++)
    add_definitions(-Wdisabled-optimization)
    add_definitions(-Wmissing-include-dirs)
    add_definitions(-pedantic)
    add_definitions(-pedantic-errors)

    # Commented out as it causes a lot of warnings in Lua and Boost. Useful to use to check for
    # potential problems in blue every now and again however.
    #add_definitions(-Wstrict-overflow)

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

    set( os_specific_files ./x11/blue_x11_application_paths.cpp )
endif( UNIX )

# Find all *.XML files in data
file ( GLOB blue_gui_data_files         "${CMAKE_CURRENT_SOURCE_DIR}/../data/gui/*.xml" )
file ( GLOB blue_styles_data_files      "${CMAKE_CURRENT_SOURCE_DIR}/../data/styles/*.xml" )
file ( GLOB blue_fonts_data_files       "${CMAKE_CURRENT_SOURCE_DIR}/../data/fonts/*.xml" )
file ( GLOB blue_hardpoints_data_files  "${CMAKE_CURRENT_SOURCE_DIR}/../data/hardpoints/*.xml" )
file ( GLOB blue_softpoints_data_files  "${CMAKE_CURRENT_SOURCE_DIR}/../data/softpoints/*.xml" )
file ( GLOB blue_sounds_data_files      "${CMAKE_CURRENT_SOURCE_DIR}/../data/sounds/*.wav" )
file ( GLOB blue_skyboxes_data_files    "${CMAKE_CURRENT_SOURCE_DIR}/../data/skyboxes/*.xml" )
file ( GLOB blue_suns_data_files        "${CMAKE_CURRENT_SOURCE_DIR}/../data/suns/*.xml" )
file ( GLOB blue_thrusters_data_files   "${CMAKE_CURRENT_SOURCE_DIR}/../data/thrusters/*.xml" )
file ( GLOB blue_chassis_data_files     "${CMAKE_CURRENT_SOURCE_DIR}/../data/chassis/*.xml" )
file ( GLOB blue_meshes_data_files      "${CMAKE_CURRENT_SOURCE_DIR}/../data/meshes/*.lua" )
file ( GLOB blue_models_data_files      "${CMAKE_CURRENT_SOURCE_DIR}/../data/models/*.lua" )
file ( GLOB blue_scripts_data_files     "${CMAKE_CURRENT_SOURCE_DIR}/../data/scripts/*.txt" )
file ( GLOB blue_translations_files     "${CMAKE_CURRENT_SOURCE_DIR}/../data/translations/*.xml" )
file ( GLOB blue_texture_files          "${CMAKE_CURRENT_SOURCE_DIR}/../data/textures/*.bmp" )
#file ( GLOB blue_geometry_shader_files  "${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/opengl/*.geom" )
#file ( GLOB blue_vertex_shader_files    "${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/opengl/*.frag" )
#file ( GLOB blue_fragment_shader_files  "${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/opengl/*.vert" )
#file ( GLOB blue_geometry_shader_files  "${CMAKE_CURRENT_SOURCE_DIR}/../data/shaders/opengl/*.geom" )

# Group data files in project
source_group( data/gui              FILES ${blue_gui_data_files} )
source_group( data/styles           FILES ${blue_styles_data_files} )
source_group( data/fonts            FILES ${blue_fonts_data_files} )
source_group( data/hardpoints       FILES ${blue_hardpoints_data_files} )
source_group( data/softpoints       FILES ${blue_softpoints_data_files} )
source_group( data/sounds           FILES ${blue_sounds_data_files} )
source_group( data/skyboxes         FILES ${blue_skyboxes_data_files} )
source_group( data/thrusters        FILES ${blue_thrusters_data_files} )
source_group( data/chassis          FILES ${blue_chassis_data_files} )
source_group( data/meshes           FILES ${blue_meshes_data_files} )
source_group( data/models           FILES ${blue_models_data_files} )
source_group( data/scripts          FILES ${blue_scripts_data_files} )
source_group( data/suns             FILES ${blue_suns_data_files} )
source_group( data/textures         FILES ${blue_textures_data_files} )
source_group( data/translations     FILES ${blue_translations_files} )
#source_group( data/shaders         FILES ${blue_fragment_shader_files} )
#source_group( data/shaders         FILES ${blue_vertex_shader_files} )
#source_group( data/shaders         FILES ${blue_geometry_shader_files} )

add_executable(
    ${bluefullname}
    WIN32 # Ignored on all platforms but Windows: has no effect on Linux etc.
    ${os_specific_files}
    ${blue_gui_data_files}
    ${blue_styles_data_files}
    ${blue_fonts_data_files}
    ${blue_hardpoints_data_files}
    ${blue_softpoints_data_files}
    ${blue_skyboxes_data_files}
    ${blue_sounds_data_files}
    ${blue_thrusters_data_files}
    ${blue_chassis_data_files}
    ${blue_meshes_data_files}
    ${blue_models_data_files}
    ${blue_scripts_data_files}
    ${blue_textures_data_files}
    ${blue_translations_files}
    #${blue_vertex_shader_files}
    #${blue_geometry_shader_files}
    main.cpp
    ../include/solosnake/blue/blue_actionbar.hpp
    blue_actionbar.cpp
    ../include/solosnake/blue/blue_actionbar_widget.hpp
    blue_actionbar_widget.cpp
    ../include/solosnake/blue/blue_actionbar_widgetfactory.hpp
    blue_actionbar_widgetfactory.cpp
    ../include/solosnake/blue/blue_asteroid.hpp
    ../include/solosnake/blue/blue_asteroid.inl
    blue_asteroid.cpp
    ../include/solosnake/blue/blue_application_paths.hpp
    ../include/solosnake/blue/blue_attachpoint.hpp
    blue_attachpoint.cpp
    ../include/solosnake/blue/blue_blueprint.hpp
    ../include/solosnake/blue/blue_blueprint.inl
    blue_blueprint.cpp
    ../include/solosnake/blue/blue_boardcamera.hpp
    blue_boardcamera.cpp
    ../include/solosnake/blue/blue_boardstate.hpp
    ../include/solosnake/blue/blue_boardstate.inl
    blue_boardstate.cpp
    ../include/solosnake/blue/blue_boardobserver.hpp
    ../include/solosnake/blue/blue_boardstategridview.hpp
    blue_boardstategridview.cpp
    ../include/solosnake/blue/blue_cargohold.hpp
    ../include/solosnake/blue/blue_cargohold.inl
    blue_cargohold.cpp
    ../include/solosnake/blue/blue_chassis.hpp
    blue_chassis.cpp
    ../include/solosnake/blue/blue_classification.hpp
    ../include/solosnake/blue/blue_classification.inl
    blue_classification.cpp
    ../include/solosnake/blue/blue_clockwise.hpp
    ../include/solosnake/blue/blue_codeeditor.hpp
    blue_codeeditor.cpp
    ../include/solosnake/blue/blue_codeeditor_widget.hpp
    blue_codeeditor_widget.cpp
    ../include/solosnake/blue/blue_codeeditor_widgetfactory.hpp
    blue_codeeditor_widgetfactory.cpp
    ../include/solosnake/blue/blue_codeinspector_widget.hpp
    blue_codeinspector_widget.cpp
    ../include/solosnake/blue/blue_codeinspector_widgetfactory.hpp
    blue_codeinspector_widgetfactory.cpp
    ../include/solosnake/blue/blue_combatsystem.hpp
    ../include/solosnake/blue/blue_combatsystem.inl
    blue_combatsystem.cpp
    ../include/solosnake/blue/blue_compilationresult.hpp
    ../include/solosnake/blue/blue_compilationresult.inl
    blue_compilationresult.cpp
    ../include/solosnake/blue/blue_compiler.hpp
    blue_compiler.cpp
    ../include/solosnake/blue/blue_component.hpp
    ../include/solosnake/blue/blue_component.inl
    blue_component.cpp
    ../include/solosnake/blue/blue_componentcategory.hpp
    ../include/solosnake/blue/blue_create_gamecamera.hpp
    blue_create_gamecamera.cpp
    ../include/solosnake/blue/blue_datapathfinder.hpp
    ../include/solosnake/blue/blue_datapaths.hpp
    blue_datapaths.cpp
    ../include/solosnake/blue/blue_deck.hpp
    blue_deck.cpp
    ../include/solosnake/blue/blue_designedmachine.hpp
    blue_designedmachine.cpp
    ../include/solosnake/blue/blue_drawasteroid.hpp
    blue_drawasteroid.cpp
    ../include/solosnake/blue/blue_drawboard.hpp
    blue_drawboard.cpp
    ../include/solosnake/blue/blue_drawgame.hpp
    blue_drawgame.cpp
    ../include/solosnake/blue/blue_drawmachine.hpp
    ../include/solosnake/blue/blue_drawmachine.inl
    blue_drawmachine.cpp
    ../include/solosnake/blue/blue_drawsun.hpp
    blue_drawsun.cpp
    ../include/solosnake/blue/blue_emissivechannels.hpp
    blue_emissivechannels.cpp
    ../include/solosnake/blue/blue_explosiondamages.hpp
    blue_explosiondamages.cpp
    ../include/solosnake/blue/blue_fixedpoint.hpp
    ../include/solosnake/blue/blue_fxfiring.hpp
    blue_fxfiring.cpp
    ../include/solosnake/blue/blue_fxmachineexplosion.hpp
    blue_fxmachineexplosion.cpp
    ../include/solosnake/blue/blue_fxsense.hpp
    blue_fxsense.cpp
    ../include/solosnake/blue/blue_fxsound.hpp
    blue_fxsound.cpp
    ../include/solosnake/blue/blue_game.hpp
    blue_game.cpp
    ../include/solosnake/blue/blue_gameaction.hpp
    ../include/solosnake/blue/blue_gameaction.inl
    blue_gameaction.cpp
    ../include/solosnake/blue/blue_gamebackground_widget.hpp
    blue_gamebackground_widget.cpp
    ../include/solosnake/blue/blue_gamebackground_widgetfactory.hpp
    blue_gamebackground_widgetfactory.cpp
    ../include/solosnake/blue/blue_gamebackground_widgetsfactory.hpp
    blue_gamebackground_widgetsfactory.cpp
    ../include/solosnake/blue/blue_gamecamera.hpp
    ../include/solosnake/blue/blue_gamecamera.inl
    blue_gamecamera.cpp
    ../include/solosnake/blue/blue_drawgameboard.hpp
    blue_drawgameboard.cpp
    ../include/solosnake/blue/blue_gamesfx.hpp
    blue_gamesfx.cpp
    ../include/solosnake/blue/blue_gamecommands.hpp
    ../include/solosnake/blue/blue_gamehexgrid.hpp
    blue_gamehexgrid.cpp
    ../include/solosnake/blue/blue_gamelogicconstants.hpp
    ../include/solosnake/blue/blue_gameloop.hpp
    blue_gameloop.cpp
    ../include/solosnake/blue/blue_gamescreen.hpp
    blue_gamescreen.cpp
    ../include/solosnake/blue/blue_gamestartupargs.hpp
    blue_gamestartupargs.cpp
    ../include/solosnake/blue/blue_gamestate.hpp
    blue_gamestate.cpp
    ../include/solosnake/blue/blue_gameviewing_widget.hpp
    blue_gameviewing_widget.cpp
    ../include/solosnake/blue/blue_get_buildstamp.hpp
    blue_get_buildstamp.cpp
    ../include/solosnake/blue/blue_guiscreenview.hpp
    blue_guiscreenview.cpp
    ../include/solosnake/blue/blue_hardpoint.hpp
    blue_hardpoint.cpp
    ../include/solosnake/blue/blue_hardpoint_angles.hpp
    blue_hardpoint_angles.cpp
    ../include/solosnake/blue/blue_hardpointshot.hpp
    blue_hardpointshot.cpp
    ../include/solosnake/blue/blue_hardpointshooter.hpp
    blue_hardpointshooter.cpp
    ../include/solosnake/blue/blue_hardpointtype.hpp
    ../include/solosnake/blue/blue_hardpointcollective.hpp
    blue_hardpointcollective.cpp
    ../include/solosnake/blue/blue_hardware.hpp
    blue_hardware.cpp
    ../include/solosnake/blue/blue_hardwaremodifier.hpp
    blue_hardwaremodifier.cpp
    ../include/solosnake/blue/blue_hexcoord.hpp
    ../include/solosnake/blue/blue_hexcontentflags.hpp
    ../include/solosnake/blue/blue_hexfacingname.hpp
    ../include/solosnake/blue/blue_hexgrid.hpp
    blue_hexgrid.cpp
    ../include/solosnake/blue/blue_hexpathfinder.hpp
    blue_hexpathfinder.cpp
    ../include/solosnake/blue/blue_hex_steps.hpp
    blue_hex_steps.cpp
    ../include/solosnake/blue/blue_hexpathcosts.hpp
    blue_hexpathcosts.cpp
    ../include/solosnake/blue/blue_hexsenseresult.hpp
    ../include/solosnake/blue/blue_iboardview.hpp
    blue_iboardview.cpp
    ../include/solosnake/blue/blue_iboardcamera.hpp
    blue_iboardcamera.cpp
    ../include/solosnake/blue/blue_ichangeview.hpp
    blue_ichangeview.cpp
    ../include/solosnake/blue/blue_idetecthit.hpp
    blue_idetecthit.cpp
    ../include/solosnake/blue/blue_igameactions.hpp
    blue_igameactions.cpp
    ../include/solosnake/blue/blue_igameview.hpp
    blue_igameview.cpp
    ../include/solosnake/blue/blue_ipendinginstructions.hpp
    blue_ipendinginstructions.cpp
    ../include/solosnake/blue/blue_iscreenview.hpp
    blue_iscreenview.cpp
    ../include/solosnake/blue/blue_ifx.hpp
    blue_ifx.cpp
    ../include/solosnake/blue/blue_ifxrenderer.hpp
    blue_ifxrenderer.cpp
    ../include/solosnake/blue/blue_iinstructionsender.hpp
    blue_iinstructionsender.cpp
    ../include/solosnake/blue/blue_instruction.hpp
    blue_instruction.cpp
    ../include/solosnake/blue/blue_instructionbutton_widget.hpp
    blue_instructionbutton_widget.cpp
    ../include/solosnake/blue/blue_instructionbutton_widgetfactory.hpp
    blue_instructionbutton_widgetfactory.cpp
    ../include/solosnake/blue/blue_instructions.hpp
    blue_instructions.cpp
    ../include/solosnake/blue/blue_isun_src.hpp
    blue_isun_src.cpp
    ../include/solosnake/blue/blue_keymapping.hpp
    blue_keymapping.cpp
    ../include/solosnake/blue/blue_linehit.hpp
    ../include/solosnake/blue/blue_load_skybox.hpp
    blue_load_skybox.cpp
    ../include/solosnake/blue/blue_load_sun.hpp
    blue_load_sun.cpp
    ../include/solosnake/blue/blue_log_events.hpp
    blue_log_events.cpp
    ../include/solosnake/blue/blue_luagameoptions.hpp
    blue_luagameoptions.cpp
    ../include/solosnake/blue/blue_machine.hpp
    blue_machine.cpp
    ../include/solosnake/blue/blue_machineevent.hpp
    blue_machineevent.cpp
    ../include/solosnake/blue/blue_machineevents.hpp
    ../include/solosnake/blue/blue_machinegridview.hpp
    ../include/solosnake/blue/blue_machinegridview.inl
    blue_machinegridview.cpp
    ../include/solosnake/blue/blue_machineparts.hpp
    blue_machineparts.cpp
    ../include/solosnake/blue/blue_machinereadouts.hpp
    blue_machinereadouts.cpp
    blue_main.cpp
    ../include/solosnake/blue/blue_mapedit.hpp
    ../include/solosnake/blue/blue_mapedit.inl
    blue_mapedit.cpp
    ../include/solosnake/blue/blue_mapeditcommands.hpp
    blue_mapeditcommands.cpp
    ../include/solosnake/blue/blue_mapedittarget.hpp
    blue_mapedittarget.cpp
    ../include/solosnake/blue/blue_mapeditbrushtype.hpp
    ../include/solosnake/blue/blue_mapeditscreen.hpp
    blue_mapeditscreen.cpp
    ../include/solosnake/blue/blue_mapeditscreenloop.hpp
    blue_mapeditscreenloop.cpp
    ../include/solosnake/blue/blue_mapedit_widget.hpp
    blue_mapedit_widget.cpp
    ../include/solosnake/blue/blue_mapedit_widgetfactory.hpp
    blue_mapedit_widgetfactory.cpp
    ../include/solosnake/blue/blue_menuwidgetsfactory.hpp
    blue_menuwidgetsfactory.cpp
    ../include/solosnake/blue/blue_menuscreen.hpp
    blue_menuscreen.cpp
    ../include/solosnake/blue/blue_messages.hpp
    blue_messages.cpp
    ../include/solosnake/blue/blue_movementgridview.hpp
    blue_movementgridview.cpp
    ../include/solosnake/blue/blue_onsale.hpp
    ../include/solosnake/blue/blue_pendinginstructions.hpp
    blue_pendinginstructions.cpp
    ../include/solosnake/blue/blue_player.hpp
    blue_player.cpp
    ../include/solosnake/blue/blue_pickresult.hpp
    blue_pickresult.cpp
    ../include/solosnake/blue/blue_positioninfo.hpp
    ../include/solosnake/blue/blue_program_options.hpp
    blue_program_options.cpp
    ../include/solosnake/blue/blue_propulsion.hpp
    blue_propulsion.cpp
    ../include/solosnake/blue/blue_quote.hpp
    ../include/solosnake/blue/blue_refine.hpp
    blue_refine.cpp
    ../include/solosnake/blue/blue_screen.hpp
    blue_screen.cpp
    ../include/solosnake/blue/blue_screenxmlnames.hpp
    ../include/solosnake/blue/blue_screenfactory.hpp
    blue_screenfactory.cpp
    ../include/solosnake/blue/blue_selecteddeck.hpp
    blue_selecteddeck.cpp
    ../include/solosnake/blue/blue_secondaryevent.hpp
    ../include/solosnake/blue/blue_secondaryevent.inl
    ../include/solosnake/blue/blue_senseresults.hpp
    ../include/solosnake/blue/blue_senseresults.inl
    blue_senseresults.cpp
    ../include/solosnake/blue/blue_settings.hpp
    ../include/solosnake/blue/blue_settings_objects.hpp
    blue_settings_objects.cpp
    ../include/solosnake/blue/blue_skyboxbackground_widget.hpp
    blue_skyboxbackground_widget.cpp
    ../include/solosnake/blue/blue_skyboxbackground_widgetfactory.hpp
    blue_skyboxbackground_widgetfactory.cpp
    ../include/solosnake/blue/blue_softpoint.hpp
    blue_softpoint.cpp
    ../include/solosnake/blue/blue_start.hpp
    blue_start.cpp
    ../include/solosnake/blue/blue_sun.hpp
    ../include/solosnake/blue/blue_sun.inl
    blue_sun.cpp
    ../include/solosnake/blue/blue_suncache.hpp
    blue_suncache.cpp
    ../include/solosnake/blue/blue_sunstrength.hpp
    ../include/solosnake/blue/blue_suntype.hpp
    ../include/solosnake/blue/blue_traderequest.hpp
    ../include/solosnake/blue/blue_tradabletypes.hpp
    blue_tradabletypes.cpp
    ../include/solosnake/blue/blue_tradables.hpp
    ../include/solosnake/blue/blue_tradables.inl
    blue_tradables.cpp
    ../include/solosnake/blue/blue_translator.hpp
    blue_translator.cpp
    ../include/solosnake/blue/blue_thruster.hpp
    blue_thruster.cpp
    ../include/solosnake/blue/blue_thrusterattachpoint.hpp
    blue_thrusterattachpoint.cpp
    ../include/solosnake/blue/blue_tiltcamera.hpp
    ../include/solosnake/blue/blue_tiltcamera.inl
    blue_tiltcamera.cpp
    ../include/solosnake/blue/blue_turninghardpoint.hpp
    ../include/solosnake/blue/blue_turninghardpoint.inl
    blue_turninghardpoint.cpp
    ../include/solosnake/blue/blue_turningsystem.hpp
    ../include/solosnake/blue/blue_turningsystem.inl
    blue_turningsystem.cpp
    ../include/solosnake/blue/blue_user_settings.hpp
    blue_user_settings.cpp
    ../include/solosnake/blue/blue_version.hpp
    ../include/solosnake/blue/blue_widgetsfactory.hpp
    blue_widgetsfactory.cpp
    ../include/solosnake/blue/blue_xmlscreenviews.hpp
    blue_xmlscreenviews.cpp
    ../include/solosnake/blue/blue_xml_board.hpp
    blue_xml_board.cpp
    ../include/solosnake/blue/blue_xmlscreenviewsgameloop.hpp
    blue_xmlscreenviewsgameloop.cpp
    )

if( UNIX )
    target_link_libraries( ${bluefullname} ${OPENGL_LIBRARY} )
    target_link_libraries( ${bluefullname} ${OPENAL_LIBRARY} )
    #target_link_libraries( ${bluefullname} "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    #target_link_libraries( ${bluefullname} "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    #target_link_libraries( ${bluefullname} "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    #target_link_libraries( ${bluefullname} "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    #target_link_libraries( ${bluefullname} "libboost_regex-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( ${bluefullname} "pthread" )
    target_link_libraries( ${bluefullname} "X11" )
    target_link_libraries( ${bluefullname} "GL" )
    target_link_libraries( ${bluefullname} "openal" )
    target_link_libraries( ${bluefullname} "dl" )
    target_link_libraries( ${bluefullname} "rt" )
endif( UNIX )

target_link_libraries( ${bluefullname} solosnake )

# Copy data directory to build location for debugging
add_custom_command(
    TARGET ${bluefullname}
    # PRE_BUILD | PRE_LINK | POST_BUILD
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/data"
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/solosnake/blue/data" "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}"
    )

# From p.163 of Mastering Cmake...
include (InstallRequiredSystemLibraries)
install (DIRECTORY ../data/ DESTINATION bin )
install (TARGETS ${bluefullname} DESTINATION bin)

if( WIN32 )
    install(PROGRAMS "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/oalinst.exe" DESTINATION bin)
    install(DIRECTORY ${dllsdir} DESTINATION bin)
endif( WIN32 )

# Installer for blue
set (assetsdir "${CMAKE_SOURCE_DIR}/solosnake/blue/installer_assets" )
set (CPACK_PACKAGE_VERSION_MAJOR ${blue_version_major} )
set (CPACK_PACKAGE_VERSION_MINOR ${blue_version_minor} )
set (CPACK_PACKAGE_VERSION_PATCH ${blue_version_patch} )
set (CPACK_PACKAGE_NAME                     "Install-${bluename}")
set (CPACK_PACKAGE_VENDOR                   "${companyname}" )
set (CPACK_PACKAGE_DESCRIPTION_SUMMARY      "${bluefullname} summary" )
set (CPACK_PACKAGE_EXECUTABLES              "${bluefullname}" "${bluefullname}")    # executable name / shortcut name
set (CPACK_PACKAGE_INSTALL_REGISTRY_KEY     "${bluefullname}")
set (CPACK_PACKAGE_INSTALL_DIRECTORY        "${companyname}\\\\${bluefullname}")
if (WIN32 AND NOT UNIX)
    # There is a bug in NSI that does not handle full unix paths properly. Make
    # sure there is at least one set of four (4) backlasshes.
    set (CPACK_PACKAGE_DESCRIPTION_FILE     "${assetsdir}\\\\packagedescription.txt" )
    set (CPACK_RESOURCE_FILE_LICENSE        "${assetsdir}\\\\license.txt" )
    set (CPACK_RESOURCE_FILE_WELCOME        "${assetsdir}\\\\welcome.txt" )
    set (CPACK_PACKAGE_ICON                 "${assetsdir}\\\\branding.bmp")
    set (CPACK_PACKAGE_INSTALL_DIRECTORY    "solosnake\\\\${bluefullname}" )
    set (CPACK_NSIS_DISPLAY_NAME            "${bluefullname}" )
    set (CPACK_NSIS_MUI_ICON                "${assetsdir}\\\\logo.ico")
    set (CPACK_NSIS_MUI_UNIICON             "${assetsdir}\\\\uninstall.ico")
    set (CPACK_PACKAGE_ICON                 "${assetsdir}\\\\packageicon.bmp")
    set (CPACK_NSIS_INSTALLED_ICON_NAME     "bin\\\\${bluefullname}.exe")
    set (CPACK_NSIS_HELP_LINK               "http:\\\\\\\\www.solosnake.com/${bluename}/help")
    set (CPACK_NSIS_URL_INFO_ABOUT          "http:\\\\\\\\www.solosnake.com/${bluename}")
    set (CPACK_NSIS_CONTACT                 "solosnake@solosnake.com")
    set (CPACK_NSIS_MENU_LINKS              "bin\\\\blue.html" "solosnake.com")
    # Install OpenAL silently.
    list(APPEND CPACK_NSIS_EXTRA_INSTALL_COMMANDS " ExecWait '$INSTDIR\\\\bin\\\\oalinst.exe /s'")
    # Asks user if he/she wants to add Blue to path (no!)
    # set (CPACK_NSIS_MODIFY_PATH            ON)
else(WIN32 AND NOT UNIX)
    set (CPACK_PACKAGE_INSTALL_DIRECTORY    "solosnake/${bluefullname}" )
    set (CPACK_STRIP_FILES                  "bin/${bluefullname}")
    set (CPACK_SOURCE_STRIP_FILES           "")
ENDIF(WIN32 AND NOT UNIX)

include(CPack)


