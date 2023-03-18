// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsApplication.h : Defines the entry point for the application on windows.

#pragma once

#include "CApplication.h"

#include "MatrixGameDllEx.hpp"
#include "CSettings.h"

class CWindowsApplication : CApplication {
public:
    CWindowsApplication(HINSTANCE hInstance);
    ~CWindowsApplication();
    
    void StartLauncher() override;
    void StartLocalGame(wchar_t *wcBattleMap) override;

private:
    CSettings m_settings;
    HINSTANCE m_hInstance;
};