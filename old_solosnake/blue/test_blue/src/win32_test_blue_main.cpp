#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <shellapi.h>
#include <fcntl.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cstdio>
#include "solosnake/testing/testing.hpp"

//! Called from main and encapsulates all the application startup into a single
// scope.
int run(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    int result = 0;
    return result;
}

//! Application entry point on Microsoft Windows.
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
    return run(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
}
