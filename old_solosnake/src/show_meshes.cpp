#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include "solosnake/case_insensitive_compare.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/filepath.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/load_meshes.hpp"
#include "solosnake/radians.hpp"
#include "solosnake/timer.hpp"
#include "solosnake/unicode_to_utf8.hpp"
#include "solosnake/vkeys.hpp"
#include "solosnake/window.hpp"
#include "solosnake/show_meshes.hpp"

using namespace solosnake;
using namespace std;

class modelsrc;

//-------------------------------------------------------------------------
// Application globals:
typedef pair<string, vector<meshhandle_t>> filecontents_t;

namespace
{
    const float g_zNear                     = 0.1f;
    const float g_zFar                      = 265.0f;
    const float g_zoomRate                  = 1.2f;
    const float g_maxCameraDistance         = 244.0f;
    const float g_minCameraDistance         = 0.75f;
    const float g_separationDistance        = 200.0f;
    const int   g_teamColorCount            = 10;
    const float g_rotateRateXPerSecond      = 0.125f;
    const float g_rotateRateYPerSecond      = 0.250f;
    const float g_rotateRateZPerSecond      = 0.125f;
    const float g_rotateRateLightXPerSecond = 0.125f;
    const float g_rotateRateLightYPerSecond = 0.750f;
    const float g_movingLightArmLength      = 1.5f;

    unique_ptr<timer>               g_timer;
    shared_ptr<window>              g_window;
    shared_ptr<windowobserver>      g_windowobserver;
    shared_ptr<deferred_renderer>   g_renderer;
    vector<filecontents_t>          g_filecontents;
    matrix4x4_t                     g_mv;
    matrix4x4_t                     g_proj;
    size_t                          g_currentlyViewedMeshes = 0;
    float                           g_cameraDistance        = 5.0f;
    float                           g_separation            = 1.0f;
    int                             g_rendermode            = 0;
    bool                            g_ambientlightOn        = true;
    bool                            g_toplightOn            = true;
    bool                            g_bottomlightOn         = true;
    bool                            g_cornerlightOn         = true;
    bool                            g_movinglightOn         = true;
    // Model movement.
    bool                            g_rotateXOn             = false;
    bool                            g_rotateYOn             = false;
    bool                            g_rotateZOn             = true;
    float                           g_xAngle                = 0.0f;
    float                           g_yAngle                = 0.0f;
    float                           g_zAngle                = 0.0f;
    float                           g_xAngleDir             = 1.0f;
    float                           g_yAngleDir             = 1.0f;
    float                           g_zAngleDir             = 1.0f;

    // Light movement.
    bool                            g_rotateLightXOn        = false;
    bool                            g_rotateLightYOn        = true;
    float                           g_xAngleLight           = 0.0f;
    float                           g_yAngleLight           = 0.0f;
    float                           g_xAngleLightDir        = 1.0f;
    float                           g_yAngleLightDir        = 1.0f;
    int                             g_instanceTeamColorMode = 0;
    unsigned int                    g_colorIndex0           = 0;
    unsigned int                    g_colorIndex1           = 1;
    bool                            g_dragging              = false;
    bool                            g_showSingleInstance    = true;

    const bgra                      g_colors[] = { bgra(0,0,255), bgra(0,255,0), bgra(255,0,0),
                                                   bgra(0,255,255), bgra(255,0,255), bgra(255,255,0),
                                                   bgra(255,255,255), bgra(0,0,0) };

    //-------------------------------------------------------------------------
    // Defines:
#define VIEWWINDOW_W 1280
#define VIEWWINDOW_H 1280
#define RENDERMODERANGEBEGIN (static_cast<unsigned int>(deferred_renderer::SceneRenderMode))
#define RENDERMODERANGEEND (1 + static_cast<unsigned int>(deferred_renderer::ShowMipMapLevels))
#define RENDERMODECOUNT (RENDERMODERANGEEND - RENDERMODERANGEBEGIN)

    //-------------------------------------------------------------------------
    // Forward declarations:
    void reset_meshviewer();
    bool is_3dsfile( const filepath& );
    void load_3dsfile( const filepath& );
    void load_meshfile( const filepath& );
    void add_file( const wstring& );
    void on_window_event( const windowchange&, const window& );
    void on_dragdropped( const window&, const std::vector<std::wstring>& );
    void on_sizechanged( const window&, const dimension2d<unsigned int>& );
    void draw();
    void set_rendermode( deferred_renderer::DebugRenderMode );
    void set_next_rendermode();
    void set_previous_rendermode();
    void set_next_teamcolour();
    void set_viewed_file( size_t n );
    void set_instance_teamcolour( meshinstancedata& );
    void set_instance_emissives( meshinstancedata& );
    void show_next_contents();
    void show_previous_contents();
    void toggle_rotate_x_up();
    void toggle_rotate_x_down();
    void toggle_rotate_y_up();
    void toggle_rotate_y_down();
    void reset_rotations();
    string make_title();

    //-------------------------------------------------------------------------

    void reset_meshviewer()
    {
        g_timer.reset();
        g_window.reset();
        g_windowobserver.reset();
        g_renderer.reset();
        g_filecontents.clear();

        g_currentlyViewedMeshes = 0;
        g_cameraDistance = 5.0f;
        g_rendermode = 0;
        g_ambientlightOn = true;
        g_toplightOn = true;
        g_bottomlightOn = true;
        g_cornerlightOn = true;
        g_movinglightOn = true;
        g_rotateXOn = false;
        g_rotateYOn = false;
        g_rotateZOn = true;
        g_xAngle = 0.0f;
        g_yAngle = 0.0f;
        g_zAngle = 0.0f;
        g_xAngleDir = 1.0f;
        g_yAngleDir = 1.0f;
        g_zAngleDir = 1.0f;
        g_instanceTeamColorMode = 0;
        g_colorIndex0 = 0;
        g_colorIndex1 = 1;
        g_dragging = false;
        g_showSingleInstance = true;

        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ),
                          float( VIEWWINDOW_W ) / VIEWWINDOW_H,
                          g_zNear,
                          g_zFar,
                          g_proj );

        reset_rotations();
    }

    void add_file( const wstring& wf )
    {
        try
        {
            auto f = unicode_to_utf8( wf );
            const filepath url( f );

            if( is_3dsfile( url ) )
            {
                load_3dsfile( url );
            }
            else
            {
                load_meshfile( url );
            }

            // Reset rotation angle so meshes always start in default.
            g_xAngle = 0.0f;
            g_yAngle = 0.0f;
            g_xAngle = 0.0f;
        }
        catch( ... )
        {
            ss_err( "Unable to load file ", unicode_to_utf8( wf ) );
        }
    }

    bool is_3dsfile( const filepath& url )
    {
        // Case insensitive comparison.
        return case_insensitive_compare( url.extension().string().c_str(), ".3ds" );
    }
    
    void load_3dsfile( const filepath& )
    {
    }

    void toggle_rotate_x_up()
    {
        g_rotateXOn = !g_rotateXOn;
        g_xAngleDir = 1.0f;
    }

    void toggle_rotate_x_down()
    {
        g_rotateXOn = !g_rotateXOn;
        g_xAngleDir = -1.0f;
    }

    void toggle_rotate_y_up()
    {
        g_rotateYOn = !g_rotateYOn;
        g_yAngleDir = 1.0f;
    }

    void toggle_rotate_y_down()
    {
        g_rotateYOn = !g_rotateYOn;
        g_yAngleDir = -1.0f;
    }
    
    void toggle_rotate_z_up()
    {
        g_rotateZOn = !g_rotateZOn;
        g_zAngleDir = 1.0f;
    }

    void toggle_rotate_z_down()
    {
        g_rotateZOn = !g_rotateZOn;
        g_zAngleDir = -1.0f;
    }
    void reset_rotations()
    {
        g_rotateXOn = false;
        g_rotateYOn = false;
        g_rotateZOn = false;
        g_xAngle = 0.0f;
        g_yAngle = 0.0f;
        g_zAngle = 0.0f;
        g_xAngleDir = 1.0f;
        g_yAngleDir = 1.0f;
        g_zAngleDir = 1.0f;
    }

    string make_title()
    {
        ostringstream os;

        if( g_currentlyViewedMeshes < g_filecontents.size() )
        {
            os << g_filecontents[g_currentlyViewedMeshes].first;
        }
        else
        {
            os << "[No file loaded]";
        }

        os << " : ";
        os << ( g_ambientlightOn ? "A" : "-" );
        os << ( g_toplightOn     ? "T" : "-" );
        os << ( g_bottomlightOn  ? "B" : "-" );
        os << ( g_cornerlightOn  ? "C" : "-" );
        os << ( g_movinglightOn  ? "M" : "-" );

        os << " : R" << g_rendermode;
        os << " : T" << g_instanceTeamColorMode;

        return os.str();
    }

    void set_viewed_file( size_t n )
    {
        if( n < g_filecontents.size() )
        {
            g_currentlyViewedMeshes = n;
            g_window->set_title( make_title() );
        }
    }

    void set_instance_teamcolour( meshinstancedata& mi )
    {
        mi.set_team_hue_shift( g_instanceTeamColorMode / static_cast<float>( g_teamColorCount ) );
    }

    void set_instance_emissives( meshinstancedata& mi )
    {
        mi.set_emissive_channel_bgra( 0, g_colors[ g_colorIndex0 ] );
        mi.set_emissive_channel_bgra( 1, g_colors[ g_colorIndex1 ] );
    }
    
    void set_next_teamcolour()
    {
        g_instanceTeamColorMode = ( g_instanceTeamColorMode + 1 ) % g_teamColorCount;
    }

    void set_next_meshviewmode()
    {
        g_showSingleInstance = !g_showSingleInstance;
    }

    void set_next_color_0()
    {
        g_colorIndex0 = (g_colorIndex0 + 1u) % (sizeof(g_colors)/sizeof(g_colors[0]));
    }

    void set_next_color_1()
    {
        g_colorIndex1 = (g_colorIndex1 + 1u) % (sizeof(g_colors)/sizeof(g_colors[0]));
    }

    void show_next_contents()
    {
        if( g_filecontents.size() > 0 )
        {
            set_viewed_file( ( g_currentlyViewedMeshes + 1 ) % g_filecontents.size() );
        }
    }

    void show_previous_contents()
    {
        if( g_filecontents.size() > 0 )
        {
            set_viewed_file( ( g_currentlyViewedMeshes + g_filecontents.size() - 1 )
                             % g_filecontents.size() );
        }
    }

    void load_meshfile( const filepath& url )
    {
        const filepath texsubdir = url.parent_path().parent_path() / "textures";

        vector<string> texLocations;
        texLocations.push_back( url.parent_path().string() );
        texLocations.push_back( texsubdir.string() );

        auto texDirs = make_shared<datadirectories>( texLocations );
        vector<shared_ptr<imesh>> meshes = load_mesh_datas( url, texDirs );

        filecontents_t contents;
        contents.first = filepath_file_stem( url ) + filepath_file_extension( url );

        for( size_t i = 0; i < meshes.size(); ++i )
        {
            contents.second.push_back( g_renderer->load_mesh( meshes[i] ) );
        }

        g_filecontents.push_back( contents );

        set_viewed_file( g_filecontents.size() - 1 );
    }

    void on_window_event( const windowchange& e, const window& w )
    {
        if( e.event_type() == windowchange::DragDroppedChange )
        {
            on_dragdropped( w, *e.drag_drops() );
        }
        else if( e.event_type() == windowchange::ResizeChange )
        {
            on_sizechanged( w, *e.resized_dimensions() );
        }
    }

    void on_dragdropped( const window&, const std::vector<std::wstring>& files )
    {
        for_each( files.cbegin(),
                  files.cend(),
                  [&]( const wstring & f )
        { add_file( f ); } );
    }

    void on_sizechanged( const window&, const dimension2d<unsigned int>& newsize )
    {
        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ),
                          float( newsize.width() ) / newsize.height(),
                          g_zNear,
                          g_zFar,
                          g_proj );

        g_renderer->set_viewport( 0, 0, newsize.width(), newsize.height() );
    }

    meshinstancedata make_instance( const float offset[3] )
    {
        meshinstancedata instance;
        set_instance_teamcolour( instance );
        set_instance_emissives( instance );

        matrix4x4_t rotX;
        matrix4x4_t rotY;
        matrix4x4_t rotZ;
        matrix4x4_t moveXYZ;
        matrix4x4_t rotYX;
        matrix4x4_t rotYXZ;
        load_rotation_y_4x4( radians( g_yAngle ), rotY );
        load_rotation_x_4x4( radians( g_xAngle ), rotX );
        load_rotation_z_4x4( radians( g_zAngle ), rotZ );

        load_translation_4x4( offset, moveXYZ );
        mul_4x4( rotY, rotX, rotYX );
        mul_4x4( rotYX, rotZ, rotYXZ );

        mul_4x4( moveXYZ, rotYXZ, instance.location_4x4() );

        return instance;
    }

    void draw()
    {
        const auto secondsSinceLastDraw = g_timer->restart();

        // Model rotation
        if( g_rotateXOn )
        {
            g_xAngle += ( g_xAngleDir * secondsSinceLastDraw * g_rotateRateXPerSecond );
        }

        if( g_rotateYOn )
        {
            g_yAngle += ( g_yAngleDir * secondsSinceLastDraw * g_rotateRateYPerSecond );
        }
        
        if( g_rotateZOn )
        {
            g_zAngle += ( g_zAngleDir * secondsSinceLastDraw * g_rotateRateZPerSecond );
        }

        g_xAngle = radians::clamp( g_xAngle );
        g_yAngle = radians::clamp( g_yAngle );
        g_zAngle = radians::clamp( g_zAngle );

        // Light rotation
        if( g_rotateLightXOn )
        {
            g_xAngleLight += ( g_xAngleLightDir * secondsSinceLastDraw * g_rotateRateLightXPerSecond );
        }

        if( g_rotateLightYOn )
        {
            g_yAngleLight += ( g_yAngleLightDir * secondsSinceLastDraw * g_rotateRateLightYPerSecond );
        }

        g_xAngleLight = radians::clamp( g_xAngleLight );
        g_yAngleLight = radians::clamp( g_yAngleLight );

        g_window->set_title( make_title() );

        const float eyePos[] = { 0.0f, 0.0f, g_cameraDistance };
        const float lookingAt[] = { 0.0f, 0.0f, 0.0f };
        const float up[] = { 0.0f, 1.0f, 0.0f };
        load_look_at( lookingAt, eyePos, up, g_mv );

        deferred_renderer::directional_light topLight;
        topLight.directionX     = 0.0f;
        topLight.directionY     = -1.0f;
        topLight.directionZ     = 0.0f;
        topLight.red            = 0.5f;
        topLight.green          = 0.5f;
        topLight.blue           = 0.5f;

        deferred_renderer::directional_light bottomLight;
        bottomLight.directionX = 1.0f;
        bottomLight.directionY = 1.0f;
        bottomLight.directionZ = 1.0f;
        bottomLight.red        = 0.1f;
        bottomLight.green      = 0.1f;
        bottomLight.blue       = 0.1f;

        deferred_renderer::point_light cornerLight;
        cornerLight.worldX     = -2.0f;
        cornerLight.worldY     = 2.0f;
        cornerLight.worldZ     = 2.0f;
        cornerLight.radius     = 4.0f;
        cornerLight.red        = 1.0f;
        cornerLight.green      = 1.0f;
        cornerLight.blue       = 1.0f;

        deferred_renderer::point_light movingLight;
        movingLight.worldX     = g_movingLightArmLength * std::sin( g_yAngleLight );
        movingLight.worldY     = std::sin( g_yAngleLight ) * 1.0f;
        movingLight.worldZ     = g_movingLightArmLength * std::cos( g_yAngleLight );
        movingLight.radius     = 1.0f;
        movingLight.red        = 1.0f;
        movingLight.green      = 1.0f;
        movingLight.blue       = 1.0f;

        g_renderer->set_proj_matrix( g_proj, g_zNear );
        g_renderer->set_view_matrix( g_mv );

        if( g_ambientlightOn )
        {
            g_renderer->set_ambiant_light( 0.1f, 0.1f, 0.1f );
        }
        else
        {
            g_renderer->set_ambiant_light( 0.0f, 0.0f, 0.0f );
        }

        g_renderer->set_debug_rendermode( static_cast<deferred_renderer::DebugRenderMode>( g_rendermode ) );

        g_renderer->start_scene();

        if( g_toplightOn )
        {
            g_renderer->draw_directional_light( topLight );
        }

        if( g_bottomlightOn )
        {
            g_renderer->draw_directional_light( bottomLight );
        }

        if( g_cornerlightOn )
        {
            g_renderer->draw_point_light( cornerLight );
        }
        
        if( g_movinglightOn )
        {
            g_renderer->draw_point_light( movingLight );
        }

        std::vector<meshinstancedata> instanceData;

        if( g_showSingleInstance )
        {
            const float offset[3] = { 0.0, 0.0f, 0.0f };
            instanceData.push_back( make_instance( offset ) );
        }
        else
        {
            // 23: 3 rows of 5, sandwiched between 2 rows of 4 (15+8=23)
            instanceData.reserve( 23 );

            // Middle 15:
            for( size_t row = 0; row < 3; ++row )
            {
                for( size_t col = 0; col < 5; ++col )
                {
                    const float offset[3] = { g_separation* ( -4.0f + col * 2.0f ), 0.0f,
                                              g_separation* ( -2.0f + row * 2.0f )
                                            };
                    instanceData.push_back( make_instance( offset ) );
                }
            }

            // Top and bottom 4
            for( size_t col = 0; col < 4; ++col )
            {
                const float offsetTop[3]
                    = { g_separation* ( -3.0f + col * 2.0f ), +0.0f, g_separation * -4.0f };
                const float offsetBottom[3]
                    = { g_separation* ( -3.0f + col * 2.0f ), +0.0f, g_separation * +4.0f };
                instanceData.push_back( make_instance( offsetTop ) );
                instanceData.push_back( make_instance( offsetBottom ) );
            }
        }

        if( g_currentlyViewedMeshes < g_filecontents.size() )
        {
            const auto& currentlyViewedMeshHandles = g_filecontents[g_currentlyViewedMeshes].second;

            for_each( currentlyViewedMeshHandles.cbegin(),
                      currentlyViewedMeshHandles.cend(),
                      [&]( meshhandle_t h )
            { 
                g_renderer->draw_mesh_instances( h, instanceData.data(), instanceData.size() ); 
            } );
        }

        g_renderer->end_scene();
    }

    void set_rendermode( deferred_renderer::DebugRenderMode m )
    {
        g_rendermode = m;
    }

    void set_next_rendermode()
    {
        g_rendermode = ( g_rendermode + 1 ) % ( RENDERMODECOUNT );
    }

    void set_previous_rendermode()
    {
        g_rendermode = ( g_rendermode + RENDERMODECOUNT - 1 ) % ( RENDERMODECOUNT );
    }
}

namespace solosnake
{

    int show_meshes( int meshCount, const char* const* meshNames, const float separation )
    {
        g_separation = g_separationDistance * separation;

        reset_meshviewer();

        auto iw = ioswindow::make_shared(
                      VIEWWINDOW_W, VIEWWINDOW_H, true, "blueview", false, "blueview", "opengl 3.2" );

        iw->enable_drag_and_drop( true );

        g_timer = make_timer();
        g_window = window::make_shared( iw );

        const rect screenRect = iw->get_screen_rect();

        g_renderer = make_shared<deferred_renderer>(
                         screenRect.width(), screenRect.height(), deferred_renderer::HighQuality );

        g_renderer->set_viewport( 0, 0, VIEWWINDOW_W, VIEWWINDOW_H );

        g_windowobserver = make_shared<windowcallback>( on_window_event );
        g_window->add_observer( g_windowobserver );

        g_window->show();

        for( int i = 0; i < meshCount; ++i )
        {
            try
            {
                const filepath url( meshNames[i] );

                if( is_3dsfile( url ) )
                {
                    load_3dsfile( url );
                }
                else
                {
                    load_meshfile( url );
                }
            }
            catch( ... )
            {
                ss_err( "Error loading ", meshNames[i] );
            }
        }

        do
        {
            const auto& inputs = g_window->get_new_events();

            for( size_t i = 0; i < inputs.events().size(); ++i )
            {
                if( input_event::keyboard_sender == inputs.events()[i].sender_type() )
                {
                    switch( inputs.events()[i].event_type() )
                    {
                        case input_event::key_down_event:

                            if( inputs.events()[i].vkey() >= VK_F1 && inputs.events()[i].vkey()
                                    < ( VK_F1 + RENDERMODECOUNT ) )
                            {
                                set_rendermode( static_cast<deferred_renderer::DebugRenderMode>(
                                                    inputs.events()[i].vkey() - VK_F1 ) );
                            }
                            else if( inputs.events()[i].vkey() >= VK_NUMPAD0
                                     && inputs.events()[i].vkey() < ( VK_NUMPAD0 + RENDERMODECOUNT ) )
                            {
                                set_rendermode( static_cast<deferred_renderer::DebugRenderMode>(
                                                    inputs.events()[i].vkey() - VK_NUMPAD0 ) );
                            }
                            else if( inputs.events()[i].vkey() == VK_UP )
                            {
                                set_next_rendermode();
                            }
                            else if( inputs.events()[i].vkey() == VK_DOWN )
                            {
                                set_previous_rendermode();
                            }
                            else if( inputs.events()[i].vkey() == VK_TAB
                                     || inputs.events()[i].vkey() == VK_RETURN
                                     || inputs.events()[i].vkey() == VK_ADD )
                            {
                                show_next_contents();
                            }
                            else if( inputs.events()[i].vkey() == VK_BACK || inputs.events()[i].vkey()
                                     == VK_SUBTRACT )
                            {
                                show_previous_contents();
                            }
                            else if( inputs.events()[i].vkey() == 'W' )
                            {
                                toggle_rotate_x_up();
                            }
                            else if( inputs.events()[i].vkey() == 'S' )
                            {
                                toggle_rotate_x_down();
                            }
                            else if( inputs.events()[i].vkey() == 'A' )
                            {
                                toggle_rotate_y_up();
                            }
                            else if( inputs.events()[i].vkey() == 'D' )
                            {
                                toggle_rotate_y_down();
                            }
                            else if( inputs.events()[i].vkey() == 'Q' )
                            {
                                toggle_rotate_z_up();
                            }
                            else if( inputs.events()[i].vkey() == 'E' )
                            {
                                toggle_rotate_z_down();
                            }
                            else if( inputs.events()[i].vkey() == 'R' )
                            {
                                set_next_rendermode();
                            }
                            else if( inputs.events()[i].vkey() == 'T' )
                            {
                                set_next_teamcolour();
                            }
                            else if( inputs.events()[i].vkey() == 'Y' )
                            {
                                set_next_meshviewmode();
                            }
                            else if( inputs.events()[i].vkey() == 'C' )
                            {
                                set_next_color_0();
                            }
                            else if( inputs.events()[i].vkey() == 'V' )
                            {
                                set_next_color_1();
                            }
                            else if( inputs.events()[i].vkey() == VK_SPACE )
                            {
                                reset_rotations();
                                g_instanceTeamColorMode = 0;
                                g_colorIndex0 = 0;
                                g_colorIndex1 = 1;
                            }
                            else if( inputs.events()[i].vkey() == '1' )
                            {
                                g_ambientlightOn = !g_ambientlightOn;
                            }
                            else if( inputs.events()[i].vkey() == '2' )
                            {
                                g_toplightOn = !g_toplightOn;
                            }
                            else if( inputs.events()[i].vkey() == '3' )
                            {
                                g_bottomlightOn = !g_bottomlightOn;
                            }
                            else if( inputs.events()[i].vkey() == '4' )
                            {
                                g_cornerlightOn = !g_cornerlightOn;
                            }
                            else if( inputs.events()[i].vkey() == '5' )
                            {
                                g_movinglightOn = !g_movinglightOn;
                            }
                            else if( inputs.events()[i].vkey() == '6' )
                            {
                                g_ambientlightOn = false;
                                g_toplightOn     = false;
                                g_bottomlightOn  = false;
                                g_cornerlightOn  = false;
                                g_movinglightOn  = false;
                            }
                            else if( inputs.events()[i].vkey() == '7' )
                            {
                                g_ambientlightOn = true;
                                g_toplightOn     = true;
                                g_bottomlightOn  = true;
                                g_cornerlightOn  = true;
                                g_movinglightOn  = true;
                            }

                            break;
                        default:
                            break;
                    };
                }
                else if( input_event::cursor_sender == inputs.events()[i].sender_type() )
                {
                    switch( inputs.events()[i].event_type() )
                    {
                        case input_event::wheel_moved:

                            if( inputs.events()[i].wheeldelta() < 0 )
                            {
                                g_cameraDistance *= g_zoomRate;
                            }
                            else
                            {
                                g_cameraDistance /= g_zoomRate;
                            }

                            g_cameraDistance = g_cameraDistance < g_minCameraDistance ? g_minCameraDistance
                                               : g_cameraDistance;
                            g_cameraDistance = g_cameraDistance > g_maxCameraDistance ? g_maxCameraDistance
                                               : g_cameraDistance;
                            break;

                        default:
                            break;
                    }
                }
            }

            draw();
            g_window->swap_buffers();
        }
        while( !g_window->get_events().is_shutdown() );

        g_renderer.reset();
        g_window.reset();

        return 0;
    }
}
