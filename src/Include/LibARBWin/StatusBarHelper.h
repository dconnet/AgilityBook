#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CStatusBarHelper class
 * @author David Connet
 *
 * Revision History
 * 2019-01-27 Created
 */

#include "LibwxARBWin.h"

#include <vector>
class wxStatusBar;


/**
 * Helper class for setting status bar widths.
 * Column 0 is reserved for menu tips (variable width).
 */
class ARBWIN_API CStatusBarHelper
{
	DECLARE_NO_COPY_IMPLEMENTED(CStatusBarHelper)
public:
	CStatusBarHelper(size_t nColumns);

	bool Initialize(wxStatusBar* statusbar);
	size_t size() const { return m_Widths.size(); }

	bool Update(
			wxFrame* frame,
			int nCol,
			wxString const& text);
	bool Update(
			wxFrame* frame,
			std::vector<wxString> const& text);

private:
	std::vector<int> m_Widths;

	void SetStatusBarWidths(
			wxStatusBar* statusbar,
			int nColumn);
};
