/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBDogRun.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBDogReferenceRun_concrete : public ARBDogReferenceRun
	{
	public:
		ARBDogReferenceRun_concrete() {}
		ARBDogReferenceRun_concrete(ARBDogReferenceRun const& rhs)
			: ARBDogReferenceRun(rhs)
		{
		}
	};
};


ARBDogReferenceRunPtr ARBDogReferenceRun::New()
{
	return std::make_shared<ARBDogReferenceRun_concrete>();
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


ARBDogReferenceRun::ARBDogReferenceRun(ARBDogReferenceRun&& rhs)
	: m_Q(std::move(rhs.m_Q))
	, m_Place(std::move(rhs.m_Place))
	, m_Name(std::move(rhs.m_Name))
	, m_Height(std::move(rhs.m_Height))
	, m_Breed(std::move(rhs.m_Breed))
	, m_Time(std::move(rhs.m_Time))
	, m_Score(std::move(rhs.m_Score))
	, m_Note(std::move(rhs.m_Note))
{
}


ARBDogReferenceRun::~ARBDogReferenceRun()
{
}


ARBDogReferenceRunPtr ARBDogReferenceRun::Clone() const
{
	return std::make_shared<ARBDogReferenceRun_concrete>(*this);
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


ARBDogReferenceRun& ARBDogReferenceRun::operator=(ARBDogReferenceRun&& rhs)
{
	if (this != &rhs)
	{
		m_Q = std::move(rhs.m_Q);
		m_Place = std::move(rhs.m_Place);
		m_Name = std::move(rhs.m_Name);
		m_Height = std::move(rhs.m_Height);
		m_Breed = std::move(rhs.m_Breed);
		m_Time = std::move(rhs.m_Time);
		m_Score = std::move(rhs.m_Score);
		m_Note = std::move(rhs.m_Note);
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


size_t ARBDogReferenceRun::GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_REF_RUN)
		return false;

	std::wstring attrib;

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


bool ARBDogReferenceRun::Save(ElementNodePtr const& ioTree) const
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
		ElementNodePtr const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBDogReferenceRunPtr thing(ARBDogReferenceRun::New());
	if (!thing->Load(inConfig, inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBDogReferenceRunList::AddReferenceRun(ARBDogReferenceRunPtr const& inRef)
{
	bool bAdded = false;
	if (inRef)
	{
		bAdded = true;
		push_back(inRef);
	}
	return bAdded;
}


bool ARBDogReferenceRunList::DeleteReferenceRun(ARBDogReferenceRunPtr const& inRef)
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
