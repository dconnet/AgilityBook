#pragma once

/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
	virtual tstring UnknownVersion() const = 0;
	virtual tstring WarningNewerDoc() const = 0;
	virtual tstring InvalidDocStructure() const = 0;
	virtual tstring InvalidFileFormat() const = 0;
	virtual tstring InvalidFileMissingAttrib() const = 0;
	virtual tstring InvalidFileBadAttrib() const = 0;
	virtual tstring InvalidDate() const = 0;
	virtual tstring InvalidDivLevel() const = 0;
	virtual tstring InvalidEventName() const = 0;
	virtual tstring InvalidRoot() const = 0;
	virtual tstring MissingConfig() const = 0;
	virtual tstring InvalidConfig() const = 0;
	virtual tstring InvalidVenueConfig() const = 0;
	virtual tstring InvalidDivName() const = 0;
	virtual tstring InvalidVenueName() const = 0;
	virtual tstring InvalidOtherPtsName() const = 0;
	virtual tstring InvalidMultiqName() const = 0;
	virtual tstring InvalidMultiqConversion() const = 0;
	virtual tstring InvalidEvent() const = 0;
	virtual tstring InvalidTitle() const = 0;
	virtual tstring ValidValues() const = 0;
	virtual tstring ValidValuesBool() const = 0;
	virtual tstring CalendarTentative() const = 0;
	virtual tstring CalendarNotEntered() const = 0;
	virtual tstring CalendarEntered() const	 = 0;
	virtual tstring CalendarPlanning() const = 0;
	virtual tstring CalendarStatusN() const = 0;
	virtual tstring CalendarStatusE() const = 0;
	virtual tstring CalendarStatusP() const = 0;
	virtual tstring CalendarOpens() const = 0;
	virtual tstring CalendarDraw() const = 0;
	virtual tstring CalendarCloses() const = 0;

	// Strings for formatting the information returned when updating configurations.
	virtual tstring UpdateCalSites(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateFaults(int nNew, int nSkipped) const = 0;
	virtual tstring UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateVenues(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateDivisionsReordered() const = 0;
	virtual tstring UpdateEvents(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateEventsReordered() const = 0;
	virtual tstring UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const = 0;
	virtual tstring UpdateMultiqsReordered() const = 0;
	virtual tstring UpdateLevels(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateTitles(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual tstring UpdateTitlesReordered() const = 0;
	virtual tstring UpdateSubLevels(int nAdded) const = 0;
	virtual tstring UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const = 0;
	virtual tstring WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const = 0;
	virtual tstring UpdateTableRuns(int nRuns) const = 0;

	// Used in ARBConfigAction.cpp
	virtual tstring ActionDeleteCalPlugin(tstring const& name) const = 0;
	virtual tstring ActionRenameOtherPoints(
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteOtherPoints(
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteOtherPoints(tstring const& name) const = 0;
	virtual tstring ActionRenameVenue(
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteVenue(
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteVenue(tstring const& name) const = 0;
	virtual tstring ActionRenameMultiQ(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteMultiQ(
			tstring const& venue,
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteMultiQ(
			tstring const& venue,
			tstring const& name) const = 0;
	virtual tstring ActionRenameDivision(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteDivision(
			tstring const& venue,
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteDivision(
			tstring const& venue,
			tstring const& name) const = 0;
	virtual tstring ActionRenameLevel(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteLevel(
			tstring const& venue,
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteLevel(
			tstring const& venue,
			tstring const& name) const = 0;
	virtual tstring ActionRenameTitle(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteTitle(
			tstring const& venue,
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteTitle(
			tstring const& venue,
			tstring const& name) const = 0;
	virtual tstring ActionRenameEvent(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const = 0;
	virtual tstring ActionPreDeleteEvent(
			tstring const& venue,
			tstring const& name,
			int nChanges) const = 0;
	virtual tstring ActionDeleteEvent(
			tstring const& venue,
			tstring const& name) const = 0;

	// Used in ARBConfigTitlePoints.cpp
	virtual tstring TitlePointsNameFormat(double points, double faults) const = 0;
	virtual tstring LifetimePointsNameFormat(double points, double faults) const = 0;
	virtual tstring PlacementPointsNameFormat(double points, short place) const = 0;

	// Used in ARBConfigScoring.cpp
	virtual tstring ScoreStyleUnknown() const = 0;
	virtual tstring ScoreStyleFaultsTime() const = 0;
	virtual tstring ScoreStyleFaults100Time() const = 0;
	virtual tstring ScoreStyleFaults200Time() const = 0;
	virtual tstring ScoreStyleOCScoreTime() const = 0;
	virtual tstring ScoreStyleScoreTime() const = 0;
	virtual tstring ScoreStyleTimePlusFaults() const = 0;

	// Used in ARBDogExistingPoints.cpp
	virtual tstring ExistingPointsOther() const = 0;
	virtual tstring ExistingPointsRun() const = 0;
	virtual tstring ExistingPointsSpeed() const = 0;
	virtual tstring ExistingPointsMQ() const = 0;
	virtual tstring ExistingPointsSQ() const = 0;

	/**
	 * Return an error message about invalid document structure.
	 *
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual tstring ErrorInvalidDocStructure(TCHAR const* const inMsg) const = 0;

	/**
	 * Return an error message about a missing required attribute.
	 *
	 * @param inElement Element containing missing attribute.
	 * @param inAttrib Attribute name that is missing.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual tstring ErrorMissingAttribute(
			TCHAR const* const inElement,
			TCHAR const* const inAttrib,
			TCHAR const* const inMsg = NULL) const = 0;

	/**
	 * Return an error message about an invalid value in an attribute.
	 *
	 * @param inElement Element containing bad attribute.
	 * @param inAttrib Attribute name whose value is bad.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual tstring ErrorInvalidAttributeValue(
			TCHAR const* const inElement,
			TCHAR const* const inAttrib,
			TCHAR const* const inMsg = NULL) const = 0;
};

// Used in ARBTypes.cpp
#define ARBQ_TYPE_NA	_T("NA")
#define ARBQ_TYPE_Q		_T("Q")
#define ARBQ_TYPE_NQ	_T("NQ")
#define ARBQ_TYPE_E		_T("E")
#define ARBQ_TYPE_SQ	_T("SQ")

extern IARBLocalization const* Localization();
