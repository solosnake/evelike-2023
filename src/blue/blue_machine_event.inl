#include "solosnake/blue/blue_machine_event.hpp"

namespace blue
{
    inline Machine_event::Machine_event( MachineEventId e, Machine& m ) noexcept
        : event_id_( e )
        , machine_( &m )
    {
        assert( machine_ );
    }

    inline std::uint32_t Machine_event::event_id() const noexcept
    {
        return event_id_;
    }

    inline Machine* Machine_event::machine() noexcept
    {
        return machine_;
    }

    inline Machine* Machine_event::machine() const noexcept
    {
        return machine_;
    }

    inline const Machine_event::EventDatum&  Machine_event::event() const noexcept
    {
        return event_;
    }

    inline Machine_event Machine_event::machine_added_to_board( Machine& m ) noexcept
    {
        return Machine_event( EMachineAddedToBoard, m );
    }

    inline Machine_event Machine_event::machine_change_tile( Machine& m,
                                                             Hex_coord oldTile,
                                                             Hex_coord newTile ) noexcept
    {
        Machine_event e( EMachineChangeTile, m );
        e.event_.changed_tile_.oldTile_ = oldTile;
        e.event_.changed_tile_.newTile_ = newTile;
        return e;
    }

    inline Machine_event Machine_event::machine_initiated_self_destruct( Machine& m,
                                                                         std::uint16_t countdown ) noexcept
    {
        Machine_event e( EMachineInitiatedSelfDestruct, m );
        e.event_.self_destructed_.countdown_ = countdown;
        return e;
    }

    inline Machine_event Machine_event::machine_text( Machine& m,
                                                      const std::array<char,6>& txt ) noexcept
    {
        Machine_event e( EMachinePrintText, m );
        e.event_.text_.io_[0]  = txt[0];
        e.event_.text_.io_[1]  = txt[1];
        e.event_.text_.io_[2]  = txt[2];
        e.event_.text_.io_[3]  = txt[3];
        e.event_.text_.io_[4]  = txt[4];
        e.event_.text_.io_[5]  = txt[5];
        e.event_.text_.io_[6]  = char( 0 );
        e.event_.text_.io_[7]  = char( 0 );
        e.event_.text_.io_[8]  = char( 0 );
        e.event_.text_.io_[9]  = char( 0 );
        e.event_.text_.io_[10] = char( 0 );
        e.event_.text_.io_[11] = char( 0 );
        return e;
    }

    inline Machine_event Machine_event::machine_print_instruction( Machine& m,
                                                                   Instruction i ) noexcept
    {
        Machine_event e( EMachinePrintInstruction, m );
        e.event_.instruction_.cmd_     = i[0];
        e.event_.instruction_.args_[0] = i[1];
        e.event_.instruction_.args_[1] = i[2];
        e.event_.instruction_.args_[2] = i[3];
        return e;
    }

    inline Machine_event Machine_event::machine_error( Machine& m,
                                                       std::uint16_t line,
                                                       std::uint8_t errorType ) noexcept
    {
        Machine_event e( EMachineError, m );
        e.event_.error_.errorLine = line;
        e.event_.error_.errorType = errorType;
        return e;
    }

    inline Machine_event Machine_event::machine_asserted( Machine& m,
                                                          std::uint16_t line,
                                                          std::uint16_t op1,
                                                          std::uint16_t op2,
                                                          std::uint16_t operand ) noexcept
    {
        Machine_event e( EMachineAsserted, m );
        e.event_.asserted_.errorLine = line;
        e.event_.asserted_.op1       = op1;
        e.event_.asserted_.op2       = op2;
        e.event_.asserted_.operand   = operand;
        return e;
    }

    inline Machine_event Machine_event::machine_started_build( Machine& m,
                                                               Hex_coord buildSite,
                                                               std::uint16_t blueprintIndex ) noexcept
    {
        Machine_event e( EMachineStartedBuild, m );
        e.event_.build_started_.buildSite      = buildSite;
        e.event_.build_started_.blueprintIndex = blueprintIndex;
        return e;
    }

    inline Machine_event Machine_event::machine_completed_build( Machine& m,
                                                                 Hex_coord buildSite,
                                                                 std::uint16_t blueprintIndex,
                                                                 std::uint16_t codeIndex ) noexcept
    {
        Machine_event e( EMachineCompletedBuild, m );
        e.event_.build_completed_.buildSite      = buildSite;
        e.event_.build_completed_.blueprintIndex = blueprintIndex;
        e.event_.build_completed_.codeIndex      = codeIndex;
        return e;
    }

    inline Machine_event Machine_event::machine_cargo_xfer( Machine& m,
                                                            Hex_coord from,
                                                            Hex_coord to,
                                                            std::uint16_t cargoType,
                                                            std::uint16_t units ) noexcept
    {
        Machine_event e( EMachineTransferredCargo, m );
        e.event_.cargo_transferred_.takeFrom     = from;
        e.event_.cargo_transferred_.giveTo       = to;
        e.event_.cargo_transferred_.tradableType = cargoType;
        e.event_.cargo_transferred_.units        = units;
        return e;
    }

    inline Machine_event Machine_event::machine_received_damage( Machine& m,
                                                                 const Damages& d ) noexcept
    {
        Machine_event e( EMachineReceivedDamage, m );
        e.event_.damage_ = d;
        return e;
    }

    inline Machine_event Machine_event::machine_received_armour_rep( Machine& m,
                                                                     int repairs ) noexcept
    {
        Machine_event e( EMachineReceivedRepairs, m );
        e.event_.armour_repairs_ = repairs;
        return e;
    }

    inline Machine_event Machine_event::machine_cap_was_altered( Machine& m,
                                                                 int delta ) noexcept
    {
        Machine_event e( EMachineCapAltered, m );
        e.event_.cap_delta_ = delta;
        return e;
    }

    inline  Machine_event Machine_event::machine_transfers_credits( Machine& m,
                                                                    Hex_coord to,
                                                                    std::uint16_t cash ) noexcept
    {
        Machine_event e( EMachineTransferCash, m );
        e.event_.cash_transfer_.giveTo = to;
        e.event_.cash_transfer_.amount = cash;
        return e;
    }
}