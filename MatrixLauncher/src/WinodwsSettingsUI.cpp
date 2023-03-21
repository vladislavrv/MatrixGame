// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsSettingsUI.cpp : Launcher settings menu class

#include "WindowsSettingsUI.h"
#include "resources/Resource.h"

CWindowsSettingsUI::CWindowsSettingsUI(CSettings *pSettings) {
    m_pSettings = pSettings;
}

void CWindowsSettingsUI::Run() {
    RunDialog(IDD_SETTINGS_DIALOG);
}

void CWindowsSettingsUI::Close() {
    DestroyWindow(m_hWnd);
}

LRESULT CWindowsSettingsUI::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            static HWND hwndList = GetDlgItem(m_hWnd, IDC_MAPLIST);

            InterateMaps([](const wchar_t *name) { SendMessage(hwndList, LB_ADDSTRING, 0, (LPARAM)name); });
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDOK:
                    Close();
                    return TRUE;
                case IDCANCEL:
                    Close();
                    return TRUE;
                case IDC_MAPLIST:
                    switch (HIWORD(wParam)) {}
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
