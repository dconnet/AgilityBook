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
 * @brief interface of the CDlgOptionsCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2004-12-18 DRC Added Opening/Closing dates to view, plus color.
 * @li 2003-08-09 DRC Moved fonts to new page.
 */

#include "AgilityBookOptions.h"
#include "DlgBasePropertyPage.h"

class CDlgOptionsCalendar : public CDlgBasePropertyPage
{
	friend class CDlgOptions;
	DECLARE_DYNCREATE(CDlgOptionsCalendar)
public:
	CDlgOptionsCalendar();
	~CDlgOptionsCalendar();

private:
// Dialog Data
	//{{AFX_DATA(CDlgOptionsCalendar)
	enum { IDD = IDD_VIEW_OPTIONS_CALENDAR };
	int		m_DayOfWeek;
	BOOL	m_bAutoDelete;
	BOOL	m_bHideOld;
	UINT	m_Days;
	BOOL	m_bHideOverlapping;
	int		m_sizeX;
	int		m_sizeY;
	BOOL	m_bOpeningNear;
	int		m_nOpeningNear;
	CEdit	m_ctrlOpeningNear;
	CStatic	m_ctrlOpeningNearColor;
	CButton	m_ctrlOpeningNearSet;
	BOOL	m_bClosingNear;
	int		m_nClosingNear;
	CEdit	m_ctrlClosingNear;
	CStatic	m_ctrlClosingNearColor;
	CButton	m_ctrlClosingNearSet;
	BOOL	m_bNotEntered;
	BOOL	m_bPlanning;
	BOOL	m_bEntered;
	BOOL	m_bOpening;
	CStatic	m_ctrlOpeningColor;
	BOOL	m_bClosing;
	CStatic	m_ctrlClosingColor;
	//}}AFX_DATA

// Overrides
	//{{AFX_VIRTUAL(CDlgOptionsCalendar)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
protected:
	void EnableControls();
	//{{AFX_MSG(CDlgOptionsCalendar)
	virtual BOOL OnInitDialog();
	afx_msg void OnOptionsCalOpeningNear();
	afx_msg void OnOptionsCalClosingNear();
	afx_msg void OnOptionsCalColorOpeningNear();
	afx_msg void OnOptionsCalColorClosingNear();
	afx_msg void OnOptionsCalColorOpen();
	afx_msg void OnOptionsCalColorClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
