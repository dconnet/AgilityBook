/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @li 2009-07-19 DRC Ported to wxWidgets.
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "Local.h"

#include "ARBStructure.h"
#include <wx/string.h>


CLocalization::CLocalization()
{
}


void CLocalization::Load()
{
	m_UnknownVersion = _("IDS_ARB_UNKNOWN_VERSION");
	m_WarningNewerDoc = _("IDS_WARNING_NEWER_DOC");
	m_InvalidDocStructure = _("IDS_INVALID_DOC_STRUCTURE");
	m_InvalidFileFormat = _("IDS_INVALID_FILE_FORMAT");
	m_InvalidFileMissingAttrib = _("IDS_INVALID_FILE_MISSING_ATTRIB");
	m_InvalidFileBadAttrib = _("IDS_INVALID_FILE_BAD_ATTRIB");
	m_InvalidDate = _("IDS_INVALID_DATE");
	m_InvalidDivLevel = _("IDS_INVALID_DIV_LEVEL");
	m_InvalidEventName = _("IDS_INVALID_EVENT_NAME");

	m_InvalidRoot = wxString::Format(_("IDS_INVALID_ROOT"), TREE_BOOK).c_str();
	m_MissingConfig = wxString::Format(_("IDS_MISSING_CONFIG"), TREE_CONFIG).c_str();
	m_InvalidConfig = wxString::Format(_("IDS_INVALID_CONFIG"), TREE_CONFIG).c_str();
	m_InvalidVenueConfig = wxString::Format(_("IDS_INVALID_VENUE_CONFIG"), TREE_DIVISION, TREE_EVENT).c_str();
	m_InvalidDivName = _("IDS_INVALID_DIV_NAME");
	m_InvalidVenueName = _("IDS_INVALID_VENUE_NAME");
	m_InvalidOtherPtsName = _("IDS_INVALID_OTHER_PTS_NAME");
	m_InvalidMultiqName = _("IDS_INVALID_MULTIQ_NAME");
	m_InvalidMultiqConversion = _("IDS_INVALID_MULTIQ_CONVERSION");
	m_InvalidEvent = _("IDS_INVALID_EVENT");
	m_InvalidTitle = _("IDS_INVALID_TITLE");
	m_ValidValues = _("IDS_VALID_VALUES");

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + wxT("'y', 'n'");

	m_CalendarTentative = _("IDS_CALENDAR_TENTATIVE");
	m_CalendarNotEntered = _("IDS_CALENDAR_NOTENTERED");
	m_CalendarEntered = _("IDS_CALENDAR_ENTERED");
	m_CalendarPlanning = _("IDS_CALENDAR_PLANNING");
	m_CalendarStatusN = m_CalendarStatusE = m_CalendarStatusP = _("IDS_CALENDAR_STATUS");
	m_CalendarStatusN += m_CalendarNotEntered;
	m_CalendarStatusE += m_CalendarEntered;
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
}


tstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_UPDATE_CALSITES")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_FAULTS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_OTHERPOINTS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_VENUES")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nNew).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_DIVISIONS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateDivisionsReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_COL_DIVISIONS")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_EVENTS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateEventsReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_COL_EVENTS")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_MULTIQS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_DELETED"), nDeleted).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateMultiqsReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_COL_MULTIQS")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_COL_LEVELS")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateLevelsReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_COL_LEVELS")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _("IDS_TITLES")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::UpdateTitlesReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_TITLES")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateSubLevels(int nAdded) const
{
	return wxString::Format(_("IDS_UPDATE_NEWSUBLEVELS"), nAdded).c_str();
}


tstring CLocalization::UpdateSubLevelsReordered() const
{
	otstringstream buffer;
	buffer << _("IDS_COL_LEVELS")
		<< wxT(": ")
		<< _("IDS_REORDERED");
	return buffer.str();
}


tstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT(" ")
		<< _("IDS_UPDATE_RULES")
		<< wxT(": ")
		<< wxString::Format(_("IDS_UPDATE_ADDED"), nAdded).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_DELETED"), nDeleted).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_UPDATED"), nUpdated).c_str()
		<< wxT(", ")
		<< wxString::Format(_("IDS_UPDATE_IDENTICAL"), nSkipped).c_str();
	return buffer.str();
}


tstring CLocalization::ActionDeleteCalPlugin(tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_CALPLUGIN"), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameOtherPoints(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_OTHERPTS"), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_OTHERPTS2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteOtherPoints(
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_OTHERPTS"), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteOtherPoints(tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_OTHERPTS"), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameVenue(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_VENUE"), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_VENUE2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteVenue(
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_VENUE"), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteVenue(tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_VENUE"), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameMultiQ(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_MULTIQ"), venue.c_str(), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_MULTIQ2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteMultiQ(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_MULTIQ"), venue.c_str(), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteMultiQ(
		tstring const& venue,
		tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_MULTIQ"), venue.c_str(), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameDivision(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_DIV"), venue.c_str(), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_DIV2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteDivision(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_DIV"), venue.c_str(), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteDivision(
		tstring const& venue,
		tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_DIV"), venue.c_str(), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameLevel(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_LEVEL"), venue.c_str(), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_LEVEL2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteLevel(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_LEVEL"), venue.c_str(), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteLevel(
		tstring const& venue,
		tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_LEVEL"), venue.c_str(), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameTitle(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_TITLE"), venue.c_str(), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_TITLE2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteTitle(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_TITLE"), venue.c_str(), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteTitle(
		tstring const& venue,
		tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_TITLE"), venue.c_str(), name.c_str()).c_str();
}


tstring CLocalization::ActionRenameEvent(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_ACTION_RENAME_EVENT"), venue.c_str(), oldName.c_str(), newName.c_str()).c_str();
	if (0 < nChanges)
	{
		buffer << wxString::Format(_("IDS_ACTION_RENAME_EVENT2"), nChanges).c_str();
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteEvent(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	return wxString::Format(_("IDS_ACTION_PREDELETE_EVENT"), venue.c_str(), name.c_str(), nChanges).c_str();
}


tstring CLocalization::ActionDeleteEvent(
		tstring const& venue,
		tstring const& name) const
{
	return wxString::Format(_("IDS_ACTION_DELETE_EVENT"), venue.c_str(), name.c_str()).c_str();
}


tstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	otstringstream dbl1, dbl2;
	dbl1 << points;
	dbl2 << faults;
	return wxString::Format(_("IDS_TITLEPOINTSNAME"), dbl1.str().c_str(), dbl2.str().c_str()).c_str();
}


tstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	otstringstream dbl1, dbl2;
	dbl1 << points;
	dbl2 << faults;
	return wxString::Format(_("IDS_LIFETITLEPOINTSNAME"), dbl1.str().c_str(), dbl2.str().c_str()).c_str();
}


tstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	otstringstream dbl1;
	dbl1 << points;
	return wxString::Format(_("IDS_PLACEMENTPOINTSNAME"), dbl1.str().c_str(), (int)place).c_str();
}


tstring CLocalization::WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_WARN_DELETED_RUNS"), nRuns).c_str()
		<< wxT("\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTeamRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
	buffer << wxString::Format(_("IDS_UPDATE_TEAM_RUNS"), nRuns).c_str()
		<< wxT("\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTableRuns(int nRuns) const
{
	return wxString::Format(_("IDS_UPDATE_TABLE_RUNS"), nRuns).c_str();
}


tstring CLocalization::ErrorInvalidDocStructure(wxChar const* const inMsg) const
{
	otstringstream buffer;
	buffer << m_InvalidDocStructure;
	if (inMsg)
		buffer << ": " << inMsg << std::endl;
	return buffer.str();
}


tstring CLocalization::ErrorMissingAttribute(
		wxChar const* const inElement,
		wxChar const* const inAttrib,
		wxChar const* const inMsg) const
{
	otstringstream buffer;
	buffer << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileMissingAttrib
		<< inAttrib
		<< "'.";
	if (inMsg)
		buffer << " " << inMsg << std::endl;
	return buffer.str();
}


tstring CLocalization::ErrorInvalidAttributeValue(
		wxChar const* const inElement,
		wxChar const* const inAttrib,
		wxChar const* const inMsg) const
{
	otstringstream buffer;
	buffer << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileBadAttrib
		<< inAttrib
		<< "'.";
	if (inMsg)
		buffer << " " << inMsg << std::endl;
	return buffer.str();
}
