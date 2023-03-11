
#pragma once

#include "CSettings.h"
#include "CApplication.h"
#include "CWindowsBaseWindowTemplate.h"

class CWindowsLauncherUI : public BaseWindow<CWindowsLauncherUI> {
public:
    CWindowsLauncherUI(CSettings *pSettings, CApplication *pApp);
    ~CWindowsLauncherUI();

    void CreateMainWindow(int nCmdShow);

    void Show(int nCmdShow);
    void Close();

    virtual LPTSTR ClassName() const override;
    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

private:
    CSettings *m_pSettings;
    CApplication* m_pApp;
};
