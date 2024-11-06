#ifndef blue_datapathfinder_hpp
#define blue_datapathfinder_hpp

#include <cassert>
#include "solosnake/ifilefinder.hpp"
#include "solosnake/blue/blue_datapaths.hpp"

namespace blue
{
    //! Binds a datapath object with one of the datapath objects own member
    //! functions to create an object that will always lookup files in that
    //! data types folder(s). This allows us to pass in per-data-type
    //! ifilefinder object from a common / shared datapaths object.
    //! e.g.
    //! auto guifinder = std::make_shared<datapathfinder>( dataPaths,
    //&datapaths::get_gui_filepath );
    class datapathfinder : public solosnake::ifilefinder
    {
    public:

        typedef solosnake::filepath( datapaths::*subpath_t )( const std::string& ) const;

        datapathfinder( const std::shared_ptr<datapaths>&, subpath_t );

        solosnake::filepath get_filepath( const std::string& filename ) const override;

    private:

        std::shared_ptr<datapaths>  paths_;
        subpath_t                   subpath_;
    };

    //--------------------------------------------------------------------------

    inline datapathfinder::datapathfinder( const std::shared_ptr<datapaths>& d, subpath_t p )
        : paths_( d )
        , subpath_( p )
    {
    }

    //! Call through the datapaths object with the stored member-function-pointer to
    //! lookup the named file.
    inline solosnake::filepath datapathfinder::get_filepath( const std::string& filename ) const
    {
        assert( paths_ );
        return ( ( *paths_ ).*subpath_ )( filename );
    }
}

#endif
