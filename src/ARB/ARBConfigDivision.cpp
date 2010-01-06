/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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

#include "stdafx.h"
#include "ARBConfigDivision.h"

#include <algorithm>
#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
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
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_DIVISION)
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
	assert(ioTree);
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
		wxString& ioInfo)
{
	wxString info;

	wxString indentBuffer, indentName;
	for (int i = 0; i < indent-1; ++i)
		indentName += wxT("   ");
	indentBuffer = indentName + wxT("   ");
	indentName += wxT("-");

	// If the order is different, we will fall into this...
	if (GetLevels() != inDivNew->GetLevels())
	{
		wxString info2;
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
				info2 += wxT("+");
				info2 += (*iterLevel)->GetName();
				info2 += wxT("\n");
			}
		}
		GetLevels().ReorderBy(inDivNew->GetLevels());
		// ... so only generate a message if we added or changed.
		if (0 < nAdded || 0 < nChanged)
		{
			info += indentBuffer;
			info += Localization()->UpdateLevels(nAdded, nChanged, nSkipped);
			info += wxT("\n");
			info += info2;
		}
		// Reordered
		else
		{
			info += indentBuffer;
			info += Localization()->UpdateLevelsReordered();
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
		ARBConfigDivisionList tmp;
		tmp.reserve(size());
		for (ARBConfigDivisionList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
		{
			ARBConfigDivisionPtr div;
			if (FindDivision((*i)->GetName(), &div))
			{
				tmp.push_back(div);
				ARBConfigDivisionList::iterator iFound = std::find(begin(), end(), div);
				assert(iFound != end());
				erase(iFound);
			}
		}
		if (0 < size())
			tmp.insert(tmp.end(), begin(), end());
		std::swap(tmp, *this);
	}
}


bool ARBConfigDivisionList::VerifyLevel(
		wxString const& inDiv,
		wxString const& inLevel) const
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
		wxString const& inDiv,
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
		wxString const& inDiv,
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
		wxString const& inDiv,
		ARBConfigEventList& ioEvents)
{
	wxString div(inDiv);
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
