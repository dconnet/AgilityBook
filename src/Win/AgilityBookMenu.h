#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Wrapper for menus to ease i18n and menu ids
 * @author David Connet
 *
 * Revision History
 * 2012-07-13 Reworked menu creation to use a generic reusable helper.
 * 2008-12-14 Created
 */

#include "LibARBWin/MenuHelper.h"


namespace dconSoft
{

enum MenuIdentityPopup
{
	IdMenuNone = 0,
	IdMenuDog,
	IdMenuTrial,
	IdMenuRun,
	IdMenuRunDifferent, // in run view, multiple select of different items
	IdMenuPoints,
	IdMenuCalendar,
	IdMenuTraining,
};


enum
{
	ID_ARB_FIRST = wxID_HIGHEST + 1,
	// wxID_NEW (these are here just to show the full menu structure)
	// wxID_OPEN
	// wxID_SAVE
	// wxID_SAVEAS
	ID_FILE_LANGUAGE_CHOOSE,
	ID_FILE_EXPORT_WIZARD,
	ID_FILE_LINKED,
	// wxID_PRINT
	ID_FILE_PRINT_BLANK_RUNS,
	// wxID_PREVIEW
	ID_FILE_PROPERTIES,
	// wxID_EXIT

	// wxID_CUT
	// wxID_COPY
	// wxID_PASTE
	// wxID_DUPLICATE
	// wxID_DELETE
	// wxID_SELECTALL
	ID_COPY_TITLES_LIST,
	ID_REORDER,
	// wxID_FIND
	ID_EDIT_FIND_NEXT,
	ID_EDIT_FIND_PREVIOUS,
	ID_EDIT_CONFIGURATION,

	ID_AGILITY_EDIT_DOG,
	ID_AGILITY_NEW_DOG,
	ID_AGILITY_DELETE_DOG,
	ID_AGILITY_EDIT_TITLE,
	ID_AGILITY_NEW_TITLE,
	ID_AGILITY_DELETE_TITLE,
	ID_AGILITY_EDIT_TRIAL,
	ID_AGILITY_NEW_TRIAL,
	ID_AGILITY_DELETE_TRIAL,
	ID_AGILITY_PRINT_TRIAL,
	ID_AGILITY_EDIT_RUN,
	ID_AGILITY_NEW_RUN,
	ID_AGILITY_DELETE_RUN,
	ID_AGILITY_PRINT_RUNS,
	ID_AGILITY_EDIT_CALENDAR,
	ID_AGILITY_NEW_CALENDAR,
	ID_AGILITY_DELETE_CALENDAR,
	ID_AGILITY_EXPORT_CALENDAR,
	ID_AGILITY_CREATEENTRY_CALENDAR,
	ID_AGILITY_EDIT_TRAINING,
	ID_AGILITY_NEW_TRAINING,
	ID_AGILITY_DELETE_TRAINING,

	ID_NOTES_CLUBS,
	ID_NOTES_JUDGES,
	ID_NOTES_LOCATIONS,
	ID_NOTES_SEARCH,

	// wxID_PREFERENCES,
	ID_VIEW_CUSTOMIZE,
	ID_VIEW_CUSTOMIZE_ACCEL,
	ID_UNSORT,
	ID_VIEW_SORTRUNS,
	ID_VIEW_RUNS_BY_TRIAL,
	ID_VIEW_TABLE_IN_YPS,
	ID_VIEW_RUNTIME_IN_OPS,
	ID_VIEW_POINTS_VIEW_SORT,
	ID_VIEW_HIDDEN,
	ID_VIEW_LIFETIME_EVENTS,
	ID_EXPAND,
	ID_COLLAPSE,
	ID_EXPAND_ALL,
	ID_COLLAPSE_ALL,
	ID_BOOK_FIRST,
	ID_BOOK_NOTEBOOK = ID_BOOK_FIRST,
	ID_BOOK_LISTBOOK,
	ID_BOOK_CHOICEBOOK,
	ID_BOOK_TREEBOOK,
	ID_BOOK_TOOLBOOK,
	ID_BOOK_LAST,
	ID_ORIENT_FIRST,
	ID_ORIENT_TOP = ID_ORIENT_FIRST,
	ID_ORIENT_BOTTOM,
	ID_ORIENT_LEFT,
	ID_ORIENT_RIGHT,
	ID_ORIENT_LAST,

	ID_WINDOW_SPLIT,
	ID_NEXT_PANE,
	ID_PREV_PANE,

	ID_HELP_UPDATE,
	ID_HELP_LOG,
	ID_HELP_LOG_VERBOSE,
	// wxID_ABOUT

	ID_DETAILS,

	ID_ARB_LAST,
	ID_HELP_DEBUG, // Outside LAST range so it doesn't get disabled
};

} // namespace dconSoft
