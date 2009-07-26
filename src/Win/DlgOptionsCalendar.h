#pragma once

/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief interface of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-11-22 DRC Turned this into Calendar only, created new Print page.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 */

#include "AgilityBookOptions.h"


class CDlgOptionsCalendar : public wxPanel
{
public:
	CDlgOptionsCalendar(wxWindow* pParent);

	void Save();

private:
	wxString GetCalText(
			CAgilityBookOptions::CalendarColorItem type,
			bool bForDisplay) const;
	void UpdateControls();
	void SetCalColor();
	void SetRichText();

	CFontInfo m_fontCalViewInfo;
	wxFont m_fontCalView;
	wxColour m_OpeningNear;
	wxColour m_ClosingNear;
	typedef std::pair<CAgilityBookOptions::CalendarColorItem, wxColour> tColorInfo;
	std::vector<tColorInfo> m_CalColors;
	bool m_bOpeningNear;
	int m_nOpeningNear;
	bool m_bClosingNear;
	int m_nClosingNear;
	bool m_bAutoDelete;
	bool m_bHideOld;
	int m_Days;
	bool m_bHideOverlapping;
	bool m_bOpening;
	bool m_bClosing;
	wxTextCtrl* m_ctrlOpeningNear;
	wxStaticText* m_ctrlOpeningNearColor;
	wxButton* m_ctrlOpeningNearSet;
	wxTextCtrl* m_ctrlClosingNear;
	wxStaticText* m_ctrlClosingNearColor;
	wxButton* m_ctrlClosingNearSet;
	wxChoice* m_ctrlDayOfWeek;
	wxChoice* m_ctrlCalEntries;
	wxStaticText* m_ctrlColor;
	wxTextCtrl* m_ctrlCalView;

	void OnSelchangeCalEntries(wxCommandEvent& evt);
	void OnCalNear(wxCommandEvent& evt);
	void OnCalColorOpeningNear(wxCommandEvent& evt);
	void OnCalColorClosingNear(wxCommandEvent& evt);
	void OnCalColors(wxCommandEvent& evt);
	void OnFontCalView(wxCommandEvent& evt);
};
