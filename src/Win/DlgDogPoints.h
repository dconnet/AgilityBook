#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgDogPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2004-02-03 DRC Created.
 */

#include "ARBDogExistingPoints.h"
#include "ColumnOrder.h"
#include "ListCtrl.h"
class ARBConfig;
class ARBDog;

/////////////////////////////////////////////////////////////////////////////

class CDlgDogPoints : public CPropertyPage
{
	friend class CDlgDog;
public:
	CDlgDogPoints(ARBConfig& config, ARBDogExistingPointsList const& points);

private:
// Dialog Data
	//{{AFX_DATA(CDlgDogPoints)
	enum { IDD = IDD_DOG_POINTS };
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	CListCtrl2	m_ctrlPoints;
	//}}AFX_DATA
	ARBConfig& m_Config;
	CColumnOrder m_sortPoints;
	ARBDogExistingPointsList m_ExistingPoints;

	//{{AFX_VIRTUAL(CDlgDogPoints)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

private:
	void SetColumnHeaders();
	void ListExistingPoints();
	void UpdateButtons();

// Implementation
protected:
	//{{AFX_MSG(CDlgDogPoints)
	virtual BOOL OnInitDialog();
	afx_msg void OnColumnclickExistingPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkExistingPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfoExistingPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedExistingPoints(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
