// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsBaseDialogTemplate.h : Template for OOP dialog classes

#pragma once
#include <tchar.h>
#include "windows.h"

template <class T>
class BaseDialog {
public:
    static INT_PTR CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        T* pThis = nullptr;

        if (uMsg == WM_INITDIALOG) {
            pThis = (T*)lParam;
            SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)pThis);

            pThis->m_hWnd = hwnd;
        }
        else {
            pThis = (T*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
        }
        if (pThis) {
            return pThis->HandleMessage(uMsg, wParam, lParam);
        }
        else {
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
        }
    }

    BaseDialog() : m_hWnd(NULL) {}

    void RunDialog(int iDialogId, HWND parent = 0) {
        DialogBoxParam(NULL, MAKEINTRESOURCE(iDialogId), parent, WindowProc, (LPARAM)this);
    }

    HWND Window() const { return m_hWnd; }

protected:
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) = 0;

    HWND m_hWnd;
};
