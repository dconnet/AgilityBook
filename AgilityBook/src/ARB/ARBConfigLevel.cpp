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
 * @li 2004-01-27 DRC Updating could cause some false-positive messages because
 *                    the ordering was different.
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

ARBConfigLevelPtr ARBConfigLevel::New()
{
	return ARBConfigLevelPtr(new ARBConfigLevel());
}

ARBConfigLevel::ARBConfigLevel()
	: m_Name()
	, m_SubLevels()
{
}

ARBConfigLevel::ARBConfigLevel(ARBConfigLevel const& rhs)
	: m_Name(rhs.m_Name)
	, m_SubLevels()
{
	rhs.m_SubLevels.Clone(m_SubLevels);
}

ARBConfigLevel::~ARBConfigLevel()
{
}

ARBConfigLevelPtr ARBConfigLevel::Clone() const
{
	return ARBConfigLevelPtr(new ARBConfigLevel(*this));
}

ARBConfigLevel& ARBConfigLevel::operator=(ARBConfigLevel const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		rhs.m_SubLevels.Clone(m_SubLevels);
	}
	return *this;
}

bool ARBConfigLevel::operator==(ARBConfigLevel const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_SubLevels == rhs.m_SubLevels;
}

void ARBConfigLevel::clear()
{
	m_Name.erase();
	m_SubLevels.clear();
}

bool ARBConfigLevel::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_LEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_LEVEL, ATTRIB_LEVEL_NAME));
		return false;
	}
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_SUBLEVEL)
		{
			// Ignore any errors...
			m_SubLevels.Load(element, inVersion, ioCallback);
		}
	}
	return true;
}

bool ARBConfigLevel::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr level = ioTree->AddElementNode(TREE_LEVEL);
	level->AddAttrib(ATTRIB_LEVEL_NAME, m_Name);
	if (!m_SubLevels.Save(level))
		return false;
	return true;
}

bool ARBConfigLevel::Update(
		int indent,
		ARBConfigLevelPtr inLevelNew,
		ARBString& ioInfo)
{
	ARBString info;
	if (GetName() != inLevelNew->GetName())
		return false;

	ARBString indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += _T("   ");
	indentBuffer = indentName + _T("   ");
	indentName += _T("-");

	// If the order is different, we will fall into this...
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
			info += indentBuffer;
			info += UPDATE_FORMAT_SUBLEVELS(nAdded);
		}
	}
	bool bChanges = false;
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += indentName + GetName() + _T("\n") + info;
	}
	return bChanges;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigLevelList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigLevelPtr thing(ARBConfigLevel::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

bool ARBConfigLevelList::VerifyLevel(
		ARBString const& inName,
		bool inAllowWildCard) const
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

bool ARBConfigLevelList::FindLevel(
		ARBString const& inName,
		ARBConfigLevelPtr* outLevel)
{
	if (outLevel)
		outLevel->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outLevel)
				*outLevel = *iter;
			return true;
		}
	}
	return false;
}

bool ARBConfigLevelList::FindSubLevel(
		ARBString const& inName,
		ARBConfigLevelPtr* outLevel) const
{
	if (outLevel)
		outLevel->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (0 < (*iter)->GetSubLevels().size())
		{
			if ((*iter)->GetSubLevels().FindSubLevel(inName))
			{
				if (outLevel)
					*outLevel = *iter;
				return true;
			}
		}
		else
		{
			if ((*iter)->GetName() == inName)
			{
				if (outLevel)
					*outLevel = *iter;
				return true;
			}
		}
	}
	return false;
}

bool ARBConfigLevelList::AddLevel(
		ARBString const& inName,
		ARBConfigLevelPtr* outLevel)
{
	if (outLevel)
		outLevel->reset();
	if (0 == inName.length())
		return false;
	if (FindSubLevel(inName))
		return false;
	ARBConfigLevelPtr pLevel(ARBConfigLevel::New());
	pLevel->SetName(inName);
	push_back(pLevel);
	if (outLevel)
		*outLevel = pLevel;
	return true;
}

bool ARBConfigLevelList::AddLevel(ARBConfigLevelPtr inLevel)
{
	if (!inLevel || 0 == inLevel->GetName().length())
		return false;
	if (FindSubLevel(inLevel->GetName()))
		return false;
	push_back(inLevel);
	return true;
}

bool ARBConfigLevelList::DeleteLevel(
		ARBString const& inName,
		ARBConfigEventList& ioEvents)
{
	ARBString name(inName);
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

bool ARBConfigLevelList::DeleteSubLevel(
		ARBString const& inName,
		bool& outLevelModified)
{
	ARBString name(inName);
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
					ARBString newName = (*iter)->GetName();
					while (FindSubLevel(newName))
					{
						outLevelModified = true;
						newName += _T("?");
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
