#pragma once

/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief Localization
 * @author David Connet
 *
 * Revision History
 * @li 2007-09-12 DRC Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

/*
 * Localization stuff
 *  There is some stuff in:
 *    ARBConfigAction.cpp (action items)
 *    Element.cpp (errors due to XML parsing failures)
 *  Windows notes:
 *    DlgOptionsCalendar.cpp (text items for planning/etc)
 */

#include "ARBTypes.h"

// Used when loading data.
#define UNKNOWN_VERSION			_T("Unknown document version")
#define WARNING_NEWER_DOC		_T("Warning: The file you are loading was created by a newer version of this program. Saving this file with this version of the program will result in a loss of data.\n\nAre you sure you want to continue?")
#define INVALID_DOC_STRUCTURE	_T("Invalid document structure")
#define INVALID_FILE_FORMAT		_T("Invalid file format: '")
#define INVALID_FILE_MISSING_ATTRIB	_T("' is missing required attribute '")
#define INVALID_FILE_BAD_ATTRIB	_T("' has an invalid value for the attribute '")
#define INVALID_DATE			_T("Invalid date: ")
#define INVALID_VALUE			_T("Invalid value: ")
#define INVALID_DIV_LEVEL		_T("Division/level pair has not been defined: ")
#define INVALID_EVENT_NAME		_T("Event has not been defined: ")
#define INVALID_ROOT			_T("'") TREE_BOOK _T("' must be the root element.")
#define MISSING_CONFIG			_T("Missing '") TREE_CONFIG _T("' section.")
#define INVALID_CONFIG			_T("Only one '") TREE_CONFIG _T("' section is allowed.")
#define INVALID_VENUE_CONFIG	_T("All '") TREE_DIVISION _T("' elements must come before '") TREE_EVENT _T("' elements.")
#define INVALID_DIV_NAME		_T("Invalid division name: ")
#define INVALID_VENUE_NAME		_T("Invalid venue name: ")
#define INVALID_OTHER_PTS_NAME	_T("Invalid Other Points name: ")
#define INVALID_MULTIQ_NAME		_T("Invalid Multiple Q name: ")
#define INVALID_MULTIQ_CONVERSION	_T("Unable to convert Existing QQ Points")
#define INVALID_EVENT			_T("Unable to locate an event description in any of the venues of the affiliated clubs: ")
#define INVALID_TITLE			_T("Title is not defined in the Venue description: ")
#define VALID_VALUES			_T("Valid values: ")
#define VALID_VALUES_BOOL		_T("Valid values: 'y', 'n'")
#define VALID_VALUES_ENTRY		_T("Valid values: 'E', 'P', 'N'")
#define VALID_VALUES_ACCOM		_T("Valid values: 'N', 'T', 'C'")
#define VALID_VALUES_OTHERPT	_T("Valid values: 'All', 'AllByEvent', 'Level', 'LevelByEvent'")
#define VALID_VALUES_SCORE		_T("Valid values: 'FaultsThenTime', 'Faults100ThenTime', 'Faults200ThenTime', 'OCScoreThenTime', 'ScoreThenTime', 'TimePlusFaults'")
#define CALENDAR_TENTATIVE		_T("Information is tentative.")
#define CALENDAR_NOTENTERED		_T("Not entered")
#define CALENDAR_ENTERED		_T("Entered")
#define CALENDAR_PLANNING		_T("Planning")
#define CALENDAR_STATUS_N		_T("Status: ") CALENDAR_NOTENTERED
#define CALENDAR_STATUS_E		_T("Status: ") CALENDAR_ENTERED
#define CALENDAR_STATUS_P		_T("Status: ") CALENDAR_PLANNING
#define CALENDAR_OPENS			_T("Trial opens: ")
#define CALENDAR_DRAW			_T("Trial draws: ")
#define CALENDAR_CLOSES			_T("Trial closes: ")

// Strings for formatting the information returned when updating configurations.
// - Changed to functions in order to remove sprintf style formatting.
extern tstring UPDATE_FORMAT_FAULTS(int nNew, int nSkipped);
extern tstring UPDATE_FORMAT_OTHERPTS(int nNew, int nUpdated, int nSkipped);
extern tstring UPDATE_FORMAT_VENUES(int nNew, int nUpdated, int nSkipped);
extern tstring UPDATE_FORMAT_DIVISIONS(int nAdded, int nChanged, int nSkipped);
extern tstring UPDATE_FORMAT_EVENTS(int nAdded, int nChanged, int nSkipped);
extern tstring UPDATE_FORMAT_MULTIQS(int nAdded, int nDeleted, int nSkipped);
extern tstring UPDATE_FORMAT_LEVELS(int nAdded, int nChanged, int nSkipped);
extern tstring UPDATE_FORMAT_TITLES(int nAdded, int nChanged, int nSkipped);
extern tstring UPDATE_FORMAT_SUBLEVELS(int nAdded);
extern tstring UPDATE_FORMAT_RULES(int nAdded, int nDeleted, int nChanged, int nSkipped);
extern tstring WARN_DELETED_RUNS(int nRuns, tstring const& inRunsMsg);
extern tstring UPDATE_TABLE_RUNS(int nRuns);

// Used in ARBConfigTitlePoints.cpp
extern tstring TITLE_POINTS_NAME_FORMAT(double points, double faults);
extern tstring LIFETIME_POINTS_NAME_FORMAT(double points, double faults);
extern tstring PLACEMENT_POINTS_NAME_FORMAT(double points, short place);

// Used in ARBConfigScoring.cpp
#define SCORE_STYLE_UNKNOWN			_T("Unknown")
#define SCORE_STYLE_FAULTSTIME		_T("Faults Then Time")
#define SCORE_STYLE_FAULTS100TIME	_T("100 Minus Faults Then Time")
#define SCORE_STYLE_FAULTS200TIME	_T("200 Minus Faults Then Time")
#define SCORE_STYLE_OCSCORETIME		_T("Opening/Closing Points Then Time")
#define SCORE_STYLE_SCORETIME		_T("Points Then Time")
#define SCORE_STYLE_TIMEPLUSFAULTS	_T("Time Plus Faults")

// Used in ARBTypes.cpp
#define ARBQ_TYPE_NA				_T("NA")
#define ARBQ_TYPE_Q					_T("Q")
#define ARBQ_TYPE_NQ				_T("NQ")
#define ARBQ_TYPE_E					_T("E")
#define ARBQ_TYPE_SQ				_T("SQ")

// Used in ARBDogExistingPoints.cpp
#define EXISTING_POINTS_OTHER		_T("OtherPoints")
#define EXISTING_POINTS_RUN			_T("Run")
#define EXISTING_POINTS_SPEED		_T("Speed")
#define EXISTING_POINTS_MQ			_T("Multiple Q")
#define EXISTING_POINTS_SQ			_T("SuperQ")

/////////////////////////////////////////////////////////////////////////////
// Global functions

/**
 * Return an error message about invalid document structure.
 *
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern tstring ErrorInvalidDocStructure(TCHAR const* const inMsg);

/**
 * Return an error message about a missing required attribute.
 *
 * @param inElement Element containing missing attribute.
 * @param inAttrib Attribute name that is missing.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern tstring ErrorMissingAttribute(
	TCHAR const* const inElement,
	TCHAR const* const inAttrib,
	TCHAR const* const inMsg = NULL);

/**
 * Return an error message about an invalid value in an attribute.
 *
 * @param inElement Element containing bad attribute.
 * @param inAttrib Attribute name whose value is bad.
 * @param inMsg Additional error information.
 * @return Message with newline.
 */
extern tstring ErrorInvalidAttributeValue(
	TCHAR const* const inElement,
	TCHAR const* const inAttrib,
	TCHAR const* const inMsg = NULL);
