/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
	, m_Note()
	, m_OpeningPts(0)
	, m_ClosingPts(0)
	, m_bSuperQ(false)
	, m_bDoubleQ(false)
	, m_bMachPts(false)
	, m_TitlePoints()
{
}

ARBConfigScoring::ARBConfigScoring(const ARBConfigScoring& rhs)
	: m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Division(rhs.m_Division)
	, m_Level(rhs.m_Level)
	, m_Style(rhs.m_Style)
	, m_bDropFractions(rhs.m_bDropFractions)
	, m_Note(rhs.m_Note)
	, m_OpeningPts(rhs.m_OpeningPts)
	, m_ClosingPts(rhs.m_ClosingPts)
	, m_bSuperQ(rhs.m_bSuperQ)
	, m_bDoubleQ(rhs.m_bDoubleQ)
	, m_bMachPts(rhs.m_bMachPts)
	, m_TitlePoints(rhs.m_TitlePoints)
{
}

ARBConfigScoring::~ARBConfigScoring()
{
}

ARBConfigScoring& ARBConfigScoring::operator=(const ARBConfigScoring& rhs)
{
	if (this != &rhs)
	{
		m_ValidFrom = rhs.m_ValidFrom;
		m_ValidTo = rhs.m_ValidTo;
		m_Division = rhs.m_Division;
		m_Level = rhs.m_Level;
		m_Style = rhs.m_Style;
		m_bDropFractions = rhs.m_bDropFractions;
		m_Note = rhs.m_Note;
		m_OpeningPts = rhs.m_OpeningPts;
		m_ClosingPts = rhs.m_ClosingPts;
		m_bSuperQ = rhs.m_bSuperQ;
		m_bDoubleQ = rhs.m_bDoubleQ;
		m_bMachPts = rhs.m_bMachPts;
		m_TitlePoints = rhs.m_TitlePoints;
	}
	return *this;
}

bool ARBConfigScoring::operator==(const ARBConfigScoring& rhs) const
{
	return m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_Division == rhs.m_Division
		&& m_Level == rhs.m_Level
		&& m_Style == rhs.m_Style
		&& m_bDropFractions == rhs.m_bDropFractions
		&& m_Note == rhs.m_Note
		&& m_OpeningPts == rhs.m_OpeningPts
		&& m_ClosingPts == rhs.m_ClosingPts
		&& m_bSuperQ == rhs.m_bSuperQ
		&& m_bDoubleQ == rhs.m_bDoubleQ
		&& m_bMachPts == rhs.m_bMachPts
		&& m_TitlePoints == rhs.m_TitlePoints;
}

bool ARBConfigScoring::operator!=(const ARBConfigScoring& rhs) const
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

bool ARBConfigScoring::Load(
	const ARBConfigDivisionList& inDivisions,
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	// Probably unnecessary since it isn't actually implemented yet!
	if (inVersion == ARBVersion(8, 0))
		inTree.GetAttrib("Date", m_ValidFrom);
	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_SCORING_VALIDFROM, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDFROM, msg.c_str());
		return false;
	}
	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_SCORING_VALIDTO, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_VALIDTO, msg.c_str());
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_SCORING_DIVISION, m_Division)
	|| 0 == m_Division.length())
	{
		ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_DIVISION);
		return false;
	}

	if (CElement::eFound != inTree.GetAttrib(ATTRIB_SCORING_LEVEL, m_Level)
	|| 0 == m_Level.length())
	{
		ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_LEVEL);
		return false;
	}
	if (!inDivisions.VerifyLevel(m_Division, m_Level))
	{
		std::string msg(INVALID_DIV_LEVEL);
		msg += m_Division;
		msg += "/";
		msg += m_Level;
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_LEVEL, msg.c_str());
		return false;
	}

	std::string attrib;
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_SCORING_TYPE, attrib)
	|| 0 == attrib.length())
	{
		ErrorMissingAttribute(TREE_SCORING, ATTRIB_SCORING_TYPE);
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
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE, VALID_VALUES_SCORE);
		return false;
	}

	// This attribute came in in version 4, but go ahead and read it even if
	// the current doc is earlier. This will allow someone to hand-edit it in.
	// Not advisable, but it doesn't hurt anything either.
	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_DROPFRACTIONS, m_bDropFractions))
	{
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DROPFRACTIONS, VALID_VALUES_BOOL);
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_SUPERQ, m_bSuperQ))
	{
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_SUPERQ, VALID_VALUES_BOOL);
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_DOUBLEQ, m_bDoubleQ))
	{
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_DOUBLEQ, VALID_VALUES_BOOL);
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_MACHPTS, m_bMachPts))
	{
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_MACHPTS, VALID_VALUES_BOOL);
		return false;
	}

	if (inVersion >= ARBVersion(5,0))
	{
		inTree.GetAttrib(ATTRIB_SCORING_OPENINGPTS, m_OpeningPts);
		inTree.GetAttrib(ATTRIB_SCORING_CLOSINGPTS, m_ClosingPts);
		for (int i = 0; i < inTree.GetElementCount(); ++i)
		{
			const CElement& element = inTree.GetElement(i);
			if (element.GetName() == TREE_NOTE)
				m_Note = element.GetValue();
			else if (element.GetName() == TREE_TITLE_POINTS)
			{
				if (!m_TitlePoints.Load(element, inVersion))
					return false;
			}
		}
		m_TitlePoints.sort();
	}

	// Migrate pre-5 files.
	else
	{
		short ptsWhenClean = 0;
		if (CElement::eFound != inTree.GetAttrib("Clean", ptsWhenClean))
		{
			ErrorMissingAttribute(TREE_SCORING, "Clean");
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

bool ARBConfigScoring::Save(CElement& ioTree) const
{
	CElement& scoring = ioTree.AddElement(TREE_SCORING);
	if (m_ValidFrom.IsValid())
		scoring.AddAttrib(ATTRIB_SCORING_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		scoring.AddAttrib(ATTRIB_SCORING_VALIDTO, m_ValidTo);
	scoring.AddAttrib(ATTRIB_SCORING_DIVISION, m_Division);
	scoring.AddAttrib(ATTRIB_SCORING_LEVEL, m_Level);
	switch (m_Style)
	{
	default:
		ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_TYPE);
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
	if (0 < m_Note.length())
	{
		CElement& note = scoring.AddElement(TREE_NOTE);
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
	if (m_bMachPts)
		scoring.AddAttrib(ATTRIB_SCORING_MACHPTS, m_bMachPts);
	if (!m_TitlePoints.Save(scoring))
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
			str += m_ValidFrom.GetString(false, false).c_str();
		else
			str += "*";
		str += "-";
		if (m_ValidTo.IsValid())
			str += m_ValidTo.GetString(false, false).c_str();
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
	const ARBConfigDivisionList& inDivisions,
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	ARBConfigScoring* thing = new ARBConfigScoring;
	if (!thing->Load(inDivisions, inTree, inVersion))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

size_t ARBConfigScoringList::FindAllEvents(
	const std::string& inDivision,
	const std::string& inLevel,
	bool inTitlePoints,
	std::vector<const ARBConfigScoring*>& outList) const
{
	outList.clear();
	const_iterator iter;
	for (iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == inDivision && (*iter)->GetLevel() == inLevel)
			outList.push_back(*iter);
	}
	// It failed, try wildcards...
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == inDivision && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			outList.push_back(*iter);
			break;
		}
	}
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == inLevel)
		{
			outList.push_back(*iter);
			break;
		}
	}
	for (iter = begin(); 0 == outList.size() && iter != end(); ++iter)
	{
		if ((*iter)->GetDivision() == WILDCARD_DIVISION && (*iter)->GetLevel() == WILDCARD_LEVEL)
		{
			outList.push_back(*iter);
			break;
		}
	}

	if (inTitlePoints)
	{
		std::vector<const ARBConfigScoring*>::iterator iter;
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

const ARBConfigScoring* ARBConfigScoringList::FindEvent(
	const std::string& inDivision,
	const std::string& inLevel,
	const ARBDate& inDate) const
{
	std::vector<const ARBConfigScoring*> items;
	FindAllEvents(inDivision, inLevel, false, items);
	if (0 == items.size())
		return NULL;
	else if (1 == items.size())
		return *(items.begin());
	else
	{
		std::vector<const ARBConfigScoring*>::iterator iter;
		for (iter = items.begin(); iter != items.end(); )
		{
			const ARBConfigScoring* pScoring = *iter;
			const ARBDate& validFrom = pScoring->GetValidFrom();
			const ARBDate& validTo = pScoring->GetValidTo();
			if ((validFrom.IsValid() && inDate < validFrom)
			|| (validTo.IsValid() && inDate > validTo))
				iter = items.erase(iter);
			else
				++iter;
		}
		if (0 == items.size())
			return NULL;
		else if (1 == items.size())
			return *(items.begin());
		else
		{
			// Umm, this means they have items with overlapping ranges...
			// Which may occur when creating the methods.
			TRACE0("Warning: Overlapping date ranges\n");
			return *(items.begin());
		}
	}
}

ARBConfigScoring* ARBConfigScoringList::AddScoring()
{
	ARBConfigScoring* pScoring = new ARBConfigScoring;
	pScoring->SetDivision(WILDCARD_DIVISION);
	pScoring->SetLevel(WILDCARD_LEVEL);
	pScoring->SetScoringStyle(ARBConfigScoring::eFaultsThenTime);
	push_back(pScoring);
	return pScoring;
}
