/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
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
 * @li 2013-09-03 DRC Changed sub level updating.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2007-10-11 DRC Created
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

	m_InvalidRoot = StringUtil::stringW(wxString::Format(_("IDS_INVALID_ROOT"), TREE_BOOK));
	m_MissingConfig = StringUtil::stringW(wxString::Format(_("IDS_MISSING_CONFIG"), TREE_CONFIG));
	m_InvalidConfig = StringUtil::stringW(wxString::Format(_("IDS_INVALID_CONFIG"), TREE_CONFIG));
	m_InvalidVenueConfig = StringUtil::stringW(wxString::Format(_("IDS_INVALID_VENUE_CONFIG"), TREE_DIVISION, TREE_EVENT));
	m_InvalidDivName = _("IDS_INVALID_DIV_NAME");
	m_InvalidVenueName = _("IDS_INVALID_VENUE_NAME");
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
	m_ExistingPointsOther = _("IDS_EXISTING_POINTS_OTHER");
	m_ExistingPointsRun = _("IDS_EXISTING_POINTS_RUN");
	m_ExistingPointsSpeed = _("IDS_EXISTING_POINTS_SPEED");
	m_ExistingPointsMQ = _("IDS_EXISTING_POINTS_MQ");
	m_ExistingPointsSQ = _("IDS_EXISTING_POINTS_SQ");

	return true;
}


std::wstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_UPDATE_CALSITES")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_FAULTS")
		<< L": "
		<<wxString::Format(_("IDS_UPDATE_ADDED"), nNew)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_OTHERPOINTS")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_VENUES")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_DIVISIONS")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateDivisionsReordered() const
{
	wxString buffer;
	buffer << _("IDS_COL_DIVISIONS")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_EVENTS")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateEventsReordered() const
{
	wxString buffer;
	buffer << _("IDS_COL_EVENTS")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_MULTIQS")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_DELETED"), nDeleted)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateMultiqsReordered() const
{
	wxString buffer;
	buffer << _("IDS_COL_MULTIQS")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_COL_LEVELS")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateLevelsReordered() const
{
	wxString buffer;
	buffer << _("IDS_COL_LEVELS")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_TITLES")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateTitlesReordered() const
{
	wxString buffer;
	buffer << _("IDS_TITLES")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << _("IDS_SUBLEVEL_NAME")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateSubLevelsReordered() const
{
	wxString buffer;
	buffer << _("IDS_SUBLEVEL_NAME")
		<< L": "
		<< _("IDS_REORDERED");
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	wxString buffer;
	buffer << L" "
		<< _("IDS_UPDATE_RULES")
		<< L": "
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_DELETED"), nDeleted)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated)
		<< L", "
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped);
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionDeleteCalPlugin(std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_CALPLUGIN"), name.c_str()));
}


std::wstring CLocalization::ActionRenameOtherPoints(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_OTHERPTS"), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_OTHERPTS2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteOtherPoints(
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_OTHERPTS"), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteOtherPoints(std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_OTHERPTS"), name.c_str()));
}


std::wstring CLocalization::ActionRenameVenue(
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_VENUE"), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_VENUE2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteVenue(
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_VENUE"), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteVenue(std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_VENUE"), name.c_str()));
}


std::wstring CLocalization::ActionRenameMultiQ(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_MULTIQ"), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_MULTIQ2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_MULTIQ"), venue.c_str(), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteMultiQ(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_MULTIQ"), venue.c_str(), name.c_str()));
}


std::wstring CLocalization::ActionRenameDivision(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_DIV"), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_DIV2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_DIV"), venue.c_str(), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteDivision(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_DIV"), venue.c_str(), name.c_str()));
}


std::wstring CLocalization::ActionRenameLevel(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_LEVEL"), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_LEVEL2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_LEVEL"), venue.c_str(), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteLevel(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_LEVEL"), venue.c_str(), name.c_str()));
}


std::wstring CLocalization::ActionRenameTitle(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_TITLE"), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_TITLE2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_TITLE"), venue.c_str(), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteTitle(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_TITLE"), venue.c_str(), name.c_str()));
}


std::wstring CLocalization::ActionRenameEvent(
		std::wstring const& venue,
		std::wstring const& oldName,
		std::wstring const& newName,
		int nChanges) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_EVENT"), venue.c_str(), oldName.c_str(), newName.c_str());
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_EVENT2"), nChanges);
	}
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::ActionPreDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name,
		int nChanges) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_PREDELETE_EVENT"), venue.c_str(), name.c_str(), nChanges));
}


std::wstring CLocalization::ActionDeleteEvent(
		std::wstring const& venue,
		std::wstring const& name) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_ACTION_DELETE_EVENT"), venue.c_str(), name.c_str()));
}


std::wstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	wxString dbl1 = wxString::Format(L"%g", points);
	wxString dbl2 = wxString::Format(L"%g", faults);
	return StringUtil::stringW(wxString::Format(_("IDS_TITLEPOINTSNAME"), dbl1.c_str(), dbl2.c_str()));
}


std::wstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	wxString dbl1 = wxString::Format(L"%g", points);
	wxString dbl2 = wxString::Format(L"%g", faults);
	return StringUtil::stringW(wxString::Format(_("IDS_LIFETITLEPOINTSNAME"), dbl1.c_str(), dbl2.c_str()));
}


std::wstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	wxString dbl1 = wxString::Format(L"%g", points);
	return StringUtil::stringW(wxString::Format(_("IDS_PLACEMENTPOINTSNAME"), dbl1.c_str(), (int)place));
}


std::wstring CLocalization::WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_WARN_DELETED_RUNS"), nRuns)
		<< L"\n" << inRunsMsg.c_str();
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const
{
	wxString buffer;
	buffer << wxString::Format(_("IDS_UPDATE_TEAM_RUNS"), nRuns)
		<< L"\n" << inRunsMsg.c_str();
	return StringUtil::stringW(buffer);
}


std::wstring CLocalization::UpdateTableRuns(int nRuns) const
{
	return StringUtil::stringW(wxString::Format(_("IDS_UPDATE_TABLE_RUNS"), nRuns));
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
