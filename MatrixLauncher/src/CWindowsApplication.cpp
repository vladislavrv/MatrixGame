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
    SRobotsSettings *robotSettings = m_settings.GetRobotGameSettings();

    SRobotGameState robotGameState{0};
    
    // Takes controll
    GetRobotInterface()->m_Run(m_hInstance, 0, map, robotSettings, nullptr,
                               (wchar_t*)L"Welcome messsage", (wchar_t*)L"You Win",
                               (wchar_t*)L"You loss", (wchar_t*)L"%PlanetName%",
                               &robotGameState);

}