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
 * 2016-06-19 Add support for Lifetime names.
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2016-01-06 Add support for named lifetime points.
 * 2013-09-03 Changed sub level updating.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-09-13 Move to wxStrings throughout.
 * 2007-09-12 Moved hardcoded strings out of ARBAgilityRecordBook.h
 */

/*
 * Localization stuff
 *  There is some stuff in:
 *    Element.cpp (errors due to XML parsing failures)
 */

#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * This class must be implemented by the user to define some strings ARB
 * requires. This allows ARB to remain platform-independent.
 */
class ARB_API IARBLocalization
{
	DECLARE_NO_COPY_IMPLEMENTED(IARBLocalization)
public:
	static void Init(IARBLocalization const* pLocalization);
	IARBLocalization()
	{
	}
	virtual ~IARBLocalization()
	{
	}

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
	virtual wxString InvalidLifetimeName() const = 0;
	virtual wxString InvalidOtherPtsName() const = 0;
	virtual wxString InvalidMultiqName() const = 0;
	virtual wxString InvalidMultiqConversion() const = 0;
	virtual wxString InvalidEvent() const = 0;
	virtual wxString InvalidTitle() const = 0;
	virtual wxString ValidValues() const = 0;
	virtual wxString ValidValuesBool() const = 0;
	virtual wxString CalendarTentative() const = 0;
	virtual wxString CalendarPast() const = 0;
	virtual wxString CalendarNotEntered() const = 0;
	virtual wxString CalendarEntered() const = 0;
	virtual wxString CalendarPending() const = 0;
	virtual wxString CalendarPlanning() const = 0;
	virtual wxString CalendarStatusN() const = 0;
	virtual wxString CalendarStatusE() const = 0;
	virtual wxString CalendarStatusO() const = 0;
	virtual wxString CalendarStatusP() const = 0;
	virtual wxString CalendarOpens() const = 0;
	virtual wxString CalendarDraw() const = 0;
	virtual wxString CalendarCloses() const = 0;

	// Strings for formatting the information returned when updating configurations.
	virtual wxString UpdateFaults(int nNew, int nSkipped) const = 0;
	virtual wxString UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateVenues(int nNew, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateLifetimeNames(int nNew, int nSkipped) const = 0;
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
	virtual wxString UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const = 0;
	virtual wxString UpdateSubLevelsReordered() const = 0;
	virtual wxString UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const = 0;
	virtual wxString WarnDeletedRuns(int nRuns, wxString const& inRunsMsg) const = 0;
	virtual wxString UpdateTeamRuns(int nRuns, wxString const& inRunsMsg) const = 0;
	virtual wxString UpdateTableRuns(int nRuns) const = 0;
	virtual wxString UpdateTableRuns(int nRuns, wxString const& inRunsMsg) const = 0;
	virtual wxString UpdateSubnameRuns(int nRuns, wxString const& inRunsMsg) const = 0;

	// Used in ARBConfigAction.cpp
	virtual wxString ActionRenameOtherPoints(wxString const& oldName, wxString const& newName, int nChanges) const = 0;
	virtual wxString ActionPreDeleteOtherPoints(wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteOtherPoints(wxString const& name) const = 0;
	virtual wxString ActionRenameVenue(wxString const& oldName, wxString const& newName, int nChanges) const = 0;
	virtual wxString ActionPreDeleteVenue(wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteVenue(wxString const& name) const = 0;
	virtual wxString ActionRenameMultiQ(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nChanges) const
		= 0;
	virtual wxString ActionPreDeleteMultiQ(wxString const& venue, wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteMultiQ(wxString const& venue, wxString const& name) const = 0;
	virtual wxString ActionRenameDivision(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nChanges) const
		= 0;
	virtual wxString ActionPreDeleteDivision(wxString const& venue, wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteDivision(wxString const& venue, wxString const& name) const = 0;
	virtual wxString ActionRenameLevel(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nChanges) const
		= 0;
	virtual wxString ActionPreDeleteLevel(wxString const& venue, wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteLevel(wxString const& venue, wxString const& name) const = 0;
	virtual wxString ActionRenameTitle(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nChanges) const
		= 0;
	virtual wxString ActionPreDeleteTitle(wxString const& venue, wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteTitle(wxString const& venue, wxString const& name) const = 0;
	virtual wxString ActionRenameEvent(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nChanges) const
		= 0;
	virtual wxString ActionPreDeleteEvent(wxString const& venue, wxString const& name, int nChanges) const = 0;
	virtual wxString ActionDeleteEvent(wxString const& venue, wxString const& name) const = 0;
	virtual wxString ActionRenameLifetimeName(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nEventChanges) const
		= 0;
	virtual wxString ActionDeleteLifetimeName(wxString const& venue, wxString const& name, int nEventChanges) const = 0;

	// Used in ARBConfigTitlePoints.cpp
	virtual wxString TitlePointsNameFormat(double points, double faults) const = 0;
	virtual wxString LifetimePointsNameWithSpeedPointsFormat(double faults) const = 0;
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
	virtual wxString ScoreStyleTimeNoPlaces() const = 0;
	virtual wxString ScoreStylePassFail() const = 0;

	// Used in ARBDogExistingPoints.cpp
	virtual wxString ExistingPointsOther() const = 0;
	virtual wxString ExistingPointsLifetime() const = 0;
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
	virtual wxString ErrorInvalidDocStructure(wchar_t const* const inMsg) const = 0;
	virtual wxString ErrorInvalidDocStructure(wxString const& inMsg) const
	{
		return ErrorInvalidDocStructure(inMsg);
	}

	/**
	 * Return an error message about a missing required attribute.
	 *
	 * @param inElement Element containing missing attribute.
	 * @param inAttrib Attribute name that is missing.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual wxString ErrorMissingAttribute(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg = nullptr) const
		= 0;

	/**
	 * Return an error message about an invalid value in an attribute.
	 *
	 * @param inElement Element containing bad attribute.
	 * @param inAttrib Attribute name whose value is bad.
	 * @param inMsg Additional error information.
	 * @return Message with newline.
	 */
	virtual wxString ErrorInvalidAttributeValue(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg = nullptr) const
		= 0;
};


ARB_API IARBLocalization const* Localization();

} // namespace ARB
} // namespace dconSoft
