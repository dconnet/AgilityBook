/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-12 DRC Added as part of file version 5.
 */

#include "StdAfx.h"
#include <algorithm>
#include <sstream>
#include "ARBConfigTitlePoints.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitlePoints::ARBConfigTitlePoints()
	: m_Points(0)
	, m_Faults(0)
{
}

ARBConfigTitlePoints::ARBConfigTitlePoints(short inPoints, short inFaults)
	: m_Points(inPoints)
	, m_Faults(inFaults)
{
}

ARBConfigTitlePoints::ARBConfigTitlePoints(const ARBConfigTitlePoints& rhs)
	: m_Points(rhs.m_Points)
	, m_Faults(rhs.m_Faults)
{
}

ARBConfigTitlePoints::~ARBConfigTitlePoints()
{
}

ARBConfigTitlePoints& ARBConfigTitlePoints::operator=(const ARBConfigTitlePoints& rhs)
{
	if (this != &rhs)
	{
		m_Points = rhs.m_Points;
		m_Faults = rhs.m_Faults;
	}
	return *this;
}

bool ARBConfigTitlePoints::operator==(const ARBConfigTitlePoints& rhs) const
{
	return m_Points == rhs.m_Points
		&& m_Faults == rhs.m_Faults;
}

bool ARBConfigTitlePoints::operator!=(const ARBConfigTitlePoints& rhs) const
{
	return !operator==(rhs);
}

std::string ARBConfigTitlePoints::GetGenericName() const
{
	char buffer[100];
	sprintf(buffer, TITLE_POINTS_NAME_FORMAT, m_Points, m_Faults);
	return buffer;
}

size_t ARBConfigTitlePoints::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigTitlePoints::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points))
	{
		ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_POINTS);
		return false;
	}
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults))
	{
		ErrorMissingAttribute(TREE_TITLE_POINTS, ATTRIB_TITLE_POINTS_FAULTS);
		return false;
	}
	return true;
}

bool ARBConfigTitlePoints::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_TITLE_POINTS);
	title.AddAttrib(ATTRIB_TITLE_POINTS_POINTS, m_Points);
	title.AddAttrib(ATTRIB_TITLE_POINTS_FAULTS, m_Faults);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortConfigTitlePoints
{
public:
	SortConfigTitlePoints() {}
	bool operator()(ARBConfigTitlePoints* one, ARBConfigTitlePoints* two) const
	{
		return one->GetFaults() < two->GetFaults();
	}
};

void ARBConfigTitlePointsList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigTitlePoints());
}

short ARBConfigTitlePointsList::GetTitlePoints(double inFaults) const
{
	// This is why we keep the list sorted!
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (inFaults <= static_cast<double>((*iter)->GetFaults()))
			return (*iter)->GetPoints();
	}
	return 0;
}

const ARBConfigTitlePoints* ARBConfigTitlePointsList::FindTitlePoints(short inFaults) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetFaults() == inFaults)
			return *iter;
	}
	return NULL;
}

ARBConfigTitlePoints* ARBConfigTitlePointsList::AddTitlePoints(short inPoints, short inFaults)
{
	if (NULL != FindTitlePoints(inFaults))
		return NULL;
	ARBConfigTitlePoints* pTitle = new ARBConfigTitlePoints(inPoints, inFaults);
	push_back(pTitle);
	sort();
	return pTitle;
}

bool ARBConfigTitlePointsList::DeleteTitlePoints(short inFaults)
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
