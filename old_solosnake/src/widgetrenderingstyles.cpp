#include "solosnake/widgetrenderingstyles.hpp"
#include "solosnake/styledwidgetrenderer.hpp"
#include "solosnake/iwidgetrenderer.hpp"
#include "solosnake/ifilefinder.hpp"
#include "solosnake/throw.hpp"

using namespace std;

namespace solosnake
{
    namespace
    {
        shared_ptr<iwidgetrenderer> load_style( const filepath& styleFile,
                                                const shared_ptr<rendering_system>& rr )
        {
            return styledwidgetrenderer::make_styledwidgetrenderer( styleFile, rr );
        }

        //! Loads the named style and adds it to the vector, and re-sorts the
        //! vector by style name. Returns the newly loaded style.
        shared_ptr<iwidgetrenderer> load_and_add_style( const string& style,
                                                        vector<shared_ptr<iwidgetrenderer>>& styles,
                                                        const shared_ptr<ifilefinder>& stylesPaths,
                                                        const shared_ptr<rendering_system>& renderingSystem )
        {
            auto styleUrl = stylesPaths->get_file( style );

            auto newStyle = load_style( styleUrl, renderingSystem );

            styles.push_back( newStyle );

            sort( styles.begin(),
                  styles.end(),
                  [&]( shared_ptr<iwidgetrenderer> lhs, shared_ptr<iwidgetrenderer> rhs )
            { return lhs->stylename() < rhs->stylename(); } );

            return newStyle;
        }
    }

    widgetrenderingstyles::widgetrenderingstyles(
        const shared_ptr<ifilefinder>& stylesPaths,
        const shared_ptr<rendering_system>& renderSystem,
        const shared_ptr<iwidgetrenderer>& defaultStyle )
        : stylesPaths_( stylesPaths ), renderingSystem_( renderSystem )
    {
        assert( renderSystem && stylesPaths );

        if( nullptr == defaultStyle.get() )
        {
            ss_throw( "Default iwidgetrenderer pointer was null." );
        }

        styles_.reserve( 16 );
        styles_.push_back( defaultStyle );
    }

    widgetrenderingstyles::~widgetrenderingstyles()
    {
    }

    //! Looks for the already loaded style with the matching name, and returns it
    //! if found. If not found, tries to load a file with a matching name - no
    //! suffix is appended, so a styles name may contain the extension, e.g. "electric_blue.xml"
    //! The empty string is a valid style name, and will always return the default style.
    shared_ptr<iwidgetrenderer>
    widgetrenderingstyles::get_widget_rendering_style( const string& style ) const
    {
        assert( false == styles_.empty() );
        assert( stylesPaths_ );

        if( style.empty() )
        {
            // Empty string returns default style.
            assert( false == styles_.empty() );
            return styles_[0];
        }

        for( size_t i = 0; i < styles_.size(); ++i )
        {
            if( styles_[i]->stylename() == style )
            {
                return styles_[i];
            }
        }

        // It wasn't found. Try loading and adding it.
        return load_and_add_style( style, styles_, stylesPaths_, renderingSystem_ );
    }
}
