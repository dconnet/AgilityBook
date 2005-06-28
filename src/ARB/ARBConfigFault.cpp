/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBConfigFault.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

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

bool ARBConfigFault::operator!=(ARBConfigFault const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBConfigFault::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigFault::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (inVersion == ARBVersion(1,0))
	{
		if (Element::eFound != inTree.GetAttrib("Name", m_Name)
		|| 0 == m_Name.length())
		{
			ioCallback.LogMessage(ErrorMissingAttribute(TREE_FAULTTYPE, "Name"));
			return false;
		}
	}
	else
		m_Name = inTree.GetValue();
	return true;
}

bool ARBConfigFault::Save(Element& ioTree) const
{
	Element& element = ioTree.AddElement(TREE_FAULTTYPE);
	element.SetValue(m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigFaultList::FindFault(
		std::string const& inName,
		ARBConfigFault** outFault) const
{
	if (outFault)
		*outFault = NULL;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outFault)
			{
				*outFault = *iter;
				(*outFault)->AddRef();
			}
			return true;
		}
	}
	return false;
}

bool ARBConfigFaultList::AddFault(
		std::string const& inName,
		ARBConfigFault** outFault)
{
	if (outFault)
		*outFault = NULL;
	if (0 == inName.length())
		return false;
	ARBConfigFault* pFault = new ARBConfigFault;
	pFault->SetName(inName);
	push_back(pFault);
	if (outFault)
	{
		*outFault = pFault;
		(*outFault)->AddRef();
	}
	return true;
}

bool ARBConfigFaultList::DeleteFault(std::string const& inName)
{
	std::string name(inName);
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
