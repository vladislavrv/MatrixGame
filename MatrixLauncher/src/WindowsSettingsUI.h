// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsSettingsUI.h : Launcher settings menu header

#pragma once

#include "Settings.h"
#include "Application.h"
#include "WindowsBaseDialogTemplate.h"

#define MAX_MAP_PATH_SIZE 256

class CWindowsSettingsUI : public BaseDialog<CWindowsSettingsUI> {
public:
    CWindowsSettingsUI(CSettings *pSettings) : m_pSettings(pSettings){};
    ~CWindowsSettingsUI() = default;

    void Run(HWND parent = 0);
    void Close();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    void ApplySettings();

private:
    CSettings *m_pSettings;
};
