/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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

ARBConfigSubLevel::ARBConfigSubLevel(const ARBConfigSubLevel& rhs)
	: m_Name(rhs.m_Name)
{
}

ARBConfigSubLevel::~ARBConfigSubLevel()
{
}

ARBConfigSubLevel& ARBConfigSubLevel::operator=(const ARBConfigSubLevel& rhs)
{
	if (this != &rhs)
		m_Name = rhs.m_Name;
	return *this;
}

bool ARBConfigSubLevel::operator==(const ARBConfigSubLevel& rhs) const
{
	return m_Name == rhs.m_Name;
}

bool ARBConfigSubLevel::operator!=(const ARBConfigSubLevel& rhs) const
{
	return !operator==(rhs);
}

bool ARBConfigSubLevel::Load(
	const CElement& inTree,
	int inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_SUBLEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ErrorMissingAttribute(TREE_SUBLEVEL, ATTRIB_SUBLEVEL_NAME);
		return false;
	}
	return true;
}

bool ARBConfigSubLevel::Save(CElement& ioTree) const
{
	CElement& element = ioTree.AddElement(TREE_SUBLEVEL);
	element.AddAttrib(ATTRIB_SUBLEVEL_NAME, m_Name);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigSubLevelList::FindSubLevel(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}

ARBConfigSubLevel* ARBConfigSubLevelList::AddSubLevel(const std::string& inName)
{
	// The calling function must make sure this name is unique within the division.
	if (0 == inName.length())
		return NULL;
	ARBConfigSubLevel* pLevel = new ARBConfigSubLevel;
	pLevel->SetName(inName);
	push_back(pLevel);
	return pLevel;
}

bool ARBConfigSubLevelList::DeleteSubLevel(const std::string& inName)
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
