/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * Revision History
 * @li 2007-10-11 DRC Created
 */

#include "stdafx.h"
#include "Localization.h"

#include "resource.h"
#include "ARBStructure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

CLocalization::CLocalization()
{
}


void CLocalization::Load()
{
	CString data;
	data.LoadString(IDS_ARB_UNKNOWN_VERSION);
	m_UnknownVersion = (LPCTSTR)data;
	data.LoadString(IDS_WARNING_NEWER_DOC);
	m_WarningNewerDoc = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_DOC_STRUCTURE);
	m_InvalidDocStructure = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_FILE_FORMAT);
	m_InvalidFileFormat = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_FILE_MISSING_ATTRIB);
	m_InvalidFileMissingAttrib = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_FILE_BAD_ATTRIB);
	m_InvalidFileBadAttrib = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_DATE);
	m_InvalidDate = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_DIV_LEVEL);
	m_InvalidDivLevel = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_EVENT_NAME);
	m_InvalidEventName = (LPCTSTR)data;
	data.FormatMessage(IDS_INVALID_ROOT, TREE_BOOK);
	m_InvalidRoot = (LPCTSTR)data;
	data.FormatMessage(IDS_MISSING_CONFIG, TREE_CONFIG);
	m_MissingConfig = (LPCTSTR)data;
	data.FormatMessage(IDS_INVALID_CONFIG, TREE_CONFIG);
	m_InvalidConfig = (LPCTSTR)data;
	data.FormatMessage(IDS_INVALID_VENUE_CONFIG, TREE_DIVISION, TREE_EVENT);
	m_InvalidVenueConfig = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_DIV_NAME);
	m_InvalidDivName = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_VENUE_NAME);
	m_InvalidVenueName = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_OTHER_PTS_NAME);
	m_InvalidOtherPtsName = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_MULTIQ_NAME);
	m_InvalidMultiqName = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_MULTIQ_CONVERSION);
	m_InvalidMultiqConversion = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_EVENT);
	m_InvalidEvent = (LPCTSTR)data;
	data.LoadString(IDS_INVALID_TITLE);
	m_InvalidTitle = (LPCTSTR)data;
	data.LoadString(IDS_VALID_VALUES);
	m_ValidValues = (LPCTSTR)data;

	// These values are fixed in Element.cpp
	m_ValidValuesBool = m_ValidValues + _T("'y', 'n'");

	data.LoadString(IDS_CALENDAR_TENTATIVE);
	m_CalendarTentative = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_NOTENTERED);
	m_CalendarNotEntered = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_ENTERED);
	m_CalendarEntered = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_PLANNING);
	m_CalendarPlanning = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_STATUS);
	m_CalendarStatusN = (LPCTSTR)data + m_CalendarNotEntered;
	m_CalendarStatusE = (LPCTSTR)data + m_CalendarEntered;
	m_CalendarStatusP = (LPCTSTR)data + m_CalendarPlanning;
	data.LoadString(IDS_CALENDAR_OPENS);
	m_CalendarOpens = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_DRAW);
	m_CalendarDraw = (LPCTSTR)data;
	data.LoadString(IDS_CALENDAR_CLOSES);
	m_CalendarCloses = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_UNKNOWN);
	m_ScoreStyleUnknown = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_FAULTS_TIME);
	m_ScoreStyleFaultsTime = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_FAULTS100_TIME);
	m_ScoreStyleFaults100Time = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_FAULTS200_TIME);
	m_ScoreStyleFaults200Time = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_OCSCORE_TIME);
	m_ScoreStyleOCScoreTime = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_SCORE_TIME);
	m_ScoreStyleScoreTime = (LPCTSTR)data;
	data.LoadString(IDS_SCORE_STYLE_TIME_PLUS_FAULTS);
	m_ScoreStyleTimePlusFaults = (LPCTSTR)data;
	data.LoadString(IDS_EXISTING_POINTS_OTHER);
	m_ExistingPointsOther = (LPCTSTR)data;
	data.LoadString(IDS_EXISTING_POINTS_RUN);
	m_ExistingPointsRun = (LPCTSTR)data;
	data.LoadString(IDS_EXISTING_POINTS_SPEED);
	m_ExistingPointsSpeed = (LPCTSTR)data;
	data.LoadString(IDS_EXISTING_POINTS_MQ);
	m_ExistingPointsMQ = (LPCTSTR)data;
	data.LoadString(IDS_EXISTING_POINTS_SQ);
	m_ExistingPointsSQ = (LPCTSTR)data;
}


tstring CLocalization::UpdateCalSites(int nNew, int nDeleted, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_UPDATE_CALSITES);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nNew);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_DELETED, nDeleted);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data << _T(", ");
	return buffer.str();
}


tstring CLocalization::UpdateFaults(int nNew, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_FAULTS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nNew);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_OTHERPOINTS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nNew);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateVenues(int nNew, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_VENUES);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nNew);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_DIVISIONS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateDivisionsReordered() const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_DIVISIONS);
	buffer << (LPCTSTR)data << _T(": ");
	data.LoadString(IDS_REORDERED);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateEvents(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_EVENTS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateEventsReordered() const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_EVENTS);
	buffer << (LPCTSTR)data << _T(": ");
	data.LoadString(IDS_REORDERED);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_MULTIQS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_DELETED, nDeleted);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateMultiqsReordered() const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_MULTIQS);
	buffer << (LPCTSTR)data << _T(": ");
	data.LoadString(IDS_REORDERED);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateLevels(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_COL_LEVELS);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateTitles(int nAdded, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_TITLES);
	buffer << (LPCTSTR)data << _T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateTitlesReordered() const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_TITLES);
	buffer << (LPCTSTR)data << _T(": ");
	data.LoadString(IDS_REORDERED);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateSubLevels(int nAdded) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_UPDATE_NEWSUBLEVELS, nAdded);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const
{
	otstringstream buffer;
	CString data;
	data.LoadString(IDS_UPDATE_RULES);
	buffer << _T(" ") << (LPCTSTR)data <<_T(": ");
	data.FormatMessage(IDS_UPDATE_ADDED, nAdded);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_DELETED, nDeleted);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_UPDATED, nUpdated);
	buffer << (LPCTSTR)data << _T(", ");
	data.FormatMessage(IDS_UPDATE_IDENTICAL, nSkipped);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameOtherPoints(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_OTHERPTS, oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_OTHERPTS2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteOtherPoints(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_OTHERPTS, name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteOtherPoints(tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_OTHERPTS, name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameVenue(
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_VENUE, oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_VENUE2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteVenue(
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_VENUE, name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteVenue(tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_VENUE, name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameMultiQ(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_MULTIQ, venue.c_str(), oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_MULTIQ2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteMultiQ(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_MULTIQ, venue.c_str(), name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteMultiQ(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_MULTIQ, venue.c_str(), name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameDivision(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_DIV, venue.c_str(), oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_DIV2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteDivision(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_DIV, venue.c_str(), name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteDivision(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_DIV, venue.c_str(), name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameLevel(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_LEVEL, venue.c_str(), oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_LEVEL2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteLevel(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_LEVEL, venue.c_str(), name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteLevel(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_LEVEL, venue.c_str(), name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameTitle(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_TITLE, venue.c_str(), oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_TITLE2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteTitle(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_TITLE, venue.c_str(), name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteTitle(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_TITLE, venue.c_str(), name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionRenameEvent(
		tstring const& venue,
		tstring const& oldName,
		tstring const& newName,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_RENAME_EVENT, venue.c_str(), oldName.c_str(), newName.c_str());
	buffer << (LPCTSTR)data;
	if (0 < nChanges)
	{
		data.FormatMessage(IDS_ACTION_RENAME_EVENT2, nChanges);
		buffer << (LPCTSTR)data;
	}
	return buffer.str();
}


tstring CLocalization::ActionPreDeleteEvent(
		tstring const& venue,
		tstring const& name,
		int nChanges) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_PREDELETE_EVENT, venue.c_str(), name.c_str(), nChanges);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::ActionDeleteEvent(
		tstring const& venue,
		tstring const& name) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_ACTION_DELETE_EVENT, venue.c_str(), name.c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::TitlePointsNameFormat(double points, double faults) const
{
	otstringstream buffer, dbl1, dbl2;
	dbl1 << points;
	dbl2 << faults;
	CString data;
	data.FormatMessage(IDS_TITLEPOINTSNAME, dbl1.str().c_str(), dbl2.str().c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::LifetimePointsNameFormat(double points, double faults) const
{
	otstringstream buffer, dbl1, dbl2;
	dbl1 << points;
	dbl2 << faults;
	CString data;
	data.FormatMessage(IDS_LIFETITLEPOINTSNAME, dbl1.str().c_str(), dbl2.str().c_str());
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::PlacementPointsNameFormat(double points, short place) const
{
	otstringstream buffer, dbl1;
	dbl1 << points;
	CString data;
	data.FormatMessage(IDS_PLACEMENTPOINTSNAME, dbl1.str().c_str(), (int)place);
	buffer << (LPCTSTR)data;
	return buffer.str();
}


tstring CLocalization::WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_WARN_DELETED_RUNS, nRuns);
	buffer << (LPCTSTR)data << _T("\n") << inRunsMsg;
	return buffer.str();
}


tstring CLocalization::UpdateTableRuns(int nRuns) const
{
	otstringstream buffer;
	CString data;
	data.FormatMessage(IDS_UPDATE_TABLE_RUNS, nRuns);
	buffer << (LPCTSTR)data;
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
