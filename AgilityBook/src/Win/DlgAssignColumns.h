#pragma once

/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @brief Assign columns for import/export.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

class CDlgAssignColumns : public CDialog
{
// Construction
public:
	CDlgAssignColumns(CWnd* pParent = NULL);   // standard constructor

private:
// Dialog Data
	//{{AFX_DATA(CDlgAssignColumns)
	enum { IDD = IDD_ASSIGN_COLUMNS };
	CComboBox	m_ctrlType;
	CListBox	m_ctrlAvailable;
	CListBox	m_ctrlColumns;
	CButton	m_ctrlAdd;
	CButton	m_ctrlRemove;
	CButton	m_ctrlUp;
	CButton	m_ctrlDown;
	//}}AFX_DATA

private:
	void FillColumns();
	void UpdateButtons();

// Overrides
	//{{AFX_VIRTUAL(CDlgAssignColumns)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgAssignColumns)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeType();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
