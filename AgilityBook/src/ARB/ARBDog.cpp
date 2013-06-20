/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-09 DRC Added 'titlePts' to 'Placement'.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
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

#include "stdafx.h"
#include "ARB/ARBDog.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDog_concrete : public ARBDog
	{
	public:
		ARBDog_concrete() {}
		ARBDog_concrete(ARBDog const& rhs)
			: ARBDog(rhs)
		{
		}
	};
};


ARBDogPtr ARBDog::New()
{
	return std::make_shared<ARBDog_concrete>();
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
	return std::make_shared<ARBDog_concrete>(*this);
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


size_t ARBDog::GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
	assert(inTree);
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
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_DOG_DOB, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_DOG, ATTRIB_DOG_DOB, msg.c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_DOG_DECEASED, m_Deceased))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_DOG_DECEASED, attrib);
		std::wstring msg(Localization()->InvalidDate());
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


bool ARBDog::Save(
		ElementNodePtr ioTree,
		ARBConfig const& inConfig) const
{
	assert(ioTree);
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
	if (!m_Trials.Save(dog, inConfig))
		return false;
	return true;
}


int ARBDog::RenameVenue(
		std::wstring const& inOldVenue,
		std::wstring const& inNewVenue)
{
	int count = m_ExistingPoints.RenameVenue(inOldVenue, inNewVenue);
	count += m_RegNums.RenameVenue(inOldVenue, inNewVenue);
	count += m_Titles.RenameVenue(inOldVenue, inNewVenue);
	count += m_Trials.RenameVenue(inOldVenue, inNewVenue);
	return count;
}


int ARBDog::DeleteVenue(std::wstring const& inVenue)
{
	int count = m_ExistingPoints.DeleteVenue(inVenue);
	count += m_RegNums.DeleteVenue(inVenue);
	count += m_Titles.DeleteVenue(inVenue);
	count += m_Trials.DeleteVenue(inVenue);
	return count;
}


int ARBDog::RenameDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
{
	int count = m_ExistingPoints.RenameDivision(inVenue->GetName(), inOldDiv, inNewDiv);
	count += m_Trials.RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}


int ARBDog::DeleteDivision(
		ARBConfig const& inConfig,
		std::wstring const& inVenue,
		std::wstring const& inDiv)
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


int ARBDogList::NumExistingPointsInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInVenue(inVenue);
	return count;
}


int ARBDogList::NumRegNumsInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetRegNums().NumRegNumsInVenue(inVenue);
	return count;
}


int ARBDogList::NumTitlesInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInVenue(inVenue);
	return count;
}


int ARBDogList::NumTrialsInVenue(std::wstring const& inVenue) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumTrialsInVenue(inVenue);
	return count;
}


int ARBDogList::RenameVenue(
		std::wstring const& inOldVenue,
		std::wstring const& inNewVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameVenue(inOldVenue, inNewVenue);
		count += (*iter)->RenameVenue(inOldVenue, inNewVenue);
	}
	return count;
}


int ARBDogList::DeleteVenue(std::wstring const& inVenue)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().DeleteVenue(inVenue);
		count += (*iter)->DeleteVenue(inVenue);
	}
	return count;
}


int ARBDogList::NumOtherPointsInUse(std::wstring const& inOther) const
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
		std::wstring const& inOldOther,
		std::wstring const& inNewOther)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().RenameOtherPoints(inOldOther, inNewOther);
		count += (*iter)->GetTrials().RenameOtherPoints(inOldOther, inNewOther);
	}
	return count;
}


int ARBDogList::DeleteOtherPoints(std::wstring const& inOther)
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
		std::wstring const& inVenue,
		std::wstring const& inMultiQ) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		count += (*iter)->GetExistingPoints().NumMultiQsInUse(inVenue, inMultiQ);
	}
	return count;
}


int ARBDogList::RenameMultiQs(
		std::wstring const& inVenue,
		std::wstring const& inOldMultiQ,
		std::wstring const& inNewMultiQ)
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
		std::wstring const& inVenue)
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
		std::wstring const& inVenue,
		std::wstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumMultiHostedTrialsInDivision(inConfig, inVenue, inDiv);
	return count;
}


int ARBDogList::NumExistingPointsInDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetExistingPoints().NumExistingPointsInDivision(inVenue, inDiv);
	return count;
}


int ARBDogList::NumRunsInDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inDiv) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTrials().NumRunsInDivision(inVenue, inDiv);
	return count;
}


int ARBDogList::RenameDivision(
		ARBConfigVenuePtr inVenue,
		std::wstring const& inOldDiv,
		std::wstring const& inNewDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->RenameDivision(inVenue, inOldDiv, inNewDiv);
	return count;
}


int ARBDogList::DeleteDivision(
		ARBConfig const& inConfig,
		std::wstring const& inVenue,
		std::wstring const& inDiv)
{
	int count = 0;
	for (iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->DeleteDivision(inConfig, inVenue, inDiv);
	return count;
}


int ARBDogList::NumLevelsInUse(
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel) const
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
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inOldLevel,
		std::wstring const& inNewLevel)
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
		std::wstring const& inVenue,
		std::wstring const& inDiv,
		std::wstring const& inLevel)
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
		std::wstring const& inVenue,
		std::wstring const& inTitle) const
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().NumTitlesInUse(inVenue, inTitle);
	return count;
}


int ARBDogList::RenameTitle(
		std::wstring const& inVenue,
		std::wstring const& inOldTitle,
		std::wstring const& inNewTitle)
{
	int count = 0;
	for (const_iterator iter = begin(); iter != end(); ++iter)
		count += (*iter)->GetTitles().RenameTitle(inVenue, inOldTitle, inNewTitle);
	return count;
}


int ARBDogList::DeleteTitle(
		std::wstring const& inVenue,
		std::wstring const& inTitle)
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
		std::wstring const& inVenue,
		std::wstring const& inEvent) const
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
		std::wstring const& inVenue,
		std::wstring const& inOldEvent,
		std::wstring const& inNewEvent)
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
		std::wstring const& inVenue,
		std::wstring const& inEvent)
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
