// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CSettings.h : Game settings file header

#pragma once

#include "MatrixGameDllEx.hpp"

class CSettings {
public:
    CSettings();
    ~CSettings(){};

    void ResetSettings();

    bool isFullScreen() { return m_sRobotSettings.m_Fullscreen; };
    void SetFullScreen(bool state) { m_sRobotSettings.m_Fullscreen = state; };

    void GetScreen(uint32_t &w, uint32_t &h);
    void SetScreen(uint32_t w, uint32_t h);

    SMatrixSettings *GetMatrixGameSettings() { return &m_sRobotSettings; };

private:
    SMatrixSettings m_sRobotSettings;
};