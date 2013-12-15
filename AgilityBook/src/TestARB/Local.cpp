/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Implementation of IARBLocalization
 * @author David Connet
 *
 * In wxWidgets, this may look kind of strange. It originated in MFC where
 * we called LoadString to load strings from the resource. This separated
 * resource management from the core ARB code. With the 'gettext' style,
 * we could remove this class and use _("id") directly in the arb core, but
 * for now, this stays. New code will probably change to that. Once the port
 * is complete and the MFC code is no longer maintained, I will work to remove
 * this.
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-19 Ported to wxWidgets.
 * 2008-01-13 Created
 */

#include "stdafx.h"
#include "Local.h"

#include "ARB/ARBStructure.h"
#include "ARBCommon/StringUtil.h"

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CLocalization::CLocalization()
{
}


bool CLocalization::Load()
{
	m_UnknownVersion = StringUtil::GetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION"));
	if (m_UnknownVersion == L"IDS_ARB_UNKNOWN_VERSION")
		return false;

	m_WarningNewerDoc = StringUtil::GetTranslation(arbT("IDS_WARNING_NEWER_DOC"));
	m_InvalidDocStructure = StringUtil::GetTranslation(arbT("IDS_INVALID_DOC_STRUCTURE"));
	m_InvalidFileFormat = StringUtil::GetTranslation(arbT("IDS_INVALID_FILE_FORMAT"));
	m_InvalidFileMissingAttrib = StringUtil::GetTranslation(arbT("IDS_INVALID_FILE_MISSING_ATTRIB"));
	m_InvalidFileBadAttrib = StringUtil::GetTranslation(arbT("IDS_INVALID_FILE_BAD_ATTRIB"));
	m_InvalidDate = StringUtil::GetTranslation(arbT("IDS_INVALID_DATE"));
	m_InvalidDivLevel = StringUtil::GetTranslation(arbT("IDS_INVALID_DIV_LEVEL"));
	m_InvalidEventName = StringUtil::GetTranslation(arbT("IDS_INVALID_EVENT_NAME"));

#if defined(__WXWINDOWS__)
	m_InvalidRoot = StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_INVALID_ROOT")).c_str(), TREE_BOOK));
	m_MissingConfig = StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_MISSING_CONFIG")).c_str(), TREE_CONFIG));
	m_InvalidConfig = StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_INVALID_CONFIG")).c_str(), TREE_CONFIG));
	m_InvalidVenueConfig = StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_INVALID_VENUE_CONFIG")).c_str(), TREE_DIVISION, TREE_EVENT));
#else
#pragma PRAGMA_TODO(Write locale stuff)
	m_InvalidRoot = arbT("IDS_INVALID_ROOT");
	m_MissingConfig = arbT("IDS_MISSING_CONFIG");
	m_InvalidConfig = arbT("IDS_INVALID_CONFIG");
	m_InvalidVenueConfig = arbT("IDS_INVALID_VENUE_CONFIG");
#endif
	m_InvalidDivName = StringUtil::GetTranslation(arbT("IDS_INVALID_DIV_NAME"));
	m_InvalidVenueName = StringUtil::GetTranslation(arbT("IDS_INVALID_VENUE_NAME"));
	m_InvalidOtherPtsName = StringUtil::GetTranslation(arbT("IDS_INVALID_OTHER_PTS_NAME"));
	m_InvalidMultiqName = StringUtil::GetTranslation(arbT("IDS_INVALID_MULTIQ_NAME"));
	m_InvalidMultiqConversion = StringUtil::GetTranslation(arbT("IDS_INVALID_MULTIQ_CONVERSION"));
	m_InvalidEvent = StringUtil::GetTranslation(arbT("IDS_INVALID_EVENT"));
	m_InvalidTitle = StringUtil::GetTranslation(arbT("IDS_INVALID_TITLE"));
	m_ValidValues = StringUtil::GetTranslation(arbT("IDS_VALID_VALUES"));

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + L"'y', 'n'";

	m_CalendarTentative = StringUtil::GetTranslation(arbT("IDS_CALENDAR_TENTATIVE"));
	m_CalendarPast = StringUtil::GetTranslation(arbT("IDS_CALENDAR_PAST"));
	m_CalendarNotEntered = StringUtil::GetTranslation(arbT("IDS_CALENDAR_NOTENTERED"));
	m_CalendarEntered = StringUtil::GetTranslation(arbT("IDS_CALENDAR_ENTERED"));
	m_CalendarPending = StringUtil::GetTranslation(arbT("IDS_CALENDAR_PENDING"));
	m_CalendarPlanning = StringUtil::GetTranslation(arbT("IDS_CALENDAR_PLANNING"));
	m_CalendarStatusN = m_CalendarStatusE = m_CalendarStatusO = m_CalendarStatusP = StringUtil::GetTranslation(arbT("IDS_CALENDAR_STATUS"));
	m_CalendarStatusN += m_CalendarNotEntered;
	m_CalendarStatusE += m_CalendarEntered;
	m_CalendarStatusO += m_CalendarPending;
	m_CalendarStatusP += m_CalendarPlanning;
	m_CalendarOpens = StringUtil::GetTranslation(arbT("IDS_CALENDAR_OPENS"));
	m_CalendarDraw = StringUtil::GetTranslation(arbT("IDS_CALENDAR_DRAW"));
	m_CalendarCloses = StringUtil::GetTranslation(arbT("IDS_CALENDAR_CLOSES"));
	m_ScoreStyleUnknown = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_UNKNOWN"));
	m_ScoreStyleFaultsTime = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_FAULTS_TIME"));
	m_ScoreStyleFaults100Time = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_FAULTS100_TIME"));
	m_ScoreStyleFaults200Time = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_FAULTS200_TIME"));
	m_ScoreStyleOCScoreTime = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_OCSCORE_TIME"));
	m_ScoreStyleScoreTime = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_SCORE_TIME"));
	m_ScoreStyleTimePlusFaults = StringUtil::GetTranslation(arbT("IDS_SCORE_STYLE_TIME_PLUS_FAULTS"));
	m_ExistingPointsOther = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_OTHER"));
	m_ExistingPointsRun = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_RUN"));
	m_ExistingPointsSpeed = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_SPEED"));
	m_ExistingPointsMQ = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_MQ"));
	m_ExistingPointsSQ = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_SQ"));

	return true;
}


std::wstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_UPDATE_CALSITES"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nNew)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_FAULTS"))
		<< L": "
		<<wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nNew)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_OTHERPOINTS"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nNew)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_VENUES"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nNew)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_DIVISIONS"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateDivisionsReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_DIVISIONS"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_EVENTS"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateEventsReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_EVENTS"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_MULTIQS"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_DELETED")).c_str(), nDeleted)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateMultiqsReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_MULTIQS"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_LEVELS"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateLevelsReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_COL_LEVELS"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_TITLES"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateTitlesReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_TITLES"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const
#if defined(__WXWINDOWS__)
{
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_SUBLEVEL_NAME"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateSubLevelsReordered() const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << StringUtil::GetTranslation(arbT("IDS_SUBLEVEL_NAME"))
		<< L": "
		<< StringUtil::GetTranslation(arbT("IDS_REORDERED"));
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << L" "
		<< StringUtil::GetTranslation(arbT("IDS_UPDATE_RULES"))
		<< L": "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")).c_str(), nAdded)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_DELETED")).c_str(), nDeleted)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")).c_str(), nUpdated)
		<< L", "
		<< wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")).c_str(), nSkipped);
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteCalPlugin(std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_CALPLUGIN")).c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameOtherPoints(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS")).c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteOtherPoints(
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_OTHERPTS")).c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteOtherPoints(std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_OTHERPTS")).c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameVenue(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_VENUE")).c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_VENUE2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteVenue(
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_VENUE")).c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteVenue(std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_VENUE")).c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameMultiQ(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ")).c_str(), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_MULTIQ")).c_str(), venue.c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_MULTIQ")).c_str(), venue.c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameDivision(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_DIV")).c_str(), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_DIV2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_DIV")).c_str(), venue.c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_DIV")).c_str(), venue.c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameLevel(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LEVEL")).c_str(), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LEVEL2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_LEVEL")).c_str(), venue.c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_LEVEL")).c_str(), venue.c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameTitle(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_TITLE")).c_str(), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_TITLE2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_TITLE")).c_str(), venue.c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_TITLE")).c_str(), venue.c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionRenameEvent(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_EVENT")).c_str(), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_EVENT2")).c_str(), nChanges);
	}
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionPreDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_EVENT")).c_str(), venue.c_str(), name.c_str(), nChanges));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ActionDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_EVENT")).c_str(), venue.c_str(), name.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
#if defined(__WXWINDOWS__)
	wxString dbl1 = wxString::Format(L"%g", points);
	wxString dbl2 = wxString::Format(L"%g", faults);
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_TITLEPOINTSNAME")).c_str(), dbl1.c_str(), dbl2.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
#if defined(__WXWINDOWS__)
	wxString dbl1 = wxString::Format(L"%g", points);
	wxString dbl2 = wxString::Format(L"%g", faults);
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_LIFETITLEPOINTSNAME")).c_str(), dbl1.c_str(), dbl2.c_str()));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
#if defined(__WXWINDOWS__)
	wxString dbl1 = wxString::Format(L"%g", points);
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_PLACEMENTPOINTSNAME")).c_str(), dbl1.c_str(), (int)place));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_WARN_DELETED_RUNS")).c_str(), nRuns)
		<< L"\n" << inRunsMsg.c_str();
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const
{
#if defined(__WXWINDOWS__)
	wxString buffer;
	buffer << wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_TEAM_RUNS")).c_str(), nRuns)
		<< L"\n" << inRunsMsg.c_str();
	return StringUtil::stringW(buffer);
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::UpdateTableRuns(int nRuns) const
{
#if defined(__WXWINDOWS__)
	return StringUtil::stringW(wxString::Format(StringUtil::GetTranslation(arbT("IDS_UPDATE_TABLE_RUNS")).c_str(), nRuns));
#else
#pragma PRAGMA_TODO(Write localization)
	return std::wstring();
#endif
}


std::wstring CLocalization::ErrorInvalidDocStructure(wchar_t const* const inMsg) const
{
	std::wostringstream buffer;
	buffer << m_InvalidDocStructure;
	if (inMsg)
		buffer << L": " << inMsg << L"\n";
	return buffer.str();
}


std::wstring CLocalization::ErrorMissingAttribute(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg) const
{
	std::wostringstream buffer;
	buffer << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileMissingAttrib
		<< inAttrib
		<< L"'.";
	if (inMsg)
		buffer << L" " << inMsg << L"\n";
	return buffer.str();
}


std::wstring CLocalization::ErrorInvalidAttributeValue(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg) const
{
	std::wostringstream buffer;
	buffer << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileBadAttrib
		<< inAttrib
		<< L"'.";
	if (inMsg)
		buffer << L" " << inMsg << L"\n";
	return buffer.str();
}
