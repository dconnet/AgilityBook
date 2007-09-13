/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigSubLevelPtr ARBConfigSubLevel::New()
{
	return ARBConfigSubLevelPtr(new ARBConfigSubLevel());
}


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


bool ARBConfigSubLevel::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_SUBLEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_SUBLEVEL, ATTRIB_SUBLEVEL_NAME));
		return false;
	}
	return true;
}


bool ARBConfigSubLevel::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr element = ioTree->AddElementNode(TREE_SUBLEVEL);
	element->AddAttrib(ATTRIB_SUBLEVEL_NAME, m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigSubLevelList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigSubLevelPtr thing(ARBConfigSubLevel::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigSubLevelList::FindSubLevel(tstring const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}


bool ARBConfigSubLevelList::AddSubLevel(
		tstring const& inName,
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


bool ARBConfigSubLevelList::DeleteSubLevel(tstring const& inName)
{
	tstring name(inName);
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
