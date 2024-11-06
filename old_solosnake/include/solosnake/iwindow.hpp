#ifndef solosnake_iwindow_hpp
#define solosnake_iwindow_hpp
#include <memory>
#include <vector>
#include <string>
#include "solosnake/fullscreen.hpp"
#include "solosnake/dimension.hpp"

namespace solosnake
{
    class ioswindow;

    //! Abstraction of a window. This is OS agnostic window which is attached
    //! to a operation system window which will feed it information on changes
    //! such as resizes etc. It can be attached to one OS window only.
    class iwindow : public std::enable_shared_from_this<iwindow>
    {
    public:

        virtual ~iwindow();
        virtual void on_fullscreen_change( const FullscreenState ) = 0;
        virtual void on_minimised() = 0;
        virtual void on_unminimised() = 0;
        virtual void on_resized( const Dimension2d<unsigned int>& ) = 0;
        virtual void on_dragdropped( const std::vector<std::wstring>& ) = 0;

    protected:

        iwindow();

        void attach_to( const std::shared_ptr<ioswindow>& );

        void detach();

        ioswindow* oswindow()
        {
            return oswindow_.get();
        }

        const ioswindow* oswindow() const
        {
            return oswindow_.get();
        }

    private:

        std::shared_ptr<ioswindow> oswindow_;
    };
}

#endif
