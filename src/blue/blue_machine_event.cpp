#include <cstdio>
#include "solosnake/blue/blue_machine_event.hpp"
#include "solosnake/blue/blue_machine.hpp"

#ifdef _MSC_VER
#   define SNPRINTF _snprintf
#else
#   define SNPRINTF snprintf
#endif

namespace blue
{
    namespace
    {
        bool same_machines(const Machine* lhs, const Machine* rhs) noexcept
        {
            if(rhs == lhs)
            {
                // Same machine pointer, or both null.
                return true;
            }

            if(lhs && rhs)
            {
                // Both non-null and same names.
                return lhs->name() == rhs->name();
            }

            // One is not null, one is null.
            return false;
        }

        bool same_event_contents(const std::uint32_t event_id,
                                 const Machine_event::EventDatum& ,
                                 const Machine_event::EventDatum& ) noexcept
        {
            ss_throw("Not correctly implemented");

            switch(event_id)
            {
                case ENotAnEvent:
                    return true;
                case EMachineAddedToBoard:
                    return true;
                case EMachinePositionalInfoUpdate:
                    return true;
                case EMachineChangeTile:
                    return true;
                case EMachineSensed:
                    return true;
                case EMachineFired:
                    return true;
                case EMachineBroadcasted:
                    return true;
                case EMachineTransmitted:
                    return true;
                case EMachineInitiatedSelfDestruct:
                    return true;
                case EMachineExploded:
                    return true;
                case EMachineError:
                    return true;
                case EMachineAsserted:
                    return true;
                case EMachinePrintText:
                    return true;
                case EMachinePrintInstruction:
                    return true;
                case EMachineStartedBuild:
                    return true;
                case EMachineCompletedBuild:
                    return true;
                case EMachineTransferredCargo:
                    return true;
                case EMachineReceivedDamage:
                    return true;
                case EMachineReceivedRepairs:
                    return true;
                case EMachineCapAltered:
                    return true;
                case EMachineTransferCash:
                    return true;
                default:
                return false;
            }

            return false;
        }

    } // End unnnamed namespace

    Machine_event Machine_event::machine_text( Machine& m,
                                               std::uint16_t n ) noexcept
    {
        Machine_event e( EMachinePrintText, m );
        SNPRINTF( e.event_.text_.io_, sizeof( e.event_.text_.io_ ), "%u", n );
        return e;
    }

    Machine_event Machine_event::machine_transmitted( Machine& m,
                                                      std::uint16_t transmissionId,
                                                      Hex_coord transmitTo ) noexcept
    {
        Machine_event e( EMachineTransmitted, m );
        e.event_.transmitted_.transmissionId_ = transmissionId;
        e.event_.transmitted_.transmitFromXY_ = m.board_xy();
        e.event_.transmitted_.transmitToXY_   = transmitTo;
        return e;
    }

    Machine_event Machine_event::machine_exploded( Machine& m ) noexcept
    {
        Machine_event e( EMachineExploded, m );
        e.event_.machine_exploded_ = m.get_explosion();
        return e;
    }

    Machine_event Machine_event::machine_sensed( Machine& m,
                                                 std::uint16_t senseRadius,
                                                 std::uint16_t senseDurationInTicks ) noexcept
    {
        Machine_event e( EMachineSensed, m );
        e.event_.sensed_.radius_            = senseRadius;
        e.event_.sensed_.duration_in_ticks_ = senseDurationInTicks;
        e.event_.sensed_.gridXY_            = m.board_xy();
        return e;
    }

    Machine_event Machine_event::machine_fired( Machine& m,
                                                const Hardpoint_shot& s,
                                                Hex_coord targetXY ) noexcept
    {
        Machine_event e( EMachineFired, m );
        e.event_.fired_.shot_     = s;
        e.event_.fired_.firerXY_  = m.board_xy();
        e.event_.fired_.targetXY_ = targetXY;
        return e;
    }

    Machine_event Machine_event::machine_broadcasted( Machine& m,
                                                      std::uint16_t broadcastId,
                                                      std::uint16_t broadcastRange ) noexcept
    {
        Machine_event e( EMachineBroadcasted, m );
        e.event_.broadcasted_.broadcastId_     = broadcastId;
        e.event_.broadcasted_.broadcastFromXY_ = m.board_xy();
        e.event_.broadcasted_.broadcastRadius_ = broadcastRange;
        return e;
    }

    Machine_event Machine_event::machine_positional_update( Machine& m ) noexcept
    {
        Machine_event e( EMachinePositionalInfoUpdate, m );
        e.event_.positional_update_.machine_position_info_ = m.get_position_info();
        return e;
    }

    bool Machine_event::operator == (const Machine_event& rhs) const noexcept
    {
        return (event_id_ == rhs.event_id_)
            && same_machines(machine_, rhs.machine_)
            && same_event_contents(event_id_, event_, rhs.event_)
            ;
    }
}