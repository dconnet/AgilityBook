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
 * 2018-10-11 Moved to Win LibARBWin
 * 2014-12-27 Created.
 */

namespace DPI
{
	/// Set scaling based on current point (for per-monitor)
	//void SetScale(int x, int y);

	/// Set scaling. For use in WM_DPICHANGED handling.
	//void SetScale(unsigned int scale);

	/// Scale pixels from 100% to current.
	int Scale(wxWindow* pWindow, int x);
	int Scale(int x);

	/// Scale pixels from current 100%.
	int UnScale(wxWindow* pWindow, int x);

	/// Current scaling (200 == 200%)
	unsigned int GetScale(wxWindow* pWindow);
};
