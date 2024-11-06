#include <cassert>
#include <memory>
#include "solosnake/modeldescription.hpp"

using namespace std;

namespace solosnake
{
    modeldescription::modeldescription( std::string&& modelname,
                                        std::vector<std::string>&& meshdependancies,
                                        modelnodedescription&& parentnode )
        : model_name_( move( modelname ) )
        , dependancies_( move( meshdependancies ) )
        , parentnode_( move( parentnode ) )
    {
        assert( !model_name_.empty() );
    }
}
