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
 * 2016-04-19 Fix loading name.
 * 2016-03-08 Fix loading empty element.
 * 2016-01-06 Created
 */

#include "stdafx.h"
#include "ARB/ARBConfigLifetimeName.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBConfigLifetimeName_concrete : public ARBConfigLifetimeName
	{
	public:
		ARBConfigLifetimeName_concrete() {}
		ARBConfigLifetimeName_concrete(ARBConfigLifetimeName const& rhs)
			: ARBConfigLifetimeName(rhs)
		{
		}
	};
};


ARBConfigLifetimeNamePtr ARBConfigLifetimeName::New()
{
	return std::make_shared<ARBConfigLifetimeName_concrete>();
}


ARBConfigLifetimeName::ARBConfigLifetimeName()
	: m_Name()
{
}


ARBConfigLifetimeName::ARBConfigLifetimeName(ARBConfigLifetimeName const& rhs)
	: m_Name(rhs.m_Name)
{
}


ARBConfigLifetimeName::~ARBConfigLifetimeName()
{
}


ARBConfigLifetimeNamePtr ARBConfigLifetimeName::Clone() const
{
	return std::make_shared<ARBConfigLifetimeName_concrete>(*this);
}


ARBConfigLifetimeName& ARBConfigLifetimeName::operator=(ARBConfigLifetimeName const& rhs)
{
	if (this != &rhs)
		m_Name = rhs.m_Name;
	return *this;
}


bool ARBConfigLifetimeName::operator==(ARBConfigLifetimeName const& rhs) const
{
	return m_Name == rhs.m_Name;
}


bool ARBConfigLifetimeName::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_VENUE_LIFETIMENAMES)
		return false;
	// Note: an empty name is allowed. That will be the default lifetime name.
	inTree->GetAttrib(ATTRIB_LIFETIMENAMES_NAME, m_Name);
	return true;
}


bool ARBConfigLifetimeName::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr element = ioTree->AddElementNode(TREE_VENUE_LIFETIMENAMES);
	if (0 < m_Name.length())
		element->AddAttrib(ATTRIB_LIFETIMENAMES_NAME, m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigLifetimeNameList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigLifetimeNamePtr thing(ARBConfigLifetimeName::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigLifetimeNameList::FindLifetimeName(
		std::wstring const& inName,
		ARBConfigLifetimeNamePtr* outLifetimeName) const
{
	if (outLifetimeName)
		outLifetimeName->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outLifetimeName)
				*outLifetimeName = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigLifetimeNameList::AddLifetimeName(
		std::wstring const& inName,
		ARBConfigLifetimeNamePtr* outLifetimeName)
{
	if (outLifetimeName)
		outLifetimeName->reset();
	if (FindLifetimeName(inName))
		return false;
	ARBConfigLifetimeNamePtr pLifetimeName(ARBConfigLifetimeName::New());
	pLifetimeName->SetName(inName);
	push_back(pLifetimeName);
	if (outLifetimeName)
		*outLifetimeName = pLifetimeName;
	return true;
}


bool ARBConfigLifetimeNameList::DeleteLifetimeName(std::wstring const& inName)
{
	std::wstring name(inName);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}
