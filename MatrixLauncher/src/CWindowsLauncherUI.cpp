// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsLauncherUI.cpp : Launcher main menu class

#include "CWindowsLauncherUI.h"
#include "resources/Resource.h"
#include "MatrixGameDllEx.hpp"
#include "WindowsSettingsUI.h"

CWindowsLauncherUI::CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp) {
    m_pSettings = pSettings;
    m_pApp = pApp;
}

void CWindowsLauncherUI::Run() {
    RunDialog(IDD_LAUNCHER_MAIN);
}

void CWindowsLauncherUI::Close() {
    DestroyWindow(m_hWnd);
}

LRESULT CWindowsLauncherUI::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            static HWND hwndList = GetDlgItem(m_hWnd, IDC_MAPLIST); 

            InterateMaps([](const wchar_t *name) { SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)name); });
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam))
            {
                case ID_BTN_PLAY:
                    Close();
                    m_pApp->StartLocalGame(m_wcSelectedMap);
                    return TRUE;
                case ID_BTN_SETTINGS: {
                    CWindowsSettingsUI settingsDialog = CWindowsSettingsUI(m_pSettings);
                    settingsDialog.Run();
                    return TRUE;
                }
                case IDC_MAPLIST:
                    switch (HIWORD(wParam)) {
                        case LBN_SELCHANGE: {
                            HWND hwndList = GetDlgItem(m_hWnd, IDC_MAPLIST);
                            int lbItem = (int)SendMessage(hwndList, LB_GETCURSEL, 0, 0);

                            SendMessage(hwndList, LB_GETTEXT, lbItem, (LPARAM)m_wcSelectedMap);
                            return TRUE;
                        }
                    }
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
