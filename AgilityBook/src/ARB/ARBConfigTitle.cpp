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
 */

#include "StdAfx.h"
#include "ARBConfigTitle.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitle::ARBConfigTitle()
	: m_Name()
	, m_Desc()
{
}

ARBConfigTitle::ARBConfigTitle(const ARBConfigTitle& rhs)
	: m_Name(rhs.m_Name)
	, m_Desc(rhs.m_Desc)
{
}

ARBConfigTitle::~ARBConfigTitle()
{
}

ARBConfigTitle& ARBConfigTitle::operator=(const ARBConfigTitle& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Desc = rhs.m_Desc;
	}
	return *this;
}

bool ARBConfigTitle::operator==(const ARBConfigTitle& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Desc == rhs.m_Desc;
}

bool ARBConfigTitle::operator!=(const ARBConfigTitle& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigTitle::clear()
{
	m_Name.erase();
	m_Desc.erase();
}

size_t ARBConfigTitle::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigTitle::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLE_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLE_NAME);
		return false;
	}
	m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigTitle::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_TITLES);
	title.AddAttrib(ATTRIB_TITLE_NAME, m_Name);
	if (0 < m_Desc.length())
		title.SetValue(m_Desc);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

const ARBConfigTitle* ARBConfigTitleList::FindTitle(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return (*iter);
	}
	return NULL;
}

ARBConfigTitle* ARBConfigTitleList::FindTitle(const std::string& inName)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return (*iter);
	}
	return NULL;
}

ARBConfigTitle* ARBConfigTitleList::AddTitle(const std::string& inName)
{
	if (0 == inName.length())
		return NULL;
	if (FindTitle(inName))
		return NULL;
	ARBConfigTitle* pTitle = new ARBConfigTitle;
	pTitle->SetName(inName);
	push_back(pTitle);
	return pTitle;
}

ARBConfigTitle* ARBConfigTitleList::AddTitle(ARBConfigTitle* inTitle)
{
	if (!inTitle)
		return NULL;
	if (FindTitle(inTitle->GetName()))
		return NULL;
	inTitle->AddRef();
	push_back(inTitle);
	return inTitle;
}

bool ARBConfigTitleList::DeleteTitle(const std::string& inName)
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
