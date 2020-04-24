/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The training logbook
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-04 Changed ARBDate::GetString to take a format code.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-14 Added FindTraining to support importing data.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-21 Created
 */

#include "stdafx.h"
#include "ARB/ARBTraining.h"

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
class ARBTraining_concrete : public ARBTraining
{
public:
	ARBTraining_concrete()
	{
	}
	ARBTraining_concrete(ARBTraining const& rhs)
		: ARBTraining(rhs)
	{
	}
};
}; // namespace


ARBTrainingPtr ARBTraining::New()
{
	return std::make_shared<ARBTraining_concrete>();
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


ARBTraining::ARBTraining(ARBTraining&& rhs)
	: m_Date(std::move(rhs.m_Date))
	, m_Name(std::move(rhs.m_Name))
	, m_SubName(std::move(rhs.m_SubName))
	, m_Note(std::move(rhs.m_Note))
{
}


ARBTraining::~ARBTraining()
{
}


ARBTrainingPtr ARBTraining::Clone() const
{
	return std::make_shared<ARBTraining_concrete>(*this);
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


ARBTraining& ARBTraining::operator=(ARBTraining&& rhs)
{
	if (this != &rhs)
	{
		m_Date = std::move(rhs.m_Date);
		m_Name = std::move(rhs.m_Name);
		m_SubName = std::move(rhs.m_SubName);
		m_Note = std::move(rhs.m_Note);
	}
	return *this;
}


bool ARBTraining::operator==(ARBTraining const& rhs) const
{
	// clang-format off
	return m_Date == rhs.m_Date
		&& m_Name == rhs.m_Name
		&& m_SubName == rhs.m_SubName
		&& m_Note == rhs.m_Note;
	// clang-format on
}


size_t ARBTraining::GetSearchStrings(std::set<std::wstring>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Date.GetString(ARBDateFormat::SlashMDY));
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


bool ARBTraining::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TRAINING)
		return false;
	switch (inTree->GetAttrib(ATTRIB_TRAINING_DATE, m_Date))
	{
	case ARBAttribLookup::Found:
		break;
	case ARBAttribLookup::NotFound:
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TRAINING, ATTRIB_TRAINING_DATE));
		return false;
	case ARBAttribLookup::Invalid:
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_TRAINING_DATE, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_TRAINING, ATTRIB_TRAINING_DATE, msg.c_str()));
	}
		return false;
	}

	inTree->GetAttrib(ATTRIB_TRAINING_NAME, m_Name);
	inTree->GetAttrib(ATTRIB_TRAINING_SUBNAME, m_SubName);

	m_Note = inTree->GetValue();
	return true;
}


bool ARBTraining::Save(ElementNodePtr const& ioTree) const
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

bool ARBTrainingList::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	ARBTrainingPtr thing(ARBTraining::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


void ARBTrainingList::sort()
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), [](ARBTrainingPtr const& one, ARBTrainingPtr const& two) {
		return one->GetDate() < two->GetDate();
	});
}


size_t ARBTrainingList::GetAllNames(std::set<std::wstring>& outNames) const
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


size_t ARBTrainingList::GetAllSubNames(std::set<std::wstring>& outSubNames) const
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


bool ARBTrainingList::FindTraining(ARBTrainingPtr const& inTraining) const
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


bool ARBTrainingList::AddTraining(ARBTrainingPtr const& inTraining)
{
	bool bAdded = false;
	if (inTraining)
	{
		bAdded = true;
		push_back(inTraining);
	}
	return bAdded;
}


bool ARBTrainingList::DeleteTraining(ARBTrainingPtr const& inTraining)
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
