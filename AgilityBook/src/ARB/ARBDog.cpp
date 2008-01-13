/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
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
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogPtr ARBDog::New()
{
	return ARBDogPtr(new ARBDog());
}


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
	, m_ExistingPoints()
	, m_RegNums()
	, m_Titles()
	, m_Trials()
{
	rhs.m_ExistingPoints.Clone(m_ExistingPoints);
	rhs.m_RegNums.Clone(m_RegNums);
	rhs.m_Titles.Clone(m_Titles);
	rhs.m_Trials.Clone(m_Trials);
}


ARBDog::~ARBDog()
{
}


ARBDogPtr ARBDog::Clone() const
{
	return ARBDogPtr(new ARBDog(*this));
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
		rhs.m_ExistingPoints.Clone(m_ExistingPoints);
		rhs.m_RegNums.Clone(m_RegNums);
		rhs.m_Titles.Clone(m_Titles);
		rhs.m_Trials.Clone(m_Trials);
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


size_t ARBDog::GetSearchStrings(std::set<tstring>& ioStrings) const
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
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree || inTree->GetName() != TREE_DOG)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_DOG_CALLNAME, m_CallName)
	|| 0 == m_CallName.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_DOG, ATTRIB_DOG_CALLNAME));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_DOG_DOB, m_DOB))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_DOG_DOB, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DOB, msg.c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_DOG_DECEASED, m_Deceased))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_DOG_DECEASED, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DECEASED, msg.c_str()));
		return false;
	}

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_REGNAME)
		{
			m_RegName = element->GetValue();
		}
		else if (element->GetName() == TREE_BREED)
		{
			m_Breed = element->GetValue();
		}
		else if (element->GetName() == TREE_NOTE)
		{
			m_Note = element->GetValue();
		}
		else if (element->GetName() == TREE_EXISTING_PTS)
		{
			// Ignore any errors...
			m_ExistingPoints.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element->GetName() == TREE_REG_NUM)
		{
			// Ignore any errors...
			m_RegNums.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element->GetName() == TREE_TITLE)
		{
			// Ignore any errors...
			m_Titles.Load(inConfig, element, inVersion, ioCallback);
		}
		else if (element->GetName() == TREE_TRIAL)
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


bool ARBDog::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr dog = ioTree->AddElementNode(TREE_DOG);
	dog->AddAttrib(ATTRIB_DOG_CALLNAME, m_CallName);
	dog->AddAttrib(ATTRIB_DOG_DOB, m_DOB);
	if (m_Deceased.IsValid())
		dog->AddAttrib(ATTRIB_DOG_DECEASED, m_Deceased);
	if (0 < m_RegName.length())
	{
		ElementNodePtr element = dog->AddElementNode(TREE_REGNAME);
		element->SetValue(m_RegName);
	}
	if (0 < m_Breed.length())
	{
		ElementNodePtr element = dog->AddElementNode(TREE_BREED);
		element->SetValue(m_Breed);
	}
	if (0 < m_Note.length())
	{
		ElementNodePtr element = dog->AddElementNode(TREE_NOTE);
		element->SetValue(m_Note);
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


int ARBDog::RenameVenue(
		tstring const& inOldVenue,
		tstring const& inNewVenue)
{
	int count = m_ExistingPoints.RenameVenue(inOldVenue, inNewVenue);
	count += m_RegNums.RenameVenue(inOldVenue, inNewVenue);
	count += m_Titles.RenameVenue(inOldVenue, inNewVenue);
	count += m_Trials.RenameVenue(inOldVenue, inNewVenue);
	return count;
}


int ARBDog::DeleteVenue(tstring const& inVenue)
{
	int count = m_ExistingPoints.DeleteVenue(inVenue);
	count += m_RegNums.DeleteVenue(inVenue);
	count += m_Titles.DeleteVenue(inVenue);
	count += m_Trials.DeleteVenue(inVenue);
	return count;
}


int ARBDog::RenameDivision(
		ARBConfigVenuePtr inVenue,
		tstring const& inOldDiv,
		tstring const& inNewDiv)
{
	int count = m_ExistingPoints.RenameDivision(inVenue->GetName(), inOldDiv, inNewDiv);
	count += m_Trials.RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}


int ARBDog::DeleteDivision(
		ARBConfig const& inConfig,
		tstring const& inVenue,
		tstring const& inDiv)
{
	int count = m_ExistingPoints.DeleteDivision(inVenue, inDiv);
	count += m_Trials.DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogPtr thing(ARBDog::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBDogList::SetMultiQs(ARBConfig const& inConfig)
{
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		for (ARBDogTrialList::iterator iterT = (*iter)->GetTrials().begin();
			iterT != (*iter)->GetTrials().end();
			++iterT)
		{
			(*iterT)->SetMultiQs(inConfig);
		}
	}
}


int ARBDogList::NumExistingPointsInVenue(tstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInVenue(inVenue);
	return count;
}


int ARBDogList::NumRegNumsInVenue(tstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetRegNums().NumRegNumsInVenue(inVenue);
	return count;
}


int ARBDogList::NumTitlesInVenue(tstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInVenue(inVenue);
	return count;
}


int ARBDogList::NumTrialsInVenue(tstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumTrialsInVenue(inVenue);
	return count;
}


int ARBDogList::RenameVenue(
		tstring const& inOldVenue,
		tstring const& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameVenue(inOldVenue, inNewVenue);
		count += (*iter)->RenameVenue(inOldVenue, inNewVenue);
	}
	return count;
}


int ARBDogList::DeleteVenue(tstring const& inVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteVenue(inVenue);
		count += (*iter)->DeleteVenue(inVenue);
	}
	return count;
}


int ARBDogList::NumOtherPointsInUse(tstring const& inOther) const
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
		tstring const& inOldOther,
		tstring const& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameOtherPoints(inOldOther, inNewOther);
		count += (*iter)->GetTrials().RenameOtherPoints(inOldOther, inNewOther);
	}
	return count;
}


int ARBDogList::DeleteOtherPoints(tstring const& inOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteOtherPoints(inOther);
		count += (*iter)->GetTrials().DeleteOtherPoints(inOther);
	}
	return count;
}


int ARBDogList::NumMultiQsInUse(
		tstring const& inVenue,
		tstring const& inMultiQ) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().NumMultiQsInUse(inVenue, inMultiQ);
	}
	return count;
}


int ARBDogList::RenameMultiQs(
		tstring const& inVenue,
		tstring const& inOldMultiQ,
		tstring const& inNewMultiQ)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameMultiQs(inVenue, inOldMultiQ, inNewMultiQ);
	}
	return count;
}


int ARBDogList::DeleteMultiQs(
		ARBConfig const& inConfig,
		tstring const& inVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteMultiQs(inConfig, inVenue);
	}
	return count;
}


int ARBDogList::NumMultiHostedTrialsInDivision(
		ARBConfig const& inConfig,
		tstring const& inVenue,
		tstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumMultiHostedTrialsInDivision(inConfig, inVenue, inDiv);
	return count;
}


int ARBDogList::NumExistingPointsInDivision(
		ARBConfigVenuePtr inVenue,
		tstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInDivision(inVenue, inDiv);
	return count;
}


int ARBDogList::NumRunsInDivision(
		ARBConfigVenuePtr inVenue,
		tstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumRunsInDivision(inVenue, inDiv);
	return count;
}


int ARBDogList::RenameDivision(
		ARBConfigVenuePtr inVenue,
		tstring const& inOldDiv,
		tstring const& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}


int ARBDogList::DeleteDivision(
		ARBConfig const& inConfig,
		tstring const& inVenue,
		tstring const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}


int ARBDogList::NumLevelsInUse(
		tstring const& inVenue,
		tstring const& inDiv,
		tstring const& inLevel) const
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
		tstring const& inVenue,
		tstring const& inDiv,
		tstring const& inOldLevel,
		tstring const& inNewLevel)
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
		tstring const& inVenue,
		tstring const& inDiv,
		tstring const& inLevel)
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
		tstring const& inVenue,
		tstring const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInUse(inVenue, inTitle);
	return count;
}


int ARBDogList::RenameTitle(
		tstring const& inVenue,
		tstring const& inOldTitle,
		tstring const& inNewTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().RenameTitle(inVenue, inOldTitle, inNewTitle);
	return count;
}


int ARBDogList::DeleteTitle(
		tstring const& inVenue,
		tstring const& inTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBDogTitlePtr pTitle;
		(*iter)->GetTitles().FindTitle(inVenue, inTitle, &pTitle);
		while (pTitle)
		{
			++count;
			(*iter)->GetTitles().DeleteTitle(pTitle);
			(*iter)->GetTitles().FindTitle(inVenue, inTitle, &pTitle);
		}
	}
	return count;
}


int ARBDogList::NumEventsInUse(
		tstring const& inVenue,
		tstring const& inEvent) const
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
		tstring const& inVenue,
		tstring const& inOldEvent,
		tstring const& inNewEvent)
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
		tstring const& inVenue,
		tstring const& inEvent)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteEvent(inVenue, inEvent);
		count += (*iter)->GetTrials().DeleteEvent(inVenue, inEvent);
	}
	return count;
}


bool ARBDogList::AddDog(ARBDogPtr inDog)
{
	bool bAdded = false;
	if (inDog)
	{
		bAdded = true;
		push_back(inDog);
	}
	return bAdded;
}


bool ARBDogList::DeleteDog(ARBDogPtr inDog)
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
