/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * 2017-12-31 Add support for using raw faults when determining title points.
 * 2016-01-06 Add support for named lifetime points.
 * 2011-07-31 Allow a time fault multipler of 0.
 * 2011-01-08 Added test around a debug message.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2008-11-21 Enable tallying runs that have only lifetime points.
 * 2006-12-24 When finding events, only insert it once.
 * 2006-11-05 Fix FindAllEvents to allow searching with wildcards.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2006-01-03 FindAllEvents was not working right (see below)
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-18 Added a time fault multiplier.
 * 2004-11-15 Added time fault computation on T+F.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-02-02 Added VerifyEvent.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-27 Added ValidTo, changed ValidFrom dtd name (was 'Date').
 * 2003-12-11 Added ValidFrom, not fully implemented yet.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-07-14 Added open/close pts to configuration.
 * 2003-07-12 Version 5: changed how title points are configured.
 * 2003-06-11 Added dropFractions attribute.
 */

#include "stdafx.h"
#include "ARB/ARBConfigScoring.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

#if !defined(_WIN32) && !defined(__WXWINDOWS__)
#include <iostream>
static void OutputDebugString(wchar_t const* msg)
{
	if (msg)
		std::wcout << msg;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// static

#define SCORING_TYPE_FT		L"FaultsThenTime"
#define SCORING_TYPE_FT100	L"Faults100ThenTime"
#define SCORING_TYPE_FT200	L"Faults200ThenTime"
#define SCORING_TYPE_OCT	L"OCScoreThenTime"
#define SCORING_TYPE_ST		L"ScoreThenTime"
#define SCORING_TYPE_TF		L"TimePlusFaults"

std::wstring ARBConfigScoring::GetScoringStyleStr(ScoringStyle inStyle)
{
	std::wstring style;
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

namespace
{
	class ARBConfigScoring_concrete : public ARBConfigScoring
	{
	public:
		ARBConfigScoring_concrete() {}
		ARBConfigScoring_concrete(ARBConfigScoring const& rhs)
			: ARBConfigScoring(rhs)
		{
		}
	};
};


ARBConfigScoringPtr ARBConfigScoring::New()
{
	return std::make_shared<ARBConfigScoring_concrete>();
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
	, m_bTitlingPointsRawFaults(false)
	, m_bSubtractTimeFaults(false)
	, m_TimeFaultMultiplier(1)
	, m_Note()
	, m_OpeningPts(0)
	, m_ClosingPts(0)
	, m_bSuperQ(false)
	, m_bDoubleQ(false)
	, m_bSpeedPts(false)
	, m_bBonusTitlePts(false)
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
	, m_bTitlingPointsRawFaults(rhs.m_bTitlingPointsRawFaults)
	, m_bSubtractTimeFaults(rhs.m_bSubtractTimeFaults)
	, m_TimeFaultMultiplier(rhs.m_TimeFaultMultiplier)
	, m_Note(rhs.m_Note)
	, m_OpeningPts(rhs.m_OpeningPts)
	, m_ClosingPts(rhs.m_ClosingPts)
	, m_bSuperQ(rhs.m_bSuperQ)
	, m_bDoubleQ(rhs.m_bDoubleQ)
	, m_bSpeedPts(rhs.m_bSpeedPts)
	, m_bBonusTitlePts(rhs.m_bBonusTitlePts)
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
	return std::make_shared<ARBConfigScoring_concrete>(*this);
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
		m_bTitlingPointsRawFaults = rhs.m_bTitlingPointsRawFaults;
		m_bSubtractTimeFaults = rhs.m_bSubtractTimeFaults;
		m_TimeFaultMultiplier = rhs.m_TimeFaultMultiplier;
		m_Note = rhs.m_Note;
		m_OpeningPts = rhs.m_OpeningPts;
		m_ClosingPts = rhs.m_ClosingPts;
		m_bSuperQ = rhs.m_bSuperQ;
		m_bDoubleQ = rhs.m_bDoubleQ;
		m_bSpeedPts = rhs.m_bSpeedPts;
		m_bBonusTitlePts = rhs.m_bBonusTitlePts;
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
		&& m_bTitlingPointsRawFaults == rhs.m_bTitlingPointsRawFaults
		&& m_bSubtractTimeFaults == rhs.m_bSubtractTimeFaults
		&& m_TimeFaultMultiplier == rhs.m_TimeFaultMultiplier
		&& m_Note == rhs.m_Note
		&& m_OpeningPts == rhs.m_OpeningPts
		&& m_ClosingPts == rhs.m_ClosingPts
		&& m_bSuperQ == rhs.m_bSuperQ
		&& m_bDoubleQ == rhs.m_bDoubleQ
		&& m_bSpeedPts == rhs.m_bSpeedPts
		&& m_bBonusTitlePts == rhs.m_bBonusTitlePts
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
		inTree->GetAttrib(L"Date", m_ValidFrom);
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_SCORING_VALIDFROM, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDFROM, msg.c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_SCORING_VALIDTO, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDTO, msg.c_str()));
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
		std::wstring msg(Localization()->InvalidDivLevel());
		msg += m_Division;
		msg += L"/";
		msg += m_Level;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_LEVEL, msg.c_str()));
		return false;
	}

	std::wstring attrib;
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
		std::wstring msg(Localization()->ValidValues());
		msg += SCORING_TYPE_FT;
		msg += L", ";
		msg += SCORING_TYPE_FT100;
		msg += L", ";
		msg += SCORING_TYPE_FT200;
		msg += L", ";
		msg += SCORING_TYPE_OCT;
		msg += L", ";
		msg += SCORING_TYPE_ST;
		msg += L", ";
		msg += SCORING_TYPE_TF;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE, msg.c_str()));
		return false;
	}

	// This attribute came in in version 4, but go ahead and read it even if
	// the current doc is earlier. This will allow someone to hand-edit it in.
	// Not advisable, but it doesn't hurt anything either.
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DROPFRACTIONS, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q, m_bCleanQ))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_CLEAN_Q, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_UNDER, m_bTimeFaultsUnder))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_UNDER, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_OVER, m_bTimeFaultsOver))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_OVER, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_TIMEFAULTS_TITLING_PTS, m_bTitlingPointsRawFaults))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_TITLING_PTS, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, m_bSubtractTimeFaults))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	inTree->GetAttrib(ATTRIB_SCORING_TF_MULTIPLIER, m_TimeFaultMultiplier);

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUPERQ, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (inVersion < ARBVersion(11, 0))
	{
		if (ElementNode::eInvalidValue == inTree->GetAttrib(L"doubleQ", m_bDoubleQ))
		{
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, L"doubleQ", Localization()->ValidValuesBool().c_str()));
			return false;
		}
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_SPEEDPTS, m_bSpeedPts))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SPEEDPTS, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_bBonusTitlePts))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_BONUSTITLEPTS, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	bool bVer10orMore = (inVersion >= ARBVersion(10, 0));
	if (inVersion >= ARBVersion(5,0))
	{
		if (inVersion < ARBVersion(10, 1))
		{
			if (ElementNode::eInvalidValue == inTree->GetAttrib(L"machPts", m_bSpeedPts))
			{
				ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, L"machPts", Localization()->ValidValuesBool().c_str()));
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
			else if (element->GetName() == TREE_LIFETIME_POINTS
			|| element->GetName() == L"LifeTime") // Changed in v14.4
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
		if (ElementNode::eFound != inTree->GetAttrib(L"Clean", ptsWhenClean))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SCORING, L"Clean"));
			return false;
		}
		if (0 < ptsWhenClean)
		{
			m_TitlePoints.AddTitlePoints(ptsWhenClean, 0.0);
		}

		short faultsAllowed = 0;
		short ptsWhenNotClean = 0;
		inTree->GetAttrib(L"FaultsAllowed", faultsAllowed);
		inTree->GetAttrib(L"WithFaults", ptsWhenNotClean);
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
#if defined(__WXWINDOWS__)
		wxLogError(L"%s", Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE));
#else
		OutputDebugString(Localization()->ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE).c_str());
		assert(0);
#endif
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
	if (m_bTitlingPointsRawFaults)
		scoring->AddAttrib(ATTRIB_SCORING_TIMEFAULTS_TITLING_PTS, m_bTitlingPointsRawFaults);
	if (m_bSubtractTimeFaults)
		scoring->AddAttrib(ATTRIB_SCORING_SUBTRACT_TIMEFAULTS, m_bSubtractTimeFaults);
	// Don't write this if it's 1 for backwards compatibility.
	// It wouldn't hurt to write it, but it causes changes in almost every
	// scoring configuration otherwise.
	if (0 <= m_TimeFaultMultiplier && 1 != m_TimeFaultMultiplier)
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
	if (m_bBonusTitlePts)
		scoring->AddAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_bBonusTitlePts);
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
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
#ifdef _DEBUG
		size_t wildcard = 0;
#endif
		ARBVector<ARBConfigScoringPtr>::iterator iter;
		for (iter = items.begin(); iter != items.end(); )
		{
			ARBConfigScoringPtr pScoring = *iter;
			if (!pScoring->IsValidOn(inDate))
				iter = items.erase(iter);
			else
			{
#ifdef _DEBUG
				if (pScoring->GetDivision() == WILDCARD_DIVISION
				|| pScoring->GetLevel() == WILDCARD_LEVEL)
					++wildcard;
#endif
				++iter;
			}
		}
		if (1 == items.size())
			pEvent = *(items.begin());
		else if (1 < items.size())
		{
			// Umm, this means they have items with overlapping ranges...
			// Which may occur when creating the methods.
#ifdef _DEBUG
			// If date is not valid, we will have multiple items.
			if (inDate.IsValid() && items.size() - wildcard > 1)
			{
#if defined(__WXWINDOWS__)
				wxLogWarning(L"FindEvent: Overlapping date ranges");
#else
				OutputDebugString(L"FindEvent: Overlapping date ranges\n");
				assert(0);
#endif
			}
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
		std::wstring const& inDivision,
		std::wstring const& inLevel,
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
