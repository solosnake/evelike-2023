#ifndef blue_machine_events_hpp
#define blue_machine_events_hpp

#include <cassert>
#include <cstdint>
#include "solosnake/blue/blue_hardpoint_shot.hpp"
#include "solosnake/blue/blue_hex_coord.hpp"
#include "solosnake/blue/blue_position_info.hpp"

namespace blue
{
    enum MachineEventId
    {
        ENotAnEvent                     ,   ///< The zero valued event is reserved and invalid.
        EMachineAddedToBoard            ,   ///< Called when a Machine is added to the board.
        EMachinePositionalInfoUpdate    ,   ///< Called when position and/or speeds change.
        EMachineChangeTile              ,   ///< Called when Machine moves completely from one tile to another.
        EMachineSensed                  ,   ///< Called when a Machine does starts a sensing of its surroundings.
        EMachineFired                   ,   ///< Called when a Machine fires a Hardpoint (may not be offensive)
        EMachineBroadcasted             ,   ///< Called when a Machine broadcasts.
        EMachineTransmitted             ,   ///< Called when a Machine transmits.
        EMachineInitiatedSelfDestruct   ,   ///< Called with a countdown timer to destroy a Machine.
        EMachineExploded                ,   ///< Called when a Machine is destroyed, accompanied by an explosion.
        EMachineError                   ,   ///< Machine has encountered an error.
        EMachineAsserted                ,   ///< Machine has encountered an error.
        EMachinePrintText               ,   ///< Machine is printing a text message.
        EMachinePrintInstruction        ,   ///< Machine is printing an Instruction.
        EMachineStartedBuild            ,   ///< Machine has begun building another Machine.
        EMachineCompletedBuild          ,   ///< Machine has completed building another Machine.
        EMachineTransferredCargo        ,   ///< Machine tries to give or take cargo.
        EMachineReceivedDamage          ,   ///< Machine has received some damage.
        EMachineReceivedRepairs         ,   ///< Machine has received some armour repairs.
        EMachineCapAltered              ,   ///< Machine has had some cap given or taken by an external agent.
        EMachineTransferCash                ///< Machine is trying to give cash to Machine on another tile.
    };

    /// @defgroup event_datums Event Datums
    /// These are POD structs containing information updates about an event,
    /// sent out as the contents of the event notification.
    /// @{

    struct Machine_added_to_board
    {
        Hex_coord               locationXY_;
    };

    struct Positional_update
    {
        Position_info           machine_position_info_; // ~ 3 x int32 + int16 = ~14 bytes.
    };

    struct Changed_tile
    {
        Hex_coord               oldTile_;
        Hex_coord               newTile_;
    };

    struct Sensed
    {
        std::uint16_t           radius_;
        std::uint16_t           duration_in_ticks_;
        Hex_coord               gridXY_;
    };

    struct Fired
    {
        Hardpoint_shot          shot_;
        Hex_coord               firerXY_;
        Hex_coord               targetXY_;
    };

    struct Broadcasted
    {
        Hex_coord               broadcastFromXY_;
        std::uint16_t           broadcastId_;
        std::uint16_t           broadcastRadius_;
    };

    struct Transmitted
    {
        std::uint16_t           transmissionId_;
        Hex_coord               transmitFromXY_;
        Hex_coord               transmitToXY_;
    };

    struct Self_destructed
    {
        std::uint16_t           countdown_;
    };

    struct Machine_exploded
    {
        std::uint16_t           explosionPercentNuclear; // What proportion of the blast is nuclear.
        std::uint16_t           explosionRange;          // How far beyond the Machine's xy it extends.
        std::uint16_t           explosionDamage;         // How much damage it does in total at distance 1.
        Hex_coord               gridXY_;
    };

    struct Assert_failed
    {
        std::uint16_t           errorLine;
        std::uint16_t           op1;
        std::uint16_t           op2;
        std::uint16_t           operand;
    };

    struct IO_error
    {
        std::uint16_t           errorLine;
        std::uint8_t            errorType; // See Machine::ErrorType.
    };

    struct Instruction_codes
    {
        std::uint16_t           cmd_;
        std::uint16_t           args_[3];
    };

    struct IO_text
    {
        char                    io_[12];
    };

    struct Build_started
    {
        Hex_coord               buildSite;
        std::uint16_t           blueprintIndex;
    };

    struct Build_completed
    {
        Hex_coord               buildSite;
        std::uint16_t           blueprintIndex;
        std::uint16_t           codeIndex;
    };

    struct Cargo_transferred
    {
        Hex_coord               takeFrom;
        Hex_coord               giveTo;
        std::uint16_t           tradableType;
        std::uint16_t           units;
    };

    struct Cash_transfer
    {
        Hex_coord               giveTo;
        std::uint16_t           amount;
    };

    /// @}
}

#endif // blue_machine_events_hpp
