// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, 2023 Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included

// New interface for Space Rangers 2 game


#pragma once

#include "MatrixGameDll.hpp"

extern "C" {
MATRIXGAMEDLL_API void __cdecl InterateMaps(void predicate(const wchar_t *name));
}