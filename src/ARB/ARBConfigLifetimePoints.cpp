/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @li 2004-10-06 DRC Added as part of file version 10.
 */

#include "StdAfx.h"
#include <algorithm>
#include "ARBConfigLifetimePoints.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigLifetimePoints::ARBConfigLifetimePoints()
	: m_Points(0.0)
	, m_Faults(0)
{
}

ARBConfigLifetimePoints::ARBConfigLifetimePoints(
		double inPoints,
		short inFaults)
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

bool ARBConfigLifetimePoints::operator!=(ARBConfigLifetimePoints const& rhs) const
{
	return !operator==(rhs);
}

ARBString ARBConfigLifetimePoints::GetGenericName() const
{
	return LIFETIME_POINTS_NAME_FORMAT(m_Points, m_Faults);
}

size_t ARBConfigLifetimePoints::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigLifetimePoints::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_POINTS));
		return false;
	}
	if (Element::eFound != inTree.GetAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_LIFETIME_POINTS, ATTRIB_LIFETIME_POINTS_FAULTS));
		return false;
	}
	return true;
}

bool ARBConfigLifetimePoints::Save(Element& ioTree) const
{
	Element& life = ioTree.AddElement(TREE_LIFETIME_POINTS);
	life.AddAttrib(ATTRIB_LIFETIME_POINTS_POINTS, m_Points);
	life.AddAttrib(ATTRIB_LIFETIME_POINTS_FAULTS, m_Faults);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortConfigLifetimePoints
{
public:
	SortConfigLifetimePoints() {}
	bool operator()(ARBConfigLifetimePoints* one, ARBConfigLifetimePoints* two) const
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
		short inFaults,
		ARBConfigLifetimePoints** outPoints) const
{
	if (outPoints)
		*outPoints = NULL;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetFaults() == inFaults)
		{
			if (outPoints)
			{
				*outPoints = *iter;
				(*outPoints)->AddRef();
			}
			return true;
		}
	}
	return false;
}

bool ARBConfigLifetimePointsList::AddLifetimePoints(
		double inPoints,
		short inFaults,
		ARBConfigLifetimePoints** outPoints)
{
	if (outPoints)
		*outPoints = NULL;
	if (FindLifetimePoints(inFaults))
		return false;
	ARBConfigLifetimePoints* pLife = new ARBConfigLifetimePoints(inPoints, inFaults);
	push_back(pLife);
	sort();
	if (outPoints)
	{
		*outPoints = pLife;
		(*outPoints)->AddRef();
	}
	return true;
}

bool ARBConfigLifetimePointsList::DeleteLifetimePoints(short inFaults)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetFaults() == inFaults)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
