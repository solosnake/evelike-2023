#include "solosnake/iwindow.hpp"
#include "solosnake/ioswindow.hpp"

namespace solosnake
{
    iwindow::iwindow()
    {
    }

    iwindow::~iwindow()
    {
    }

    void iwindow::attach_to( const std::shared_ptr<ioswindow>& newwindow )
    {
        detach();

        if( newwindow )
        {
            oswindow_ = newwindow;
            newwindow->attach( shared_from_this() );
        }
    }

    void iwindow::detach()
    {
        if( oswindow_ )
        {
            std::shared_ptr<iwindow> empty;
            oswindow_->attach( empty );
        }
    }
}