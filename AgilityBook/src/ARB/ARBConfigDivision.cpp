/*
 * Copyright � 2002-2003 David Connet. All Rights Reserved.
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
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#include "ARBConfigDivision.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigDivision::ARBConfigDivision()
	: m_Name()
	, m_Levels()
	, m_Titles()
{
}

ARBConfigDivision::ARBConfigDivision(const ARBConfigDivision& rhs)
	: m_Name(rhs.m_Name)
	, m_Levels(rhs.m_Levels)
	, m_Titles(rhs.m_Titles)
{
}

ARBConfigDivision::~ARBConfigDivision()
{
}

ARBConfigDivision& ARBConfigDivision::operator=(const ARBConfigDivision& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Levels = rhs.m_Levels;
		m_Titles = rhs.m_Titles;
	}
	return *this;
}

bool ARBConfigDivision::operator==(const ARBConfigDivision& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Levels == rhs.m_Levels
		&& m_Titles == rhs.m_Titles;
}

bool ARBConfigDivision::operator!=(const ARBConfigDivision& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigDivision::clear()
{
	m_Name.erase();
	m_Levels.clear();
	m_Titles.clear();
}

bool ARBConfigDivision::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_DIVISION_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ErrorMissingAttribute(TREE_DIVISION, ATTRIB_DIVISION_NAME);
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		if (element.GetName() == TREE_LEVEL)
		{
			// Ignore any errors...
			m_Levels.Load(element, inVersion);
		}
		else if (element.GetName() == TREE_TITLES)
		{
			// Ignore any errors...
			m_Titles.Load(element, inVersion);
		}
	}
	return true;
}

bool ARBConfigDivision::Save(CElement& ioTree) const
{
	CElement& division = ioTree.AddElement(TREE_DIVISION);
	division.AddAttrib(ATTRIB_DIVISION_NAME, m_Name);
	if (!m_Levels.Save(division))
		return false;
	if (!m_Titles.Save(division))
		return false;
	return true;
}

std::string ARBConfigDivision::Update(int indent, const ARBConfigDivision* inDivNew)
{
	std::string info;

	std::string indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += "   ";
	indentBuffer = indentName + "   ";
	indentName += "-";

	if (GetLevels() != inDivNew->GetLevels())
	{
		std::string info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigLevelList::const_iterator iterLevel = inDivNew->GetLevels().begin();
			iterLevel != inDivNew->GetLevels().end();
			++iterLevel)
		{
			ARBConfigLevel* pLevel = GetLevels().FindLevel((*iterLevel)->GetName());
			if (pLevel)
			{
				if (*(*iterLevel) == *pLevel)
					++nSkipped;
				else
				{
					++nChanged;
					info2 += pLevel->Update(indent+1, (*iterLevel));
				}
			}
			else
			{
				++nAdded;
				GetLevels().AddLevel((*iterLevel));
				info2 += indentBuffer;
				info2 += "+";
				info2 += (*iterLevel)->GetName();
				info2 += "\n";
			}
		}
		if (0 < nAdded || 0 < nChanged || 0 < nSkipped)
		{
			char buffer[1000];
			sprintf(buffer, UPDATE_FORMAT_LEVELS, nAdded, nChanged, nSkipped);
			info += indentBuffer;
			info += buffer;
			info += info2;
		}
	}
	if (GetTitles() != inDivNew->GetTitles())
	{
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigTitleList::const_iterator iterTitle = inDivNew->GetTitles().begin();
			iterTitle != inDivNew->GetTitles().end();
			++iterTitle)
		{
			ARBConfigTitle* pTitle = GetTitles().FindTitle((*iterTitle)->GetName());
			if (pTitle)
			{
				if (*(*iterTitle) == *pTitle)
					++nSkipped;
				else
				{
					++nChanged;
					pTitle->SetDescription((*iterTitle)->GetDescription());
				}
			}
			else
			{
				++nAdded;
				GetTitles().AddTitle((*iterTitle));
			}
		}
		if (0 < nAdded || 0 < nChanged || 0 < nSkipped)
		{
			char buffer[1000];
			sprintf(buffer, UPDATE_FORMAT_TITLES, nAdded, nChanged, nSkipped);
			info += indentBuffer;
			info += buffer;
		}
	}
	if (0 < info.length())
		info = indentName + GetName() + "\n" + info;
	return info;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigDivisionList::Load(
	const CElement& inTree,
	const ARBVersion& inVersion)
{
	ARBConfigDivision* thing = new ARBConfigDivision;
	if (!thing->Load(inTree, inVersion))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

bool ARBConfigDivisionList::VerifyLevel(const std::string& inDiv, const std::string& inLevel) const
{
	// Wildcards are only used in the ARBConfigScoring object.
	bool bWildCard = (inDiv == WILDCARD_DIVISION);
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if (bWildCard || (*iter)->GetName() == inDiv)
		{
			bool bVerified = (*iter)->GetLevels().VerifyLevel(inLevel);
			// For wildcard, all we need is at least one level match.
			if (!bWildCard || bVerified)
				return bVerified;
		}
	}
	return false;
}

bool ARBConfigDivisionList::VerifyTitle(const std::string& inTitle) const
{
	if (FindTitle(inTitle))
		return true;
	else
		return false;
}

const ARBConfigDivision* ARBConfigDivisionList::FindDivision(const std::string& inDiv) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inDiv)
			return (*iter);
	}
	return NULL;
}

ARBConfigDivision* ARBConfigDivisionList::FindDivision(const std::string& inDiv)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inDiv)
			return (*iter);
	}
	return NULL;
}

ARBConfigDivision* ARBConfigDivisionList::AddDivision(const std::string& inDiv)
{
	if (0 == inDiv.length())
		return NULL;
	if (NULL != FindDivision(inDiv))
		return NULL;
	ARBConfigDivision* pDiv = new ARBConfigDivision;
	pDiv->SetName(inDiv);
	push_back(pDiv);
	return pDiv;
}

ARBConfigDivision* ARBConfigDivisionList::AddDivision(ARBConfigDivision* inDiv)
{
	if (!inDiv
	|| 0 == inDiv->GetName().length()
	|| FindDivision(inDiv->GetName()))
		return NULL;
	inDiv->AddRef();
	push_back(inDiv);
	return inDiv;
}

int ARBConfigDivisionList::DeleteDivision(const std::string& inDiv, ARBConfigEventList& ioEvents)
{
	std::string div(inDiv);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == div)
		{
			ioEvents.DeleteDivision(div);
			erase(iter);
			return 1; // Names are unique, no need to continue.
		}
	}
	return 0;
}

const ARBConfigTitle* ARBConfigDivisionList::FindTitle(const std::string& inTitle) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		const ARBConfigTitle* pTitle = (*iter)->GetTitles().FindTitle(inTitle);
		if (pTitle)
			return pTitle;
	}
	return NULL;
}
