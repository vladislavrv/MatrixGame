// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

#pragma once

#include "MatrixFormGame.hpp"
#include "MatrixGameDllEx.hpp"

class CMatrixMapLogic;
class CIFaceList;
class CRenderPipeline;
class CLoadProgress;
class CHistory;
struct SMenuItemText;

extern Base::CHeap *g_MatrixHeap;
extern Base::CBlockPar *g_MatrixData;
extern CMatrixMapLogic *g_MatrixMap;
extern CIFaceList *g_IFaceList;
extern CRenderPipeline *g_Render;
extern CLoadProgress *g_LoadProgress;
extern SMenuItemText *g_PopupHead;
extern SMenuItemText *g_PopupWeaponNormal;
extern SMenuItemText *g_PopupWeaponExtern;
extern SMenuItemText *g_PopupHull;
extern SMenuItemText *g_PopupChassis;
extern CHistory *g_ConfigHistory;

class CGame {
public:
    CGame(){};
    ~CGame(){};

    void Init(HINSTANCE hInstance, HWND wnd, wchar *map = NULL, uint32_t seed = 0, SMatrixSettings *set = nullptr,
              SMatrixTextParams *textParams = nullptr);
    void Deinit();
    void SafeFree();
    void RunGameLoop(CFormMatrixGame *formGame);
    void SaveResult(SRobotGameState *state);

private:
    void ApplyVideoParams(SMatrixSettings *settings, bool autodetectFullscreen);
    void ApplyTextsReplaces(SMatrixTextParams *textParams);
};
