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
 * @li 2004-01-27 DRC Updating could cause some false-positive messages because
 *                    the ordering was different.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "stdafx.h"
#include "ARBConfigLevel.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"
#include <algorithm>

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
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
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_LEVEL)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_LEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_LEVEL, ATTRIB_LEVEL_NAME));
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
	assert(ioTree);
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
		wxString& ioInfo)
{
	wxString info;
	if (GetName() != inLevelNew->GetName())
		return false;

	wxString indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += wxT("   ");
	indentBuffer = indentName + wxT("   ");
	indentName += wxT("-");

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
		GetSubLevels().ReorderBy(inLevelNew->GetSubLevels());
		if (0 < nAdded)
		{
			info += indentBuffer;
			info += Localization()->UpdateSubLevels(nAdded);
			info += wxT("\n");
		}
		// Reordered
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateSubLevelsReordered();
			info += wxT("\n");
		}
	}
	bool bChanges = false;
	if (0 < info.length())
	{
		bChanges = true;
		ioInfo += indentName + GetName() + wxT("\n") + info;
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


void ARBConfigLevelList::ReorderBy(ARBConfigLevelList const& inList)
{
	if (*this != inList)
	{
		ARBConfigLevelList tmp;
		tmp.reserve(size());
		for (ARBConfigLevelList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigLevelPtr level;
			if (FindLevel((*i)->GetName(), &level))
			{
				tmp.push_back(level);
				ARBConfigLevelList::iterator iFound = std::find(begin(), end(), level);
				assert(iFound != end());
				erase(iFound);
			}
		}
		if (0 < size())
			tmp.insert(tmp.end(), begin(), end());
		std::swap(tmp, *this);
	}
}


bool ARBConfigLevelList::VerifyLevel(
		wxString const& inName,
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
		wxString const& inName,
		ARBConfigLevelPtr* outLevel) const
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
		wxString const& inName,
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
		wxString const& inName,
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
		wxString const& inDiv,
		wxString const& inName,
		ARBConfigEventList& ioEvents)
{
	wxString name(inName);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			// Remember, we don't need to worry about the sublevel names here.
			// Events only use level names.
			ioEvents.DeleteLevel(inDiv, name);
			erase(iter);
			return true;
		}
	}
	return false;
}


bool ARBConfigLevelList::DeleteSubLevel(
		wxString const& inName,
		bool& outLevelModified)
{
	wxString name(inName);
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
					wxString newName = (*iter)->GetName();
					while (FindSubLevel(newName))
					{
						outLevelModified = true;
						newName += wxT("?");
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
