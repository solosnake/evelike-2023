#include "solosnake/layout.hpp"
#include "solosnake/iwidget.hpp"
#include <cassert>
#include <algorithm>
#include <limits>

namespace solosnake
{
    namespace
    {
#ifndef NDEBUG // This is only used in assert here.         
         
        //! Used to validate callback - check to see the widget we are referring to
        //! is really in our collection. Returns true if widget pointer w
        //! is in the widget collection.
        bool contains( const iwidget* w, const iwidgetcollection& wc )
        {
            bool found = false;

            auto i = wc.cbegin();
            auto end = wc.cend();

            while( ( false == found ) && ( i != end ) )
            {
                found = ( w == i->get() );
                ++i;
            }

            return found;
        }
#endif

    }

    // Ctor
    layout::layout( iwidgetcollection&& w, const HeapOnly& ) : gui_( nullptr ), widgets_( std::move( w ) )
    {
    }

    // Creates the layout ptr and initialises it by adding all the widgets
    // in the collection to the layout. The widgets will all know about the
    // layout and vice-versa.
    std::shared_ptr<layout> layout::make_shared( iwidgetcollection&& wc )
    {
        std::shared_ptr<layout> layt = std::make_shared<layout>( std::move( wc ), HeapOnly() );

        auto i = layt->widgets_.begin();
        while( i != layt->widgets_.end() )
        {
            ( *i )->set_layout( layt );
            layt->hittester_.add_rect( ( *i )->active_area(), i->get() );
            ++i;
        }

        return layt;
    }

    void layout::set_gui( gui* g )
    {
        gui_ = g;
    }

    void layout::widget_changed_location( iwidget& w )
    {
        assert( contains( &w, widgets_ ) );

        hittester_.update_rect( &w, w.active_area() );

        if( gui_ )
        {
            gui_->on_widget_changed_location( w );
        }
    }

    void layout::widget_changed_enabled( iwidget& w )
    {
        if( gui_ )
        {
            gui_->on_widget_changed_enabled( w );
        }
    }

    // Returns a pointer to the named widget, or null if it cannot be found.
    // Comparison is case sensitive.
    iwidget* layout::find_widget( const char* widgetname )
    {
        if( widgetname )
        {
            auto w = std::find_if(
                         widgets_.begin(),
                         widgets_.end(),
            [&]( const std::shared_ptr<iwidget>& wgt ) { return wgt->name() == widgetname; } );

            if( w != widgets_.end() )
            {
                return w->get();
            }
        }

        return nullptr;
    }

    // Returns first widget with highest z found in widgets under (x,y), or
    // nullptr if none underneath.
    iwidget* layout::highest_widget_under( int x, int y ) const
    {
        std::vector<iwidget*> under;
        widgets_under( x, y, under );

        int highestZlayer = std::numeric_limits<int>::lowest();

        // We will agree to only consider one widget as the handler of the event:
        iwidget* xyWidget = nullptr;

        // Find widget with highest layer:
        std::for_each(
            under.begin(),
            under.end(),
            [&]( iwidget * w )
        {
            if( w->zlayer() >= highestZlayer )
            {
                highestZlayer = w->zlayer();
                xyWidget = w;
            }
        } );

        return xyWidget;
    }

    // Returns all the enabled widgets who lie under (x,y), and those that do not.
    void layout::widgets_under(
        int x,
        int y,
        std::vector<iwidget*>& under ) const
    {
        hittester_.get_rects_under( x, y, under );

        // Erase all the non enabled widgets.
        under.erase(
            std::remove_if( under.begin(),
                            under.end(),
        []( const iwidget * w ) { return false == w->is_widget_enabled(); } ),
        under.end() );

        // Erase all the widgets where x, y is not inside.
        under.erase(
            std::remove_if( under.begin(),
                            under.end(),
        [ = ]( const iwidget * w ) { return false == w->contains( x, y ); } ),
        under.end() );
    }

    // Returns all the enabled widgets who lie under (x,y), and those that do not.
    void layout::widgets_under(
        int x,
        int y,
        std::vector<iwidget*>& under,
        std::vector<iwidget*>& notUnder ) const
    {
        hittester_.get_rects_under( x, y, under, notUnder );

        // Erase all the non enabled widgets.
        under.erase(
            std::remove_if( under.begin(),
                            under.end(),
                            []( const iwidget * w ) { return false == w->is_widget_enabled(); } ),
            under.end() );

        notUnder.erase(
            std::remove_if( notUnder.begin(),
                            notUnder.end(),
                            []( const iwidget * w ) { return false == w->is_widget_enabled(); } ),
            notUnder.end() );

        // Move all widgets where x, y is not inside to the not under array.
        std::vector<iwidget*> underTmp;
        underTmp.reserve( under.size() );

        for( size_t i = 0; i < under.size(); ++i )
        {
            if( under[i]->contains( x, y ) )
            {
                underTmp.push_back( under[i] );
            }
            else
            {
                notUnder.push_back( under[i] );
            }
        }

        under = std::move( underTmp );
    }

    // Tells all the widgets the screen was resized.
    void layout::screen_was_resized( const dimension2d<unsigned int>& windowSize )
    {
        std::for_each(
            widgets_.begin(),
            widgets_.end(),
            [ = ]( std::shared_ptr<iwidget>& w ) 
        { 
            w->screen_resized_to( windowSize ); 
        } );
    }

    // Reset all widgets - the application is no longer receiving mouse and
    // keyboard events and so any inner states relying on this should be reset.
    void layout::handle_application_focus_change()
    {
        std::for_each(
            widgets_.begin(),
            widgets_.end(),
            [ = ]( std::shared_ptr<iwidget>& w ) 
        { 
            w->reset();
        } );
    }
}
