#include "solosnake/x11/window_opengl.hpp"
#include <exception>
#include <iostream>

int main(int argc, char* argv[])
{
    try
    {
        using namespace solosnake;
        x11windowopengl w("My 1st X Window");
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what();
        return -1;
    }
}
