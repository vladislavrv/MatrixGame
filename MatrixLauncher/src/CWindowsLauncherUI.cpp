// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsLauncherUI.cpp : Launcher main menu class

#include "CWindowsLauncherUI.h"

enum class eWindowControll { ID_PLAY_BUTTON, ID_SETTINGS_BUTTON };

static const LPTSTR strWindowName = _T("Matrix launcher");

CWindowsLauncherUI::CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp) {
    m_pSettings = pSettings;
    m_pApp = pApp;
}

void CWindowsLauncherUI::CreateMainWindow(int nCmdShow) {
    const boolean success = Create(strWindowName, WS_OVERLAPPEDWINDOW, NULL, CW_USEDEFAULT,
                                   CW_USEDEFAULT, 350, 500);

    if (!success) {
        MessageBox(NULL, _T("Failed to create window"), strWindowName, NULL);
        return;
    }

    CreateWindow(_T("BUTTON"), _T("Settings"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT, 110,
                                        420, 100, 30, m_hWnd, (HMENU)eWindowControll::ID_SETTINGS_BUTTON,
                                        (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), NULL);

    CreateWindow(_T("BUTTON"), _T("Play"), WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_FLAT, 220, 420,
                                    100, 30, m_hWnd, (HMENU)eWindowControll::ID_PLAY_BUTTON,
                                    (HINSTANCE)GetWindowLongPtr(m_hWnd, GWLP_HINSTANCE), NULL);
}

void CWindowsLauncherUI::Show(int nCmdShow) {
    CreateMainWindow(nCmdShow);

    if (m_hWnd) {
        ShowWindow(m_hWnd, nCmdShow);
        UpdateWindow(m_hWnd);

        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
}

void CWindowsLauncherUI::Close() {
    DestroyWindow(m_hWnd);
}

LPTSTR CWindowsLauncherUI::ClassName() const {
    return _T("Matrix game launcher");
}

LRESULT CWindowsLauncherUI::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND: {
            switch ((eWindowControll)wParam)  // the ID is is wParam
            {
                case eWindowControll::ID_PLAY_BUTTON:
                    Close();
                    m_pApp->StartLocalGame(nullptr);
                    break;
                case eWindowControll::ID_SETTINGS_BUTTON:
                    MessageBox(NULL, _T("Not implement"), strWindowName, NULL);
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
