#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
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

#include "ARBTypes2.h"


/**
 * This class must be implemented by the user to define some strings ARB
 * requires. This allows ARB to remain platform-independent.
 */
class IARBLocalization
{
public:
	static void Init(IARBLocalization const* pLocalization);

	// Used when loading data.
	virtual std::wstring UnknownVersion() const = 0;
	virtual std::wstring WarningNewerDoc() const = 0;
	virtual std::wstring InvalidDocStructure() const = 0;
	virtual std::wstring InvalidFileFormat() const = 0;
	virtual std::wstring InvalidFileMissingAttrib() const = 0;
	virtual std::wstring InvalidFileBadAttrib() const = 0;
	virtual std::wstring InvalidDate() const = 0;
	virtual std::wstring InvalidDivLevel() const = 0;
	virtual std::wstring InvalidEventName() const = 0;
	virtual std::wstring InvalidRoot() const = 0;
	virtual std::wstring MissingConfig() const = 0;
	virtual std::wstring InvalidConfig() const = 0;
	virtual std::wstring InvalidVenueConfig() const = 0;
	virtual std::wstring InvalidDivName() const = 0;
	virtual std::wstring InvalidVenueName() const = 0;
	virtual std::wstring InvalidOtherPtsName() const = 0;
	virtual std::wstring InvalidMultiqName() const = 0;
	virtual std::wstring InvalidMultiqConversion() const = 0;
	virtual std::wstring InvalidEvent() const = 0;
	virtual std::wstring InvalidTitle() const = 0;
	virtual std::wstring ValidValues() const = 0;
	virtual std::wstring ValidValuesBool() const = 0;
	virtual std::wstring CalendarTentative() const = 0;
	virtual std::wstring CalendarPast() const = 0;
	virtual std::wstring CalendarNotEntered() const = 0;
	virtual std::wstring CalendarEntered() const	 = 0;
	virtual std::wstring CalendarPending() const = 0;
	virtual std::wstring CalendarPlanning() const = 0;
	virtual std::wstring CalendarStatusN() const = 0;
	virtual std::wstring CalendarStatusE() const = 0;
	virtual std::wstring CalendarStatusO() const = 0;
	virtual std::wstring CalendarStatusP() const = 0;
	virtual std::wstring CalendarOpens() const = 0;
	virtual std::wstring CalendarDraw() const = 0;
	virtual std::wstring CalendarCloses() const = 0;

	// Strings for formatting the information returned when updating configurations.
	virtual std::wstring UpdateCalSites(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateFaults(int nNew, int nSkipped) const = 0;
	virtual std::wstring UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateVenues(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateDivisionsReordered() const = 0;
	virtual std::wstring UpdateEvents(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateEventsReordered() const = 0;
	virtual std::wstring UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const = 0;
	virtual std::wstring UpdateMultiqsReordered() const = 0;
	virtual std::wstring UpdateLevels(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateLevelsReordered() const = 0;
	virtual std::wstring UpdateTitles(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring UpdateTitlesReordered() const = 0;
	virtual std::wstring UpdateSubLevels(int nAdded) const = 0;
	virtual std::wstring UpdateSubLevelsReordered() const = 0;
	virtual std::wstring UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const = 0;
	virtual std::wstring WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const = 0;
	virtual std::wstring UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const = 0;
	virtual std::wstring UpdateTableRuns(int nRuns) const = 0;

	// Used in ARBConfigAction.cpp
	virtual std::wstring ActionDeleteCalPlugin(std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameOtherPoints(
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteOtherPoints(
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteOtherPoints(std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameVenue(
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteVenue(
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteVenue(std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameMultiQ(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteMultiQ(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteMultiQ(
			std::wstring const& venue,
			std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameDivision(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteDivision(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteDivision(
			std::wstring const& venue,
			std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameLevel(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteLevel(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteLevel(
			std::wstring const& venue,
			std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameTitle(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteTitle(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteTitle(
			std::wstring const& venue,
			std::wstring const& name) const = 0;
	virtual std::wstring ActionRenameEvent(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const = 0;
	virtual std::wstring ActionPreDeleteEvent(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const = 0;
	virtual std::wstring ActionDeleteEvent(
			std::wstring const& venue,
			std::wstring const& name) const = 0;

	// Used in ARBConfigTitlePoints.cpp
	virtual std::wstring TitlePointsNameFormat(double points, double faults) const = 0;
	virtual std::wstring LifetimePointsNameFormat(double points, double faults) const = 0;
	virtual std::wstring PlacementPointsNameFormat(double points, short place) const = 0;

	// Used in ARBConfigScoring.cpp
	virtual std::wstring ScoreStyleUnknown() const = 0;
	virtual std::wstring ScoreStyleFaultsTime() const = 0;
	virtual std::wstring ScoreStyleFaults100Time() const = 0;
	virtual std::wstring ScoreStyleFaults200Time() const = 0;
	virtual std::wstring ScoreStyleOCScoreTime() const = 0;
	virtual std::wstring ScoreStyleScoreTime() const = 0;
	virtual std::wstring ScoreStyleTimePlusFaults() const = 0;

	// Used in ARBDogExistingPoints.cpp
	virtual std::wstring ExistingPointsOther() const = 0;
	virtual std::wstring ExistingPointsRun() const = 0;
	virtual std::wstring ExistingPointsSpeed() const = 0;
	virtual std::wstring ExistingPointsMQ() const = 0;
	virtual std::wstring ExistingPointsSQ() const = 0;

	/**
	 * Return an error message about invalid document structure.
	 *
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual std::wstring ErrorInvalidDocStructure(wchar_t const* const inMsg) const = 0;
	virtual std::wstring ErrorInvalidDocStructure(std::wstring const& inMsg) const
	{
		return ErrorInvalidDocStructure(inMsg.c_str());
	}

	/**
	 * Return an error message about a missing required attribute.
	 *
	 * @param inElement Element containing missing attribute.
	 * @param inAttrib Attribute name that is missing.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual std::wstring ErrorMissingAttribute(
			wchar_t const* const inElement,
			wchar_t const* const inAttrib,
			wchar_t const* const inMsg = NULL) const = 0;

	/**
	 * Return an error message about an invalid value in an attribute.
	 *
	 * @param inElement Element containing bad attribute.
	 * @param inAttrib Attribute name whose value is bad.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual std::wstring ErrorInvalidAttributeValue(
			wchar_t const* const inElement,
			wchar_t const* const inAttrib,
			wchar_t const* const inMsg = NULL) const = 0;
};


extern IARBLocalization const* Localization();
