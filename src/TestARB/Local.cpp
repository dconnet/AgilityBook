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


namespace dconSoft
{
using namespace ARBCommon;

CLocalization::CLocalization()
{
}


bool CLocalization::Load()
{
	m_UnknownVersion = wxGetTranslation(arbT("IDS_ARB_UNKNOWN_VERSION"));
	if (m_UnknownVersion == L"IDS_ARB_UNKNOWN_VERSION")
		return false;

	m_WarningNewerDoc = wxGetTranslation(arbT("IDS_WARNING_NEWER_DOC"));
	m_InvalidDocStructure = wxGetTranslation(arbT("IDS_INVALID_DOC_STRUCTURE"));
	m_InvalidFileFormat = wxGetTranslation(arbT("IDS_INVALID_FILE_FORMAT"));
	m_InvalidFileMissingAttrib = wxGetTranslation(arbT("IDS_INVALID_FILE_MISSING_ATTRIB"));
	m_InvalidFileBadAttrib = wxGetTranslation(arbT("IDS_INVALID_FILE_BAD_ATTRIB"));
	m_InvalidDate = wxGetTranslation(arbT("IDS_INVALID_DATE"));
	m_InvalidDivLevel = wxGetTranslation(arbT("IDS_INVALID_DIV_LEVEL"));
	m_InvalidEventName = wxGetTranslation(arbT("IDS_INVALID_EVENT_NAME"));

	m_InvalidRoot = wxString::Format(wxGetTranslation(arbT("IDS_INVALID_ROOT")), TREE_BOOK);
	m_MissingConfig = wxString::Format(wxGetTranslation(arbT("IDS_MISSING_CONFIG")), TREE_CONFIG);
	m_InvalidConfig = wxString::Format(wxGetTranslation(arbT("IDS_INVALID_CONFIG")), TREE_CONFIG);
	m_InvalidVenueConfig
		= wxString::Format(wxGetTranslation(arbT("IDS_INVALID_VENUE_CONFIG")), TREE_DIVISION, TREE_EVENT);
	m_InvalidDivName = wxGetTranslation(arbT("IDS_INVALID_DIV_NAME"));
	m_InvalidVenueName = wxGetTranslation(arbT("IDS_INVALID_VENUE_NAME"));
	m_InvalidLifetimeName = wxGetTranslation(arbT("IDS_INVALID_LIFETIME_NAME"));
	m_InvalidOtherPtsName = wxGetTranslation(arbT("IDS_INVALID_OTHER_PTS_NAME"));
	m_InvalidMultiqName = wxGetTranslation(arbT("IDS_INVALID_MULTIQ_NAME"));
	m_InvalidMultiqConversion = wxGetTranslation(arbT("IDS_INVALID_MULTIQ_CONVERSION"));
	m_InvalidEvent = wxGetTranslation(arbT("IDS_INVALID_EVENT"));
	m_InvalidTitle = wxGetTranslation(arbT("IDS_INVALID_TITLE"));
	m_ValidValues = wxGetTranslation(arbT("IDS_VALID_VALUES"));

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + L"'y', 'n'";

	m_CalendarTentative = wxGetTranslation(arbT("IDS_CALENDAR_TENTATIVE"));
	m_CalendarPast = wxGetTranslation(arbT("IDS_CALENDAR_PAST"));
	m_CalendarNotEntered = wxGetTranslation(arbT("IDS_CALENDAR_NOTENTERED"));
	m_CalendarEntered = wxGetTranslation(arbT("IDS_CALENDAR_ENTERED"));
	m_CalendarPending = wxGetTranslation(arbT("IDS_CALENDAR_PENDING"));
	m_CalendarPlanning = wxGetTranslation(arbT("IDS_CALENDAR_PLANNING"));
	m_CalendarStatusN = m_CalendarStatusE = m_CalendarStatusO = m_CalendarStatusP
		= wxGetTranslation(arbT("IDS_CALENDAR_STATUS"));
	m_CalendarStatusN += m_CalendarNotEntered;
	m_CalendarStatusE += m_CalendarEntered;
	m_CalendarStatusO += m_CalendarPending;
	m_CalendarStatusP += m_CalendarPlanning;
	m_CalendarOpens = wxGetTranslation(arbT("IDS_CALENDAR_OPENS"));
	m_CalendarDraw = wxGetTranslation(arbT("IDS_CALENDAR_DRAW"));
	m_CalendarCloses = wxGetTranslation(arbT("IDS_CALENDAR_CLOSES"));
	m_ScoreStyleUnknown = wxGetTranslation(arbT("IDS_SCORE_STYLE_UNKNOWN"));
	m_ScoreStyleFaultsTime = wxGetTranslation(arbT("IDS_SCORE_STYLE_FAULTS_TIME"));
	m_ScoreStyleFaults100Time = wxGetTranslation(arbT("IDS_SCORE_STYLE_FAULTS100_TIME"));
	m_ScoreStyleFaults200Time = wxGetTranslation(arbT("IDS_SCORE_STYLE_FAULTS200_TIME"));
	m_ScoreStyleOCScoreTime = wxGetTranslation(arbT("IDS_SCORE_STYLE_OCSCORE_TIME"));
	m_ScoreStyleScoreTime = wxGetTranslation(arbT("IDS_SCORE_STYLE_SCORE_TIME"));
	m_ScoreStyleTimePlusFaults = wxGetTranslation(arbT("IDS_SCORE_STYLE_TIME_PLUS_FAULTS"));
	m_ScoreStyleTimeNoPlaces = wxGetTranslation(arbT("IDS_SCORE_STYLE_TIME_NO_PLACES"));
	m_ScoreStylePassFail = wxGetTranslation(arbT("IDS_SCORE_STYLE_PASS_FAIL"));
	m_ExistingPointsOther = wxGetTranslation(arbT("IDS_EXISTING_POINTS_OTHER"));
	m_ExistingPointsLifetime = wxGetTranslation(arbT("IDS_EXISTING_POINTS_LIFETIME"));
	m_ExistingPointsRun = wxGetTranslation(arbT("IDS_EXISTING_POINTS_RUN"));
	m_ExistingPointsSpeed = wxGetTranslation(arbT("IDS_EXISTING_POINTS_SPEED"));
	m_ExistingPointsMQ = wxGetTranslation(arbT("IDS_EXISTING_POINTS_MQ"));
	m_ExistingPointsSQ = wxGetTranslation(arbT("IDS_EXISTING_POINTS_SQ"));

	return true;
}


wxString CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_FAULTS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nNew) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_OTHERPOINTS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nNew) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_VENUES")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nNew) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateLifetimeNames(int nNew, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_LIFETIMENAMES")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nNew) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_DIVISIONS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateDivisionsReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_DIVISIONS")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_EVENTS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateEventsReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_EVENTS")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_MULTIQS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_DELETED")), nDeleted) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateMultiqsReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_MULTIQS")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_LEVELS")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateLevelsReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_COL_LEVELS")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_TITLES")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateTitlesReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_TITLES")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_SUBLEVEL_NAME")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::UpdateSubLevelsReordered() const
{
	wxString str;
	str << wxGetTranslation(arbT("IDS_SUBLEVEL_NAME")) << L": " << wxGetTranslation(arbT("IDS_REORDERED"));
	return str;
}


wxString CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	wxString str;
	str << L" " << wxGetTranslation(arbT("IDS_UPDATE_RULES")) << L": "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_ADDED")), nAdded) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_DELETED")), nDeleted) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_UPDATED")), nUpdated) << L", "
		<< wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_IDENTICAL")), nSkipped);
	return str;
}


wxString CLocalization::ActionRenameOtherPoints(wxString const& oldName, wxString const& newName, int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS")), oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_OTHERPTS2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteOtherPoints(wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_OTHERPTS")), name, nChanges);
}


wxString CLocalization::ActionDeleteOtherPoints(wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_OTHERPTS")), name);
}


wxString CLocalization::ActionRenameVenue(wxString const& oldName, wxString const& newName, int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_VENUE")), oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_VENUE2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteVenue(wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_VENUE")), name, nChanges);
}


wxString CLocalization::ActionDeleteVenue(wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_VENUE")), name);
}


wxString CLocalization::ActionRenameMultiQ(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nChanges) const
{
	wxString str;
	wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ")), venue, oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_MULTIQ2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteMultiQ(wxString const& venue, wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_MULTIQ")), venue, name, nChanges);
}


wxString CLocalization::ActionDeleteMultiQ(wxString const& venue, wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_MULTIQ")), venue, name);
}


wxString CLocalization::ActionRenameDivision(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_DIV")), venue, oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_DIV2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteDivision(wxString const& venue, wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_DIV")), venue, name, nChanges);
}


wxString CLocalization::ActionDeleteDivision(wxString const& venue, wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_DIV")), venue, name);
}


wxString CLocalization::ActionRenameLevel(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_LEVEL")), venue, oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_LEVEL2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteLevel(wxString const& venue, wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_LEVEL")), venue, name, nChanges);
}


wxString CLocalization::ActionDeleteLevel(wxString const& venue, wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_LEVEL")), venue, name);
}


wxString CLocalization::ActionRenameTitle(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_TITLE")), venue, oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_TITLE2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteTitle(wxString const& venue, wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_TITLE")), venue, name, nChanges);
}


wxString CLocalization::ActionDeleteTitle(wxString const& venue, wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_TITLE")), venue, name);
}


wxString CLocalization::ActionRenameEvent(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nChanges) const
{
	wxString str = wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_EVENT")), venue, oldName, newName);
	if (0 < nChanges)
	{
		str << wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_EVENT2")), nChanges);
	}
	return str;
}


wxString CLocalization::ActionPreDeleteEvent(wxString const& venue, wxString const& name, int nChanges) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_PREDELETE_EVENT")), venue, name, nChanges);
}


wxString CLocalization::ActionDeleteEvent(wxString const& venue, wxString const& name) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_EVENT")), venue, name);
}


wxString CLocalization::ActionRenameLifetimeName(
	wxString const& venue,
	wxString const& oldName,
	wxString const& newName,
	int nEventChanges) const
{
	if (0 == nEventChanges)
		return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_RENAME_LIFETIMENAME_0")), venue, oldName, newName);
	else
		return wxString::Format(
			wxGetTranslation(arbT("IDS_ACTION_RENAME_LIFETIMENAME_N")),
			venue,
			oldName,
			newName,
			nEventChanges);
}


wxString CLocalization::ActionDeleteLifetimeName(wxString const& venue, wxString const& name, int nEventChanges) const
{
	if (0 == nEventChanges)
		return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_LIFETIMENAME_0")), venue, name);
	else
		return wxString::Format(wxGetTranslation(arbT("IDS_ACTION_DELETE_LIFETIMENAME_N")), venue, name, nEventChanges);
}


wxString CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_TITLEPOINTSNAME")), points, faults);
}


wxString CLocalization::LifetimePointsNameWithSpeedPointsFormat(double faults) const
{
	return wxString::Format(
		wxGetTranslation(arbT("IDS_LIFETITLEPOINTSNAME")),
		wxGetTranslation(arbT("IDS_COL_SPEED")),
		faults);
}


wxString CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	wxString dbl1 = ARBDouble::ToString(points, 0);
	return wxString::Format(wxGetTranslation(arbT("IDS_LIFETITLEPOINTSNAME")), dbl1, faults);
}


wxString CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_PLACEMENTPOINTSNAME")), points, place);
}


wxString CLocalization::WarnDeletedRuns(int nRuns, wxString const& inRunsMsg) const
{
	wxString str;
	str << wxString::Format(wxGetTranslation(arbT("IDS_WARN_DELETED_RUNS")), nRuns) << L"\n" << inRunsMsg;
	return str;
}


wxString CLocalization::UpdateTeamRuns(int nRuns, wxString const& inRunsMsg) const
{
	wxString str;
	str << wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_TEAM_RUNS")), nRuns) << L"\n" << inRunsMsg;
	return str;
}


wxString CLocalization::UpdateTableRuns(int nRuns) const
{
	return wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_TABLE_RUNS")), nRuns);
}


wxString CLocalization::UpdateTableRuns(int nRuns, wxString const& inRunsMsg) const
{
	wxString str;
	str << wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_TABLE_RUNS2")), nRuns) << L"\n" << inRunsMsg;
	return str;
}


wxString CLocalization::UpdateSubnameRuns(int nRuns, wxString const& inRunsMsg) const
{
	wxString str;
	str << wxString::Format(wxGetTranslation(arbT("IDS_UPDATE_SUBNAME_RUNS")), nRuns) << L"\n" << inRunsMsg;
	return str;
}


wxString CLocalization::ErrorInvalidDocStructure(wchar_t const* const inMsg) const
{
	if (inMsg)
	{
		wxString str;
		str << m_InvalidDocStructure << L": " << inMsg << L"\n";
		return str;
	}
	else
		return m_InvalidDocStructure;
}


wxString CLocalization::ErrorMissingAttribute(
	wchar_t const* const inElement,
	wchar_t const* const inAttrib,
	wchar_t const* const inMsg) const
{
	wxString str;
	str << m_InvalidFileFormat << inElement << m_InvalidFileMissingAttrib << inAttrib << L"'.";
	if (inMsg)
		str << L" " << inMsg << L"\n";
	return str;
}


wxString CLocalization::ErrorInvalidAttributeValue(
	wchar_t const* const inElement,
	wchar_t const* const inAttrib,
	wchar_t const* const inMsg) const
{
	wxString str;
	str << m_InvalidFileFormat << inElement << m_InvalidFileBadAttrib << inAttrib << L"'.";
	if (inMsg)
		str << L" " << inMsg << L"\n";
	return str;
}

} // namespace dconSoft
