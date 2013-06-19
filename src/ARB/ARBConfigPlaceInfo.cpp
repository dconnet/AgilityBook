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
 * @li 2009-02-22 DRC Added wildcard placement points (for UK Warrant points)
 * @li 2006-11-03 DRC Created
 */

#include "stdafx.h"
#include "ARB/ARBConfigPlaceInfo.h"

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
	class ARBConfigPlaceInfo_concrete : public ARBConfigPlaceInfo
	{
	public:
		ARBConfigPlaceInfo_concrete() {}
		ARBConfigPlaceInfo_concrete(short inPlace, double inValue, bool bMustQ)
			: ARBConfigPlaceInfo(inPlace, inValue, bMustQ)
		{
		}
		ARBConfigPlaceInfo_concrete(ARBConfigPlaceInfo const& rhs)
			: ARBConfigPlaceInfo(rhs)
		{
		}
	};
};


ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::New()
{
	return std::make_shared<ARBConfigPlaceInfo_concrete>();
}


ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::New(
		short inPlace,
		double inValue,
		bool bMustQ)
{
	return std::make_shared<ARBConfigPlaceInfo_concrete>(inPlace, inValue, bMustQ);
}


ARBConfigPlaceInfo::ARBConfigPlaceInfo()
	: m_Place(0)
	, m_Value(0.0)
	, m_MustQ(true)
{
}


ARBConfigPlaceInfo::ARBConfigPlaceInfo(
		short inPlace,
		double inValue,
		bool bMustQ)
	: m_Place(inPlace)
	, m_Value(inValue)
	, m_MustQ(bMustQ)
{
}


ARBConfigPlaceInfo::ARBConfigPlaceInfo(ARBConfigPlaceInfo const& rhs)
	: m_Place(rhs.m_Place)
	, m_Value(rhs.m_Value)
	, m_MustQ(rhs.m_MustQ)
{
}


ARBConfigPlaceInfo::~ARBConfigPlaceInfo()
{
}


ARBConfigPlaceInfoPtr ARBConfigPlaceInfo::Clone() const
{
	return std::make_shared<ARBConfigPlaceInfo_concrete>(*this);
}


ARBConfigPlaceInfo& ARBConfigPlaceInfo::operator=(ARBConfigPlaceInfo const& rhs)
{
	if (this != &rhs)
	{
		m_Place = rhs.m_Place;
		m_Value = rhs.m_Value;
		m_MustQ = rhs.m_MustQ;
	}
	return *this;
}


bool ARBConfigPlaceInfo::operator==(ARBConfigPlaceInfo const& rhs) const
{
	return m_Place == rhs.m_Place
		&& m_Value == rhs.m_Value
		&& m_MustQ == rhs.m_MustQ;
}


std::wstring ARBConfigPlaceInfo::GetGenericName() const
{
	return Localization()->PlacementPointsNameFormat(m_Value, m_Place);
}


bool ARBConfigPlaceInfo::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_PLACE_INFO)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PLACE_INFO_PLACE, m_Place))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PLACE_INFO, ATTRIB_PLACE_INFO_PLACE));
		return false;
	}
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PLACE_INFO_VALUE, m_Value))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PLACE_INFO, ATTRIB_PLACE_INFO_VALUE));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_PLACE_INFO_MUSTQ, m_MustQ))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_PLACE_INFO, ATTRIB_PLACE_INFO_MUSTQ, Localization()->ValidValuesBool().c_str()));
		return false;
	}
	return true;
}


bool ARBConfigPlaceInfo::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_PLACE_INFO);
	title->AddAttrib(ATTRIB_PLACE_INFO_PLACE, m_Place);
	title->AddAttrib(ATTRIB_PLACE_INFO_VALUE, m_Value, 0);
	if (!m_MustQ)
		title->AddAttrib(ATTRIB_PLACE_INFO_MUSTQ, m_MustQ);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigPlaceInfoList::Load(
		ElementNodePtr inTree,
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
	// Special case: -1: wildcard match if none of the others matched
	if (0 < inPlace)
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (-1 == (*iter)->GetPlace())
			{
				if (outPlace)
					*outPlace = *iter;
				return true;
			}
		}
	}
	return false;
}


bool ARBConfigPlaceInfoList::AddPlaceInfo(
		short inPlace,
		double inValue,
		bool inMustQ,
		ARBConfigPlaceInfoPtr* outPlace)
{
	if (outPlace)
		outPlace->reset();
	if (FindPlaceInfo(inPlace))
		return false;
	ARBConfigPlaceInfoPtr pPlace(ARBConfigPlaceInfo::New(inPlace, inValue, inMustQ));
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
