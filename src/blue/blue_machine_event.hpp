#ifndef blue_machineevent_hpp
#define blue_machineevent_hpp

#include <array>
#include <cassert>
#include <cstdint>
#include "solosnake/blue/blue_instruction.hpp"
#include "solosnake/blue/blue_machine_events.hpp"

namespace blue
{
    /// Forward declare the machine class: only used as pointer in this file.
    class Machine;


    /// Simple class representing the information about all types of events.
    /// Machines use this to notify listeners of changes to their state.
    /// The board uses these to notify listeners of game changes.
    class Machine_event
    {
    public:

        /// A union of possible events, only one is ever valid, matching
        /// the EventType.
        union EventDatum
        {
            Machine_added_to_board  machine_added_to_board_;
            Positional_update       positional_update_;
            Changed_tile            changed_tile_;
            Sensed                  sensed_;
            Fired                   fired_;
            Broadcasted             broadcasted_;
            Transmitted             transmitted_;
            Self_destructed         self_destructed_;
            Machine_exploded        machine_exploded_;
            IO_text                 text_;
            IO_error                error_;
            Assert_failed           asserted_;
            Instruction_codes       instruction_;
            Build_started           build_started_;
            Build_completed         build_completed_;
            Cargo_transferred       cargo_transferred_;
            Damages                 damage_;
            Cash_transfer           cash_transfer_;
            int                     armour_repairs_;
            int                     cap_delta_;
        };

        std::uint32_t              event_id() const noexcept;

        Machine*                   machine() noexcept;

        Machine*                   machine() const noexcept;

        const EventDatum&          event() const noexcept;

        static Machine_event       machine_added_to_board( Machine& ) noexcept;

        static Machine_event       machine_positional_update( Machine& ) noexcept;

        static Machine_event       machine_change_tile( Machine&, Hex_coord oldTile, Hex_coord newTile ) noexcept;

        static Machine_event       machine_sensed( Machine&, std::uint16_t senseRadius, std::uint16_t senseDurationInTicks ) noexcept;

        static Machine_event       machine_fired( Machine&, const Hardpoint_shot&, Hex_coord targetXY ) noexcept;

        static Machine_event       machine_broadcasted( Machine&, std::uint16_t broadcastId, std::uint16_t broadcastRange ) noexcept;

        static Machine_event       machine_transmitted( Machine&, std::uint16_t transmissionId, Hex_coord transmitTo ) noexcept;

        static Machine_event       machine_initiated_self_destruct( Machine&, std::uint16_t countdown ) noexcept;

        static Machine_event       machine_exploded( Machine& ) noexcept;

        static Machine_event       machine_text( Machine&, const std::array<char,6>& text ) noexcept;

        static Machine_event       machine_text( Machine&, std::uint16_t ) noexcept;

        static Machine_event       machine_print_instruction( Machine&, Instruction ) noexcept;

        static Machine_event       machine_error( Machine&, std::uint16_t line, std::uint8_t errorType ) noexcept;

        static Machine_event       machine_asserted( Machine&, std::uint16_t line, std::uint16_t op1, std::uint16_t op2, std::uint16_t operand ) noexcept;

        static Machine_event       machine_started_build( Machine&, Hex_coord buildSite, std::uint16_t blueprintIndex ) noexcept;

        static Machine_event       machine_completed_build( Machine&, Hex_coord buildSite, std::uint16_t blueprintIndex, std::uint16_t codeIndex ) noexcept;

        static Machine_event       machine_cargo_xfer( Machine&, Hex_coord from, Hex_coord to, std::uint16_t cargoType, std::uint16_t units ) noexcept;

        static Machine_event       machine_received_damage( Machine&, const Damages& ) noexcept;

        static Machine_event       machine_received_armour_rep( Machine&, int repairs ) noexcept;

        static Machine_event       machine_cap_was_altered( Machine&, int delta ) noexcept;

        static Machine_event       machine_transfers_credits( Machine&, Hex_coord to, std::uint16_t cash ) noexcept;

        bool operator == (const Machine_event& rhs) const noexcept;

    private:

        Machine_event( MachineEventId e, Machine& m ) noexcept;

    private:

        std::uint32_t   event_id_;  ///< An MachineEventId.
        Machine*        machine_;
        EventDatum      event_;
    };
}

#include "solosnake/blue/blue_machine_event.inl"
#endif
