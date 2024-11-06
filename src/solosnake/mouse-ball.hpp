#ifndef SOLOSNAKE_MOUSEBALL_HPP
#define SOLOSNAKE_MOUSEBALL_HPP

#include <array>

namespace solosnake
{
    /// Class which creates a rotation matrix to match a mouse clicking on
    /// the screen and 'dragging' the scene as if it were contained in a ball
    /// centred at the screen midpoint, with a radius decided by the size of
    /// the screen.
    ///
    /// The click point and the drag-to point must lie within the window bounds.
    ///
    /// Assumes a window coordinate system where the top-left of the screen
    /// is (0,0).
    class Mouseball
    {
    public:

        /// Create a default Mouseball assuming a window 1024x768.
        Mouseball() = default;

        /// Set the correct window size.
        void set_window_size(unsigned int width, unsigned int height) noexcept;

        /// Set the window position to begin dragging at. Assumes a window
        /// coordinate system where the top-left of the screen is (0,0).
        /// Sets the `is_dragging` to true until `end_drag` is called.
        void start_drag(unsigned int x0,  unsigned int y0) noexcept;

        /// Calculate the matrix to rotate the scene such that it would appear
        /// to have been dragged by the click point to the current drag point.
        /// Returns an OpenGL compatible 4x4 rotation matrix. If the rotation
        /// matrix cannot be calculation, this will return the identity
        /// transform. If the Mouseball is not currently being dragged, this
        /// will also return the identity matrix.
        void drag_to(unsigned int x1, unsigned int y1) noexcept;

        /// Ends any current drag: `drag_to` will return the identity matrix
        /// now until a new drag is started.
        void end_drag() noexcept;

        /// Returns true if the Mouseball has a started drag.
        bool is_dragging() const noexcept;

        /// Returns the matrix to rotate the scene such that it would appear
        /// to have been dragged by the click point to the current drag point.
        /// Returns an OpenGL compatible 4x4 rotation matrix. If the rotation
        /// matrix cannot be calculation, this will return the identity
        /// transform. If the Mouseball is not currently being dragged, this
        /// will also return the identity matrix.
        /// The returned 4x4 3D rotation matrix has been ortho-normalised using
        /// the modifed Gram-Schmidt algorithm, and is safe to accumulate with
        /// other transforms.
        std::array<float, 16> get_drag_rotation() const noexcept;

    private:

        unsigned int    m_window_width      { 1024u };
        unsigned int    m_window_height     {  768u };
        unsigned int    m_click_x0          {    0u };
        unsigned int    m_click_y0          {    0u };
        unsigned int    m_dragged_x1        {    0u };
        unsigned int    m_dragged_y1        {    0u };
        bool            m_dragging          { false };
    };

//---------------------------------- inlines -----------------------------------

inline void Mouseball::end_drag() noexcept
{
    m_dragging = false;
}

inline bool Mouseball::is_dragging() const noexcept
{
    return m_dragging;
}

inline void Mouseball::drag_to(const unsigned int x1,
                               const unsigned int y1) noexcept
{
    m_dragged_x1 = x1;
    m_dragged_y1 = y1;
}

} // solosnake
#endif
