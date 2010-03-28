/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBDogRunOtherPoints.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogRunOtherPointsPtr ARBDogRunOtherPoints::New()
{
	return ARBDogRunOtherPointsPtr(new ARBDogRunOtherPoints());
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


ARBDogRunOtherPoints::~ARBDogRunOtherPoints()
{
}


ARBDogRunOtherPointsPtr ARBDogRunOtherPoints::Clone() const
{
	return ARBDogRunOtherPointsPtr(new ARBDogRunOtherPoints(*this));
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


bool ARBDogRunOtherPoints::operator==(ARBDogRunOtherPoints const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Points == rhs.m_Points;
}


size_t ARBDogRunOtherPoints::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	ioStrings.insert(m_Name);
	return 1;
}


bool ARBDogRunOtherPoints::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_PLACEMENT_OTHERPOINTS)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PLACEMENT_OTHERPOINTS, ATTRIB_PLACEMENT_OTHERPOINTS_NAME));
		return false;
	}
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PLACEMENT_OTHERPOINTS_POINTS, m_Points))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PLACEMENT_OTHERPOINTS, ATTRIB_PLACEMENT_OTHERPOINTS_POINTS));
		return false;
	}
	return true;
}


bool ARBDogRunOtherPoints::Save(ElementNodePtr ioTree) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogRunOtherPointsPtr thing(ARBDogRunOtherPoints::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogRunOtherPointsList::AddOtherPoints(ARBDogRunOtherPointsPtr inOther)
{
	bool bAdded = false;
	if (inOther)
	{
		bAdded = true;
		push_back(inOther);
	}
	return bAdded;
}
