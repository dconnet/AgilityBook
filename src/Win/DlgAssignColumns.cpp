/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
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
 * 2018-01-19 Added missing Speed points to F/T runs export fields.
 * 2014-12-30 Changed pixels to dialog units.
 * 2012-12-23 Changed default Runs view columns.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-11-04 When adding items, add after the current selection.
 * 2010-02-17 Added SubName to runs view.
 * 2009-03-12 Fixed a bug that assumed items in sc_Fields lists had ids
 *            that were in ascending order. Adding verified broke that.
 *            (Only issue was 'remove' would put item into wrong place in
 *            available list)
 * 2009-01-26 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-08-11 Added verified column to trial in tree.
 * 2003-12-30 Implemented customized text in tree.
 * 2003-12-10 Created
 */

#include "stdafx.h"
#include "DlgAssignColumns.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ListCtrl.h"
#include "Validators.h"

#include <wx/listctrl.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static struct
{
	unsigned int valid;
	int index;
	int sortOrder; // only used for view types
	wchar_t const* name;
	wchar_t const* desc;
} const sc_Types[] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_FAULTS_TIME, 0,
		arbT("IDS_ASSCOL_RUNS_FAULTS_TIME"),
		arbT("IDS_ASSCOL_RUNS_FAULTS_TIME_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_TIME_FAULTS, 0,
		arbT("IDS_ASSCOL_RUNS_TIME_FAULTS"),
		arbT("IDS_ASSCOL_RUNS_TIME_FAULTS_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_OPEN_CLOSE, 0,
		arbT("IDS_ASSCOL_RUNS_OPEN_CLOSE"),
		arbT("IDS_ASSCOL_RUNS_OPEN_CLOSE_DESC")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport,
		IO_TYPE_RUNS_POINTS, 0,
		arbT("IDS_ASSCOL_RUNS_POINTS"),
		arbT("IDS_ASSCOL_RUNS_POINTS_DESC")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_TYPE_CALENDAR, 0,
		arbT("IDS_ASSCOL_CALENDAR"),
		arbT("IDS_ASSCOL_CALENDAR_DESC")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport,
		IO_TYPE_TRAINING, 0,
		arbT("IDS_ASSCOL_TRAINING"),
		arbT("IDS_ASSCOL_TRAINING_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_DOG, 1,
		arbT("IDS_ASSCOL_VIEW_TREE_DOG"),
		arbT("IDS_ASSCOL_VIEW_TREE_DOG_DESC")},

	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_TRIAL, 2,
		arbT("IDS_ASSCOL_VIEW_TREE_TRIAL"),
		arbT("IDS_ASSCOL_VIEW_TREE_TRIAL_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TREE_RUN, 3,
		arbT("IDS_ASSCOL_VIEW_TREE_RUN"),
		arbT("IDS_ASSCOL_VIEW_TREE_RUN_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_RUNS_LIST, 4,
		arbT("IDS_ASSCOL_VIEW_RUNS_LIST"),
		arbT("IDS_ASSCOL_VIEW_RUNS_LIST_DESC")},

	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR_LIST, 5,
		arbT("IDS_ASSCOL_VIEW_CALENDAR_LIST"),
		arbT("IDS_ASSCOL_VIEW_CALENDAR_LIST_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_TRAINING_LIST, 7,
		arbT("IDS_ASSCOL_VIEW_TRAINING_LIST"),
		arbT("IDS_ASSCOL_VIEW_TRAINING_LIST_DESC")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_TYPE_CALENDAR_APPT, 0,
		arbT("IDS_ASSCOL_CALENDAR_APPT"),
		arbT("IDS_ASSCOL_CALENDAR_APPT_DESC")},
	{CAgilityBookOptions::eCalExportTask,
		IO_TYPE_CALENDAR_TASK, 0,
		arbT("IDS_ASSCOL_CALENDAR_TASK"),
		arbT("IDS_ASSCOL_CALENDAR_TASK_DESC")},
	{CAgilityBookOptions::eView,
		IO_TYPE_VIEW_CALENDAR, 6,
		arbT("IDS_ASSCOL_VIEW_CALENDAR"),
		arbT("IDS_ASSCOL_VIEW_CALENDAR_DESC")},
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
	wchar_t const* name;
} const sc_FieldNames[IO_MAX] =
{
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REG_NAME,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_REG_NAME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CALL_NAME,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_CALLNAME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DATE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DATE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_VENUE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLUB,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LOCATION,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TRIAL_NOTES,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_TRIALNOTES")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DIVISION,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DIVISION")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_LEVEL,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_LEVEL")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_EVENT,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_EVENT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HEIGHT,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_HEIGHT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_JUDGE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_JUDGE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_HANDLER,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_HANDLER")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CONDITIONS,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_CONDITIONS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COURSE_FAULTS,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_COURSEFAULTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TIME,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_TIME")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YARDS,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_YARDS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_YPS,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_YPS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCT,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_SCT")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TOTAL_FAULTS,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_TOTAL_FAULTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_OPENING,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_REQ_OPENING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_CLOSING,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_REQ_CLOSING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPENING,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_OPENING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_CLOSING,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_CLOSING")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_REQ_POINTS,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_REQ_POINTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_POINTS,
		true, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_POINTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_PLACE,
		true, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_PLACE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_IN_CLASS,
		true, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_IN_CLASS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_DOGSQD,
		true, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_Q_D")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_Q,
		true, wxLIST_FORMAT_CENTRE, arbT("IDS_COL_Q")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SCORE,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_SCORE")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_TITLE_POINTS,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_TITLE_PTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_COMMENTS,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_COMMENTS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_FAULTS,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_FAULTS")},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_START_DATE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_END_DATE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_TENTATIVE,
		true, 0, arbT("IDS_COL_TENTATIVE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport,
		IO_CAL_ENTERED,
		true, 0, arbT("IDS_COL_ENTERED")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_LOCATION,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLUB,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_VENUE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_OPENS,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_OPENS")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_CLOSES,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_CLOSES")},
	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_NOTES,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_DATE,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DATE")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NAME,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NAME")},
	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_NOTES,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eView,
		IO_TREE_DOG_REGNAME,
		false, 0, arbT("IDS_COL_REG_NAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_CALLNAME,
		false, 0, arbT("IDS_COL_CALLNAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_BREED,
		false, 0, arbT("IDS_COL_BREED")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_DOB,
		false, 0, arbT("IDS_COL_DOB")},
	{CAgilityBookOptions::eView,
		IO_TREE_DOG_AGE,
		false, 0, arbT("IDS_COL_AGE")},

	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_START,
		false, 0, arbT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_END,
		false, 0, arbT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_CLUB,
		false, 0, arbT("IDS_COL_CLUB")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_VENUE,
		false, 0, arbT("IDS_COL_VENUE")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_LOCATION,
		false, 0, arbT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_NOTES,
		false, 0, arbT("IDS_COL_NOTES")},

	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DATE,
		false, 0, arbT("IDS_COL_DATE")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_Q,
		false, 0, arbT("IDS_COL_Q")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_EVENT,
		false, 0, arbT("IDS_COL_EVENT")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_DIVISION,
		false, 0, arbT("IDS_COL_DIVISION")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_LEVEL,
		false, 0, arbT("IDS_COL_LEVEL")},
	{CAgilityBookOptions::eView,
		IO_TREE_RUN_HEIGHT,
		false, 0, arbT("IDS_COL_HEIGHT")},

	{CAgilityBookOptions::eLogImport | CAgilityBookOptions::eLogExport | CAgilityBookOptions::eView,
		IO_LOG_SUBNAME,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_SUBNAME")},
	{CAgilityBookOptions::eView,
		IO_TREE_TRIAL_VERIFIED,
		false, 0, arbT("IDS_COL_VERIFIED")},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SPEED,
		false, wxLIST_FORMAT_LEFT, arbT("IDS_COL_SPEED")},

	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SUBJECT,
		false, 0, arbT("IDS_COL_SUBJECT")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_DATE,
		false, 0, arbT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_START_TIME,
		false, 0, arbT("IDS_COL_START_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_DATE,
		false, 0, arbT("IDS_COL_END_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_END_TIME,
		false, 0, arbT("IDS_COL_END_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ALLDAY,
		false, 0, arbT("IDS_COL_ALL_DAY_EVENT")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER,
		false, 0, arbT("IDS_COL_REMINDER")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_DATE,
		false, 0, arbT("IDS_COL_REMINDER_DATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REMINDER_TIME,
		false, 0, arbT("IDS_COL_REMINDER_TIME")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_ORGANIZER,
		false, 0, arbT("IDS_COL_ORGANIZER")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_REQ_ATTENDEES,
		false, 0, arbT("IDS_COL_REQ_ATTENDEES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_OPT_ATTENDEES,
		false, 0, arbT("IDS_COL_OPT_ATTENDEES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_RESOURCES,
		false, 0, arbT("IDS_COL_RESOURCES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_BILLING,
		false, 0, arbT("IDS_COL_BILLING")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_CATEGORIES,
		false, 0, arbT("IDS_COL_CATEGORIES")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_DESCRIPTION,
		false, 0, arbT("IDS_COL_DESCRIPTION")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_LOCATION,
		false, 0, arbT("IDS_COL_LOCATION")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_MILEAGE,
		false, 0, arbT("IDS_COL_MILEAGE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIORITY,
		false, 0, arbT("IDS_COL_PRIORITY")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_PRIVATE,
		false, 0, arbT("IDS_COL_PRIVATE")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SENSITIVITY,
		false, 0, arbT("IDS_COL_SENSITIVITY")},
	{CAgilityBookOptions::eCalExportAppt,
		IO_CAL_APPT_SHOW_TIME_AS,
		false, 0, arbT("IDS_COL_SHOW_TIME_AS")},

	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SUBJECT,
		false, 0, arbT("IDS_COL_SUBJECT")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_START_DATE,
		false, 0, arbT("IDS_COL_START_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_DUE_DATE,
		false, 0, arbT("IDS_COL_DUE_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER,
		false, 0, arbT("IDS_COL_REMINDER")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_DATE,
		false, 0, arbT("IDS_COL_REMINDER_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_REMINDER_TIME,
		false, 0, arbT("IDS_COL_REMINDER_TIME")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETED_DATE,
		false, 0, arbT("IDS_COL_COMPLETED_DATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPLETE,
		false, 0, arbT("IDS_COL_COMPLETE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_TOTAL_WORK,
		false, 0, arbT("IDS_COL_TOTAL_WORK")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ACTUAL_WORK,
		false, 0, arbT("IDS_COL_ACTUAL_WORK")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_BILLING,
		false, 0, arbT("IDS_COL_BILLING")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CATEGORIES,
		false, 0, arbT("IDS_COL_CATEGORIES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_COMPANIES,
		false, 0, arbT("IDS_COL_COMPANIES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_CONTACTS,
		false, 0, arbT("IDS_COL_CONTACTS")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_MILEAGE,
		false, 0, arbT("IDS_COL_MILEAGE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_NOTES,
		false, 0, arbT("IDS_COL_NOTES")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIORITY,
		false, 0, arbT("IDS_COL_PRIORITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_PRIVATE,
		false, 0, arbT("IDS_COL_PRIVATE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_ROLE,
		false, 0, arbT("IDS_COL_ROLE")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SCH_PRIORITY,
		false, 0, arbT("IDS_COL_SCH_PRIORITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_SENSITIVITY,
		false, 0, arbT("IDS_COL_SENSITIVITY")},
	{CAgilityBookOptions::eCalExportTask,
		IO_CAL_TASK_STATUS,
		false, 0, arbT("IDS_COL_STATUS")},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_MIN_YPS,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_MIN_YPS")},

	{CAgilityBookOptions::eCalImport | CAgilityBookOptions::eCalExport | CAgilityBookOptions::eView,
		IO_CAL_DRAWS,
		true, wxLIST_FORMAT_LEFT, arbT("IDS_COL_DRAWS")},

	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OBSTACLES,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_OBSTACLES")},
	{CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_OPS,
		false, wxLIST_FORMAT_RIGHT, arbT("IDS_COL_OPS")},
	{CAgilityBookOptions::eRunsImport | CAgilityBookOptions::eRunsExport | CAgilityBookOptions::eView,
		IO_RUNS_SUBNAME,
		false, 0, arbT("IDS_COL_SUBNAME")},
};


int CDlgAssignColumns::GetFormatFromColumnID(long column)
{
	int fmt = 0;
	if (0 <= column && column < IO_MAX)
		fmt = sc_FieldNames[column].fmt;
	return fmt;
}


std::wstring CDlgAssignColumns::GetNameFromColumnID(long column)
{
	std::wstring name;
	if (0 <= column && column < IO_MAX)
		name = wxGetTranslation(sc_FieldNames[column].name);
	return name;
}


// Only called during initialization/reset (also from users of custom columns)
bool CDlgAssignColumns::GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::vector<long>& values,
		bool bDefaultValues)

{
	return GetColumnOrder(eOrder, idxColumn, wxEmptyString, values, bDefaultValues);
}


bool CDlgAssignColumns::GetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long>& values,
		bool bDefaultValues)
{
	bool bOk = false;
	if (idxColumn < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].valid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::GetColumnOrder(eOrder, idxColumn, namedColumn, values, bDefaultValues);
		}
	}
	return bOk;
}


bool CDlgAssignColumns::SetColumnOrder(
		CAgilityBookOptions::ColumnOrder eOrder,
		size_t idxColumn,
		std::wstring const& namedColumn,
		std::vector<long> const& values)
{
	bool bOk = false;
	if (idxColumn < IO_TYPE_MAX)
	{
		if (sc_Types[idxColumn].valid & eOrder)
		{
			bOk = true;
			CAgilityBookOptions::SetColumnOrder(eOrder, idxColumn, namedColumn, values);
		}
	}
	return bOk;
}


static int const idxRunsFaultsTime[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_SUBNAME,
	IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,			IO_RUNS_OBSTACLES,		IO_RUNS_OPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,			IO_RUNS_SPEED,
-1};
static int const idxRunsTimeFaults[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_SUBNAME,
	IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,			IO_RUNS_OBSTACLES,		IO_RUNS_OPS,
	IO_RUNS_SCT,			IO_RUNS_TOTAL_FAULTS,	IO_RUNS_PLACE,
	IO_RUNS_IN_CLASS,		IO_RUNS_DOGSQD,			IO_RUNS_Q,
	IO_RUNS_SCORE,			IO_RUNS_TITLE_POINTS,	IO_RUNS_COMMENTS,
	IO_RUNS_FAULTS,
-1};
static int const idxRunsOpenClose[] = {
	IO_RUNS_REG_NAME,		IO_RUNS_CALL_NAME,		IO_RUNS_DATE,
	IO_RUNS_VENUE,			IO_RUNS_CLUB,			IO_RUNS_LOCATION,
	IO_RUNS_TRIAL_NOTES,	IO_RUNS_DIVISION,		IO_RUNS_LEVEL,
	IO_RUNS_EVENT,			IO_RUNS_SUBNAME,
	IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
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
	IO_RUNS_EVENT,			IO_RUNS_SUBNAME,
	IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
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
	IO_RUNS_EVENT,			IO_RUNS_SUBNAME,
	IO_RUNS_HEIGHT,			IO_RUNS_JUDGE,
	IO_RUNS_HANDLER,		IO_RUNS_CONDITIONS,		IO_RUNS_COURSE_FAULTS,
	IO_RUNS_TIME,			IO_RUNS_YARDS,			IO_RUNS_MIN_YPS,
	IO_RUNS_YPS,			IO_RUNS_OBSTACLES,		IO_RUNS_OPS,
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

int wxCALLBACK CompareTypes(wxIntPtr lParam1, wxIntPtr lParam2, wxIntPtr lParam3)
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
		int initSelection)
	: wxDialog()
	, m_pDoc(pDoc)
	, m_Configs(eOrder)
	, m_initSelection(initSelection)
	, m_bIncludeBlank(false)
	, m_ConfigName()
	, m_ctrlConfig(nullptr)
	, m_ctrlType(nullptr)
	, m_ctrlAvailable(nullptr)
	, m_ctrlColumns(nullptr)
	, m_btnAdd(nullptr)
	, m_btnRemove(nullptr)
	, m_btnUp(nullptr)
	, m_btnDown(nullptr)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_ASSIGN_COLUMNS"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER);

	switch (m_Configs.Order())
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

	wxStaticText* textNames = new wxStaticText(this, wxID_ANY,
			_("IDC_ASSIGN_NAMES"),
			wxDefaultPosition, wxDefaultSize, 0);
	textNames->Wrap(-1);

	m_ctrlConfig = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, nullptr, wxCB_DROPDOWN|wxCB_SORT,
		CTrimValidator(&m_ConfigName, TRIMVALIDATOR_TRIM_BOTH));
	BIND_OR_CONNECT_CTRL(m_ctrlConfig, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgAssignColumns::OnSelchangeNames);
	m_ctrlConfig->SetHelpText(_("HIDC_ASSIGN_NAMES"));
	m_ctrlConfig->SetToolTip(_("HIDC_ASSIGN_NAMES"));
	std::vector<std::wstring> configNames;
	m_Configs.GetAllConfigNames(configNames);
	wxArrayString choices;
	for (std::vector<std::wstring>::iterator iterName = configNames.begin();
		iterName != configNames.end();
		++iterName)
	{
		wxString wxName(StringUtil::stringWX((*iterName)));
		int idx = m_ctrlConfig->Append(wxName);
		choices.Add(wxName);
		if ((*iterName) == m_Configs.GetCurrentConfig())
		{
			m_ConfigName = StringUtil::stringWX(m_Configs.GetCurrentConfig());
			m_ctrlConfig->SetSelection(idx);
		}
	}
	m_ctrlConfig->AutoComplete(choices);

	wxButton* btnSave = new wxButton(this, wxID_ANY,
		_("IDC_ASSIGN_NAMES_SAVE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnSave, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnClickedOptNamesSave);
	btnSave->SetHelpText(_("HIDC_ASSIGN_NAMES_SAVE"));
	btnSave->SetToolTip(_("HIDC_ASSIGN_NAMES_SAVE"));

	wxButton* btnDelete = new wxButton(this, wxID_ANY,
		_("IDC_ASSIGN_NAMES_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnDelete, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnClickedOptNamesDelete);
	btnDelete->SetHelpText(_("HIDC_ASSIGN_NAMES_DELETE"));
	btnDelete->SetToolTip(_("HIDC_ASSIGN_NAMES_DELETE"));

	m_ctrlType = new CListCtrl(this, wxID_ANY,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(200, 75)),
		wxLC_REPORT|wxLC_SINGLE_SEL|wxBORDER);
	BIND_OR_CONNECT_CTRL(m_ctrlType, wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler, CDlgAssignColumns::OnItemchanged);
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
		if (!(sc_Types[index].valid & m_Configs.Order()))
			continue;
		int idx = m_ctrlType->InsertItem(static_cast<long>(index), wxGetTranslation(sc_Types[index].name));
		if (0 <= idx)
		{
			m_ctrlType->SetItemData(idx, static_cast<long>(index));
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
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(70, 90)),
		0, nullptr, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlAvailable, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler, CDlgAssignColumns::OnSelchangeAvailable);
	m_ctrlAvailable->SetHelpText(_("HIDC_ASSIGN_AVAILABLE"));
	m_ctrlAvailable->SetToolTip(_("HIDC_ASSIGN_AVAILABLE"));

	wxStaticText* textSpacer = new wxStaticText(this, wxID_ANY, L"",
		wxDefaultPosition, wxDefaultSize, 0);
	textSpacer->Wrap(-1);

	m_btnAdd = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_ADD"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_btnAdd, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnAdd);
	m_btnAdd->SetHelpText(_("HIDC_ASSIGN_ADD"));
	m_btnAdd->SetToolTip(_("HIDC_ASSIGN_ADD"));

	m_btnRemove = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_btnRemove, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnRemove);
	m_btnRemove->SetHelpText(_("HIDC_ASSIGN_DELETE"));
	m_btnRemove->SetToolTip(_("HIDC_ASSIGN_DELETE"));

	m_btnUp = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_btnUp, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnMoveUp);
	m_btnUp->SetHelpText(_("HIDC_ASSIGN_MOVE_UP"));
	m_btnUp->SetToolTip(_("HIDC_ASSIGN_MOVE_UP"));

	m_btnDown = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_btnDown, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnMoveDown);
	m_btnDown->SetHelpText(_("HIDC_ASSIGN_MOVE_DOWN"));
	m_btnDown->SetToolTip(_("HIDC_ASSIGN_MOVE_DOWN"));

	wxButton* btnReset = new wxButton(this, wxID_ANY, _("IDC_ASSIGN_RESET"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnReset, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CDlgAssignColumns::OnReset);
	btnReset->SetHelpText(_("HIDC_ASSIGN_RESET"));
	btnReset->SetToolTip(_("HIDC_ASSIGN_RESET"));

	wxStaticText* textOrder = new wxStaticText(this, wxID_ANY,
		_("IDC_ASSIGN_COLUMNS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textOrder->Wrap(-1);

	m_ctrlColumns = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDLG_UNIT(this, wxSize(70, 90)),
		0, nullptr, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlColumns, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler, CDlgAssignColumns::OnSelchangeColumns);
	m_ctrlColumns->SetHelpText(_("HIDC_ASSIGN_COLUMNS"));
	m_ctrlColumns->SetToolTip(_("HIDC_ASSIGN_COLUMNS"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(textNames, 0, wxLEFT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerSettings = new wxBoxSizer(wxHORIZONTAL);
	sizerSettings->Add(m_ctrlConfig, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerSettings->Add(btnSave, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerSettings->Add(btnDelete, 0, wxALIGN_CENTER_VERTICAL, 0);

	bSizer->Add(sizerSettings, 0, wxEXPAND | wxLEFT | wxRIGHT, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlType, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFields = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerAvail = new wxBoxSizer(wxVERTICAL);
	sizerAvail->Add(textAvail, 0, 0, 0);
	sizerAvail->Add(m_ctrlAvailable, 1, wxEXPAND, 0);

	sizerFields->Add(sizerAvail, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(textSpacer, 0, 0, 0);
	sizerBtns->Add(m_btnAdd, 0, 0, 0);
	sizerBtns->Add(m_btnRemove, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_btnUp, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(m_btnDown, 0, wxTOP, wxDLG_UNIT_X(this, 2));
	sizerBtns->Add(0, 0, 1, wxEXPAND, 0);
	sizerBtns->Add(btnReset, 0, wxTOP, wxDLG_UNIT_X(this, 2));

	sizerFields->Add(sizerBtns, 0, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerOrder = new wxBoxSizer(wxVERTICAL);
	sizerOrder->Add(textOrder, 0, 0, 0);
	sizerOrder->Add(m_ctrlColumns, 1, wxEXPAND, 0);

	sizerFields->Add(sizerOrder, 1, wxEXPAND, 0);

	bSizer->Add(sizerFields, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	m_ctrlType->Select(idxSelect); // Causes change event that fills columns
	if (m_Configs.Order() == CAgilityBookOptions::eView)
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
		for (i = 0; i < m_Configs.Columns(idxType).size(); ++i)
		{
			if (0 <= m_Configs.Columns(idxType)[i])
			{
				bInUse[m_Configs.Columns(idxType)[i]] = true;
				std::wstring name = GetNameFromColumnID(m_Configs.Columns(idxType)[i]);
				int idx = m_ctrlColumns->Append(StringUtil::stringWX(name));
				if (0 <= idx)
					m_ctrlColumns->SetClientObject(idx, new ColumnData(i, m_Configs.Columns(idxType)[i]));
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
		bool bImport = (CAgilityBookOptions::eRunsImport == m_Configs.Order()
			|| CAgilityBookOptions::eCalImport == m_Configs.Order()
			|| CAgilityBookOptions::eLogImport == m_Configs.Order());
		for (i = 0; 0 <= sc_Fields[idxType][i]; ++i)
		{
			if (
			!(sc_FieldNames[sc_Fields[idxType][i]].valid & m_Configs.Order())
			|| (bImport && !sc_FieldNames[sc_Fields[idxType][i]].bImportable)
			|| bInUse[sc_Fields[idxType][i]])
				continue;
			std::wstring name = GetNameFromColumnID(sc_Fields[idxType][i]);
			int idx = m_ctrlAvailable->Append(StringUtil::stringWX(name));
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
		std::vector<long> columns;
		columns.reserve(m_ctrlColumns->GetCount());
		for (unsigned int idx = 0; idx < m_ctrlColumns->GetCount(); ++idx)
		{
			columns.push_back(GetInUseData(idx)->m_Data);
		}
		m_Configs.SetColumn(idxType, columns);
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


void CDlgAssignColumns::OnSelchangeNames(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idx = m_ctrlConfig->GetSelection();
	if (0 <= idx)
	{
		m_Configs.SetCurrentConfig(StringUtil::stringW(m_ctrlConfig->GetString(idx)));
		FillColumns();
	}
	else
		wxBell();
}


void CDlgAssignColumns::OnClickedOptNamesSave(wxCommandEvent& evt)
{
	if (!TransferDataFromWindow())
		return;
	if (!m_ConfigName.empty())
	{
		if (m_Configs.AddConfig(StringUtil::stringW(m_ConfigName)))
			m_ctrlConfig->Append(m_ConfigName);
		FillColumns();
	}
	else
		wxMessageBox(_("IDS_NAME_FILTER"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
}


void CDlgAssignColumns::OnClickedOptNamesDelete(wxCommandEvent& evt)
{
	if (!TransferDataFromWindow())
		return;
	if (!m_ConfigName.empty())
	{
		int idx = m_ctrlConfig->FindString(m_ConfigName, true);
		if (0 <= idx)
		{
			m_Configs.DeleteConfig(StringUtil::stringW(m_ConfigName));
			m_ctrlConfig->Delete(idx);
			m_ConfigName.clear();
			m_ctrlConfig->SetSelection(wxNOT_FOUND);
		}
	}
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
		int idxCol = -1;
		int idxCur = m_ctrlColumns->GetSelection();
		if (0 <= idxCur)
		{
			idxCol = m_ctrlColumns->Insert(str, idxCur + 1);
		}
		else
			idxCol = m_ctrlColumns->Append(str);
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
	int idxCol = m_ctrlColumns->GetSelection();
	if (0 <= idxCol)
	{
		wxString str = m_ctrlColumns->GetString(idxCol);
		ColumnData* pData = GetInUseData(idxCol);
		if (pData->m_Data >= 0)
		{
			int idxAvail = 0;
			if (m_bIncludeBlank)
				++idxAvail; // Skip special "blank column"
			bool bDone = false;
			for (; idxAvail < static_cast<int>(m_ctrlAvailable->GetCount()); ++idxAvail)
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
				if (idxCol == static_cast<int>(m_ctrlColumns->GetCount()))
					--idxCol;
				m_ctrlColumns->SetSelection(idxCol);
			}
		}
		else
		{
			m_ctrlColumns->Delete(idxCol);
			if (idxCol == static_cast<int>(m_ctrlColumns->GetCount()))
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
	int idxCol = m_ctrlColumns->GetSelection();
	if (0 <= idxCol && 1 < m_ctrlColumns->GetCount()
		&& static_cast<int>(m_ctrlColumns->GetCount()) - 1 != idxCol)
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
		m_Configs.ResetDefault(idxType);
		FillColumns();
	}
}


void CDlgAssignColumns::OnOk(wxCommandEvent& evt)
{
	m_Configs.Save();
	if (m_pDoc && CAgilityBookOptions::eView == m_Configs.Order())
	{
		CUpdateHint hint(UPDATE_CUSTOMIZE);
		m_pDoc->UpdateAllViews(nullptr, &hint);
	}
	EndDialog(wxID_OK);
}
