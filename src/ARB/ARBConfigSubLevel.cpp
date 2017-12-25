/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * 2013-09-03 Added short name.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBConfigSubLevel.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBConfigSubLevel_concrete : public ARBConfigSubLevel
	{
	public:
		ARBConfigSubLevel_concrete() {}
		ARBConfigSubLevel_concrete(ARBConfigSubLevel const& rhs)
			: ARBConfigSubLevel(rhs)
		{
		}
	};
};


ARBConfigSubLevelPtr ARBConfigSubLevel::New()
{
	return std::make_shared<ARBConfigSubLevel_concrete>();
}


ARBConfigSubLevel::ARBConfigSubLevel()
	: m_Name()
	, m_ShortName()
{
}


ARBConfigSubLevel::ARBConfigSubLevel(ARBConfigSubLevel const& rhs)
	: m_Name(rhs.m_Name)
	, m_ShortName(rhs.m_ShortName)
{
}


ARBConfigSubLevel::~ARBConfigSubLevel()
{
}


ARBConfigSubLevelPtr ARBConfigSubLevel::Clone() const
{
	return std::make_shared<ARBConfigSubLevel_concrete>(*this);
}


ARBConfigSubLevel& ARBConfigSubLevel::operator=(ARBConfigSubLevel const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_ShortName = rhs.m_ShortName;
	}
	return *this;
}


bool ARBConfigSubLevel::operator==(ARBConfigSubLevel const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_ShortName == rhs.m_ShortName;
}


bool ARBConfigSubLevel::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_SUBLEVEL)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_SUBLEVEL_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_SUBLEVEL, ATTRIB_SUBLEVEL_NAME));
		return false;
	}
	inTree->GetAttrib(ATTRIB_SUBLEVEL_SHORTNAME, m_ShortName);
	return true;
}


bool ARBConfigSubLevel::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr element = ioTree->AddElementNode(TREE_SUBLEVEL);
	element->AddAttrib(ATTRIB_SUBLEVEL_NAME, m_Name);
	if (!m_ShortName.empty())
		element->AddAttrib(ATTRIB_SUBLEVEL_SHORTNAME, m_ShortName);
	return true;
}


bool ARBConfigSubLevel::Update(
		int indent,
		ARBConfigSubLevelPtr inLevelNew,
		std::wstring& ioInfo)
{
	std::wstring info;
	if (GetName() != inLevelNew->GetName())
		return false;

	bool bChanges = false;

	if (GetShortName() != inLevelNew->GetShortName())
	{
		bChanges = true;
		SetShortName(inLevelNew->GetShortName());
	}

	return bChanges;
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


void ARBConfigSubLevelList::ReorderBy(ARBConfigSubLevelList const& inList)
{
	if (*this != inList)
	{
		ARBConfigSubLevelList tmp;
		tmp.reserve(size());
		for (ARBConfigSubLevelList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigSubLevelPtr level;
			if (FindSubLevel((*i)->GetName(), &level))
			{
				tmp.push_back(level);
				ARBConfigSubLevelList::iterator iFound = std::find(begin(), end(), level);
				assert(iFound != end());
				erase(iFound);
			}
		}
		if (0 < size())
			tmp.insert(tmp.end(), begin(), end());
		std::swap(tmp, *this);
	}
}


bool ARBConfigSubLevelList::FindSubLevel(
		std::wstring const& inName,
		ARBConfigSubLevelPtr* outLevel) const
{
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


bool ARBConfigSubLevelList::AddSubLevel(
		std::wstring const& inName,
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


bool ARBConfigSubLevelList::DeleteSubLevel(std::wstring const& inName)
{
	std::wstring name(inName);
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
