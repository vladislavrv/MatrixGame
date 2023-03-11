// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CSettings.h : Game settings file header

#pragma once

#include "MatrixGameDll.hpp"

class CSettings {
public:
    CSettings();
    ~CSettings(){};

    void ResetSettings();

    bool isFullScreen() { return m_bFullScreen; };
    void SetFullScreen(bool state) { m_bFullScreen = state; };

    void GetScreen(uint32_t &w, uint32_t &h);
    void SetScreen(uint32_t w, uint32_t h);

    SRobotsSettings *GetRobotGameSettings() { return &m_sRobotSettings; };

private:
    bool m_bFullScreen;
    SRobotsSettings m_sRobotSettings;
};