/*
 * Copyright © 2003-2008 David Connet. All Rights Reserved.
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
	Obstacles     ObstaclesPerSecond
  Place/of
  DogsQd
  Q?
  Score
  Title Points
  Comments
  Faults
  Reference Runs: Q, Place, Score, Time, Name, Breed, Note
  CRCD
  Speed (MACH)

Calendar [L/V]
  Start Date [L]
  End Date [L]
  Tentative? [L]
  Not Entered,Planning,Entered [L]
  Location [LV]
  Club [LV]
  Venue [LV]
  Opens [L]
  Closes [L]
  Notes [LV]
Training Log:
  Date
  Name
  Notes

 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-08-11 DRC Added verified column to trial in tree.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgAssignColumns.h"

#include "AgilityBookDoc.h"
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
	int sortOrder; // only used for view types
	UINT name;
	UINT desc;
} const sc_Types[] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_FAULTS_TIME, 0,
		IDS_ASSCOL_RUNS_FAULTS_TIME, IDS_ASSCOL_RUNS_FAULTS_TIME_DESC},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_TIME_FAULTS, 0,
		IDS_ASSCOL_RUNS_TIME_FAULTS, IDS_ASSCOL_RUNS_TIME_FAULTS_DESC},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_OPEN_CLOSE, 0,
		IDS_ASSCOL_RUNS_OPEN_CLOSE, IDS_ASSCOL_RUNS_OPEN_CLOSE_DESC},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_POINTS, 0,
		IDS_ASSCOL_RUNS_POINTS, IDS_ASSCOL_RUNS_POINTS_DESC},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_TYPE_CALENDAR, 0,
		IDS_ASSCOL_CALENDAR, IDS_ASSCOL_CALENDAR_DESC},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport,
		IO_TYPE_TRAINING, 0,
		IDS_ASSCOL_TRAINING, IDS_ASSCOL_TRAINING_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_DOG, 1,
		IDS_ASSCOL_VIEW_TREE_DOG, IDS_ASSCOL_VIEW_TREE_DOG_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_TRIAL, 2,
		IDS_ASSCOL_VIEW_TREE_TRIAL, IDS_ASSCOL_VIEW_TREE_TRIAL_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_RUN, 3,
		IDS_ASSCOL_VIEW_TREE_RUN, IDS_ASSCOL_VIEW_TREE_RUN_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_RUNS_LIST, 4,
		IDS_ASSCOL_VIEW_RUNS_LIST, IDS_ASSCOL_VIEW_RUNS_LIST_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR_LIST, 5,
		IDS_ASSCOL_VIEW_CALENDAR_LIST, IDS_ASSCOL_VIEW_CALENDAR_LIST_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TRAINING_LIST, 7,
		IDS_ASSCOL_VIEW_TRAINING_LIST, IDS_ASSCOL_VIEW_TRAINING_LIST_DESC},
	{CAgilityBookOptions::eCalExportAppt,
		IO_TYPE_CALENDAR_APPT, 0,
		IDS_ASSCOL_CALENDAR_APPT, IDS_ASSCOL_CALENDAR_APPT_DESC},
	{CAgilityBookOptions::eCalExportTask,
		IO_TYPE_CALENDAR_TASK, 0,
		IDS_ASSCOL_CALENDAR_TASK, IDS_ASSCOL_CALENDAR_TASK_DESC},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR, 6,
		IDS_ASSCOL_VIEW_CALENDAR, IDS_ASSCOL_VIEW_CALENDAR_DESC},
	// Note: Remember to update sc_Fields when adding a type.
};


// This simply defines the fields and what those fields are valid for.
// sc_Fields defines what fields will be shown (and the order)
// NOTE: Currently, the fields must be in index order - so any new fields must
//       be added at the bottom. The physical ordering is not controlled by the
//       order in this structure.
static struct
{
	WORD bValid;
	int index;
	bool bImportable;
	UINT fmt; // Only applicable to eView items
	UINT name;
} const sc_FieldNames[IO_MAX] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REG_NAME,
		true, LVCFMT_LEFT, IDS_COL_REG_NAME},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CALL_NAME,
		true, LVCFMT_LEFT, IDS_COL_CALLNAME},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DATE,
		true, LVCFMT_LEFT, IDS_COL_DATE},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_VENUE,
		true, LVCFMT_LEFT, IDS_COL_VENUE},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLUB,
		true, LVCFMT_LEFT, IDS_COL_CLUB},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LOCATION,
		true, LVCFMT_LEFT, IDS_COL_LOCATION},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TRIAL_NOTES,
		true, LVCFMT_LEFT, IDS_COL_TRIALNOTES},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DIVISION,
		true, LVCFMT_LEFT, IDS_COL_DIVISION},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LEVEL,
		true, LVCFMT_LEFT, IDS_COL_LEVEL},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_EVENT,
		true, LVCFMT_LEFT, IDS_COL_EVENT},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HEIGHT,
		true, LVCFMT_LEFT, IDS_COL_HEIGHT},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_JUDGE,
		true, LVCFMT_LEFT, IDS_COL_JUDGE},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HANDLER,
		true, LVCFMT_LEFT, IDS_COL_HANDLER},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CONDITIONS,
		true, LVCFMT_LEFT, IDS_COL_CONDITIONS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COURSE_FAULTS,
		true, LVCFMT_RIGHT, IDS_COL_COURSEFAULTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TIME,
		true, LVCFMT_RIGHT, IDS_COL_TIME},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YARDS,
		true, LVCFMT_RIGHT, IDS_COL_YARDS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YPS,
		false, LVCFMT_RIGHT, IDS_COL_YPS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCT,
		true, LVCFMT_RIGHT, IDS_COL_SCT},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TOTAL_FAULTS,
		false, LVCFMT_RIGHT, IDS_COL_TOTAL_FAULTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_OPENING,
		true, LVCFMT_RIGHT, IDS_COL_REQ_OPENING},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_CLOSING,
		true, LVCFMT_RIGHT, IDS_COL_REQ_CLOSING},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPENING,
		true, LVCFMT_RIGHT, IDS_COL_OPENING},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLOSING,
		true, LVCFMT_RIGHT, IDS_COL_CLOSING},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_POINTS,
		true, LVCFMT_RIGHT, IDS_COL_REQ_POINTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_POINTS,
		true, LVCFMT_RIGHT, IDS_COL_POINTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_PLACE,
		true, LVCFMT_CENTER, IDS_COL_PLACE},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_IN_CLASS,
		true, LVCFMT_CENTER, IDS_COL_IN_CLASS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DOGSQD,
		true, LVCFMT_CENTER, IDS_COL_Q_D},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_Q,
		true, LVCFMT_CENTER, IDS_COL_Q},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCORE,
		false, LVCFMT_RIGHT, IDS_COL_SCORE},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TITLE_POINTS,
		false, LVCFMT_RIGHT, IDS_COL_TITLE_PTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COMMENTS,
		true, LVCFMT_LEFT, IDS_COL_COMMENTS},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_FAULTS,
		true, LVCFMT_LEFT, IDS_COL_FAULTS},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_START_DATE,
		true, LVCFMT_LEFT, IDS_COL_START_DATE},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_END_DATE,
		true, LVCFMT_LEFT, IDS_COL_END_DATE},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_TENTATIVE,
		true, 0, IDS_COL_TENTATIVE},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_ENTERED,
		true, 0, IDS_COL_ENTERED},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_LOCATION,
		true, LVCFMT_LEFT, IDS_COL_LOCATION},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLUB,
		true, LVCFMT_LEFT, IDS_COL_CLUB},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_VENUE,
		true, LVCFMT_LEFT, IDS_COL_VENUE},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_OPENS,
		true, LVCFMT_LEFT, IDS_COL_OPENS},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLOSES,
		true, LVCFMT_LEFT, IDS_COL_CLOSES},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_NOTES,
		true, LVCFMT_LEFT, IDS_COL_NOTES},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_DATE,
		true, LVCFMT_LEFT, IDS_COL_DATE},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NAME,
		true, LVCFMT_LEFT, IDS_COL_NAME},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NOTES,
		true, LVCFMT_LEFT, IDS_COL_NOTES},

	{CAgilityBookOptions::eView,
		IO_TREE_DOG_REGNAME,
		false, 0, IDS_COL_REG_NAME},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_CALLNAME,
		false, 0, IDS_COL_CALLNAME},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_BREED,
		false, 0, IDS_COL_BREED},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_DOB,
		false, 0, IDS_COL_DOB},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_AGE,
		false, 0, IDS_COL_AGE},

	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_START,
		false, 0, IDS_COL_START_DATE},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_END,
		false, 0, IDS_COL_END_DATE},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_CLUB,
		false, 0, IDS_COL_CLUB},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_VENUE,
		false, 0, IDS_COL_VENUE},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_LOCATION,
		false, 0, IDS_COL_LOCATION},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_NOTES,
		false, 0, IDS_COL_NOTES},

	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DATE,
		false, 0, IDS_COL_DATE},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_Q,
		false, 0, IDS_COL_Q},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_EVENT,
		false, 0, IDS_COL_EVENT},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DIVISION,
		false, 0, IDS_COL_DIVISION},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_LEVEL,
		false, 0, IDS_COL_LEVEL},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_HEIGHT,
		false, 0, IDS_COL_HEIGHT},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_SUBNAME,
		true, LVCFMT_LEFT, IDS_COL_SUBNAME},
	{0, IO_RESERVED, false, 0, 0},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SPEED,
		false, LVCFMT_LEFT, IDS_COL_SPEED},

	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SUBJECT,
		false, 0, IDS_COL_SUBJECT},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_DATE,
		false, 0, IDS_COL_START_DATE},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_TIME,
		false, 0, IDS_COL_START_TIME},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_DATE,
		false, 0, IDS_COL_END_DATE},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_TIME,
		false, 0, IDS_COL_END_TIME},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ALLDAY,
		false, 0, IDS_COL_ALL_DAY_EVENT},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER,
		false, 0, IDS_COL_REMINDER},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_DATE,
		false, 0, IDS_COL_REMINDER_DATE},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_TIME,
		false, 0, IDS_COL_REMINDER_TIME},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ORGANIZER,
		false, 0, IDS_COL_ORGANIZER},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REQ_ATTENDEES,
		false, 0, IDS_COL_REQ_ATTENDEES},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_OPT_ATTENDEES,
		false, 0, IDS_COL_OPT_ATTENDEES},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_RESOURCES,
		false, 0, IDS_COL_RESOURCES},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_BILLING,
		false, 0, IDS_COL_BILLING},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_CATEGORIES,
		false, 0, IDS_COL_CATEGORIES},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_DESCRIPTION,
		false, 0, IDS_COL_DESCRIPTION},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_LOCATION,
		false, 0, IDS_COL_LOCATION},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_MILEAGE,
		false, 0, IDS_COL_MILEAGE},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIORITY,
		false, 0, IDS_COL_PRIORITY},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIVATE,
		false, 0, IDS_COL_PRIVATE},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SENSITIVITY,
		false, 0, IDS_COL_SENSITIVITY},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SHOW_TIME_AS,
		false, 0, IDS_COL_SHOW_TIME_AS},

	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SUBJECT,
		false, 0, IDS_COL_SUBJECT},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_START_DATE,
		false, 0, IDS_COL_START_DATE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_DUE_DATE,
		false, 0, IDS_COL_DUE_DATE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER,
		false, 0, IDS_COL_REMINDER},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_DATE,
		false, 0, IDS_COL_REMINDER_DATE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_TIME,
		false, 0, IDS_COL_REMINDER_TIME},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETED_DATE,
		false, 0, IDS_COL_COMPLETED_DATE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETE,
		false, 0, IDS_COL_COMPLETE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_TOTAL_WORK,
		false, 0, IDS_COL_TOTAL_WORK},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ACTUAL_WORK,
		false, 0, IDS_COL_ACTUAL_WORK},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_BILLING,
		false, 0, IDS_COL_BILLING},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CATEGORIES,
		false, 0, IDS_COL_CATEGORIES},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPANIES,
		false, 0, IDS_COL_COMPANIES},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CONTACTS,
		false, 0, IDS_COL_CONTACTS},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_MILEAGE,
		false, 0, IDS_COL_MILEAGE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_NOTES,
		false, 0, IDS_COL_NOTES},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIORITY,
		false, 0, IDS_COL_PRIORITY},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIVATE,
		false, 0, IDS_COL_PRIVATE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ROLE,
		false, 0, IDS_COL_ROLE},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SCH_PRIORITY,
		false, 0, IDS_COL_SCH_PRIORITY},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SENSITIVITY,
		false, 0, IDS_COL_SENSITIVITY},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_STATUS,
		false, 0, IDS_COL_STATUS},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_MIN_YPS,
		false, LVCFMT_RIGHT, IDS_COL_MIN_YPS},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_DRAWS,
		true, LVCFMT_LEFT, IDS_COL_DRAWS},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OBSTACLES,
		false, LVCFMT_RIGHT, IDS_COL_OBSTACLES},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPS,
		false, LVCFMT_RIGHT, IDS_COL_OPS},
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


bool CDlgAssignColumns::GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<int>& values)
{
	bool bOk = false;
	if (0 <= idxColumn && idxColumn < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].bValid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::GetColumnOrder(eOrder, idxColumn, values);
		}
	}
	return bOk;
}


bool CDlgAssignColumns::SetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<int> const& values)
{
	bool bOk = false;
	if (0 <= idxColumn && idxColumn < IO_TYPE_MAX)
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
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,            IO_RUNS_OBSTACLES,      IO_RUNS_OPS,
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
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,            IO_RUNS_OBSTACLES,      IO_RUNS_OPS,
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
	IO_CAL_VENUE,			IO_CAL_OPENS,			IO_CAL_DRAWS,
	IO_CAL_CLOSES,			IO_CAL_NOTES,
-1};
static int const idxTraining[] = {
	IO_LOG_DATE,			IO_LOG_NAME,			IO_LOG_SUBNAME,
	IO_LOG_NOTES,
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
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,            IO_RUNS_OBSTACLES,      IO_RUNS_OPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_REQ_OPENING,
	IO_RUNS_REQ_CLOSING,	IO_RUNS_OPENING,		IO_RUNS_CLOSING,
	IO_RUNS_REQ_POINTS,		IO_RUNS_POINTS,			IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_SPEED,
	IO_RUNS_COMMENTS,		IO_RUNS_FAULTS,
-1};
static int const idxViewCalendarList[] = {
	IO_CAL_START_DATE,		IO_CAL_END_DATE,		IO_CAL_LOCATION,
	IO_CAL_CLUB,			IO_CAL_VENUE,			IO_CAL_OPENS,
	IO_CAL_DRAWS,			IO_CAL_CLOSES,			IO_CAL_NOTES,
-1};
static int const idxViewTrainingList[] = {
	IO_LOG_DATE,			IO_LOG_NAME,			IO_LOG_SUBNAME,
	IO_LOG_NOTES,
-1};
static int const idxCalendarAppt[] = {
	IO_CAL_APPT_SUBJECT,	IO_CAL_APPT_START_DATE,	IO_CAL_APPT_START_TIME,
	IO_CAL_APPT_END_DATE,	IO_CAL_APPT_END_TIME,	IO_CAL_APPT_ALLDAY,
	IO_CAL_APPT_REMINDER,	IO_CAL_APPT_REMINDER_DATE, IO_CAL_APPT_REMINDER_TIME,
	IO_CAL_APPT_ORGANIZER,	IO_CAL_APPT_REQ_ATTENDEES, IO_CAL_APPT_OPT_ATTENDEES,
	IO_CAL_APPT_RESOURCES,	IO_CAL_APPT_BILLING,	IO_CAL_APPT_CATEGORIES,
	IO_CAL_APPT_DESCRIPTION, IO_CAL_APPT_LOCATION,	IO_CAL_APPT_MILEAGE,
	IO_CAL_APPT_PRIORITY,	IO_CAL_APPT_PRIVATE,	IO_CAL_APPT_SENSITIVITY,
	IO_CAL_APPT_SHOW_TIME_AS,
-1};
static int const idxCalendarTask[] = {
	IO_CAL_TASK_SUBJECT,	IO_CAL_TASK_START_DATE,		IO_CAL_TASK_DUE_DATE,
	IO_CAL_TASK_REMINDER,	IO_CAL_TASK_REMINDER_DATE,	IO_CAL_TASK_REMINDER_TIME,
	IO_CAL_TASK_COMPLETED_DATE, IO_CAL_TASK_COMPLETE,	IO_CAL_TASK_TOTAL_WORK,
	IO_CAL_TASK_ACTUAL_WORK, IO_CAL_TASK_BILLING,		IO_CAL_TASK_CATEGORIES,
	IO_CAL_TASK_COMPANIES,	IO_CAL_TASK_CONTACTS,		IO_CAL_TASK_MILEAGE,
	IO_CAL_TASK_NOTES,		IO_CAL_TASK_PRIORITY,		IO_CAL_TASK_PRIVATE,
	IO_CAL_TASK_ROLE,		IO_CAL_TASK_SCH_PRIORITY,	IO_CAL_TASK_SENSITIVITY,
	IO_CAL_TASK_STATUS,
-1};
static int const idxViewCalendar[] = {
	IO_CAL_LOCATION,	IO_CAL_CLUB,	IO_CAL_VENUE,	IO_CAL_NOTES,
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
	idxViewTrainingList,
	idxCalendarAppt,
	idxCalendarTask,
	idxViewCalendar,
};

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns dialog

CDlgAssignColumns::CDlgAssignColumns(
		CAgilityBookOptions::ColumnOrder eOrder,
		CWnd* pParent,
		CAgilityBookDoc* pDoc,
		int initSelection)
	: CDlgBaseDialog(CDlgAssignColumns::IDD, pParent)
	, m_ctrlType(false)
	, m_ctrlAvailable(false)
	, m_ctrlColumns(false)
	, m_pDoc(pDoc)
	, m_eOrder(eOrder)
	, m_initSelection(initSelection)
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
	CDlgBaseDialog::DoDataExchange(pDX);
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


BEGIN_MESSAGE_MAP(CDlgAssignColumns, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgAssignColumns)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_ASSIGN_TYPE, OnItemchanged)
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
	int index = m_ctrlType.GetSelection();
	int idxType = -1;
	if (0 <= index)
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
	int index = m_ctrlType.GetSelection();
	int idxType = -1;
	if (0 <= index)
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


int CALLBACK CompareTypes(LPARAM lParam1, LPARAM lParam2, LPARAM lParam3)
{
	if (lParam1 >= 0 && lParam1 < IO_TYPE_MAX
	&& lParam2 >= 0 && lParam2 < IO_TYPE_MAX)
	{
		if (sc_Types[lParam1].sortOrder < sc_Types[lParam2].sortOrder)
			return -1;
		else if (sc_Types[lParam1].sortOrder > sc_Types[lParam2].sortOrder)
			return 1;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns message handlers

BOOL CDlgAssignColumns::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlType.SetExtendedStyle(m_ctrlType.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
	CString col;
	col.LoadString(IDS_COL_RUNTYPE);
	m_ctrlType.InsertColumn(0, col);
	col.LoadString(IDS_COL_DESCRIPTION);
	m_ctrlType.InsertColumn(1, col);
	int index;
#ifdef _DEBUG
	for (index = 0; index < IO_MAX; ++index)
	{
		assert(sc_FieldNames[index].index == index);
	}
#endif
	int idxSelect = 0;
	for (index = 0; index < IO_TYPE_MAX; ++index)
	{
		assert(sc_Types[index].index == index);
		if (!(sc_Types[index].bValid & m_eOrder))
			continue;
		CString str;
		str.LoadString(sc_Types[index].name);
		int idx = m_ctrlType.InsertItem(index, str);
		if (0 <= idx)
		{
			str.LoadString(sc_Types[index].desc);
			m_ctrlType.SetItemText(idx, 1, str);
			m_ctrlType.SetItemData(idx, index);
			if (m_initSelection == index)
				idxSelect = idx;
		}
	}
	m_ctrlType.SetColumnWidth(0, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlType.SetColumnWidth(1, LVSCW_AUTOSIZE_USEHEADER);
	m_ctrlType.SetSelection(idxSelect);
	if (m_eOrder == CAgilityBookOptions::eView)
		m_ctrlType.SortItems(CompareTypes, 0);
	m_ctrlType.EnsureVisible(idxSelect, FALSE);
	//FillColumns();
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgAssignColumns::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	FillColumns();
	*pResult = 0;
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
	if (m_pDoc && CAgilityBookOptions::eView == m_eOrder)
		m_pDoc->UpdateAllViews(NULL, UPDATE_CUSTOMIZE);
	CDlgBaseDialog::OnOK();
}
