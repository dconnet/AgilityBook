/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBDogRun.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfig.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

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

//static
ARBDogReferenceRunPtr ARBDogReferenceRun::New()
{
	return ARBDogReferenceRunPtr(new ARBDogReferenceRun());
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

bool ARBDogReferenceRun::operator!=(ARBDogReferenceRun const& rhs) const
{
	return !operator==(rhs);
}

size_t ARBDogReferenceRun::GetSearchStrings(std::set<ARBString>& ioStrings) const
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
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBString attrib;

	if (Element::eFound != inTree.GetAttrib(ATTRIB_REF_RUN_Q, attrib))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_REF_RUN, ATTRIB_REF_RUN_Q));
		return false;
	}
	if (!m_Q.Load(attrib, inVersion, ioCallback))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_REF_RUN, ATTRIB_REF_RUN_Q));
		return false;
	}

	inTree.GetAttrib(ATTRIB_REF_RUN_TIME, m_Time);
	inTree.GetAttrib(ATTRIB_REF_RUN_PLACE, m_Place);
	inTree.GetAttrib(ATTRIB_REF_RUN_HEIGHT, m_Height);

	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		if (element.GetName() == TREE_REF_NAME)
		{
			m_Name = element.GetValue();
		}
		else if (element.GetName() == TREE_REF_BREED)
		{
			m_Breed = element.GetValue();
		}
		else if (element.GetName() == TREE_REF_SCORE)
		{
			m_Score = element.GetValue();
		}
		else if (element.GetName() == TREE_REF_NOTE)
		{
			m_Note = element.GetValue();
		}
	}

	return true;
}

bool ARBDogReferenceRun::Save(Element& ioTree) const
{
	Element& refRun = ioTree.AddElement(TREE_REF_RUN);
	m_Q.Save(refRun, ATTRIB_REF_RUN_Q);
	refRun.AddAttrib(ATTRIB_REF_RUN_PLACE, m_Place);
	if (0 < m_Height.length())
		refRun.AddAttrib(ATTRIB_REF_RUN_HEIGHT, m_Height);

	{
		Element& element = refRun.AddElement(TREE_REF_NAME);
		element.SetValue(m_Name);
	}
	if (0 < m_Breed.length())
	{
		Element& element = refRun.AddElement(TREE_REF_BREED);
		element.SetValue(m_Breed);
	}
	if (0 < m_Score.length())
	{
		Element& element = refRun.AddElement(TREE_REF_SCORE);
		element.SetValue(m_Score);
	}
	refRun.AddAttrib(ATTRIB_REF_RUN_TIME, m_Time);
	if (0 < m_Note.length())
	{
		Element& element = refRun.AddElement(TREE_REF_NOTE);
		element.SetValue(m_Note);
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBDogReferenceRunList::Load(
		ARBConfig const& inConfig,
		Element const& inTree,
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
