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
 * 2009-07-19 Ported to wxWidgets.
 * 2008-01-13 Created
 */

#include "ARB/ARBLocalization.h"


class CLocalization : public IARBLocalization
{
public:
	CLocalization();
	bool Load();

	std::wstring UnknownVersion() const override			{return m_UnknownVersion;}
	std::wstring WarningNewerDoc() const override			{return m_WarningNewerDoc;}
	std::wstring InvalidDocStructure() const override		{return m_InvalidDocStructure;}
	std::wstring InvalidFileFormat() const override			{return m_InvalidFileFormat;}
	std::wstring InvalidFileMissingAttrib() const override	{return m_InvalidFileMissingAttrib;}
	std::wstring InvalidFileBadAttrib() const override		{return m_InvalidFileBadAttrib;}
	std::wstring InvalidDate() const override				{return m_InvalidDate;}
	std::wstring InvalidDivLevel() const override			{return m_InvalidDivLevel;}
	std::wstring InvalidEventName() const override			{return m_InvalidEventName;}
	std::wstring InvalidRoot() const override				{return m_InvalidRoot;}
	std::wstring MissingConfig() const override				{return m_MissingConfig;}
	std::wstring InvalidConfig() const override				{return m_InvalidConfig;}
	std::wstring InvalidVenueConfig() const override		{return m_InvalidVenueConfig;}
	std::wstring InvalidDivName() const override			{return m_InvalidDivName;}
	std::wstring InvalidVenueName() const override			{return m_InvalidVenueName;}
	std::wstring InvalidOtherPtsName() const override		{return m_InvalidOtherPtsName;}
	std::wstring InvalidLifetimeName() const override		{return m_InvalidLifetimeName;}
	std::wstring InvalidMultiqName() const override			{return m_InvalidMultiqName;}
	std::wstring InvalidMultiqConversion() const override	{return m_InvalidMultiqConversion;}
	std::wstring InvalidEvent() const override				{return m_InvalidEvent;}
	std::wstring InvalidTitle() const override				{return m_InvalidTitle;}
	std::wstring ValidValues() const override				{return m_ValidValues;}
	std::wstring ValidValuesBool() const override			{return m_ValidValuesBool;}
	std::wstring CalendarTentative() const override			{return m_CalendarTentative;}
	std::wstring CalendarPast() const override				{return m_CalendarPast;}
	std::wstring CalendarNotEntered() const override		{return m_CalendarNotEntered;}
	std::wstring CalendarEntered() const override			{return m_CalendarEntered;}
	std::wstring CalendarPending() const override			{return m_CalendarPending;}
	std::wstring CalendarPlanning() const override			{return m_CalendarPlanning;}
	std::wstring CalendarStatusN() const override			{return m_CalendarStatusN;}
	std::wstring CalendarStatusE() const override			{return m_CalendarStatusE;}
	std::wstring CalendarStatusO() const override			{return m_CalendarStatusO;}
	std::wstring CalendarStatusP() const override			{return m_CalendarStatusP;}
	std::wstring CalendarOpens() const override				{return m_CalendarOpens;}
	std::wstring CalendarDraw() const override				{return m_CalendarDraw;}
	std::wstring CalendarCloses() const override			{return m_CalendarCloses;}

	std::wstring UpdateCalSites(int nNew, int nUpdated, int nSkipped) const override;
	std::wstring UpdateFaults(int nNew, int nSkipped) const override;
	std::wstring UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const override;
	std::wstring UpdateVenues(int nNew, int nUpdated, int nSkipped) const override;
	std::wstring UpdateLifetimeNames(int nNew, int nSkipped) const override;
	std::wstring UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const override;
	std::wstring UpdateDivisionsReordered() const override;
	std::wstring UpdateEvents(int nAdded, int nUpdated, int nSkipped) const override;
	std::wstring UpdateEventsReordered() const override;
	std::wstring UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const override;
	std::wstring UpdateMultiqsReordered() const override;
	std::wstring UpdateLevels(int nAdded, int nUpdated, int nSkipped) const override;
	std::wstring UpdateLevelsReordered() const override;
	std::wstring UpdateTitles(int nAdded, int nUpdated, int nSkipped) const override;
	std::wstring UpdateTitlesReordered() const override;
	std::wstring UpdateSubLevels(int nAdded, int nUpdated, int nSkipped) const override;
	std::wstring UpdateSubLevelsReordered() const override;
	std::wstring UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const override;
	std::wstring WarnDeletedRuns(int nRuns, std::wstring const& inRunsMsg) const override;
	std::wstring UpdateTeamRuns(int nRuns, std::wstring const& inRunsMsg) const override;
	std::wstring UpdateTableRuns(int nRuns) const override;

	std::wstring ActionDeleteCalPlugin(std::wstring const& name) const override;
	std::wstring ActionRenameOtherPoints(
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteOtherPoints(
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteOtherPoints(std::wstring const& name) const override;
	std::wstring ActionRenameVenue(
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteVenue(
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteVenue(std::wstring const& name) const override;
	std::wstring ActionRenameMultiQ(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteMultiQ(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteMultiQ(
			std::wstring const& venue,
			std::wstring const& name) const override;
	std::wstring ActionRenameDivision(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteDivision(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteDivision(
			std::wstring const& venue,
			std::wstring const& name) const override;
	std::wstring ActionRenameLevel(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteLevel(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteLevel(
			std::wstring const& venue,
			std::wstring const& name) const override;
	std::wstring ActionRenameTitle(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteTitle(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteTitle(
			std::wstring const& venue,
			std::wstring const& name) const override;
	std::wstring ActionRenameEvent(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nChanges) const override;
	std::wstring ActionPreDeleteEvent(
			std::wstring const& venue,
			std::wstring const& name,
			int nChanges) const override;
	std::wstring ActionDeleteEvent(
			std::wstring const& venue,
			std::wstring const& name) const override;
	std::wstring ActionRenameLifetimeName(
			std::wstring const& venue,
			std::wstring const& oldName,
			std::wstring const& newName,
			int nEventChanges) const override;
	std::wstring ActionDeleteLifetimeName(
			std::wstring const& venue,
			std::wstring const& name,
			int nEventChanges) const override;

	std::wstring TitlePointsNameFormat(double points, double faults) const override;
	std::wstring LifetimePointsNameWithSpeedPointsFormat(double faults) const override;
	std::wstring LifetimePointsNameFormat(double points, double faults) const override;
	std::wstring PlacementPointsNameFormat(double points, short place) const override;

	std::wstring ScoreStyleUnknown() const override			{return m_ScoreStyleUnknown;}
	std::wstring ScoreStyleFaultsTime() const override		{return m_ScoreStyleFaultsTime;}
	std::wstring ScoreStyleFaults100Time() const override	{return m_ScoreStyleFaults100Time;}
	std::wstring ScoreStyleFaults200Time() const override	{return m_ScoreStyleFaults200Time;}
	std::wstring ScoreStyleOCScoreTime() const override		{return m_ScoreStyleOCScoreTime;}
	std::wstring ScoreStyleScoreTime() const override		{return m_ScoreStyleScoreTime;}
	std::wstring ScoreStyleTimePlusFaults() const override	{return m_ScoreStyleTimePlusFaults;}

	std::wstring ExistingPointsOther() const override		{return m_ExistingPointsOther;}
	std::wstring ExistingPointsLifetime() const override	{return m_ExistingPointsLifetime;}
	std::wstring ExistingPointsRun() const override			{return m_ExistingPointsRun;}
	std::wstring ExistingPointsSpeed() const override		{return m_ExistingPointsSpeed;}
	std::wstring ExistingPointsMQ() const override			{return m_ExistingPointsMQ;}
	std::wstring ExistingPointsSQ() const override			{return m_ExistingPointsSQ;}

	std::wstring ErrorInvalidDocStructure(wchar_t const* const inMsg) const override;
	std::wstring ErrorMissingAttribute(
			wchar_t const* const inElement,
			wchar_t const* const inAttrib,
			wchar_t const* const inMsg = nullptr) const override;
	std::wstring ErrorInvalidAttributeValue(
			wchar_t const* const inElement,
			wchar_t const* const inAttrib,
			wchar_t const* const inMsg = nullptr) const override;

private:
	std::wstring m_UnknownVersion;
	std::wstring m_WarningNewerDoc;
	std::wstring m_InvalidDocStructure;
	std::wstring m_InvalidFileFormat;
	std::wstring m_InvalidFileMissingAttrib;
	std::wstring m_InvalidFileBadAttrib;
	std::wstring m_InvalidDate;
	std::wstring m_InvalidDivLevel;
	std::wstring m_InvalidEventName;
	std::wstring m_InvalidRoot;
	std::wstring m_MissingConfig;
	std::wstring m_InvalidConfig;
	std::wstring m_InvalidVenueConfig;
	std::wstring m_InvalidDivName;
	std::wstring m_InvalidVenueName;
	std::wstring m_InvalidLifetimeName;
	std::wstring m_InvalidOtherPtsName;
	std::wstring m_InvalidMultiqName;
	std::wstring m_InvalidMultiqConversion;
	std::wstring m_InvalidEvent;
	std::wstring m_InvalidTitle;
	std::wstring m_ValidValues;
	std::wstring m_ValidValuesBool;
	std::wstring m_CalendarTentative;
	std::wstring m_CalendarPast;
	std::wstring m_CalendarNotEntered;
	std::wstring m_CalendarEntered;
	std::wstring m_CalendarPending;
	std::wstring m_CalendarPlanning;
	std::wstring m_CalendarStatusN;
	std::wstring m_CalendarStatusE;
	std::wstring m_CalendarStatusO;
	std::wstring m_CalendarStatusP;
	std::wstring m_CalendarOpens;
	std::wstring m_CalendarDraw;
	std::wstring m_CalendarCloses;

	std::wstring m_ScoreStyleUnknown;
	std::wstring m_ScoreStyleFaultsTime;
	std::wstring m_ScoreStyleFaults100Time;
	std::wstring m_ScoreStyleFaults200Time;
	std::wstring m_ScoreStyleOCScoreTime;
	std::wstring m_ScoreStyleScoreTime;
	std::wstring m_ScoreStyleTimePlusFaults;

	std::wstring m_QTypeNA;
	std::wstring m_QTypeQ;
	std::wstring m_QTypeNQ;
	std::wstring m_QTypeE;
	std::wstring m_QTypeSQ;

	std::wstring m_ExistingPointsOther;
	std::wstring m_ExistingPointsLifetime;
	std::wstring m_ExistingPointsRun;
	std::wstring m_ExistingPointsSpeed;
	std::wstring m_ExistingPointsMQ;
	std::wstring m_ExistingPointsSQ;
};
