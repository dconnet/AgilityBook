#pragma once

/*
 * Copyright (c) 2008-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-19 DRC Ported to wxWidgets.
 * @li 2008-01-13 DRC Created
 */

#include "ARBLocalization.h"

class CLocalization : public IARBLocalization
{
public:
	CLocalization();
	void Load();

	wxString UnknownVersion() const				{return m_UnknownVersion;}
	wxString WarningNewerDoc() const			{return m_WarningNewerDoc;}
	wxString InvalidDocStructure() const		{return m_InvalidDocStructure;}
	wxString InvalidFileFormat() const			{return m_InvalidFileFormat;}
	wxString InvalidFileMissingAttrib() const	{return m_InvalidFileMissingAttrib;}
	wxString InvalidFileBadAttrib() const		{return m_InvalidFileBadAttrib;}
	wxString InvalidDate() const				{return m_InvalidDate;}
	wxString InvalidDivLevel() const			{return m_InvalidDivLevel;}
	wxString InvalidEventName() const			{return m_InvalidEventName;}
	wxString InvalidRoot() const				{return m_InvalidRoot;}
	wxString MissingConfig() const				{return m_MissingConfig;}
	wxString InvalidConfig() const				{return m_InvalidConfig;}
	wxString InvalidVenueConfig() const			{return m_InvalidVenueConfig;}
	wxString InvalidDivName() const				{return m_InvalidDivName;}
	wxString InvalidVenueName() const			{return m_InvalidVenueName;}
	wxString InvalidOtherPtsName() const		{return m_InvalidOtherPtsName;}
	wxString InvalidMultiqName() const			{return m_InvalidMultiqName;}
	wxString InvalidMultiqConversion() const	{return m_InvalidMultiqConversion;}
	wxString InvalidEvent() const				{return m_InvalidEvent;}
	wxString InvalidTitle() const				{return m_InvalidTitle;}
	wxString ValidValues() const				{return m_ValidValues;}
	wxString ValidValuesBool() const			{return m_ValidValuesBool;}
	wxString CalendarTentative() const			{return m_CalendarTentative;}
	wxString CalendarNotEntered() const			{return m_CalendarNotEntered;}
	wxString CalendarEntered() const			{return m_CalendarEntered;}
	wxString CalendarPending() const			{return m_CalendarPending;}
	wxString CalendarPlanning() const			{return m_CalendarPlanning;}
	wxString CalendarStatusN() const			{return m_CalendarStatusN;}
	wxString CalendarStatusE() const			{return m_CalendarStatusE;}
	wxString CalendarStatusO() const			{return m_CalendarStatusO;}
	wxString CalendarStatusP() const			{return m_CalendarStatusP;}
	wxString CalendarOpens() const				{return m_CalendarOpens;}
	wxString CalendarDraw() const				{return m_CalendarDraw;}
	wxString CalendarCloses() const				{return m_CalendarCloses;}

	wxString UpdateCalSites(int nNew, int nUpdated, int nSkipped) const;
	wxString UpdateFaults(int nNew, int nSkipped) const;
	wxString UpdateOtherPts(int nNew, int nUpdated, int nSkipped) const;
	wxString UpdateVenues(int nNew, int nUpdated, int nSkipped) const;
	wxString UpdateDivisions(int nAdded, int nUpdated, int nSkipped) const;
	wxString UpdateDivisionsReordered() const;
	wxString UpdateEvents(int nAdded, int nUpdated, int nSkipped) const;
	wxString UpdateEventsReordered() const;
	wxString UpdateMultiqs(int nAdded, int nDeleted, int nSkipped) const;
	wxString UpdateMultiqsReordered() const;
	wxString UpdateLevels(int nAdded, int nUpdated, int nSkipped) const;
	wxString UpdateLevelsReordered() const;
	wxString UpdateTitles(int nAdded, int nUpdated, int nSkipped) const;
	wxString UpdateTitlesReordered() const;
	wxString UpdateSubLevels(int nAdded) const;
	wxString UpdateSubLevelsReordered() const;
	wxString UpdateRules(int nAdded, int nDeleted, int nUpdated, int nSkipped) const;
	wxString WarnDeletedRuns(int nRuns, wxString const& inRunsMsg) const;
	wxString UpdateTeamRuns(int nRuns, wxString const& inRunsMsg) const;
	wxString UpdateTableRuns(int nRuns) const;

	wxString ActionDeleteCalPlugin(wxString const& name) const;
	wxString ActionRenameOtherPoints(
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteOtherPoints(
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteOtherPoints(wxString const& name) const;
	wxString ActionRenameVenue(
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteVenue(
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteVenue(wxString const& name) const;
	wxString ActionRenameMultiQ(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteMultiQ(
			wxString const& venue,
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteMultiQ(
			wxString const& venue,
			wxString const& name) const;
	wxString ActionRenameDivision(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteDivision(
			wxString const& venue,
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteDivision(
			wxString const& venue,
			wxString const& name) const;
	wxString ActionRenameLevel(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteLevel(
			wxString const& venue,
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteLevel(
			wxString const& venue,
			wxString const& name) const;
	wxString ActionRenameTitle(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteTitle(
			wxString const& venue,
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteTitle(
			wxString const& venue,
			wxString const& name) const;
	wxString ActionRenameEvent(
			wxString const& venue,
			wxString const& oldName,
			wxString const& newName,
			int nChanges) const;
	wxString ActionPreDeleteEvent(
			wxString const& venue,
			wxString const& name,
			int nChanges) const;
	wxString ActionDeleteEvent(
			wxString const& venue,
			wxString const& name) const;

	wxString TitlePointsNameFormat(double points, double faults) const;
	wxString LifetimePointsNameFormat(double points, double faults) const;
	wxString PlacementPointsNameFormat(double points, short place) const;

	wxString ScoreStyleUnknown() const			{return m_ScoreStyleUnknown;}
	wxString ScoreStyleFaultsTime() const		{return m_ScoreStyleFaultsTime;}
	wxString ScoreStyleFaults100Time() const	{return m_ScoreStyleFaults100Time;}
	wxString ScoreStyleFaults200Time() const	{return m_ScoreStyleFaults200Time;}
	wxString ScoreStyleOCScoreTime() const		{return m_ScoreStyleOCScoreTime;}
	wxString ScoreStyleScoreTime() const		{return m_ScoreStyleScoreTime;}
	wxString ScoreStyleTimePlusFaults() const	{return m_ScoreStyleTimePlusFaults;}

	wxString ExistingPointsOther() const		{return m_ExistingPointsOther;}
	wxString ExistingPointsRun() const			{return m_ExistingPointsRun;}
	wxString ExistingPointsSpeed() const		{return m_ExistingPointsSpeed;}
	wxString ExistingPointsMQ() const			{return m_ExistingPointsMQ;}
	wxString ExistingPointsSQ() const			{return m_ExistingPointsSQ;}

	wxString ErrorInvalidDocStructure(wxChar const* const inMsg) const;
	wxString ErrorMissingAttribute(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const;
	wxString ErrorInvalidAttributeValue(
			wxChar const* const inElement,
			wxChar const* const inAttrib,
			wxChar const* const inMsg = NULL) const;

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
	wxString m_InvalidOtherPtsName;
	wxString m_InvalidMultiqName;
	wxString m_InvalidMultiqConversion;
	wxString m_InvalidEvent;
	wxString m_InvalidTitle;
	wxString m_ValidValues;
	wxString m_ValidValuesBool;
	wxString m_CalendarTentative;
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

	wxString m_QTypeNA;
	wxString m_QTypeQ;
	wxString m_QTypeNQ;
	wxString m_QTypeE;
	wxString m_QTypeSQ;

	wxString m_ExistingPointsOther;
	wxString m_ExistingPointsRun;
	wxString m_ExistingPointsSpeed;
	wxString m_ExistingPointsMQ;
	wxString m_ExistingPointsSQ;
};
