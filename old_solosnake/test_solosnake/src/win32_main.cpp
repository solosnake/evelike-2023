#define WIN32_LEAN_AND_MEAN
#include "solosnake/win32/includewindows.hpp"
#include "solosnake/logging.hpp"
#include "solosnake/throw.hpp"
#include "solosnake/unicode_to_utf8.hpp"
#include <stdlib.h>
#include <crtdbg.h>
#include <shellapi.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cstdio>
#include "solosnake/testing/testing.hpp"
#include <iostream>
#include <fstream>

//
// TO RUN ONE TEST USE:
// --gtest_filter=*TESTNAME*
//

#pragma comment(lib, "opengl32.lib") // link with Microsoft OpenGL lib
#pragma comment(lib, "glu32.lib")    // link with Microsoft OpenGL Utility lib

typedef std::pair<std::vector<char*>, std::vector<std::string>> args_t;

// Returns two arrays, one pointing into the other simulating a
// the char* argv[] of CRT main.
std::shared_ptr<args_t> make_args()
{
    using namespace solosnake;
    std::shared_ptr<args_t> result = std::make_shared<args_t>();

    int nArgs;
    LPWSTR* szArglist = ::CommandLineToArgvW(::GetCommandLineW(), &nArgs);

    if (NULL == szArglist)
    {
        ss_werr(L"CommandLineToArgvW failed.");
        ss_throw("CommandLineToArgvW failed.");
    }
    else
    {
        result->second.reserve(nArgs);

        for (int i = 0; i < nArgs; i++)
        {
            result->second.push_back(unicode_to_utf8(std::wstring(szArglist[i])));
        }
    }

    ::LocalFree(szArglist);

    result->first.reserve(result->second.size());

    for (size_t i = 0; i < result->second.size(); i++)
    {
        result->first.push_back(&result->second[i][0]);
    }

    return result;
}

/**
* Called from main and encapsulates all the application startup into a single
* scope.
*/
int run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    auto args = make_args();
    int argc = static_cast<int>(args->first.size());
    char** argv = &args->first.at(0);

    testing::InitGoogleTest(&argc, argv);
    int result = RUN_ALL_TESTS();

    return result;
}

/**
* Application entry point on Microsoft Windows.
*/
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    return run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    ;
}
