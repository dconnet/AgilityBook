/*
 * Copyright © 2004-2008 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-10-06 DRC Added as part of file version 10.
 */

#include "StdAfx.h"
#include <algorithm>
#include "ARBConfigLifetimePoints.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New()
{
	return ARBConfigLifetimePointsPtr(new ARBConfigLifetimePoints());
}


ARBConfigLifetimePointsPtr ARBConfigLifetimePoints::New(
		double inPoints,
		double inFaults)
{
	return ARBConfigLifetimePointsPtr(new ARBConfigLifetimePoints(inPoints, inFaults));
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
	return ARBConfigLifetimePointsPtr(new ARBConfigLifetimePoints(*this));
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


tstring ARBConfigLifetimePoints::GetGenericName() const
{
	return Localization()->LifetimePointsNameFormat(m_Points, m_Faults);
}


bool ARBConfigLifetimePoints::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
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
	ASSERT(ioTree);
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


class SortConfigLifetimePoints
{
public:
	SortConfigLifetimePoints() {}
	bool operator()(ARBConfigLifetimePointsPtr one, ARBConfigLifetimePointsPtr two) const
	{
		return one->GetFaults() < two->GetFaults();
	}
};


void ARBConfigLifetimePointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigLifetimePoints());
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
