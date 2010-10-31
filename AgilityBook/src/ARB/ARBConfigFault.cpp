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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBConfigFault.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigFaultPtr ARBConfigFault::New()
{
	return ARBConfigFaultPtr(new ARBConfigFault());
}


ARBConfigFault::ARBConfigFault()
	: m_Name()
{
}


ARBConfigFault::ARBConfigFault(ARBConfigFault const& rhs)
	: m_Name(rhs.m_Name)
{
}


ARBConfigFault::~ARBConfigFault()
{
}


ARBConfigFaultPtr ARBConfigFault::Clone() const
{
	return ARBConfigFaultPtr(new ARBConfigFault(*this));
}


ARBConfigFault& ARBConfigFault::operator=(ARBConfigFault const& rhs)
{
	if (this != &rhs)
		m_Name = rhs.m_Name;
	return *this;
}


bool ARBConfigFault::operator==(ARBConfigFault const& rhs) const
{
	return m_Name == rhs.m_Name;
}


bool ARBConfigFault::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_FAULTTYPE)
		return false;
	if (inVersion == ARBVersion(1,0))
	{
		if (ElementNode::eFound != inTree->GetAttrib(wxT("Name"), m_Name)
		|| 0 == m_Name.length())
		{
			ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_FAULTTYPE, wxT("Name")));
			return false;
		}
	}
	else
		m_Name = inTree->GetValue();
	return true;
}


bool ARBConfigFault::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr element = ioTree->AddElementNode(TREE_FAULTTYPE);
	if (0 < m_Name.length())
		element->SetValue(m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigFaultList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigFaultPtr thing(ARBConfigFault::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigFaultList::FindFault(
		wxString const& inName,
		ARBConfigFaultPtr* outFault) const
{
	if (outFault)
		outFault->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outFault)
				*outFault = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigFaultList::AddFault(
		wxString const& inName,
		ARBConfigFaultPtr* outFault)
{
	if (outFault)
		outFault->reset();
	if (0 == inName.length())
		return false;
	ARBConfigFaultPtr pFault(ARBConfigFault::New());
	pFault->SetName(inName);
	push_back(pFault);
	if (outFault)
		*outFault = pFault;
	return true;
}


bool ARBConfigFaultList::DeleteFault(wxString const& inName)
{
	wxString name(inName);
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