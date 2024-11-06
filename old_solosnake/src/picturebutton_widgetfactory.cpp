#include <cassert>
#include <memory>
#include "solosnake/picturebutton_widget.hpp"
#include "solosnake/picturebutton_widgetfactory.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/rendering_system.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/external/lua/lunar.hpp"

using namespace std;

namespace solosnake
{
    const char picturebutton_widgetfactory::AttributeTexX[]         = "texx";
    const char picturebutton_widgetfactory::AttributeTexY[]         = "texy";
    const char picturebutton_widgetfactory::AttributeTexWidth[]     = "texwidth";
    const char picturebutton_widgetfactory::AttributeTexHeight[]    = "texheight";
    const char picturebutton_widgetfactory::AttributeTexName[]      = "texname";
    const char picturebutton_widgetfactory::AttributeTextSize[]     = "textsize";
    const char picturebutton_widgetfactory::AttributeTextId[]       = "text_id";

    picturebutton_widgetfactory::picturebutton_widgetfactory( const shared_ptr<luaguicommandexecutor>& lce,
                                                              const shared_ptr<widgetrenderingstyles>& styles,
                                                              const shared_ptr<ilanguagetext>& language )
        : ilua_widgetfactory( "picturebutton" )
        , lce_( lce )
        , renderingStyles_( styles )
        , language_( language )
    {
        if( false == lunar<picturebutton_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<picturebutton_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    picturebutton_widgetfactory::~picturebutton_widgetfactory()
    {
    }

    shared_ptr<iwidget>
    picturebutton_widgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // This can throw if params are missing or invalid.
        const widgetparams params = ixml_widgetfactory::read_widgetparams( xmlWidget );

        float tx = 0.0;
        float ty = 0.0;
        float tw = 0.0;
        float th = 0.0;
        string tn;

        const bool isOptional = false;

        bool allOK = read_attribute( xmlWidget, AttributeTexX, tx, isOptional )
                     && read_attribute( xmlWidget, AttributeTexY, ty, isOptional )
                     && read_attribute( xmlWidget, AttributeTexWidth, tw, isOptional )
                     && read_attribute( xmlWidget, AttributeTexHeight, th, isOptional )
                     && read_attribute( xmlWidget, AttributeTexName, tn, isOptional );

        if( false == allOK )
        {
            ss_throw( "picturebutton_widget missing required attributes." );
        }

        if( tw < 0.0 || th < 0.0 )
        {
            ss_throw( "texwidth and/or texheight attributes are out of bounds." );
        }

        // Read optional UTF-8 "text"
        string textid;
        solosnake::read_attribute( xmlWidget, AttributeTextId, textid, true );

        // read optional text size (default is zero)
        int textsize = 0;
        solosnake::read_attribute( xmlWidget, AttributeTextSize, textsize, true );

        auto stylename = read_style( xmlWidget );
        auto style = renderingStyles_->get_widget_rendering_style( stylename );

        const rectf texturearea = rectf( tx, ty, dimension2df( tw, th ) );
        const auto fnames = read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        return make_shared<picturebutton_widget>( params,
                                                  iwidget::NoKeyBoardFocus,
                                                  texturearea,
                                                  tn,
                                                  lce_,
                                                  style,
                                                  fnames,
                                                  textid,
                                                  textsize,
                                                  language_ );
    }
}
