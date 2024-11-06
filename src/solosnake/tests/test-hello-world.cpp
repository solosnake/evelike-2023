#include "solosnake/testing/testing.hpp"
#include <iostream>

TEST( hello_world, hello_world )
{
    try
    {
        std::cout << "Hello World!\n";
    }
    catch(const std::exception& e)
    {
        ADD_FAILURE(e.what());
    }
}