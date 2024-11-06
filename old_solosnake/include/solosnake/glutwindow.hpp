#ifndef solosnake_glutwindow_hpp
#define solosnake_glutwindow_hpp

#include "solosnake/window.hpp"
#include <functional>
#include <string>

namespace solosnake
{
    //! Window class to ease use of GLUT code samples in testing.
    class glutWindow : public solosnake::window
    {
    public:

        enum Profile
        {
            AllowOldOpenGL,
            CoreProfileOnly
        };

        glutWindow( 
            const std::shared_ptr<ioswindow>& w,
            const solosnake::window::HeapOnly& );

        static std::shared_ptr<glutWindow> make_shared( 
            int major,
            int minor,
            int width,
            int height,
            Profile profile = AllowOldOpenGL,
            bool debug = false );

        //! Permanently stops the display loop, if it is running.
        void end_display_loop();

        void glutDisplayFunc( const std::function<void( void )>& );

        void glutReshapeFunc( const std::function<void( int width, int height )>& );

        void glutExitFunc( const std::function<void( int )>& );

        void glutMainLoop();

        void gluLookAt( 
            float* matrix,
            float eyex,
            float eyey,
            float eyez,
            float centerx,
            float centery,
            float centerz,
            float upx,
            float upy,
            float upz );

        //! @param fovy   Specifies the field of view angle, in degrees, in the
        //!               y direction.
        //! @param aspect Specifies the aspect ratio that determines the field 
        //!               of view in the x direction. The aspect ratio is the 
        //!               ratio of x (width) to y (height).
        void gluPerspective(
            float* matrix, 
            float fovy,
            float aspect,
            float zNear,
            float zFar );

        virtual ~glutWindow();

    private:

        glutWindow( const glutWindow& ); // = delete
        glutWindow& operator=( const glutWindow& ); // = delete

        void on_resized( const dimension2d<unsigned int>& ) override;

    private:

        std::function<void( void )> display_callback_;
        std::function<void( int width, int height )> reshape_callback_;
        std::function<void( int )> exit_callback_;
        bool continue_looping_;
    };
}

#endif
