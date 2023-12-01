#pragma once

#include <CBitmap.hpp>

#include <string>

void RenderText(
    std::wstring_view text,
    std::wstring_view font,
    uint32_t color,
    int sizex,
    int sizey,
    int alignx,
    int aligny,
    int wordwrap,
    int smex,
    int smy,
    const Base::CRect& clipr,
    CBitmap& dst);