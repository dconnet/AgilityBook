#pragma once

/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-10 DRC Created
 */

/*
 * If columns are added/removed, do not change these numbers. These numbers
 * are stored in a users registry for the order. If a column is removed,
 * then the number must be reserved.
 */
#define IO_TYPE_RUNS_FAULTS_TIME	0
#define IO_TYPE_RUNS_TIME_FAULTS	1
#define IO_TYPE_RUNS_OPEN_CLOSE		2
#define IO_TYPE_RUNS_POINTS			3
#define IO_TYPE_CALENDAR			4
#define IO_TYPE_TRAINING			5
#define IO_TYPE_VIEW_TREE_DOG		6
#define IO_TYPE_VIEW_TREE_TRIAL		7
#define IO_TYPE_VIEW_TREE_RUN		8
#define IO_TYPE_VIEW_RUNS_LIST		9
#define IO_TYPE_VIEW_CALENDAR_LIST	10
#define IO_TYPE_VIEW_TRAINING_LIST	11
#define IO_TYPE_MAX					12

#define IO_RUNS_REG_NAME			0
#define IO_RUNS_CALL_NAME			1
#define IO_RUNS_DATE				2
#define IO_RUNS_VENUE				3
#define IO_RUNS_CLUB				4
#define IO_RUNS_LOCATION			5
#define IO_RUNS_TRIAL_NOTES			6
#define IO_RUNS_DIVISION			7
#define IO_RUNS_LEVEL				8
#define IO_RUNS_EVENT				9
#define IO_RUNS_HEIGHT				10
#define IO_RUNS_JUDGE				11
#define IO_RUNS_HANDLER				12
#define IO_RUNS_CONDITIONS			13
#define IO_RUNS_COURSE_FAULTS		14
#define IO_RUNS_TIME				15
#define IO_RUNS_YARDS				16
#define IO_RUNS_YPS					17
#define IO_RUNS_SCT					18
#define IO_RUNS_TOTAL_FAULTS		19
#define IO_RUNS_REQ_OPENING			20
#define IO_RUNS_REQ_CLOSING			21
#define IO_RUNS_OPENING				22
#define IO_RUNS_CLOSING				23
#define IO_RUNS_REQ_POINTS			24
#define IO_RUNS_POINTS				25
#define IO_RUNS_PLACE				26
#define IO_RUNS_IN_CLASS			27
#define IO_RUNS_DOGSQD				28
#define IO_RUNS_Q					29
#define IO_RUNS_SCORE				30
#define IO_RUNS_TITLE_POINTS		31
#define IO_RUNS_COMMENTS			32
#define IO_RUNS_FAULTS				33
#define IO_CAL_START_DATE			34
#define IO_CAL_END_DATE				35
#define IO_CAL_TENTATIVE			36
#define IO_CAL_ENTERED				37
#define IO_CAL_LOCATION				38
#define IO_CAL_CLUB					39
#define IO_CAL_VENUE				40
#define IO_CAL_OPENS				41
#define IO_CAL_CLOSES				42
#define IO_CAL_NOTES				43
#define IO_LOG_DATE					44
#define IO_LOG_NAME					45
#define IO_LOG_NOTES				46
#define IO_TREE_DOG_REGNAME			47
#define IO_TREE_DOG_CALLNAME		48
#define IO_TREE_DOG_BREED			49
#define IO_TREE_DOG_DOB				50
#define IO_TREE_DOG_AGE				51
#define IO_TREE_TRIAL_START			52
#define IO_TREE_TRIAL_END			53
#define IO_TREE_TRIAL_CLUB			54
#define IO_TREE_TRIAL_VENUE			55
#define IO_TREE_TRIAL_LOCATION		56
#define IO_TREE_TRIAL_NOTES			57
#define IO_TREE_RUN_DATE			58
#define IO_TREE_RUN_Q				59
#define IO_TREE_RUN_EVENT			60
#define IO_TREE_RUN_DIVISION		61
#define IO_TREE_RUN_LEVEL			62
#define IO_TREE_RUN_HEIGHT			63
#define IO_MAX						64

#include <vector>
#include "AgilityBookOptions.h"
#include "ListBox.h"

class CDlgAssignColumns : public CDialog
{
// Construction
public:
	static UINT GetFormatFromColumnID(int column);
	static CString GetNameFromColumnID(int column);
	static bool GetColumnOrder(CAgilityBookOptions::ColumnOrder eOrder, size_t idxColumn, std::vector<int>& values);
	static bool SetColumnOrder(CAgilityBookOptions::ColumnOrder eOrder, size_t idxColumn, std::vector<int> const& values);

	CDlgAssignColumns(CAgilityBookOptions::ColumnOrder eOrder, CWnd* pParent = NULL);   // standard constructor

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
	CAgilityBookOptions::ColumnOrder m_eOrder;
	bool m_bIncludeBlank;
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
