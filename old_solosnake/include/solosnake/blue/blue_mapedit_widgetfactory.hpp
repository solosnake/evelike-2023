#ifndef blue_mapedit_widgetfactory_hpp
#define blue_mapedit_widgetfactory_hpp

#include "solosnake/ilua_widgetfactory.hpp"
#include "solosnake/blue/blue_widgetsfactory.hpp"

namespace solosnake
{
    class ilanguagetext;
    class luaguicommandexecutor;
    class widgetrenderingstyles;
}

namespace blue
{
    class iboardview;
    class mapedit;
    class user_settings;

    //! Factory for constructing mapedit_widgets.
    class mapedit_widgetfactory : public solosnake::ilua_widgetfactory
    {
    public:

        mapedit_widgetfactory(
            const std::shared_ptr<mapedit>&,
            const std::shared_ptr<user_settings>&,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::widgetrenderingstyles>&,
            const std::shared_ptr<solosnake::ilanguagetext>& );

        //! Creates a mapedit_widget.
        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;

    private:

        std::shared_ptr<user_settings>                      settings_;
        std::shared_ptr<solosnake::luaguicommandexecutor>   lce_;
        std::shared_ptr<solosnake::widgetrenderingstyles>   rendering_styles_;
        std::shared_ptr<solosnake::ilanguagetext>           language_;
        std::shared_ptr<mapedit>                            sharedmap_;
    };

    class mapeditscreen_widgetsfactory : public widgetsfactory
    {
    public:

        mapeditscreen_widgetsfactory( const std::shared_ptr<mapedit_widgetfactory>&, 
                                      const std::shared_ptr<widgetsfactory>&, 
                                      const std::shared_ptr<solosnake::luaguicommandexecutor>& );
        
        std::shared_ptr<solosnake::iwidget> create_widget( const TiXmlElement& ) const override;    
    private:

        std::shared_ptr<mapedit_widgetfactory>  mapeditwidgets_;
        std::shared_ptr<widgetsfactory>         menuwidgets_;
    };
}

#endif
