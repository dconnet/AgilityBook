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
 * @brief interface of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2004-07-29 DRC Created
 */

#include "resource.h"
#include <set>
#include "DlgBaseDialog.h"
#include "ListCtrl.h"
#include "PointsData.h"
class ARBDate;
class CAgilityBookDoc;

class ScoringRunInfo
{
public:
	typedef enum
	{
		eNotScoringDetail,
		eScoringDeleted,
		eScoringChanged
	} eScoringDetail;
	ScoringRunInfo(
		ARBDog const* inDog,
		ARBDogTrial const* inTrial,
		ARBDogRun const* inRun,
		eScoringDetail inScoringDetail)
		: m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_ScoringDetail(inScoringDetail)
	{
	}
	ARBDog const* m_Dog;
	ARBDogTrial const* m_Trial;
	ARBDogRun const* m_Run;
	eScoringDetail m_ScoringDetail;
};

class CDlgListViewer : public CDlgBaseDialog
{
public:
	// Viewing runs
	CDlgListViewer(CAgilityBookDoc* inDoc, CString const& inCaption,
		std::list<RunInfo> const& inRuns,
		CWnd* pParent = NULL);
	// Viewing runs affected by configuration changes
	CDlgListViewer(CAgilityBookDoc* inDoc, CString const& inCaption,
		std::list<ScoringRunInfo> const& inScoringRuns,
		CWnd* pParent = NULL);
	// Viewing double-Qs
	CDlgListViewer(CAgilityBookDoc* inDoc, CString const& inCaption,
		std::set<DoubleQdata> const& inQQs,
		CWnd* pParent = NULL);
	// Viewing other points
	CDlgListViewer(CAgilityBookDoc* inDoc, CString const& inCaption,
		std::list<OtherPtInfo> const& inRunList,
		CWnd* pParent = NULL);

private:
// Dialog Data
	//{{AFX_DATA(CDlgListViewer)
	enum { IDD = IDD_LIST_VIEWER };
	CListCtrl2	m_ctrlList;
	CButton	m_ctrlCopy;
	CButton	m_ctrlClose;
	//}}AFX_DATA
	CString m_Caption;
	CAgilityBookDoc* m_pDoc;
	std::list<RunInfo> const* m_Runs;
	std::list<ScoringRunInfo> const* m_ScoringRuns;
	std::set<DoubleQdata> const* m_DoubleQData;
	std::list<OtherPtInfo> const* m_OtherData;
	CRect m_rWin;
	CRect m_rDlg;
	CRect m_rList;
	CRect m_rCopy;
	CRect m_rOK;
	int m_SortColumn;

	//{{AFX_VIRTUAL(CDlgListViewer)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgListViewer)
	virtual BOOL OnInitDialog();
	afx_msg void OnGetdispinfoList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteitemList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedListCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
