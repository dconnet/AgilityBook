#pragma once

/*
 * Copyright © 2003-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgFilterDate class
 * @author David Connet
 *
 * Revision History
 * @li 2005-08-18 DRC Created
 */

#include "DlgBasePropertyPage.h"
class CAgilityBookDoc;

class CDlgFilterDate : public CDlgBasePropertyPage
{
	friend class CDlgFilter;
	DECLARE_DYNAMIC(CDlgFilterDate)
public:
	CDlgFilterDate();
	~CDlgFilterDate();

private:
// Dialog Data
	//{{AFX_DATA(CDlgFilterDate)
	enum { IDD = IDD_VIEW_FILTER_DATE };
	int		m_ViewDates;
	CButton	m_ctrlDateStartCheck;
	BOOL	m_bDateStart;
	CDateTimeCtrl	m_ctrlDateStart;
	CTime	m_timeStart;
	CButton	m_ctrlDateEndCheck;
	BOOL	m_bDateEnd;
	CDateTimeCtrl	m_ctrlDateEnd;
	CTime	m_timeEnd;
	//}}AFX_DATA

private:
	void UpdateControls();

// Overrides
	//{{AFX_VIRTUAL(CDlgFilterDate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgFilterDate)
	virtual BOOL OnInitDialog();
	afx_msg void OnViewUpdate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
