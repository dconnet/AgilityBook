#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 */

#include "DlgBaseDialog.h"
class ARBCalendar;
class CAgilityBookDoc;

class CDlgCalendar : public CDlgBaseDialog
{
public:
	CDlgCalendar(ARBCalendar* pCal, CAgilityBookDoc* pDoc, CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgCalendar)
	enum { IDD = IDD_CALENDAR };
	CTime	m_dateStart;
	CTime	m_dateEnd;
	BOOL	m_bTentative;
	int		m_Entered;
	CComboBox	m_ctrlLocation;
	CString	m_Location;
	CComboBox	m_ctrlClub;
	CString	m_Club;
	CComboBox	m_ctrlVenue;
	CString	m_Venue;
	CTime	m_dateOpens;
	BOOL	m_bOpeningUnknown;
	CTime	m_dateCloses;
	BOOL	m_bClosingUnknown;
	CString	m_Notes;
	//}}AFX_DATA
	ARBCalendar* m_pCal;
	CAgilityBookDoc* m_pDoc;

	//{{AFX_VIRTUAL(CDlgCalendar)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgCalendar)
	virtual BOOL OnInitDialog();
	afx_msg void OnDateOpensUnknown();
	afx_msg void OnDateClosesUnknown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
