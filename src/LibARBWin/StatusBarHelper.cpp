/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CStatusBarHelper class
 * @author David Connet
 *
 * Revision History
 * 2019-01-27 Created
 */

#include "stdafx.h"
#include "LibARBWin/StatusBarHelper.h"

#include "LibARBWin/DPI.h"
//#include "ARBCommon/ARBTypes.h"
//#include "ARBCommon/StringUtil.h"

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

CStatusBarHelper::CStatusBarHelper(size_t nColumns)
	: m_Widths(nColumns, -1)
{
}


bool CStatusBarHelper::Initialize(wxStatusBar* statusbar)
{
	if (!statusbar || m_Widths.size() == 0)
		return false;

	if (m_Widths.size() != statusbar->GetFieldsCount())
	{
		assert(0);
		m_Widths.clear();
		return false;
	}

	wxClientDC dc(statusbar);
	dc.SetFont(statusbar->GetFont());

	std::vector<int> style(m_Widths.size(), 0);

	m_Widths[0] = -1; // First column is variable width
	style[0] = wxSB_FLAT;
	for (size_t i = 1; i < m_Widths.size(); ++i)
	{
		m_Widths[i] = DPI::Scale(50);
		style[i] = wxSB_NORMAL;
	}
	statusbar->SetStatusStyles(static_cast<int>(m_Widths.size()), style.data());
	SetStatusBarWidths(statusbar, -1);

	return true;
}


bool CStatusBarHelper::Update(
		wxFrame* frame,
		int nCol,
		wxString const& text)
{
	if (!frame || m_Widths.size() == 0)
		return false;

	wxStatusBar* statusbar = frame->GetStatusBar();
	if (!statusbar)
		return false;

	assert(nCol > 0 && nCol < m_Widths.size());

	frame->SetStatusText(text, nCol);

	wxClientDC dc(statusbar);
	dc.SetFont(statusbar->GetFont());
	m_Widths[nCol] = dc.GetTextExtent(text).x;

	SetStatusBarWidths(statusbar, nCol);

	return true;
}


bool CStatusBarHelper::Update(
		wxFrame* frame,
		std::vector<wxString> const& text)
{
	if (!frame || m_Widths.size() == 0)
		return false;

	wxStatusBar* statusbar = frame->GetStatusBar();
	if (!statusbar)
		return false;

	assert(text.size() + 1 == m_Widths.size());

	for (int i = 0; i < text.size(); ++i)
		frame->SetStatusText(text[i], i+1);

	wxClientDC dc(statusbar);
	dc.SetFont(statusbar->GetFont());
	for (size_t i = 0; i < text.size(); ++i)
	{
		m_Widths[i+1] = dc.GetTextExtent(text[i]).x;
	}

	SetStatusBarWidths(statusbar, -1);

	return true;
}


void CStatusBarHelper::SetStatusBarWidths(
		wxStatusBar* statusbar,
		int nColumn)
{
	bool bAddKludge = (statusbar->HasFlag(wxSTB_SIZEGRIP) && (0 > nColumn || nColumn == m_Widths.size() - 1));
	// The gripper isn't right on hidpi. Add a fudge factor.
	if (bAddKludge && DPI::GetScale(statusbar) > 100)
		m_Widths[m_Widths.size() - 1] += DPI::Scale(statusbar, 10);

	statusbar->SetStatusWidths(static_cast<int>(m_Widths.size()), m_Widths.data());
#if defined(__WXMAC__)
	// On the Mac, setting the width is always a bit small.
	// For instance, we want 36, but it gets set to 32.
	// So kludge it and force it larger.
	bool bFix = false;
	for (int i = 0; i < m_Widths.size(); ++i)
	{
		if ((0 > nColumn || i == nColumn) && 0 < m_Widths[i])
		{
			wxRect r;
			statusbar->GetFieldRect(i, r);
			if (r.width < m_Widths[i])
			{
				bFix = true;
				m_Widths[i] += 2 * (m_Widths[i] - r.width);
			}
		}
	}
	if (bFix)
		statusbar->SetStatusWidths(static_cast<int>(m_Widths.size()), m_Widths.data());
#endif
}
