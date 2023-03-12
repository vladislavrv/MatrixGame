// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsApplication.h : Defines the entry point for the application on windows.

#pragma once

#include "CApplication.h"

#include "MatrixGameDll.hpp"
#include "d3d9.h"
#include "CSettings.h"

class CWindowsApplication : CApplication {
public:
    CWindowsApplication(HINSTANCE hInstance);
    ~CWindowsApplication();
    
    void StartLauncher() override;
    void StartLocalGame(wchar_t *wcBattleMap) override;

private:
    void InitD3D(HWND hWnd, uint32_t w, uint32_t h, bool windowed, IDirect3D9 *&FDirect3D, IDirect3DDevice9 *&D3DDevice);

private:
    CSettings m_settings;
    HINSTANCE m_hInstance;
    int m_nCmdShow;
};