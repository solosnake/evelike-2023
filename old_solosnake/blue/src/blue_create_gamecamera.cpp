#include "solosnake/blue/blue_gamecamera.hpp"
#include "solosnake/blue/blue_tiltcamera.hpp"
#include "solosnake/blue/blue_create_gamecamera.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_settings.hpp"
#include "solosnake/blue/blue_user_settings.hpp"
#include "solosnake/box.hpp"
#include "solosnake/dimension.hpp"
#include "solosnake/point.hpp"
#include "solosnake/rect.hpp"
#include <utility>

// How many times larger than the board the bounding box is.
#define BLUE_BOUNDINGBOX_FACTOR   (3.0f)

using namespace solosnake;

namespace blue
{
    std::unique_ptr<iboardcamera> create_tiltcamera( const float cameraArmlength,
                                                     const float cameraTilt,
                                                     const float cameraRotation,
                                                     const float cameraRadius,
                                                     const float cameraCeilingHeight,
                                                     const float cameraZoomFactor,
                                                     const hexgrid& grid )
    {
        assert( cameraCeilingHeight > cameraRadius );
        assert( cameraRadius > 0.0f );

        const rectf area       = grid.surface_rect();
        const auto boardCentre = area.centrepoint();
        const float boxWidth   = 2.0f * cameraRadius + ( BLUE_BOUNDINGBOX_FACTOR * area.width() );
        const float boxLength  = 2.0f * cameraRadius + ( BLUE_BOUNDINGBOX_FACTOR * area.height() );

        const point3d boundingCorner( boardCentre.xy[0] - ( 0.5f * boxWidth ),
                                      BLUE_BOARD_Y,
                                      boardCentre.xy[1] - ( 0.5f * boxLength ) );

        const dimension3df boundingRectSize( boxWidth, cameraCeilingHeight + cameraRadius, boxLength );

        const box boundingBox( boundingCorner, boundingRectSize );

        const float largestDim = std::max( std::max( boundingRectSize.width(), boundingRectSize.height() ),
                                           boundingRectSize.depth() );

        const float boundingSphereRadius = largestDim;

        point3d boxCentre = boundingBox.centre();
        point3d boundingSphereCentre( boxCentre.xyz[0], BLUE_BOARD_Y, boxCentre.xyz[2] );

        auto cam = std::make_unique<tiltcamera>( BLUE_BOARD_Y,
                                                 cameraRadius,
                                                 std::min( cameraArmlength, boundingSphereRadius ),
                                                 0.2f * boundingSphereRadius,
                                                 boundingSphereCentre.xyz[0],
                                                 boundingSphereCentre.xyz[2],
                                                 HALFPI_RADIANS * 0.125,
                                                 HALFPI_RADIANS * 0.99f,
                                                 radians( cameraRotation ),
                                                 sphere( boundingSphereCentre, boundingSphereRadius ),
                                                 cameraZoomFactor );


        // Rotate by 90 because we prefer to be aligned with the
        // grids x and y row/column directions:
        cam->rotate( HALFPI_RADIANS );

        return std::move( cam );
    }


    std::unique_ptr<iboardcamera> create_tiltcamera( const hexgrid& grid,
                                                     const user_settings& settings )
    {
        // Create good viewing matrix.
        const float camballRadius    = settings.value( BLUE_GAME_CAMERA_RADIUS ).as_float();
        const float camballCeiling   = settings.value( BLUE_GAME_CAMERA_CEILING_HEIGHT ).as_float();
        const float camballTilt      = settings.value( BLUE_GAME_CAMERA_TILT ).as_float();
        const float camballRotate    = settings.value( BLUE_GAME_CAMERA_ROTATION ).as_float();
        const float camballArmlength = settings.value( BLUE_GAME_CAMERA_ARMLENGTH ).as_float();
        const float cameraZoomFactor = settings.value( BLUE_GAME_CAMERA_ZOOMFACTOR ).as_float();

        return create_tiltcamera( camballArmlength,
                                  camballTilt,
                                  camballRotate,
                                  camballRadius,
                                  camballCeiling,
                                  cameraZoomFactor,
                                  grid );
    }
}
