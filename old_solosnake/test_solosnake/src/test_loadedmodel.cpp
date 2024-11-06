//#define RUN_TESTS
#ifdef RUN_TESTS

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "solosnake/testing/testing.hpp"
#include "solosnake/datadirectory.hpp"
#include "solosnake/deferred_renderer.hpp"
#include "solosnake/glutwindow.hpp"
#include "solosnake/image.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/blue/blue_testmodel.hpp"
#include "solosnake/blue/blue_load_models.hpp"

using namespace solosnake;
using namespace blue;

#define WINDOW_W 800
#define WINDOW_H 600

namespace
{
    bool check_OpenGL()
    {
        switch( glGetError() )
        {
            case GL_NO_ERROR:
                return true;

            case GL_INVALID_ENUM:
                std::cerr << "\nGL_INVALID_ENUM\n";
                break;

            case GL_INVALID_VALUE:
                std::cerr << "\nGL_INVALID_VALUE\n";
                break;

            case GL_INVALID_OPERATION:
                std::cerr << "\nGL_INVALID_OPERATION\n";
                break;

            case GL_STACK_OVERFLOW:
                std::cerr << "\nGL_STACK_OVERFLOW\n";
                break;

            case GL_STACK_UNDERFLOW:
                std::cerr << "\nGL_STACK_UNDERFLOW\n";
                break;

            case GL_OUT_OF_MEMORY:
                std::cerr << "\nGL_OUT_OF_MEMORY\n";
                break;

            default:
                std::cerr << "Unknown OpenGL error code.\n";
                break;
        }

        assert( glGetError() == GL_NO_ERROR );

        return false;
    }

    std::shared_ptr<deferred_renderer> rr;
    meshhandle_t h = 0;
    std::shared_ptr<imesh> msrc;
    matrix4x4_t mv;
    matrix4x4_t proj;

    class modelsrc : public imesh
    {
    public:
        modelsrc( std::shared_ptr<meshgeometry> g, std::shared_ptr<imeshtextureinfo> t )
        {
            geometry_ = g;
            texInfo_ = t;
        }

        virtual std::shared_ptr<meshgeometry> get_meshgeometry()
        {
            return geometry_;
        }

        virtual std::shared_ptr<imeshtextureinfo> get_meshgeometrytexinfo()
        {
            return texInfo_;
        }

    private:
        std::shared_ptr<meshgeometry> geometry_;
        std::shared_ptr<imeshtextureinfo> texInfo_;
    };

    //-------------------------------------------------------------------------

    void init()
    {
        solosnake::datadirectory assetsDir( "./meshes" );

        auto url = assetsDir.get_file( "thruster.lua" );

        auto meshes = load_mesh_datas( url );
        auto texinfos = load_texinfos( url, assetsDir );

        EXPECT_TRUE( meshes.size() == 1 );
        EXPECT_TRUE( texinfos.size() == 1 );

        std::shared_ptr<meshgeometry> geom = meshes[0];
        std::shared_ptr<imeshtextureinfo> tex = texinfos[0];

        msrc.reset( new modelsrc( geom, tex ) );

        rr = deferred_renderer::create(
                 WINDOW_W, WINDOW_H, deferred_renderer::HighQuality ); // 1920 x 1080
        h = rr->load_mesh( msrc );
    }

    void displayFn()
    {
        static float ry = 0.0f;

        ry += 0.01f;

        // load_xyz_ry( 0.0f, 0.0f, -10.0f, radians(ry), mv );
        load_identity_4x4( mv );
        set_translation( 0.0f, 0.0f, -10.0f, mv );

        const size_t instance_count = 5;
        std::vector<meshinstancedata> modeldatas;

        modeldatas.resize( instance_count );

        float range = 22.0f;
        float start = -range / 2.0f;
        float step = range / ( instance_count - 1 );

        for( size_t i = 0; i < instance_count; ++i )
        {
            load_xyz_ry( start + step * i, 0.0f, -12.0f, radians( ry ), modeldatas[i].transform );
            modeldatas[i].bgraTeam[0] = i % 2 ? 1.0f : 0.1f;
            modeldatas[i].bgraTeam[1] = i % 3 ? 1.0f : 0.1f;
            modeldatas[i].bgraTeam[2] = i % 5 ? 1.0f : 0.1f;
            modeldatas[i].bgraTeam[3] = i % 5 ? 1.0f : 0.1f;
        }

        assert( check_OpenGL() );
        rr->set_proj_matrix( proj );
        rr->set_view_matrix( mv );
        rr->start_scene();
        rr->draw_mesh_instances( h, &modeldatas.at( 0 ), modeldatas.size() );
        rr->draw_point_light();
        rr->end_scene();
        assert( check_OpenGL() );
    }

    void reshapeFn( int w, int h )
    {
        load_perspective( solosnake::radians( PI_RADIANS * 0.25f ), float( w ) / h, 0.1f, 100.0f, proj );
    }

    void exitFn( int )
    {
        rr.reset();
    }
}

// Undefine this to create the 1st images.
#if 0
TEST( load_models, createlayers )
{
    const size_t w = 512;
    const size_t h = 1024;

    solosnake::image img1( w, h, solosnake::image::format_bgra );
    solosnake::image img2( w, h, solosnake::image::format_bgra );
    solosnake::image img3( w, h, solosnake::image::format_bgra );

    solosnake::image::clear_layer_to( img1, 0, 0xFF );
    solosnake::image::clear_layer_to( img2, 1, 0xFF );
    solosnake::image::clear_layer_to( img3, 2, 0xFF );

    img1.save( "./meshes/cube.diffuseteam.bmp" );
    img2.save( "./meshes/cube.emissivespec.bmp" );
    img3.save( "./meshes/cube.normals.bmp" );

    solosnake::image img4( "./meshes/cube.diffuseteam.bmp" );
    solosnake::image img5( "./meshes/cube.emissivespec.bmp" );
    solosnake::image img6( "./meshes/cube.normals.bmp" );
}
#endif

TEST( test_blue_loadedmodel, ctor )
{
    try
    {
        auto glut = glutWindow::create( 3, 3, WINDOW_W, WINDOW_H );

        EXPECT_TRUE( GL_NO_ERROR == glGetError() );

        glut->glutExitFunc( exitFn );
        glut->glutDisplayFunc( displayFn );
        glut->glutReshapeFunc( reshapeFn );

        init();

        glut->glutMainLoop();
    }
    catch( ... )
    {
        ADD_FAILURE("Unexpected exception.");
    }
}

#endif
