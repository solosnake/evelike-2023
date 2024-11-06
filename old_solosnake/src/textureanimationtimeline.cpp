#include "solosnake/textureanimationtimeline.hpp"

using namespace std;

namespace solosnake
{
    textureanimationtimeline::textureanimationtimeline()
    {
    }

    textureanimationtimeline::textureanimationtimeline( const vector<uint8_t>& values )
        : timeline_values_( values )
    {
    }
}
