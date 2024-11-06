#ifndef blue_explosion_damages_hpp
#define blue_explosion_damages_hpp

namespace blue
{
    class Machine;
    struct Damages;
    struct Machine_exploded;

    /// Returns the damage caused by this explosion type.
    Damages get_explosion_damages( const Machine_exploded& );

    /// Returns an explosion whose power is based on the Hardware of a machine,
    /// the amount of cap it has remaining, and the size of the machine. The
    /// type is based on the main power source of the machine.
    Machine_exploded get_machines_explosive_power( const Machine& ) noexcept;
}

#endif // blue_explosion_damages_hpp
