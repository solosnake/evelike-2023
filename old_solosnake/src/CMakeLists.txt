###############################################################################
# Main solosnake lib building
###############################################################################
cmake_minimum_required(VERSION 3.1)

project (solosnake-lib)
set (CMAKE_CXX_STANDARD 17)

if(NOT DEFINED SOLOSNAKE_WORDSIZE)
  set(SOLOSNAKE_WORDSIZE "64")
endif()

if(SOLOSNAKE_WORDSIZE STREQUAL "32")
    message("solosnake-lib 32 bit build")
elseif(SOLOSNAKE_WORDSIZE STREQUAL "64")
    message("solosnake-lib 64 bit build")
endif()

include_directories ("./external")
include_directories ("../include")
include_directories ("../../external/${ALUT_DIR}/include")
include_directories ("../../external/${FREETYPE_DIR}/include")
include_directories ("../../external/${GLEW_DIR}/include")
include_directories ("../../external/${LUA_DIR}/src")
include_directories ("../../external/${TINYXML_DIR}/tinyxml")
include_directories ("../../external/${UTF8_DIR}/source")

option(USE_GLEW "Use GLEW in deferred renderer instead of loading OpenGL calls from library."
       OFF)

if (UNIX)
   set(USE_GLEW OFF)
endif()

if (USE_GLEW)
    add_definitions(-DSOLOSNAKE_DEFERRED_RENDERER_USE_GLEW -DGLEW_STATIC)
    set( glew_files_list "${CMAKE_SOURCE_DIR}/external/${GLEW_DIR}/src/glew.c" )
    message( "Using GLEW in solosnake lib." )
endif(USE_GLEW)

# Add paths to x32 or x64 libs.
link_directories( ${LINK_DIRECTORIES} "${CMAKE_SOURCE_DIR}/external/${OPENAL_DIR}/openal/libs/Win${SOLOSNAKE_WORDSIZE}" )

if( WIN32 )
    # Directly inlude local OpenAL for MSVC.
    include_directories ("../../external/${OPENAL_DIR}/openal/include")
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

    # CMake will create the Lint options file for this project and it will contain the
    # contents of the solosnake.lnt.in file.
    configure_file( "../include/solosnake/solosnake.lnt.in" "../../../solosnake.vcxproj.lnt" )

    # Release - Add whole program optimisation & link-time code generation.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL" )

    # Release - Add intrinsics
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /Oi" )

    # Release - 'Fibre Safe' optimisations.
    set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GT" )

    # Release - Add linker optimisations.
    set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /OPT:REF /OPT:ICF /LTCG" )

    # Enable Multiprocessor compilation.
    set( CMAKE_CXX_FLAGS_RELEASE        "${CMAKE_CXX_FLAGS_RELEASE}        /MP" )
    set( CMAKE_CXX_FLAGS_DEBUG          "${CMAKE_CXX_FLAGS_DEBUG}          /MP" )
    set( CMAKE_CXX_FLAGS_MINSIZEREL     "${CMAKE_CXX_FLAGS_MINSIZEREL}     /MP" )
    set( CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /MP" )

    # Additional checks. Debug only as these have run-time costs.
    if(MSVC AND MSVC_VERSION GREATER 1600)
        set( CMAKE_CXX_FLAGS_DEBUG      "${CMAKE_CXX_FLAGS_DEBUG}          /sdl" )
    endif()

    # Add machine type
    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /MACHINE:X86" )
    else ()
        set (CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /MACHINE:X64" )
    endif ()

    # 64-bit automatically has this enabled, so this will generate a warning.
    if(SOLOSNAKE_WORDSIZE STREQUAL "32")
        add_definitions(/arch:SSE2)
    endif()

    add_definitions(-D_WIN32_WINDOWS=0x0501 -D_WIN32)
    add_definitions(-DVC_EXTRALEAN)
    add_definitions(-DWIN32_LEAN_AND_MEAN)
    add_definitions(-DWIN32_EXTRA_LEAN)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS=0)
    add_definitions(-D_WINSOCK_DEPRECATED_NO_WARNINGS)
    add_definitions(-D_VARIADIC_MAX=10)

    add_definitions(-DSS_OS_WINDOWS)

    set ( os_files_list
          ../include/solosnake/win32/window_opengl.hpp
          win32/window_opengl.cpp
          ../include/solosnake/win32/includewindows.hpp
          ../include/solosnake/win32/exception_code_description.hpp
          win32/exception_code_description.cpp
          win32/show_console.cpp
          win32/waitcursor
         )

    source_group( win32 FILES ${os_files_list} )

endif( MSVC )

if( UNIX )
    #list( APPEND CMAKE_CXX_FLAGS "-std=c++1y ${CMAKE_CXX_FLAGS} ")

    if((CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX) AND NOT CMAKE_COMPILER_IS_MINGW)
        add_definitions(-fnothrow-opt)
        add_definitions(-funsafe-loop-optimizations)
        # Silence warnings mainly from Boost
        add_definitions(-Wno-unused-local-typedefs)
    elseif("${CMAKE_C_COMPILER}" MATCHES "clang$" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "Clang")

    endif()

    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
    else()
      add_definitions(-O3)
    endif()

    add_definitions(-mfpmath=sse -msse -msse2 -msse3)
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
    add_definitions(-Wdisabled-optimization)
    #add_definitions(-Wunsafe-loop-optimizations)
    add_definitions(-Wmissing-include-dirs)
    add_definitions(-pedantic)
    add_definitions(-pedantic-errors)

    add_definitions(-DSS_OS_LINUX)

    # Commented out as it causes a lot of warnings in Lua and Boost. Useful to use to check for
    # potential problems in solosnake libs every now and again however.
    #add_definitions(-Wstrict-overflow)

    # OpenAL
    find_package(OpenAL REQUIRED)
    include_directories(${OPENAL_INCLUDE_DIR})
    message(${OPENAL_INCLUDE_DIR})

    # OpenGL
    find_package(OpenGL REQUIRED)
    include_directories(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_INCLUDE_DIR})
    message(${OPENGL_LIBRARIES})

    set ( os_files_list
          ../include/solosnake/x11/x11.hpp
          ../include/solosnake/x11/window_opengl.hpp
          ./x11/window_opengl.cpp
          ./x11/waitcursor.cpp
          ./x11/x11.cpp
          ./x11/show_console.cpp )

    # Compile the .c files as .cpp
    set_source_files_properties( external/alut.c PROPERTIES LANGUAGE CXX )

    source_group( x11 FILES ${os_files_list} )
endif( UNIX )

# Compile the .c files as .cpp
if (USE_GLEW)
    set_source_files_properties( ${glew_files_list} PROPERTIES LANGUAGE CXX )
    source_group( glew FILES ${glew_files_list} )
endif (USE_GLEW)

#add_definitions(-DSS_DEBUGGING_RENDERER=1)

add_library(
    solosnake
    STATIC
    ${os_files_list}
    ${glew_files_list}
    ../include/solosnake/angles.hpp
    angles.cpp
    ../include/solosnake/background_widget.hpp
    background_widget.cpp
    ../include/solosnake/background_widgetfactory.hpp
    background_widgetfactory.cpp
    ../include/solosnake/barytriangle.hpp
    barytriangle.cpp
    ../include/solosnake/bestfit.hpp
    bestfit.cpp
    ../include/solosnake/bgr.hpp
    ../include/solosnake/bgr.inl
    bgr.cpp
    ../include/solosnake/bgra.hpp
    ../include/solosnake/bgra.inl
    bgra.cpp
    ../include/solosnake/buttonpressoutcome.hpp
    ../include/solosnake/buttonreleaseoutcome.hpp
    ../include/solosnake/byte.hpp
    ../include/solosnake/bytebuffer.hpp
    ../include/solosnake/bytebuffer.inl
    ../include/solosnake/box.hpp
    box.cpp
    bytebuffer.cpp
    ../include/solosnake/cameraball.hpp
    cameraball.cpp
    ../include/solosnake/case_insensitive_compare.hpp
    case_insensitive_compare.cpp
    ../include/solosnake/checkbutton_widget.hpp
    checkbutton_widget.cpp
    ../include/solosnake/checkbutton_widgetfactory.hpp
    checkbutton_widgetfactory.cpp
    ../include/solosnake/circle.hpp
    ../include/solosnake/clamp.hpp
    ../include/solosnake/colour.hpp
    # colour.cpp
    ../include/solosnake/colourtriangle.hpp
    colourtriangle.cpp
    ../include/solosnake/commandpattern.hpp
    ../include/solosnake/commandpattern.inl
    #../include/solosnake/connection.hpp
    #../include/solosnake/connection.inl
    #connection.cpp
    ../include/solosnake/cursorbuttons_state.hpp
    ../include/solosnake/cursorstate.hpp
    ../include/solosnake/datadirectory.hpp
    datadirectory.cpp
    ../include/solosnake/deferred_renderer_to_lua.hpp
    deferred_renderer_to_lua.cpp
    ../include/solosnake/deferred_renderer_fwd.hpp
    ../include/solosnake/deferred_renderer.hpp
    deferred_renderer.cpp
    ../include/solosnake/deferred_renderer_meshinstancedata.hpp
    ../include/solosnake/deferred_renderer_types.hpp
    deferred_renderer_types.cpp
    deferred_renderer_extensions.inl
    deferred_renderer_shaders.inl
    ../include/solosnake/directoryfiles.hpp
    directoryfiles.cpp
    ../include/solosnake/dimension.hpp
    ../include/solosnake/draginfo.hpp
    draginfo.cpp
    ../include/solosnake/dragpackage.hpp
    dragpackage.cpp
    ../include/solosnake/filepath.hpp
    ../include/solosnake/font.hpp
    font.cpp
    ../include/solosnake/fontcache.hpp
    fontcache.cpp
    ../include/solosnake/fontfamily.hpp
    fontfamily.cpp
    ../include/solosnake/fontglyph.hpp
    fontglyph.cpp
    ../include/solosnake/fontletters.hpp
    fontletters.cpp
    ../include/solosnake/fontprinter.hpp
    fontprinter.cpp
    ../include/solosnake/foreachtriangle.hpp
    foreachtriangle.cpp
    ../include/solosnake/fxtriangles.hpp
    fxtriangles.cpp
    ../include/solosnake/get_opengl_version.hpp
    get_opengl_version.cpp
    ../include/solosnake/gl_err_to_string.hpp
    ../include/solosnake/good_rand.hpp
    good_rand.cpp
    gl_err_to_string.cpp
    ../include/solosnake/glutwindow.hpp
    glutwindow.cpp
    ../include/solosnake/gui.hpp
    gui.cpp
    ../include/solosnake/guicommandexecutor.hpp
    guicommandexecutor.cpp
    ../include/solosnake/guiloader.hpp
    guiloader.cpp
    ../include/solosnake/guishape.hpp
    guishape.cpp
    ../include/solosnake/guiscreenloop.hpp
    guiscreenloop.cpp
    ../include/solosnake/hexspacing.hpp
    hexspacing.cpp
    ../include/solosnake/hexbutton_widgetfactory.hpp
    hexbutton_widgetfactory.cpp
    ../include/solosnake/html.hpp
    html.cpp
    ../include/solosnake/iattributereader.hpp
    iattributereader.cpp
    ../include/solosnake/ibackbuffers.hpp
    ibackbuffers.cpp
    ../include/solosnake/icosahedron.hpp
    icosahedron.cpp
    ../include/solosnake/ifilefinder.hpp
    ifilefinder.cpp
    ../include/solosnake/igameloop.hpp
    igameloop.cpp
    ../include/solosnake/iimg_cache.hpp
    iimg_cache.cpp
    ../include/solosnake/iinput_events.hpp
    iinput_events.cpp
    ../include/solosnake/ilanguagetext.hpp
    ilanguagetext.cpp
    ../include/solosnake/ilua_widget.hpp
    ilua_widget.cpp
    ../include/solosnake/ilua_widgetfactory.hpp
    ilua_widgetfactory.cpp
    ../include/solosnake/iluawidget_make_function.hpp
    iluawidget_make_function.cpp
    ../include/solosnake/image.hpp
    image.cpp
    ../include/solosnake/imesh_cache.hpp
    imesh_cache.cpp
    ../include/solosnake/imesh_src.hpp
    imesh_src.cpp
    ../include/solosnake/img.hpp
    img.cpp
    ../include/solosnake/img_cache.hpp
    img_cache.cpp
    ../include/solosnake/indexed_triangles.hpp
    indexed_triangles.cpp
    ../include/solosnake/inline.hpp
    ../include/solosnake/input_event.hpp
    ../include/solosnake/inputs.hpp
    ../include/solosnake/inputs.inl
    inputs.cpp
    ../include/solosnake/intersection2d.hpp
    ../include/solosnake/intrinsics.hpp
    ../include/solosnake/ioswindow.hpp
    ioswindow.cpp
    ../include/solosnake/irand.hpp
    irand.cpp
    ../include/solosnake/iscreen_factory.hpp
    iscreen_factory.cpp
    ../include/solosnake/iscreen.hpp
    iscreen.cpp
    ../include/solosnake/iscreenstate.hpp
    iscreenstate.cpp
    ../include/solosnake/ishape.hpp
    ishape.cpp
    ../include/solosnake/iskybox_cache.hpp
    iskybox_cache.cpp
    ../include/solosnake/iskybox_src.hpp
    iskybox_src.cpp
    ../include/solosnake/itextureloader.hpp
    itextureloader.cpp
    ../include/solosnake/iwidget.hpp
    iwidget.cpp
    ../include/solosnake/iwidgetcollection.hpp
    iwidgetcollection.cpp
    ../include/solosnake/iwidgetname.hpp
    iwidgetname.cpp
    ../include/solosnake/iwidgetrenderer.hpp
    ../include/solosnake/iwidgetrenderer.inl
    iwidgetrenderer.cpp
    ../include/solosnake/iwindow.hpp
    iwindow.cpp
    ../include/solosnake/iworldobject.hpp
    iworldobject.cpp
    ../include/solosnake/ixmlelementreader.hpp
    ixmlelementreader.cpp
    ../include/solosnake/ixml_widgetfactory.hpp
    ixml_widgetfactory.cpp
    ../include/solosnake/languagetexts.hpp
    languagetexts.cpp
    ../include/solosnake/layout.hpp
    layout.cpp
    ../include/solosnake/linearregression3d.hpp
    linearregression3d.cpp
    ../include/solosnake/line2d.hpp
    ../include/solosnake/line_circle_intersection.hpp
    line_circle_intersection.cpp
    ../include/solosnake/linesegment.hpp
    linesegment.cpp
    ../include/solosnake/list_widget.hpp
    list_widget.cpp
    ../include/solosnake/list_widgetfactory.hpp
    list_widgetfactory.cpp
    ../include/solosnake/load_meshes.hpp
    load_meshes.cpp
    ../include/solosnake/load_models.hpp
    load_models.cpp
    ../include/solosnake/load_skybox.hpp
    load_skybox.cpp
    #../include/solosnake/lockstepserver.hpp
    #lockstepserver.cpp
    ../include/solosnake/logging.hpp
    logging.cpp
    ../include/solosnake/log_type.hpp
    log_type.cpp
    ../include/solosnake/loopresult.hpp
    ../include/solosnake/luaguicommandexecutor.hpp
    luaguicommandexecutor.cpp
    ../include/solosnake/luasinglewidgetfactory.hpp
    ../include/solosnake/make_font.hpp
    make_font.cpp
    ../include/solosnake/make_iimg.hpp
    make_iimg.cpp
    ../include/solosnake/make_normalmap.hpp
    make_normalmap.cpp
    ../include/solosnake/make_noise_texture_3d.hpp
    make_noise_texture_3d.cpp
    ../include/solosnake/manifest.hpp
    manifest.cpp
    ../include/solosnake/manifest_filepath.hpp
    manifest_filepath.cpp
    ../include/solosnake/matrix2d.hpp
    # matrix2d.cpp
    ../include/solosnake/matrix3d.hpp
    matrix3d.cpp
    ../include/solosnake/mesh_cache.hpp
    mesh_cache.cpp
    ../include/solosnake/message.hpp
    message.cpp
    ../include/solosnake/modeldescription.hpp
    modeldescription.cpp
    ../include/solosnake/modelnode.hpp
    modelnode.cpp
    ../include/solosnake/modelnodedescription.hpp
    modelnodedescription.cpp
    ../include/solosnake/modelscene.hpp
    modelscene.cpp
    ../include/solosnake/nextscreen.hpp
    nextscreen.cpp
    ../include/solosnake/noexcept.hpp
    ../include/solosnake/normaldistribution.hpp
    normaldistribution.cpp
    ../include/solosnake/octahedron.hpp
    octahedron.cpp
    ../include/solosnake/observer.hpp
    ../include/solosnake/opengl.hpp
    ../include/solosnake/opengl_debug_output.hpp
    opengl_debug_output.cpp
    ../include/solosnake/perlin_noise_skybox.hpp
    perlin_noise_skybox.cpp
    ../include/solosnake/persistance.hpp
    persistance.cpp
    ../include/solosnake/picturebutton_widget.hpp
    picturebutton_widget.cpp
    ../include/solosnake/picturebutton_widgetfactory.hpp
    picturebutton_widgetfactory.cpp
    ../include/solosnake/point.hpp
    ../include/solosnake/position.hpp
    # point.cpp
    ../include/solosnake/power_of_2.hpp
    ../include/solosnake/polygonshape.hpp
    polygonshape.cpp
    power_of_2.cpp
    ../include/solosnake/quad.hpp
    ../include/solosnake/rect.hpp
    # rect.cpp
    ../include/solosnake/rectangle_hittest.hpp
    ../include/solosnake/renderer_cache.hpp
    renderer_cache.cpp
    ../include/solosnake/rendering_system_fwd.hpp
    ../include/solosnake/rendering_system.hpp
    rendering_system.cpp
    ../include/solosnake/run_gameloop.hpp
    run_gameloop.cpp
    ../include/solosnake/save_models.hpp
    save_models.cpp
    ../include/solosnake/screenloop.hpp
    screenloop.cpp
    ../include/solosnake/screenxy.hpp
    screenxy.cpp
    ../include/solosnake/set_vsynch.hpp
    set_vsynch.cpp
    ../include/solosnake/show_meshes.hpp
    show_meshes.cpp
    ../include/solosnake/show_screens.hpp
    show_screens.cpp
    ../include/solosnake/show_console.hpp
    ../include/solosnake/singlexml_widgetfactory.hpp
    singlexml_widgetfactory.cpp
    ../include/solosnake/skybox_cache.hpp
    skybox_cache.cpp
    ../include/solosnake/sound_renderer_fwd.hpp
    ../include/solosnake/sound_renderer.hpp
    sound_renderer.cpp
    ../include/solosnake/sound_renderer_to_lua.hpp
    sound_renderer_to_lua.cpp
    ../include/solosnake/sphere.hpp
    sphere.cpp
    ../include/solosnake/sphere_geometry.hpp
    sphere_geometry.cpp
    ../include/solosnake/styledwidgetrenderer.hpp
    styledwidgetrenderer.cpp
    ../include/solosnake/textbutton_widget.hpp
    textbutton_widget.cpp
    ../include/solosnake/textbutton_widgetfactory.hpp
    textbutton_widgetfactory.cpp
    ../include/solosnake/text_widget.hpp
    text_widget.cpp
    ../include/solosnake/text_widgetfactory.hpp
    text_widgetfactory.cpp
    ../include/solosnake/textureanimationtimeline.hpp
    textureanimationtimeline.cpp
    ../include/solosnake/tsv.hpp
    tsv.cpp
    ../include/solosnake/tsvattributereader.hpp
    tsvattributereader.cpp
    ../include/solosnake/throw.hpp
    ../include/solosnake/timer.hpp
    timer.cpp
    ../include/solosnake/unaliased.hpp
    ../include/solosnake/unreachable.hpp
    ../include/solosnake/utf8text.hpp
    utf8text.cpp
    ../include/solosnake/utf8_to_unicode.hpp
    utf8_to_unicode.cpp
    ../include/solosnake/unicode_to_utf8.hpp
    unicode_to_utf8.cpp
    ../include/solosnake/vkeys.hpp
    ../include/solosnake/waitcursor.hpp
    ../include/solosnake/window.hpp
    window.cpp
    ../include/solosnake/windowchange.hpp
    ../include/solosnake/windowinputs.hpp
    ../include/solosnake/widgetparams.hpp
    ../include/solosnake/widgetstate.hpp
    ../include/solosnake/widgetvisualstates.hpp
    ../include/solosnake/widgetrenderingstyles.hpp
    widgetrenderingstyles.cpp
    ../include/solosnake/widgetlook.hpp
    widgetlook.cpp
    window_opengl.cpp
    ../include/solosnake/xmlwidgetsmaker.hpp
    xmlwidgetsmaker.cpp
    ../include/solosnake/xmlattributereader.hpp
    xmlattributereader.cpp
    external/alut.c
    external/lua.cpp
    external/lua2.cpp
    external/lua3.cpp
    external/lua4.cpp
    external/openal.cpp
    ../include/solosnake/external/xml.hpp
    external/xml.cpp
    ../include/solosnake/external/lua.hpp
    ../include/solosnake/external/lua/lunar.hpp
    external/lua/lunar.cpp
    )

if( UNIX )
    if((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "DEBUG"))
      add_definitions(-O0)
      message("solosnake Debug build")
    else()
      add_definitions(-O3)
      message("solosnake Optimised (-O3) build")
    endif()

    if (SOLOSNAKE_WORDSIZE STREQUAL "32")
      set_target_properties(solosnake PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
    endif ()

    target_link_libraries( solosnake "pthread" )
    target_link_libraries( solosnake "GL" )
    target_link_libraries( solosnake "X11" )
    target_link_libraries( solosnake "dl" )
    target_link_libraries( solosnake "rt" )
    #target_link_libraries( solosnake "libboost_program_options-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    #target_link_libraries( solosnake "libboost_filesystem-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    #target_link_libraries( solosnake "libboost_thread-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a"  )
    #target_link_libraries( solosnake "libboost_system-${GCC_VERSION}-mt-${BOOST_LIBNAME}.a" )
    target_link_libraries( solosnake "openal" )
endif( UNIX )

#target_link_libraries( solosnake "freetype${SOLOSNAKE_WORDSIZE}-lib" )
