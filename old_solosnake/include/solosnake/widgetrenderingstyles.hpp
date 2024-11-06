#ifndef solosnake_widgetrenderingstyles_hpp
#define solosnake_widgetrenderingstyles_hpp

#include <memory>
#include <string>
#include <vector>

namespace solosnake
{
    class ifilefinder;
    class iwidgetrenderer;
    class rendering_system;

    //! Loads and retrieves named widget rendering styles. Throws if an
    //! attempt is made to access a style which it does not contain. The empty
    //! string must be registered as the default style.
    //! Each widget which renders can be passed an abstract widget renderer
    // interface
    //! which knows how to draw given types in a certain style.
    class widgetrenderingstyles
    {
    public:

        widgetrenderingstyles(
            const std::shared_ptr<ifilefinder>& stylesPaths,
            const std::shared_ptr<rendering_system>& renderSystem,
            const std::shared_ptr<iwidgetrenderer>& defaultStyle );

        ~widgetrenderingstyles();

        std::shared_ptr<iwidgetrenderer> get_widget_rendering_style( const std::string& ) const;

    private:

        mutable std::vector<std::shared_ptr<iwidgetrenderer>> styles_;
        std::shared_ptr<ifilefinder>                          stylesPaths_;
        std::shared_ptr<rendering_system>                     renderingSystem_;
    };
}

#endif
