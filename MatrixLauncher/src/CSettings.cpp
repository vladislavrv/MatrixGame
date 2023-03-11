// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CSettings.h : Game settings file class implementation

#include "CSettings.h"

CSettings::CSettings() {
    ResetSettings();
}

void CSettings::ResetSettings() {
    m_sRobotSettings.m_ShowStencilShadows = 1;
    m_sRobotSettings.m_ShowProjShadows = 1;
    m_sRobotSettings.m_IzvratMS = 0;
    m_sRobotSettings.m_LandTexturesGloss = 1;
    m_sRobotSettings.m_ObjTexturesGloss = 1;
    m_sRobotSettings.m_SoftwareCursor = 0;
    m_sRobotSettings.m_SkyBox = 2;
    m_sRobotSettings.m_RobotShadow = 1;
    m_sRobotSettings.m_BPP = 32;
    m_sRobotSettings.m_RefreshRate = 60;
    m_sRobotSettings.m_Brightness = 0.5;
    m_sRobotSettings.m_Contrast = 0.5;
    m_sRobotSettings.m_FSAASamples = 0;
    m_sRobotSettings.m_AFDegree = 0;
    m_sRobotSettings.m_MaxDistance = 1;
    m_sRobotSettings.m_VSync = 1;

    SetScreen(1920, 1080);
    SetFullScreen(true);
}

void CSettings::GetScreen(uint32_t &w, uint32_t &h) {
    w = m_sRobotSettings.m_ResolutionX;
    h = m_sRobotSettings.m_ResolutionY;
}

void CSettings::SetScreen(uint32_t w, uint32_t h) {
    m_sRobotSettings.m_ResolutionX = w;
    m_sRobotSettings.m_ResolutionY = h;
}
