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
 * @brief Make inserting items in a listbox easier.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

class CListData;

class CListBox2 : public CListBox
{
	DECLARE_DYNAMIC(CListBox2)
public:
	CListBox2(bool bAutoDelete);
	virtual ~CListBox2();

	/*
	 * Simple wrappers to control data access.
	 * Only valid if autodelete is set.
	 */
	CListData* GetData(int index) const;
	void SetData(int index, CListData* inData);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListBox2)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ComputeExtent(LPCTSTR lpszItem);
	//}}AFX_VIRTUAL
	bool m_bAutoDelete;
	int m_HorzExtent;

protected:
	//{{AFX_MSG(CListBox2)
	afx_msg void OnDestroy();
	LRESULT OnSetFont(WPARAM, LPARAM);
	LRESULT OnAddString(WPARAM, LPARAM);
	LRESULT OnInsertString(WPARAM, LPARAM);
	LRESULT OnResetContent(WPARAM, LPARAM);
	LRESULT OnDeleteString(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

class CCheckListBox2 : public CCheckListBox
{
	DECLARE_DYNAMIC(CCheckListBox2)
public:
	CCheckListBox2(bool bAutoDelete);
	virtual ~CCheckListBox2();

	/*
	 * Simple wrappers to control data access.
	 * Only valid if autodelete is set.
	 */
	CListData* GetData(int index) const;
	void SetData(int index, CListData* inData);

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCheckListBox2)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void ComputeExtent(LPCTSTR lpszItem);
	//}}AFX_VIRTUAL
	bool m_bAutoDelete;
	int m_HorzExtent;

protected:
	//{{AFX_MSG(CCheckListBox2)
	afx_msg void OnDestroy();
	LRESULT OnSetFont(WPARAM, LPARAM);
	LRESULT OnAddString(WPARAM, LPARAM);
	LRESULT OnInsertString(WPARAM, LPARAM);
	LRESULT OnResetContent(WPARAM, LPARAM);
	LRESULT OnDeleteString(WPARAM, LPARAM);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
