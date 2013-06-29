/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-10-06 DRC Added as part of file version 10.
 */

#include "stdafx.h"
#include "ARB/ARBConfigLifetimePoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBConfigLifetimePoints_concrete : public ARBConfigLifetimePoints
	{
	public:
		ARBConfigLifetimePoints_concrete() {}
		ARBConfigLifetimePoints_concrete(double inPoints, double inFaults)
			: ARBConfigLifetimePoints(inPoints, inFaults)
		{
		}
		ARBConfigLifetimePoints_concrete(ARBConfigLifetimePoints const& rhs)
			: ARBConfigLifetimePoints(rhs)
		{
		}
	};
};


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New()
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>();
}


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New(
		double inPoints,
		double inFaults)
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>(inPoints, inFaults);
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints()
	: m_Points(0.0)
	, m_Faults(0.0)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(
		double inPoints,
		double inFaults)
	: m_Points(inPoints)
	, m_Faults(inFaults)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(ARBConfigLifetimePoints const& rhs)
	: m_Points(rhs.m_Points)
	, m_Faults(rhs.m_Faults)
{
}


ARBConfigLifetimePoints::~ARBConfigLifetimePoints()
{
}


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::Clone() const
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>(*this);
}


ARBConfigLifetimePoints& ARBConfigLifetimePoints::operator=(ARBConfigLifetimePoints const& rhs)
{
	if (this != &rhs)
	{
		m_Points = rhs.m_Points;
		m_Faults = rhs.m_Faults;
	}
	return *this;
}


bool ARBConfigLifetimePoints::operator==(ARBConfigLifetimePoints const& rhs) const
{
	return m_Points == rhs.m_Points
		&& m_Faults == rhs.m_Faults;
}


std::wstring ARBConfigLifetimePoints::GetGenericName() const
{
	return Localization()->LifetimePointsNameFormat(m_Points, m_Faults);
}


bool ARBConfigLifetimePoints::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_LIFETIME_POINTS)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_POINTS));
		return false;
	}
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_FAULTS));
		return false;
	}
	return true;
}


bool ARBConfigLifetimePoints::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr life = ioTree->AddElementNode(TREE_LIFETIME_POINTS);
	life->AddAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points, 0);
	life->AddAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults, 0);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigLifetimePointsList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigLifetimePointsPtr thing(ARBConfigLifetimePoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


#ifndef ARB_HAS_LAMBDA
class SortConfigLifetimePoints
{
public:
	SortConfigLifetimePoints() {}
	bool operator()(ARBConfigLifetimePointsPtr const& one, ARBConfigLifetimePointsPtr const& two) const
	{
		return one->GetFaults() < two->GetFaults();
	}
};
#endif


void ARBConfigLifetimePointsList::sort()
{
	if (2 > size())
		return;
#ifdef ARB_HAS_LAMBDA
	std::stable_sort(begin(), end(),
		[](ARBConfigLifetimePointsPtr const& one, ARBConfigLifetimePointsPtr const& two)
		{
			return one->GetFaults() < two->GetFaults();
		}
	);
#else
	std::stable_sort(begin(), end(), SortConfigLifetimePoints());
#endif
}


double ARBConfigLifetimePointsList::GetLifetimePoints(double inFaults) const
{
	// This is why we keep the list sorted!
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (inFaults <= static_cast<double>((*iter)->GetFaults()))
			return (*iter)->GetPoints();
	}
	return 0.0;
}


bool ARBConfigLifetimePointsList::FindLifetimePoints(
		double inFaults,
		ARBConfigLifetimePointsPtr* outPoints) const
{
	if (outPoints)
		outPoints->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			if (outPoints)
				*outPoints = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigLifetimePointsList::AddLifetimePoints(
		double inPoints,
		double inFaults,
		ARBConfigLifetimePointsPtr* outPoints)
{
	if (outPoints)
		outPoints->reset();
	if (FindLifetimePoints(inFaults))
		return false;
	ARBConfigLifetimePointsPtr pLife(ARBConfigLifetimePoints::New(inPoints, inFaults));
	push_back(pLife);
	sort();
	if (outPoints)
		*outPoints = pLife;
	return true;
}


bool ARBConfigLifetimePointsList::DeleteLifetimePoints(double inFaults)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
