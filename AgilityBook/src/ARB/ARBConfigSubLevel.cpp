/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBConfigSubLevel.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigSubLevel::ARBConfigSubLevel()
	: m_Name()
{
}

ARBConfigSubLevel::ARBConfigSubLevel(ARBConfigSubLevel const& rhs)
	: m_Name(rhs.m_Name)
{
}

ARBConfigSubLevel::~ARBConfigSubLevel()
{
}

//static
ARBConfigSubLevelPtr ARBConfigSubLevel::New()
{
	return ARBConfigSubLevelPtr(new ARBConfigSubLevel());
}

ARBConfigSubLevelPtr ARBConfigSubLevel::Clone() const
{
	return ARBConfigSubLevelPtr(new ARBConfigSubLevel(*this));
}

ARBConfigSubLevel& ARBConfigSubLevel::operator=(ARBConfigSubLevel const& rhs)
{
	if (this != &rhs)
		m_Name = rhs.m_Name;
	return *this;
}

bool ARBConfigSubLevel::operator==(ARBConfigSubLevel const& rhs) const
{
	return m_Name == rhs.m_Name;
}

bool ARBConfigSubLevel::operator!=(ARBConfigSubLevel const& rhs) const
{
	return !operator==(rhs);
}

ARBString ARBConfigSubLevel::GetGenericName() const
{
	return m_Name;
}

size_t ARBConfigSubLevel::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigSubLevel::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_SUBLEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_SUBLEVEL, ATTRIB_SUBLEVEL_NAME));
		return false;
	}
	return true;
}

bool ARBConfigSubLevel::Save(Element& ioTree) const
{
	Element& element = ioTree.AddElement(TREE_SUBLEVEL);
	element.AddAttrib(ATTRIB_SUBLEVEL_NAME, m_Name);
	return true;
}

ARBString const& ARBConfigSubLevel::GetName() const
{
	return m_Name;
}

void ARBConfigSubLevel::SetName(ARBString const& inName)
{
	m_Name = inName;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigSubLevelList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigSubLevelPtr thing(ARBConfigSubLevel::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

bool ARBConfigSubLevelList::FindSubLevel(ARBString const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}

bool ARBConfigSubLevelList::AddSubLevel(
		ARBString const& inName,
		ARBConfigSubLevelPtr* outLevel)
{
	if (outLevel)
		outLevel->reset();
	// The calling function must make sure this name is unique within the division.
	if (0 == inName.length())
		return false;
	ARBConfigSubLevelPtr pLevel(ARBConfigSubLevel::New());
	pLevel->SetName(inName);
	push_back(pLevel);
	if (outLevel)
		*outLevel = pLevel;
	return true;
}

bool ARBConfigSubLevelList::DeleteSubLevel(ARBString const& inName)
{
	ARBString name(inName);
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
