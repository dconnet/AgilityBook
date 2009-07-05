/*
 * Copyright © 2003-2009 David Connet. All Rights Reserved.
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
 * @li 2009-03-12 DRC Fixed a bug that assumed items in sc_Fields lists had ids
 *                that were in ascending order. Adding verified broke that.
 *                (Only issue was 'remove' would put item into wrong place in
 *                available list)
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-08-11 DRC Added verified column to trial in tree.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "DlgAssignColumns.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include <wx/listctrl.h>

/////////////////////////////////////////////////////////////////////////////

static struct
{
	unsigned int valid;
	int index;
	int sortOrder; // only used for view types
	wxChar const* name;
	wxChar const* desc;
} const sc_Types[] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_FAULTS_TIME, 0,
		wxT("IDS_ASSCOL_RUNS_FAULTS_TIME"),
		wxT("IDS_ASSCOL_RUNS_FAULTS_TIME_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_TIME_FAULTS, 0,
		wxT("IDS_ASSCOL_RUNS_TIME_FAULTS"),
		wxT("IDS_ASSCOL_RUNS_TIME_FAULTS_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_OPEN_CLOSE, 0,
		wxT("IDS_ASSCOL_RUNS_OPEN_CLOSE"),
		wxT("IDS_ASSCOL_RUNS_OPEN_CLOSE_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_POINTS, 0,
		wxT("IDS_ASSCOL_RUNS_POINTS"),
		wxT("IDS_ASSCOL_RUNS_POINTS_DESC")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_TYPE_CALENDAR, 0,
		wxT("IDS_ASSCOL_CALENDAR"),
		wxT("IDS_ASSCOL_CALENDAR_DESC")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport,
		IO_TYPE_TRAINING, 0,
		wxT("IDS_ASSCOL_TRAINING"),
		wxT("IDS_ASSCOL_TRAINING_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_DOG, 1,
		wxT("IDS_ASSCOL_VIEW_TREE_DOG"),
		wxT("IDS_ASSCOL_VIEW_TREE_DOG_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_TRIAL, 2,
		wxT("IDS_ASSCOL_VIEW_TREE_TRIAL"),
		wxT("IDS_ASSCOL_VIEW_TREE_TRIAL_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_RUN, 3,
		wxT("IDS_ASSCOL_VIEW_TREE_RUN"),
		wxT("IDS_ASSCOL_VIEW_TREE_RUN_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_RUNS_LIST, 4,
		wxT("IDS_ASSCOL_VIEW_RUNS_LIST"),
		wxT("IDS_ASSCOL_VIEW_RUNS_LIST_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR_LIST, 5,
		wxT("IDS_ASSCOL_VIEW_CALENDAR_LIST"),
		wxT("IDS_ASSCOL_VIEW_CALENDAR_LIST_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TRAINING_LIST, 7,
		wxT("IDS_ASSCOL_VIEW_TRAINING_LIST"),
		wxT("IDS_ASSCOL_VIEW_TRAINING_LIST_DESC")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_TYPE_CALENDAR_APPT, 0,
		wxT("IDS_ASSCOL_CALENDAR_APPT"),
		wxT("IDS_ASSCOL_CALENDAR_APPT_DESC")},
	{CAgilityBookOptions::eCalExportTask,
		IO_TYPE_CALENDAR_TASK, 0,
		wxT("IDS_ASSCOL_CALENDAR_TASK"),
		wxT("IDS_ASSCOL_CALENDAR_TASK_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR, 6,
		wxT("IDS_ASSCOL_VIEW_CALENDAR"),
		wxT("IDS_ASSCOL_VIEW_CALENDAR_DESC")},
	// Note: Remember to update sc_Fields when adding a type.
};


// This simply defines the fields and what those fields are valid for.
// sc_Fields defines what fields will be shown (and the order)
// NOTE: Currently, the fields must be in index order - so any new fields must
//       be added at the bottom. The physical ordering is not controlled by the
//       order in this structure.
static struct
{
	unsigned int valid;
	int index;
	bool bImportable;
	int fmt; // Only applicable to eView items
	wxChar const* name;
} const sc_FieldNames[IO_MAX] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REG_NAME,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_REG_NAME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CALL_NAME,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_CALLNAME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DATE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_DATE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_VENUE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLUB,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LOCATION,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TRIAL_NOTES,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_TRIALNOTES")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DIVISION,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_DIVISION")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LEVEL,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_LEVEL")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_EVENT,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_EVENT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HEIGHT,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_HEIGHT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_JUDGE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_JUDGE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HANDLER,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_HANDLER")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CONDITIONS,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_CONDITIONS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COURSE_FAULTS,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_COURSEFAULTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TIME,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_TIME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YARDS,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_YARDS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YPS,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_YPS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCT,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_SCT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TOTAL_FAULTS,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_TOTAL_FAULTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_OPENING,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_REQ_OPENING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_CLOSING,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_REQ_CLOSING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPENING,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_OPENING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLOSING,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_CLOSING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_POINTS,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_REQ_POINTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_POINTS,
		true, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_POINTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_PLACE,
		true, wxLIST_FORMAT_CENTRE, wxT("IDS_COL_PLACE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_IN_CLASS,
		true, wxLIST_FORMAT_CENTRE, wxT("IDS_COL_IN_CLASS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DOGSQD,
		true, wxLIST_FORMAT_CENTRE, wxT("IDS_COL_Q_D")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_Q,
		true, wxLIST_FORMAT_CENTRE, wxT("IDS_COL_Q")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCORE,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_SCORE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TITLE_POINTS,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_TITLE_PTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COMMENTS,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_COMMENTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_FAULTS,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_FAULTS")},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_START_DATE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_END_DATE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_TENTATIVE,
		true, 0, wxT("IDS_COL_TENTATIVE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_ENTERED,
		true, 0, wxT("IDS_COL_ENTERED")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_LOCATION,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLUB,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_VENUE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_OPENS,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_OPENS")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLOSES,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_CLOSES")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_NOTES,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_DATE,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_DATE")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NAME,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_NAME")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NOTES,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eView,
		IO_TREE_DOG_REGNAME,
		false, 0, wxT("IDS_COL_REG_NAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_CALLNAME,
		false, 0, wxT("IDS_COL_CALLNAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_BREED,
		false, 0, wxT("IDS_COL_BREED")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_DOB,
		false, 0, wxT("IDS_COL_DOB")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_AGE,
		false, 0, wxT("IDS_COL_AGE")},

	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_START,
		false, 0, wxT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_END,
		false, 0, wxT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_CLUB,
		false, 0, wxT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_VENUE,
		false, 0, wxT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_LOCATION,
		false, 0, wxT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_NOTES,
		false, 0, wxT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DATE,
		false, 0, wxT("IDS_COL_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_Q,
		false, 0, wxT("IDS_COL_Q")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_EVENT,
		false, 0, wxT("IDS_COL_EVENT")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DIVISION,
		false, 0, wxT("IDS_COL_DIVISION")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_LEVEL,
		false, 0, wxT("IDS_COL_LEVEL")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_HEIGHT,
		false, 0, wxT("IDS_COL_HEIGHT")},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_SUBNAME,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_SUBNAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_VERIFIED,
		false, 0, wxT("IDS_COL_VERIFIED")},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SPEED,
		false, wxLIST_FORMAT_LEFT, wxT("IDS_COL_SPEED")},

	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SUBJECT,
		false, 0, wxT("IDS_COL_SUBJECT")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_DATE,
		false, 0, wxT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_TIME,
		false, 0, wxT("IDS_COL_START_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_DATE,
		false, 0, wxT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_TIME,
		false, 0, wxT("IDS_COL_END_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ALLDAY,
		false, 0, wxT("IDS_COL_ALL_DAY_EVENT")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER,
		false, 0, wxT("IDS_COL_REMINDER")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_DATE,
		false, 0, wxT("IDS_COL_REMINDER_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_TIME,
		false, 0, wxT("IDS_COL_REMINDER_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ORGANIZER,
		false, 0, wxT("IDS_COL_ORGANIZER")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REQ_ATTENDEES,
		false, 0, wxT("IDS_COL_REQ_ATTENDEES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_OPT_ATTENDEES,
		false, 0, wxT("IDS_COL_OPT_ATTENDEES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_RESOURCES,
		false, 0, wxT("IDS_COL_RESOURCES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_BILLING,
		false, 0, wxT("IDS_COL_BILLING")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_CATEGORIES,
		false, 0, wxT("IDS_COL_CATEGORIES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_DESCRIPTION,
		false, 0, wxT("IDS_COL_DESCRIPTION")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_LOCATION,
		false, 0, wxT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_MILEAGE,
		false, 0, wxT("IDS_COL_MILEAGE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIORITY,
		false, 0, wxT("IDS_COL_PRIORITY")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIVATE,
		false, 0, wxT("IDS_COL_PRIVATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SENSITIVITY,
		false, 0, wxT("IDS_COL_SENSITIVITY")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SHOW_TIME_AS,
		false, 0, wxT("IDS_COL_SHOW_TIME_AS")},

	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SUBJECT,
		false, 0, wxT("IDS_COL_SUBJECT")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_START_DATE,
		false, 0, wxT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_DUE_DATE,
		false, 0, wxT("IDS_COL_DUE_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER,
		false, 0, wxT("IDS_COL_REMINDER")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_DATE,
		false, 0, wxT("IDS_COL_REMINDER_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_TIME,
		false, 0, wxT("IDS_COL_REMINDER_TIME")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETED_DATE,
		false, 0, wxT("IDS_COL_COMPLETED_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETE,
		false, 0, wxT("IDS_COL_COMPLETE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_TOTAL_WORK,
		false, 0, wxT("IDS_COL_TOTAL_WORK")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ACTUAL_WORK,
		false, 0, wxT("IDS_COL_ACTUAL_WORK")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_BILLING,
		false, 0, wxT("IDS_COL_BILLING")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CATEGORIES,
		false, 0, wxT("IDS_COL_CATEGORIES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPANIES,
		false, 0, wxT("IDS_COL_COMPANIES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CONTACTS,
		false, 0, wxT("IDS_COL_CONTACTS")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_MILEAGE,
		false, 0, wxT("IDS_COL_MILEAGE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_NOTES,
		false, 0, wxT("IDS_COL_NOTES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIORITY,
		false, 0, wxT("IDS_COL_PRIORITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIVATE,
		false, 0, wxT("IDS_COL_PRIVATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ROLE,
		false, 0, wxT("IDS_COL_ROLE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SCH_PRIORITY,
		false, 0, wxT("IDS_COL_SCH_PRIORITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SENSITIVITY,
		false, 0, wxT("IDS_COL_SENSITIVITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_STATUS,
		false, 0, wxT("IDS_COL_STATUS")},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_MIN_YPS,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_MIN_YPS")},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_DRAWS,
		true, wxLIST_FORMAT_LEFT, wxT("IDS_COL_DRAWS")},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OBSTACLES,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_OBSTACLES")},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPS,
		false, wxLIST_FORMAT_RIGHT, wxT("IDS_COL_OPS")},
};


int CDlgAssignColumns::GetFormatFromColumnID(long column)
{
	int fmt = 0;
	if (0 <= column && column < IO_MAX)
		fmt = sc_FieldNames[column].fmt;
	return fmt;
}


wxString CDlgAssignColumns::GetNameFromColumnID(long column)
{
	wxString name;
	if (0 <= column && column < IO_MAX)
		name = wxGetTranslation(sc_FieldNames[column].name);
	return name;
}


// Only called during initialization/reset
bool CDlgAssignColumns::GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long>& values,
		bool bDefaultValues)
{
	bool bOk = false;
	if (0 <= idxColumn && idxColumn < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].valid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::GetColumnOrder(eOrder, idxColumn, values, bDefaultValues);
		}
	}
	return bOk;
}


bool CDlgAssignColumns::SetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long> const& values)
{
	bool bOk = false;
	if (0 <= idxColumn && idxColumn < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].valid & eOrder)
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
	IO_TREE_TRIAL_VERIFIED,	IO_TREE_TRIAL_START,	IO_TREE_TRIAL_END,
	IO_TREE_TRIAL_CLUB,		IO_TREE_TRIAL_VENUE,	IO_TREE_TRIAL_LOCATION,
	IO_TREE_TRIAL_NOTES,
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

class ColumnData: public wxClientData 
{
public:
	ColumnData(
			size_t index,
			long data)
		: m_Index(index)
		, m_Data(data)
	{
	}
	ColumnData(ColumnData const& rhs)
		: m_Index(rhs.m_Index)
		, m_Data(rhs.m_Data)
	{
	}
	size_t m_Index; // Sort index for available fields
	long m_Data; // Column identifier
};

/////////////////////////////////////////////////////////////////////////////
// CDlgAssignColumns dialog

int wxCALLBACK CompareTypes(long lParam1, long lParam2, long lParam3)
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


BEGIN_EVENT_TABLE(CDlgAssignColumns, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgAssignColumns::OnOk)
END_EVENT_TABLE()


CDlgAssignColumns::CDlgAssignColumns(
		CAgilityBookOptions::ColumnOrder eOrder,
		wxWindow* pParent,
		CAgilityBookDoc* pDoc,
		long initSelection)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_eOrder(eOrder)
	, m_initSelection(initSelection)
	, m_bIncludeBlank(false)
	, m_Columns()
	, m_ctrlType(NULL)
	, m_ctrlAvailable(NULL)
	, m_ctrlColumns(NULL)
	, m_btnAdd(NULL)
	, m_btnRemove(NULL)
	, m_btnUp(NULL)
	, m_btnDown(NULL)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);
	Create(pParent, wxID_ANY, _("IDD_ASSIGN_COLUMNS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

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

	// Controls (these are done first to control tab order)

	m_ctrlType = new wxListView(this, wxID_ANY,
		wxDefaultPosition, wxSize(-1, 100), wxLC_REPORT|wxLC_SINGLE_SEL|wxBORDER);
	m_ctrlType->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgAssignColumns::OnItemchanged), NULL, this);
	m_ctrlType->SetHelpText(_("HIDC_ASSIGN_TYPE"));
	m_ctrlType->SetToolTip(_("HIDC_ASSIGN_TYPE"));
	m_ctrlType->InsertColumn(0, _("IDS_COL_RUNTYPE"));
	m_ctrlType->InsertColumn(1, _("IDS_COL_DESCRIPTION"));
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
		if (!(sc_Types[index].valid & m_eOrder))
			continue;
		int idx = m_ctrlType->InsertItem(index, wxGetTranslation(sc_Types[index].name));
		if (0 <= idx)
		{
			m_ctrlType->SetItemData(idx, index);
			wxListItem info;
			info.SetId(idx);
			info.SetColumn(1);
			info.SetMask(wxLIST_MASK_TEXT);
			info.SetText(wxGetTranslation(sc_Types[index].desc));
			m_ctrlType->SetItem(info);
			if (m_initSelection == index)
				idxSelect = idx;
		}
	}
	m_ctrlType->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlType->SetColumnWidth(1, wxLIST_AUTOSIZE_USEHEADER);

	wxStaticText* textAvail = new wxStaticText(this, wxID_ANY,
		_("IDC_ASSIGN_AVAILABLE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textAvail->Wrap(-1);

	m_ctrlAvailable = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(-1, 250), 0, NULL, 0);
	m_ctrlAvailable->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgAssignColumns::OnSelchangeAvailable), NULL, this);
	m_ctrlAvailable->SetHelpText(_("HIDC_ASSIGN_AVAILABLE"));
	m_ctrlAvailable->SetToolTip(_("HIDC_ASSIGN_AVAILABLE"));

	wxStaticText* textSpacer = new wxStaticText(this, wxID_ANY, wxT("Hidden"),
		wxDefaultPosition, wxDefaultSize, 0);
	textSpacer->Wrap(-1);
	textSpacer->Show(false);

	m_btnAdd = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_ADD"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnAdd->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAssignColumns::OnAdd), NULL, this);
	m_btnAdd->SetHelpText(_("HIDC_ASSIGN_ADD"));
	m_btnAdd->SetToolTip(_("HIDC_ASSIGN_ADD"));

	m_btnRemove = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnRemove->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAssignColumns::OnRemove), NULL, this);
	m_btnRemove->SetHelpText(_("HIDC_ASSIGN_DELETE"));
	m_btnRemove->SetToolTip(_("HIDC_ASSIGN_DELETE"));

	m_btnUp = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAssignColumns::OnMoveUp), NULL, this);
	m_btnUp->SetHelpText(_("HIDC_ASSIGN_MOVE_UP"));
	m_btnUp->SetToolTip(_("HIDC_ASSIGN_MOVE_UP"));

	m_btnDown = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_btnDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAssignColumns::OnMoveDown), NULL, this);
	m_btnDown->SetHelpText(_("HIDC_ASSIGN_MOVE_DOWN"));
	m_btnDown->SetToolTip(_("HIDC_ASSIGN_MOVE_DOWN"));

	wxButton* btnReset = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_RESET"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnReset->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgAssignColumns::OnReset), NULL, this);
	btnReset->SetHelpText(_("HIDC_ASSIGN_RESET"));
	btnReset->SetToolTip(_("HIDC_ASSIGN_RESET"));

	wxStaticText* textOrder = new wxStaticText(this, wxID_ANY,
		_("IDC_ASSIGN_COLUMNS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textOrder->Wrap(-1);

	m_ctrlColumns = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(-1, 250), 0, NULL, 0);
	m_ctrlColumns->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgAssignColumns::OnSelchangeColumns), NULL, this);
	m_ctrlColumns->SetHelpText(_("HIDC_ASSIGN_COLUMNS"));
	m_ctrlColumns->SetToolTip(_("HIDC_ASSIGN_COLUMNS"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlType, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerFields = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerAvail = new wxBoxSizer(wxVERTICAL);
	sizerAvail->Add(textAvail, 0, wxLEFT, 5);
	sizerAvail->Add(m_ctrlAvailable, 1, wxALL|wxEXPAND, 5);

	sizerFields->Add(sizerAvail, 1, wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(textSpacer, 0, 0, 5);
	sizerBtns->Add(m_btnAdd, 0, wxALL, 5);
	sizerBtns->Add(m_btnRemove, 0, wxALL, 5);
	sizerBtns->Add(m_btnUp, 0, wxALL, 5);
	sizerBtns->Add(m_btnDown, 0, wxALL, 5);
	sizerBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerBtns->Add(btnReset, 0, wxALL, 5);

	sizerFields->Add(sizerBtns, 0, wxEXPAND, 5);

	wxBoxSizer* sizerOrder = new wxBoxSizer(wxVERTICAL);
	sizerOrder->Add(textOrder, 0, wxLEFT, 5);
	sizerOrder->Add(m_ctrlColumns, 1, wxALL|wxEXPAND, 5);

	sizerFields->Add(sizerOrder, 1, wxEXPAND, 5);

	bSizer->Add(sizerFields, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlType->Select(idxSelect); // Causes change event that fills columns
	if (m_eOrder == CAgilityBookOptions::eView)
		m_ctrlType->SortItems(CompareTypes, 0);
	m_ctrlType->Focus(idxSelect);
	m_ctrlType->SetFocus();
}


ColumnData* CDlgAssignColumns::GetAvailableData(long idx) const
{
	return dynamic_cast<ColumnData*>(m_ctrlAvailable->GetClientObject(idx));
}


ColumnData* CDlgAssignColumns::GetInUseData(long idx) const
{
	return dynamic_cast<ColumnData*>(m_ctrlColumns->GetClientObject(idx));
}


void CDlgAssignColumns::FillColumns()
{
	m_ctrlAvailable->Clear();
	m_ctrlColumns->Clear();
	int index = m_ctrlType->GetFirstSelected();
	long idxType = -1;
	if (0 <= index)
		idxType = static_cast<long>(m_ctrlType->GetItemData(index));
	if (0 <= idxType)
	{
		wxString blank(_("IDS_BLANK_COLUMN"));
		size_t i;
		bool bInUse[IO_MAX];
		for (i = 0; i < IO_MAX; ++i)
			bInUse[i] = false;
		for (i = 0; i < m_Columns[idxType].size(); ++i)
		{
			if (0 <= m_Columns[idxType][i])
			{
				bInUse[m_Columns[idxType][i]] = true;
				wxString name = GetNameFromColumnID(m_Columns[idxType][i]);
				int idx = m_ctrlColumns->Append(name);
				if (0 <= idx)
					m_ctrlColumns->SetClientObject(idx, new ColumnData(i, m_Columns[idxType][i]));
			}
			else
			{
				if (m_bIncludeBlank)
				{
					int idx = m_ctrlColumns->Append(blank);
					if (0 <= idx)
						m_ctrlColumns->SetClientObject(idx, new ColumnData(0, -1));
				}
			}
		}
		if (m_bIncludeBlank)
		{
			int idx = m_ctrlAvailable->Append(blank);
			if (0 <= idx)
				m_ctrlAvailable->SetClientObject(idx, new ColumnData(0, -1));
		}
		bool bImport = (CAgilityBookOptions::eRunsImport == m_eOrder
			|| CAgilityBookOptions::eCalImport == m_eOrder
			|| CAgilityBookOptions::eLogImport == m_eOrder);
		for (i = 0; 0 <= sc_Fields[idxType][i]; ++i)
		{
			if (
			!(sc_FieldNames[sc_Fields[idxType][i]].valid & m_eOrder)
			|| (bImport && !sc_FieldNames[sc_Fields[idxType][i]].bImportable)
			|| bInUse[sc_Fields[idxType][i]])
				continue;
			wxString name = GetNameFromColumnID(sc_Fields[idxType][i]);
			int idx = m_ctrlAvailable->Append(name);
			if (0 <= idx)
				m_ctrlAvailable->SetClientObject(idx, new ColumnData(i, sc_Fields[idxType][i]));
		}
	}
	UpdateButtons();
}


void CDlgAssignColumns::UpdateColumnVector()
{
	int index = m_ctrlType->GetFirstSelected();
	long idxType = -1;
	if (0 <= index)
		idxType = static_cast<long>(m_ctrlType->GetItemData(index));
	if (0 <= idxType)
	{
		m_Columns[idxType].clear();
		m_Columns[idxType].reserve(m_ctrlColumns->GetCount());
		for (unsigned int idx = 0; idx < m_ctrlColumns->GetCount(); ++idx)
		{
			m_Columns[idxType].push_back(GetInUseData(idx)->m_Data);
		}
	}
}


void CDlgAssignColumns::UpdateButtons()
{
	int idxAvail = m_ctrlAvailable->GetSelection();
	int idxCol = m_ctrlColumns->GetSelection();
	m_btnAdd->Enable(0 <= idxAvail && 0 < m_ctrlAvailable->GetCount());
	m_btnRemove->Enable(0 <= idxCol && 0 < m_ctrlColumns->GetCount());
	m_btnUp->Enable(0 <= idxCol && 1 < m_ctrlColumns->GetCount() && 0 != idxCol);
	m_btnDown->Enable(0 <= idxCol && 1 < m_ctrlColumns->GetCount() && static_cast<int>(m_ctrlColumns->GetCount())-1 != idxCol);
}


void CDlgAssignColumns::OnItemchanged(wxListEvent& evt)
{
	FillColumns();
}


void CDlgAssignColumns::OnSelchangeAvailable(wxCommandEvent& evt)
{
	UpdateButtons();
}


void CDlgAssignColumns::OnSelchangeColumns(wxCommandEvent& evt)
{
	UpdateButtons();
}


void CDlgAssignColumns::OnAdd(wxCommandEvent& evt)
{
	int idxAvail = m_ctrlAvailable->GetSelection();
	if (0 <= idxAvail)
	{
		wxString str = m_ctrlAvailable->GetString(idxAvail);
		ColumnData* pData = GetAvailableData(idxAvail);
		int idxCol = m_ctrlColumns->Append(str);
		if (0 <= idxCol)
		{
			m_ctrlColumns->SetClientObject(idxCol, new ColumnData(*pData));
			m_ctrlColumns->SetSelection(idxCol);
			if (pData->m_Data >= 0)
			{
				m_ctrlAvailable->Delete(idxAvail);
				if (idxAvail == static_cast<int>(m_ctrlAvailable->GetCount()))
					--idxAvail;
				m_ctrlAvailable->SetSelection(idxAvail);
			}
		}
		UpdateColumnVector();
		UpdateButtons();
	}
}


void CDlgAssignColumns::OnRemove(wxCommandEvent& evt)
{
	unsigned int idxCol = m_ctrlColumns->GetSelection();
	if (0 <= idxCol)
	{
		wxString str = m_ctrlColumns->GetString(idxCol);
		ColumnData* pData = GetInUseData(idxCol);
		if (pData->m_Data >= 0)
		{
			unsigned int idxAvail = 0;
			if (m_bIncludeBlank)
				++idxAvail; // Skip special "blank column"
			bool bDone = false;
			for (; idxAvail < m_ctrlAvailable->GetCount(); ++idxAvail)
			{
				ColumnData* pData2 = GetAvailableData(idxAvail);
				if (pData->m_Index < pData2->m_Index)
				{
					bDone = true;
					m_ctrlAvailable->Insert(str, idxAvail);
					break;
				}
			}
			if (!bDone)
				idxAvail = m_ctrlAvailable->Append(str);
			m_ctrlAvailable->SetSelection(idxAvail);
			// Find where to insert it...
			if (0 <= idxAvail)
			{
				m_ctrlAvailable->SetClientObject(idxAvail, new ColumnData(*pData));
				m_ctrlColumns->Delete(idxCol);
				if (idxCol == m_ctrlColumns->GetCount())
					--idxCol;
				m_ctrlColumns->SetSelection(idxCol);
			}
		}
		else
		{
			m_ctrlColumns->Delete(idxCol);
			if (idxCol == m_ctrlColumns->GetCount())
				--idxCol;
			m_ctrlColumns->SetSelection(idxCol);
		}
		UpdateColumnVector();
		UpdateButtons();
	}
}


void CDlgAssignColumns::OnMoveUp(wxCommandEvent& evt)
{
	int idxCol = m_ctrlColumns->GetSelection();
	if (0 <= idxCol && 1 < m_ctrlColumns->GetCount() && 0 != idxCol)
	{
		wxString str = m_ctrlColumns->GetString(idxCol);
		ColumnData* pData = new ColumnData(*GetInUseData(idxCol));
		m_ctrlColumns->Delete(idxCol);
		m_ctrlColumns->Insert(str, --idxCol);
		m_ctrlColumns->SetClientObject(idxCol, pData);
		m_ctrlColumns->SetSelection(idxCol);
		UpdateColumnVector();
		UpdateButtons();
	}
}


void CDlgAssignColumns::OnMoveDown(wxCommandEvent& evt)
{
	unsigned int idxCol = m_ctrlColumns->GetSelection();
	if (0 <= idxCol && 1 < m_ctrlColumns->GetCount() && m_ctrlColumns->GetCount() - 1 != idxCol)
	{
		wxString str = m_ctrlColumns->GetString(idxCol);
		ColumnData* pData = new ColumnData(*GetInUseData(idxCol));
		m_ctrlColumns->Delete(idxCol);
		m_ctrlColumns->Insert(str, ++idxCol);
		m_ctrlColumns->SetClientObject(idxCol, pData);
		m_ctrlColumns->SetSelection(idxCol);
		UpdateColumnVector();
		UpdateButtons();
	}
}


void CDlgAssignColumns::OnReset(wxCommandEvent& evt)
{
	int index = m_ctrlType->GetFirstSelected();
	long idxType = -1;
	if (0 <= index)
		idxType = static_cast<long>(m_ctrlType->GetItemData(index));
	if (0 <= idxType)
	{
		GetColumnOrder(m_eOrder, idxType, m_Columns[idxType], true);
		FillColumns();
	}
}


void CDlgAssignColumns::OnOk(wxCommandEvent& evt)
{
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		SetColumnOrder(m_eOrder, i, m_Columns[i]);
	if (m_pDoc && CAgilityBookOptions::eView == m_eOrder)
	{
		CUpdateHint hint(UPDATE_CUSTOMIZE);
		m_pDoc->UpdateAllViews(NULL, &hint);
	}
	EndDialog(wxID_OK);
}
