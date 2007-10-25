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
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigDivisionPtr ARBConfigDivision::New()
{
	return ARBConfigDivisionPtr(new ARBConfigDivision());
}


ARBConfigDivision::ARBConfigDivision()
	: m_Name()
	, m_Levels()
{
}


ARBConfigDivision::ARBConfigDivision(ARBConfigDivision const& rhs)
	: m_Name(rhs.m_Name)
	, m_Levels()
{
	rhs.m_Levels.Clone(m_Levels);
}


ARBConfigDivision::~ARBConfigDivision()
{
}


ARBConfigDivisionPtr ARBConfigDivision::Clone() const
{
	return ARBConfigDivisionPtr(new ARBConfigDivision(*this));
}


ARBConfigDivision& ARBConfigDivision::operator=(ARBConfigDivision const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		rhs.m_Levels.Clone(m_Levels);
	}
	return *this;
}


bool ARBConfigDivision::operator==(ARBConfigDivision const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Levels == rhs.m_Levels;
}


void ARBConfigDivision::clear()
{
	m_Name.erase();
	m_Levels.clear();
}


bool ARBConfigDivision::Load(
		ARBConfigVenue& ioVenue,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_DIVISION_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_DIVISION, ATTRIB_DIVISION_NAME));
		return false;
	}
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_LEVEL)
		{
			// Ignore any errors...
			m_Levels.Load(element, inVersion, ioCallback);
		}
		else if (element->GetName() == TREE_TITLES)
		{
			if (inVersion < ARBVersion(12,0))
			{
				ioVenue.GetTitles().Load(element, inVersion, ioCallback, true);
			}
		}
	}
	return true;
}


bool ARBConfigDivision::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr division = ioTree->AddElementNode(TREE_DIVISION);
	division->AddAttrib(ATTRIB_DIVISION_NAME, m_Name);
	if (!m_Levels.Save(division))
		return false;
	return true;
}


bool ARBConfigDivision::Update(
		int indent,
		ARBConfigDivisionPtr inDivNew,
		tstring& ioInfo)
{
	tstring info;

	tstring indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += _T("   ");
	indentBuffer = indentName + _T("   ");
	indentName += _T("-");

	// If the order is different, we will fall into this...
	if (GetLevels() != inDivNew->GetLevels())
	{
		tstring info2;
		int nChanged, nAdded, nSkipped;
		nChanged = nAdded = nSkipped = 0;
		for (ARBConfigLevelList::const_iterator iterLevel = inDivNew->GetLevels().begin();
			iterLevel != inDivNew->GetLevels().end();
			++iterLevel)
		{
			ARBConfigLevelPtr pLevel;
			if (GetLevels().FindLevel((*iterLevel)->GetName(), &pLevel))
			{
				if (*(*iterLevel) == *pLevel)
					++nSkipped;
				else
				{
					if (pLevel->Update(indent+1, *iterLevel, info2))
						++nChanged;
				}
			}
			else
			{
				++nAdded;
				GetLevels().AddLevel(*iterLevel);
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
			info += Localization()->UpdateLevels(nAdded, nChanged, nSkipped);
			info += _T("\n");
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigDivisionPtr thing(ARBConfigDivision::New());
	if (!thing->Load(ioVenue, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBConfigDivisionList::ReorderBy(ARBConfigDivisionList const& inList)
{
	if (*this != inList)
	{
		ASSERT(inList.size() == size());
		ARBConfigDivisionList tmp;
		tmp.reserve(size());
		for (ARBConfigDivisionList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigDivisionPtr div;
			FindDivision((*i)->GetName(), &div);
			tmp.push_back(div);
		}
		std::swap(tmp, *this);
	}
}


bool ARBConfigDivisionList::VerifyLevel(
		tstring const& inDiv,
		tstring const& inLevel) const
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
		tstring const& inDiv,
		ARBConfigDivisionPtr* outDiv) const
{
	if (outDiv)
		outDiv->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inDiv)
		{
			if (outDiv)
				*outDiv = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigDivisionList::AddDivision(
		tstring const& inDiv,
		ARBConfigDivisionPtr* outDiv)
{
	if (outDiv)
		outDiv->reset();
	if (0 == inDiv.length())
		return false;
	if (FindDivision(inDiv))
		return false;
	ARBConfigDivisionPtr pDiv(ARBConfigDivision::New());
	pDiv->SetName(inDiv);
	push_back(pDiv);
	if (outDiv)
		*outDiv = pDiv;
	return true;
}


bool ARBConfigDivisionList::AddDivision(ARBConfigDivisionPtr inDiv)
{
	if (!inDiv
	|| 0 == inDiv->GetName().length()
	|| FindDivision(inDiv->GetName()))
		return false;
	push_back(inDiv);
	return true;
}


int ARBConfigDivisionList::DeleteDivision(
		tstring const& inDiv,
		ARBConfigEventList& ioEvents)
{
	tstring div(inDiv);
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
