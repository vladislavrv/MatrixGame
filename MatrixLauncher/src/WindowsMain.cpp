// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// WindowsMain.cpp : Windows application entrypoint

#include <windows.h>
#include "CWindowsApplication.h"

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int) {
    const wchar_t *cmd = GetCommandLineW();

    int numarg;
    wchar_t **args = CommandLineToArgvW(cmd, &numarg);

    CWindowsApplication app(hInstance);

    if (numarg > 1) {
        app.StartLocalGame(args[1]);
    }
    else {
        app.StartLauncher();
    }

    return 0;
}