/*
 * Copyright © 2003-2007 David Connet. All Rights Reserved.
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
 * @brief The training logbook
 * @author David Connet
 *
 * Revision History
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

#include "StdAfx.h"
#include "ARBTraining.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
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


size_t ARBTraining::GetSearchStrings(std::set<ARBString>& ioStrings) const
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
	ASSERT(inTree);
	if (!inTree)
		return false;
	switch (inTree->GetAttrib(ATTRIB_TRAINING_DATE, m_Date))
	{
	case ElementNode::eNotFound:
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_TRAINING, ATTRIB_TRAINING_DATE));
		return false;
	case ElementNode::eInvalidValue:
		{
			ARBString attrib;
			inTree->GetAttrib(ATTRIB_TRAINING_DATE, attrib);
			ARBString msg(INVALID_DATE);
			msg += attrib;
			ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TRAINING, ATTRIB_TRAINING_DATE, msg.c_str()));
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
	ASSERT(ioTree);
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


size_t ARBTrainingList::GetAllNames(std::set<ARBString>& outNames) const
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


size_t ARBTrainingList::GetAllSubNames(std::set<ARBString>& outSubNames) const
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
