
#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_hexcontentflags.hpp"
#include "solosnake/blue/blue_hex_grid.hpp"
#include "solosnake/blue/blue_game_hex_grid.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Game_hex_grid, to_from_json )
{
    Hex_grid grid(5, 4, blue::empty_tile_value(), blue::off_board_tile_value() );
    Game_hex_grid gamegrid(std::move(grid));

    nlohmann::json j = gamegrid;
    auto gamegrid2 = j.get<Game_hex_grid>();
    EXPECT_EQ( gamegrid, gamegrid2 );
}
