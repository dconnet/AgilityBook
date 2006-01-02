/*
 * Copyright � 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-10-14 DRC Added option to prefix a title.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-27 DRC Updating could cause some false-positive messages because
 *                    the ordering was different.
 * @li 2004-01-21 DRC Added DeleteTitle.
 * @li 2003-12-28 DRC Added GetSearchStrings.
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
{
}

ARBConfigDivision::ARBConfigDivision(ARBConfigDivision const& rhs)
	: m_Name(rhs.m_Name)
	, m_Levels(rhs.m_Levels)
{
}

ARBConfigDivision::~ARBConfigDivision()
{
}

ARBConfigDivision& ARBConfigDivision::operator=(ARBConfigDivision const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Levels = rhs.m_Levels;
	}
	return *this;
}

bool ARBConfigDivision::operator==(ARBConfigDivision const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Levels == rhs.m_Levels;
}

bool ARBConfigDivision::operator!=(ARBConfigDivision const& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigDivision::clear()
{
	m_Name.erase();
	m_Levels.clear();
}

size_t ARBConfigDivision::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigDivision::Load(
		ARBConfigVenue& ioVenue,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_DIVISION_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_DIVISION, ATTRIB_DIVISION_NAME));
		return false;
	}
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		if (element.GetName() == TREE_LEVEL)
		{
			// Ignore any errors...
			m_Levels.Load(element, inVersion, ioCallback);
		}
		else if (element.GetName() == TREE_TITLES)
		{
			if (inVersion < ARBVersion(12,0))
			{
				ioVenue.GetTitles().Load(element, inVersion, ioCallback, true);
			}
		}
	}
	return true;
}

bool ARBConfigDivision::Save(Element& ioTree) const
{
	Element& division = ioTree.AddElement(TREE_DIVISION);
	division.AddAttrib(ATTRIB_DIVISION_NAME, m_Name);
	if (!m_Levels.Save(division))
		return false;
	return true;
}

bool ARBConfigDivision::Update(
		int indent,
		ARBConfigDivision const* inDivNew,
		ARBString& ioInfo)
{
	ARBString info;

	ARBString indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += _T("   ");
	indentBuffer = indentName + _T("   ");
	indentName += _T("-");

	// If the order is different, we will fall into this...
	if (GetLevels() != inDivNew->GetLevels())
	{
		ARBString info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigLevelList::const_iterator iterLevel = inDivNew->GetLevels().begin();
			iterLevel != inDivNew->GetLevels().end();
			++iterLevel)
		{
			ARBConfigLevel* pLevel;
			if (GetLevels().FindLevel((*iterLevel)->GetName(), &pLevel))
			{
				if (*(*iterLevel) == *pLevel)
					++nSkipped;
				else
				{
					if (pLevel->Update(indent+1, (*iterLevel), info2))
						++nChanged;
				}
				pLevel->Release();
			}
			else
			{
				++nAdded;
				GetLevels().AddLevel((*iterLevel));
				info2 += indentBuffer;
				info2 += _T("+");
				info2 += (*iterLevel)->GetName();
				info2 += _T("\n");
			}
		}
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += UPDATE_FORMAT_LEVELS(nAdded, nChanged, nSkipped);
			info += info2;
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

bool ARBConfigDivisionList::Load(
		ARBConfigVenue& ioVenue,
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigDivision* thing = new ARBConfigDivision();
	if (!thing->Load(ioVenue, inTree, inVersion, ioCallback))
	{
		thing->Release();
		return false;
	}
	push_back(thing);
	return true;
}

bool ARBConfigDivisionList::VerifyLevel(
		ARBString const& inDiv,
		ARBString const& inLevel) const
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

bool ARBConfigDivisionList::FindDivision(
		ARBString const& inDiv,
		ARBConfigDivision** outDiv) const
{
	if (outDiv)
		*outDiv = NULL;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inDiv)
		{
			if (outDiv)
			{
				*outDiv = *iter;
				(*outDiv)->AddRef();
			}
			return true;
		}
	}
	return false;
}

bool ARBConfigDivisionList::AddDivision(
		ARBString const& inDiv,
		ARBConfigDivision** outDiv)
{
	if (outDiv)
		*outDiv = NULL;
	if (0 == inDiv.length())
		return false;
	if (FindDivision(inDiv))
		return false;
	ARBConfigDivision* pDiv = new ARBConfigDivision();
	pDiv->SetName(inDiv);
	push_back(pDiv);
	if (outDiv)
	{
		*outDiv = pDiv;
		(*outDiv)->AddRef();
	}
	return true;
}

bool ARBConfigDivisionList::AddDivision(ARBConfigDivision* inDiv)
{
	if (!inDiv
	|| 0 == inDiv->GetName().length()
	|| FindDivision(inDiv->GetName()))
		return false;
	inDiv->AddRef();
	push_back(inDiv);
	return true;
}

int ARBConfigDivisionList::DeleteDivision(
		ARBString const& inDiv,
		ARBConfigEventList& ioEvents)
{
	ARBString div(inDiv);
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
