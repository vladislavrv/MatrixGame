// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsApplication.cpp : Defines the entry point for the application on windows.

#include "CWindowsApplication.h"
#include "CWindowsLauncherUI.h"

#include <exception>

CWindowsApplication::CWindowsApplication(HINSTANCE hInstance) {
    m_hInstance = hInstance;
    m_settings.ResetSettings();
}

CWindowsApplication::~CWindowsApplication() {}

void CWindowsApplication::StartLauncher() {
    CWindowsLauncherUI mainMenu = CWindowsLauncherUI(&m_settings, this);
    mainMenu.Run();
}

void CWindowsApplication::StartLocalGame(wchar_t* map) {
    SMatrixSettings *matrixSettings = m_settings.GetMatrixGameSettings();

    SRobotGameState robotGameState{0};

    SMatrixTextParams texts{};
    texts.lossText = L"Fail";
    texts.winText = L"Win";
    texts.planetName = L"Minsk";
    
    // Takes controll
    RunStandalone(m_hInstance, map, matrixSettings, &texts, &robotGameState);
}
