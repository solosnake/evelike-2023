#ifndef blue_components_hpp
#define blue_components_hpp

#include <memory>
#include <vector>
#include "solosnake/blue/blue_chassis.hpp"
#include "solosnake/blue/blue_softpoint.hpp"
#include "solosnake/blue/blue_hardpoint.hpp"
#include "solosnake/blue/blue_thruster.hpp"

namespace blue
{
    class datapaths;

    /// This is a single collection of all the parts of a Machine (Chassis,
    /// hardpoints, softpoints, thrusters etc) available to the game. These
    /// are loaded via the manifest files in the data paths.
    class machineparts
    {
    public:

        machineparts( const datapaths& );

        size_t chassis_count() const;

        size_t softpoint_count() const;

        size_t hardpoint_count() const;

        size_t thruster_count() const;

        const Chassis& get_chassis(const size_t n) const;

        const Softpoint& get_softpoint( const size_t n ) const;

        const Hardpoint& get_hardpoint( const size_t n ) const;

        const Thruster& get_thruster( const size_t n ) const;

        const Chassis* get_chassis(const std::string&) const;

        const Softpoint* get_softpoint( const std::string& ) const;

        const Hardpoint* get_hardpoint( const std::string& ) const;

        const Thruster* get_thruster( const std::string& ) const;

    private:

        std::vector<Chassis>    chassis_;
        std::vector<Softpoint>  softpoints_;
        std::vector<Hardpoint>  hardpoints_;
        std::vector<Thruster>   thrusters_;
    };

    //-------------------------------------------------------------------------

    inline size_t machineparts::chassis_count() const
    {
        return chassis_.size();
    }

    inline size_t machineparts::softpoint_count() const
    {
        return softpoints_.size();
    }

    inline size_t machineparts::hardpoint_count() const
    {
        return hardpoints_.size();
    }

    inline size_t machineparts::thruster_count() const
    {
        return thrusters_.size();
    }

    inline const Chassis& machineparts::get_chassis(const size_t n) const
    {
        return chassis_[n];
    }

    inline const Softpoint& machineparts::get_softpoint( const size_t n ) const
    {
        return softpoints_[n];
    }

    inline const Hardpoint& machineparts::get_hardpoint( const size_t n ) const
    {
        return hardpoints_[n];
    }

    inline const Thruster& machineparts::get_thruster( const size_t n ) const
    {
        return thrusters_[n];
    }
}

#endif
