#pragma once

/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgListCtrl class
 * @author David Connet
 *
 * Revision History
 * @li 2006-12-26 DRC Made dialog resizable.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include <vector>
#include "ARBDate.h"
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
class CAgilityBookDoc;
class CDlgListCtrlData;
class CTabView;

class CDlgListCtrl : public CDlgBaseDialog
{
public:
	typedef enum
	{
		eCalendar,
		eFaults,
		eOtherPoints,
		ePartners
	} WhatToList;

	// Calendar
	CDlgListCtrl(
			CAgilityBookDoc* pDoc,
			ARBDate const& date,
			std::vector<ARBCalendarPtr> const* entries,
			CTabView* pTabView,
			CWnd* pParent = NULL);
	// Faults, Partners
	CDlgListCtrl(
			WhatToList inType,
			CAgilityBookDoc* pDoc,
			ARBDogRunPtr run,
			CWnd* pParent = NULL);
	// OtherPoints
	CDlgListCtrl(
			ARBConfig& pConfig,
			ARBDogRunPtr run,
			CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgListCtrl)
	enum { IDD = IDD_LISTCTRL };
	CListCtrl2	m_ctrlList;
	CButton	m_ctrlNew;
	CButton	m_ctrlEdit;
	CButton	m_ctrlDelete;
	CButton	m_ctrlUp;
	CButton	m_ctrlDown;
	CButton m_ctrlCreateTrial;
	CButton	m_ctrlOk;
	CButton	m_ctrlCancel;
	//}}AFX_DATA
	CRect m_rWin;
	CRect m_rDlg;
	CRect m_rList;
	CRect m_rNew;
	CRect m_rEdit;
	CRect m_rDelete;
	CRect m_rUp;
	CRect m_rDown;
	CRect m_rTrial;
	CRect m_rOK;
	CRect m_rCancel;
	WhatToList m_What;
	CAgilityBookDoc* m_pDoc;
	ARBDate m_Date;
	std::vector<ARBCalendarPtr> const* m_CalEntries;
	CTabView* m_pTabView;
	ARBConfig* m_pConfig;
	ARBDogRunPtr m_pRun;
	CImageList m_ImageList;
	int m_imgEmpty;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;

	//{{AFX_VIRTUAL(CDlgListCtrl)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	void UpdateControls();
	void SwapEntries(
			int oldIndex,
			int newIndex);
public:
	void GetAllPartners(
			std::set<tstring>& ioPartners,
			std::set<tstring>& ioDogs) const;

// Implementation
protected:
	//{{AFX_MSG(CDlgListCtrl)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchangedList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydownList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnNew();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnCreateTrial();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
