#ifndef blue_skyboxbackground_widget_hpp
#define blue_skyboxbackground_widget_hpp

#include <string>
#include <memory>
#include "solosnake/point.hpp"
#include "solosnake/ilua_widget.hpp"
#include "solosnake/radians.hpp"

namespace solosnake
{
    struct cubemap;
    class deferred_renderer;
    class luaguicommandexecutor;
}

namespace blue
{
    //! Lua background widget which just enables skybox rendering and
    //! causes the skybox to rotate. The rate of rotation, the angle 
    //! of rotation, and the skybox loaded is read from xml file.
    class skyboxbackground_widget : public solosnake::ilua_widget
    {
    public:

        LUNAR_CLASS( skyboxbackground_widget );

        skyboxbackground_widget( lua_State* );

        //!
        skyboxbackground_widget(
            const std::string& name,
            const solosnake::cubemap& skybox,
            const bool show_skybox,
            const float axisx,
            const float axisy,
            const float axisz,
            const float radians_per_ms,
            const std::shared_ptr<solosnake::luaguicommandexecutor>&,
            const std::shared_ptr<solosnake::deferred_renderer>&,
            const solosnake::ilua_widget::functionnames& );

        void on_screen_resized( const solosnake::dimension2d<unsigned int>& ) override;
        
        void render( const unsigned long dt ) const override;

        ~skyboxbackground_widget();

    private:

        std::shared_ptr<solosnake::deferred_renderer>   renderer_;
        float                                           unit_axis_[3];
        float                                           radians_per_ms_;
        mutable float                                   angle_;
        bool                                            skybox_on_;
    };  
}

#endif
