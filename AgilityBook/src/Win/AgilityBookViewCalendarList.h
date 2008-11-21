#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewCalendarList class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-27 DRC Add color coding to calendar for entries that need attention.
 * @li 2005-01-25 DRC Remember the sort column between program invocations.
 * @li 2004-12-31 DRC Make F1 invoke context help.
 * @li 2004-06-24 DRC Added a sort header image.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-04-06 DRC Added simple sorting by column.
 * @li 2003-12-27 DRC Implemented Find/FindNext.
 * @li 2003-11-21 DRC Enabled copy and select all.
 * @li 2003-08-27 DRC Cleaned up selection synchronization.
 */

#include <vector>
#include "CommonView.h"
#include "DlgFind.h"
#include "ListCtrl.h"
class CAgilityBookDoc;
class CAgilityBookViewCalendarList;

class CFindCalendar : public IFindCallback
{
public:
	CFindCalendar(CAgilityBookViewCalendarList* pView)
		: m_pView(pView)
	{
	}
	virtual bool Search(CDlgFind* pDlg) const;
private:
	CAgilityBookViewCalendarList* m_pView;
};

class CAgilityBookViewCalendarList : public CListView2, public ICommonView
{
	friend class CAgilityBookViewCalendarData;
	friend class CFindCalendar;
	friend int CALLBACK CompareCalendar(LPARAM, LPARAM, LPARAM);
protected: // create from serialization only
	CAgilityBookViewCalendarList();
	DECLARE_DYNCREATE(CAgilityBookViewCalendarList)

public:
	CAgilityBookDoc* GetDocument() const;

	void SuppressSelect(bool bSuppress)		{m_bSuppressSelect = bSuppress;}

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;
	virtual bool GetMessage2(CString& msg) const;

private:
	CAgilityBookViewCalendarData* GetItemCalData(int index) const;
	void SetupColumns();
	void LoadData();
	CImageList m_ImageList;
	CImageList m_ImageStateList;
	int m_imgEmpty;
	int m_imgTentative;
	int m_imgPlan;
	int m_imgPlanTentative;
	int m_imgEntered;
	int m_imgEnteredTentative;
	int m_imgStateEmpty;
	int m_imgStateNone;
	int m_imgStateTodo;
	int m_imgStateConfirm;
	class CSortColumn
	{
	public:
		CSortColumn(std::vector<int>& inColumns);
		void Initialize();
		int GetColumn() const		{return m_iCol;}
		void SetColumn(int iCol);
	private:
		int LookupColumn(int iCol) const;
		std::vector<int>& m_Columns;
		int m_iCol;
	} m_SortColumn;

protected:
	virtual void GetPrintLine(
			int nItem,
			CStringArray& line); // CListView2 override
	//{{AFX_VIRTUAL(CAgilityBookViewCalendarList)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	protected:
	virtual void OnInitialUpdate();
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
	virtual ~CAgilityBookViewCalendarList();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	DWORD GetFormatFlags(int iCol) const;
	std::vector<int> m_Columns;
	bool m_bSuppressSelect;
	CFindCalendar m_Callback;

// Generated message map functions
protected:
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	//{{AFX_MSG(CAgilityBookViewCalendarList)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnEditFind();
	afx_msg void OnEditFindNext();
	afx_msg void OnEditFindPrevious();
	afx_msg void OnUpdateCalendarCreateEntry(CCmdUI* pCmdUI);
	afx_msg void OnCalendarCreateEntry();
	afx_msg void OnUpdateCalendarExport(CCmdUI* pCmdUI);
	afx_msg void OnCalendarExport();
	afx_msg void OnUpdateCalendarEdit(CCmdUI* pCmdUI);
	afx_msg void OnCalendarEdit();
	afx_msg void OnCalendarNew();
	afx_msg void OnUpdateEditDuplicate(CCmdUI* pCmdUI);
	afx_msg void OnEditDuplicate();
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnEditCut();
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnEditPaste();
	afx_msg void OnUpdateCalendarDelete(CCmdUI* pCmdUI);
	afx_msg void OnCalendarDelete();
	afx_msg void OnViewCustomize();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AgilityBookView.cpp
inline CAgilityBookDoc* CAgilityBookViewCalendarList::GetDocument() const
{
	return reinterpret_cast<CAgilityBookDoc*>(m_pDocument);
}
#endif
