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
 * 2021-05-25 Changed initialization
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
	CStatusBarHelper(wxFrame* frame, size_t nColumns);

	wxStatusBar* Initialize();
	size_t size() const
	{
		return m_Widths.size();
	}

	bool Update(int nCol, wxString const& text);
	bool Update(std::vector<wxString> const& text);

	// Helpers for context menu and double click handlers
	int GetContextMenuFieldId(wxContextMenuEvent const& evt, wxStatusBar*& statusbar, wxPoint& point) const;
	int GetDoubleClickFieldId(wxMouseEvent const& evt) const;

private:
	wxFrame* m_frame;
	std::vector<int> m_Widths;

	void SetStatusBarWidths(wxStatusBar* statusbar, int nColumn);
};
