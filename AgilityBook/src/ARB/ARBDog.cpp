/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-08-18 DRC Added a deceased date. While this does change the format
 *                of the file, it's backwards compatible, so it doesn't warrant
 *                a file version change.
 * @li 2003-07-24 DRC Removed built-in sort on dogs. Dogs are user-sorted now.
 * @li 2003-07-16 DRC Allow the code to keep processing after errors are found.
 */

#include "StdAfx.h"
#include "ARBDog.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDog::ARBDog()
	: m_CallName()
	, m_DOB()
	, m_Deceased()
	, m_RegName()
	, m_Breed()
	, m_Note()
	, m_RegNums()
	, m_Titles()
	, m_Trials()
{
}

ARBDog::ARBDog(const ARBDog& rhs)
	: m_CallName(rhs.m_CallName)
	, m_DOB(rhs.m_DOB)
	, m_Deceased(rhs.m_Deceased)
	, m_RegName(rhs.m_RegName)
	, m_Breed(rhs.m_Breed)
	, m_Note(rhs.m_Note)
	, m_RegNums(rhs.m_RegNums)
	, m_Titles(rhs.m_Titles)
	, m_Trials(rhs.m_Trials)
{
}

ARBDog::~ARBDog()
{
}

ARBDog& ARBDog::operator=(const ARBDog& rhs)
{
	if (this != &rhs)
	{
		m_CallName = rhs.m_CallName;
		m_DOB = rhs.m_DOB;
		m_Deceased = rhs.m_Deceased;
		m_RegName = rhs.m_RegName;
		m_Breed = rhs.m_Breed;
		m_Note = rhs.m_Note;
		m_RegNums = rhs.m_RegNums;
		m_Titles = rhs.m_Titles;
		m_Trials = rhs.m_Trials;
	}
	return *this;
}

bool ARBDog::operator==(const ARBDog& rhs) const
{
	return m_CallName == rhs.m_CallName
		&& m_DOB == rhs.m_DOB
		&& m_Deceased == rhs.m_Deceased
		&& m_RegName == rhs.m_RegName
		&& m_Breed == rhs.m_Breed
		&& m_Note == rhs.m_Note
		&& m_RegNums == rhs.m_RegNums
		&& m_Titles == rhs.m_Titles
		&& m_Trials == rhs.m_Trials;
}

bool ARBDog::operator!=(const ARBDog& rhs) const
{
	return !operator==(rhs);
}

bool ARBDog::Load(
	const ARBConfig& inConfig,
	const CElement& inTree,
	int inVersion)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_DOG_CALLNAME, m_CallName)
	|| 0 == m_CallName.length())
	{
		ErrorMissingAttribute(TREE_DOG, ATTRIB_DOG_CALLNAME);
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_DOG_DOB, m_DOB))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_DOG_DOB, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DOB, msg.c_str());
		return false;
	}

	if (CElement::eInvalidValue == inTree.GetAttrib(ATTRIB_DOG_DECEASED, m_Deceased))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_DOG_DECEASED, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DECEASED, msg.c_str());
		return false;
	}

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		const CElement& element = inTree.GetElement(i);
		if (element.GetName() == TREE_REGNAME)
		{
			m_RegName = element.GetValue();
		}
		else if (element.GetName() == TREE_BREED)
		{
			m_Breed = element.GetValue();
		}
		else if (element.GetName() == TREE_NOTE)
		{
			m_Note = element.GetValue();
		}
		else if (element.GetName() == TREE_REG_NUM)
		{
			// Ignore any errors...
			m_RegNums.Load(inConfig, element, inVersion);
		}
		else if (element.GetName() == TREE_TITLE)
		{
			// Ignore any errors...
			m_Titles.Load(inConfig, element, inVersion);
		}
		else if (element.GetName() == TREE_TRIAL)
		{
			// Ignore any errors...
			m_Trials.Load(inConfig, element, inVersion);
		}
	}
	m_RegNums.sort();
	m_Titles.sort();
	m_Trials.sort(true);
	return true;
}

bool ARBDog::Save(CElement& ioTree) const
{
	CElement& dog = ioTree.AddElement(TREE_DOG);
	dog.AddAttrib(ATTRIB_DOG_CALLNAME, m_CallName);
	dog.AddAttrib(ATTRIB_DOG_DOB, m_DOB);
	if (m_Deceased.IsValid())
		dog.AddAttrib(ATTRIB_DOG_DECEASED, m_Deceased);
	if (0 < m_RegName.length())
	{
		CElement& element = dog.AddElement(TREE_REGNAME);
		element.SetValue(m_RegName);
	}
	if (0 < m_Breed.length())
	{
		CElement& element = dog.AddElement(TREE_BREED);
		element.SetValue(m_Breed);
	}
	if (0 < m_Note.length())
	{
		CElement& element = dog.AddElement(TREE_NOTE);
		element.SetValue(m_Note);
	}
	if (!m_RegNums.Save(dog))
		return false;
	if (!m_Titles.Save(dog))
		return false;
	if (!m_Trials.Save(dog))
		return false;
	return true;
}

int ARBDog::RenameVenue(const std::string& inOldVenue, const std::string& inNewVenue)
{
	int count = m_RegNums.RenameVenue(inOldVenue, inNewVenue);
	count += m_Titles.RenameVenue(inOldVenue, inNewVenue);
	count += m_Trials.RenameVenue(inOldVenue, inNewVenue);
	return count;
}

int ARBDog::DeleteVenue(const std::string& inVenue)
{
	int count = m_RegNums.DeleteVenue(inVenue);
	count += m_Titles.DeleteVenue(inVenue);
	count += m_Trials.DeleteVenue(inVenue);
	return count;
}

int ARBDog::RenameDivision(const ARBConfigVenue* inVenue, const std::string& inOldDiv, const std::string& inNewDiv)
{
	int count = m_Titles.RenameDivision(inVenue, inOldDiv, inNewDiv);
	count += m_Trials.RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}

int ARBDog::DeleteDivision(const ARBConfig& inConfig, const std::string& inVenue, const std::string& inDiv)
{
	int count = m_Titles.DeleteDivision(inConfig, inVenue, inDiv);
	count += m_Trials.DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}

/////////////////////////////////////////////////////////////////////////////

int ARBDogList::NumRegNumsInVenue(const std::string& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetRegNums().NumRegNumsInVenue(inVenue);
	return count;
}

int ARBDogList::NumTitlesInVenue(const std::string& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInVenue(inVenue);
	return count;
}

int ARBDogList::NumTrialsInVenue(const std::string& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumTrialsInVenue(inVenue);
	return count;
}

int ARBDogList::RenameVenue(
	const std::string& inOldVenue,
	const std::string& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->RenameVenue(inOldVenue, inNewVenue);
	return count;
}

int ARBDogList::DeleteVenue(const std::string& inVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->DeleteVenue(inVenue);
	return count;
}

int ARBDogList::NumOtherPointsInUse(const std::string& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumOtherPointsInUse(inOther);
	return count;
}

int ARBDogList::RenameOtherPoints(
	const std::string& inOldOther,
	const std::string& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().RenameOtherPoints(inOldOther, inNewOther);
	return count;
}

int ARBDogList::DeleteOtherPoints(const std::string& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().DeleteOtherPoints(inOther);
	return count;
}

int ARBDogList::NumMultiHostedTrialsInDivision(
	const ARBConfig& inConfig,
	const std::string& inVenue,
	const std::string& inDiv)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumMultiHostedTrialsInDivision(inConfig, inVenue, inDiv);
	return count;
}

int ARBDogList::NumTitlesInDivision(
	const ARBConfigVenue* inVenue,
	const std::string& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInDivision(inVenue, inDiv);
	return count;
}

int ARBDogList::NumRunsInDivision(
	const ARBConfigVenue* inVenue,
	const std::string& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumRunsInDivision(inVenue, inDiv);
	return count;
}

int ARBDogList::RenameDivision(
	const ARBConfigVenue* inVenue,
	const std::string& inOldDiv,
	const std::string& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}

int ARBDogList::DeleteDivision(
	const ARBConfig& inConfig,
	const std::string& inVenue,
	const std::string& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}

int ARBDogList::NumLevelsInUse(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumLevelsInUse(inVenue, inDiv, inLevel);
	return count;
}

int ARBDogList::RenameLevel(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inOldLevel,
	const std::string& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().RenameLevel(inVenue, inDiv, inOldLevel, inNewLevel);
	return count;
}

int ARBDogList::DeleteLevel(
	const std::string& inVenue,
	const std::string& inDiv,
	const std::string& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().DeleteLevel(inVenue, inDiv, inLevel);
	return count;
}

int ARBDogList::NumTitlesInUse(
	const std::string& inVenue,
	const std::string& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInUse(inVenue, inTitle);
	return count;
}

int ARBDogList::RenameTitle(
	const std::string& inVenue,
	const std::string& inOldTitle,
	const std::string& inNewTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().RenameTitle(inVenue, inOldTitle, inNewTitle);
	return count;
}

int ARBDogList::DeleteTitle(
	const std::string& inVenue,
	const std::string& inTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().DeleteTitle(inVenue, inTitle);
	return count;
}

int ARBDogList::NumEventsInUse(
	const std::string& inVenue,
	const std::string& inEvent) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumEventsInUse(inVenue, inEvent);
	return count;
}

int ARBDogList::RenameEvent(
	const std::string& inVenue,
	const std::string& inOldEvent,
	const std::string& inNewEvent)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().RenameEvent(inVenue, inOldEvent, inNewEvent);
	return count;
}

int ARBDogList::DeleteEvent(
	const std::string& inVenue,
	const std::string& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().DeleteEvent(inVenue, inEvent);
	return count;
}

ARBDog* ARBDogList::AddDog(ARBDog* inDog)
{
	if (inDog)
	{
		inDog->AddRef();
		push_back(inDog);
	}
	return inDog;
}

bool ARBDogList::DeleteDog(const ARBDog* inDog)
{
	if (inDog)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inDog)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
