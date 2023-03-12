// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsApplication.cpp : Defines the entry point for the application on windows.

#include "CWindowsApplication.h"
#include "CWindowsLauncherUI.h"
#include "CWindowsGameWindow.h"

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

    CWindowsGameWindow gameWnd = CWindowsGameWindow();
    HWND hGameWindow = gameWnd.CreateDefault(robotSettings->m_ResolutionX, robotSettings->m_ResolutionY);

    if (!hGameWindow) {
        throw std::exception("Can't create game window");
    }

    IDirect3D9 *FDirect3D = nullptr;
    IDirect3DDevice9 *D3DDevice = nullptr;

    InitD3D(hGameWindow, robotSettings->m_ResolutionX, robotSettings->m_ResolutionY, !m_settings.isFullScreen(), FDirect3D,
            D3DDevice);

    // ќторвать €йца тому, кто так придумал передавать параметры
    // TODO define new interface in lib with better setup procedure
    robotSettings->FDirect3D = (long)FDirect3D;
    robotSettings->FD3DDevice = (long)D3DDevice;

    SRobotGameState robotGameState{0, 0, 0, 0, 0, 0};
    
    // Takes controll
    GetRobotInterface()->m_Run(m_hInstance, hGameWindow, map, robotSettings, nullptr, L"Welcome messsage", L"You Win",
                               L"You loss", L"%PlanetName%", &robotGameState);

    gameWnd.Close();
}

void CWindowsApplication::InitD3D(HWND hWnd, uint32_t w, uint32_t h, bool windowed, IDirect3D9* &FDirect3D, IDirect3DDevice9* &D3DDevice) {
    FDirect3D = Direct3DCreate9(D3D_SDK_VERSION);
    if (!FDirect3D) {
        throw L"Direct3DCreate9 failed";
    }

    D3DDISPLAYMODE mode;
    HRESULT res = FDirect3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);

    if (res != S_OK) {
        throw std::exception("Can not get display mode");
    }

    D3DPRESENT_PARAMETERS d3dpp;
    d3dpp.BackBufferWidth = w;
    d3dpp.BackBufferHeight = h;
    d3dpp.BackBufferFormat = mode.Format;
    d3dpp.BackBufferCount = 2;
    d3dpp.MultiSampleType = D3DMULTISAMPLE_NONE;
    d3dpp.MultiSampleQuality = 0;
    d3dpp.SwapEffect = D3DSWAPEFFECT_FLIP;
    d3dpp.hDeviceWindow = 0;
    d3dpp.Windowed = windowed;
    d3dpp.EnableAutoDepthStencil = 0;
    d3dpp.Flags = 0;
    d3dpp.FullScreen_RefreshRateInHz = 0;
    d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

    HRESULT cd_res = FDirect3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING,
                                    &d3dpp, &D3DDevice);

    switch (cd_res) {
        case D3D_OK:
            break;
        case D3DERR_DEVICELOST:
            throw std::exception("CreateDevice failed: D3DERR_DEVICELOST");
        case D3DERR_INVALIDCALL:
            throw std::exception("CreateDevice failed: D3DERR_INVALIDCALL");
        case D3DERR_NOTAVAILABLE:
            throw std::exception("CreateDevice failed: D3DERR_NOTAVAILABLE");
        case D3DERR_OUTOFVIDEOMEMORY:
            throw std::exception("CreateDevice failed: D3DERR_OUTOFVIDEOMEMORY");
    }
}
