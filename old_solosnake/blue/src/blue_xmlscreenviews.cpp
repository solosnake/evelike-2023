#include <string>
#include "solosnake/filepath.hpp"
#include "solosnake/external/lua.hpp"
#include "solosnake/external/xml.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/gui.hpp"
#include "solosnake/ixmlelementreader.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/blue/blue_xmlscreenviews.hpp"
#include "solosnake/blue/blue_guiscreenview.hpp"
#include "solosnake/blue/blue_iscreenview.hpp"
#include "solosnake/blue/blue_widgetsfactory.hpp"
#include "solosnake/blue/blue_screenxmlnames.hpp"

using namespace std;
using namespace solosnake;

namespace blue
{
    namespace
    {
        //! Called from within Lua to activate the named screen.
        //! Accessible from Lua as "bool set_active_view( const char* )"
        //! Returns true if the named view is the active view after the call
        //! (even if no change was made by the call).
        int lua_set_active_view( lua_State* L )
        {
            // Get upvalue (the xmlscreenviews*)
            xmlscreenviews* views = reinterpret_cast<xmlscreenviews*>(
                                        lua_touserdata( L, lua_upvalueindex( 1 ) ) );

            assert( views );
            assert( lua_isstring( L, 1 ) );

            if( lua_isstring( L, 1 ) )
            {
                const bool activated = views->try_set_as_active_view( luaL_checkstring( L, 1 ) );
                lua_pushboolean( L, activated ? 1 : 0 );
            }

            return 1;
        }

        void expose_xmlscreenviews_to_lua( xmlscreenviews* v, lua_State* L )
        {
            // "bool set_active_view( const char* )"
            lua_pushlightuserdata( L, v );
            lua_pushcclosure( L, &lua_set_active_view, 1 );
            lua_setglobal( L, "set_active_view" );
        }

        inline bool is_view_xml_element( const TiXmlElement* pElement )
        {
            return ( 0 == pElement->ValueStr().compare( BLUE_VIEW_XML_ELEMENT ) );
        }

        //!  Constructs a named screen view from an XML definition. The view //
        //!   must contain a single gui element.                             //
        //!  \code                                                           //
        //!  <view name="View1">                                             //
        //!  <gui>                                                           //
        //!  <widget />                                                      //
        //!  <widget />                                                      //
        //!  <widget />                                                      //
        //!  </gui>                                                          //
        //!  </view>                                                         //
        //!  <view name="View2">                                             //
        //!  <gui>                                                           //
        //!  <widget />                                                      //
        //!  <widget />                                                      //
        //!  </gui>                                                          //
        //!  </view>                                                         //
        //!  \endcode                                                        //
        shared_ptr<iscreenview> read_iscreenview_from_xml( const TiXmlElement* pViewElement,
                                                           shared_ptr<widgetsfactory> widgetFactory,
                                                           const dimension2d<unsigned int>& windowSize )
        {
            assert( pViewElement );

            const string viewName = read_attribute( *pViewElement, BLUE_VIEWNAME_XML_ATTRIBUTE );

            const TiXmlElement& guiNode = get_child_element( *pViewElement, SOLOSNAKE_XML_ELEMENT_GUI );

            if( nullptr != guiNode.NextSiblingElement( SOLOSNAKE_XML_ELEMENT_GUI ) )
            {
                ss_throw( BLUE_VIEW_XML_ELEMENT " contains more than one " SOLOSNAKE_XML_ELEMENT_GUI " element." );
            }

            auto g = make_shared<gui>( guiNode, windowSize, widgetFactory->get_luaguicommandexecutor(), widgetFactory );

            return make_shared<guiscreenview>( g, viewName );
        }

        vector<shared_ptr<iscreenview>> load_screenviews_from_xml(
                                         const filepath& xmlScreenFile,
                                         const dimension2d<unsigned int>& windowSize,
                                         shared_ptr<widgetsfactory> widgetFactory )
        {
            vector<shared_ptr<iscreenview>> views;
            views.reserve( 8 );

            TiXmlDocument doc;

            solosnake::load_xml_doc( doc, xmlScreenFile.string().c_str() );

            const TiXmlNode* pScreenNode = doc.FirstChild( BLUE_SCREEN_XML_ELEMENT );

            if( pScreenNode )
            {
                const TiXmlNode* pViewNode = pScreenNode->FirstChild( BLUE_VIEW_XML_ELEMENT );

                while( pViewNode && solosnake::xml_doc_no_err( doc ) )
                {
                    const TiXmlElement* pElement = pViewNode->ToElement();

                    if( pElement && is_view_xml_element( pElement ) )
                    {
                        views.push_back( read_iscreenview_from_xml( pElement, widgetFactory, windowSize ) );
                    }

                    pViewNode = pViewNode->NextSibling();
                }
            }
            else
            {
                ss_throw( "Invalid screen XML file, no '" BLUE_SCREEN_XML_ELEMENT "' element present." );
            }

            return views;
        }
    }

    //-------------------------------------------------------------------------

    shared_ptr<xmlscreenviews>
    xmlscreenviews::make_shared( const shared_ptr<iscreenview>& activeView,
                                 const shared_ptr<iscreenstate>& screenState )
    {
        assert( activeView );
        assert( screenState );

        shared_ptr<xmlscreenviews> views = std::make_shared<xmlscreenviews>( HeapOnly() );

        views->store_view( activeView );
        views->set_as_active_view( activeView );
        views->set_screen_state( screenState );

        assert( views );
        assert( views->active_view_ );
        assert( views->screen_state_ );
        assert( false == views->views_.empty() );

        return views;
    }

    shared_ptr<xmlscreenviews>
    xmlscreenviews::make_shared( const filepath& xmlScreenFile,
                                 const dimension2d<unsigned int>& windowSize,
                                 const shared_ptr<widgetsfactory>& widgetFactory )
    {
        assert( widgetFactory );
        assert( std::filesystem::is_regular_file( xmlScreenFile ) );

        shared_ptr<xmlscreenviews> views = std::make_shared<xmlscreenviews>( HeapOnly() );

        views->set_screen_state( widgetFactory->get_luaguicommandexecutor() );

        lua_State* L = widgetFactory->get_luaguicommandexecutor()->lua();

        expose_xmlscreenviews_to_lua( views.get(), L );

        auto loadedViews = load_screenviews_from_xml( xmlScreenFile, windowSize, widgetFactory );

        // If a screen defines no views, it is an error.
        if( loadedViews.empty() )
        {
            ss_err( "Error, '",
                    xmlScreenFile.string(),
                    "' defines no views. A screen must have at least one view." );

            ss_throw( "XML " BLUE_SCREEN_XML_ELEMENT " element contained no " BLUE_VIEW_XML_ELEMENT
                      " elements - at least one view is required." );
        }

        for_each(
            loadedViews.cbegin(),
            loadedViews.cend(),
        [&]( const std::shared_ptr<iscreenview>& i ) { views->store_view( i ); } );

        // By convention we will say the first view in the list is the initial
        // active view.
        views->set_as_active_view( loadedViews.at( 0 ) );

        // Force a GC.
        lua_gc( L, LUA_GCCOLLECT, 0 );

        // Log usage.
        ss_log( "Total Lua memory usage (Kb) : ", lua_gc( L, LUA_GCCOUNT, 0 ) );
        ss_log( "Active view set to : ", loadedViews.at( 0 )->screenview_name().c_str() );

        assert( views );
        assert( views->active_view_ );
        assert( views->screen_state_ );
        assert( false == views->views_.empty() );

        return views;
    }

    xmlscreenviews::xmlscreenviews( const HeapOnly& )
    {
    }

    void xmlscreenviews::advance_one_frame()
    {
        for_each(
            views_.begin(),
            views_.end(),
            [&]( const shared_ptr<iscreenview>& v )
        {
            v->advance_one_frame();
        } );
    }

    void xmlscreenviews::render_active_view( const unsigned long dt ) const
    {
        active_view_->render_view( dt );
    }

    solosnake::LoopResult xmlscreenviews::handle_inputs( const solosnake::iinput_events& e )
    {
        return active_view_->handle_inputs( e );
    }

    //! Forwards call on to game screen state object (this is also shared
    //! via Lua with all the widgets etc.
    solosnake::nextscreen xmlscreenviews::get_next_screen() const
    {
        assert( screen_state_ );
        return screen_state_->get_next_screen();
    }

    //! Call to handle correct changing of active view.
    void xmlscreenviews::set_as_active_view( const shared_ptr<iscreenview>& v )
    {
        assert( v );

        if( v != active_view_ )
        {
            if( active_view_ )
            {
                active_view_->deactivate_view();
            }

            active_view_ = v;
            active_view_->activate_view();
        }
    }

    //! Returns true if the named view is set as the active view after this call.
    //! Returns true even if it was the active view all alone and no change was
    //! made.
    bool xmlscreenviews::try_set_as_active_view( const char* viewName )
    {
        auto v = try_get_named_view( viewName );

        if( v )
        {
            set_as_active_view( v );
        }

        return v.get() != nullptr;
    }

    //! If there is a view stored with this name, it will be returned, else
    //! returns the nullptr.
    shared_ptr<iscreenview> xmlscreenviews::try_get_named_view( const char* viewName ) const
    {
        for( size_t i = 0u; i < views_.size(); ++i )
        {
            if( 0 == views_[i]->screenview_name().compare( viewName ) )
            {
                return views_[i];
            }
        }

        return shared_ptr<iscreenview>();
    }

    //! If there is a view stored with this name, it will be returned, else
    //! returns the nullptr.
    shared_ptr<iscreenview> xmlscreenviews::try_get_named_view( const string& viewName ) const
    {
        return try_get_named_view( viewName.c_str() );
    }

    //! Stores the controller to ensure its lifetime is associated with the game
    // lifetime.
    void xmlscreenviews::store_view( const shared_ptr<iscreenview>& v )
    {
        assert( v );

        if( try_get_named_view( v->screenview_name() ) )
        {
            ss_err( "A view with this name was already registered : ", v->screenview_name() );
            ss_throw( "Attempt to store view with same name as existing view." );
        }

        views_.push_back( v );
    }

    //! External call to change screen size, forwards on to internal call.
    void xmlscreenviews::handle_screensize_changed( const solosnake::dimension2d<unsigned int>& r )
    {
        set_screensize( r );
    }

    //! Sets the screen state pointer. This is typically the state shared amongst
    //! all the objects, such as the widgets etc.
    void xmlscreenviews::set_screen_state( const shared_ptr<solosnake::iscreenstate>& ss )
    {
        assert( ss );
        screen_state_ = ss;
    }

    //! Informs ALL VIEWS of the screen size changes.
    void xmlscreenviews::set_screensize( const solosnake::dimension2d<unsigned int>& r )
    {
        for_each(
            views_.begin(),
            views_.end(),
            [&]( const shared_ptr<iscreenview>& v )
        {
            v->handle_screensize_changed( r );
        } );
    }

    //! Informs ALL VIEWS of the screen size state change.
    void xmlscreenviews::handle_fullscreen_changed( const solosnake::FullscreenState& full )
    {
        for_each(
            views_.begin(),
            views_.end(),
            [&]( const shared_ptr<iscreenview>& v )
        {
            v->handle_fullscreen_changed( full );
        } );
    }

    //! Informs ALL VIEWS of the screen size state change.
    void xmlscreenviews::handle_minimised_changed( const solosnake::MinimisedState& mini )
    {
        for_each(
            views_.begin(),
            views_.end(),
            [&]( const shared_ptr<iscreenview>& v )
        {
            v->handle_minimised_changed( mini );
        } );
    }
}
