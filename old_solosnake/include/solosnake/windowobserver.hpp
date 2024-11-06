#ifndef solosnake_windowobserver_hpp
#define solosnake_windowobserver_hpp

#include "solosnake/observer.hpp"

namespace solosnake
{
    class windowchange;
    class window;

    typedef observer<windowchange, window> windowobserver;
    typedef callback<windowchange, window> windowcallback;
}

#endif
