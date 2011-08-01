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

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsPtr ARBCalcPoints::New(ARBPointsType type)
{
	switch (type)
	{
	default:
		assert(0);
		// fall thru
	case ePointsTypeNormal:
		return ARBCalcPointsNormal::New();
	case ePointsTypeT2B:
		return ARBCalcPointsT2B::New();
	case ePointsTypeUKI:
		return ARBCalcPointsUKI::New();
	}
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
	return ARBCalcPointsNormalPtr(new ARBCalcPointsNormal());
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
		short inClass) const
{
	// Pass through.
	return inPoints;
}

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsT2BPtr ARBCalcPointsT2B::New()
{
	return ARBCalcPointsT2BPtr(new ARBCalcPointsT2B());
}


ARBCalcPointsT2B::ARBCalcPointsT2B()
{
}


wxString ARBCalcPointsT2B::GetGenericName(double points, double faults) const
{
	return _("IDS_TITLEPOINT_NORMAL_T2B");
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
		// Formula from p65 of the Sept 2011 AKC rule book
		long t = static_cast<long>((0.1 * i) * inSCT + inSCT);
		if (static_cast<long>(inTime) <= t)
			return 10.0 - i;
	}
	return 0.0;
}

/////////////////////////////////////////////////////////////////////////////

ARBCalcPointsUKIPtr ARBCalcPointsUKI::New()
{
	return ARBCalcPointsUKIPtr(new ARBCalcPointsUKI());
}


ARBCalcPointsUKI::ARBCalcPointsUKI()
{
}


wxString ARBCalcPointsUKI::GetGenericName(double points, double faults) const
{
	return _("IDS_TITLEPOINT_NORMAL_UKI");
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
