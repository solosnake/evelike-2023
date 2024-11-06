#include <algorithm>
#include <cassert>
#include "solosnake/blue/blue_datapaths.hpp"
#include "solosnake/blue/blue_machineparts.hpp"
#include "solosnake/manifest.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"

#define BLUE_EXPECTED_CHASSIS_TYPES_COUNT    (64)
#define BLUE_EXPECTED_SOFTPOINT_TYPES_COUNT (128)
#define BLUE_EXPECTED_HARDPOINT_TYPES_COUNT (128)
#define BLUE_EXPECTED_THRUSTER_TYPES_COUNT   (64)

using namespace std;

namespace blue
{
    namespace
    {
        template <typename T>
        std::vector<solosnake::filepath> get_manifests( const datapaths& );

        template <typename T>
        solosnake::filepath get_filepath( const datapaths&, const std::string& );

        template <typename T>
        vector<T> read_xml( const solosnake::filepath& );

        template <typename T>
        const char* name_of_type();

        //-------------------------------------------------------------------------
        // Specialisations:

        // CHASSIS

        template <>
        std::vector<solosnake::filepath> get_manifests<Chassis>( const datapaths& paths )
        {
            return paths.get_chassis_manifests();
        }

        template <>
        solosnake::filepath get_filepath<Chassis>( const datapaths& paths,
                                                   const std::string& filename )
        {
            return paths.get_chassis_filepath( filename );
        }

        template <>
        vector<Chassis> read_xml<Chassis>( const solosnake::filepath& filename )
        {
            return Chassis::read_chassis_xml( filename );
        }

        template <>
        const char* name_of_type<Chassis>()
        {
            return "Chassis";
        }

        // SOFTPOINTS

        template <>
        std::vector<solosnake::filepath> get_manifests<Softpoint>( const datapaths& paths )
        {
            return paths.get_softpoints_manifests();
        }

        template <>
        solosnake::filepath get_filepath<Softpoint>( const datapaths& paths,
                                                     const std::string& filename )
        {
            return paths.get_softpoints_filepath( filename );
        }

        template <>
        vector<Softpoint> read_xml<Softpoint>( const solosnake::filepath& filename )
        {
            return Softpoint::read_softpoint_xml( filename );
        }

        template <>
        const char* name_of_type<Softpoint>()
        {
            return "Softpoint";
        }

        // HARDPOINTS

        template <>
        std::vector<solosnake::filepath> get_manifests<Hardpoint>( const datapaths& paths )
        {
            return paths.get_hardpoints_manifests();
        }

        template <>
        solosnake::filepath get_filepath<Hardpoint>( const datapaths& paths,
                                                     const std::string& filename )
        {
            return paths.get_hardpoints_filepath( filename );
        }

        template <>
        vector<Hardpoint> read_xml<Hardpoint>( const solosnake::filepath& filename )
        {
            return Hardpoint::read_hardpoint_xml( filename );
        }

        template <>
        const char* name_of_type<Hardpoint>()
        {
            return "Hardpoint";
        }

        // THRUSTERS

        template <>
        std::vector<solosnake::filepath> get_manifests<Thruster>( const datapaths& paths )
        {
            return paths.get_thrusters_manifests();
        }

        template <>
        solosnake::filepath get_filepath
        <Thruster>( const datapaths& paths, const std::string& filename )
        {
            return paths.get_thrusters_filepath( filename );
        }

        template <>
        vector<Thruster> read_xml<Thruster>( const solosnake::filepath& filename )
        {
            return Thruster::read_thruster_xml( filename );
        }

        template <>
        const char* name_of_type<Thruster>()
        {
            return "Thruster";
        }

        //-------------------------------------------------------------------------

        template <typename T>
        void check_names_dont_conflict( const char* nameoftype,
                                        const char* manifestName,
                                        const vector<T>& lhs,
                                        const vector<T>& rhs )
        {
            for( size_t i = 0; i < lhs.size(); ++i )
            {
                const auto it = find_if( rhs.cbegin(),
                                         rhs.cend(),
                [&]( const T & c ) { return c.name() == lhs[i].name(); } );

                if( it != rhs.cend() )
                {
                    ss_err( "Duplicate ",
                            nameoftype,
                            " name '",
                            lhs[i].name(),
                            "' found while loading '",
                            manifestName,
                            "'." );
                    ss_throw( "Duplicated name found in manifest." );
                }
            }
        }

        template <typename T>
        vector<T> load_manifests( const datapaths& paths, const solosnake::filepath& manifestFile )
        {
            vector<T> results;

            solosnake::manifest fileList;
            solosnake::ixmlelementreader::read( manifestFile.string().c_str(), fileList );

            for( size_t i = 0; i < fileList.size(); ++i )
            {
                auto listedFile = get_filepath<T>( paths, fileList[i] );
                vector<T> manifestContents = read_xml<T>( listedFile.string().c_str() );
                check_names_dont_conflict( name_of_type<T>(), listedFile.string().c_str(), manifestContents, results );
                results.insert( results.end(), manifestContents.cbegin(), manifestContents.cend() );
                ss_dbg( "Loaded ", name_of_type<T>, " ", listedFile.string().c_str() );
            }

            return results;
        }

        //! Returns vector of machineparts sorted by Component name.
        template <typename T>
        vector<T> load_manifests( const datapaths& paths, const size_t numToReserve )
        {
            vector<T> results;
            results.reserve( numToReserve );

            auto manifestFiles = get_manifests<T>( paths );

            auto manifestLoader = [&]( const solosnake::filepath & manifestXML )
            {
                auto manifestContents = load_manifests<T>( paths, manifestXML );
                results.insert( results.end(), manifestContents.cbegin(), manifestContents.cend() );
            };

            for_each( manifestFiles.cbegin(), manifestFiles.cend(), manifestLoader );

            sort( results.begin(), results.end(), [ = ]( const T & lhs, const T & rhs ) { return lhs.name() < rhs.name(); } );

            return results;
        }

        template <typename T>
        const T* try_find_part_named( const string& name, const vector<T>& parts )
        {
            // NB "If no element in the range compares less than val, the function
            // returns last."
            auto i = lower_bound( parts.cbegin(),
                                  parts.cend(),
                                  name,
                                  [&]( const T & t, const string & n )
            { return t.name() < n; } );

            return ( i == parts.end() || i->name() != name ) ? nullptr : &*i;
        }
    }

    //! Loads all the available files named in the manifests.
    machineparts::machineparts( const datapaths& paths )
        : chassis_( load_manifests<Chassis>( paths, BLUE_EXPECTED_CHASSIS_TYPES_COUNT ) )
        , softpoints_( load_manifests<Softpoint>( paths, BLUE_EXPECTED_SOFTPOINT_TYPES_COUNT ) )
        , hardpoints_( load_manifests<Hardpoint>( paths, BLUE_EXPECTED_HARDPOINT_TYPES_COUNT ) )
        , thrusters_( load_manifests<Thruster>( paths, BLUE_EXPECTED_THRUSTER_TYPES_COUNT ) )
    {
    }

    const Thruster* machineparts::get_thruster( const std::string& name ) const
    {
        return try_find_part_named( name, thrusters_ );
    }

    const Hardpoint* machineparts::get_hardpoint( const std::string& name ) const
    {
        return try_find_part_named( name, hardpoints_ );
    }

    const Softpoint* machineparts::get_softpoint( const std::string& name ) const
    {
        return try_find_part_named( name, softpoints_ );
    }

    const Chassis* machineparts::get_chassis( const std::string& name ) const
    {
        return try_find_part_named( name, chassis_ );
    }


}
