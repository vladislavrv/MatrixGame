// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsGameWindow.cpp : Main game window ui class

#include "CWindowsGameWindow.h"

HWND CWindowsGameWindow::CreateDefault(int w, int h) {
    Create((wchar_t*)_T("Matrix game"), WS_OVERLAPPEDWINDOW, NULL, CW_USEDEFAULT, CW_USEDEFAULT, w, h);

    return m_hWnd;
}

void CWindowsGameWindow::Close() {
    DestroyWindow(m_hWnd);
}

LPTSTR CWindowsGameWindow::ClassName() const {
    return (wchar_t*)_T("Matrix game");
}

LRESULT CWindowsGameWindow::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
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
