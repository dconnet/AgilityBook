/*
 * Copyright © 2006-2007 David Connet. All Rights Reserved.
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
 * @li 2006-11-03 DRC Created
 */

#include "StdAfx.h"
#include <algorithm>
#include "ARBConfigPlaceInfo.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::New()
{
	return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo());
}

ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::New(short inPlace, double inValue)
{
	return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo(inPlace, inValue));
}

ARBConfigPlaceInfo::ARBConfigPlaceInfo()
	: m_Place(0)
	, m_Value(0.0)
{
}

ARBConfigPlaceInfo::ARBConfigPlaceInfo(
		short inPlace,
		double inValue)
	: m_Place(inPlace)
	, m_Value(inValue)
{
}

ARBConfigPlaceInfo::ARBConfigPlaceInfo(ARBConfigPlaceInfo const& rhs)
	: m_Place(rhs.m_Place)
	, m_Value(rhs.m_Value)
{
}

ARBConfigPlaceInfo::~ARBConfigPlaceInfo()
{
}

ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::Clone() const
{
	return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo(*this));
}

ARBConfigPlaceInfo& ARBConfigPlaceInfo::operator=(ARBConfigPlaceInfo const& rhs)
{
	if (this != &rhs)
	{
		m_Place = rhs.m_Place;
		m_Value = rhs.m_Value;
	}
	return *this;
}

bool ARBConfigPlaceInfo::operator==(ARBConfigPlaceInfo const& rhs) const
{
	return m_Place == rhs.m_Place
		&& m_Value == rhs.m_Value;
}

ARBString ARBConfigPlaceInfo::GetGenericName() const
{
	return PLACEMENT_POINTS_NAME_FORMAT(m_Value, m_Place);
}

bool ARBConfigPlaceInfo::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_PLACE_INFO_PLACE, m_Place))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_PLACE_INFO, ATTRIB_PLACE_INFO_PLACE));
		return false;
	}
	if (Element::eFound != inTree.GetAttrib(ATTRIB_PLACE_INFO_VALUE, m_Value))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_PLACE_INFO, ATTRIB_PLACE_INFO_VALUE));
		return false;
	}
	return true;
}

bool ARBConfigPlaceInfo::Save(Element& ioTree) const
{
	Element& title = ioTree.AddElement(TREE_PLACE_INFO);
	title.AddAttrib(ATTRIB_PLACE_INFO_PLACE, m_Place);
	title.AddAttrib(ATTRIB_PLACE_INFO_VALUE, m_Value, 0);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigPlaceInfoList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigPlaceInfoPtr thing(ARBConfigPlaceInfo::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

class SortConfigPlaceInfo
{
public:
	SortConfigPlaceInfo() {}
	bool operator()(ARBConfigPlaceInfoPtr one, ARBConfigPlaceInfoPtr two) const
	{
		return one->GetPlace() < two->GetPlace();
	}
};

void ARBConfigPlaceInfoList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortConfigPlaceInfo());
}

bool ARBConfigPlaceInfoList::GetPlaceInfo(short inPlace, double &outValue) const
{
	ARBConfigPlaceInfoPtr place;
	bool bOk = FindPlaceInfo(inPlace, &place);
	if (bOk)
	{
		outValue = place->GetValue();
	}
	return bOk;
}

bool ARBConfigPlaceInfoList::FindPlaceInfo(
		short inPlace,
		ARBConfigPlaceInfoPtr* outPlace) const
{
	if (outPlace)
		outPlace->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (inPlace == (*iter)->GetPlace())
		{
			if (outPlace)
				*outPlace = *iter;
			return true;
		}
	}
	return false;
}

bool ARBConfigPlaceInfoList::AddPlaceInfo(
		short inPlace,
		double inValue,
		ARBConfigPlaceInfoPtr* outPlace)
{
	if (outPlace)
		outPlace->reset();
	if (FindPlaceInfo(inPlace))
		return false;
	ARBConfigPlaceInfoPtr pPlace(ARBConfigPlaceInfo::New(inPlace, inValue));
	push_back(pPlace);
	sort();
	if (outPlace)
		*outPlace = pPlace;
	return true;
}

bool ARBConfigPlaceInfoList::DeletePlaceInfo(short inPlace)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if (inPlace == (*iter)->GetPlace())
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
