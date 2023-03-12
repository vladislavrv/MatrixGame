// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsLauncherUI.cpp : Launcher main menu class

#include "CWindowsLauncherUI.h"
#include "resources/Resource.h"

CWindowsLauncherUI::CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp) {
    m_pSettings = pSettings;
    m_pApp = pApp;
}

void CWindowsLauncherUI::Run() {
    RunDialog(ID_LAUNCHER_MAIN);
}

void CWindowsLauncherUI::Close() {
    DestroyWindow(m_hWnd);
}

LRESULT CWindowsLauncherUI::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND: {
            switch (wParam)
            {
                case ID_BTN_PLAY:
                    Close();
                    m_pApp->StartLocalGame(nullptr);
                    break;
                case ID_BTN_SETTINGS:
                    MessageBox(NULL, _T("Not implement"), _T("Error"), NULL);
                    break;
            }
            break;
        }
        case WM_CLOSE:
            Close();
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(m_hWnd, msg, wParam, lParam);
    }
    return 0;
}
