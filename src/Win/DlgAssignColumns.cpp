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

Dog:
  Registered Name
  Call Name
  Breed
  DOB
  DOD
  Age
  Titles: Date,Venue,Name,Received
  Numbers: Venue,Number,Height,Received
  Notes
Trials:
  Location
  Notes
  Club,Venue
Runs Available fields:
  Date
  Venue
  Trial
  Location
  Division
  Level
  Event
  Height
  Judge
  Handler
  Conditions
  Course Faults
  Time
    Yards         Required Opening  Required Points
    YPS           Required Closing  Points
    SCT           Opening
    Total Faults  Closing
  Place/of
  DogsQd
  Q?
  Score
  Title Points
  Comments
  Faults
  Reference Runs: Q, Place, Score, Time, Name, Breed, Note
  CRCD

Calendar
  Start Date
  End Date
  Tentative?
  Not Entered,Planning,Entered
  Location
  Club
  Venue
  Opens
  Closes
  Notes
Training Log:
  Date
  Name
  Notes

 *
 * Revision History
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgAssignColumns.h"

#include "AgilityBookOptions.h"

#if _MSC_VER < 1300
typedef DWORD DWORD_PTR;
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static const struct
{
	int index;
	const char* name;
} sc_Types[] =
{
	{IO_TYPE_FAULTS_TIME, "Faults Then Time"},
	{IO_TYPE_TIME_FAULTS, "Time Plus Faults"},
	{IO_TYPE_OPEN_CLOSE, "Opening/Closing Points Then Time"},
	{IO_TYPE_POINTS, "Points Then Time"}
};

static const struct
{
	int index;
	bool bImportable;
	UINT name;
	const char* desc;
} sc_FieldNames[] =
{
	{IO_REG_NAME,      true,  IDS_COL_REG_NAME, NULL},
	{IO_CALL_NAME,     true,  IDS_COL_CALLNAME, NULL},
	{IO_DATE,          true,  IDS_COL_DATE, NULL},
	{IO_VENUE,         true,  IDS_COL_VENUE, NULL},
	{IO_CLUB,          true,  IDS_COL_CLUB, NULL},
	{IO_LOCATION,      true,  IDS_COL_LOCATION, NULL},
	{IO_TRIAL_NOTES,   true,  IDS_COL_TRIALNOTES, NULL},
	{IO_DIVISION,      true,  IDS_COL_DIVISION, NULL},
	{IO_LEVEL,         true,  IDS_COL_LEVEL, NULL},
	{IO_EVENT,         true,  IDS_COL_EVENT, NULL},
	{IO_HEIGHT,        true,  IDS_COL_HEIGHT, NULL},
	{IO_JUDGE,         true,  IDS_COL_JUDGE, NULL},
	{IO_HANDLER,       true,  IDS_COL_HANDLER, NULL},
	{IO_CONDITIONS,    true,  IDS_COL_CONDITIONS, NULL},
	{IO_COURSE_FAULTS, true,  IDS_COL_COURSEFAULTS, NULL},
	{IO_TIME,          true,  IDS_COL_TIME, NULL},
	{IO_YARDS,         true,  IDS_COL_YARDS, NULL},
	{IO_YPS,           false, IDS_COL_YPS, NULL},
	{IO_SCT,           true,  IDS_COL_SCT, NULL},
	{IO_TOTAL_FAULTS,  false, IDS_COL_TOTAL_FAULTS, NULL},
	{IO_REQ_OPENING,   true,  IDS_COL_REQ_OPENING, NULL},
	{IO_REQ_CLOSING,   true,  IDS_COL_REQ_CLOSING, NULL},
	{IO_OPENING,       true,  IDS_COL_OPENING, NULL},
	{IO_CLOSING,       true,  IDS_COL_CLOSING, NULL},
	{IO_REQ_POINTS,    true,  IDS_COL_REQ_POINTS, NULL},
	{IO_POINTS,        true,  IDS_COL_POINTS, NULL},
	{IO_PLACE,         true,  IDS_COL_PLACE, NULL},
	{IO_IN_CLASS,      true,  IDS_COL_IN_CLASS, NULL},
	{IO_DOGSQD,        true,  IDS_COL_Q_D, NULL},
	{IO_Q,             true,  IDS_COL_Q, NULL},
	{IO_SCORE,         false, IDS_COL_SCORE, NULL},
	{IO_TITLE_POINTS,  false, IDS_COL_TITLE_PTS, NULL},
	{IO_COMMENTS,      true,  IDS_COL_COMMENTS, NULL},
	{IO_FAULTS,        true,  IDS_COL_FAULTS, NULL},
};

CString CDlgAssignColumns::GetNameFromColumnID(int column)
{
	CString name;
	if (0 <= column && column < IO_MAX)
		name.LoadString(sc_FieldNames[column].name);
	return name;
}

static const struct
{
	int idxName[IO_TYPE_MAX];
} sc_Fields[] =
{
	{{IO_REG_NAME,      IO_REG_NAME,      IO_REG_NAME,      IO_REG_NAME}},
	{{IO_CALL_NAME,     IO_CALL_NAME,     IO_CALL_NAME,     IO_CALL_NAME}},
	{{IO_DATE,          IO_DATE,          IO_DATE,          IO_DATE}},
	{{IO_VENUE,         IO_VENUE,         IO_VENUE,         IO_VENUE}},
	{{IO_CLUB,          IO_CLUB,          IO_CLUB,          IO_CLUB}},
	{{IO_LOCATION,      IO_LOCATION,      IO_LOCATION,      IO_LOCATION}},
	{{IO_TRIAL_NOTES,   IO_TRIAL_NOTES,   IO_TRIAL_NOTES,   IO_TRIAL_NOTES}},
	{{IO_DIVISION,      IO_DIVISION,      IO_DIVISION,      IO_DIVISION}},
	{{IO_LEVEL,         IO_LEVEL,         IO_LEVEL,         IO_LEVEL}},
	{{IO_EVENT,         IO_EVENT,         IO_EVENT,         IO_EVENT}},
	{{IO_HEIGHT,        IO_HEIGHT,        IO_HEIGHT,        IO_HEIGHT}},
	{{IO_JUDGE,         IO_JUDGE,         IO_JUDGE,         IO_JUDGE}},
	{{IO_HANDLER,       IO_HANDLER,       IO_HANDLER,       IO_HANDLER}},
	{{IO_CONDITIONS,    IO_CONDITIONS,    IO_CONDITIONS,    IO_CONDITIONS}},
	{{IO_COURSE_FAULTS, IO_COURSE_FAULTS, IO_COURSE_FAULTS, IO_COURSE_FAULTS}},
	{{IO_TIME,          IO_TIME,          IO_TIME,          IO_TIME}},
	{{IO_YARDS,         IO_YARDS,         IO_REQ_OPENING,   IO_REQ_POINTS}},
	{{IO_YPS,           IO_YPS,           IO_REQ_CLOSING,   IO_POINTS}},
	{{IO_SCT,           IO_SCT,           IO_OPENING,       -1}},
	{{IO_TOTAL_FAULTS,  IO_TOTAL_FAULTS,  IO_CLOSING,       -1}},
	{{IO_PLACE,         IO_PLACE,         IO_PLACE,         IO_PLACE}},
	{{IO_IN_CLASS,      IO_IN_CLASS,      IO_IN_CLASS,      IO_IN_CLASS}},
	{{IO_DOGSQD,        IO_DOGSQD,        IO_DOGSQD,        IO_DOGSQD}},
	{{IO_Q,             IO_Q,             IO_Q,             IO_Q}},
	{{IO_SCORE,         IO_SCORE,         IO_SCORE,         IO_SCORE}},
	{{IO_TITLE_POINTS,  IO_TITLE_POINTS,  IO_TITLE_POINTS,  IO_TITLE_POINTS}},
	{{IO_COMMENTS,      IO_COMMENTS,      IO_COMMENTS,      IO_COMMENTS}},
	{{IO_FAULTS,        IO_FAULTS,        IO_FAULTS,        IO_FAULTS}},
};
static const int sc_nFields = sizeof(sc_Fields) / sizeof(sc_Fields[0]);

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns dialog

CDlgAssignColumns::CDlgAssignColumns(CAgilityBookOptions::ColumnOrder eOrder, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAssignColumns::IDD, pParent)
	, m_eOrder(eOrder)
	, m_bIncludeBlank(false)
{
	//{{AFX_DATA_INIT(CDlgAssignColumns)
	//}}AFX_DATA_INIT
	for (int i = 0; i < IO_TYPE_MAX; ++i)
		CAgilityBookOptions::GetColumnOrder(m_eOrder, i, m_Columns[i]);
	switch (m_eOrder)
	{
	default:
		break;
	case CAgilityBookOptions::eImport:
	case CAgilityBookOptions::eExport:
		m_bIncludeBlank = true;
		break;
	}
}

void CDlgAssignColumns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAssignColumns)
	DDX_Control(pDX, IDC_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_AVAILABLE, m_ctrlAvailable);
	DDX_Control(pDX, IDC_COLUMNS, m_ctrlColumns);
	DDX_Control(pDX, IDC_ADD, m_ctrlAdd);
	DDX_Control(pDX, IDC_DELETE, m_ctrlRemove);
	DDX_Control(pDX, IDC_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_MOVE_DOWN, m_ctrlDown);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAssignColumns, CDialog)
	//{{AFX_MSG_MAP(CDlgAssignColumns)
	ON_CBN_SELCHANGE(IDC_TYPE, OnSelchangeType)
	ON_LBN_SELCHANGE(IDC_AVAILABLE, OnSelchangeAvailable)
	ON_LBN_SELCHANGE(IDC_COLUMNS, OnSelchangeColumns)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_DELETE, OnRemove)
	ON_BN_CLICKED(IDC_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_MOVE_DOWN, OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgAssignColumns::FillColumns()
{
	m_ctrlAvailable.ResetContent();
	m_ctrlColumns.ResetContent();
	int index = m_ctrlType.GetCurSel();
	if (CB_ERR != index)
	{
		CString blank;
		blank.LoadString(IDS_BLANK_COLUMN);
		size_t i;
		bool bInUse[IO_MAX];
		for (i = 0; i < IO_MAX; ++i)
			bInUse[i] = false;
		for (i = 0; i < m_Columns[index].size(); ++i)
		{
			if (0 <= m_Columns[index][i])
			{
				bInUse[m_Columns[index][i]] = true;
				CString name = GetNameFromColumnID(m_Columns[index][i]);
				int idx = m_ctrlColumns.AddString(name);
				if (LB_ERR != idx)
					m_ctrlColumns.SetItemData(idx, m_Columns[index][i]);
			}
			else
			{
				if (m_bIncludeBlank)
				{
					int idx = m_ctrlColumns.AddString(blank);
					if (LB_ERR != idx)
						m_ctrlColumns.SetItemData(idx, static_cast<DWORD>(-1));
				}
			}
		}
		if (m_bIncludeBlank)
		{
			int idx = m_ctrlAvailable.AddString(blank);
			if (LB_ERR != idx)
				m_ctrlAvailable.SetItemData(idx, static_cast<DWORD>(-1));
		}
		for (i = 0; i < sc_nFields; ++i)
		{
			if (-1 == sc_Fields[i].idxName[index]
			|| (CAgilityBookOptions::eImport == m_eOrder && !sc_FieldNames[sc_Fields[i].idxName[index]].bImportable)
			|| bInUse[sc_Fields[i].idxName[index]])
				continue;
			CString name = GetNameFromColumnID(sc_Fields[i].idxName[index]);
			int idx = m_ctrlAvailable.AddString(name);
			if (LB_ERR != idx)
				m_ctrlAvailable.SetItemData(idx, sc_Fields[i].idxName[index]);
		}
	}
	UpdateButtons();
}

void CDlgAssignColumns::UpdateColumnVector()
{
	int index = m_ctrlType.GetCurSel();
	if (CB_ERR != index)
	{
		m_Columns[index].clear();
		m_Columns[index].reserve(m_ctrlColumns.GetCount());
		for (int idx = 0; idx < m_ctrlColumns.GetCount(); ++idx)
		{
			m_Columns[index].push_back(static_cast<int>(m_ctrlColumns.GetItemData(idx)));
		}
	}
}

void CDlgAssignColumns::UpdateButtons()
{
	int idxAvail = m_ctrlAvailable.GetCurSel();
	int idxCol = m_ctrlColumns.GetCurSel();
	m_ctrlAdd.EnableWindow(idxAvail != LB_ERR && 0 < m_ctrlAvailable.GetCount());
	m_ctrlRemove.EnableWindow(idxCol != LB_ERR && 0 < m_ctrlColumns.GetCount());
	m_ctrlUp.EnableWindow(idxCol != LB_ERR && 1 < m_ctrlColumns.GetCount() && 0 != idxCol);
	m_ctrlDown.EnableWindow(idxCol != LB_ERR && 1 < m_ctrlColumns.GetCount() && m_ctrlColumns.GetCount()-1 != idxCol);
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns message handlers

BOOL CDlgAssignColumns::OnInitDialog() 
{
	CDialog::OnInitDialog();
	int index;
#ifdef _DEBUG
	for (index = 0; index < IO_MAX; ++index)
	{
		ASSERT(sc_FieldNames[index].index == index);
	}
#endif
	for (index = 0; index < IO_TYPE_MAX; ++index)
	{
		ASSERT(sc_Types[index].index == index);
		int idx = m_ctrlType.AddString(sc_Types[index].name);
		if (CB_ERR != idx)
			m_ctrlType.SetItemData(idx, index);
	}
	m_ctrlType.SetCurSel(0);
	FillColumns();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgAssignColumns::OnSelchangeType() 
{
	FillColumns();
}

void CDlgAssignColumns::OnSelchangeAvailable() 
{
	UpdateButtons();
}

void CDlgAssignColumns::OnSelchangeColumns() 
{
	UpdateButtons();
}

void CDlgAssignColumns::OnAdd() 
{
	int idxAvail = m_ctrlAvailable.GetCurSel();
	if (LB_ERR != idxAvail)
	{
		CString str;
		m_ctrlAvailable.GetText(idxAvail, str);
		DWORD_PTR dwData = m_ctrlAvailable.GetItemData(idxAvail);
		int idxCol = m_ctrlColumns.AddString(str);
		if (LB_ERR != idxCol)
		{
			m_ctrlColumns.SetItemData(idxCol, dwData);
			m_ctrlColumns.SetCurSel(idxCol);
			if (static_cast<int>(dwData) >= 0)
			{
				m_ctrlAvailable.DeleteString(idxAvail);
				if (idxAvail == m_ctrlAvailable.GetCount())
					--idxAvail;
				m_ctrlAvailable.SetCurSel(idxAvail);
			}
		}
		UpdateColumnVector();
		UpdateButtons();
	}
}

void CDlgAssignColumns::OnRemove() 
{
	int idxCol = m_ctrlColumns.GetCurSel();
	if (LB_ERR != idxCol)
	{
		CString str;
		m_ctrlColumns.GetText(idxCol, str);
		DWORD_PTR dwData = m_ctrlColumns.GetItemData(idxCol);
		if (static_cast<int>(dwData) >= 0)
		{
			int idxAvail = 0;
			if (m_bIncludeBlank)
				++idxAvail; // Skip special "blank column"
			bool bDone = false;
			for (; idxAvail < m_ctrlAvailable.GetCount(); ++idxAvail)
			{
				DWORD_PTR data = m_ctrlAvailable.GetItemData(idxAvail);
				if (dwData < data)
				{
					bDone = true;
					idxAvail = m_ctrlAvailable.InsertString(idxAvail, str);
					break;
				}
			}
			if (!bDone)
				idxAvail = m_ctrlAvailable.AddString(str);
			m_ctrlAvailable.SetCurSel(idxAvail);
			// Find where to insert it...
			if (LB_ERR != idxAvail)
			{
				m_ctrlAvailable.SetItemData(idxAvail, dwData);
				m_ctrlColumns.DeleteString(idxCol);
				if (idxCol == m_ctrlColumns.GetCount())
					--idxCol;
				m_ctrlColumns.SetCurSel(idxCol);
			}
		}
		else
		{
			m_ctrlColumns.DeleteString(idxCol);
			if (idxCol == m_ctrlColumns.GetCount())
				--idxCol;
			m_ctrlColumns.SetCurSel(idxCol);
		}
		UpdateColumnVector();
		UpdateButtons();
	}
}

void CDlgAssignColumns::OnMoveUp() 
{
	int idxCol = m_ctrlColumns.GetCurSel();
	if (LB_ERR != idxCol && 1 < m_ctrlColumns.GetCount() && 0 != idxCol)
	{
		CString str;
		m_ctrlColumns.GetText(idxCol, str);
		DWORD_PTR dwData = m_ctrlColumns.GetItemData(idxCol);
		m_ctrlColumns.DeleteString(idxCol);
		idxCol = m_ctrlColumns.InsertString(--idxCol, str);
		m_ctrlColumns.SetItemData(idxCol, dwData);
		m_ctrlColumns.SetCurSel(idxCol);
		UpdateColumnVector();
		UpdateButtons();
	}
}

void CDlgAssignColumns::OnMoveDown() 
{
	int idxCol = m_ctrlColumns.GetCurSel();
	if (LB_ERR != idxCol && 1 < m_ctrlColumns.GetCount() && m_ctrlColumns.GetCount() - 1 != idxCol)
	{
		CString str;
		m_ctrlColumns.GetText(idxCol, str);
		DWORD_PTR dwData = m_ctrlColumns.GetItemData(idxCol);
		m_ctrlColumns.DeleteString(idxCol);
		idxCol = m_ctrlColumns.InsertString(++idxCol, str);
		m_ctrlColumns.SetItemData(idxCol, dwData);
		m_ctrlColumns.SetCurSel(idxCol);
		UpdateColumnVector();
		UpdateButtons();
	}
}

void CDlgAssignColumns::OnOK() 
{
	for (int i = 0; i < IO_TYPE_MAX; ++i)
		CAgilityBookOptions::SetColumnOrder(m_eOrder, i, m_Columns[i]);
	CDialog::OnOK();
}
