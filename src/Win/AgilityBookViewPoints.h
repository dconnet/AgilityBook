#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 */

#include <list>
#include <vector>
#include "CommonView.h"
#include "ListCtrl.h"
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

private:
	int DoEvents(
		ARBDog const* inDog,
		std::vector<CVenueFilter> const& venues,
		int index,
		std::list<ARBDogTrial const*> const& trials,
		ARBConfigVenue const* inVenue,
		ARBConfigDivision const* inDiv,
		ARBConfigLevel const* inLevel);
	size_t FindMatchingRuns(
		std::list<ARBDogRun const*> const& runs,
		std::string const& div,
		std::string const& level,
		std::string const& event,
		std::list<ARBDogRun const*>& matching);
	int TallyPoints(
		std::list<ARBDogRun const*> const& runs,
		ARBConfigScoring const* pScoringMethod,
		int& nCleanQ,
		int& nNotCleanQ);
	void LoadData();

protected:
	virtual void GetPrintLine(int nItem, CStringArray& line); // CListView2 override
	//{{AFX_VIRTUAL(CAgilityBookViewPoints)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
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
	//{{AFX_MSG(CAgilityBookViewPoints)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewPoints::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
