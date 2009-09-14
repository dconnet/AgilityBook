/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2008-11-21 DRC Enable tallying runs that have only lifetime points.
 * @li 2006-12-24 DRC When finding events, only insert it once.
 * @li 2006-11-05 DRC Fix FindAllEvents to allow searching with wildcards.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2006-01-03 DRC FindAllEvents was not working right (see below)
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-11-15 DRC Added time fault computation on T+F.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Added ValidTo, changed ValidFrom dtd name (was 'Date').
 * @li 2003-12-11 DRC Added ValidFrom, not fully implemented yet.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-14 DRC Added open/close pts to configuration.
 * @li 2003-07-12 DRC Version 5: changed how title points are configured.
 * @li 2003-06-11 DRC Added dropFractions attribute.
 */

#include "stdafx.h"
#include "ARBConfigScoring.h"

#include <algorithm>
#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// static

#define SCORING_TYPE_FT		wxT("FaultsThenTime")
#define SCORING_TYPE_FT100	wxT("Faults100ThenTime")
#define SCORING_TYPE_FT200	wxT("Faults200ThenTime")
#define SCORING_TYPE_OCT	wxT("OCScoreThenTime")
#define SCORING_TYPE_ST		wxT("ScoreThenTime")
#define SCORING_TYPE_TF		wxT("TimePlusFaults")

wxString ARBConfigScoring::GetScoringStyleStr(ScoringStyle inStyle)
{
	wxString style;
	switch (inStyle)
	{
	default:
	case eUnknown:
		style = Localization()->ScoreStyleUnknown();
		break;
	case eFaultsThenTime:
		style = Localization()->ScoreStyleFaultsTime();
		break;
	case eFaults100ThenTime:
		style = Localization()->ScoreStyleFaults100Time();
		break;
	case eFaults200ThenTime:
		style = Localization()->ScoreStyleFaults200Time();
		break;
	case eOCScoreThenTime:
		style = Localization()->ScoreStyleOCScoreTime();
		break;
	case eScoreThenTime:
		style = Localization()->ScoreStyleScoreTime();
		break;
	case eTimePlusFaults:
		style = Localization()->ScoreStyleTimePlusFaults();
		break;
	}
	return style;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigScoringPtr ARBConfigScoring::New()
{
	return ARBConfigScoringPtr(new ARBConfigScoring());
}


ARBConfigScoring::ARBConfigScoring()
	: m_ValidFrom()
	, m_ValidTo()
	, m_Division()
	, m_Level()
	, m_Style(eUnknown)
	, m_bDropFractions(false)
	, m_bCleanQ(false)
	, m_bTimeFaultsUnder(false)
	, m_bTimeFaultsOver(false)
	, m_bSubtractTimeFaults(false)
	, m_TimeFaultMultiplier(1)
	, m_Note()
	, m_OpeningPts(0)
	, m_ClosingPts(0)
	, m_bSuperQ(false)
	, m_bDoubleQ(false)
	, m_bSpeedPts(false)
	, m_bBonusPts(false)
	, m_PlaceInfo()
	, m_TitlePoints()
	, m_LifePoints()
	, m_Placements()
{
}


ARBConfigScoring::ARBConfigScoring(ARBConfigScoring const& rhs)
	: m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Style(rhs.m_Style)
	, m_bDropFractions(rhs.m_bDropFractions)
	, m_bCleanQ(rhs.m_bCleanQ)
	, m_bTimeFaultsUnder(rhs.m_bTimeFaultsUnder)
	, m_bTimeFaultsOver(rhs.m_bTimeFaultsOver)
	, m_bSubtractTimeFaults(rhs.m_bSubtractTimeFaults)
	, m_TimeFaultMultiplier(rhs.m_TimeFaultMultiplier)
	, m_Note(rhs.m_Note)
	, m_OpeningPts(rhs.m_OpeningPts)
	, m_ClosingPts(rhs.m_ClosingPts)
	, m_bSuperQ(rhs.m_bSuperQ)
	, m_bDoubleQ(rhs.m_bDoubleQ)
	, m_bSpeedPts(rhs.m_bSpeedPts)
	, m_bBonusPts(rhs.m_bBonusPts)
	, m_PlaceInfo()
	, m_TitlePoints()
	, m_LifePoints()
	, m_Placements()
{
	rhs.m_PlaceInfo.Clone(m_PlaceInfo);
	rhs.m_TitlePoints.Clone(m_TitlePoints);
	rhs.m_LifePoints.Clone(m_LifePoints);
	rhs.m_Placements.Clone(m_Placements);
}


ARBConfigScoring::~ARBConfigScoring()
{
}


ARBConfigScoringPtr ARBConfigScoring::Clone() const
{
	return ARBConfigScoringPtr(new ARBConfigScoring(*this));
}


ARBConfigScoring& ARBConfigScoring::operator=(ARBConfigScoring const& rhs)
{
	if (this != &rhs)
	{
		m_ValidFrom = rhs.m_ValidFrom;
		m_ValidTo = rhs.m_ValidTo;
		m_Division = rhs.m_Division;
		m_Level = rhs.m_Level;
		m_Style = rhs.m_Style;
		m_bDropFractions = rhs.m_bDropFractions;
		m_bCleanQ = rhs.m_bCleanQ;
		m_bTimeFaultsUnder = rhs.m_bTimeFaultsUnder;
		m_bTimeFaultsOver = rhs.m_bTimeFaultsOver;
		m_bSubtractTimeFaults = rhs.m_bSubtractTimeFaults;
		m_TimeFaultMultiplier = rhs.m_TimeFaultMultiplier;
		m_Note = rhs.m_Note;
		m_OpeningPts = rhs.m_OpeningPts;
		m_ClosingPts = rhs.m_ClosingPts;
		m_bSuperQ = rhs.m_bSuperQ;
		m_bDoubleQ = rhs.m_bDoubleQ;
		m_bSpeedPts = rhs.m_bSpeedPts;
		m_bBonusPts = rhs.m_bBonusPts;
		rhs.m_PlaceInfo.Clone(m_PlaceInfo);
		rhs.m_TitlePoints.Clone(m_TitlePoints);
		rhs.m_LifePoints.Clone(m_LifePoints);
		rhs.m_Placements.Clone(m_Placements);
	}
	return *this;
}


bool ARBConfigScoring::operator==(ARBConfigScoring const& rhs) const
{
	return m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_Division == rhs.m_Division
		&& m_Level == rhs.m_Level
		&& m_Style == rhs.m_Style
		&& m_bDropFractions == rhs.m_bDropFractions
		&& m_bCleanQ == rhs.m_bCleanQ
		&& m_bTimeFaultsUnder == rhs.m_bTimeFaultsUnder
		&& m_bTimeFaultsOver == rhs.m_bTimeFaultsOver
		&& m_bSubtractTimeFaults == rhs.m_bSubtractTimeFaults
		&& m_TimeFaultMultiplier == rhs.m_TimeFaultMultiplier
		&& m_Note == rhs.m_Note
		&& m_OpeningPts == rhs.m_OpeningPts
		&& m_ClosingPts == rhs.m_ClosingPts
		&& m_bSuperQ == rhs.m_bSuperQ
		&& m_bDoubleQ == rhs.m_bDoubleQ
		&& m_bSpeedPts == rhs.m_bSpeedPts
		&& m_bBonusPts == rhs.m_bBonusPts
		&& m_PlaceInfo == rhs.m_PlaceInfo
		&& m_TitlePoints == rhs.m_TitlePoints
		&& m_LifePoints == rhs.m_LifePoints
		&& m_Placements == rhs.m_Placements;
}


bool ARBConfigScoring::Load(
		ARBConfigDivisionList const& inDivisions,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_SCORING)
		return false;
	// Probably unnecessary since it isn't actually implemented yet!
	if (inVersion == ARBVersion(8, 0))
		inTree->GetAttrib(wxT("Date"), m_ValidFrom);
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom))
	{
		wxString attrib;
		inTree->GetAttrib(ATTRIB_SCORING_VALIDFROM, attrib);
		wxString msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDFROM, msg));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo))
	{
		wxString attrib;
		inTree->GetAttrib(ATTRIB_SCORING_VALIDTO, attrib);
		wxString msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDTO, msg));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_SCORING_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_DIVISION));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_SCORING_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_LEVEL));
		return false;
	}
	if (!inDivisions.VerifyLevel(m_Division, m_Level))
	{
		wxString msg(Localization()->InvalidDivLevel());
		msg += m_Division;
		msg += wxT("/");
		msg += m_Level;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_LEVEL, msg));
		return false;
	}

	wxString attrib;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_SCORING_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_TYPE));
		return false;
	}
	if (attrib == SCORING_TYPE_FT)
		m_Style = eFaultsThenTime;
	else if (attrib == SCORING_TYPE_FT100)
	{
		m_Style = eFaults100ThenTime;
		if (inVersion <= ARBVersion(3,0))
			m_bDropFractions = true;
	}
	else if (attrib == SCORING_TYPE_FT200) // Version5.
		m_Style = eFaults200ThenTime;
	else if (attrib == SCORING_TYPE_OCT)
		m_Style = eOCScoreThenTime;
	else if (attrib == SCORING_TYPE_ST)
		m_Style = eScoreThenTime;
	else if (attrib == SCORING_TYPE_TF)
		m_Style = eTimePlusFaults;
	else
	{
		wxString msg(Localization()->ValidValues());
		msg += SCORING_TYPE_FT;
		msg += wxT(", ");
		msg += SCORING_TYPE_FT100;
		msg += wxT(", ");
		msg += SCORING_TYPE_FT200;
		msg += wxT(", ");
		msg += SCORING_TYPE_OCT;
		msg += wxT(", ");
		msg += SCORING_TYPE_ST;
		msg += wxT(", ");
		msg += SCORING_TYPE_TF;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE, msg));
		return false;
	}

	// This attribute came in in version 4, but go ahead and read it even if
	// the current doc is earlier. This will allow someone to hand-edit it in.
	// Not advisable, but it doesn't hurt anything either.
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DROPFRACTIONS, Localization()->ValidValuesBool()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q, m_bCleanQ))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q, Localization()->ValidValuesBool()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_UNDER, m_bTimeFaultsUnder))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_UNDER, Localization()->ValidValuesBool()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_OVER, m_bTimeFaultsOver))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_OVER, Localization()->ValidValuesBool()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, m_bSubtractTimeFaults))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, Localization()->ValidValuesBool()));
		return false;
	}
	inTree->GetAttrib(ATTRIB_SCORING_TF_MULTIPLIER, m_TimeFaultMultiplier);

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUPERQ, Localization()->ValidValuesBool()));
		return false;
	}

	if (inVersion < ARBVersion(11, 0))
	{
		if (ElementNode::eInvalidValue == inTree->GetAttrib(wxT("doubleQ"), m_bDoubleQ))
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, wxT("doubleQ"), Localization()->ValidValuesBool()));
			return false;
		}
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SPEEDPTS, m_bSpeedPts))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SPEEDPTS, Localization()->ValidValuesBool()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_BONUSPTS, m_bBonusPts))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_BONUSPTS, Localization()->ValidValuesBool()));
		return false;
	}

	bool bVer10orMore = (inVersion >= ARBVersion(10, 0));
	if (inVersion >= ARBVersion(5,0))
	{
		if (inVersion < ARBVersion(10, 1))
		{
			if (ElementNode::eInvalidValue == inTree->GetAttrib(wxT("machPts"), m_bSpeedPts))
			{
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, wxT("machPts"), Localization()->ValidValuesBool()));
				return false;
			}
		}
		if (m_bSpeedPts && inVersion < ARBVersion(12, 3))
		{
			m_PlaceInfo.AddPlaceInfo(1, 2.0, true);
			m_PlaceInfo.AddPlaceInfo(2, 1.5, true);
		}
		inTree->GetAttrib(ATTRIB_SCORING_OPENINGPTS, m_OpeningPts);
		inTree->GetAttrib(ATTRIB_SCORING_CLOSINGPTS, m_ClosingPts);
		for (int i = 0; i < inTree->GetElementCount(); ++i)
		{
			ElementNodePtr element = inTree->GetElementNode(i);
			if (!element)
				continue;
			if (element->GetName() == TREE_NOTE)
				m_Note = element->GetValue();
			else if (element->GetName() == TREE_PLACE_INFO)
			{
				if (!m_PlaceInfo.Load(element, inVersion, ioCallback))
					return false;
			}
			else if (element->GetName() == TREE_TITLE_POINTS)
			{
				if (!m_TitlePoints.Load(element, inVersion, ioCallback, m_LifePoints))
					return false;
			}
			else if (element->GetName() == TREE_LIFETIME_POINTS)
			{
				if (bVer10orMore)
				{
					if (!m_LifePoints.Load(element, inVersion, ioCallback))
						return false;
				}
			}
			else if (element->GetName() == TREE_PLACEMENTS)
			{
				for (int iPlace = 0; iPlace < element->GetElementCount(); ++iPlace)
				{
					ElementNodePtr place = element->GetElementNode(iPlace);
					if (!place)
						continue;
					if (place->GetName() == TREE_PLACE_INFO)
					{
						if (!m_Placements.Load(place, inVersion, ioCallback))
							return false;
					}
				}
			}
		}
		m_TitlePoints.sort();
		m_LifePoints.sort();
		m_Placements.sort();
	}

	// Migrate pre-5 files.
	else
	{
		short ptsWhenClean = 0;
		if (ElementNode::eFound != inTree->GetAttrib(wxT("Clean"), ptsWhenClean))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SCORING, wxT("Clean")));
			return false;
		}
		if (0 < ptsWhenClean)
		{
			m_TitlePoints.AddTitlePoints(ptsWhenClean, 0);
		}

		short faultsAllowed = 0;
		short ptsWhenNotClean = 0;
		inTree->GetAttrib(wxT("FaultsAllowed"), faultsAllowed);
		inTree->GetAttrib(wxT("WithFaults"), ptsWhenNotClean);
		if (0 < faultsAllowed && 0 < ptsWhenNotClean)
		{
			m_TitlePoints.AddTitlePoints(ptsWhenNotClean, faultsAllowed);
		}

		if (inVersion >= ARBVersion(3,0))
			m_Note = inTree->GetValue();
	}

	return true;
}


bool ARBConfigScoring::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr scoring = ioTree->AddElementNode(TREE_SCORING);
	if (m_ValidFrom.IsValid())
		scoring->AddAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		scoring->AddAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo);
	scoring->AddAttrib(ATTRIB_SCORING_DIVISION, m_Division);
	scoring->AddAttrib(ATTRIB_SCORING_LEVEL, m_Level);
	switch (m_Style)
	{
	default:
		assert(0);
#ifdef _DEBUG
		DumpErrorMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE), true);
#endif
		return false;
	case eFaultsThenTime:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_FT);
		break;
	case eFaults100ThenTime:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_FT100);
		break;
	case eFaults200ThenTime:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_FT200);
		break;
	case eOCScoreThenTime:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_OCT);
		break;
	case eScoreThenTime:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_ST);
		break;
	case eTimePlusFaults:
		scoring->AddAttrib(ATTRIB_SCORING_TYPE, SCORING_TYPE_TF);
		break;
	}
	if (m_bDropFractions)
		scoring->AddAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions);
	if (m_bCleanQ)
		scoring->AddAttrib(ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q, m_bCleanQ);
	if (m_bTimeFaultsUnder)
		scoring->AddAttrib(ATTRIB_SCORING_TIMEFAULTS_UNDER, m_bTimeFaultsUnder);
	if (m_bTimeFaultsOver)
		scoring->AddAttrib(ATTRIB_SCORING_TIMEFAULTS_OVER, m_bTimeFaultsOver);
	if (m_bSubtractTimeFaults)
		scoring->AddAttrib(ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, m_bSubtractTimeFaults);
	if (1 < m_TimeFaultMultiplier)
		scoring->AddAttrib(ATTRIB_SCORING_TF_MULTIPLIER, m_TimeFaultMultiplier);
	if (0 < m_Note.length())
	{
		ElementNodePtr note = scoring->AddElementNode(TREE_NOTE);
		note->SetValue(m_Note);
	}
	if (0 < m_OpeningPts)
		scoring->AddAttrib(ATTRIB_SCORING_OPENINGPTS, m_OpeningPts);
	if (0 < m_ClosingPts)
		scoring->AddAttrib(ATTRIB_SCORING_CLOSINGPTS, m_ClosingPts);
	if (m_bSuperQ)
		scoring->AddAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ);
	if (m_bSpeedPts)
		scoring->AddAttrib(ATTRIB_SCORING_SPEEDPTS, m_bSpeedPts);
	if (m_bBonusPts)
		scoring->AddAttrib(ATTRIB_SCORING_BONUSPTS, m_bBonusPts);
	if (m_bSpeedPts)
	{
		if (!m_PlaceInfo.Save(scoring))
			return false;
	}
	if (!m_TitlePoints.Save(scoring))
		return false;
	if (!m_LifePoints.Save(scoring))
		return false;
	if (0 < m_Placements.size())
	{
		ElementNodePtr place = scoring->AddElementNode(TREE_PLACEMENTS);
		if (!m_Placements.Save(place))
			return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigScoringList::Load(
		ARBConfigDivisionList const& inDivisions,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigScoringPtr thing(ARBConfigScoring::New());
	if (!thing->Load(inDivisions, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


size_t ARBConfigScoringList::FindAllEvents(
		wxString const& inDivision,
		wxString const& inLevel,
		ARBDate const& inDate,
		bool inTitlePoints,
		ARBVector<ARBConfigScoringPtr>& outList) const
{
	outList.clear();
	const_iterator iter;
	for (iter = begin(); iter != end(); ++iter)
	{
		if (((*iter)->GetDivision() == inDivision
			|| (*iter)->GetDivision() == WILDCARD_DIVISION
			|| inDivision == WILDCARD_DIVISION)
		&& ((*iter)->GetLevel() == inLevel
			|| (*iter)->GetLevel() == WILDCARD_LEVEL
			|| inLevel == WILDCARD_LEVEL))
		{
			if ((*iter)->IsValidOn(inDate))
				outList.push_back(*iter);
		}
	}
	// Note: When the input date is not valid, we want ALL scoring methods
	// that match, so don't shortcircuit based on outList.size. The problem
	// showed up in 1.7.0.12 with NADAC Gamblers. All the gambler runs earned
	// in 2005 were no longer showing up in the points.

	// It failed, try wildcards...
	for (iter = begin(); (!inDate.IsValid() || 0 == outList.size()) && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == inDivision && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			if ((*iter)->IsValidOn(inDate))
			{
				// Make sure the entry isn't already there.
				// (We were double-listing points in the Points view)
				if (outList.end() == std::find(outList.begin(), outList.end(), *iter))
					outList.push_back(*iter);
				break;
			}
		}
	}
	for (iter = begin(); (!inDate.IsValid() || 0 == outList.size()) && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == inLevel)
		{
			if ((*iter)->IsValidOn(inDate))
			{
				if (outList.end() == std::find(outList.begin(), outList.end(), *iter))
					outList.push_back(*iter);
				break;
			}
		}
	}
	for (iter = begin(); (!inDate.IsValid() || 0 == outList.size()) && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			if ((*iter)->IsValidOn(inDate))
			{
				if (outList.end() == std::find(outList.begin(), outList.end(), *iter))
					outList.push_back(*iter);
				break;
			}
		}
	}

	if (inTitlePoints)
	{
		ARBVector<ARBConfigScoringPtr>::iterator iter2;
		for (iter2 = outList.begin(); iter2 != outList.end(); )
		{
			if (0 < (*iter2)->GetTitlePoints().size() || 0 < (*iter2)->GetLifetimePoints().size())
				++iter2;
			else
				iter2 = outList.erase(iter2);
		}
	}
	return outList.size();
}


bool ARBConfigScoringList::FindEvent(
		wxString const& inDivision,
		wxString const& inLevel,
		ARBDate const& inDate,
		ARBConfigScoringPtr* outEvent) const
{
	if (outEvent)
		outEvent->reset();
	ARBVector<ARBConfigScoringPtr> items;
	FindAllEvents(inDivision, inLevel, inDate, false, items);
	if (0 == items.size())
		return false;
	ARBConfigScoringPtr pEvent;
	if (1 == items.size())
		pEvent = *(items.begin());
	else
	{
		ARBVector<ARBConfigScoringPtr>::iterator iter;
		for (iter = items.begin(); iter != items.end(); )
		{
			ARBConfigScoringPtr pScoring = *iter;
			if (!pScoring->IsValidOn(inDate))
				iter = items.erase(iter);
			else
				++iter;
		}
		if (1 == items.size())
			pEvent = *(items.begin());
		else if (1 < items.size())
		{
			// Umm, this means they have items with overlapping ranges...
			// Which may occur when creating the methods.
#ifdef _DEBUG
			DumpErrorMessage(wxT("Warning: Overlapping date ranges"), true);
#endif
			pEvent = *(items.begin());
		}
	}
	if (pEvent)
	{
		if (outEvent)
			*outEvent = pEvent;
		return true;
	}
	else
		return false;
}


bool ARBConfigScoringList::VerifyEvent(
		wxString const& inDivision,
		wxString const& inLevel,
		ARBDate const& inDate) const
{
	ARBVector<ARBConfigScoringPtr> items;
	FindAllEvents(inDivision, inLevel, inDate, false, items);
	return (0 < items.size());
}


ARBConfigScoringPtr ARBConfigScoringList::AddScoring()
{
	ARBConfigScoringPtr pScoring(ARBConfigScoring::New());
	pScoring->SetDivision(WILDCARD_DIVISION);
	pScoring->SetLevel(WILDCARD_LEVEL);
	pScoring->SetScoringStyle(ARBConfigScoring::eFaultsThenTime);
	push_back(pScoring);
	return pScoring;
}
