/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-02-02 DRC Added ExistingPoints.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
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
	, m_ExistingPoints()
	, m_RegNums()
	, m_Titles()
	, m_Trials()
{
}

ARBDog::ARBDog(ARBDog const& rhs)
	: m_CallName(rhs.m_CallName)
	, m_DOB(rhs.m_DOB)
	, m_Deceased(rhs.m_Deceased)
	, m_RegName(rhs.m_RegName)
	, m_Breed(rhs.m_Breed)
	, m_Note(rhs.m_Note)
	, m_ExistingPoints(rhs.m_ExistingPoints)
	, m_RegNums(rhs.m_RegNums)
	, m_Titles(rhs.m_Titles)
	, m_Trials(rhs.m_Trials)
{
}

ARBDog::~ARBDog()
{
}

ARBDog& ARBDog::operator=(ARBDog const& rhs)
{
	if (this != &rhs)
	{
		m_CallName = rhs.m_CallName;
		m_DOB = rhs.m_DOB;
		m_Deceased = rhs.m_Deceased;
		m_RegName = rhs.m_RegName;
		m_Breed = rhs.m_Breed;
		m_Note = rhs.m_Note;
		m_ExistingPoints = rhs.m_ExistingPoints;
		m_RegNums = rhs.m_RegNums;
		m_Titles = rhs.m_Titles;
		m_Trials = rhs.m_Trials;
	}
	return *this;
}

bool ARBDog::operator==(ARBDog const& rhs) const
{
	return m_CallName == rhs.m_CallName
		&& m_DOB == rhs.m_DOB
		&& m_Deceased == rhs.m_Deceased
		&& m_RegName == rhs.m_RegName
		&& m_Breed == rhs.m_Breed
		&& m_Note == rhs.m_Note
		&& m_ExistingPoints == rhs.m_ExistingPoints
		&& m_RegNums == rhs.m_RegNums
		&& m_Titles == rhs.m_Titles
		&& m_Trials == rhs.m_Trials;
}

bool ARBDog::operator!=(ARBDog const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDog::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_CallName);
	++nItems;

	if (m_DOB.IsValid())
	{
		ioStrings.insert(m_DOB.GetString(ARBDate::eSlashMDY));
		++nItems;
	}
	
	if (m_Deceased.IsValid())
	{
		ioStrings.insert(m_Deceased.GetString(ARBDate::eSlashMDY));
		++nItems;
	}

	if (0 < m_RegName.length())
	{
		ioStrings.insert(m_RegName);
		++nItems;
	}

	if (0 < m_Breed.length())
	{
		ioStrings.insert(m_Breed);
		++nItems;
	}

	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}

	nItems += m_ExistingPoints.GetSearchStrings(ioStrings);

	nItems += m_RegNums.GetSearchStrings(ioStrings);

	nItems += m_Titles.GetSearchStrings(ioStrings);

	return nItems;
}

bool ARBDog::Load(
	ARBConfig const& inConfig,
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_DOG_CALLNAME, m_CallName)
	|| 0 == m_CallName.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_DOG, ATTRIB_DOG_CALLNAME));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_DOG_DOB, m_DOB))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_DOG_DOB, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DOB, msg.c_str()));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_DOG_DECEASED, m_Deceased))
	{
		std::string attrib;
		inTree.GetAttrib(ATTRIB_DOG_DECEASED, attrib);
		std::string msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DECEASED, msg.c_str()));
		return false;
	}

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
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
		else if (element.GetName() == TREE_EXISTING_PTS)
		{
			// Ignore any errors...
			m_ExistingPoints.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element.GetName() == TREE_REG_NUM)
		{
			// Ignore any errors...
			m_RegNums.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element.GetName() == TREE_TITLE)
		{
			// Ignore any errors...
			m_Titles.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element.GetName() == TREE_TRIAL)
		{
			// Ignore any errors...
			m_Trials.Load(inConfig, element, inVersion, ioCallback);
		}
	}
	m_ExistingPoints.sort();
	m_RegNums.sort();
	m_Titles.sort();
	m_Trials.sort(true);
	return true;
}

bool ARBDog::Save(Element& ioTree) const
{
	Element& dog = ioTree.AddElement(TREE_DOG);
	dog.AddAttrib(ATTRIB_DOG_CALLNAME, m_CallName);
	dog.AddAttrib(ATTRIB_DOG_DOB, m_DOB);
	if (m_Deceased.IsValid())
		dog.AddAttrib(ATTRIB_DOG_DECEASED, m_Deceased);
	if (0 < m_RegName.length())
	{
		Element& element = dog.AddElement(TREE_REGNAME);
		element.SetValue(m_RegName);
	}
	if (0 < m_Breed.length())
	{
		Element& element = dog.AddElement(TREE_BREED);
		element.SetValue(m_Breed);
	}
	if (0 < m_Note.length())
	{
		Element& element = dog.AddElement(TREE_NOTE);
		element.SetValue(m_Note);
	}
	if (!m_ExistingPoints.Save(dog))
		return false;
	if (!m_RegNums.Save(dog))
		return false;
	if (!m_Titles.Save(dog))
		return false;
	if (!m_Trials.Save(dog))
		return false;
	return true;
}

int ARBDog::RenameVenue(std::string const& inOldVenue, std::string const& inNewVenue)
{
	int count = m_ExistingPoints.RenameVenue(inOldVenue, inNewVenue);
	count = m_RegNums.RenameVenue(inOldVenue, inNewVenue);
	count += m_Titles.RenameVenue(inOldVenue, inNewVenue);
	count += m_Trials.RenameVenue(inOldVenue, inNewVenue);
	return count;
}

int ARBDog::DeleteVenue(std::string const& inVenue)
{
	int count = m_ExistingPoints.DeleteVenue(inVenue);
	count = m_RegNums.DeleteVenue(inVenue);
	count += m_Titles.DeleteVenue(inVenue);
	count += m_Trials.DeleteVenue(inVenue);
	return count;
}

int ARBDog::RenameDivision(ARBConfigVenue const* inVenue, std::string const& inOldDiv, std::string const& inNewDiv)
{
	int count = m_ExistingPoints.RenameDivision(inVenue->GetName(), inOldDiv, inNewDiv);
	count = m_Titles.RenameDivision(inVenue, inOldDiv, inNewDiv);
	count += m_Trials.RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}

int ARBDog::DeleteDivision(ARBConfig const& inConfig, std::string const& inVenue, std::string const& inDiv)
{
	int count = m_ExistingPoints.DeleteDivision(inVenue, inDiv);
	count = m_Titles.DeleteDivision(inConfig, inVenue, inDiv);
	count += m_Trials.DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}

/////////////////////////////////////////////////////////////////////////////

int ARBDogList::NumExistingPointsInVenue(std::string const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInVenue(inVenue);
	return count;
}

int ARBDogList::NumRegNumsInVenue(std::string const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetRegNums().NumRegNumsInVenue(inVenue);
	return count;
}

int ARBDogList::NumTitlesInVenue(std::string const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInVenue(inVenue);
	return count;
}

int ARBDogList::NumTrialsInVenue(std::string const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumTrialsInVenue(inVenue);
	return count;
}

int ARBDogList::RenameVenue(
	std::string const& inOldVenue,
	std::string const& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameVenue(inOldVenue, inNewVenue);
		count += (*iter)->RenameVenue(inOldVenue, inNewVenue);
	}
	return count;
}

int ARBDogList::DeleteVenue(std::string const& inVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteVenue(inVenue);
		count += (*iter)->DeleteVenue(inVenue);
	}
	return count;
}

int ARBDogList::NumOtherPointsInUse(std::string const& inOther) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().NumOtherPointsInUse(inOther);
		count += (*iter)->GetTrials().NumOtherPointsInUse(inOther);
	}
	return count;
}

int ARBDogList::RenameOtherPoints(
	std::string const& inOldOther,
	std::string const& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameOtherPoints(inOldOther, inNewOther);
		count += (*iter)->GetTrials().RenameOtherPoints(inOldOther, inNewOther);
	}
	return count;
}

int ARBDogList::DeleteOtherPoints(std::string const& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteOtherPoints(inOther);
		count += (*iter)->GetTrials().DeleteOtherPoints(inOther);
	}
	return count;
}

int ARBDogList::NumMultiHostedTrialsInDivision(
	ARBConfig const& inConfig,
	std::string const& inVenue,
	std::string const& inDiv)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumMultiHostedTrialsInDivision(inConfig, inVenue, inDiv);
	return count;
}

int ARBDogList::NumExistingPointsInDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInDivision(inVenue, inDiv);
	return count;
}

int ARBDogList::NumTitlesInDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInDivision(inVenue, inDiv);
	return count;
}

int ARBDogList::NumRunsInDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumRunsInDivision(inVenue, inDiv);
	return count;
}

int ARBDogList::RenameDivision(
	ARBConfigVenue const* inVenue,
	std::string const& inOldDiv,
	std::string const& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}

int ARBDogList::DeleteDivision(
	ARBConfig const& inConfig,
	std::string const& inVenue,
	std::string const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}

int ARBDogList::NumLevelsInUse(
	std::string const& inVenue,
	std::string const& inDiv,
	std::string const& inLevel) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().NumLevelsInUse(inVenue, inDiv, inLevel);
		count += (*iter)->GetTrials().NumLevelsInUse(inVenue, inDiv, inLevel);
	}
	return count;
}

int ARBDogList::RenameLevel(
	std::string const& inVenue,
	std::string const& inDiv,
	std::string const& inOldLevel,
	std::string const& inNewLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameLevel(inVenue, inDiv, inOldLevel, inNewLevel);
		count += (*iter)->GetTrials().RenameLevel(inVenue, inDiv, inOldLevel, inNewLevel);
	}
	return count;
}

int ARBDogList::DeleteLevel(
	std::string const& inVenue,
	std::string const& inDiv,
	std::string const& inLevel)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteLevel(inVenue, inDiv, inLevel);
		count += (*iter)->GetTrials().DeleteLevel(inVenue, inDiv, inLevel);
	}
	return count;
}

int ARBDogList::NumTitlesInUse(
	std::string const& inVenue,
	std::string const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInUse(inVenue, inTitle);
	return count;
}

int ARBDogList::RenameTitle(
	std::string const& inVenue,
	std::string const& inOldTitle,
	std::string const& inNewTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().RenameTitle(inVenue, inOldTitle, inNewTitle);
	return count;
}

int ARBDogList::DeleteTitle(
	std::string const& inVenue,
	std::string const& inTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().DeleteTitle(inVenue, inTitle);
	return count;
}

int ARBDogList::NumEventsInUse(
	std::string const& inVenue,
	std::string const& inEvent) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().NumEventsInUse(inVenue, inEvent);
		count += (*iter)->GetTrials().NumEventsInUse(inVenue, inEvent);
	}
	return count;
}

int ARBDogList::RenameEvent(
	std::string const& inVenue,
	std::string const& inOldEvent,
	std::string const& inNewEvent)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameEvent(inVenue, inOldEvent, inNewEvent);
		count += (*iter)->GetTrials().RenameEvent(inVenue, inOldEvent, inNewEvent);
	}
	return count;
}

int ARBDogList::DeleteEvent(
	std::string const& inVenue,
	std::string const& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteEvent(inVenue, inEvent);
		count += (*iter)->GetTrials().DeleteEvent(inVenue, inEvent);
	}
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

bool ARBDogList::DeleteDog(ARBDog const* inDog)
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
