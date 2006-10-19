#pragma once

/*
 * Copyright © 2006 David Connet. All Rights Reserved.
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
 * @brief interface of the CAgilityBookViewHtml class
 * @author David Connet
 *
 * Revision History
 * @li 2006-09-10 DRC Created
 */

#include <afxhtml.h>
#include "CommonView.h"

class CAgilityBookDoc;
class CPointsDataItems;

class CAgilityBookViewHtml : public CHtmlView, public ICommonView
{
	DECLARE_DYNCREATE(CAgilityBookViewHtml)
	CAgilityBookViewHtml();
public:
	/**
	 * Use as a view or a control in a dialog.
	 * @param inDocAsControl For use as a control in a dialog
	 */
	CAgilityBookViewHtml(CAgilityBookDoc* inDocAsControl);
	virtual ~CAgilityBookViewHtml();
	CAgilityBookDoc* GetDocument() const;

	BOOL CreateFromStatic(UINT nID, CWnd* pParent);

	bool SetHTML(ARBString const& csHTML);

	// ICommonView interface
	virtual bool IsFiltered() const;
	virtual bool GetMessage(CString& msg) const;
	virtual bool GetMessage2(CString& msg) const;

private:
	void LoadData();

	CAgilityBookDoc* m_DocAsControl; ///< If this is set, the view is really a control.
	CPointsDataItems* m_Items;

// Operations
public:
	// <q241750>
#if _MSC_VER < 1300
	CString GetFullName() const;
	CString GetType() const;
	CString GetLocationName() const;
	CString GetLocationURL() const;
	void Navigate(
			LPCTSTR lpszURL,
			DWORD dwFlags = 0,
			LPCTSTR lpszTargetFrameName = NULL,
			LPCTSTR lpszHeaders = NULL,
			LPVOID lpvPostData = NULL,
			DWORD dwPostDataLen = 0);
	//Add the following functions. This action prevents the leak because the
	//CHtmlView versions of LoadFromResource() call CHtmlView::Navigate().
	//These need to be defined because CHtmlView::Navigate() is not virtual.
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);
#endif
	// </q241750>

// Overrides
	// ClassWizard generated virtual function overrides
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(
			BOOL bActivate,
			CView* pActivateView,
			CView* pDeactiveView);
	virtual void OnUpdate(
			CView* pSender,
			LPARAM lHint,
			CObject* pHint);
	virtual void PostNcDestroy();
public:
	virtual void OnInitialUpdate();
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	afx_msg LRESULT OnCommandHelp(WPARAM, LPARAM);
	afx_msg int OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnDestroy();
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd,UINT nHitTest,UINT message);
	afx_msg void OnUpdateAgilityNewTitle(CCmdUI* pCmdUI);
	afx_msg void OnAgilityNewTitle();
	afx_msg void OnViewPointsViewSort();
	afx_msg void OnViewHiddenTitles();
	afx_msg void OnUpdateCopyTitles(CCmdUI* pCmdUI);
	afx_msg void OnCopyTitles();
	DECLARE_MESSAGE_MAP()
};
