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
 * @brief interface of the CCheckTreeCtrl class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

class CCheckTreeCtrl : public CTreeCtrl
{
public:
	CCheckTreeCtrl();

	void ShowCheckbox(
			HTREEITEM hItem,
			bool bShow = true);
	bool IsCheckVisible(HTREEITEM hItem) const;
	bool GetChecked(HTREEITEM hItem) const;
	bool SetChecked(
			HTREEITEM hItem,
			bool bChecked,
			bool bCascade = true);

private:
	void CheckParentCheck(HTREEITEM hItem);
	int Cascade(
			HTREEITEM hItem,
			bool bChecked);
	void SendDispInfo(HTREEITEM hItem);
	// Hide the standard versions to make sure any usage to this tree
	// shows/hides the checkboxes properly.
	// (It's not quite a straight-drop in for CTreeCtrl)
	// These are not virtual, so access via CTreeCtrl is possible.
	// Don't do that!
	BOOL GetCheck(HTREEITEM hItem) const;
	BOOL SetCheck(
			HTREEITEM hItem,
			BOOL fCheck = TRUE);
	// No one should be setting the state directly.
	BOOL SetItemState(
			HTREEITEM hItem,
			UINT nState,
			UINT nStateMask);

protected:
	CImageList m_stateList;
	UINT m_stateUnChecked;
	UINT m_stateChecked;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckTreeCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CCheckTreeCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CCheckTreeCtrl)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
