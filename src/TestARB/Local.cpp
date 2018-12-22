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
 * 2018-12-16 Convert to fmt.
 * 2016-06-19 Add support for Lifetime names.
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2016-01-06 Add support for named lifetime points.
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

	m_InvalidRoot = fmt::format(StringUtil::GetTranslation(arbT("IDS_INVALID_ROOT")), TREE_BOOK);
	m_MissingConfig = fmt::format(StringUtil::GetTranslation(arbT("IDS_MISSING_CONFIG")), TREE_CONFIG);
	m_InvalidConfig = fmt::format(StringUtil::GetTranslation(arbT("IDS_INVALID_CONFIG")), TREE_CONFIG);
	m_InvalidVenueConfig = fmt::format(StringUtil::GetTranslation(arbT("IDS_INVALID_VENUE_CONFIG")), TREE_DIVISION, TREE_EVENT);
	m_InvalidDivName = StringUtil::GetTranslation(arbT("IDS_INVALID_DIV_NAME"));
	m_InvalidVenueName = StringUtil::GetTranslation(arbT("IDS_INVALID_VENUE_NAME"));
	m_InvalidLifetimeName = StringUtil::GetTranslation(arbT("IDS_INVALID_LIFETIME_NAME"));
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
	m_ExistingPointsLifetime = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_LIFETIME"));
	m_ExistingPointsRun = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_RUN"));
	m_ExistingPointsSpeed = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_SPEED"));
	m_ExistingPointsMQ = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_MQ"));
	m_ExistingPointsSQ = StringUtil::GetTranslation(arbT("IDS_EXISTING_POINTS_SQ"));

	return true;
}


std::wstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_UPDATE_CALSITES")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nNew),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_FAULTS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nNew),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_OTHERPOINTS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nNew),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_VENUES")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nNew),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateLifetimeNames(int nNew, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}",
		StringUtil::GetTranslation(arbT("IDS_LIFETIMENAMES")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nNew),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_DIVISIONS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateDivisionsReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_COL_DIVISIONS")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_EVENTS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateEventsReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_COL_EVENTS")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_MULTIQS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_DELETED")), nDeleted),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateMultiqsReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_COL_MULTIQS")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_COL_LEVELS")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateLevelsReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_COL_LEVELS")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_TITLES")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateTitlesReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_TITLES")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(L"{}: {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_SUBLEVEL_NAME")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::UpdateSubLevelsReordered() const
{
	return fmt::format(L"{}: {}",
		StringUtil::GetTranslation(arbT("IDS_SUBLEVEL_NAME")),
		StringUtil::GetTranslation(arbT("IDS_REORDERED")));
}


std::wstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	return fmt::format(L" {}: {}, {}, {}, {}",
		StringUtil::GetTranslation(arbT("IDS_UPDATE_RULES")),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_DELETED")), nDeleted),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated),
		fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped));
}


std::wstring CLocalization::ActionDeleteCalPlugin(std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_CALPLUGIN")), name);
}


std::wstring CLocalization::ActionRenameOtherPoints(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS")), oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteOtherPoints(
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_OTHERPTS")), name, nChanges);
}


std::wstring CLocalization::ActionDeleteOtherPoints(std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_OTHERPTS")), name);
}


std::wstring CLocalization::ActionRenameVenue(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_VENUE")), oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_VENUE2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteVenue(
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_VENUE")), name, nChanges);
}


std::wstring CLocalization::ActionDeleteVenue(std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_VENUE")), name);
}


std::wstring CLocalization::ActionRenameMultiQ(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ")), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_MULTIQ")), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_MULTIQ")), venue, name);
}


std::wstring CLocalization::ActionRenameDivision(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_DIV")), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_DIV2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_DIV")), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_DIV")), venue, name);
}


std::wstring CLocalization::ActionRenameLevel(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LEVEL")), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LEVEL2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_LEVEL")), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_LEVEL")), venue, name);
}


std::wstring CLocalization::ActionRenameTitle(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_TITLE")), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_TITLE2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_TITLE")), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_TITLE")), venue, name);
}


std::wstring CLocalization::ActionRenameEvent(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_EVENT")), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(buffer, StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_EVENT2")), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_PREDELETE_EVENT")), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_EVENT")), venue, name);
}


std::wstring CLocalization::ActionRenameLifetimeName(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nEventChanges) const
{
	if (0 == nEventChanges)
		return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LIFETIMENAME_0")), venue, oldName, newName);
	else
		return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_RENAME_LIFETIMENAME_N")), venue, oldName, newName, nEventChanges);
}


std::wstring CLocalization::ActionDeleteLifetimeName(
		std::wstring const& venue,
		std::wstring const& name,
		int nEventChanges) const
{
	if (0 == nEventChanges)
		return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_LIFETIMENAME_0")), venue, name);
	else
		return fmt::format(StringUtil::GetTranslation(arbT("IDS_ACTION_DELETE_LIFETIMENAME_N")), venue, name, nEventChanges);
}


std::wstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_TITLEPOINTSNAME")), points, faults);
}


std::wstring CLocalization::LifetimePointsNameWithSpeedPointsFormat(double faults) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_LIFETITLEPOINTSNAME")), StringUtil::GetTranslation(arbT("IDS_COL_SPEED")), faults);
}


std::wstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	std::wstring dbl1 = fmt::format(L"{:g}", points);
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_LIFETITLEPOINTSNAME")), dbl1, faults);
}


std::wstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_PLACEMENTPOINTSNAME")), points, place);
}


std::wstring CLocalization::WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(StringUtil::GetTranslation(arbT("IDS_WARN_DELETED_RUNS")), nRuns), inRunsMsg);
}


std::wstring CLocalization::UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_TEAM_RUNS")), nRuns), inRunsMsg);
}


std::wstring CLocalization::UpdateTableRuns(int nRuns) const
{
	return fmt::format(StringUtil::GetTranslation(arbT("IDS_UPDATE_TABLE_RUNS")), nRuns);
}


std::wstring CLocalization::ErrorInvalidDocStructure(wchar_t const* const inMsg) const
{
	if (inMsg)
		return fmt::format(L"{}: {}\n", m_InvalidDocStructure, inMsg);
	else
		return m_InvalidDocStructure;
}


std::wstring CLocalization::ErrorMissingAttribute(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, L"{}{}{}{}'.",
		m_InvalidFileFormat,
		inElement,
		m_InvalidFileMissingAttrib,
		inAttrib);
	if (inMsg)
		fmt::format_to(buffer, L" {}\n", inMsg);
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ErrorInvalidAttributeValue(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(buffer, L"{}{}{}{}'.",
		m_InvalidFileFormat,
		inElement,
		m_InvalidFileBadAttrib,
		inAttrib);
	if (inMsg)
		fmt::format_to(buffer, L" {}\n", inMsg);
	return fmt::to_string(buffer);
}
