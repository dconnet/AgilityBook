#pragma once

/*
 * Copyright � 2002-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-13 DRC Added direct access to Notes dialog.
 * @li 2005-01-21 DRC Added Location/Club info fields.
 */

#include "ComboBox.h"
#include "DlgBaseDialog.h"
#include "NoteButton.h"
#include "RichEditCtrl2.h"
class CAgilityBookDoc;

class CDlgCalendar : public CDlgBaseDialog
{
public:
	CDlgCalendar(
			ARBCalendarPtr pCal,
			CAgilityBookDoc* pDoc,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgCalendar)
	enum { IDD = IDD_CALENDAR };
	CTime	m_dateStart;
	CTime	m_dateEnd;
	BOOL	m_bTentative;
	CTime	m_dateOpens;
	BOOL	m_bOpeningUnknown;
	CTime	m_dateDraws;
	BOOL	m_bDrawingUnknown;
	CTime	m_dateCloses;
	BOOL	m_bClosingUnknown;
	int		m_Entered;
	CButton	m_ctrlOnlineUrlEntry;
	CString	m_OnlineUrl;
	CEdit	m_ctrlOnlineUrl;
	int		m_Accommodation;
	CString	m_Confirmation;
	CEdit	m_ctrlConfirmation;
	CButton	m_ctrlPremiumEntry;
	CString	m_PremiumUrl;
	CEdit	m_ctrlPremiumUrl;
	CButton	m_ctrlEMailSec;
	CString	m_EMailSecAddr;
	CEdit	m_ctrlEMailSecAddr;
	CComboBox2	m_ctrlVenue;
	CString	m_Venue;
	CComboBox2	m_ctrlClub;
	CString	m_Club;
	CNoteButton	m_ctrlClubNotes;
	CRichEditCtrl2	m_ctrlClubInfo;
	CComboBox2	m_ctrlLocation;
	CString	m_Location;
	CNoteButton	m_ctrlLocationNotes;
	CRichEditCtrl2	m_ctrlLocationInfo;
	CString	m_Notes;
	//}}AFX_DATA
	ARBCalendarPtr m_pCal;
	CAgilityBookDoc* m_pDoc;
	int m_Span;

	//{{AFX_VIRTUAL(CDlgCalendar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void UpdateLocationInfo(TCHAR const* pLocation);
	void ListLocations();
	void UpdateClubInfo(TCHAR const* pClub);
	void ListClubs();
	//{{AFX_MSG(CDlgCalendar)
	virtual BOOL OnInitDialog();
	afx_msg void OnDatetimechangeStart(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDateOpensUnknown();
	afx_msg void OnDateDrawsUnknown();
	afx_msg void OnDateClosesUnknown();
	afx_msg void OnCalEntry();
	afx_msg void OnEnChangeCalOnlineUrl();
	afx_msg void OnOnlineEntry();
	afx_msg void OnEnChangeCalPremiumUrl();
	afx_msg void OnPremiumEntry();
	afx_msg void OnEnChangeCalEmailSecAddr();
	afx_msg void OnEmailSec();
	afx_msg void OnAccommodation();
	afx_msg void OnSelchangeClub();
	afx_msg void OnKillfocusClub();
	afx_msg void OnClubNotes();
	afx_msg void OnSelchangeLocation();
	afx_msg void OnKillfocusLocation();
	afx_msg void OnLocationNotes();

	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
