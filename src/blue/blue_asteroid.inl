#include "solosnake/blue/blue_asteroid.hpp"
#include <cassert>

namespace blue
{
  inline std::uint16_t Asteroid::volume() const noexcept
  {
    return remaining_volume_;
  }

  inline OreTypes Asteroid::ore_type() const noexcept
  {
    return static_cast<OreTypes>( ore_type_ );
  }

  inline Hex_coord Asteroid::board_xy() const noexcept
  {
    return board_xy_;
  }

  inline bool Asteroid::is_on_board() const noexcept
  {
    return on_board_;
  }

  inline void Asteroid::set_as_removed_from_board() noexcept
  {
    assert( on_board_ );
    on_board_ = false;
  }
}
