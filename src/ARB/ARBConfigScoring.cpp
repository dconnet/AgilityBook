/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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

#include "StdAfx.h"
#include "ARBConfigScoring.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

std::string ARBConfigScoring::GetScoringStyleStr(ScoringStyle inStyle)
{
	std::string style;
	switch (inStyle)
	{
	default:
	case eUnknown:
		style = SCORE_STYLE_UNKNOWN;
		break;
	case eFaultsThenTime:
		style = SCORE_STYLE_FAULTSTIME;
		break;
	case eFaults100ThenTime:
		style = SCORE_STYLE_FAULTS100TIME;
		break;
	case eFaults200ThenTime:
		style = SCORE_STYLE_FAULTS200TIME;
		break;
	case eOCScoreThenTime:
		style = SCORE_STYLE_OCSCORETIME;
		break;
	case eScoreThenTime:
		style = SCORE_STYLE_SCORETIME;
		break;
	case eTimePlusFaults:
		style = SCORE_STYLE_TIMEPLUSFAULTS;
		break;
	}
	return style;
}

ARBConfigScoring::ARBConfigScoring()
	: m_ValidFrom()
	, m_ValidTo()
	, m_Division()
	, m_Level()
	, m_Style(eUnknown)
	, m_bDropFractions(false)
	, m_bTimeFaultsUnder(false)
	, m_bTimeFaultsOver(false)
	, m_TimeFaultMultiplier(1)
	, m_Note()
	, m_OpeningPts(0)
	, m_ClosingPts(0)
	, m_bSuperQ(false)
	, m_bDoubleQ(false)
	, m_bSpeedPts(false)
	, m_TitlePoints()
	, m_LifePoints()
{
}

ARBConfigScoring::ARBConfigScoring(ARBConfigScoring const& rhs)
	: m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Style(rhs.m_Style)
	, m_bDropFractions(rhs.m_bDropFractions)
	, m_bTimeFaultsUnder(rhs.m_bTimeFaultsUnder)
	, m_bTimeFaultsOver(rhs.m_bTimeFaultsOver)
	, m_TimeFaultMultiplier(rhs.m_TimeFaultMultiplier)
	, m_Note(rhs.m_Note)
	, m_OpeningPts(rhs.m_OpeningPts)
	, m_ClosingPts(rhs.m_ClosingPts)
	, m_bSuperQ(rhs.m_bSuperQ)
	, m_bDoubleQ(rhs.m_bDoubleQ)
	, m_bSpeedPts(rhs.m_bSpeedPts)
	, m_TitlePoints(rhs.m_TitlePoints)
	, m_LifePoints(rhs.m_LifePoints)
{
}

ARBConfigScoring::~ARBConfigScoring()
{
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
		m_bTimeFaultsUnder = rhs.m_bTimeFaultsUnder;
		m_bTimeFaultsOver = rhs.m_bTimeFaultsOver;
		m_TimeFaultMultiplier = rhs.m_TimeFaultMultiplier;
		m_Note = rhs.m_Note;
		m_OpeningPts = rhs.m_OpeningPts;
		m_ClosingPts = rhs.m_ClosingPts;
		m_bSuperQ = rhs.m_bSuperQ;
		m_bDoubleQ = rhs.m_bDoubleQ;
		m_bSpeedPts = rhs.m_bSpeedPts;
		m_TitlePoints = rhs.m_TitlePoints;
		m_LifePoints = rhs.m_LifePoints;
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
		&& m_bTimeFaultsUnder == rhs.m_bTimeFaultsUnder
		&& m_bTimeFaultsOver == rhs.m_bTimeFaultsOver
		&& m_TimeFaultMultiplier == rhs.m_TimeFaultMultiplier
		&& m_Note == rhs.m_Note
		&& m_OpeningPts == rhs.m_OpeningPts
		&& m_ClosingPts == rhs.m_ClosingPts
		&& m_bSuperQ == rhs.m_bSuperQ
		&& m_bDoubleQ == rhs.m_bDoubleQ
		&& m_bSpeedPts == rhs.m_bSpeedPts
		&& m_TitlePoints == rhs.m_TitlePoints
		&& m_LifePoints == rhs.m_LifePoints;
}

bool ARBConfigScoring::operator!=(ARBConfigScoring const& rhs) const
{
	return !operator==(rhs);
}

std::string ARBConfigScoring::GetGenericName() const
{
	std::string name = m_Division;
	name += " ";
	name += m_Level;
	return name;
}

size_t ARBConfigScoring::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigScoring::Load(
		ARBConfigDivisionList const& inDivisions,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	// Probably unnecessary since it isn't actually implemented yet!
	if (inVersion == ARBVersion(8, 0))
		inTree.GetAttrib("Date", m_ValidFrom);
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_SCORING_VALIDFROM, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDFROM, msg.c_str()));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_SCORING_VALIDTO, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDTO, msg.c_str()));
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_SCORING_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_DIVISION));
		return false;
	}

	if (Element::eFound != inTree.GetAttrib(ATTRIB_SCORING_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_LEVEL));
		return false;
	}
	if (!inDivisions.VerifyLevel(m_Division, m_Level))
	{
		std::string msg(INVALID_DIV_LEVEL);
		msg += m_Division;
		msg += "/";
		msg += m_Level;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_LEVEL, msg.c_str()));
		return false;
	}

	std::string attrib;
	if (Element::eFound != inTree.GetAttrib(ATTRIB_SCORING_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_TYPE));
		return false;
	}
	if (attrib == "FaultsThenTime")
		m_Style = eFaultsThenTime;
	else if (attrib == "Faults100ThenTime")
	{
		m_Style = eFaults100ThenTime;
		if (inVersion <= ARBVersion(3,0))
			m_bDropFractions = true;
	}
	else if (attrib == "Faults200ThenTime") // Version5.
		m_Style = eFaults200ThenTime;
	else if (attrib == "OCScoreThenTime")
		m_Style = eOCScoreThenTime;
	else if (attrib == "ScoreThenTime")
		m_Style = eScoreThenTime;
	else if (attrib == "TimePlusFaults")
		m_Style = eTimePlusFaults;
	else
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE, VALID_VALUES_SCORE));
		return false;
	}

	// This attribute came in in version 4, but go ahead and read it even if
	// the current doc is earlier. This will allow someone to hand-edit it in.
	// Not advisable, but it doesn't hurt anything either.
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DROPFRACTIONS, VALID_VALUES_BOOL));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_TIMEFAULTS_UNDER, m_bTimeFaultsUnder))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_UNDER, VALID_VALUES_BOOL));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_TIMEFAULTS_OVER, m_bTimeFaultsOver))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TIMEFAULTS_OVER, VALID_VALUES_BOOL));
		return false;
	}
	inTree.GetAttrib(ATTRIB_SCORING_TF_MULTIPLIER, m_TimeFaultMultiplier);

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUPERQ, VALID_VALUES_BOOL));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_DOUBLEQ, m_bDoubleQ))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DOUBLEQ, VALID_VALUES_BOOL));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_SPEEDPTS, m_bSpeedPts))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SPEEDPTS, VALID_VALUES_BOOL));
		return false;
	}

	bool bVer10orMore = (inVersion >= ARBVersion(10, 0));
	if (inVersion >= ARBVersion(5,0))
	{
		if (inVersion < ARBVersion(10, 1))
		{
			if (Element::eInvalidValue == inTree.GetAttrib("machPts", m_bSpeedPts))
			{
				ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, "machPts", VALID_VALUES_BOOL));
				return false;
			}
		}
		inTree.GetAttrib(ATTRIB_SCORING_OPENINGPTS, m_OpeningPts);
		inTree.GetAttrib(ATTRIB_SCORING_CLOSINGPTS, m_ClosingPts);
		for (int i = 0; i < inTree.GetElementCount(); ++i)
		{
			Element const& element = inTree.GetElement(i);
			if (element.GetName() == TREE_NOTE)
				m_Note = element.GetValue();
			else if (element.GetName() == TREE_TITLE_POINTS)
			{
				if (!m_TitlePoints.Load(element, inVersion, ioCallback, m_LifePoints))
					return false;
			}
			else if (element.GetName() == TREE_LIFETIME_POINTS)
			{
				if (bVer10orMore)
				{
					if (!m_LifePoints.Load(element, inVersion, ioCallback))
						return false;
				}
			}
		}
		m_TitlePoints.sort();
		m_LifePoints.sort();
	}

	// Migrate pre-5 files.
	else
	{
		short ptsWhenClean = 0;
		if (Element::eFound != inTree.GetAttrib("Clean", ptsWhenClean))
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_SCORING, "Clean"));
			return false;
		}
		if (0 < ptsWhenClean)
		{
			m_TitlePoints.AddTitlePoints(ptsWhenClean, 0);
		}

		short faultsAllowed = 0;
		short ptsWhenNotClean = 0;
		inTree.GetAttrib("FaultsAllowed", faultsAllowed);
		inTree.GetAttrib("WithFaults", ptsWhenNotClean);
		if (0 < faultsAllowed && 0 < ptsWhenNotClean)
		{
			m_TitlePoints.AddTitlePoints(ptsWhenNotClean, faultsAllowed);
		}

		if (inVersion >= ARBVersion(3,0))
			m_Note = inTree.GetValue();
	}

	return true;
}

bool ARBConfigScoring::Save(Element& ioTree) const
{
	Element& scoring = ioTree.AddElement(TREE_SCORING);
	if (m_ValidFrom.IsValid())
		scoring.AddAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		scoring.AddAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo);
	scoring.AddAttrib(ATTRIB_SCORING_DIVISION, m_Division);
	scoring.AddAttrib(ATTRIB_SCORING_LEVEL, m_Level);
	switch (m_Style)
	{
	default:
		ASSERT(0);
#ifdef _DEBUG
		TRACE("%s\n", ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE).c_str());
#endif
		return false;
	case eFaultsThenTime:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "FaultsThenTime");
		break;
	case eFaults100ThenTime:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "Faults100ThenTime");
		break;
	case eFaults200ThenTime:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "Faults200ThenTime");
		break;
	case eOCScoreThenTime:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "OCScoreThenTime");
		break;
	case eScoreThenTime:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "ScoreThenTime");
		break;
	case eTimePlusFaults:
		scoring.AddAttrib(ATTRIB_SCORING_TYPE, "TimePlusFaults");
		break;
	}
	if (m_bDropFractions)
		scoring.AddAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions);
	if (m_bTimeFaultsUnder)
		scoring.AddAttrib(ATTRIB_SCORING_TIMEFAULTS_UNDER, m_bTimeFaultsUnder);
	if (m_bTimeFaultsOver)
		scoring.AddAttrib(ATTRIB_SCORING_TIMEFAULTS_OVER, m_bTimeFaultsOver);
	if (1 < m_TimeFaultMultiplier)
		scoring.AddAttrib(ATTRIB_SCORING_TF_MULTIPLIER, m_TimeFaultMultiplier);
	if (0 < m_Note.length())
	{
		Element& note = scoring.AddElement(TREE_NOTE);
		note.SetValue(m_Note);
	}
	if (0 < m_OpeningPts)
		scoring.AddAttrib(ATTRIB_SCORING_OPENINGPTS, m_OpeningPts);
	if (0 < m_ClosingPts)
		scoring.AddAttrib(ATTRIB_SCORING_CLOSINGPTS, m_ClosingPts);
	if (m_bSuperQ)
		scoring.AddAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ);
	if (m_bDoubleQ)
		scoring.AddAttrib(ATTRIB_SCORING_DOUBLEQ, m_bDoubleQ);
	if (m_bSpeedPts)
		scoring.AddAttrib(ATTRIB_SCORING_SPEEDPTS, m_bSpeedPts);
	if (!m_TitlePoints.Save(scoring))
		return false;
	if (!m_LifePoints.Save(scoring))
		return false;
	return true;
}

std::string ARBConfigScoring::GetValidDateString() const
{
	std::string str;
	if (m_ValidFrom.IsValid() || m_ValidTo.IsValid())
	{
		str += "[";
		if (m_ValidFrom.IsValid())
			str += m_ValidFrom.GetString(ARBDate::eDashYMD).c_str();
		else
			str += "*";
		str += "-";
		if (m_ValidTo.IsValid())
			str += m_ValidTo.GetString(ARBDate::eDashYMD).c_str();
		else
			str += "*";
		str += "]";
	}
	return str;
}

std::string ARBConfigScoring::GetScoringStyleStr() const
{
	return GetScoringStyleStr(m_Style);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigScoringList::Load(
		ARBConfigDivisionList const& inDivisions,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigScoring* thing = new ARBConfigScoring();
	if (!thing->Load(inDivisions, inTree, inVersion, ioCallback))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

size_t ARBConfigScoringList::FindAllEvents(
		std::string const& inDivision,
		std::string const& inLevel,
		bool inTitlePoints,
		ARBVector<ARBConfigScoring>& outList) const
{
	outList.clear();
	const_iterator iter;
	for (iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == inDivision && (*iter)->GetLevel() == inLevel)
		{
			(*iter)->AddRef();
			outList.push_back(*iter);
		}
	}
	// It failed, try wildcards...
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == inDivision && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			(*iter)->AddRef();
			outList.push_back(*iter);
			break;
		}
	}
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == inLevel)
		{
			(*iter)->AddRef();
			outList.push_back(*iter);
			break;
		}
	}
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			(*iter)->AddRef();
			outList.push_back(*iter);
			break;
		}
	}

	if (inTitlePoints)
	{
		ARBVector<ARBConfigScoring>::iterator iter;
		for (iter = outList.begin(); iter != outList.end(); )
		{
			if (0 < (*iter)->GetTitlePoints().size())
				++iter;
			else
				iter = outList.erase(iter);
		}
	}
	return outList.size();
}

bool ARBConfigScoringList::FindEvent(
		std::string const& inDivision,
		std::string const& inLevel,
		ARBDate const& inDate,
		ARBConfigScoring** outEvent) const
{
	if (outEvent)
		*outEvent = NULL;
	ARBVector<ARBConfigScoring> items;
	FindAllEvents(inDivision, inLevel, false, items);
	if (0 == items.size())
		return false;
	ARBConfigScoring* pEvent = NULL;
	if (1 == items.size())
		pEvent = *(items.begin());
	else
	{
		ARBVector<ARBConfigScoring>::iterator iter;
		for (iter = items.begin(); iter != items.end(); )
		{
			ARBConfigScoring const* pScoring = *iter;
			ARBDate const& validFrom = pScoring->GetValidFrom();
			ARBDate const& validTo = pScoring->GetValidTo();
			if ((validFrom.IsValid() && inDate < validFrom)
			|| (validTo.IsValid() && inDate > validTo))
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
			TRACE0("Warning: Overlapping date ranges\n");
			pEvent = *(items.begin());
		}
	}
	if (pEvent)
	{
		if (outEvent)
		{
			*outEvent = pEvent;
			(*outEvent)->AddRef();
		}
		return true;
	}
	else
		return false;
}

bool ARBConfigScoringList::VerifyEvent(
		std::string const& inDivision,
		std::string const& inLevel) const
{
	ARBVector<ARBConfigScoring> items;
	FindAllEvents(inDivision, inLevel, false, items);
	return (0 < items.size());
}

ARBConfigScoring* ARBConfigScoringList::AddScoring()
{
	ARBConfigScoring* pScoring = new ARBConfigScoring();
	pScoring->SetDivision(WILDCARD_DIVISION);
	pScoring->SetLevel(WILDCARD_LEVEL);
	pScoring->SetScoringStyle(ARBConfigScoring::eFaultsThenTime);
	push_back(pScoring);
	pScoring->AddRef();
	return pScoring;
}
