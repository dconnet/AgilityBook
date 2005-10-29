#pragma once

/*
 * Copyright � 2002-2005 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-08-12 DRC Allow creating a new title.
 * @li 2004-05-27 DRC Changed lifetime points structure to take a string.
 */

#include <list>
#include <vector>
#include "CommonView.h"
#include "ListCtrl.h"
#include "PointsData.h"
class ARBConfigDivision;
class ARBConfigLevel;
class ARBConfigScoring;
class ARBConfigVenue;
class ARBDog;
class ARBDogRun;
class ARBDogTrial;
class CAgilityBookDoc;
struct CVenueFilter;

class CAgilityBookViewPoints : public CListView2, public ICommonView
{
protected: // create from serialization only
	CAgilityBookViewPoints();
	DECLARE_DYNCREATE(CAgilityBookViewPoints)

public:
	CAgilityBookDoc* GetDocument() const;

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;
	virtual bool GetMessage2(CString& msg) const;

private:
	struct LifeTimePoint
	{
		std::string event;
		int points;
		bool bFiltered;
		LifeTimePoint()
			: event()
			, points(0)
			, bFiltered(false)
		{
		}
		LifeTimePoint(
				const std::string inEvent,
				int inPoints,
				bool inFiltered)
			: event(inEvent)
			, points(inPoints)
			, bFiltered(inFiltered)
		{
		}
	};
	typedef std::list<LifeTimePoint> LifeTimePointList;
	struct LifeTimePoints
	{
		ARBConfigDivision const* pDiv;
		ARBConfigLevel const* pLevel;
		LifeTimePointList ptList;
	};
	typedef std::list<LifeTimePoints> LifeTimePointsList;

	PointsDataBase* GetItemData(int index) const;
	size_t FindMatchingRuns(
			std::list<RunInfo> const& runs,
			std::string const& div,
			std::string const& level,
			std::string const& event,
			std::list<RunInfo>& matching);
	int TallyPoints(
			std::list<RunInfo> const& runs,
			ARBConfigScoring const* pScoringMethod,
			int& nCleanQ,
			int& nNotCleanQ);
	void InsertData(int& ioIndex, PointsDataBase* inData);
	void LoadData();

protected:
	virtual void GetPrintLine(
			int nItem,
			CStringArray& line); // CListView2 override
	//{{AFX_VIRTUAL(CAgilityBookViewPoints)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(
			BOOL bActivate,
			CView* pActivateView,
			CView* pDeactiveView);
	virtual void OnUpdate(
			CView* pSender,
			LPARAM lHint,
			CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookViewPoints();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//{{AFX_MSG(CAgilityBookViewPoints)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclk(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateDetails(CCmdUI* pCmdUI);
	afx_msg void OnDetails();
	afx_msg void OnUpdateAgilityNewTitle(CCmdUI* pCmdUI);
	afx_msg void OnAgilityNewTitle();
	afx_msg void OnViewHiddenTitles();
	afx_msg void OnUpdateCopyTitles(CCmdUI* pCmdUI);
	afx_msg void OnCopyTitles();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewPoints::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
