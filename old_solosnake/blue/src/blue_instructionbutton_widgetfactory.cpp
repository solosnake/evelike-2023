#include <cassert>
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_instructionbutton_widgetfactory.hpp"
#include "solosnake/blue/blue_instructionbutton_widget.hpp"
#include "solosnake/luaguicommandexecutor.hpp"
#include "solosnake/textbutton_widgetfactory.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/widgetrenderingstyles.hpp"

using namespace solosnake;
using namespace std;

namespace blue
{
    const char instructionbuttonwidgetfactory::AttributeInstruction[] = "i";
    const char instructionbuttonwidgetfactory::AttributeInstructions[] = "instructions";

    instructionbuttonwidgetfactory::instructionbuttonwidgetfactory(
        const shared_ptr<ipendinginstructions>& pender,
        const shared_ptr<luaguicommandexecutor>& lce,
        const shared_ptr<widgetrenderingstyles>& styles,
        const shared_ptr<ilanguagetext>& language,
        const shared_ptr<compiler>& cc )
        : ilua_widgetfactory( "instructionbutton" )
        , lce_( lce )
        , renderingStyles_( styles )
        , language_( language )
        , pendinginstructions_( pender )
        , compiler_( cc )
    {
        assert( cc );

        // They are registered within lua as textbutton_widgets
        if( !lunar<textbutton_widget>::is_type_registered( lce_->lua() ) )
        {
            const bool allowCreationWithinLua = false;
            lunar<textbutton_widget>::register_type( lce_->lua(), allowCreationWithinLua );
        }
    }

    // Reads in an instructions from the 'instructions' xml element.
    instructions 
    instructionbuttonwidgetfactory::read_instructions( const TiXmlElement& xml, const compiler& cc )
    {
        vector<string> lines;
        lines.reserve( 128 );

        const TiXmlElement* i = xml.FirstChildElement( AttributeInstruction );

        while( i )
        {
            lines.push_back( i->GetText() );
            i = i->NextSiblingElement( AttributeInstruction );
        }

        // Zero instructions is handled by compiler.
        return cc.compile( lines );
    }

    shared_ptr<iwidget>
    instructionbuttonwidgetfactory::create_widget( const TiXmlElement& xmlWidget ) const
    {
        // This can throw if params are missing or invalid.
        const widgetparams params = ixml_widgetfactory::read_widgetparams( xmlWidget );

        // Read optional UTF-8 "text_id" - reuse textbutton_widgetfactory attributes.
        string textid;
        read_attribute( xmlWidget, textbutton_widgetfactory::AttributeTextId, textid, true );

        // read optional text size (default is zero) - reuse textbutton_widgetfactory
        // attributes.
        int textsize = 0;
        read_attribute( xmlWidget, textbutton_widgetfactory::AttributeTextSize, textsize, true );

        // Create an instructions and read in the instructions. This means
        // in theory we can have an instructions button with no
        // instructions defined in XML.
        blue::instructions code;
        const TiXmlElement* xmlInstructions = xmlWidget.FirstChildElement( AttributeInstructions );

        if( xmlInstructions )
        {
            code = read_instructions( *xmlInstructions, *compiler_ );
        }
        else
        {
            ss_wrn( "No instructions defined by instruction widget." );
        }

        const string stylename = read_style( xmlWidget );
        auto widgetrender = renderingStyles_->get_widget_rendering_style( stylename );
        const auto fnames = read_and_register_functions( lce_->lua(), params.name, xmlWidget );

        return make_shared<instructionbuttonwidget>(
                   params,
                   pendinginstructions_,
                   move( code ),
                   lce_,
                   fnames,
                   textid,
                   textsize,
                   widgetrender,
                   language_ );
    }
}
