/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 * @li 2003-09-21 DRC Created
 */

#include "StdAfx.h"
#include "ARBTraining.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBTraining::ARBTraining()
	: m_Date()
	, m_Name()
	, m_Note()
{
}

ARBTraining::ARBTraining(const ARBTraining& rhs)
	: m_Date(rhs.m_Date)
	, m_Name(rhs.m_Name)
	, m_Note(rhs.m_Note)
{
}

ARBTraining::~ARBTraining()
{
}

ARBTraining& ARBTraining::operator=(const ARBTraining& rhs)
{
	if (this != &rhs)
	{
		m_Date = rhs.m_Date;
		m_Name = rhs.m_Name;
		m_Note = rhs.m_Note;
	}
	return *this;
}

bool ARBTraining::operator==(const ARBTraining& rhs) const
{
	return m_Date == rhs.m_Date
		&& m_Name == rhs.m_Name
		&& m_Note == rhs.m_Note;
}

bool ARBTraining::operator!=(const ARBTraining& rhs) const
{
	return !operator==(rhs);
}

std::string ARBTraining::GetGenericName() const
{
	return m_Date.GetString(false, false);
}

bool ARBTraining::Load(
	const CElement& inTree,
	int inVersion)
{
	switch (inTree.GetAttrib(ATTRIB_TRAINING_DATE, m_Date))
	{
	case CElement::eNotFound:
		ErrorMissingAttribute(TREE_TRAINING, ATTRIB_TRAINING_DATE);
		return false;
	case CElement::eInvalidValue:
		{
			std::string attrib;
			inTree.GetAttrib(ATTRIB_TRAINING_DATE, attrib);
			std::string msg(INVALID_DATE);
			msg += attrib;
			ErrorInvalidAttributeValue(TREE_TRAINING, ATTRIB_TRAINING_DATE, msg.c_str());
		}
		return false;
	}

	inTree.GetAttrib(ATTRIB_TRAINING_NAME, m_Name);

	m_Note = inTree.GetValue();
	return true;
}

bool ARBTraining::Save(CElement& ioTree) const
{
	CElement& training = ioTree.AddElement(TREE_TRAINING);
	training.AddAttrib(ATTRIB_TRAINING_DATE, m_Date);
	if (0 < m_Name.length())
		training.AddAttrib(ATTRIB_TRAINING_NAME, m_Name);
	if (0 < m_Note.length())
		training.SetValue(m_Note);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortTraining
{
public:
	SortTraining(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBTraining* one, ARBTraining* two) const
	{
		if (one->GetDate() < two->GetDate())
			return m_bDescending;
		else if (one->GetDate() > two->GetDate())
			return !m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBTrainingList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortTraining(inDescending));
}

size_t ARBTrainingList::GetAllNames(std::set<std::string>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		const ARBTraining* training = *iter;
		if (0 < training->GetName().length())
			outNames.insert(training->GetName());
	}
	return outNames.size();
}

ARBTraining* ARBTrainingList::AddTraining(ARBTraining* inTraining)
{
	if (inTraining)
	{
		inTraining->AddRef();
		push_back(inTraining);
	}
	return inTraining;
}

bool ARBTrainingList::DeleteTraining(const ARBTraining* inTraining)
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
