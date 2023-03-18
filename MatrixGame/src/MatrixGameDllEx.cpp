// MatrixGame - SR2 Planetary battles engine
// Copyright (C) 2012, Elemental Games, Katauri Interactive, CHK-Games
// Licensed under GPLv2 or any later version
// Refer to the LICENSE file included
// New intereace implementation


#include "MatrixGameDllEx.hpp"
#include "stdafx.h"
#include "Interface/CInterface.h"
#include <string_view>

// TODO more map info
MATRIXGAMEDLL_API void __cdecl InterateMaps(void predicate(const wchar_t *name)) {
#ifndef MAXEXP_EXPORTS
    CHeap heap = CHeap{};
    CPackFile robotPack = CPackFile(&heap, L"DATA\\robots.pkg");
    robotPack.OpenPacketFile();

    CHsFolder *mapsFolder = robotPack.GetRootFolder()->GetFolderEx("Matrix\\Map\\");

    static auto _predicate = predicate;
    static wchar_t buff[100];
    mapsFolder->ForFiles([](const char *name) {
        std::string_view nameStr = std::string_view(name);

        if (nameStr.ends_with(".CMAP")) {
            std::mbstowcs(buff, name, 100);
            _predicate(buff);
        }
    });

    robotPack.ClosePacketFile();
#endif
}
