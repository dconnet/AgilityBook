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
 * 2016-06-14 Add support for speed point based lifetime points.
 * 2016-01-06 Add support for named lifetime points.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-10-06 Added as part of file version 10.
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


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
class ARBConfigLifetimePoints_concrete : public ARBConfigLifetimePoints
{
public:
	ARBConfigLifetimePoints_concrete()
	{
	}
	ARBConfigLifetimePoints_concrete(wxString const& name, double inPoints, double inFaults)
		: ARBConfigLifetimePoints(name, inPoints, inFaults)
	{
	}
	ARBConfigLifetimePoints_concrete(wxString const& name, double inFaults)
		: ARBConfigLifetimePoints(name, inFaults)
	{
	}
	ARBConfigLifetimePoints_concrete(ARBConfigLifetimePoints const& rhs)
		: ARBConfigLifetimePoints(rhs)
	{
	}
};
}; // namespace


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New()
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>();
}


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New(wxString const& name, double inPoints, double inFaults)
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>(name, inPoints, inFaults);
}


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New(wxString const& name, double inFaults)
{
	return std::make_shared<ARBConfigLifetimePoints_concrete>(name, inFaults);
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints()
	: m_Name()
	, m_UseSpeedPts(false)
	, m_Points(0.0)
	, m_Faults(0.0)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(wxString const& name, double inPoints, double inFaults)
	: m_Name(name)
	, m_UseSpeedPts(false)
	, m_Points(inPoints)
	, m_Faults(inFaults)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(wxString const& name, double inFaults)
	: m_Name(name)
	, m_UseSpeedPts(true)
	, m_Points(0)
	, m_Faults(inFaults)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(ARBConfigLifetimePoints const& rhs)
	: m_Name(rhs.m_Name)
	, m_UseSpeedPts(rhs.m_UseSpeedPts)
	, m_Points(rhs.m_Points)
	, m_Faults(rhs.m_Faults)
{
}


ARBConfigLifetimePoints::ARBConfigLifetimePoints(ARBConfigLifetimePoints&& rhs)
	: m_Name(std::move(rhs.m_Name))
	, m_UseSpeedPts(std::move(rhs.m_UseSpeedPts))
	, m_Points(std::move(rhs.m_Points))
	, m_Faults(std::move(rhs.m_Faults))
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
		m_Name = rhs.m_Name;
		m_UseSpeedPts = rhs.m_UseSpeedPts;
		m_Points = rhs.m_Points;
		m_Faults = rhs.m_Faults;
	}
	return *this;
}


ARBConfigLifetimePoints& ARBConfigLifetimePoints::operator=(ARBConfigLifetimePoints&& rhs)
{
	if (this != &rhs)
	{
		m_Name = std::move(rhs.m_Name);
		m_UseSpeedPts = std::move(rhs.m_UseSpeedPts);
		m_Points = std::move(rhs.m_Points);
		m_Faults = std::move(rhs.m_Faults);
	}
	return *this;
}


bool ARBConfigLifetimePoints::operator==(ARBConfigLifetimePoints const& rhs) const
{
	// clang-format off
	return m_Name == rhs.m_Name
		&& m_UseSpeedPts == rhs.m_UseSpeedPts
		&& m_Points == rhs.m_Points
		&& m_Faults == rhs.m_Faults;
	// clang-format on
}


wxString ARBConfigLifetimePoints::GetGenericName() const
{
	if (m_UseSpeedPts)
		return Localization()->LifetimePointsNameWithSpeedPointsFormat(m_Faults);
	else
		return Localization()->LifetimePointsNameFormat(m_Points, m_Faults);
}


bool ARBConfigLifetimePoints::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree)
		return false;
	if (inVersion < ARBVersion(14, 4))
	{
		if (inTree->GetName() != L"LifeTime")
			return false;

		// pre-v14.4: Points is required. v14.4+ is not.
		if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points))
		{
			ioCallback.LogMessage(
				Localization()->ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_POINTS));
			return false;
		}
	}
	else
	{
		if (inTree->GetName() != TREE_LIFETIME_POINTS)
			return false;

		// Note: Allow an empty name - that's the default.
		// There should only be one.
		inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_NAME, m_Name);
		inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_SPEEDPTS, m_UseSpeedPts);
		inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points);
	}

	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults))
	{
		ioCallback.LogMessage(
			Localization()->ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_FAULTS));
		return false;
	}
	return true;
}


bool ARBConfigLifetimePoints::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr life = ioTree->AddElementNode(TREE_LIFETIME_POINTS);
	if (!m_Name.empty())
		life->AddAttrib(ATTRIB_LIFETIME_POINTS_NAME, m_Name);
	life->AddAttrib(ATTRIB_LIFETIME_POINTS_SPEEDPTS, m_UseSpeedPts);
	if (!m_UseSpeedPts)
		life->AddAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points, 0);
	life->AddAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults, 0);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigLifetimePointsList::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBConfigLifetimePointsPtr thing(ARBConfigLifetimePoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBConfigLifetimePointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBConfigLifetimePointsPtr const& one, ARBConfigLifetimePointsPtr const& two) {
		if (one->GetName() == two->GetName())
			return one->GetFaults() < two->GetFaults();
		return one->GetName() < two->GetName();
	});
}


double ARBConfigLifetimePointsList::GetLifetimePoints(wxString const& inName, double inFaults, short inSpeedPts) const
{
	// This is why we keep the list sorted!
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName && inFaults <= static_cast<double>((*iter)->GetFaults()))
		{
			if ((*iter)->UseSpeedPts())
				return inSpeedPts;
			else
				return (*iter)->GetPoints();
		}
	}
	return 0.0;
}


bool ARBConfigLifetimePointsList::FindLifetimePoints(
	wxString const& inName,
	double inFaults,
	ARBConfigLifetimePointsPtr* outPoints) const
{
	if (outPoints)
		outPoints->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName && ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			if (outPoints)
				*outPoints = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigLifetimePointsList::AddLifetimePoints(
	wxString const& inName,
	bool inUseSpeedPts,
	double inPoints,
	double inFaults,
	ARBConfigLifetimePointsPtr* outPoints)
{
	if (outPoints)
		outPoints->reset();
	if (FindLifetimePoints(inName, inFaults))
		return false;
	ARBConfigLifetimePointsPtr pLife;
	if (inUseSpeedPts)
		pLife = ARBConfigLifetimePoints::New(inName, inFaults);
	else
		pLife = ARBConfigLifetimePoints::New(inName, inPoints, inFaults);
	push_back(pLife);
	sort();
	if (outPoints)
		*outPoints = pLife;
	return true;
}


bool ARBConfigLifetimePointsList::DeleteLifetimePoints(wxString const& inName, double inFaults)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName && ARBDouble::equal((*iter)->GetFaults(), inFaults))
		{
			erase(iter);
			return true;
		}
	}
	return false;
}

} // namespace ARB
} // namespace dconSoft
