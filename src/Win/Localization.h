#pragma once

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
 * Revision History
 * 2016-06-19 Add support for Lifetime names.
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2016-01-06 Add support for named lifetime points.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-06 Ported to wxWidgets.
 * 2007-10-11 Created
 */

#include "ARB/ARBLocalization.h"


namespace dconSoft
{


class CLocalization : public ARB::IARBLocalization
{
public:
	CLocalization();
	bool Load();

	wxString UnknownVersion() const override
	{
		return m_UnknownVersion;
	}
	wxString WarningNewerDoc() const override
	{
		return m_WarningNewerDoc;
	}
	wxString InvalidDocStructure() const override
	{
		return m_InvalidDocStructure;
	}
	wxString InvalidFileFormat() const override
	{
		return m_InvalidFileFormat;
	}
	wxString InvalidFileMissingAttrib() const override
	{
		return m_InvalidFileMissingAttrib;
	}
	wxString InvalidFileBadAttrib() const override
	{
		return m_InvalidFileBadAttrib;
	}
	wxString InvalidDate() const override
	{
		return m_InvalidDate;
	}
	wxString InvalidDivLevel() const override
	{
		return m_InvalidDivLevel;
	}
	wxString InvalidEventName() const override
	{
		return m_InvalidEventName;
	}
	wxString InvalidRoot() const override
	{
		return m_InvalidRoot;
	}
	wxString MissingConfig() const override
	{
		return m_MissingConfig;
	}
	wxString InvalidConfig() const override
	{
		return m_InvalidConfig;
	}
	wxString InvalidVenueConfig() const override
	{
		return m_InvalidVenueConfig;
	}
	wxString InvalidDivName() const override
	{
		return m_InvalidDivName;
	}
	wxString InvalidVenueName() const override
	{
		return m_InvalidVenueName;
	}
	wxString InvalidLifetimeName() const override
	{
		return m_InvalidLifetimeName;
	}
	wxString InvalidOtherPtsName() const override
	{
		return m_InvalidOtherPtsName;
	}
	wxString InvalidMultiqName() const override
	{
		return m_InvalidMultiqName;
	}
	wxString InvalidMultiqConversion() const override
	{
		return m_InvalidMultiqConversion;
	}
	wxString InvalidEvent() const override
	{
		return m_InvalidEvent;
	}
	wxString InvalidTitle() const override
	{
		return m_InvalidTitle;
	}
	wxString ValidValues() const override
	{
		return m_ValidValues;
	}
	wxString ValidValuesBool() const override
	{
		return m_ValidValuesBool;
	}
	wxString CalendarTentative() const override
	{
		return m_CalendarTentative;
	}
	wxString CalendarPast() const override
	{
		return m_CalendarPast;
	}
	wxString CalendarNotEntered() const override
	{
		return m_CalendarNotEntered;
	}
	wxString CalendarEntered() const override
	{
		return m_CalendarEntered;
	}
	wxString CalendarPending() const override
	{
		return m_CalendarPending;
	}
	wxString CalendarPlanning() const override
	{
		return m_CalendarPlanning;
	}
	wxString CalendarStatusN() const override
	{
		return m_CalendarStatusN;
	}
	wxString CalendarStatusE() const override
	{
		return m_CalendarStatusE;
	}
	wxString CalendarStatusO() const override
	{
		return m_CalendarStatusO;
	}
	wxString CalendarStatusP() const override
	{
		return m_CalendarStatusP;
	}
	wxString CalendarOpens() const override
	{
		return m_CalendarOpens;
	}
	wxString CalendarDraw() const override
	{
		return m_CalendarDraw;
	}
	wxString CalendarCloses() const override
	{
		return m_CalendarCloses;
	}

	wxString UpdateFaults(int nNew, int nSkipped) const override;
	wxString UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const override;
	wxString UpdateVenues(int nNew, int nUpdated, int nSkipped) const override;
	wxString UpdateLifetimeNames(int nNew, int nSkipped) const override;
	wxString UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const override;
	wxString UpdateDivisionsReordered() const override;
	wxString UpdateEvents(int nAdded, int nUpdated, int nSkipped) const override;
	wxString UpdateEventsReordered() const override;
	wxString UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const override;
	wxString UpdateMultiqsReordered() const override;
	wxString UpdateLevels(int nAdded, int nUpdated, int nSkipped) const override;
	wxString UpdateLevelsReordered() const override;
	wxString UpdateTitles(int nAdded, int nUpdated, int nSkipped) const override;
	wxString UpdateTitlesReordered() const override;
	wxString UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const override;
	wxString UpdateSubLevelsReordered() const override;
	wxString UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const override;
	wxString WarnDeletedRuns(int nRuns, wxString const& inRunsMsg) const override;
	wxString UpdateTeamRuns(int nRuns, wxString const& inRunsMsg) const override;
	wxString UpdateTableRuns(int nRuns) const override;
	wxString UpdateTableRuns(int nRuns, wxString const& inRunsMsg) const override;
	wxString UpdateSubnameRuns(int nRuns, wxString const& inRunsMsg) const override;

	wxString ActionRenameOtherPoints(wxString const& oldName, wxString const& newName, int nChanges) const override;
	wxString ActionPreDeleteOtherPoints(wxString const& name, int nChanges) const override;
	wxString ActionDeleteOtherPoints(wxString const& name) const override;
	wxString ActionRenameVenue(wxString const& oldName, wxString const& newName, int nChanges) const override;
	wxString ActionPreDeleteVenue(wxString const& name, int nChanges) const override;
	wxString ActionDeleteVenue(wxString const& name) const override;
	wxString ActionRenameMultiQ(wxString const& venue, wxString const& oldName, wxString const& newName, int nChanges)
		const override;
	wxString ActionPreDeleteMultiQ(wxString const& venue, wxString const& name, int nChanges) const override;
	wxString ActionDeleteMultiQ(wxString const& venue, wxString const& name) const override;
	wxString ActionRenameDivision(wxString const& venue, wxString const& oldName, wxString const& newName, int nChanges)
		const override;
	wxString ActionPreDeleteDivision(wxString const& venue, wxString const& name, int nChanges) const override;
	wxString ActionDeleteDivision(wxString const& venue, wxString const& name) const override;
	wxString ActionRenameLevel(wxString const& venue, wxString const& oldName, wxString const& newName, int nChanges)
		const override;
	wxString ActionPreDeleteLevel(wxString const& venue, wxString const& name, int nChanges) const override;
	wxString ActionDeleteLevel(wxString const& venue, wxString const& name) const override;
	wxString ActionRenameTitle(wxString const& venue, wxString const& oldName, wxString const& newName, int nChanges)
		const override;
	wxString ActionPreDeleteTitle(wxString const& venue, wxString const& name, int nChanges) const override;
	wxString ActionDeleteTitle(wxString const& venue, wxString const& name) const override;
	wxString ActionRenameEvent(wxString const& venue, wxString const& oldName, wxString const& newName, int nChanges)
		const override;
	wxString ActionPreDeleteEvent(wxString const& venue, wxString const& name, int nChanges) const override;
	wxString ActionDeleteEvent(wxString const& venue, wxString const& name) const override;
	wxString ActionRenameLifetimeName(
		wxString const& venue,
		wxString const& oldName,
		wxString const& newName,
		int nEventChanges) const override;
	wxString ActionDeleteLifetimeName(wxString const& venue, wxString const& name, int nEventChanges) const override;

	wxString TitlePointsNameFormat(double points, double faults) const override;
	wxString LifetimePointsNameWithSpeedPointsFormat(double faults) const override;
	wxString LifetimePointsNameFormat(double points, double faults) const override;
	wxString PlacementPointsNameFormat(double points, short place) const override;

	wxString ScoreStyleUnknown() const override
	{
		return m_ScoreStyleUnknown;
	}
	wxString ScoreStyleFaultsTime() const override
	{
		return m_ScoreStyleFaultsTime;
	}
	wxString ScoreStyleFaults100Time() const override
	{
		return m_ScoreStyleFaults100Time;
	}
	wxString ScoreStyleFaults200Time() const override
	{
		return m_ScoreStyleFaults200Time;
	}
	wxString ScoreStyleOCScoreTime() const override
	{
		return m_ScoreStyleOCScoreTime;
	}
	wxString ScoreStyleScoreTime() const override
	{
		return m_ScoreStyleScoreTime;
	}
	wxString ScoreStyleTimePlusFaults() const override
	{
		return m_ScoreStyleTimePlusFaults;
	}
	wxString ScoreStyleTimeNoPlaces() const override
	{
		return m_ScoreStyleTimeNoPlaces;
	}
	wxString ScoreStylePassFail() const override
	{
		return m_ScoreStylePassFail;
	}

	wxString ExistingPointsOther() const override
	{
		return m_ExistingPointsOther;
	}
	wxString ExistingPointsLifetime() const override
	{
		return m_ExistingPointsLifetime;
	}
	wxString ExistingPointsRun() const override
	{
		return m_ExistingPointsRun;
	}
	wxString ExistingPointsSpeed() const override
	{
		return m_ExistingPointsSpeed;
	}
	wxString ExistingPointsMQ() const override
	{
		return m_ExistingPointsMQ;
	}
	wxString ExistingPointsSQ() const override
	{
		return m_ExistingPointsSQ;
	}

	wxString ErrorInvalidDocStructure(wchar_t const* const inMsg) const override;
	wxString ErrorMissingAttribute(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg = nullptr) const override;
	wxString ErrorInvalidAttributeValue(
		wchar_t const* const inElement,
		wchar_t const* const inAttrib,
		wchar_t const* const inMsg = nullptr) const override;

private:
	wxString m_UnknownVersion;
	wxString m_WarningNewerDoc;
	wxString m_InvalidDocStructure;
	wxString m_InvalidFileFormat;
	wxString m_InvalidFileMissingAttrib;
	wxString m_InvalidFileBadAttrib;
	wxString m_InvalidDate;
	wxString m_InvalidDivLevel;
	wxString m_InvalidEventName;
	wxString m_InvalidRoot;
	wxString m_MissingConfig;
	wxString m_InvalidConfig;
	wxString m_InvalidVenueConfig;
	wxString m_InvalidDivName;
	wxString m_InvalidVenueName;
	wxString m_InvalidLifetimeName;
	wxString m_InvalidOtherPtsName;
	wxString m_InvalidMultiqName;
	wxString m_InvalidMultiqConversion;
	wxString m_InvalidEvent;
	wxString m_InvalidTitle;
	wxString m_ValidValues;
	wxString m_ValidValuesBool;
	wxString m_CalendarTentative;
	wxString m_CalendarPast;
	wxString m_CalendarNotEntered;
	wxString m_CalendarEntered;
	wxString m_CalendarPending;
	wxString m_CalendarPlanning;
	wxString m_CalendarStatusN;
	wxString m_CalendarStatusE;
	wxString m_CalendarStatusO;
	wxString m_CalendarStatusP;
	wxString m_CalendarOpens;
	wxString m_CalendarDraw;
	wxString m_CalendarCloses;

	wxString m_ScoreStyleUnknown;
	wxString m_ScoreStyleFaultsTime;
	wxString m_ScoreStyleFaults100Time;
	wxString m_ScoreStyleFaults200Time;
	wxString m_ScoreStyleOCScoreTime;
	wxString m_ScoreStyleScoreTime;
	wxString m_ScoreStyleTimePlusFaults;
	wxString m_ScoreStyleTimeNoPlaces;
	wxString m_ScoreStylePassFail;

	wxString m_QTypeNA;
	wxString m_QTypeQ;
	wxString m_QTypeNQ;
	wxString m_QTypeE;
	wxString m_QTypeSQ;

	wxString m_ExistingPointsOther;
	wxString m_ExistingPointsLifetime;
	wxString m_ExistingPointsRun;
	wxString m_ExistingPointsSpeed;
	wxString m_ExistingPointsMQ;
	wxString m_ExistingPointsSQ;
};

} // namespace dconSoft
