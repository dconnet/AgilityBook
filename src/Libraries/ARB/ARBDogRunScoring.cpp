/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * 2012-07-04 Add option to use run time or opening time in gamble OPS.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2007-07-01 Fixed a problem with table flag on a run.
 * 2007-02-14 Fixed a problem in YPS table file conversion.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2004-11-15 Changed time fault computation on T+F events.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-09-01 Fix a file that may have been corrupted (see history.txt)
 * 2004-03-26 File version 8.6. Changed Table-in-YPS to hasTable.
 * 2004-02-14 Added Table-in-YPS flag.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-13 Made Time/CourseFaults common to all scoring methods.
 */

#include "stdafx.h"
#include "ARB/ARBDogRunScoring.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <math.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace
{
// Magic / time == MPH
// This number is from the AKC rule book.
// Note: My computed MPH is often lower than the official one. WTF Excel?
// (not excel: putting in "=204.545 / A1" results in the numbers I have.
constexpr double sc_magicMPH = 204.545;
} // namespace


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

/////////////////////////////////////////////////////////////////////////////
// static

ARBScoringType ARBDogRunScoring::TranslateConfigScoring(ARBScoringStyle inType)
{
	switch (inType)
	{
	case ARBScoringStyle::Unknown:
		assert(0);
		return ARBScoringType::Unknown;
	case ARBScoringStyle::FaultsThenTime:
	case ARBScoringStyle::Faults100ThenTime:
	case ARBScoringStyle::Faults200ThenTime:
	case ARBScoringStyle::TimePlusFaults:
		return ARBScoringType::ByTime;
	case ARBScoringStyle::OCScoreThenTime:
		return ARBScoringType::ByOpenClose;
	case ARBScoringStyle::ScoreThenTime:
		return ARBScoringType::ByPoints;
	case ARBScoringStyle::TimeNoPlaces:
		return ARBScoringType::BySpeed;
	case ARBScoringStyle::PassFail:
		return ARBScoringType::ByPass;
	}
	// 'enum class' handles all cases via the switch above
	return ARBScoringType::Unknown;
}

/////////////////////////////////////////////////////////////////////////////

ARBDogRunScoring::ARBDogRunScoring()
	: m_type(ARBScoringType::Unknown)
	, m_bRoundTimeFaults(false)
	, m_SCT(0.0)
	, m_SCT2(0.0)
	, m_Yards(0.0)
	, m_Obstacles(0)
	, m_Time(0.0)
	, m_Table(false)
	, m_ConvertTable(false)
	, m_CourseFaults(0)
	, m_NeedOpenPts(0)
	, m_NeedClosePts(0)
	, m_OpenPts(0)
	, m_ClosePts(0)
	, m_BonusTitlePts(0.0)
{
}


ARBDogRunScoring::ARBDogRunScoring(ARBDogRunScoring const& rhs)
	: m_type(rhs.m_type)
	, m_bRoundTimeFaults(rhs.m_bRoundTimeFaults)
	, m_SCT(rhs.m_SCT)
	, m_SCT2(rhs.m_SCT2)
	, m_Yards(rhs.m_Yards)
	, m_Obstacles(rhs.m_Obstacles)
	, m_Time(rhs.m_Time)
	, m_Table(rhs.m_Table)
	, m_ConvertTable(rhs.m_ConvertTable)
	, m_CourseFaults(rhs.m_CourseFaults)
	, m_NeedOpenPts(rhs.m_NeedOpenPts)
	, m_NeedClosePts(rhs.m_NeedClosePts)
	, m_OpenPts(rhs.m_OpenPts)
	, m_ClosePts(rhs.m_ClosePts)
	, m_BonusTitlePts(rhs.m_BonusTitlePts)
{
}


ARBDogRunScoring::ARBDogRunScoring(ARBDogRunScoring&& rhs)
	: m_type(std::move(rhs.m_type))
	, m_bRoundTimeFaults(std::move(rhs.m_bRoundTimeFaults))
	, m_SCT(std::move(rhs.m_SCT))
	, m_SCT2(std::move(rhs.m_SCT2))
	, m_Yards(std::move(rhs.m_Yards))
	, m_Obstacles(std::move(rhs.m_Obstacles))
	, m_Time(std::move(rhs.m_Time))
	, m_Table(std::move(rhs.m_Table))
	, m_ConvertTable(std::move(rhs.m_ConvertTable))
	, m_CourseFaults(std::move(rhs.m_CourseFaults))
	, m_NeedOpenPts(std::move(rhs.m_NeedOpenPts))
	, m_NeedClosePts(std::move(rhs.m_NeedClosePts))
	, m_OpenPts(std::move(rhs.m_OpenPts))
	, m_ClosePts(std::move(rhs.m_ClosePts))
	, m_BonusTitlePts(std::move(rhs.m_BonusTitlePts))
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
		m_Obstacles = rhs.m_Obstacles;
		m_Time = rhs.m_Time;
		m_Table = rhs.m_Table;
		m_CourseFaults = rhs.m_CourseFaults;
		m_NeedOpenPts = rhs.m_NeedOpenPts;
		m_NeedClosePts = rhs.m_NeedClosePts;
		m_OpenPts = rhs.m_OpenPts;
		m_ClosePts = rhs.m_ClosePts;
		m_BonusTitlePts = rhs.m_BonusTitlePts;
	}
	return *this;
}


ARBDogRunScoring& ARBDogRunScoring::operator=(ARBDogRunScoring&& rhs)
{
	if (this != &rhs)
	{
		m_type = std::move(rhs.m_type);
		m_bRoundTimeFaults = std::move(rhs.m_bRoundTimeFaults);
		m_SCT = std::move(rhs.m_SCT);
		m_SCT2 = std::move(rhs.m_SCT2);
		m_Yards = std::move(rhs.m_Yards);
		m_Obstacles = std::move(rhs.m_Obstacles);
		m_Time = std::move(rhs.m_Time);
		m_Table = std::move(rhs.m_Table);
		m_CourseFaults = std::move(rhs.m_CourseFaults);
		m_NeedOpenPts = std::move(rhs.m_NeedOpenPts);
		m_NeedClosePts = std::move(rhs.m_NeedClosePts);
		m_OpenPts = std::move(rhs.m_OpenPts);
		m_ClosePts = std::move(rhs.m_ClosePts);
		m_BonusTitlePts = std::move(rhs.m_BonusTitlePts);
	}
	return *this;
}


bool ARBDogRunScoring::operator==(ARBDogRunScoring const& rhs) const
{
	// clang-format off
	return m_type == rhs.m_type
		&& m_bRoundTimeFaults == rhs.m_bRoundTimeFaults
		&& m_SCT == rhs.m_SCT
		&& m_SCT2 == rhs.m_SCT2
		&& m_Yards == rhs.m_Yards
		&& m_Obstacles == rhs.m_Obstacles
		&& m_Time == rhs.m_Time
		&& m_Table == rhs.m_Table
		&& m_CourseFaults == rhs.m_CourseFaults
		&& m_NeedOpenPts == rhs.m_NeedOpenPts
		&& m_NeedClosePts == rhs.m_NeedClosePts
		&& m_OpenPts == rhs.m_OpenPts
		&& m_ClosePts == rhs.m_ClosePts
		&& m_BonusTitlePts == rhs.m_BonusTitlePts;
	// clang-format on
}


bool ARBDogRunScoring::Load(
	short inConfigVersion,
	ARBConfigEventPtr const& inEvent,
	ARBConfigScoringPtr const& inEventScoring,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	std::wstring const& name = inTree->GetName();
	if (!inTree
		|| !(
			name == TREE_BY_TIME || name == TREE_BY_OPENCLOSE || name == TREE_BY_POINTS || name == TREE_BY_SPEED
			|| name == TREE_BY_PASS))
		return false;

	m_bRoundTimeFaults = inEventScoring->DropFractions();
	m_type = ARBDogRunScoring::TranslateConfigScoring(inEventScoring->GetScoringStyle());
	switch (m_type)
	{
	case ARBScoringType::Unknown:
		break;

	case ARBScoringType::ByTime:
		if (name == TREE_BY_TIME)
		{
			if (inVersion < ARBVersion(8, 6))
			{
				// File version 8.4 and 8.5 stored whether to adjust the YPS.
				// If false, the YPS would be adjusted - this means there was
				// a table, so migrate that information. If true, we don't know
				// if they set the value, or it's the default - so set up a
				// conversion. This is used when merging the new configuration.
				// If the existing config is <=2 and the new one is 3 (only),
				// then we invoke the conversion. This is handled from the UI
				// level. [note, changed to handle conversion on newone == 3+]
				m_ConvertTable = true;
				bool bTableInYPS = true;
				if (ARBAttribLookup::Found == inTree->GetAttrib(L"TableInYPS", bTableInYPS))
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
				if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_SCORING_HAS_TABLE, m_Table))
				{
					ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
						TREE_SCORING,
						ATTRIB_SCORING_HAS_TABLE,
						Localization()->ValidValuesBool().c_str()));
					// Report the error, but keep going.
					m_Table = false;
				}
			}
			// There was a problem where the table was being set in pre 12.4
			// files. I suspect this was due to having the table flag set on
			// non-table runs in 8.4 files - then when converted above, the
			// table flag was set - but never fixed in the cleanup code in
			// ARBAgilityRecordBook::Update (that code has been updated too)
			// Besides, the Update code only ran for v2->3 configurations.
			// If we convert directly from v2 to v4+, we'd still need to do it.
			//
			// Note 7/1/07: Discovered another way table flag can be set wrong.
			// If you take an existing run with a table and change it to a non
			// table run, the table flag was retained (thru v1.9.1.12).
			// v1.9.2 == 12.6file
			if (m_Table && inVersion < ARBVersion(12, 6) && inConfigVersion >= 3)
			{
				// Only do this if the config is higher than 2. Otherwise, when
				// we read the v2 config, the table bit is cleared before we
				// get around to merging the new configuration.
				if (!inEventScoring->HasTable())
					m_Table = false;
			}

			inTree->GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			inTree->GetAttrib(ATTRIB_SCORING_TIME, m_Time);
			inTree->GetAttrib(ATTRIB_SCORING_SCT, m_SCT);
			inTree->GetAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
			inTree->GetAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
			inTree->GetAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
			return true;
		}
		break;

	case ARBScoringType::ByOpenClose:
		if (name == TREE_BY_OPENCLOSE)
		{
			inTree->GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			inTree->GetAttrib(ATTRIB_SCORING_TIME, m_Time);
			inTree->GetAttrib(ATTRIB_SCORING_SCT, m_SCT);
			inTree->GetAttrib(ATTRIB_SCORING_SCT2, m_SCT2);
			inTree->GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
			inTree->GetAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
			inTree->GetAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
			inTree->GetAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
			inTree->GetAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
			inTree->GetAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
			return true;
		}
		break;

	case ARBScoringType::ByPoints:
		if (name == TREE_BY_POINTS)
		{
			inTree->GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			inTree->GetAttrib(ATTRIB_SCORING_TIME, m_Time);
			inTree->GetAttrib(ATTRIB_SCORING_SCT, m_SCT);
			inTree->GetAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
			inTree->GetAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
			inTree->GetAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
			inTree->GetAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
			return true;
		}
		break;

	case ARBScoringType::BySpeed:
		if (name == TREE_BY_SPEED)
		{
			inTree->GetAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
			inTree->GetAttrib(ATTRIB_SCORING_TIME, m_Time);
			inTree->GetAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
			inTree->GetAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
			return true;
		}
		break;

	case ARBScoringType::ByPass:
		if (name == TREE_BY_PASS)
		{
			inTree->GetAttrib(ATTRIB_SCORING_TIME, m_Time);
			return true;
		}
		break;
	}
	return false;
}


bool ARBDogRunScoring::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	switch (m_type)
	{
	case ARBScoringType::Unknown:
		break;

	case ARBScoringType::ByTime:
	{
		ElementNodePtr scoring = ioTree->AddElementNode(TREE_BY_TIME);
		if (m_Table) // Default is no
			scoring->AddAttrib(ATTRIB_SCORING_HAS_TABLE, m_Table);
		scoring->AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
		scoring->AddAttrib(ATTRIB_SCORING_TIME, m_Time);
		scoring->AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
		scoring->AddAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
		scoring->AddAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
		if (0 < m_Obstacles)
			scoring->AddAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
	}
		return true;

	case ARBScoringType::ByOpenClose:
	{
		ElementNodePtr scoring = ioTree->AddElementNode(TREE_BY_OPENCLOSE);
		if (0 < m_CourseFaults)
			scoring->AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
		scoring->AddAttrib(ATTRIB_SCORING_TIME, m_Time);
		if (0 < m_SCT)
			scoring->AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
		if (0 < m_SCT2)
			scoring->AddAttrib(ATTRIB_SCORING_SCT2, m_SCT2);
		scoring->AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDOPEN, m_NeedOpenPts);
		scoring->AddAttrib(ATTRIB_BY_OPENCLOSE_NEEDCLOSE, m_NeedClosePts);
		scoring->AddAttrib(ATTRIB_BY_OPENCLOSE_GOTOPEN, m_OpenPts);
		scoring->AddAttrib(ATTRIB_BY_OPENCLOSE_GOTCLOSE, m_ClosePts);
		scoring->AddAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
		if (0 < m_Obstacles)
			scoring->AddAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
	}
		return true;

	case ARBScoringType::ByPoints:
	{
		ElementNodePtr scoring = ioTree->AddElementNode(TREE_BY_POINTS);
		if (0 < m_CourseFaults)
			scoring->AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
		scoring->AddAttrib(ATTRIB_SCORING_TIME, m_Time);
		if (0 < m_SCT)
			scoring->AddAttrib(ATTRIB_SCORING_SCT, m_SCT);
		scoring->AddAttrib(ATTRIB_BY_POINTS_NEED, m_NeedOpenPts);
		scoring->AddAttrib(ATTRIB_BY_POINTS_GOT, m_OpenPts);
		scoring->AddAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
		if (0 < m_Obstacles)
			scoring->AddAttrib(ATTRIB_SCORING_OBSTACLES, m_Obstacles);
	}
		return true;

	case ARBScoringType::BySpeed:
	{
		ElementNodePtr scoring = ioTree->AddElementNode(TREE_BY_SPEED);
		scoring->AddAttrib(ATTRIB_SCORING_FAULTS, m_CourseFaults);
		scoring->AddAttrib(ATTRIB_SCORING_TIME, m_Time);
		scoring->AddAttrib(ATTRIB_BY_TIME_YARDS, m_Yards);
		scoring->AddAttrib(ATTRIB_SCORING_BONUSTITLEPTS, m_BonusTitlePts);
	}
		return true;

	case ARBScoringType::ByPass:
		ElementNodePtr scoring = ioTree->AddElementNode(TREE_BY_PASS);
		scoring->AddAttrib(ATTRIB_SCORING_TIME, m_Time);
		return true;
	}
	return false;
}


bool ARBDogRunScoring::GetMinYPS(bool inTableInYPS, double& outYPS) const
{
	bool bOk = false;
	if (ARBScoringType::ByTime == GetType() && 0 < GetYards() && 0.0 < GetSCT())
	{
		bOk = true;
		double t = GetSCT();
		if (HasTable() && 5.0 < t && !inTableInYPS)
			t -= 5;
		outYPS = GetYards() / t;
	}
	return bOk;
}


bool ARBDogRunScoring::GetYPS(bool inTableInYPS, double& outYPS) const
{
	return GetYPS(inTableInYPS, GetTime(), outYPS);
}


bool ARBDogRunScoring::GetYPS(bool inTableInYPS, double inTime, double& outYPS) const
{
	bool bOk = false;
	if ((ARBScoringType::ByTime == GetType() || ARBScoringType::BySpeed == GetType()) && 0 < GetYards() && 0.0 < inTime)
	{
		bOk = true;
		double t = inTime;
		if (HasTable() && 5.0 < t && !inTableInYPS)
			t -= 5;
		outYPS = GetYards() / t;
	}
	return bOk;
}


bool ARBDogRunScoring::GetObstaclesPS(bool inTableInYPS, bool inRunTimeInOPS, double& outOPS, int& outPrec) const
{
	bool bOk = false;
	double t = GetTime();
	if (0.0 < t)
	{
		if (ARBScoringType::BySpeed == m_type)
		{
			bOk = true;
			outOPS = sc_magicMPH / t;
			outPrec = 2;
		}
		else if (0 < GetObstacles())
		{
			bOk = true;
			if (ARBScoringType::ByTime == m_type && HasTable() && 5.0 < t && !inTableInYPS)
			{
				t -= 5;
			}
			else if (!inRunTimeInOPS && ARBScoringType::ByOpenClose == m_type && t > m_SCT && m_SCT2 > 0.0)
			{
				t = m_SCT;
			}
			outOPS = GetObstacles() / t;
			outPrec = 3;
		}
	}
	return bOk;
}


double ARBDogRunScoring::GetTimeFaults(ARBConfigScoringPtr const& inScoring) const
{
	double timeFaults = 0.0;
	if (ARBScoringType::ByTime == m_type || ARBScoringType::ByOpenClose == m_type || ARBScoringType::ByPoints == m_type)
	{
		double timeSCT = m_SCT;
		bool bAddTimeFaultsUnder = false;
		bool bAddTimeFaultsOver = true;
		if (ARBScoringType::ByTime != m_type)
			bAddTimeFaultsOver = false;
		if (inScoring)
		{
			// Compute time faults on gamble-style events now.
			// This currently applies only to DOCNA strategic time gamble
			// and AKC FAST.
			if (ARBScoringType::ByTime != m_type)
			{
				timeSCT += m_SCT2;
				bAddTimeFaultsUnder = inScoring->ComputeTimeFaultsUnder();
				bAddTimeFaultsOver = inScoring->ComputeTimeFaultsOver();
			}
			else if (ARBScoringStyle::TimePlusFaults == inScoring->GetScoringStyle())
			{
				bAddTimeFaultsUnder = inScoring->ComputeTimeFaultsUnder();
				bAddTimeFaultsOver = inScoring->ComputeTimeFaultsOver();
			}
		}
		if (0.0 < timeSCT)
		{
			if (bAddTimeFaultsUnder)
			{
				double time = m_Time;
				if (m_bRoundTimeFaults)
					time = ceil(m_Time);
				if (time < timeSCT)
					timeFaults = timeSCT - time;
			}
			if (bAddTimeFaultsOver)
			{
				double time = m_Time;
				if (m_bRoundTimeFaults)
					time = floor(m_Time);
				if (time > timeSCT)
					timeFaults = time - timeSCT;
			}
		}
	}
	return timeFaults * inScoring->TimeFaultMultiplier();
}

} // namespace ARB
} // namespace dconSoft
