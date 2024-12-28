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
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBDogRunOtherPoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

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
class ARBDogRunOtherPoints_concrete : public ARBDogRunOtherPoints
{
public:
	ARBDogRunOtherPoints_concrete()
	{
	}
	ARBDogRunOtherPoints_concrete(ARBDogRunOtherPoints const& rhs)
		: ARBDogRunOtherPoints(rhs)
	{
	}
};
}; // namespace


ARBDogRunOtherPointsPtr ARBDogRunOtherPoints::New()
{
	return std::make_shared<ARBDogRunOtherPoints_concrete>();
}


ARBDogRunOtherPoints::ARBDogRunOtherPoints()
	: m_Name()
	, m_Points(0)
{
}


ARBDogRunOtherPoints::ARBDogRunOtherPoints(ARBDogRunOtherPoints const& rhs)
	: m_Name(rhs.m_Name)
	, m_Points(rhs.m_Points)
{
}


ARBDogRunOtherPoints::ARBDogRunOtherPoints(ARBDogRunOtherPoints&& rhs)
	: m_Name(std::move(rhs.m_Name))
	, m_Points(std::move(rhs.m_Points))
{
}


ARBDogRunOtherPoints::~ARBDogRunOtherPoints()
{
}


ARBDogRunOtherPointsPtr ARBDogRunOtherPoints::Clone() const
{
	return std::make_shared<ARBDogRunOtherPoints_concrete>(*this);
}


ARBDogRunOtherPoints& ARBDogRunOtherPoints::operator=(ARBDogRunOtherPoints const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Points = rhs.m_Points;
	}
	return *this;
}


ARBDogRunOtherPoints& ARBDogRunOtherPoints::operator=(ARBDogRunOtherPoints&& rhs)
{
	if (this != &rhs)
	{
		m_Name = std::move(rhs.m_Name);
		m_Points = std::move(rhs.m_Points);
	}
	return *this;
}


bool ARBDogRunOtherPoints::operator==(ARBDogRunOtherPoints const& rhs) const
{
	return m_Name == rhs.m_Name && m_Points == rhs.m_Points;
}


size_t ARBDogRunOtherPoints::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	ioStrings.insert(m_Name);
	return 1;
}


bool ARBDogRunOtherPoints::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_PLACEMENT_OTHERPOINTS)
		return false;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_NAME, m_Name) || 0 == m_Name.length())
	{
		ioCallback.LogMessage(
			Localization()->ErrorMissingAttribute(TREE_PLACEMENT_OTHERPOINTS, ATTRIB_PLACEMENT_OTHERPOINTS_NAME));
		return false;
	}
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_POINTS, m_Points))
	{
		ioCallback.LogMessage(
			Localization()->ErrorMissingAttribute(TREE_PLACEMENT_OTHERPOINTS, ATTRIB_PLACEMENT_OTHERPOINTS_POINTS));
		return false;
	}
	return true;
}


bool ARBDogRunOtherPoints::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr other = ioTree->AddElementNode(TREE_PLACEMENT_OTHERPOINTS);
	other->AddAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_NAME, m_Name);
	other->AddAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_POINTS, m_Points);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRunOtherPointsList::Load(
	ARBConfig const& inConfig,
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBDogRunOtherPointsPtr thing(ARBDogRunOtherPoints::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogRunOtherPointsList::AddOtherPoints(ARBDogRunOtherPointsPtr const& inOther)
{
	bool bAdded = false;
	if (inOther)
	{
		bAdded = true;
		push_back(inOther);
	}
	return bAdded;
}

} // namespace ARB
} // namespace dconSoft
