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
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-05 DRC Added LongName.
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
	, m_LongName()
	, m_AllowMany(false)
	, m_Desc()
{
}

ARBConfigTitle::ARBConfigTitle(ARBConfigTitle const& rhs)
	: m_Name(rhs.m_Name)
	, m_LongName(rhs.m_LongName)
	, m_AllowMany(rhs.m_AllowMany)
	, m_Desc(rhs.m_Desc)
{
}

ARBConfigTitle::~ARBConfigTitle()
{
}

ARBConfigTitle& ARBConfigTitle::operator=(ARBConfigTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_LongName = rhs.m_LongName;
		m_AllowMany = rhs.m_AllowMany;
		m_Desc = rhs.m_Desc;
	}
	return *this;
}

bool ARBConfigTitle::operator==(ARBConfigTitle const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_LongName == rhs.m_LongName
		&& m_AllowMany == rhs.m_AllowMany
		&& m_Desc == rhs.m_Desc;
}

bool ARBConfigTitle::operator!=(ARBConfigTitle const& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigTitle::clear()
{
	m_Name.erase();
	m_LongName.erase();
	m_AllowMany = false;
	m_Desc.erase();
}

size_t ARBConfigTitle::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigTitle::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLES_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLES_NAME));
		return false;
	}

	inTree.GetAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLES_ALLOW_MANY, m_AllowMany))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_ALLOW_MANY, VALID_VALUES_BOOL));
		return false;
	}

	m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigTitle::Save(Element& ioTree) const
{
	Element& title = ioTree.AddElement(TREE_TITLES);
	title.AddAttrib(ATTRIB_TITLES_NAME, m_Name);
	if (m_AllowMany)
		title.AddAttrib(ATTRIB_TITLES_ALLOW_MANY, m_AllowMany);
	if (0 < m_LongName.length())
		title.AddAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	if (0 < m_Desc.length())
		title.SetValue(m_Desc);
	return true;
}

std::string ARBConfigTitle::GetCompleteName(short inInstance, bool bAbbrevFirst) const
{
	char buffer[20];
	buffer[0] = 0;
	if (1 < inInstance)
		sprintf(buffer, "%hd", inInstance);
	else if (0 > inInstance && m_AllowMany)
		strcpy(buffer, "+");
	std::string name;
	if (0 < m_LongName.length())
	{
		if (bAbbrevFirst)
		{
			name += "[";
			name += m_Name;
			if (buffer[0])
				name += buffer;
			name += "] ";
		}
		name += m_LongName;
		if (!bAbbrevFirst)
		{
			name += " [";
			name += m_Name;
			if (buffer[0])
				name += buffer;
			name += "]";
		}
	}
	else
	{
		name = m_Name;
		if (buffer[0])
			name += buffer;
	}
	return name;
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitle const* ARBConfigTitleList::FindTitleCompleteName(std::string const& inName, short inInstance, bool bAbbrevFirst) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetCompleteName(inInstance, bAbbrevFirst) == inName)
			return (*iter);
	}
	return NULL;
}

ARBConfigTitle const* ARBConfigTitleList::FindTitle(std::string const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return (*iter);
	}
	return NULL;
}

ARBConfigTitle* ARBConfigTitleList::FindTitle(std::string const& inName)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return (*iter);
	}
	return NULL;
}

ARBConfigTitle* ARBConfigTitleList::AddTitle(std::string const& inName)
{
	if (0 == inName.length())
		return NULL;
	if (FindTitle(inName))
		return NULL;
	ARBConfigTitle* pTitle = new ARBConfigTitle();
	pTitle->SetName(inName);
	push_back(pTitle);
	return pTitle;
}

ARBConfigTitle* ARBConfigTitleList::AddTitle(ARBConfigTitle* inTitle)
{
	if (!inTitle || 0 == inTitle->GetName().length())
		return NULL;
	if (FindTitle(inTitle->GetName()))
		return NULL;
	inTitle->AddRef();
	push_back(inTitle);
	return inTitle;
}

bool ARBConfigTitleList::DeleteTitle(std::string const& inName)
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
