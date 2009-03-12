#pragma once

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
 * @brief Implementation of IARBLocalization
 * @author David Connet
 *
 * Revision History
 * @li 2008-01-13 DRC Created
 */

#include "ARBLocalization.h"

class CLocalization : public IARBLocalization
{
public:
	CLocalization();
	void Load();

	tstring UnknownVersion() const				{return m_UnknownVersion;}
	tstring WarningNewerDoc() const				{return m_WarningNewerDoc;}
	tstring InvalidDocStructure() const			{return m_InvalidDocStructure;}
	tstring InvalidFileFormat() const			{return m_InvalidFileFormat;}
	tstring InvalidFileMissingAttrib() const	{return m_InvalidFileMissingAttrib;}
	tstring InvalidFileBadAttrib() const		{return m_InvalidFileBadAttrib;}
	tstring InvalidDate() const					{return m_InvalidDate;}
	tstring InvalidDivLevel() const				{return m_InvalidDivLevel;}
	tstring InvalidEventName() const			{return m_InvalidEventName;}
	tstring InvalidRoot() const					{return m_InvalidRoot;}
	tstring MissingConfig() const				{return m_MissingConfig;}
	tstring InvalidConfig() const				{return m_InvalidConfig;}
	tstring InvalidVenueConfig() const			{return m_InvalidVenueConfig;}
	tstring InvalidDivName() const				{return m_InvalidDivName;}
	tstring InvalidVenueName() const			{return m_InvalidVenueName;}
	tstring InvalidOtherPtsName() const			{return m_InvalidOtherPtsName;}
	tstring InvalidMultiqName() const			{return m_InvalidMultiqName;}
	tstring InvalidMultiqConversion() const		{return m_InvalidMultiqConversion;}
	tstring InvalidEvent() const				{return m_InvalidEvent;}
	tstring InvalidTitle() const				{return m_InvalidTitle;}
	tstring ValidValues() const					{return m_ValidValues;}
	tstring ValidValuesBool() const				{return m_ValidValuesBool;}
	tstring CalendarTentative() const			{return m_CalendarTentative;}
	tstring CalendarNotEntered() const			{return m_CalendarNotEntered;}
	tstring CalendarEntered() const				{return m_CalendarEntered;}
	tstring CalendarPlanning() const			{return m_CalendarPlanning;}
	tstring CalendarStatusN() const				{return m_CalendarStatusN;}
	tstring CalendarStatusE() const				{return m_CalendarStatusE;}
	tstring CalendarStatusP() const				{return m_CalendarStatusP;}
	tstring CalendarOpens() const				{return m_CalendarOpens;}
	tstring CalendarDraw() const				{return m_CalendarDraw;}
	tstring CalendarCloses() const				{return m_CalendarCloses;}

	tstring UpdateCalSites(int nNew, int nUpdated, int nSkipped) const;
	tstring UpdateFaults(int nNew, int nSkipped) const;
	tstring UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const;
	tstring UpdateVenues(int nNew, int nUpdated, int nSkipped) const;
	tstring UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const;
	tstring UpdateDivisionsReordered() const;
	tstring UpdateEvents(int nAdded, int nUpdated, int nSkipped) const;
	tstring UpdateEventsReordered() const;
	tstring UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const;
	tstring UpdateMultiqsReordered() const;
	tstring UpdateLevels(int nAdded, int nUpdated, int nSkipped) const;
	virtual tstring UpdateLevelsReordered() const;
	tstring UpdateTitles(int nAdded, int nUpdated, int nSkipped) const;
	tstring UpdateTitlesReordered() const;
	tstring UpdateSubLevels(int nAdded) const;
	virtual tstring UpdateSubLevelsReordered() const;
	tstring UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const;
	tstring WarnDeletedRuns(int nRuns, tstring const& inRunsMsg) const;
	tstring UpdateTeamRuns(int nRuns, tstring const& inRunsMsg) const;
	tstring UpdateTableRuns(int nRuns) const;

	tstring ActionDeleteCalPlugin(tstring const& name) const;
	tstring ActionRenameOtherPoints(
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteOtherPoints(
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteOtherPoints(tstring const& name) const;
	tstring ActionRenameVenue(
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteVenue(
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteVenue(tstring const& name) const;
	tstring ActionRenameMultiQ(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteMultiQ(
			tstring const& venue,
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteMultiQ(
			tstring const& venue,
			tstring const& name) const;
	tstring ActionRenameDivision(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteDivision(
			tstring const& venue,
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteDivision(
			tstring const& venue,
			tstring const& name) const;
	tstring ActionRenameLevel(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteLevel(
			tstring const& venue,
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteLevel(
			tstring const& venue,
			tstring const& name) const;
	tstring ActionRenameTitle(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteTitle(
			tstring const& venue,
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteTitle(
			tstring const& venue,
			tstring const& name) const;
	tstring ActionRenameEvent(
			tstring const& venue,
			tstring const& oldName,
			tstring const& newName,
			int nChanges) const;
	tstring ActionPreDeleteEvent(
			tstring const& venue,
			tstring const& name,
			int nChanges) const;
	tstring ActionDeleteEvent(
			tstring const& venue,
			tstring const& name) const;

	tstring TitlePointsNameFormat(double points, double faults) const;
	tstring LifetimePointsNameFormat(double points, double faults) const;
	tstring PlacementPointsNameFormat(double points, short place) const;

	tstring ScoreStyleUnknown() const			{return m_ScoreStyleUnknown;}
	tstring ScoreStyleFaultsTime() const		{return m_ScoreStyleFaultsTime;}
	tstring ScoreStyleFaults100Time() const		{return m_ScoreStyleFaults100Time;}
	tstring ScoreStyleFaults200Time() const		{return m_ScoreStyleFaults200Time;}
	tstring ScoreStyleOCScoreTime() const		{return m_ScoreStyleOCScoreTime;}
	tstring ScoreStyleScoreTime() const			{return m_ScoreStyleScoreTime;}
	tstring ScoreStyleTimePlusFaults() const	{return m_ScoreStyleTimePlusFaults;}

	tstring ExistingPointsOther() const			{return m_ExistingPointsOther;}
	tstring ExistingPointsRun() const			{return m_ExistingPointsRun;}
	tstring ExistingPointsSpeed() const			{return m_ExistingPointsSpeed;}
	tstring ExistingPointsMQ() const			{return m_ExistingPointsMQ;}
	tstring ExistingPointsSQ() const			{return m_ExistingPointsSQ;}

	tstring ErrorInvalidDocStructure(wxChar const* const inMsg) const;
	tstring ErrorMissingAttribute(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const;
	tstring ErrorInvalidAttributeValue(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const;

private:
	tstring m_UnknownVersion;
	tstring m_WarningNewerDoc;
	tstring m_InvalidDocStructure;
	tstring m_InvalidFileFormat;
	tstring m_InvalidFileMissingAttrib;
	tstring m_InvalidFileBadAttrib;
	tstring m_InvalidDate;
	tstring m_InvalidDivLevel;
	tstring m_InvalidEventName;
	tstring m_InvalidRoot;
	tstring m_MissingConfig;
	tstring m_InvalidConfig;
	tstring m_InvalidVenueConfig;
	tstring m_InvalidDivName;
	tstring m_InvalidVenueName;
	tstring m_InvalidOtherPtsName;
	tstring m_InvalidMultiqName;
	tstring m_InvalidMultiqConversion;
	tstring m_InvalidEvent;
	tstring m_InvalidTitle;
	tstring m_ValidValues;
	tstring m_ValidValuesBool;
	tstring m_CalendarTentative;
	tstring m_CalendarNotEntered;
	tstring m_CalendarEntered;
	tstring m_CalendarPlanning;
	tstring m_CalendarStatusN;
	tstring m_CalendarStatusE;
	tstring m_CalendarStatusP;
	tstring m_CalendarOpens;
	tstring m_CalendarDraw;
	tstring m_CalendarCloses;

	tstring m_ScoreStyleUnknown;
	tstring m_ScoreStyleFaultsTime;
	tstring m_ScoreStyleFaults100Time;
	tstring m_ScoreStyleFaults200Time;
	tstring m_ScoreStyleOCScoreTime;
	tstring m_ScoreStyleScoreTime;
	tstring m_ScoreStyleTimePlusFaults;

	tstring m_QTypeNA;
	tstring m_QTypeQ;
	tstring m_QTypeNQ;
	tstring m_QTypeE;
	tstring m_QTypeSQ;

	tstring m_ExistingPointsOther;
	tstring m_ExistingPointsRun;
	tstring m_ExistingPointsSpeed;
	tstring m_ExistingPointsMQ;
	tstring m_ExistingPointsSQ;
};
