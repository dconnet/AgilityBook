/*
 * Copyright © 2008-2009 David Connet. All Rights Reserved.
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
 * @brief Simple localization object to use for testing other apis
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "stdafx.h"
#include "Local.h"

#include "ARBStructure.h"


CLocalization::CLocalization()
{
}


void CLocalization::Load()
{
	m_UnknownVersion = wxT("Unknown document version");
	m_WarningNewerDoc = wxT("Warning: The file you are loading was created by a newer version of this program. Saving this file with this version of the program will result in a loss of data.\n\nAre you sure you want to continue?");
	m_InvalidDocStructure = wxT("Invalid document structure");
	m_InvalidFileFormat = wxT("Invalid file format: '");
	m_InvalidFileMissingAttrib = wxT("' is missing required attribute '");
	m_InvalidFileBadAttrib = wxT("' has an invalid value for the attribute '");
	m_InvalidDate = wxT("Invalid date: ");
	m_InvalidDivLevel = wxT("Division/level pair has not been defined: ");
	m_InvalidEventName = wxT("Event has not been defined: ");
	m_InvalidRoot = wxT("'") TREE_BOOK wxT("' must be the root element.");
	m_MissingConfig = wxT("Missing '") TREE_CONFIG wxT("' section.");
	m_InvalidConfig = wxT("Only one '") TREE_CONFIG wxT("' section is allowed.");
	m_InvalidVenueConfig = wxT("All '") TREE_DIVISION wxT("' elements must come before '") TREE_EVENT wxT("' elements.");
	m_InvalidDivName = wxT("Invalid division name: ");
	m_InvalidVenueName = wxT("Invalid venue name: ");
	m_InvalidOtherPtsName = wxT("Invalid Other Points name: ");
	m_InvalidMultiqName = wxT("Invalid Multiple Q name: ");
	m_InvalidMultiqConversion = wxT("Unable to convert Existing QQ Points");
	m_InvalidEvent = wxT("Unable to locate an event description in any of the venues of the affiliated clubs: ");
	m_InvalidTitle = wxT("Title is not defined in the Venue description: ");
	m_ValidValues = wxT("Valid values: ");

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + wxT("'y', 'n'");

	m_CalendarTentative = wxT("Information is tentative.");
	m_CalendarNotEntered = wxT("Not entered");
	m_CalendarEntered = wxT("Entered");
	m_CalendarPlanning = wxT("Planning");
	m_CalendarStatusN = wxT("Status: ") + m_CalendarNotEntered;
	m_CalendarStatusE = wxT("Status: ") + m_CalendarEntered;
	m_CalendarStatusP = wxT("Status: ") + m_CalendarPlanning;
	m_CalendarOpens = wxT("Trial opens: ");
	m_CalendarDraw = wxT("Trial draws: ");
	m_CalendarCloses = wxT("Trial closes: ");
	m_ScoreStyleUnknown = wxT("Unknown");
	m_ScoreStyleFaultsTime = wxT("Faults Then Time");
	m_ScoreStyleFaults100Time = wxT("100 Minus Faults Then Time");
	m_ScoreStyleFaults200Time = wxT("200 Minus Faults Then Time");
	m_ScoreStyleOCScoreTime = wxT("Opening/Closing Points Then Time");
	m_ScoreStyleScoreTime = wxT("Points Then Time");
	m_ScoreStyleTimePlusFaults = wxT("Time Plus Faults");
	m_ExistingPointsOther = wxT("OtherPoints");
	m_ExistingPointsRun = wxT("Run");
	m_ExistingPointsSpeed = wxT("Speed");
	m_ExistingPointsMQ = wxT("Multiple Q");
	m_ExistingPointsSQ = wxT("SuperQ");
}


tstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
    buffer << wxT("Calendar Sites: ");
	buffer << nNew << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Faults: ");
	buffer << nNew << wxT(" added, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("OtherPoints: ");
	buffer << nNew << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Venues: ");
	buffer << nNew << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Divisions: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateDivisionsReordered() const
{
	otstringstream buffer;
	buffer << wxT("Divisions: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Events: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateEventsReordered() const
{
	otstringstream buffer;
	buffer << wxT("Events: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("MultiQs: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nDeleted << wxT(" deleted, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateMultiqsReordered() const
{
	otstringstream buffer;
	buffer << wxT("MultiQs: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Levels: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateLevelsReordered() const
{
	otstringstream buffer;
	buffer << wxT("Levels: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT("Titles: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateTitlesReordered() const
{
	otstringstream buffer;
	buffer << wxT("Titles: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateSubLevels(int nAdded) const
{
	otstringstream buffer;
	buffer << nAdded << wxT(" new sub-levels");
	return buffer.str();
}


tstring CLocalization::UpdateSubLevelsReordered() const
{
	otstringstream buffer;
	buffer << wxT("Levels: ");
	buffer << wxT("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << wxT(" rules: ");
	buffer << nAdded << wxT(" added, ");
	buffer << nDeleted << wxT(" deleted, ");
	buffer << nUpdated << wxT(" updated, ");
	buffer << nSkipped << wxT(" identical");
	return buffer.str();
}


tstring CLocalization::ActionDeleteCalPlugin(tstring const& name) const
{
	otstringstream buffer;
    buffer << wxT("Action: Deleting Plugin [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameOtherPoints(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << wxT("Action: Renaming OtherPoints [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT(" OtherPoints");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteOtherPoints(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
    buffer << wxT("DELETING: OtherPoints [") << name << wxT("] ") << nChanges << wxT(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteOtherPoints(tstring const& name) const
{
	otstringstream buffer;
    buffer << wxT("Action: Deleting OtherPoints [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameVenue(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << wxT("Action: Renaming Venue [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
    	buffer << wxT(", updating ") << nChanges << wxT(" related items");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteVenue(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
    buffer << wxT("DELETING: Venue [") << name << wxT("] ") << nChanges << wxT(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteVenue(tstring const& name) const
{
	otstringstream buffer;
    buffer << wxT("Action: Deleting Venue [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameMultiQ(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << wxT("Action: Renaming ") << venue << wxT(" MultiQ [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT(" MultiQs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteMultiQ(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("DELETING: ") << venue << wxT(" MultiQ [") << name << wxT("] ") << nChanges << wxT(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteMultiQ(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << wxT("Action: Deleting ") << venue << wxT(" MultiQ [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameDivision(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("Action: Renaming ") << venue << wxT(" Division [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT(" runs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteDivision(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("DELETING: ") << venue << wxT(" Division [") << name << wxT("] ") << nChanges << wxT(" runs and existing points");
	return buffer.str();
}


tstring CLocalization::ActionDeleteDivision(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << wxT("Action: Deleting ") << venue << wxT(" Division [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameLevel(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("Action: Renaming ") << venue << wxT(" Level [") << oldName  << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT("!d! runs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteLevel(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("DELETING: ") << venue << wxT(" Level [") << name << wxT("] ") << nChanges << wxT(" runs");
	return buffer.str();
}


tstring CLocalization::ActionDeleteLevel(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << wxT("Action: Deleting ") << venue << wxT(" Level [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameTitle(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("Action: Renaming ") << venue << wxT(" Title [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT("!d! titles");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteTitle(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("DELETING: ") << venue << wxT(" Title []") << name << wxT(" ") << nChanges << wxT(" titles");
	return buffer.str();
}


tstring CLocalization::ActionDeleteTitle(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << wxT("Action: Deleting ") << venue << wxT(" Title [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameEvent(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("Action: Renaming ") << venue << wxT(" Event [") << oldName << wxT("] to [") << newName << wxT("]");
	if (0 < nChanges)
	{
		buffer << wxT(", updating ") << nChanges << wxT(" events");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteEvent(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << wxT("DELETING: ") << venue << wxT(" Event [") << name << wxT("] ") << nChanges << wxT(" events");
	return buffer.str();
}


tstring CLocalization::ActionDeleteEvent(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << wxT("Action: Deleting ") << venue << wxT(" Event [") << name << wxT("]");
	return buffer.str();
}


tstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	otstringstream buffer;
	buffer << points << wxT(" points with ") << faults << wxT(" faults");
	return buffer.str();
}


tstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	otstringstream buffer;
	buffer << points << wxT(" lifetime points with ") << faults << wxT(" faults");
	return buffer.str();
}


tstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	otstringstream buffer;
	buffer << points << wxT(" points with place of ") << place;
	return buffer.str();
}


tstring CLocalization::WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
    buffer << wxT("WARNING: ") << nRuns << wxT(" run(s) deleted due to configuration changes.\n(Date, Venue, Event, Division/Level)\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTeamRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
	buffer << wxT("USDAA 'Pairs' converted to 'Team' in ") << nRuns << wxT(" runs.") << wxT("\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTableRuns(int nRuns) const
{
	otstringstream buffer;
	buffer << wxT("Table setting updated in ") << nRuns << wxT(" runs.");
	return buffer.str();
}


tstring CLocalization::ErrorInvalidDocStructure(wxChar const* const inMsg) const
{
	otstringstream str;
	str << m_InvalidDocStructure;
	if (inMsg)
		str << ": " << inMsg << std::endl;
	return str.str();
}


tstring CLocalization::ErrorMissingAttribute(
		wxChar const* const inElement,
		wxChar const* const inAttrib,
		wxChar const* const inMsg) const
{
	otstringstream str;
	str << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileMissingAttrib
		<< inAttrib
		<< "'.";
	if (inMsg)
		str << " " << inMsg << std::endl;
	return str.str();
}


tstring CLocalization::ErrorInvalidAttributeValue(
		wxChar const* const inElement,
		wxChar const* const inAttrib,
		wxChar const* const inMsg) const
{
	otstringstream str;
	str << m_InvalidFileFormat
		<< inElement
		<< m_InvalidFileBadAttrib
		<< inAttrib
		<< "'.";
	if (inMsg)
		str << " " << inMsg << std::endl;
	return str.str();
}
