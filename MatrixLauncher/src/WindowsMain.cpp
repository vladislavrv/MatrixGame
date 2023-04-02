// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// WindowsMain.cpp : Windows application entrypoint

#include <windows.h>
#include "WindowsApplication.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInsatnce, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd) {
    const wchar_t *cmd = GetCommandLine();

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