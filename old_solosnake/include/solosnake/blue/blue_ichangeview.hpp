#ifndef blue_ichangeview_hpp
#define blue_ichangeview_hpp

namespace blue
{
    class iscreenview;

    //! Changes the active view based on the current view.
    class ichangeview
    {
    public:

        virtual ~ichangeview();

        //! Activates the view that follows this view.
        virtual void activate_next_view( iscreenview* currentView ) = 0;

        //! Activate the view the preceded this view.
        virtual void activate_prev_view( iscreenview* currentView ) = 0;
    };
}

#endif
