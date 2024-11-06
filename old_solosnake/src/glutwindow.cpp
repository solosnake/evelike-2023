#include <cstddef>
#include <sstream>
#include "solosnake/glutwindow.hpp"
#include "solosnake/ioswindow.hpp"
#include "solosnake/opengl.hpp"
#include "solosnake/matrix3d.hpp"
#include "solosnake/throw.hpp"

namespace solosnake
{
    std::shared_ptr<glutWindow> glutWindow::make_shared( int major,
                                                         int minor,
                                                         int width,
                                                         int height,
                                                         Profile profile,
                                                         bool debug )
    {
        std::ostringstream os;

        os << "opengl " << major << '.' << minor;

        if( debug )
        {
            os << " -d";
        }

        if( profile == AllowOldOpenGL )
        {
            os << " -old";
        }

        auto w = ioswindow::make_shared( width,
                                         height,
                                         false,
                                         "glutWindow",
                                         false,
                                         "glut_test_class",
                                         os.str() );

        assert( GL_NO_ERROR == glGetError() );
        assert( w.get() != NULL );

        assert( GL_NO_ERROR == glGetError() );
        auto glut = std::make_shared<glutWindow>( w, solosnake::window::HeapOnly() );
        assert( GL_NO_ERROR == glGetError() );

        glut->attach_to( w );

        return glut;
    }

    glutWindow::glutWindow( const std::shared_ptr<ioswindow>& w,
                            const solosnake::window::HeapOnly& h )
        : window( w, h ), continue_looping_( true )
    {
        assert( GL_NO_ERROR == glGetError() );
    }

    glutWindow::~glutWindow()
    {
        if( exit_callback_ )
        {
            exit_callback_( 0 );
        }

        exit_callback_ = std::function<void( int )>();
        display_callback_ = std::function<void( void )>();
        reshape_callback_ = std::function<void( int width, int height )>();

        detach();
    }

    void glutWindow::end_display_loop()
    {
        continue_looping_ = false;
    }

    void glutWindow::glutDisplayFunc( const std::function<void( void )>& func )
    {
        display_callback_ = func;
    }

    void glutWindow::glutReshapeFunc( const std::function<void( int width, int height )>& func )
    {
        reshape_callback_ = func;
    }

    void glutWindow::glutExitFunc( const std::function<void( int )>& func )
    {
        exit_callback_ = func;
    }

    void glutWindow::gluLookAt( float* matrix,
                                float eyex,
                                float eyey,
                                float eyez,
                                float centerx,
                                float centery,
                                float centerz,
                                float upx,
                                float upy,
                                float upz )
    {
        const float lookedAt3f[] = { centerx, centery, centerz };
        const float position3f[] = { eyex, eyey, eyez };
        const float upVec3f[] = { upx, upy, upz };
        solosnake::load_look_at( lookedAt3f, position3f, upVec3f, matrix );
    }

    void glutWindow::gluPerspective( float* matrix, float fovy, float aspect, float zNear, float zFar )
    {
        const float rFov = fovy * 3.1415926536f / 180.0f;
        solosnake::load_perspective( solosnake::radians( rFov ), aspect, zNear, zFar, matrix );
    }

    void glutWindow::on_resized( const dimension2d<unsigned int>& newSize )
    {
        window::on_resized( newSize );

        if( reshape_callback_ )
        {
            reshape_callback_( static_cast<int>( newSize.width() ), static_cast<int>( newSize.height() ) );
        }
    }

    void glutWindow::glutMainLoop()
    {
        if( oswindow() )
        {
            show();

            while( ( false == get_events().is_shutdown() ) && continue_looping_ )
            {
                get_new_events();

                if( display_callback_ )
                {
                    display_callback_();
                }

                assert( GL_NO_ERROR == glGetError() );
                swap_buffers();
                assert( GL_NO_ERROR == glGetError() );
            }
        }
        else
        {
            ss_throw( "glutMainLoop previously called." );
        }
    }
}
