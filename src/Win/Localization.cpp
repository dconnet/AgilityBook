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
 * 2013-09-03 Changed sub level updating.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-10-11 Created
 */

#include "stdafx.h"
#include "Localization.h"

#include "ARB/ARBStructure.h"
#include "ARBCommon/StringUtil.h"
#include <wx/string.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CLocalization::CLocalization()
{
}


bool CLocalization::Load()
{
	m_UnknownVersion = _("IDS_ARB_UNKNOWN_VERSION");
	if (m_UnknownVersion == L"IDS_ARB_UNKNOWN_VERSION")
		return false;

	m_WarningNewerDoc = _("IDS_WARNING_NEWER_DOC");
	m_InvalidDocStructure = _("IDS_INVALID_DOC_STRUCTURE");
	m_InvalidFileFormat = _("IDS_INVALID_FILE_FORMAT");
	m_InvalidFileMissingAttrib = _("IDS_INVALID_FILE_MISSING_ATTRIB");
	m_InvalidFileBadAttrib = _("IDS_INVALID_FILE_BAD_ATTRIB");
	m_InvalidDate = _("IDS_INVALID_DATE");
	m_InvalidDivLevel = _("IDS_INVALID_DIV_LEVEL");
	m_InvalidEventName = _("IDS_INVALID_EVENT_NAME");

	m_InvalidRoot = fmt::format(_("IDS_INVALID_ROOT").wx_str(), TREE_BOOK);
	m_MissingConfig = fmt::format(_("IDS_MISSING_CONFIG").wx_str(), TREE_CONFIG);
	m_InvalidConfig = fmt::format(_("IDS_INVALID_CONFIG").wx_str(), TREE_CONFIG);
	m_InvalidVenueConfig = fmt::format(_("IDS_INVALID_VENUE_CONFIG").wx_str(), TREE_DIVISION, TREE_EVENT);
	m_InvalidDivName = _("IDS_INVALID_DIV_NAME");
	m_InvalidVenueName = _("IDS_INVALID_VENUE_NAME");
	m_InvalidLifetimeName = _("IDS_INVALID_LIFETIME_NAME");
	m_InvalidOtherPtsName = _("IDS_INVALID_OTHER_PTS_NAME");
	m_InvalidMultiqName = _("IDS_INVALID_MULTIQ_NAME");
	m_InvalidMultiqConversion = _("IDS_INVALID_MULTIQ_CONVERSION");
	m_InvalidEvent = _("IDS_INVALID_EVENT");
	m_InvalidTitle = _("IDS_INVALID_TITLE");
	m_ValidValues = _("IDS_VALID_VALUES");

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + L"'y', 'n'";

	m_CalendarTentative = _("IDS_CALENDAR_TENTATIVE");
	m_CalendarPast = _("IDS_CALENDAR_PAST");
	m_CalendarNotEntered = _("IDS_CALENDAR_NOTENTERED");
	m_CalendarEntered = _("IDS_CALENDAR_ENTERED");
	m_CalendarPending = _("IDS_CALENDAR_PENDING");
	m_CalendarPlanning = _("IDS_CALENDAR_PLANNING");
	m_CalendarStatusN = m_CalendarStatusE = m_CalendarStatusO = m_CalendarStatusP = _("IDS_CALENDAR_STATUS");
	m_CalendarStatusN += m_CalendarNotEntered;
	m_CalendarStatusE += m_CalendarEntered;
	m_CalendarStatusO += m_CalendarPending;
	m_CalendarStatusP += m_CalendarPlanning;
	m_CalendarOpens = _("IDS_CALENDAR_OPENS");
	m_CalendarDraw = _("IDS_CALENDAR_DRAW");
	m_CalendarCloses = _("IDS_CALENDAR_CLOSES");
	m_ScoreStyleUnknown = _("IDS_SCORE_STYLE_UNKNOWN");
	m_ScoreStyleFaultsTime = _("IDS_SCORE_STYLE_FAULTS_TIME");
	m_ScoreStyleFaults100Time = _("IDS_SCORE_STYLE_FAULTS100_TIME");
	m_ScoreStyleFaults200Time = _("IDS_SCORE_STYLE_FAULTS200_TIME");
	m_ScoreStyleOCScoreTime = _("IDS_SCORE_STYLE_OCSCORE_TIME");
	m_ScoreStyleScoreTime = _("IDS_SCORE_STYLE_SCORE_TIME");
	m_ScoreStyleTimePlusFaults = _("IDS_SCORE_STYLE_TIME_PLUS_FAULTS");
	m_ScoreStyleTimeNoPlaces = _("IDS_SCORE_STYLE_TIME_NO_PLACES");
	m_ExistingPointsOther = _("IDS_EXISTING_POINTS_OTHER");
	m_ExistingPointsLifetime = _("IDS_EXISTING_POINTS_LIFETIME");
	m_ExistingPointsRun = _("IDS_EXISTING_POINTS_RUN");
	m_ExistingPointsSpeed = _("IDS_EXISTING_POINTS_SPEED");
	m_ExistingPointsMQ = _("IDS_EXISTING_POINTS_MQ");
	m_ExistingPointsSQ = _("IDS_EXISTING_POINTS_SQ");

	return true;
}


std::wstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}",
		_("IDS_COL_FAULTS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nNew),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_OTHERPOINTS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nNew),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_COL_VENUES").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nNew),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateLifetimeNames(int nNew, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}",
		_("IDS_LIFETIMENAMES").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nNew),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_COL_DIVISIONS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateDivisionsReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_COL_DIVISIONS").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_COL_EVENTS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateEventsReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_COL_EVENTS").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_COL_MULTIQS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_DELETED").wx_str(), nDeleted),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateMultiqsReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_COL_MULTIQS").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_COL_LEVELS").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateLevelsReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_COL_LEVELS").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_TITLES").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateTitlesReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_TITLES").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L"{}: {}, {}, {}",
		_("IDS_SUBLEVEL_NAME").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::UpdateSubLevelsReordered() const
{
	return fmt::format(L"{}: {}", _("IDS_SUBLEVEL_NAME").wx_str(), _("IDS_REORDERED").wx_str());
}


std::wstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	return fmt::format(
		L" {}: {}, {}, {}, {}",
		_("IDS_UPDATE_RULES").wx_str(),
		fmt::format(_("IDS_UPDATE_ADDED").wx_str(), nAdded),
		fmt::format(_("IDS_UPDATE_DELETED").wx_str(), nDeleted),
		fmt::format(_("IDS_UPDATE_UPDATED").wx_str(), nUpdated),
		fmt::format(_("IDS_UPDATE_IDENTICAL").wx_str(), nSkipped));
}


std::wstring CLocalization::ActionRenameOtherPoints(
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_OTHERPTS").wx_str(), oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_OTHERPTS2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteOtherPoints(std::wstring const& name, int nChanges) const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_OTHERPTS").wx_str(), name, nChanges);
}


std::wstring CLocalization::ActionDeleteOtherPoints(std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_OTHERPTS").wx_str(), name);
}


std::wstring CLocalization::ActionRenameVenue(std::wstring const& oldName, std::wstring const& newName, int nChanges)
	const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_VENUE").wx_str(), oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_VENUE2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteVenue(std::wstring const& name, int nChanges) const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_VENUE").wx_str(), name, nChanges);
}


std::wstring CLocalization::ActionDeleteVenue(std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_VENUE").wx_str(), name);
}


std::wstring CLocalization::ActionRenameMultiQ(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_MULTIQ").wx_str(), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_MULTIQ2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteMultiQ(std::wstring const& venue, std::wstring const& name, int nChanges)
	const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_MULTIQ").wx_str(), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteMultiQ(std::wstring const& venue, std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_MULTIQ").wx_str(), venue, name);
}


std::wstring CLocalization::ActionRenameDivision(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_DIV").wx_str(), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_DIV2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteDivision(std::wstring const& venue, std::wstring const& name, int nChanges)
	const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_DIV").wx_str(), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteDivision(std::wstring const& venue, std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_DIV").wx_str(), venue, name);
}


std::wstring CLocalization::ActionRenameLevel(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_LEVEL").wx_str(), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_LEVEL2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteLevel(std::wstring const& venue, std::wstring const& name, int nChanges)
	const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_LEVEL").wx_str(), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteLevel(std::wstring const& venue, std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_LEVEL").wx_str(), venue, name);
}


std::wstring CLocalization::ActionRenameTitle(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_TITLE").wx_str(), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_TITLE2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteTitle(std::wstring const& venue, std::wstring const& name, int nChanges)
	const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_TITLE").wx_str(), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteTitle(std::wstring const& venue, std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_TITLE").wx_str(), venue, name);
}


std::wstring CLocalization::ActionRenameEvent(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nChanges) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_EVENT").wx_str(), venue, oldName, newName);
	if (0 < nChanges)
	{
		fmt::format_to(std::back_inserter(buffer), _("IDS_ACTION_RENAME_EVENT2").wx_str(), nChanges);
	}
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ActionPreDeleteEvent(std::wstring const& venue, std::wstring const& name, int nChanges)
	const
{
	return fmt::format(_("IDS_ACTION_PREDELETE_EVENT").wx_str(), venue, name, nChanges);
}


std::wstring CLocalization::ActionDeleteEvent(std::wstring const& venue, std::wstring const& name) const
{
	return fmt::format(_("IDS_ACTION_DELETE_EVENT").wx_str(), venue, name);
}


std::wstring CLocalization::ActionRenameLifetimeName(
	std::wstring const& venue,
	std::wstring const& oldName,
	std::wstring const& newName,
	int nEventChanges) const
{
	if (0 == nEventChanges)
		return fmt::format(_("IDS_ACTION_RENAME_LIFETIMENAME_0").wx_str(), venue, oldName, newName);
	else
		return fmt::format(_("IDS_ACTION_RENAME_LIFETIMENAME_N").wx_str(), venue, oldName, newName, nEventChanges);
}


std::wstring CLocalization::ActionDeleteLifetimeName(
	std::wstring const& venue,
	std::wstring const& name,
	int nEventChanges) const
{
	if (0 == nEventChanges)
		return fmt::format(_("IDS_ACTION_DELETE_LIFETIMENAME_0").wx_str(), venue, name);
	else
		return fmt::format(_("IDS_ACTION_DELETE_LIFETIMENAME_N").wx_str(), venue, name, nEventChanges);
}


std::wstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	return fmt::format(_("IDS_TITLEPOINTSNAME").wx_str(), points, faults);
}


std::wstring CLocalization::LifetimePointsNameWithSpeedPointsFormat(double faults) const
{
	return fmt::format(
		_("IDS_LIFETITLEPOINTSNAME").wx_str(),
		StringUtil::GetTranslation(arbT("IDS_COL_SPEED")),
		faults);
}


std::wstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	std::wstring dbl1 = ARBDouble::ToString(points, 0);
	return fmt::format(_("IDS_LIFETITLEPOINTSNAME").wx_str(), dbl1, faults);
}


std::wstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	return fmt::format(_("IDS_PLACEMENTPOINTSNAME").wx_str(), points, place);
}


std::wstring CLocalization::WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(_("IDS_WARN_DELETED_RUNS").wx_str(), nRuns), inRunsMsg);
}


std::wstring CLocalization::UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(_("IDS_UPDATE_TEAM_RUNS").wx_str(), nRuns), inRunsMsg);
}


std::wstring CLocalization::UpdateTableRuns(int nRuns) const
{
	return fmt::format(_("IDS_UPDATE_TABLE_RUNS").wx_str(), nRuns);
}


std::wstring CLocalization::UpdateTableRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(_("IDS_UPDATE_TABLE_RUNS2").wx_str(), nRuns), inRunsMsg);
}


std::wstring CLocalization::UpdateSubnameRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	return fmt::format(L"{}\n{}", fmt::format(_("IDS_UPDATE_SUBNAME_RUNS").wx_str(), nRuns), inRunsMsg);
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
	fmt::format_to(
		std::back_inserter(buffer),
		L"{}{}{}{}'.",
		m_InvalidFileFormat,
		inElement,
		m_InvalidFileMissingAttrib,
		inAttrib);
	if (inMsg)
		fmt::format_to(std::back_inserter(buffer), L" {}\n", inMsg);
	return fmt::to_string(buffer);
}


std::wstring CLocalization::ErrorInvalidAttributeValue(
	wchar_t const* const inElement,
	wchar_t const* const inAttrib,
	wchar_t const* const inMsg) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(
		std::back_inserter(buffer),
		L"{}{}{}{}'.",
		m_InvalidFileFormat,
		inElement,
		m_InvalidFileBadAttrib,
		inAttrib);
	if (inMsg)
		fmt::format_to(std::back_inserter(buffer), L" {}\n", inMsg);
	return fmt::to_string(buffer);
}
