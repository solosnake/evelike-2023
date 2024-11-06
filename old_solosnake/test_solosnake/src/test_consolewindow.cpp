#define RUN_TESTS

#ifdef RUN_TESTS

#include "solosnake/testing/testing.hpp"
#include <iostream>
#include "solosnake/console_window.hpp"

TEST(console_window, outputs)
{
    {
        solosnake::console_window wndw(_TEXT("Test Console Window"));

        std::cout << "Hello World cout.\n";
        std::clog << "Hello World clog.\n";
        std::cerr << "Hello World cerr.\n";

        std::wcout << L"Hello World cout.\n";
        std::wclog << L"Hello World clog.\n";
        std::wcerr << L"Hello World cerr.\n";
    }
}

#endif
