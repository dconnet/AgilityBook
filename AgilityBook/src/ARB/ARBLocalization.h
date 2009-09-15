#pragma once

/*
 * Copyright (c) 2007-2009 David Connet. All Rights Reserved.
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
 * @brief Localization
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-09-13 DRC Move to wxStrings throughout.
 * @li 2007-09-12 DRC Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

/*
 * Localization stuff
 *  There is some stuff in:
 *    Element.cpp (errors due to XML parsing failures)
 */

#include "ARBTypes.h"

/**
 * This class must be implemented by the user to define some strings ARB
 * requires. This allows ARB to remain platform-independent.
 */
class IARBLocalization
{
public:
	static void Init(IARBLocalization const* pLocalization);

	// Used when loading data.
	virtual wxString UnknownVersion() const = 0;
	virtual wxString WarningNewerDoc() const = 0;
	virtual wxString InvalidDocStructure() const = 0;
	virtual wxString InvalidFileFormat() const = 0;
	virtual wxString InvalidFileMissingAttrib() const = 0;
	virtual wxString InvalidFileBadAttrib() const = 0;
	virtual wxString InvalidDate() const = 0;
	virtual wxString InvalidDivLevel() const = 0;
	virtual wxString InvalidEventName() const = 0;
	virtual wxString InvalidRoot() const = 0;
	virtual wxString MissingConfig() const = 0;
	virtual wxString InvalidConfig() const = 0;
	virtual wxString InvalidVenueConfig() const = 0;
	virtual wxString InvalidDivName() const = 0;
	virtual wxString InvalidVenueName() const = 0;
	virtual wxString InvalidOtherPtsName() const = 0;
	virtual wxString InvalidMultiqName() const = 0;
	virtual wxString InvalidMultiqConversion() const = 0;
	virtual wxString InvalidEvent() const = 0;
	virtual wxString InvalidTitle() const = 0;
	virtual wxString ValidValues() const = 0;
	virtual wxString ValidValuesBool() const = 0;
	virtual wxString CalendarTentative() const = 0;
	virtual wxString CalendarNotEntered() const = 0;
	virtual wxString CalendarEntered() const	 = 0;
	virtual wxString CalendarPlanning() const = 0;
	virtual wxString CalendarStatusN() const = 0;
	virtual wxString CalendarStatusE() const = 0;
	virtual wxString CalendarStatusP() const = 0;
	virtual wxString CalendarOpens() const = 0;
	virtual wxString CalendarDraw() const = 0;
	virtual wxString CalendarCloses() const = 0;

	// Strings for formatting the information returned when updating configurations.
	virtual wxString UpdateCalSites(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateFaults(int nNew, int nSkipped) const = 0;
	virtual wxString UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateVenues(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateDivisionsReordered() const = 0;
	virtual wxString UpdateEvents(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateEventsReordered() const = 0;
	virtual wxString UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const = 0;
	virtual wxString UpdateMultiqsReordered() const = 0;
	virtual wxString UpdateLevels(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateLevelsReordered() const = 0;
	virtual wxString UpdateTitles(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateTitlesReordered() const = 0;
	virtual wxString UpdateSubLevels(int nAdded) const = 0;
	virtual wxString UpdateSubLevelsReordered() const = 0;
	virtual wxString UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const = 0;
	virtual wxString WarnDeletedRuns(int nRuns, wxString const& inRunsMsg) const = 0;
	virtual wxString UpdateTeamRuns(int nRuns, wxString const& inRunsMsg) const = 0;
	virtual wxString UpdateTableRuns(int nRuns) const = 0;

	// Used in ARBConfigAction.cpp
	virtual wxString ActionDeleteCalPlugin(wxString const& name) const = 0;
	virtual wxString ActionRenameOtherPoints(
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteOtherPoints(
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteOtherPoints(wxString const& name) const = 0;
	virtual wxString ActionRenameVenue(
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteVenue(
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteVenue(wxString const& name) const = 0;
	virtual wxString ActionRenameMultiQ(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteMultiQ(
			wxString const& venue,
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteMultiQ(
			wxString const& venue,
			wxString const& name) const = 0;
	virtual wxString ActionRenameDivision(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteDivision(
			wxString const& venue,
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteDivision(
			wxString const& venue,
			wxString const& name) const = 0;
	virtual wxString ActionRenameLevel(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteLevel(
			wxString const& venue,
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteLevel(
			wxString const& venue,
			wxString const& name) const = 0;
	virtual wxString ActionRenameTitle(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteTitle(
			wxString const& venue,
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteTitle(
			wxString const& venue,
			wxString const& name) const = 0;
	virtual wxString ActionRenameEvent(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const = 0;
	virtual wxString ActionPreDeleteEvent(
			wxString const& venue,
			wxString const& name,
			int nChanges) const = 0;
	virtual wxString ActionDeleteEvent(
			wxString const& venue,
			wxString const& name) const = 0;

	// Used in ARBConfigTitlePoints.cpp
	virtual wxString TitlePointsNameFormat(double points, double faults) const = 0;
	virtual wxString LifetimePointsNameFormat(double points, double faults) const = 0;
	virtual wxString PlacementPointsNameFormat(double points, short place) const = 0;

	// Used in ARBConfigScoring.cpp
	virtual wxString ScoreStyleUnknown() const = 0;
	virtual wxString ScoreStyleFaultsTime() const = 0;
	virtual wxString ScoreStyleFaults100Time() const = 0;
	virtual wxString ScoreStyleFaults200Time() const = 0;
	virtual wxString ScoreStyleOCScoreTime() const = 0;
	virtual wxString ScoreStyleScoreTime() const = 0;
	virtual wxString ScoreStyleTimePlusFaults() const = 0;

	// Used in ARBDogExistingPoints.cpp
	virtual wxString ExistingPointsOther() const = 0;
	virtual wxString ExistingPointsRun() const = 0;
	virtual wxString ExistingPointsSpeed() const = 0;
	virtual wxString ExistingPointsMQ() const = 0;
	virtual wxString ExistingPointsSQ() const = 0;

	/**
	 * Return an error message about invalid document structure.
	 *
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual wxString ErrorInvalidDocStructure(wxChar const* const inMsg) const = 0;

	/**
	 * Return an error message about a missing required attribute.
	 *
	 * @param inElement Element containing missing attribute.
	 * @param inAttrib Attribute name that is missing.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual wxString ErrorMissingAttribute(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const = 0;

	/**
	 * Return an error message about an invalid value in an attribute.
	 *
	 * @param inElement Element containing bad attribute.
	 * @param inAttrib Attribute name whose value is bad.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual wxString ErrorInvalidAttributeValue(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const = 0;
};


extern IARBLocalization const* Localization();
