// MatrixLauncher - Launcher for SR2 planetary battle engine
// Copyright (C) 2023, Uladzislau "TheNornalnij" Nikalayevich
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// WindowsSettingsUI.cpp : Launcher settings menu class

#include "WindowsSettingsUI.h"
#include "resources/Resource.h"
#include <string>

const char MAX_SCREEN_SIZE_CHAR = 8;

struct SLangInfo {
    const wchar_t *name;
    const wchar_t *code;
};

const SLangInfo sLangInfo[] = {
    {L"Default", nullptr}, // ./data/robots.pkg
    {L"Russian", L"ru"},   // ./data/ru/robots.pkg
    {L"English", L"en"}    // ./data/en/robots.pkg
};

void CWindowsSettingsUI::Run(HWND parent) {
   RunDialog(IDD_SETTINGS_DIALOG, parent);
}

void CWindowsSettingsUI::Close() {
    EndDialog(m_hWnd, 0);
}

void CWindowsSettingsUI::ApplySettings() {
    // Fullscreen
    HWND hwndFullScreen = GetDlgItem(m_hWnd, IDC_FULLSCREEN);
    LRESULT fullscreenStatus = SendMessage(hwndFullScreen, BM_GETCHECK, NULL, NULL);
    m_pSettings->SetFullScreen(fullscreenStatus > 0);

    // Screen size
    const char buffer[MAX_SCREEN_SIZE_CHAR + 1] = {0};
    uint32_t uiScreenW, uiScreenH;

    HWND hwndScreenW = GetDlgItem(m_hWnd, IDC_SCREEN_W); 
    GetWindowTextA(hwndScreenW, (LPSTR)buffer, MAX_SCREEN_SIZE_CHAR);
    uiScreenW = atoi(buffer);

    HWND hwndScreenH = GetDlgItem(m_hWnd, IDC_SCREEN_H);
    GetWindowTextA(hwndScreenH, (LPSTR)buffer, MAX_SCREEN_SIZE_CHAR);
    uiScreenH = atoi(buffer);

    m_pSettings->SetScreen(uiScreenW, uiScreenH);

    // Lang
    HWND hwndSelectedLang = GetDlgItem(m_hWnd, IDC_COMBO_LANG);
    LRESULT selLang = SendMessage(hwndSelectedLang, CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
    
    m_pSettings->SetLang(sLangInfo[selLang].code);
}

LRESULT CWindowsSettingsUI::HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_INITDIALOG: {
            // Fullscreen
            HWND hwndFullScreen = GetDlgItem(m_hWnd, IDC_FULLSCREEN);
            int fullscreenStatus = m_pSettings->isFullScreen() ? BST_CHECKED : BST_UNCHECKED;
            SendMessage(hwndFullScreen, BM_SETCHECK, fullscreenStatus, 0);

            // Screen size
            uint32_t screenW, screenH;
            m_pSettings->GetScreen(screenW, screenH);

            HWND hwndScreenW = GetDlgItem(m_hWnd, IDC_SCREEN_W); 
            SetWindowTextW(hwndScreenW, std::to_wstring(screenW).c_str());
            SendMessage(hwndScreenW, EM_SETLIMITTEXT, MAX_SCREEN_SIZE_CHAR, 0);

            HWND hwndScreenH = GetDlgItem(m_hWnd, IDC_SCREEN_H);
            SetWindowTextW(hwndScreenH, std::to_wstring(screenH).c_str());
            SendMessage(hwndScreenH, EM_SETLIMITTEXT, MAX_SCREEN_SIZE_CHAR, 0);

            // Lang
            HWND hwndSelectedLang = GetDlgItem(m_hWnd, IDC_COMBO_LANG);

            for (const SLangInfo info : sLangInfo) {
                SendMessageW(hwndSelectedLang, CB_ADDSTRING, (WPARAM)0, (LPARAM)info.name);
            }

            int selectedLangIndex = 0;

            if (const wchar_t *currentLangCode = m_pSettings->GetLang()) {
                const std::wstring_view currentLang = std::wstring_view(currentLangCode);

                // We skip first element, it's ok
                for (int i = 1; i < sizeof(sLangInfo) / sizeof(SLangInfo); i++) {
                    if (currentLang.compare(sLangInfo[i].code) == 0) {
                        selectedLangIndex = i;
                        break;
                    }
                }
            }

            SendMessage(hwndSelectedLang, CB_SETCURSEL, selectedLangIndex, 0);

            return TRUE;
        }
        case WM_COMMAND: {
            switch (LOWORD(wParam)) {
                case IDOK:
                    ApplySettings();
                    Close();
                    return TRUE;
                case IDCANCEL:
                    Close();
                    return TRUE;
            }
            break;
        }
        case WM_CLOSE:
            Close();
            return TRUE;
        case WM_DESTROY:
            return TRUE;
        default:
            return FALSE;
    }
    return FALSE;
}
