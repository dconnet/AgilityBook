/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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


size_t ARBDogRunOtherPoints::GetSearchStrings(std::set<tstring>& ioStrings) const
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
	ASSERT(inTree);
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
	ASSERT(ioTree);
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
