// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsLauncherUI.h : Launcher main menu header

#pragma once

#include "CSettings.h"
#include "CApplication.h"
#include "CWindowsBaseDialogTemplate.h"
#include "WindowsSettingsUI.h"

#define MAX_MAP_PATH_SIZE 256

class CWindowsLauncherUI : public BaseDialog<CWindowsLauncherUI> {
public:
    CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp);
    ~CWindowsLauncherUI() = default;

    void Run();
    void Close();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    CSettings *m_pSettings;
    CApplication* m_pApp;

    wchar_t m_wcSelectedMap[MAX_MAP_PATH_SIZE] = {0};
};
