#ifndef blue_start_hpp
#define blue_start_hpp

#include <memory>

namespace blue
{
    class user_settings;

    int start( const std::shared_ptr<user_settings>& );
}

#endif
