#pragma once

/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-02 DRC Show existing points in the list viewer.
 * @li 2004-07-29 DRC Created
 */

#include "resource.h"
#include <set>
#include "ARBInfo.h"
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
	ScoringRunInfo()
		: m_Dog()
		, m_Trial()
		, m_Run()
		, m_ScoringDetail(eNotScoringDetail)
	{
	}
	ScoringRunInfo(
			ARBDogPtr inDog,
			ARBDogTrialPtr inTrial,
			ARBDogRunPtr inRun,
			eScoringDetail inScoringDetail)
		: m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_ScoringDetail(inScoringDetail)
	{
	}
	ARBDogPtr m_Dog;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	eScoringDetail m_ScoringDetail;
};

class RunInfoData
{
	friend class CDlgListViewer;
public:
	RunInfoData(
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_Div(inDiv)
		, m_Level(inLevel)
		, m_Event(inEvent)
	{
	}
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	ARBConfigLevelPtr m_Level;
	ARBConfigEventPtr m_Event;
};

class MultiQInfoData
{
	friend class CDlgListViewer;
public:
	MultiQInfoData(
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_MultiQ(inMultiQ)
	{
	}
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
};

struct CFindItemInfo
{
	ARBInfo::eInfoType type;
	ARBString name;
	ARBInfoItemPtr pItem;
};

class CDlgListViewer : public CDlgBaseDialog
{
public:
	// Viewing runs
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			RunInfoData const* inData,
			std::list<RunInfo> const& inRuns,
			CWnd* pParent = NULL);
	// Viewing runs affected by configuration changes
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			std::list<ScoringRunInfo> const& inScoringRuns,
			CWnd* pParent = NULL);
	// Viewing multi-Qs
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			MultiQInfoData const* inData,
			std::set<MultiQdata> const& inMQs,
			CWnd* pParent = NULL);
	// Viewing lifetime data
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			std::list<LifeTimePointInfo> const& inLifetime,
			CWnd* pParent = NULL);
	// Viewing other points
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			std::list<OtherPtInfo> const& inRunList,
			CWnd* pParent = NULL);
	// Viewing Info
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			CString const& inCaption,
			std::vector<CFindItemInfo> const& inItems,
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
	RunInfoData const* m_DataRun;
	MultiQInfoData const* m_DataMultiQ;
	std::list<RunInfo> const* m_Runs;
	std::list<ScoringRunInfo> const* m_ScoringRuns;
	std::set<MultiQdata> const* m_MultiQdata;
	std::list<LifeTimePointInfo> const* m_Lifetime;
	std::list<OtherPtInfo> const* m_OtherData;
	std::vector<CFindItemInfo> const* m_Items;
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
	afx_msg void OnColumnclickList(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnBnClickedListCopy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
