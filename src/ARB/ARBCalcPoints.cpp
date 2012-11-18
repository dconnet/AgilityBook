/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief General information to be tracked.
 * @author David Connet
 *
 * Revision History
 * @li 2011-07-31 DRC Created
 */

#include "stdafx.h"
#include "ARBCalcPoints.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"
#include "StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsPtr ARBCalcPoints::New(ARBPointsType type)
{
	ARBCalcPointsPtr p;
	switch (type)
	{
	default:
		assert(0);
	case ePointsTypeMax:
		break;
	case ePointsTypeNormal:
		p = ARBCalcPointsNormal::New();
		break;
	case ePointsTypeT2B:
		p = ARBCalcPointsT2B::New();
		break;
	case ePointsTypeUKI:
		p = ARBCalcPointsUKI::New();
		break;
	}
	return p;
}


ARBCalcPoints::ARBCalcPoints()
{
}


ARBCalcPoints::~ARBCalcPoints()
{
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBCalcPointsNormal_concrete : public ARBCalcPointsNormal
	{
	public:
		ARBCalcPointsNormal_concrete() {}
	};
};


ARBCalcPointsNormalPtr ARBCalcPointsNormal::New()
{
	return make_shared<ARBCalcPointsNormal_concrete>();
}


ARBCalcPointsNormal::ARBCalcPointsNormal()
{
}


std::wstring ARBCalcPointsNormal::GetGenericName(double points, double faults) const
{
	return Localization()->TitlePointsNameFormat(points, faults);
}


double ARBCalcPointsNormal::GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass) const
{
	// Pass through.
	return inPoints;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBCalcPointsT2B_concrete : public ARBCalcPointsT2B
	{
	public:
		ARBCalcPointsT2B_concrete() {}
	};
};


ARBCalcPointsT2BPtr ARBCalcPointsT2B::New()
{
	return make_shared<ARBCalcPointsT2B_concrete>();
}


ARBCalcPointsT2B::ARBCalcPointsT2B()
{
}


std::wstring ARBCalcPointsT2B::GetGenericName(double points, double faults) const
{
	return StringUtil::stringW(_("IDS_TITLEPOINT_NORMAL_T2B"));
}


double ARBCalcPointsT2B::GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass) const
{
	if (inTime == inSCT && inPlace == 1)
		return 10.0;
	for (long i = 1; i <= 9; ++i)
	{
		// Formula from p65 of the Sept 2011 AKC rule book:
		// The formula for the upper end of the percentage range is:
		// (Fastest time in a jump height multiplied by the percentage)
		long t = static_cast<long>((0.1 * i) * inSCT
		// plus the fastest time in the jump height.
		// Both the low and high end of the percentage range are truncated.
		//  [static_cast takes care of that]
			+ inSCT);
		// Each dog’s time gets truncated.
		// A dog’s time falls into a percentage range if it is greater than
		// the low end and less than or equal to the high end of the range.
		//  [since we check from top/bottom, we only need to test one end of
		//  the range]
		if (static_cast<long>(inTime) <= t)
			return 10.0 - i;
	}
	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBCalcPointsUKI_concrete : public ARBCalcPointsUKI
	{
	public:
		ARBCalcPointsUKI_concrete() {}
	};
};


ARBCalcPointsUKIPtr ARBCalcPointsUKI::New()
{
	return make_shared<ARBCalcPointsUKI_concrete>();
}


ARBCalcPointsUKI::ARBCalcPointsUKI()
{
}


std::wstring ARBCalcPointsUKI::GetGenericName(double points, double faults) const
{
	return StringUtil::stringW(_("IDS_TITLEPOINT_NORMAL_UKI"));
}


double ARBCalcPointsUKI::GetPoints(
		double inPoints,
		double inTime,
		double inSCT,
		short inPlace,
		short inClass) const
{
	double pts = 4.0;
	if (inClass <= 10)
	{
		switch (inPlace)
		{
		case 1:
			pts = 6.0;
			break;
		case 2:
			pts = 5.0;
			break;
		}
	}
	else if (inClass <= 30)
	{
		switch (inPlace)
		{
		case 1:
			pts = 12.0;
			break;
		case 2:
			pts = 8.0;
			break;
		case 3:
			pts = 7.0;
			break;
		case 4:
			pts = 5.0;
			break;
		}
	}
	else
	{
		if (1 <= inPlace && inPlace < 9)
			pts = 12.0 - (inPlace - 1);
	}
	return pts;
}
