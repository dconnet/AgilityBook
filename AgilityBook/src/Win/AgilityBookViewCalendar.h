#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewCalendar class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-10-29 DRC Support the mouse wheel for scrolling.
 * @li 2003-10-22 DRC Right click sets the current date.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include <vector>
#include "ARBDate.h"
#include "CommonView.h"
class CAgilityBookDoc;

class CAgilityBookViewCalendar : public CScrollView, public ICommonView
{
	friend class CAgilityBookViewCalendarData;
protected: // create from serialization only
	CAgilityBookViewCalendar();
	DECLARE_DYNCREATE(CAgilityBookViewCalendar)

public:
	CAgilityBookDoc* GetDocument() const;
	bool SetCurrentDate(
			ARBDate const& date,
			bool bEnsureVisible);

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;
	virtual bool GetMessage2(CString& msg) const;

private:
	size_t GetEntriesOn(
			ARBDate const& date,
			std::vector<ARBCalendarPtr>& entries,
			bool bGetHidden) const;
	void LoadData();
	CRect GetDateRect(
			ARBDate const& date,
			bool bLogical) const;
	void GetDateFromPoint(
			CPoint pt,
			ARBDate& date);
	std::vector<ARBCalendarPtr> m_Calendar;
	std::vector<ARBCalendarPtr> m_CalendarHidden;
	ARBDate m_First;	///< First date, adjusted to Mon of that week.
	ARBDate m_Last;		///< Last trial date.
	int m_nWeeks;		///< Number of visible weeks (range of first/last)
	CSize m_szEntry;
	ARBDate m_Current;	///< Currently selected date.

// Overrides
	//{{AFX_VIRTUAL(CAgilityBookViewCalendar)
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
	virtual void OnDraw(CDC* pDC);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(
			CDC* pDC,
			CPrintInfo* pInfo);
	virtual void OnEndPrinting(
			CDC* pDC,
			CPrintInfo* pInfo);
	virtual void OnPrint(
			CDC* pDC,
			CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAgilityBookViewCalendar();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	bool m_bSuppressSelect;

// Generated message map functions
protected:
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//{{AFX_MSG(CAgilityBookViewCalendar)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateCalendarEdit(CCmdUI* pCmdUI);
	afx_msg void OnCalendarEdit();
	afx_msg void OnCalendarNew();
	afx_msg void OnUpdateCalendarDelete(CCmdUI* pCmdUI);
	afx_msg void OnAgilityCalendarDelete();
	afx_msg void OnUpdateAgilityCreateentryCalendar(CCmdUI* pCmdUI);
	afx_msg void OnAgilityCreateentryCalendar();
	//}}AFX_MSG
	afx_msg void OnUpdateNotHandled(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewCalendar::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
