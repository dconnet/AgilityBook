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

#define IO_TYPE_FAULTS_TIME	0
#define IO_TYPE_TIME_FAULTS	1
#define IO_TYPE_OPEN_CLOSE	2
#define IO_TYPE_POINTS		3
#define IO_TYPE_MAX			4

#define IO_REG_NAME			0
#define IO_CALL_NAME		1
#define IO_DATE				2
#define IO_VENUE			3
#define IO_CLUB				4
#define IO_LOCATION			5
#define IO_TRIAL_NOTES		6
#define IO_DIVISION			7
#define IO_LEVEL			8
#define IO_EVENT			9
#define IO_HEIGHT			10
#define IO_JUDGE			11
#define IO_HANDLER			12
#define IO_CONDITIONS		13
#define IO_COURSE_FAULTS	14
#define IO_TIME				15
#define IO_YARDS			16
#define IO_YPS				17
#define IO_SCT				18
#define IO_TOTAL_FAULTS		19
#define IO_REQ_OPENING		20
#define IO_REQ_CLOSING		21
#define IO_OPENING			22
#define IO_CLOSING			23
#define IO_REQ_POINTS		24
#define IO_POINTS			25
#define IO_PLACE			26
#define IO_IN_CLASS			27
#define IO_DOGSQD			28
#define IO_Q				29
#define IO_SCORE			30
#define IO_TITLE_POINTS		31
#define IO_COMMENTS			32
#define IO_FAULTS			33
#define IO_MAX				34

#include <vector>
#include "ListBox.h"

class CDlgAssignColumns : public CDialog
{
// Construction
public:
	static CString CDlgAssignColumns::GetNameFromColumnID(int column);

	CDlgAssignColumns(bool bImport, CWnd* pParent = NULL);   // standard constructor

private:
// Dialog Data
	//{{AFX_DATA(CDlgAssignColumns)
	enum { IDD = IDD_ASSIGN_COLUMNS };
	CComboBox	m_ctrlType;
	CListBox2	m_ctrlAvailable;
	CListBox2	m_ctrlColumns;
	CButton	m_ctrlAdd;
	CButton	m_ctrlRemove;
	CButton	m_ctrlUp;
	CButton	m_ctrlDown;
	//}}AFX_DATA
	bool m_bImport;
	std::vector<int> m_Columns[IO_TYPE_MAX];

private:
	void FillColumns();
	void UpdateColumnVector();
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
	afx_msg void OnSelchangeAvailable();
	afx_msg void OnSelchangeColumns();
	afx_msg void OnAdd();
	afx_msg void OnRemove();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
