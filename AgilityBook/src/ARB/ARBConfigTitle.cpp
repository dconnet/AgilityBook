/*
 * Copyright � 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-03-11 DRC Added 'Requires', moved desc to 'Desc'.
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
	, m_Desc()
	, m_Requires()
{
}

ARBConfigTitle::ARBConfigTitle(const ARBConfigTitle& rhs)
	: m_Name(rhs.m_Name)
	, m_LongName(rhs.m_LongName)
	, m_Desc(rhs.m_Desc)
	, m_Requires(rhs.m_Requires)
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
		m_LongName = rhs.m_LongName;
		m_Desc = rhs.m_Desc;
		m_Requires = rhs.m_Requires;
	}
	return *this;
}

bool ARBConfigTitle::operator==(const ARBConfigTitle& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_LongName == rhs.m_LongName
		&& m_Desc == rhs.m_Desc
		&& m_Requires == rhs.m_Requires;
}

bool ARBConfigTitle::operator!=(const ARBConfigTitle& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigTitle::clear()
{
	m_Name.erase();
	m_LongName.erase();
	m_Desc.erase();
	m_Requires.clear();
}

size_t ARBConfigTitle::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigTitle::Load(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_TITLES_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLES_NAME);
		return false;
	}
	inTree.GetAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	if (inVersion < ARBVersion(9,0))
		m_Desc = inTree.GetValue();
	else
	{
		for (int i = 0; i < inTree.GetElementCount(); ++i)
		{
			const CElement& element = inTree.GetElement(i);
			const std::string& name = element.GetName();
			if (name == TREE_TITLES_REQUIRES)
			{
				m_Requires.insert(element.GetValue());
			}
			else if (name == TREE_TITLES_DESC)
				m_Desc = element.GetValue();
		}
	}
	return true;
}

bool ARBConfigTitle::Save(CElement& ioTree) const
{
	CElement& title = ioTree.AddElement(TREE_TITLES);
	title.AddAttrib(ATTRIB_TITLES_NAME, m_Name);
	if (0 < m_LongName.length())
		title.AddAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	for (ARBConfigTitleRequiresList::const_iterator iter = m_Requires.begin();
		iter != m_Requires.end();
		++iter)
	{
		CElement& req = title.AddElement(TREE_TITLES_REQUIRES);
		req.SetValue(*iter);
	}
	if (0 < m_Desc.length())
	{
		CElement& desc = title.AddElement(TREE_TITLES_DESC);
		desc.SetValue(m_Desc);
	}
	return true;
}

std::string ARBConfigTitle::GetCompleteName(bool bAbbrevFirst) const
{
	std::string name;
	if (0 < m_LongName.length())
	{
		if (bAbbrevFirst)
		{
			name += "[";
			name += m_Name;
			name += "] ";
		}
		name += m_LongName;
		if (!bAbbrevFirst)
		{
			name += " [";
			name += m_Name;
			name += "]";
		}
	}
	else
		name = m_Name;
	return name;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitleList::VerifyRequiresLists()
{
	iterator iter;
	ARBConfigTitleRequiresList items;
	ARBConfigTitleRequiresList::iterator iterItem, iterReq;
	for (iter = begin(); iter != end(); ++iter)
	{
		for (iterReq = (*iter)->GetRequires().begin();
			iterReq != (*iter)->GetRequires().end();
			++iterReq)
		{
			items.insert(*iterReq);
		}
	}
	for (iterItem = items.begin(); iterItem != items.end(); )
	{
		ARBConfigTitle* pTitle = FindTitle(*iterItem);
		if (pTitle)
			iterItem = items.erase(iterItem);
		else
			++iterItem;
	}
	// Remove titles not in this division.
	// Note, this shouldn't actually happen - if the UI works properly!
	// This is really more of a paranoid test.
	for (iterItem = items.begin(); iterItem != items.end(); ++iterItem)
	{
		for (iter = begin(); iter != end(); ++iter)
		{
			for (iterReq = (*iter)->GetRequires().begin();
				iterReq != (*iter)->GetRequires().end();
				)
			{
				if (*iterItem == *iterReq)
					iterReq = (*iter)->GetRequires().erase(iterReq);
				else
					++iterReq;
			}
		}
	}
	return true;
}

bool ARBConfigTitleList::Requires(const ARBConfigTitle* inTitle, const std::string& inName) const
{
	if (!inTitle)
		return false;
	for (ARBConfigTitleRequiresList::const_iterator iterReq = inTitle->GetRequires().begin();
		iterReq != inTitle->GetRequires().end();
		++iterReq)
	{
		if (*iterReq == inName)
			return true;
		const ARBConfigTitle* pTitle = FindTitle(*iterReq);
		if (pTitle)
		{
			if (Requires(pTitle, inName))
				return true;
		}
	}
	return false;
}

const ARBConfigTitle* ARBConfigTitleList::FindTitleCompleteName(const std::string& inName, bool bAbbrevFirst) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetCompleteName(bAbbrevFirst) == inName)
			return (*iter);
	}
	return NULL;
}

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

int ARBConfigTitleList::RenameTitle(const std::string& inOldName, const std::string& inNewName)
{
	int nChanged = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inOldName)
		{
			(*iter)->SetName(inNewName);
			++nChanged;
		}
		bool bAdd = false;
		for (ARBConfigTitleRequiresList::iterator iterReq = (*iter)->GetRequires().begin();
			iterReq != (*iter)->GetRequires().end();
			)
		{
			if (*iterReq == inOldName)
			{
				bAdd = true;
				iterReq = (*iter)->GetRequires().erase(iterReq);
				++nChanged;
			}
			else
				++iterReq;
		}
		if (bAdd)
			(*iter)->GetRequires().insert(inNewName);
	}
	return nChanged;
}

bool ARBConfigTitleList::DeleteTitle(const std::string& inName)
{
	bool bDeleteTitle = false;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			erase(iter);
			bDeleteTitle = true;
			break;
		}
	}
	// Cleanup required links.
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigTitleRequiresList::iterator iterReq = (*iter)->GetRequires().begin();
			iterReq != (*iter)->GetRequires().end();
			)
		{
			if (*iterReq == inName)
				iterReq = (*iter)->GetRequires().erase(iterReq);
			else
				++iterReq;
		}
	}
	return bDeleteTitle;
}
