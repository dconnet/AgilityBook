/*
 * Copyright (c) 2002-2009 David Connet. All Rights Reserved.
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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBDogRun.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////

ARBDogReferenceRunPtr ARBDogReferenceRun::New()
{
	return ARBDogReferenceRunPtr(new ARBDogReferenceRun());
}


ARBDogReferenceRun::ARBDogReferenceRun()
	: m_Q()
	, m_Place(0)
	, m_Name()
	, m_Height()
	, m_Breed()
	, m_Time(0.0)
	, m_Score()
	, m_Note()
{
}


ARBDogReferenceRun::ARBDogReferenceRun(ARBDogReferenceRun const& rhs)
	: m_Q(rhs.m_Q)
	, m_Place(rhs.m_Place)
	, m_Name(rhs.m_Name)
	, m_Height(rhs.m_Height)
	, m_Breed(rhs.m_Breed)
	, m_Time(rhs.m_Time)
	, m_Score(rhs.m_Score)
	, m_Note(rhs.m_Note)
{
}


ARBDogReferenceRun::~ARBDogReferenceRun()
{
}


ARBDogReferenceRunPtr ARBDogReferenceRun::Clone() const
{
	return ARBDogReferenceRunPtr(new ARBDogReferenceRun(*this));
}


ARBDogReferenceRun& ARBDogReferenceRun::operator=(ARBDogReferenceRun const& rhs)
{
	if (this != &rhs)
	{
		m_Q = rhs.m_Q;
		m_Place = rhs.m_Place;
		m_Name = rhs.m_Name;
		m_Height = rhs.m_Height;
		m_Breed = rhs.m_Breed;
		m_Time = rhs.m_Time;
		m_Score = rhs.m_Score;
		m_Note = rhs.m_Note;
	}
	return *this;
}


bool ARBDogReferenceRun::operator==(ARBDogReferenceRun const& rhs) const
{
	return m_Q == rhs.m_Q
		&& m_Place == rhs.m_Place
		&& m_Name == rhs.m_Name
		&& m_Height == rhs.m_Height
		&& m_Breed == rhs.m_Breed
		&& m_Time == rhs.m_Time
		&& m_Score == rhs.m_Score
		&& m_Note == rhs.m_Note;
}


size_t ARBDogReferenceRun::GetSearchStrings(std::set<wxString>& ioStrings) const
{
	size_t nItems = 0;

	if (0 < m_Name.length())
	{
		ioStrings.insert(m_Name);
		++nItems;
	}

	if (0 < m_Height.length())
	{
		ioStrings.insert(m_Height);
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

	return nItems;
}


bool ARBDogReferenceRun::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_REF_RUN)
		return false;

	wxString attrib;

	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_REF_RUN_Q, attrib))
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_REF_RUN, ATTRIB_REF_RUN_Q));
		return false;
	}
	if (!m_Q.Load(attrib, inVersion, ioCallback))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_REF_RUN, ATTRIB_REF_RUN_Q));
		return false;
	}

	inTree->GetAttrib(ATTRIB_REF_RUN_TIME, m_Time);
	inTree->GetAttrib(ATTRIB_REF_RUN_PLACE, m_Place);
	inTree->GetAttrib(ATTRIB_REF_RUN_HEIGHT, m_Height);

	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == TREE_REF_NAME)
		{
			m_Name = element->GetValue();
		}
		else if (element->GetName() == TREE_REF_BREED)
		{
			m_Breed = element->GetValue();
		}
		else if (element->GetName() == TREE_REF_SCORE)
		{
			m_Score = element->GetValue();
		}
		else if (element->GetName() == TREE_REF_NOTE)
		{
			m_Note = element->GetValue();
		}
	}

	return true;
}


bool ARBDogReferenceRun::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr refRun = ioTree->AddElementNode(TREE_REF_RUN);
	m_Q.Save(refRun, ATTRIB_REF_RUN_Q);
	refRun->AddAttrib(ATTRIB_REF_RUN_PLACE, m_Place);
	if (0 < m_Height.length())
		refRun->AddAttrib(ATTRIB_REF_RUN_HEIGHT, m_Height);

	if (0 < m_Name.length())
	{
		ElementNodePtr element = refRun->AddElementNode(TREE_REF_NAME);
		element->SetValue(m_Name);
	}
	if (0 < m_Breed.length())
	{
		ElementNodePtr element = refRun->AddElementNode(TREE_REF_BREED);
		element->SetValue(m_Breed);
	}
	if (0 < m_Score.length())
	{
		ElementNodePtr element = refRun->AddElementNode(TREE_REF_SCORE);
		element->SetValue(m_Score);
	}
	refRun->AddAttrib(ATTRIB_REF_RUN_TIME, m_Time);
	if (0 < m_Note.length())
	{
		ElementNodePtr element = refRun->AddElementNode(TREE_REF_NOTE);
		element->SetValue(m_Note);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogReferenceRunList::Load(
		ARBConfig const& inConfig,
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogReferenceRunPtr thing(ARBDogReferenceRun::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogReferenceRunList::AddReferenceRun(ARBDogReferenceRunPtr inRef)
{
	bool bAdded = false;
	if (inRef)
	{
		bAdded = true;
		push_back(inRef);
	}
	return bAdded;
}


bool ARBDogReferenceRunList::DeleteReferenceRun(ARBDogReferenceRunPtr inRef)
{
	if (inRef)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inRef)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
