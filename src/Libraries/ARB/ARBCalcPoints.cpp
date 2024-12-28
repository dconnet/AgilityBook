/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief General information to be tracked.
 * @author David Connet
 *
 * Revision History
 * 2020-01-18 USDAA Top10 changed.
 * 2018-06-23 Added USDAA Top10.
 * 2011-07-31 Created
 */

#include "stdafx.h"
#include "ARB/ARBCalcPoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

ARBCalcPointsPtr ARBCalcPoints::New(ARBPointsType type)
{
	ARBCalcPointsPtr p;
	switch (type)
	{
	case ARBPointsType::Normal:
		p = ARBCalcPointsNormal::New();
		break;
	case ARBPointsType::T2B:
		p = ARBCalcPointsT2B::New();
		break;
	case ARBPointsType::UKI:
		p = ARBCalcPointsUKI::New();
		break;
	case ARBPointsType::Top10USDAA:
		p = ARBCalcPointsTop10USDAA::New();
		break;
	case ARBPointsType::Max:
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

ARBCalcPointsNormalPtr ARBCalcPointsNormal::New()
{
	return std::make_shared<ARBCalcPointsNormal>();
}


ARBCalcPointsNormal::ARBCalcPointsNormal()
{
}


wxString ARBCalcPointsNormal::GetGenericName(double points, double faults) const
{
	return Localization()->TitlePointsNameFormat(points, faults);
}


double ARBCalcPointsNormal::GetPoints(
	double inPoints,
	double inTime,
	double inSCT,
	short inPlace,
	short inClass,
	ARBDate date,
	bool isTourney,
	bool isAtHome) const
{
	// Pass through.
	return inPoints;
}

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsT2BPtr ARBCalcPointsT2B::New()
{
	return std::make_shared<ARBCalcPointsT2B>();
}


ARBCalcPointsT2B::ARBCalcPointsT2B()
{
}


wxString ARBCalcPointsT2B::GetGenericName(double points, double faults) const
{
	return wxGetTranslation(arbT("IDS_TITLEPOINT_NORMAL_T2B"));
}


double ARBCalcPointsT2B::GetPoints(
	double inPoints,
	double inTime,
	double inSCT,
	short inPlace,
	short inClass,
	ARBDate date,
	bool isTourney,
	bool isAtHome) const
{
	if (inTime == inSCT && inPlace == 1)
		return 10.0;
	for (long i = 1; i <= 9; ++i)
	{
		// Formula from p65 of the Sept 2011 AKC rule book:
		// The formula for the upper end of the percentage range is:
		// (Fastest time in a jump height multiplied by the percentage)
		long t = static_cast<long>(
			(0.1 * i) * inSCT
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

ARBCalcPointsUKIPtr ARBCalcPointsUKI::New()
{
	return std::make_shared<ARBCalcPointsUKI>();
}


ARBCalcPointsUKI::ARBCalcPointsUKI()
{
}


wxString ARBCalcPointsUKI::GetGenericName(double points, double faults) const
{
	return wxGetTranslation(arbT("IDS_TITLEPOINT_NORMAL_UKI"));
}


double ARBCalcPointsUKI::GetPoints(
	double inPoints,
	double inTime,
	double inSCT,
	short inPlace,
	short inClass,
	ARBDate date,
	bool isTourney,
	bool isAtHome) const
{
	double pts = 4.0;
	if (isAtHome)
	{
		pts = 2.0;
	}
	else if (inClass <= 10)
	{
		switch (inPlace)
		{
		case 1:
			pts = 6.0;
			break;
		case 2:
			pts = 5.0;
			break;
		default:
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
		default:
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

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsTop10USDAAPtr ARBCalcPointsTop10USDAA::New()
{
	return std::make_shared<ARBCalcPointsTop10USDAA>();
}


ARBCalcPointsTop10USDAA::ARBCalcPointsTop10USDAA()
{
}


wxString ARBCalcPointsTop10USDAA::GetGenericName(double points, double faults) const
{
	return wxGetTranslation(arbT("IDS_TITLEPOINT_TOP10_USDAA"));
}


double ARBCalcPointsTop10USDAA::GetPoints(
	double inPoints,
	double inTime,
	double inSCT,
	short inPlace,
	short inClass,
	ARBDate date,
	bool isTourney,
	bool isAtHome) const
{
	static const ARBDate ruleChange(2020, 1, 1);

	// Compute number of places to rank.
	int numPlaces = 0;
	if (2 <= inClass && inClass <= 3)
		numPlaces = 1;
	else if (4 <= inClass && inClass <= 5)
		numPlaces = 2;
	else if (6 <= inClass && inClass <= 10)
		numPlaces = 3;
	else if (10 < inClass)
		numPlaces = ((inClass - 1) / 10) + 3;

	// If none or no placement specified, outa here!
	if (numPlaces < 1 || inPlace < 1)
		return 0.0;
	if (date < ruleChange)
	{
		// In old style, only the top N places get points.
		if (inPlace > numPlaces)
			return 0.0;
	}
	else
	{
		// In new style, only the top 5 get extra points.
		if (numPlaces > 5)
			numPlaces = 5;
	}

	double pts = (numPlaces - inPlace) * 2 + 1;
	if (pts < 0.0) // Only happens in new style, we already returned in old.
		pts = 0.0;

	if (date < ruleChange)
		return pts;
	else
		return (isTourney ? 5.0 : 10.0) + pts;
}

} // namespace ARB
} // namespace dconSoft
