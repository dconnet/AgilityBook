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
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#include "ARBConfigLevel.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigLevel::ARBConfigLevel()
	: m_Name()
	, m_SubLevels()
{
}

ARBConfigLevel::ARBConfigLevel(const ARBConfigLevel& rhs)
	: m_Name(rhs.m_Name)
	, m_SubLevels(rhs.m_SubLevels)
{
}

ARBConfigLevel::~ARBConfigLevel()
{
}

ARBConfigLevel& ARBConfigLevel::operator=(const ARBConfigLevel& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_SubLevels = rhs.m_SubLevels;
	}
	return *this;
}

bool ARBConfigLevel::operator==(const ARBConfigLevel& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_SubLevels == rhs.m_SubLevels;
}

bool ARBConfigLevel::operator!=(const ARBConfigLevel& rhs) const
{
	return !operator==(rhs);
}

size_t ARBConfigLevel::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigLevel::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_LEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ErrorMissingAttribute(TREE_LEVEL, ATTRIB_LEVEL_NAME);
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		if (element.GetName() == TREE_SUBLEVEL)
		{
			// Ignore any errors...
			m_SubLevels.Load(element, inVersion);
		}
	}
	return true;
}

bool ARBConfigLevel::Save(CElement& ioTree) const
{
	CElement& level = ioTree.AddElement(TREE_LEVEL);
	level.AddAttrib(ATTRIB_LEVEL_NAME, m_Name);
	if (!m_SubLevels.Save(level))
		return false;
	return true;
}

std::string ARBConfigLevel::Update(int indent, const ARBConfigLevel* inLevelNew)
{
	std::string info;
	if (GetName() != inLevelNew->GetName())
		return info;

	std::string indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += "   ";
	indentBuffer = indentName + "   ";
	indentName += "-";

	if (GetSubLevels() != inLevelNew->GetSubLevels())
	{
		int nAdded = 0;
		for (ARBConfigSubLevelList::const_iterator iterSub = inLevelNew->GetSubLevels().begin();
			iterSub != inLevelNew->GetSubLevels().end();
			++iterSub)
		{
			// Here, we happen to know a sublevel consists of only a name.
			if (!GetSubLevels().FindSubLevel((*iterSub)->GetName()))
			{
				++nAdded;
				GetSubLevels().AddSubLevel((*iterSub)->GetName());
			}
		}
		if (0 < nAdded)
		{
			char buffer[100];
			sprintf(buffer, UPDATE_FORMAT_SUBLEVELS, nAdded);
			info += indentBuffer;
			info += buffer;
		}
	}
	if (0 < info.length())
		info = indentName + GetName() + "\n" + info;
	return info;
}

void ARBConfigLevel::clear()
{
	m_Name.erase();
	m_SubLevels.clear();
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigLevelList::VerifyLevel(const std::string& inName, bool inAllowWildCard) const
{
	// Wildcards are only used in the ARBConfigScoring object.
	if (inAllowWildCard && inName == WILDCARD_LEVEL)
		return true;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}

ARBConfigLevel* ARBConfigLevelList::FindLevel(const std::string& inName)
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return *iter;
	}
	return NULL;
}

// When finding a level, we're only looking at leaf nodes.
bool ARBConfigLevelList::FindTrueLevel(const std::string& inName) const
{
	return NULL != FindSubLevel(inName);
}

const ARBConfigLevel* ARBConfigLevelList::FindSubLevel(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (0 < (*iter)->GetSubLevels().size())
		{
			if ((*iter)->GetSubLevels().FindSubLevel(inName))
				return *iter;
		}
		else
		{
			if ((*iter)->GetName() == inName)
				return *iter;
		}
	}
	return NULL;
}

ARBConfigLevel* ARBConfigLevelList::AddLevel(const std::string& inName)
{
	if (0 == inName.length())
		return NULL;
	if (FindTrueLevel(inName))
		return NULL;
	ARBConfigLevel* pLevel = new ARBConfigLevel;
	pLevel->SetName(inName);
	push_back(pLevel);
	return pLevel;
}

ARBConfigLevel* ARBConfigLevelList::AddLevel(ARBConfigLevel* inLevel)
{
	if (!inLevel)
		return NULL;
	if (FindTrueLevel(inLevel->GetName()))
		return NULL;
	inLevel->AddRef();
	push_back(inLevel);
	return inLevel;
}

bool ARBConfigLevelList::DeleteLevel(const std::string& inName, ARBConfigEventList& ioEvents)
{
	std::string name(inName);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			// Remember, we don't need to worry about the sublevel names here.
			// Events only use level names.
			ioEvents.DeleteLevel(name);
			erase(iter);
			return true;
		}
	}
	return false;
}

bool ARBConfigLevelList::DeleteSubLevel(const std::string& inName, bool& outLevelModified)
{
	std::string name(inName);
	outLevelModified = false;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBConfigSubLevelList::iterator iterSub = (*iter)->GetSubLevels().begin(); iterSub != (*iter)->GetSubLevels().end(); ++iterSub)
		{
			if ((*iterSub)->GetName() == name)
			{
				if (1 == (*iter)->GetSubLevels().size())
				{
					// At this point, we know all the items at 'level' are
					// unique. However, it is possible (because of the more
					// relaxed nature of a level that has sublevels) that by
					// deleting this sublevel, we may have created a situation
					// where (*iter)->GetName() (the parent) is now in conflict
					// with an existing sublevel (of a different level)
					// Start with:
					//  level1-sub1
					//  level2-sub2
					//        -level1
					// The above level names are valid. But deleting sublevel
					// 'sub1' now creates a conflict for 'level1'.
					// So, before deleting the node, search for a leaf of the
					// level that is about to become a leaf - if we find one,
					// we'll get creative and auto-rename.
					std::string newName = (*iter)->GetName();
					while (FindTrueLevel(newName))
					{
						outLevelModified = true;
						newName += "?";
						(*iter)->SetName(newName);
					}
				}
				(*iter)->GetSubLevels().erase(iterSub);
				return true;
			}
		}
	}
	return false;
}
