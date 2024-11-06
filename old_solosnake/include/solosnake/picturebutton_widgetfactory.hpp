#ifndef solosnake_picturebuttonwidgetfactory_hpp
#define solosnake_picturebuttonwidgetfactory_hpp

#include <memory>
#include "solosnake/ilua_widgetfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class luaguicommandexecutor;
    class widgetrenderingstyles;

    //! Factory class for creating (Lua) buttonwidget types from XML.
    class picturebutton_widgetfactory : public ilua_widgetfactory
    {
    public:

        explicit picturebutton_widgetfactory( 
            const std::shared_ptr<luaguicommandexecutor>&,
            const std::shared_ptr<widgetrenderingstyles>&,
            const std::shared_ptr<ilanguagetext>& );

        ~picturebutton_widgetfactory();

        std::shared_ptr<iwidget> create_widget( const TiXmlElement& ) const override;

    protected:

        //! The texture X location for widgets that require this.
        static const char AttributeTexX[];

        //! The texture Y location for widgets that require this.
        static const char AttributeTexY[];

        //! The widget texture width attribute for widgets that require this.
        static const char AttributeTexWidth[];

        //! The widget texture height attribute for widgets that require this.
        static const char AttributeTexHeight[];

        //! The name of a texture. This should be the full texture name,
        //! minus the path, e.g. spaceshipexterior.bmp
        static const char AttributeTexName[];
        
        //! The text id. This is the identifier of the text in the language table. 
        //! It is recommended that the English language text be used as the ID.
        static const char AttributeTextId[];

        //! Size of the rendered text, default is zero.
        static const char AttributeTextSize[];

    private:

        std::shared_ptr<luaguicommandexecutor> lce_;
        std::shared_ptr<widgetrenderingstyles> renderingStyles_;
        std::shared_ptr<ilanguagetext>         language_;
    };
}

#endif
