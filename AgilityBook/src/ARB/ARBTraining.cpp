/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief The training logbook
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-14 DRC Added FindTraining to support importing data.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-21 DRC Created
 */

#include "stdafx.h"
#include "ARBTraining.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

ARBTrainingPtr ARBTraining::New()
{
	return ARBTrainingPtr(new ARBTraining());
}


ARBTraining::ARBTraining()
	: m_Date()
	, m_Name()
	, m_SubName()
	, m_Note()
{
}


ARBTraining::ARBTraining(ARBTraining const& rhs)
	: m_Date(rhs.m_Date)
	, m_Name(rhs.m_Name)
	, m_SubName(rhs.m_SubName)
	, m_Note(rhs.m_Note)
{
}


ARBTraining::~ARBTraining()
{
}


ARBTrainingPtr ARBTraining::Clone() const
{
	return ARBTrainingPtr(new ARBTraining(*this));
}


ARBTraining& ARBTraining::operator=(ARBTraining const& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Name = rhs.m_Name;
		m_SubName = rhs.m_SubName;
		m_Note = rhs.m_Note;
	}
	return *this;
}


bool ARBTraining::operator==(ARBTraining const& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Name == rhs.m_Name
		&& m_SubName == rhs.m_SubName
		&& m_Note == rhs.m_Note;
}


size_t ARBTraining::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDate::eSlashMDY));
	++nItems;

	if (0 < m_Name.length())
	{
		ioStrings.insert(m_Name);
		++nItems;
	}

	if (0 < m_SubName.length())
	{
		ioStrings.insert(m_SubName);
		++nItems;
	}

	if (0 < m_Note.length())
	{
		ioStrings.insert(m_Note);
		++nItems;
	}

	return nItems;
}


bool ARBTraining::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TRAINING)
		return false;
	switch (inTree->GetAttrib(ATTRIB_TRAINING_DATE, m_Date))
	{
	case ElementNode::eNotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TRAINING, ATTRIB_TRAINING_DATE));
		return false;
	case ElementNode::eInvalidValue:
		{
			wxString attrib;
			inTree->GetAttrib(ATTRIB_TRAINING_DATE, attrib);
			wxString msg(Localization()->InvalidDate());
			msg += attrib;
			ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TRAINING, ATTRIB_TRAINING_DATE, msg));
		}
		return false;
	}

	inTree->GetAttrib(ATTRIB_TRAINING_NAME, m_Name);
	inTree->GetAttrib(ATTRIB_TRAINING_SUBNAME, m_SubName);

	m_Note = inTree->GetValue();
	return true;
}


bool ARBTraining::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr training = ioTree->AddElementNode(TREE_TRAINING);
	training->AddAttrib(ATTRIB_TRAINING_DATE, m_Date);
	if (0 < m_Name.length())
		training->AddAttrib(ATTRIB_TRAINING_NAME, m_Name);
	if (0 < m_SubName.length())
		training->AddAttrib(ATTRIB_TRAINING_SUBNAME, m_SubName);
	if (0 < m_Note.length())
		training->SetValue(m_Note);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBTrainingList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBTrainingPtr thing(ARBTraining::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


class SortTraining
{
public:
	SortTraining() {}
	bool operator()(ARBTrainingPtr one, ARBTrainingPtr two) const
	{
		return one->GetDate() < two->GetDate();
	}
};


void ARBTrainingList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortTraining());
}


size_t ARBTrainingList::GetAllNames(std::set<wxString>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBTrainingPtr training = *iter;
		if (0 < training->GetName().length())
			outNames.insert(training->GetName());
	}
	return outNames.size();
}


size_t ARBTrainingList::GetAllSubNames(std::set<wxString>& outSubNames) const
{
	outSubNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBTrainingPtr training = *iter;
		if (0 < training->GetSubName().length())
			outSubNames.insert(training->GetSubName());
	}
	return outSubNames.size();
}


bool ARBTrainingList::FindTraining(ARBTrainingPtr inTraining) const
{
	if (inTraining)
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (*(*iter) == *inTraining)
				return true;
		}
	}
	return false;
}


bool ARBTrainingList::AddTraining(ARBTrainingPtr inTraining)
{
	bool bAdded = false;
	if (inTraining)
	{
		bAdded = true;
		push_back(inTraining);
	}
	return bAdded;
}


bool ARBTrainingList::DeleteTraining(ARBTrainingPtr inTraining)
{
	if (inTraining)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inTraining)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}