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
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgAssignColumns.h"

#include "AgilityBookOptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	WORD bValid;
	int index;
	char const* name;
} const sc_Types[] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_FAULTS_TIME,   "Faults Then Time"},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_TIME_FAULTS,   "Time Plus Faults"},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_OPEN_CLOSE,    "Opening/Closing Points Then Time"},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_POINTS,        "Points Then Time"},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_TYPE_CALENDAR,           "Calendar"},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport,
		IO_TYPE_TRAINING,           "Training Log"},
	{CAgilityBookOptions::eViewTree,
		IO_TYPE_VIEW_TREE_DOG,      "Tree - Dog"},
	{CAgilityBookOptions::eViewTree,
		IO_TYPE_VIEW_TREE_TRIAL,    "Tree - Trial"},
	{CAgilityBookOptions::eViewTree,
		IO_TYPE_VIEW_TREE_RUN,      "Tree - Run"},
	{CAgilityBookOptions::eViewRuns,
		IO_TYPE_VIEW_RUNS_LIST,     "Runs"},
	{CAgilityBookOptions::eViewCal,
		IO_TYPE_VIEW_CALENDAR_LIST, "Calendar"},
	{CAgilityBookOptions::eViewLog,
		IO_TYPE_VIEW_TRAINING_LIST, "Training Log"},
	// Note: Remember to update sc_Fields when adding a type.
};

// This simply defines the fields and what those fields are valid for.
// sc_Fields defines what fields will be shown (and the order)
static struct
{
	WORD bValid;
	int index;
	bool bImportable;
	UINT fmt; // Only applicable to eView* items
	UINT name;
	char const* desc;
} const sc_FieldNames[IO_MAX] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_REG_NAME,      true,  LVCFMT_LEFT,   IDS_COL_REG_NAME, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_CALL_NAME,     true,  LVCFMT_LEFT,   IDS_COL_CALLNAME, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_DATE,          true,  LVCFMT_LEFT,   IDS_COL_DATE, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_VENUE,         true,  LVCFMT_LEFT,   IDS_COL_VENUE, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_CLUB,          true,  LVCFMT_LEFT,   IDS_COL_CLUB, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_LOCATION,      true,  LVCFMT_LEFT,   IDS_COL_LOCATION, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_TRIAL_NOTES,   true,  LVCFMT_LEFT,   IDS_COL_TRIALNOTES, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_DIVISION,      true,  LVCFMT_LEFT,   IDS_COL_DIVISION, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_LEVEL,         true,  LVCFMT_LEFT,   IDS_COL_LEVEL, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_EVENT,         true,  LVCFMT_LEFT,   IDS_COL_EVENT, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_HEIGHT,        true,  LVCFMT_LEFT,   IDS_COL_HEIGHT, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_JUDGE,         true,  LVCFMT_LEFT,   IDS_COL_JUDGE, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_HANDLER,       true,  LVCFMT_LEFT,   IDS_COL_HANDLER, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_CONDITIONS,    true,  LVCFMT_LEFT,   IDS_COL_CONDITIONS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_COURSE_FAULTS, true,  LVCFMT_RIGHT,  IDS_COL_COURSEFAULTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_TIME,          true,  LVCFMT_RIGHT,  IDS_COL_TIME, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_YARDS,         true,  LVCFMT_RIGHT,  IDS_COL_YARDS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_YPS,           false, LVCFMT_RIGHT,  IDS_COL_YPS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_SCT,           true,  LVCFMT_RIGHT,  IDS_COL_SCT, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_TOTAL_FAULTS,  false, LVCFMT_RIGHT,  IDS_COL_TOTAL_FAULTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_REQ_OPENING,   true,  LVCFMT_RIGHT,  IDS_COL_REQ_OPENING, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_REQ_CLOSING,   true,  LVCFMT_RIGHT,  IDS_COL_REQ_CLOSING, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_OPENING,       true,  LVCFMT_RIGHT,  IDS_COL_OPENING, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_CLOSING,       true,  LVCFMT_RIGHT,  IDS_COL_CLOSING, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_REQ_POINTS,    true,  LVCFMT_RIGHT,  IDS_COL_REQ_POINTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_POINTS,        true,  LVCFMT_RIGHT,  IDS_COL_POINTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_PLACE,         true,  LVCFMT_CENTER, IDS_COL_PLACE, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_IN_CLASS,      true,  LVCFMT_CENTER, IDS_COL_IN_CLASS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_DOGSQD,        true,  LVCFMT_CENTER, IDS_COL_Q_D, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_Q,             true,  LVCFMT_CENTER, IDS_COL_Q, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_SCORE,         false, LVCFMT_RIGHT,  IDS_COL_SCORE, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_TITLE_POINTS,  false, LVCFMT_RIGHT,  IDS_COL_TITLE_PTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_COMMENTS,      true,  LVCFMT_LEFT,   IDS_COL_COMMENTS, NULL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eViewRuns,
		IO_RUNS_FAULTS,        true,  LVCFMT_LEFT,   IDS_COL_FAULTS, NULL},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_START_DATE,     true,  LVCFMT_LEFT,   IDS_COL_START_DATE, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_END_DATE,       true,  LVCFMT_LEFT,   IDS_COL_END_DATE, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_TENTATIVE,      true,  0,   IDS_COL_TENTATIVE, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_ENTERED,        true,  0,   IDS_COL_ENTERED, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_LOCATION,       true,  LVCFMT_LEFT,   IDS_COL_LOCATION, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_CLUB,           true,  LVCFMT_LEFT,   IDS_COL_CLUB, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_VENUE,          true,  LVCFMT_LEFT,   IDS_COL_VENUE, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_OPENS,          true,  LVCFMT_LEFT,   IDS_COL_OPENS, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_CLOSES,         true,  LVCFMT_LEFT,   IDS_COL_CLOSES, NULL},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eViewCal,
		IO_CAL_NOTES,          true,  LVCFMT_LEFT,   IDS_COL_NOTES, NULL},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eViewLog,
		IO_LOG_DATE,           true,  LVCFMT_LEFT,   IDS_COL_DATE, NULL},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eViewLog,
		IO_LOG_NAME,           true,  LVCFMT_LEFT,   IDS_COL_NAME, NULL},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eViewLog,
		IO_LOG_NOTES,          true,  LVCFMT_LEFT,   IDS_COL_NOTES, NULL},

	{CAgilityBookOptions::eViewTree,
		IO_TREE_DOG_REGNAME,   false, 0, IDS_COL_REG_NAME, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_DOG_CALLNAME,  false, 0, IDS_COL_CALLNAME, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_DOG_BREED,     false, 0, IDS_COL_BREED, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_DOG_DOB,       false, 0, IDS_COL_DOB, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_DOG_AGE,       false, 0, IDS_COL_AGE, NULL},

	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_START,   false, 0, IDS_COL_START_DATE, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_END,     false, 0, IDS_COL_END_DATE, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_CLUB,    false, 0, IDS_COL_CLUB, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_VENUE,   false, 0, IDS_COL_VENUE, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_LOCATION, false, 0, IDS_COL_LOCATION, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_TRIAL_NOTES,   false, 0, IDS_COL_NOTES, NULL},

	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_DATE,      false, 0, IDS_COL_DATE, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_Q,         false, 0, IDS_COL_Q, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_EVENT,     false, 0, IDS_COL_EVENT, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_DIVISION,  false, 0, IDS_COL_DIVISION, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_LEVEL,     false, 0, IDS_COL_LEVEL, NULL},
	{CAgilityBookOptions::eViewTree,
		IO_TREE_RUN_HEIGHT,    false, 0, IDS_COL_HEIGHT, NULL},
};

UINT CDlgAssignColumns::GetFormatFromColumnID(int column)
{
	UINT fmt = 0;
	if (0 <= column && column < IO_MAX)
		fmt = sc_FieldNames[column].fmt;
	return fmt;
}

CString CDlgAssignColumns::GetNameFromColumnID(int column)
{
	CString name;
	if (0 <= column && column < IO_MAX)
		name.LoadString(sc_FieldNames[column].name);
	return name;
}

bool CDlgAssignColumns::GetColumnOrder(CAgilityBookOptions::ColumnOrder eOrder, size_t idxColumn, std::vector<int>& values)
{
	bool bOk = false;
	if (0 <= idxColumn && 0 < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].bValid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::GetColumnOrder(eOrder, idxColumn, values);
		}
	}
	return bOk;
}

bool CDlgAssignColumns::SetColumnOrder(CAgilityBookOptions::ColumnOrder eOrder, size_t idxColumn, std::vector<int> const& values)
{
	bool bOk = false;
	if (0 <= idxColumn && 0 < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].bValid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::SetColumnOrder(eOrder, idxColumn, values);
		}
	}
	return bOk;
}

static int const idxRunsFaultsTime[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_YPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,
-1};
static int const idxRunsTimeFaults[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_YPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,
-1};
static int const idxRunsOpenClose[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_REQ_OPENING,	IO_RUNS_REQ_CLOSING,
	IO_RUNS_OPENING,		IO_RUNS_CLOSING,		IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,
-1};
static int const idxRunsPoints[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_REQ_POINTS,		IO_RUNS_POINTS,
	IO_RUNS_PLACE,			IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,
	IO_RUNS_Q,				IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,
	IO_RUNS_COMMENTS,		IO_RUNS_FAULTS,
-1};
static int const idxCalendar[] = {
	IO_CAL_START_DATE,		IO_CAL_END_DATE,		IO_CAL_TENTATIVE,
	IO_CAL_ENTERED,			IO_CAL_LOCATION,		IO_CAL_CLUB,
	IO_CAL_VENUE,			IO_CAL_OPENS,			IO_CAL_CLOSES,
	IO_CAL_NOTES,
-1};
static int const idxTraining[] = {
	IO_LOG_DATE,			IO_LOG_NAME,			IO_LOG_NOTES,
-1};
static int const idxViewTreeDog[] = {
	IO_TREE_DOG_CALLNAME,	IO_TREE_DOG_CALLNAME,	IO_TREE_DOG_BREED,
	IO_TREE_DOG_DOB,		IO_TREE_DOG_AGE,
-1};
static int const idxViewTreeTrial[] = {
	IO_TREE_TRIAL_START,	IO_TREE_TRIAL_END,		IO_TREE_TRIAL_CLUB,
	IO_TREE_TRIAL_VENUE,	IO_TREE_TRIAL_LOCATION,	IO_TREE_TRIAL_NOTES,
-1};
static int const idxViewTreeRun[] = {
	IO_TREE_RUN_DATE,		IO_TREE_RUN_Q,			IO_TREE_RUN_EVENT,
	IO_TREE_RUN_DIVISION,	IO_TREE_RUN_LEVEL,		IO_TREE_RUN_HEIGHT,
-1};
static int const idxViewRunsList[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_YPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_REQ_OPENING,
	IO_RUNS_REQ_CLOSING,	IO_RUNS_OPENING,		IO_RUNS_CLOSING,
	IO_RUNS_REQ_POINTS,		IO_RUNS_POINTS,			IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,
-1};
static int const idxViewCalendarList[] = {
	IO_CAL_START_DATE,		IO_CAL_END_DATE,		IO_CAL_LOCATION,
	IO_CAL_CLUB,			IO_CAL_VENUE,			IO_CAL_OPENS,
	IO_CAL_CLOSES,			IO_CAL_NOTES,
-1};
static int const idxViewTrainingList[] = {
	IO_LOG_DATE,			IO_LOG_NAME,			IO_LOG_NOTES,
-1};

static int const* sc_Fields[IO_TYPE_MAX] =
{
	idxRunsFaultsTime,
	idxRunsTimeFaults,
	idxRunsOpenClose,
	idxRunsPoints,
	idxCalendar,
	idxTraining,
	idxViewTreeDog,
	idxViewTreeTrial,
	idxViewTreeRun,
	idxViewRunsList,
	idxViewCalendarList,
	idxViewTrainingList
};

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns dialog

CDlgAssignColumns::CDlgAssignColumns(CAgilityBookOptions::ColumnOrder eOrder, CWnd* pParent /*=NULL*/)
	: CDialog(CDlgAssignColumns::IDD, pParent)
	, m_eOrder(eOrder)
	, m_bIncludeBlank(false)
{
	//{{AFX_DATA_INIT(CDlgAssignColumns)
	//}}AFX_DATA_INIT
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		GetColumnOrder(m_eOrder, i, m_Columns[i]);
	switch (m_eOrder)
	{
	default:
		break;
	case CAgilityBookOptions::eRunsImport:
	case CAgilityBookOptions::eRunsExport:
	case CAgilityBookOptions::eCalImport:
	case CAgilityBookOptions::eCalExport:
	case CAgilityBookOptions::eLogImport:
	case CAgilityBookOptions::eLogExport:
		m_bIncludeBlank = true;
		break;
	}
}

void CDlgAssignColumns::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgAssignColumns)
	DDX_Control(pDX, IDC_ASSIGN_TYPE, m_ctrlType);
	DDX_Control(pDX, IDC_ASSIGN_AVAILABLE, m_ctrlAvailable);
	DDX_Control(pDX, IDC_ASSIGN_COLUMNS, m_ctrlColumns);
	DDX_Control(pDX, IDC_ASSIGN_ADD, m_ctrlAdd);
	DDX_Control(pDX, IDC_ASSIGN_DELETE, m_ctrlRemove);
	DDX_Control(pDX, IDC_ASSIGN_MOVE_UP, m_ctrlUp);
	DDX_Control(pDX, IDC_ASSIGN_MOVE_DOWN, m_ctrlDown);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgAssignColumns, CDialog)
	//{{AFX_MSG_MAP(CDlgAssignColumns)
	ON_CBN_SELCHANGE(IDC_ASSIGN_TYPE, OnSelchangeType)
	ON_LBN_SELCHANGE(IDC_ASSIGN_AVAILABLE, OnSelchangeAvailable)
	ON_LBN_SELCHANGE(IDC_ASSIGN_COLUMNS, OnSelchangeColumns)
	ON_BN_CLICKED(IDC_ASSIGN_ADD, OnAdd)
	ON_BN_CLICKED(IDC_ASSIGN_DELETE, OnRemove)
	ON_BN_CLICKED(IDC_ASSIGN_MOVE_UP, OnMoveUp)
	ON_BN_CLICKED(IDC_ASSIGN_MOVE_DOWN, OnMoveDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////

void CDlgAssignColumns::FillColumns()
{
	m_ctrlAvailable.ResetContent();
	m_ctrlColumns.ResetContent();
	int index = m_ctrlType.GetCurSel();
	int idxType = -1;
	if (CB_ERR != index)
		idxType = static_cast<int>(m_ctrlType.GetItemData(index));
	if (0 <= idxType)
	{
		CString blank;
		blank.LoadString(IDS_BLANK_COLUMN);
		size_t i;
		bool bInUse[IO_MAX];
		for (i = 0; i < IO_MAX; ++i)
			bInUse[i] = false;
		for (i = 0; i < m_Columns[idxType].size(); ++i)
		{
			if (0 <= m_Columns[idxType][i])
			{
				bInUse[m_Columns[idxType][i]] = true;
				CString name = GetNameFromColumnID(m_Columns[idxType][i]);
				int idx = m_ctrlColumns.AddString(name);
				if (LB_ERR != idx)
					m_ctrlColumns.SetItemData(idx, m_Columns[idxType][i]);
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
		bool bImport = (CAgilityBookOptions::eRunsImport == m_eOrder
			|| CAgilityBookOptions::eCalImport == m_eOrder
			|| CAgilityBookOptions::eLogImport == m_eOrder);
		for (i = 0; 0 <= sc_Fields[idxType][i]; ++i)
		{
			if (
			!(sc_FieldNames[sc_Fields[idxType][i]].bValid & m_eOrder)
			|| (bImport && !sc_FieldNames[sc_Fields[idxType][i]].bImportable)
			|| bInUse[sc_Fields[idxType][i]])
				continue;
			CString name = GetNameFromColumnID(sc_Fields[idxType][i]);
			int idx = m_ctrlAvailable.AddString(name);
			if (LB_ERR != idx)
				m_ctrlAvailable.SetItemData(idx, sc_Fields[idxType][i]);
		}
	}
	UpdateButtons();
}

void CDlgAssignColumns::UpdateColumnVector()
{
	int index = m_ctrlType.GetCurSel();
	int idxType = -1;
	if (CB_ERR != index)
		idxType = static_cast<int>(m_ctrlType.GetItemData(index));
	if (0 <= idxType)
	{
		m_Columns[idxType].clear();
		m_Columns[idxType].reserve(m_ctrlColumns.GetCount());
		for (int idx = 0; idx < m_ctrlColumns.GetCount(); ++idx)
		{
			m_Columns[idxType].push_back(static_cast<int>(m_ctrlColumns.GetItemData(idx)));
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
		if (!(sc_Types[index].bValid & m_eOrder))
			continue;
		int idx = m_ctrlType.AddString(sc_Types[index].name);
		if (CB_ERR != idx)
			m_ctrlType.SetItemData(idx, index);
	}
	m_ctrlType.SetCurSel(0);
	FillColumns();
	return TRUE;	// return TRUE unless you set the focus to a control
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
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		SetColumnOrder(m_eOrder, i, m_Columns[i]);
	CDialog::OnOK();
}
