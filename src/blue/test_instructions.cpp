#include "solosnake/testing/testing.hpp"
#include "solosnake/blue/blue_instructions.hpp"
#include "solosnake/blue/blue_compiler.hpp"
#include "solosnake/blue/blue_to_from_json.hpp"

using namespace blue;

TEST( Instructions, Instructions )
{
    std::vector<Instruction> code;
    code.push_back( blue::compile( "[2] = get_sense_result_count()" ) );
    code.push_back( blue::compile( "[5] = can_fire_at([3])" ) );
    Instructions x(code);

    nlohmann::json j = x;
    auto x2 = j.get<Instructions>();
    EXPECT_EQ( x, x2 );
}
