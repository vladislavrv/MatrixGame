// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// CWindowsGameWindow.h : Main game window ui header

#pragma once

#include "CWindowsBaseWindowTemplate.h"

class CWindowsGameWindow : public BaseWindow<CWindowsGameWindow> {
public:
    CWindowsGameWindow(){};
    ~CWindowsGameWindow(){};

    HWND CreateDefault(int w, int h);
    void Close();

    virtual LPTSTR ClassName() const override;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
};
