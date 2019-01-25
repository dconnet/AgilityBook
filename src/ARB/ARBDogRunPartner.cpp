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
 * 2003-09-08 Changed AddPartner usage.
 */

#include "stdafx.h"
#include "ARB/ARBDogRunPartner.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDogRunPartner_concrete : public ARBDogRunPartner
	{
	public:
		ARBDogRunPartner_concrete() {}
		ARBDogRunPartner_concrete(ARBDogRunPartner const& rhs)
			: ARBDogRunPartner(rhs)
		{
		}
	};
};


ARBDogRunPartnerPtr ARBDogRunPartner::New()
{
	return std::make_shared<ARBDogRunPartner_concrete>();
}


ARBDogRunPartner::ARBDogRunPartner()
	: m_Handler()
	, m_Dog()
	, m_RegNum()
{
}


ARBDogRunPartner::ARBDogRunPartner(ARBDogRunPartner const& rhs)
	: m_Handler(rhs.m_Handler)
	, m_Dog(rhs.m_Dog)
	, m_RegNum(rhs.m_RegNum)
{
}


ARBDogRunPartner::ARBDogRunPartner(ARBDogRunPartner&& rhs)
	: m_Handler(std::move(rhs.m_Handler))
	, m_Dog(std::move(rhs.m_Dog))
	, m_RegNum(std::move(rhs.m_RegNum))
{
}


ARBDogRunPartner::~ARBDogRunPartner()
{
}


ARBDogRunPartnerPtr ARBDogRunPartner::Clone() const
{
	return std::make_shared<ARBDogRunPartner_concrete>(*this);
}


ARBDogRunPartner& ARBDogRunPartner::operator=(ARBDogRunPartner const& rhs)
{
	if (this != &rhs)
	{
		m_Handler = rhs.m_Handler;
		m_Dog = rhs.m_Dog;
		m_RegNum = rhs.m_RegNum;
	}
	return *this;
}


ARBDogRunPartner& ARBDogRunPartner::operator=(ARBDogRunPartner&& rhs)
{
	if (this != &rhs)
	{
		m_Handler = std::move(rhs.m_Handler);
		m_Dog = std::move(rhs.m_Dog);
		m_RegNum = std::move(rhs.m_RegNum);
	}
	return *this;
}


bool ARBDogRunPartner::operator==(ARBDogRunPartner const& rhs) const
{
	return m_Handler == rhs.m_Handler
		&& m_Dog == rhs.m_Dog
		&& m_RegNum == rhs.m_RegNum;
}


size_t ARBDogRunPartner::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	if (0 < m_Handler.length())
	{
		ioStrings.insert(m_Handler);
		++nItems;
	}

	if (0 < m_Dog.length())
	{
		ioStrings.insert(m_Dog);
		++nItems;
	}

	if (0 < m_RegNum.length())
	{
		ioStrings.insert(m_RegNum);
		++nItems;
	}

	return nItems;
}


bool ARBDogRunPartner::Load(
		ARBConfig const& inConfig,
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_PARTNER)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PARTNER_HANDLER, m_Handler)
	|| 0 == m_Handler.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PARTNER, ATTRIB_PARTNER_HANDLER));
		return false;
	}

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_PARTNER_DOG, m_Dog)
	|| 0 == m_Dog.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_PARTNER, ATTRIB_PARTNER_DOG));
		return false;
	}

	inTree->GetAttrib(ATTRIB_PARTNER_REGNUM, m_RegNum);

	return true;
}


bool ARBDogRunPartner::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr partner = ioTree->AddElementNode(TREE_PARTNER);
	partner->AddAttrib(ATTRIB_PARTNER_HANDLER, m_Handler);
	partner->AddAttrib(ATTRIB_PARTNER_DOG, m_Dog);
	if (0 < m_RegNum.length())
		partner->AddAttrib(ATTRIB_PARTNER_REGNUM, m_RegNum);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogRunPartnerList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogRunPartnerPtr thing(ARBDogRunPartner::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogRunPartnerList::AddPartner(ARBDogRunPartnerPtr const& inPartner)
{
	bool bAdded = false;
	if (inPartner)
	{
		bAdded = true;
		push_back(inPartner);
	}
	return bAdded;
}
