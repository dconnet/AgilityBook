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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
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

ARBDogRunScoring::ARBDogRunScoring()
	: m_type(eTypeUnknown)
	, m_bRoundTimeFaults(false)
	, m_SCT(0)
	, m_Yards(0)
	, m_Time(0)
	, m_CourseFaults(0)
	, m_NeedOpenPts(0)
	, m_NeedClosePts(0)
	, m_OpenPts(0)
	, m_ClosePts(0)
{
}

ARBDogRunScoring::ARBDogRunScoring(const ARBDogRunScoring& rhs)
	: m_type(rhs.m_type)
	, m_bRoundTimeFaults(rhs.m_bRoundTimeFaults)
	, m_SCT(rhs.m_SCT)
	, m_Yards(rhs.m_Yards)
	, m_Time(rhs.m_Time)
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

ARBDogRunScoring& ARBDogRunScoring::operator=(const ARBDogRunScoring& rhs)
{
	if (this != &rhs)
	{
		m_type = rhs.m_type;
		m_bRoundTimeFaults = rhs.m_bRoundTimeFaults;
		m_SCT = rhs.m_SCT;
		m_Yards = rhs.m_Yards;
		m_Time = rhs.m_Time;
		m_CourseFaults = rhs.m_CourseFaults;
		m_NeedOpenPts = rhs.m_NeedOpenPts;
		m_NeedClosePts = rhs.m_NeedClosePts;
		m_OpenPts = rhs.m_OpenPts;
		m_ClosePts = rhs.m_ClosePts;
	}
	return *this;
}

bool ARBDogRunScoring::operator==(const ARBDogRunScoring& rhs) const
{
	return m_type == rhs.m_type
		&& m_bRoundTimeFaults == rhs.m_bRoundTimeFaults
		&& m_SCT == rhs.m_SCT
		&& m_Yards == rhs.m_Yards
		&& m_Time == rhs.m_Time
		&& m_CourseFaults == rhs.m_CourseFaults
		&& m_NeedOpenPts == rhs.m_NeedOpenPts
		&& m_NeedClosePts == rhs.m_NeedClosePts
		&& m_OpenPts == rhs.m_OpenPts
		&& m_ClosePts == rhs.m_ClosePts;
}

bool ARBDogRunScoring::operator!=(const ARBDogRunScoring& rhs) const
{
	return !operator==(rhs);
}

bool ARBDogRunScoring::Load(
	const ARBConfigScoring* inEvent,
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	std::string attrib;

	m_bRoundTimeFaults = inEvent->DropFractions();
	const std::string& name = inTree.GetName();
	double d;
	if (CElement::eFound == inTree.GetAttrib(ATTRIB_SCORING_TIME, d))
		m_Time = d;
	inTree.GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
	switch (inEvent->GetScoringStyle())
	{
	default:
		break;

	case ARBConfigScoring::eFaults100ThenTime:
	case ARBConfigScoring::eFaults200ThenTime:
	case ARBConfigScoring::eFaultsThenTime:
	case ARBConfigScoring::eTimePlusFaults:
		if (name == TREE_BY_TIME)
		{
			m_type = eTypeByTime;
			if (CElement::eFound == inTree.GetAttrib(ATTRIB_BY_TIME_SCT, d))
				m_SCT = d;
			inTree.GetAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
			return true;
		}
		break;

	case ARBConfigScoring::eOCScoreThenTime:
		if (name == TREE_BY_OPENCLOSE)
		{
			m_type = eTypeByOpenClose;
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
			inTree.GetAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
			return true;
		}
		break;

	case ARBConfigScoring::eScoreThenTime:
		if (name == TREE_BY_POINTS)
		{
			m_type = eTypeByPoints;
			inTree.GetAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
			inTree.GetAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
			return true;
		}
		break;
	}
	return false;
}

bool ARBDogRunScoring::Save(CElement& ioTree) const
{
	switch (m_type)
	{
	default:
		break;
	case eTypeByTime:
		{
			CElement& scoring = ioTree.AddElement(TREE_BY_TIME);
			scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			scoring.AddAttrib(ATTRIB_BY_TIME_SCT, m_SCT);
			scoring.AddAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
		}
		return true;
	case eTypeByOpenClose:
		{
			CElement& scoring = ioTree.AddElement(TREE_BY_OPENCLOSE);
			if (0 < m_CourseFaults)
				scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
			scoring.AddAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
		}
		return true;
	case eTypeByPoints:
		{
			CElement& scoring = ioTree.AddElement(TREE_BY_POINTS);
			if (0 < m_CourseFaults)
				scoring.AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			m_Time.Save(scoring, ATTRIB_SCORING_TIME);
			scoring.AddAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
			scoring.AddAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
		}
		return true;
	}
	return false;
}

double ARBDogRunScoring::GetTimeFaults() const
{
	double timeFaults = 0.0;
	if (ARBDogRunScoring::eTypeByTime == m_type)
	{
		double time = m_Time;
		if (m_bRoundTimeFaults)
			time = floor(m_Time);
		if (0.0 < m_SCT && time > m_SCT)
			timeFaults = time - m_SCT;
	}
	return timeFaults;
}
