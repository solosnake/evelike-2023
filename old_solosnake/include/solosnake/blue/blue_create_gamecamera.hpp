#ifndef blue_create_gamecamera_hpp
#define blue_create_gamecamera_hpp

#include <memory>

namespace blue
{
    class Hex_grid;
    class iboardcamera;
    class user_settings;

    //! Creates a camera which works inside a box containing the grid.
    std::unique_ptr<iboardcamera> create_tiltcamera( const float cameraArmlength,
                                                     const float cameraTilt,
                                                     const float cameraRotation,
                                                     const float cameraRadius,
                                                     const float cameraCeilingHeight,
                                                     const float cameraZoomFactor,
                                                     const Hex_grid& grid );

    //! Reads the values from the user_settings and uses them to call the other create_camera
    //! call.
    std::unique_ptr<iboardcamera> create_tiltcamera( const Hex_grid& grid,
                                                     const user_settings& );
}

#endif
