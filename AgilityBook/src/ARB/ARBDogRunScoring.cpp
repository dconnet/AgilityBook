/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-09-01 DRC Fix a file that may have been corrupted (see history.txt)
 * @li 2004-03-26 DRC File version 8.6. Changed Table-in-YPS to hasTable.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-13 DRC Made Time/CourseFaults common to all scoring methods.
 */

#include "StdAfx.h"
#include <math.h>
#include "ARBDogRunScoring.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// static

ARBDogRunScoring::ScoringType ARBDogRunScoring::TranslateConfigScoring(
	ARBConfigScoring::ScoringStyle inType)
{
	switch (inType)
	{
	default:
		ASSERT(0);
	case ARBConfigScoring::eFaultsThenTime:
	case ARBConfigScoring::eFaults100ThenTime:
	case ARBConfigScoring::eFaults200ThenTime:
	case ARBConfigScoring::eTimePlusFaults:
		return ARBDogRunScoring::eTypeByTime;
	case ARBConfigScoring::eOCScoreThenTime:
		return ARBDogRunScoring::eTypeByOpenClose;
	case ARBConfigScoring::eScoreThenTime:
		return ARBDogRunScoring::eTypeByPoints;
	}
}

/////////////////////////////////////////////////////////////////////////////

ARBDogRunScoring::ARBDogRunScoring()
	: m_type(eTypeUnknown)
	, m_bRoundTimeFaults(false)
	, m_SCT(0)
	, m_SCT2(0)
	, m_Yards(0)
	, m_Time(0)
	, m_Table(false)
	, m_ConvertTable(false)
	, m_CourseFaults(0)
	, m_NeedOpenPts(0)
	, m_NeedClosePts(0)
	, m_OpenPts(0)
	, m_ClosePts(0)
{
}

ARBDogRunScoring::ARBDogRunScoring(ARBDogRunScoring const& rhs)
	: m_type(rhs.m_type)
	, m_bRoundTimeFaults(rhs.m_bRoundTimeFaults)
	, m_SCT(rhs.m_SCT)
	, m_SCT2(rhs.m_SCT2)
	, m_Yards(rhs.m_Yards)
	, m_Time(rhs.m_Time)
	, m_Table(rhs.m_Table)
	, m_ConvertTable(rhs.m_Table)
	, m_CourseFaults(rhs.m_CourseFaults)
	, m_NeedOpenPts(rhs.m_NeedOpenPts)
	, m_NeedClosePts(rhs.m_NeedClosePts)
	, m_OpenPts(rhs.m_OpenPts)
	, m_ClosePts(rhs.m_ClosePts)
{
}

ARBDogRunScoring::~ARBDogRunScoring()
{
}

ARBDogRunScoring& ARBDogRunScoring::operator=(ARBDogRunScoring const& rhs)
{
	if (this != &rhs)
	{
		m_type = rhs.m_type;
		m_bRoundTimeFaults = rhs.m_bRoundTimeFaults;
		m_SCT = rhs.m_SCT;
		m_SCT2 = rhs.m_SCT2;
		m_Yards = rhs.m_Yards;
		m_Time = rhs.m_Time;
		m_Table = rhs.m_Table;
		m_CourseFaults = rhs.m_CourseFaults;
		m_NeedOpenPts = rhs.m_NeedOpenPts;
		m_NeedClosePts = rhs.m_NeedClosePts;
		m_OpenPts = rhs.m_OpenPts;
		m_ClosePts = rhs.m_ClosePts;
	}
	return *this;
}

bool ARBDogRunScoring::operator==(ARBDogRunScoring const& rhs) const
{
	return m_type == rhs.m_type
		&& m_bRoundTimeFaults == rhs.m_bRoundTimeFaults
		&& m_SCT == rhs.m_SCT
		&& m_SCT2 == rhs.m_SCT2
		&& m_Yards == rhs.m_Yards
		&& m_Time == rhs.m_Time
		&& m_Table == rhs.m_Table
		&& m_CourseFaults == rhs.m_CourseFaults
		&& m_NeedOpenPts == rhs.m_NeedOpenPts
		&& m_NeedClosePts == rhs.m_NeedClosePts
		&& m_OpenPts == rhs.m_OpenPts
		&& m_ClosePts == rhs.m_ClosePts;
}

bool ARBDogRunScoring::operator!=(ARBDogRunScoring const& rhs) const
{
	return !operator==(rhs);
}

bool ARBDogRunScoring::Load(
	ARBConfigScoring const* inEvent,
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	std::string attrib;

	m_bRoundTimeFaults = inEvent->DropFractions();
	std::string const& name = inTree.GetName();
	double d;
	if (Element::eFound == inTree.GetAttrib(ATTRIB_SCORING_TIME, d))
		m_Time = d;
	inTree.GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
	m_type = ARBDogRunScoring::TranslateConfigScoring(inEvent->GetScoringStyle());
	switch (m_type)
	{
	default:
		break;

	case eTypeByTime:
		if (name == TREE_BY_TIME)
		{
			if (inVersion < ARBVersion(8,6))
			{
				// File version 8.4 and 8.5 stored whether to adjust the YPS.
				// If false, the YPS would be adjusted - this means there was
				// a table, so migrate that information. If true, we don't know
				// if they set the value, or it's the default - so set up a
				// conversion. This is used when merging the new configuration.
				// If the existing config is <=2 and the new one is 3 (only),
				// then we invoke the conversion. This is handled from the UI
				// level.
				m_ConvertTable = true;
				bool bTableInYPS = true;
				if (Element::eFound == inTree.GetAttrib("TableInYPS", bTableInYPS))
				{
					if (!bTableInYPS)
					{
						m_Table = true;
						m_ConvertTable = false;
					}
				}
			}
			else
			{
				if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_SCORING_HAS_TABLE, m_Table))
				{
					ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_SCORING, ATTRIB_SCORING_HAS_TABLE, VALID_VALUES_BOOL));
					// Report the error, but keep going.
					m_Table = false;
				}
			}
			if (Element::eFound == inTree.GetAttrib(ATTRIB_SCORING_SCT, d))
				m_SCT = d;
			inTree.GetAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
			return true;
		}
		break;

	case eTypeByOpenClose:
		if (name == TREE_BY_OPENCLOSE)
		{
			if (Element::eFound == inTree.GetAttrib(ATTRIB_SCORING_SCT, d))
				m_SCT = d;
			if (Element::eFound == inTree.GetAttrib(ATTRIB_SCORING_SCT2, d))
				m_SCT2 = d;
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
			return true;
		}
		break;

	case eTypeByPoints:
		if (name == TREE_BY_POINTS)
		{
			if (Element::eFound == inTree.GetAttrib(ATTRIB_SCORING_SCT, d))
				m_SCT = d;
			inTree.GetAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
			inTree.GetAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
			return true;
		}
		break;
	}
	return false;
}

bool ARBDogRunScoring::Save(Element& ioTree) const
{
	switch (m_type)
	{
	default:
		break;
	case eTypeByTime:
		{
			Element& scoring = ioTree.AddElement(TREE_BY_TIME);
			if (m_Table) // Default is no
				scoring.AddAttrib(ATTRIB_SCORING_HAS_TABLE, m_Table);
			scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			scoring.AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
			scoring.AddAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
		}
		return true;
	case eTypeByOpenClose:
		{
			Element& scoring = ioTree.AddElement(TREE_BY_OPENCLOSE);
			if (0 < m_CourseFaults)
				scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			if (0 < m_SCT)
				scoring.AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
			if (0 < m_SCT2)
				scoring.AddAttrib(ATTRIB_SCORING_SCT2, m_SCT2);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
		}
		return true;
	case eTypeByPoints:
		{
			Element& scoring = ioTree.AddElement(TREE_BY_POINTS);
			if (0 < m_CourseFaults)
				scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			if (0 < m_SCT)
				scoring.AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
			scoring.AddAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
			scoring.AddAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
		}
		return true;
	}
	return false;
}

bool ARBDogRunScoring::GetYPS(bool inTableInYPS, double& outYPS) const
{
	bool bOk = false;
	if (eTypeByTime == GetType()
	&& 0 < GetYards() && 0.0 < GetTime())
	{
		bOk = true;
		double t = GetTime();
		if (HasTable() && 5.0 < t && !inTableInYPS)
			t -= 5;
		outYPS = GetYards() / t;
	}
	return bOk;
}

double ARBDogRunScoring::GetTimeFaults(ARBConfigScoring const* inScoring) const
{
	double timeFaults = 0.0;
	if (ARBDogRunScoring::eTypeByTime == m_type)
	{
		bool bAddTimeFaults = true;
		if (inScoring && ARBConfigScoring::eTimePlusFaults == inScoring->GetScoringStyle())
			bAddTimeFaults = false;
		if (bAddTimeFaults)
		{
			double time = m_Time;
			if (m_bRoundTimeFaults)
				time = floor(m_Time);
			if (0.0 < m_SCT && time > m_SCT)
				timeFaults = time - m_SCT;
		}
	}
	return timeFaults;
}
