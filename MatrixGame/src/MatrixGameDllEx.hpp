// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// New interface for Space Rangers 2 game

#pragma once

#include "MatrixGameDll.hpp"

struct SMatrixTextParams {
    const wchar_t *startText;
    const wchar_t *winText;
    const wchar_t *lossText;
    const wchar_t *planetName;
};

struct SMatrixSettings : public SRobotsSettings {
    bool m_Fullscreen;
    const wchar_t *m_Lang;
};

extern "C" {
MATRIXGAMEDLL_API void __cdecl InterateMaps(void predicate(const wchar_t *name));
MATRIXGAMEDLL_API int __cdecl RunStandalone(HINSTANCE hinst, wchar *map, SMatrixSettings *set,
                                               SMatrixTextParams *textParams, SRobotGameState *rgs);
}