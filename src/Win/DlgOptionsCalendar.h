#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-11-22 Turned this into Calendar only, created new Print page.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-12-18 Added Opening/Closing dates to view, plus color.
 * 2003-08-09 Moved fonts to new page.
 */

#include "AgilityBookOptions.h"


namespace dconSoft
{
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgOptionsCalendar : public wxPanel
{
public:
	CDlgOptionsCalendar(wxWindow* pParent);

	void Save();

private:
	wxString GetCalText(ARBCalColorItem type, bool bForDisplay) const;
	void UpdateControls();
	void SetCalColor();
	void SetRichText();

	CFontInfo m_fontCalViewInfo;
	wxFont m_fontCalView;
	wxColour m_OpeningNear;
	wxColour m_ClosingNear;
	typedef std::pair<ARBCalColorItem, wxColour> tColorInfo;
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
	ARBWin::CTextCtrl* m_ctrlOpeningNear;
	wxStaticText* m_ctrlOpeningNearColor;
	wxButton* m_ctrlOpeningNearSet;
	ARBWin::CTextCtrl* m_ctrlClosingNear;
	wxStaticText* m_ctrlClosingNearColor;
	wxButton* m_ctrlClosingNearSet;
	wxChoice* m_ctrlDayOfWeek;
	wxChoice* m_ctrlCalEntries;
	wxStaticText* m_ctrlColor;
	ARBWin::CTextCtrl* m_ctrlCalView;

	void OnSelchangeCalEntries(wxCommandEvent& evt);
	void OnCalNear(wxCommandEvent& evt);
	void OnCalColorOpeningNear(wxCommandEvent& evt);
	void OnCalColorClosingNear(wxCommandEvent& evt);
	void OnCalColors(wxCommandEvent& evt);
	void OnFontCalView(wxCommandEvent& evt);
};

} // namespace dconSoft
