#include "solosnake/blue/blue_logging.hpp"
#include "solosnake/blue/blue_throw.hpp"
#include "solosnake/blue/blue_machine_readout.hpp"

namespace blue
{
    std::string_view machine_readout_to_string( const std::uint16_t n )
    {
        switch( n )
        {
            case ReadoutOne:
                return "One";
            case ReadoutCPUBenchmark:
                return "CPUBenchmark";
            case Pulse0:
                return "Pulse0";
            case Pulse1:
                return "Pulse1";
            case ReadoutAdvanceSpeedLvl:
                return "AdvanceSpeedLvl";
            case ReadoutTurnSpeedLvl:
                return "TurnSpeedLvl";
            case ReadoutCapacitorLvl:
                return "CapacitorLvl";
            case ReadoutSensingLvl:
                return "SensingLvl";
            case ReadoutBroadcastingLvl:
                return "BroadcastingLvl";
            case ReadoutTransmittingLvl:
                return "TransmittingLvl";
            case ReadoutBuildingLvl:
                return "BuildingLvl";
            default:
                break;
        }

        ss_err( "Unknown readout name enum:", n );
        ss_throw( "Unknown Machine readout enum when writing Component." );
    }


    Machine_readout string_to_readout( const std::string_view& txt )
    {
        if( 0 == txt.compare( "One" ) || 0 == txt.compare( "1" ) || 0 == txt.compare( "1.0" ) )
        {
            return ReadoutOne;
        }

        if( 0 == txt.compare( "Pulse0" ) )
        {
            return Pulse0;
        }

        if( 0 == txt.compare( "Pulse1" ) )
        {
            return Pulse1;
        }

        if( 0 == txt.compare( "AdvanceSpeedLvl" ) )
        {
            return ReadoutAdvanceSpeedLvl;
        }

        if( 0 == txt.compare( "TurnSpeedLvl" ) )
        {
            return ReadoutTurnSpeedLvl;
        }

        if( 0 == txt.compare( "CapacitorLvl" ) )
        {
            return ReadoutCapacitorLvl;
        }

        if( 0 == txt.compare( "CPUBenchmark" ) )
        {
            return ReadoutCPUBenchmark;
        }

        if( 0 == txt.compare( "SensingLvl" ) )
        {
            return ReadoutSensingLvl;
        }

        if( 0 == txt.compare( "BroadcastingLvl" ) )
        {
            return ReadoutBroadcastingLvl;
        }

        if( 0 == txt.compare( "TransmittingLvl" ) )
        {
            return ReadoutTransmittingLvl;
        }

        if( 0 == txt.compare( "BuildingLvl" ) )
        {
            return ReadoutBuildingLvl;
        }

        ss_err( "Unknown readout name :", txt );
        ss_throw( "Unknown Machine readout name when constructing Component." );
    }
}
