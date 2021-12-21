#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief DPI Awareness
 * @author David Connet
 *
 * Revision History
 * 2020-05-20 Change GetScale from uint to int.
 * 2018-10-11 Moved to Win LibARBWin
 * 2014-12-27 Created.
 */

#include "LibwxARBWin.h"


namespace DPI
{
/// Set scaling based on current point (for per-monitor)
// ARBWIN_API void SetScale(int x, int y);

/// Set scaling. For use in WM_DPICHANGED handling.
// ARBWIN_API void SetScale(unsigned int scale);

/// Scale pixels from 100% to current.
ARBWIN_API int Scale(wxWindow const* pWindow, int x);
ARBWIN_API int Scale(int x);

/// Scale pixels from current 100%.
ARBWIN_API int UnScale(wxWindow const* pWindow, int x);

/// Current scaling (200 == 200%)
ARBWIN_API unsigned int GetScale(wxWindow const* pWindow);
}; // namespace DPI
