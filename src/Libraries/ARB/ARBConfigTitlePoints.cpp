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
 * 2016-01-06 Add support for named lifetime points.
 * 2011-07-31 Added 'Type' to configuration.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-03-31 Started adding auto-lifetime point accumulation.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-07-12 Added as part of file version 5.
 */

#include "stdafx.h"
#include "ARB/ARBConfigTitlePoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfigLifetimePoints.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
constexpr struct PointsEnum
{
	wchar_t const* pPoints; ///< Actual text in file
	ARBPointsType type;     ///< Enum type
} const sc_Points[] = {
	{ATTRIB_TITLE_POINTS_TYPE_NORMAL, ARBPointsType::Normal},
	{ATTRIB_TITLE_POINTS_TYPE_T2B, ARBPointsType::T2B},
	{ATTRIB_TITLE_POINTS_TYPE_UKI, ARBPointsType::UKI},
};
constexpr size_t sc_nPoints = sizeof(sc_Points) / sizeof(sc_Points[0]);


ARBPointsType PointsToType(std::wstring const& str)
{
	for (size_t n = 0; n < sc_nPoints; ++n)
	{
		if (str == sc_Points[n].pPoints)
			return sc_Points[n].type;
	}
	assert(0);
	return ARBPointsType::Normal;
}


std::wstring TypeToPoints(ARBPointsType type)
{
	for (size_t n = 0; n < sc_nPoints; ++n)
	{
		if (type == sc_Points[n].type)
			return sc_Points[n].pPoints;
	}
	assert(0);
	return std::wstring();
}
} // namespace

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ARBConfigTitlePoints_concrete : public ARBConfigTitlePoints
{
public:
	ARBConfigTitlePoints_concrete()
	{
	}
	ARBConfigTitlePoints_concrete(double inPoints, double inFaults, ARBPointsType inType)
		: ARBConfigTitlePoints(inPoints, inFaults, inType)
	{
	}
	ARBConfigTitlePoints_concrete(ARBConfigTitlePoints const& rhs)
		: ARBConfigTitlePoints(rhs)
	{
	}
};
}; // namespace

ARBConfigTitlePointsPtr ARBConfigTitlePoints::New()
{
	return std::make_shared<ARBConfigTitlePoints_concrete>();
}


ARBConfigTitlePointsPtr ARBConfigTitlePoints::New(double inPoints, double inFaults, ARBPointsType inType)
{
	return std::make_shared<ARBConfigTitlePoints_concrete>(inPoints, inFaults, inType);
}


ARBConfigTitlePoints::ARBConfigTitlePoints()
	: m_Points(0.0)
	, m_Faults(0.0)
	, m_Calc(ARBCalcPointsNormal::New())
{
}


ARBConfigTitlePoints::ARBConfigTitlePoints(double inPoints, double inFaults, ARBPointsType inType)
	: m_Points(inPoints)
	, m_Faults(inFaults)
	, m_Calc(ARBCalcPoints::New(inType))
{
}


ARBConfigTitlePoints::ARBConfigTitlePoints(ARBConfigTitlePoints const& rhs)
	: m_Points(rhs.m_Points)
	, m_Faults(rhs.m_Faults)
	, m_Calc(rhs.m_Calc)
{
}


ARBConfigTitlePoints::ARBConfigTitlePoints(ARBConfigTitlePoints&& rhs)
	: m_Points(std::move(rhs.m_Points))
	, m_Faults(std::move(rhs.m_Faults))
	, m_Calc(std::move(rhs.m_Calc))
{
}


ARBConfigTitlePoints::~ARBConfigTitlePoints()
{
}


ARBConfigTitlePointsPtr ARBConfigTitlePoints::Clone() const
{
	return std::make_shared<ARBConfigTitlePoints_concrete>(*this);
}


ARBConfigTitlePoints& ARBConfigTitlePoints::operator=(ARBConfigTitlePoints const& rhs)
{
	if (this != &rhs)
	{
		m_Points = rhs.m_Points;
		m_Faults = rhs.m_Faults;
		m_Calc = rhs.m_Calc;
	}
	return *this;
}


ARBConfigTitlePoints& ARBConfigTitlePoints::operator=(ARBConfigTitlePoints&& rhs)
{
	if (this != &rhs)
	{
		m_Points = std::move(rhs.m_Points);
		m_Faults = std::move(rhs.m_Faults);
		m_Calc = std::move(rhs.m_Calc);
	}
	return *this;
}


bool ARBConfigTitlePoints::operator==(ARBConfigTitlePoints const& rhs) const
{
	return m_Points == rhs.m_Points && m_Faults == rhs.m_Faults && m_Calc->GetType() == rhs.m_Calc->GetType();
}


std::wstring ARBConfigTitlePoints::GetGenericName() const
{
	return m_Calc->GetGenericName(m_Points, m_Faults);
}


bool ARBConfigTitlePoints::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback,
	ARBConfigLifetimePointsList& ioLifetimePoints)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLE_POINTS)
		return false;
	// Added in ARBVersion 13.1. Made this a backwards incompatible change
	// as the configuration will be damaged if this were saved in a 12.x form.
	std::wstring type;
	if (ARBAttribLookup::Found == inTree->GetAttrib(ATTRIB_TITLE_POINTS_TYPE, type))
	{
		m_Calc = ARBCalcPoints::New(PointsToType(type));
	}
	if (ARBPointsType::Normal == m_Calc->GetType())
	{
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_POINTS));
			return false;
		}
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults))
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_FAULTS));
			return false;
		}
		if (inVersion < ARBVersion(10, 0))
		{
			bool bLifetime;
			if (ARBAttribLookup::Found == inTree->GetAttrib(L"LifeTime", bLifetime))
			{
				if (bLifetime)
				{
					ioLifetimePoints.AddLifetimePoints(L"", false, m_Points, m_Faults);
				}
			}
		}
	}
	return true;
}


bool ARBConfigTitlePoints::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_TITLE_POINTS);
	if (ARBPointsType::Normal == m_Calc->GetType())
	{
		title->AddAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points, 0);
		title->AddAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults, 0);
	}
	else
	{
		title->AddAttrib(ATTRIB_TITLE_POINTS_TYPE, TypeToPoints(m_Calc->GetType()));
	}
	return true;
}


bool ARBConfigTitlePoints::SetPoints(double inPoints)
{
	bool bSet = false;
	if (m_Calc->AllowConfiguration())
	{
		bSet = true;
		m_Points = inPoints;
	}
	return bSet;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitlePointsList::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback,
	ARBConfigLifetimePointsList& ioLifetimePoints)
{
	ARBConfigTitlePointsPtr thing(ARBConfigTitlePoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback, ioLifetimePoints))
		return false;
	push_back(thing);
	return true;
}


void ARBConfigTitlePointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBConfigTitlePointsPtr const& one, ARBConfigTitlePointsPtr const& two) {
		return one->GetFaults() < two->GetFaults();
	});
}


ARBCalcPointsPtr ARBConfigTitlePointsList::GetCalc() const
{
	if (0 < size())
		return (*begin())->GetCalc();
	return ARBCalcPointsPtr();
}


ARBPointsType ARBConfigTitlePointsList::GetType() const
{
	ARBCalcPointsPtr calc = GetCalc();
	if (calc)
		return calc->GetType();
	return ARBPointsType::Normal;
}


void ARBConfigTitlePointsList::SetType(ARBPointsType inType)
{
	if (GetType() != inType)
	{
		clear();
		switch (inType)
		{
		case ARBPointsType::Normal:
		case ARBPointsType::Top10USDAA:
			break;
		case ARBPointsType::T2B:
		case ARBPointsType::UKI:
			push_back(ARBConfigTitlePoints::New(0.0, 0.0, inType));
			break;
		case ARBPointsType::Max:
			assert(0);
			break;
		}
	}
}


double ARBConfigTitlePointsList::GetTitlePoints(
	double inFaults,
	double inTime,
	double inSCT,
	short inPlace,
	short inClass,
	ARBDate inDate,
	bool isTourney) const
{
	double pts = 0.0;
	// This is why we keep the list sorted!
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (inFaults <= (*iter)->GetFaults())
		{
			pts = (*iter)->GetPoints();
			break;
		}
	}
	ARBCalcPointsPtr calc = GetCalc();
	if (calc)
		pts = calc->GetPoints(pts, inTime, inSCT, inPlace, inClass, inDate, isTourney);
	return pts;
}


bool ARBConfigTitlePointsList::FindTitlePoints(double inFaults, ARBConfigTitlePointsPtr* outPoints) const
{
	if (outPoints)
		outPoints->reset();
	ARBCalcPointsPtr calc = GetCalc();
	if (!calc || calc->AllowConfiguration())
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
			{
				if (outPoints)
					*outPoints = *iter;
				return true;
			}
		}
	}
	return false;
}


bool ARBConfigTitlePointsList::AddTitlePoints(double inPoints, double inFaults, ARBConfigTitlePointsPtr* outPoints)
{
	if (outPoints)
		outPoints->reset();
	ARBCalcPointsPtr calc = GetCalc();
	if ((calc && !calc->AllowConfiguration()) || FindTitlePoints(inFaults))
		return false;
	ARBConfigTitlePointsPtr pTitle(ARBConfigTitlePoints::New(inPoints, inFaults, ARBPointsType::Normal));
	push_back(pTitle);
	sort();
	if (outPoints)
		*outPoints = pTitle;
	return true;
}


bool ARBConfigTitlePointsList::DeleteTitlePoints(ARBPointsType inType, double inFaults)
{
	ARBCalcPointsPtr calc = GetCalc();
	if (!calc || calc->AllowConfiguration())
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
			{
				erase(iter);
				return true;
			}
		}
	}
	else if (calc && calc->GetType() != ARBPointsType::Normal && 1 == size())
	{
		clear();
	}
	return false;
}

} // namespace ARB
} // namespace dconSoft
