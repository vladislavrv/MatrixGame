// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsLauncherUI.h : Launcher main menu header

#pragma once

#include "CSettings.h"
#include "CApplication.h"
#include "CWindowsBaseWindowTemplate.h"

class CWindowsLauncherUI : public BaseWindow<CWindowsLauncherUI> {
public:
    CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp);
    ~CWindowsLauncherUI(){};

    void CreateMainWindow(int nCmdShow);

    void Show(int nCmdShow);
    void Close();

    virtual LPTSTR ClassName() const override;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    CSettings *m_pSettings;
    CApplication* m_pApp;
};
