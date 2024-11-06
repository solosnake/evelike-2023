#ifndef solosnake_x11_hpp
#define solosnake_x11_hpp

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>
#include <memory>

namespace solosnake
{
    class X11ColorMap;
    class X11Window;
    class X11Display;

    //////////////////////////////////////////////////////////////////////////

    class X11FBConfig
    {
    public:
        explicit X11FBConfig( GLXFBConfig* );

        ~X11FBConfig();

        operator GLXFBConfig* () const
        {
            return config_;
        }

    private:
        X11FBConfig( const X11FBConfig& );
        X11FBConfig& operator=( X11FBConfig& );
        GLXFBConfig* config_;
    };

    //////////////////////////////////////////////////////////////////////////

    class X11VisualInfo
    {
    public:
        explicit X11VisualInfo( XVisualInfo* vi );

        ~X11VisualInfo();

        operator XVisualInfo* () const
        {
            return vi_;
        }

    private:
        X11VisualInfo( const X11VisualInfo& );
        X11VisualInfo& operator=( X11VisualInfo& );
        XVisualInfo* vi_;
    };

    //////////////////////////////////////////////////////////////////////////

    class X11ColorMap
    {
    public:
        X11ColorMap( XVisualInfo* vi, Display* );

        ~X11ColorMap();

        operator Colormap() const
        {
            return cmap_;
        }

    private:
        X11ColorMap( const X11ColorMap& );
        X11ColorMap& operator=( X11ColorMap& );
        Display* owner_;
        Colormap cmap_;
    };

    //////////////////////////////////////////////////////////////////////////

    class X11Display
    {
    public:
        X11Display();

        ~X11Display();

        inline operator Display* () const
        {
            return display_;
        }

    private:
        X11Display( const X11Display& );
        X11Display& operator=( X11Display& );
        Display* display_;
    };

    //////////////////////////////////////////////////////////////////////////

    class X11Window
    {
    public:
        X11Window( const char* name, unsigned int width, unsigned int height, unsigned int zDepth );

        ~X11Window();

        operator Window() const
        {
            return window_;
        }

        Display* display() const
        {
            return static_cast<Display*>( *display_ );
        }

        const Window* get_window_ptr() const
        {
            return &window_;
        }

        const GLXFBConfig& best_config() const
        {
            return bestFBConfig_;
        }
        
        const Atom& del_win_atom() const
        {
            return delete_window_atom_;
        }

    private:
        X11Window( const X11Window& );
        X11Window& operator=( const X11Window& );

    private:
        std::unique_ptr<X11Display>     display_;
        std::unique_ptr<X11ColorMap>    cmap_;
        Window                          window_;
        Atom                            delete_window_atom_;
        GLXFBConfig                     bestFBConfig_;
    };

    //////////////////////////////////////////////////////////////////////////
}

#endif
