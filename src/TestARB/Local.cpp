/*
 * Copyright © 2008 David Connet. All Rights Reserved.
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
	m_UnknownVersion = _T("Unknown document version");
	m_WarningNewerDoc = _T("Warning: The file you are loading was created by a newer version of this program. Saving this file with this version of the program will result in a loss of data.\n\nAre you sure you want to continue?");
	m_InvalidDocStructure = _T("Invalid document structure");
	m_InvalidFileFormat = _T("Invalid file format: '");
	m_InvalidFileMissingAttrib = _T("' is missing required attribute '");
	m_InvalidFileBadAttrib = _T("' has an invalid value for the attribute '");
	m_InvalidDate = _T("Invalid date: ");
	m_InvalidDivLevel = _T("Division/level pair has not been defined: ");
	m_InvalidEventName = _T("Event has not been defined: ");
	m_InvalidRoot = _T("'") TREE_BOOK _T("' must be the root element.");
	m_MissingConfig = _T("Missing '") TREE_CONFIG _T("' section.");
	m_InvalidConfig = _T("Only one '") TREE_CONFIG _T("' section is allowed.");
	m_InvalidVenueConfig = _T("All '") TREE_DIVISION _T("' elements must come before '") TREE_EVENT _T("' elements.");
	m_InvalidDivName = _T("Invalid division name: ");
	m_InvalidVenueName = _T("Invalid venue name: ");
	m_InvalidOtherPtsName = _T("Invalid Other Points name: ");
	m_InvalidMultiqName = _T("Invalid Multiple Q name: ");
	m_InvalidMultiqConversion = _T("Unable to convert Existing QQ Points");
	m_InvalidEvent = _T("Unable to locate an event description in any of the venues of the affiliated clubs: ");
	m_InvalidTitle = _T("Title is not defined in the Venue description: ");
	m_ValidValues = _T("Valid values: ");

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + _T("'y', 'n'");

	m_CalendarTentative = _T("Information is tentative.");
	m_CalendarNotEntered = _T("Not entered");
	m_CalendarEntered = _T("Entered");
	m_CalendarPlanning = _T("Planning");
	m_CalendarStatusN = _T("Status: ") + m_CalendarNotEntered;
	m_CalendarStatusE = _T("Status: ") + m_CalendarEntered;
	m_CalendarStatusP = _T("Status: ") + m_CalendarPlanning;
	m_CalendarOpens = _T("Trial opens: ");
	m_CalendarDraw = _T("Trial draws: ");
	m_CalendarCloses = _T("Trial closes: ");
	m_ScoreStyleUnknown = _T("Unknown");
	m_ScoreStyleFaultsTime = _T("Faults Then Time");
	m_ScoreStyleFaults100Time = _T("100 Minus Faults Then Time");
	m_ScoreStyleFaults200Time = _T("200 Minus Faults Then Time");
	m_ScoreStyleOCScoreTime = _T("Opening/Closing Points Then Time");
	m_ScoreStyleScoreTime = _T("Points Then Time");
	m_ScoreStyleTimePlusFaults = _T("Time Plus Faults");
	m_ExistingPointsOther = _T("OtherPoints");
	m_ExistingPointsRun = _T("Run");
	m_ExistingPointsSpeed = _T("Speed");
	m_ExistingPointsMQ = _T("Multiple Q");
	m_ExistingPointsSQ = _T("SuperQ");
}


tstring CLocalization::UpdateCalSites(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
    buffer << _T("Calendar Sites: ");
	buffer << nNew << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Faults: ");
	buffer << nNew << _T(" added, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("OtherPoints: ");
	buffer << nNew << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Venues: ");
	buffer << nNew << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Divisions: ");
	buffer << nAdded << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateDivisionsReordered() const
{
	otstringstream buffer;
	buffer << _T("Divisions: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Events: ");
	buffer << nAdded << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateEventsReordered() const
{
	otstringstream buffer;
	buffer << _T("Events: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("MultiQs: ");
	buffer << nAdded << _T(" added, ");
	buffer << nDeleted << _T(" deleted, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateMultiqsReordered() const
{
	otstringstream buffer;
	buffer << _T("MultiQs: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Levels: ");
	buffer << nAdded << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateLevelsReordered() const
{
	otstringstream buffer;
	buffer << _T("Levels: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T("Titles: ");
	buffer << nAdded << _T(" added, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::UpdateTitlesReordered() const
{
	otstringstream buffer;
	buffer << _T("Titles: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateSubLevels(int nAdded) const
{
	otstringstream buffer;
	buffer << nAdded << _T(" new sub-levels");
	return buffer.str();
}


tstring CLocalization::UpdateSubLevelsReordered() const
{
	otstringstream buffer;
	buffer << _T("Levels: ");
	buffer << _T("Reordered");
	return buffer.str();
}


tstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	buffer << _T(" rules: ");
	buffer << nAdded << _T(" added, ");
	buffer << nDeleted << _T(" deleted, ");
	buffer << nUpdated << _T(" updated, ");
	buffer << nSkipped << _T(" identical");
	return buffer.str();
}


tstring CLocalization::ActionDeleteCalPlugin(tstring const& name) const
{
	otstringstream buffer;
    buffer << _T("Action: Deleting Plugin [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameOtherPoints(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << _T("Action: Renaming OtherPoints [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T(" OtherPoints");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteOtherPoints(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
    buffer << _T("DELETING: OtherPoints [") << name << _T("] ") << nChanges << _T(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteOtherPoints(tstring const& name) const
{
	otstringstream buffer;
    buffer << _T("Action: Deleting OtherPoints [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameVenue(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << _T("Action: Renaming Venue [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
    	buffer << _T(", updating ") << nChanges << _T(" related items");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteVenue(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
    buffer << _T("DELETING: Venue [") << name << _T("] ") << nChanges << _T(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteVenue(tstring const& name) const
{
	otstringstream buffer;
    buffer << _T("Action: Deleting Venue [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameMultiQ(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
    buffer << _T("Action: Renaming ") << venue << _T(" MultiQ [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T(" MultiQs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteMultiQ(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("DELETING: ") << venue << _T(" MultiQ [") << name << _T("] ") << nChanges << _T(" items");
	return buffer.str();
}


tstring CLocalization::ActionDeleteMultiQ(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << _T("Action: Deleting ") << venue << _T(" MultiQ [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameDivision(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("Action: Renaming ") << venue << _T(" Division [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T(" runs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteDivision(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("DELETING: ") << venue << _T(" Division [") << name << _T("] ") << nChanges << _T(" runs and existing points");
	return buffer.str();
}


tstring CLocalization::ActionDeleteDivision(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << _T("Action: Deleting ") << venue << _T(" Division [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameLevel(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("Action: Renaming ") << venue << _T(" Level [") << oldName  << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T("!d! runs");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteLevel(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("DELETING: ") << venue << _T(" Level [") << name << _T("] ") << nChanges << _T(" runs");
	return buffer.str();
}


tstring CLocalization::ActionDeleteLevel(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << _T("Action: Deleting ") << venue << _T(" Level [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameTitle(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("Action: Renaming ") << venue << _T(" Title [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T("!d! titles");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteTitle(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("DELETING: ") << venue << _T(" Title []") << name << _T(" ") << nChanges << _T(" titles");
	return buffer.str();
}


tstring CLocalization::ActionDeleteTitle(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << _T("Action: Deleting ") << venue << _T(" Title [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::ActionRenameEvent(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("Action: Renaming ") << venue << _T(" Event [") << oldName << _T("] to [") << newName << _T("]");
	if (0 < nChanges)
	{
		buffer << _T(", updating ") << nChanges << _T(" events");
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteEvent(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	buffer << _T("DELETING: ") << venue << _T(" Event [") << name << _T("] ") << nChanges << _T(" events");
	return buffer.str();
}


tstring CLocalization::ActionDeleteEvent(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	buffer << _T("Action: Deleting ") << venue << _T(" Event [") << name << _T("]");
	return buffer.str();
}


tstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	otstringstream buffer;
	buffer << points << _T(" points with ") << faults << _T(" faults");
	return buffer.str();
}


tstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	otstringstream buffer;
	buffer << points << _T(" lifetime points with ") << faults << _T(" faults");
	return buffer.str();
}


tstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	otstringstream buffer;
	buffer << points << _T(" points with place of ") << place;
	return buffer.str();
}


tstring CLocalization::WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
    buffer << _T("WARNING: ") << nRuns << _T(" run(s) deleted due to configuration changes.\n(Date, Venue, Event, Division/Level)\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTeamRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
	buffer << _T("USDAA 'Pairs' converted to 'Team' in ") << nRuns << _T(" runs.") << _T("\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTableRuns(int nRuns) const
{
	otstringstream buffer;
	buffer << _T("Table setting updated in ") << nRuns << _T(" runs.");
	return buffer.str();
}


tstring CLocalization::ErrorInvalidDocStructure(TCHAR const* const inMsg) const
{
	otstringstream str;
	str << m_InvalidDocStructure;
	if (inMsg)
		str << ": " << inMsg << std::endl;
	return str.str();
}


tstring CLocalization::ErrorMissingAttribute(
		TCHAR const* const inElement,
		TCHAR const* const inAttrib,
		TCHAR const* const inMsg) const
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
		TCHAR const* const inElement,
		TCHAR const* const inAttrib,
		TCHAR const* const inMsg) const
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
