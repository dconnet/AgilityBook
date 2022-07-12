/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBConfigOtherPoints.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
namespace ARB
{

namespace
{
constexpr wchar_t OTHERPTS_COUNT_ALL[] = L"All";
constexpr wchar_t OTHERPTS_COUNT_ALLBYEVENT[] = L"AllByEvent";
constexpr wchar_t OTHERPTS_COUNT_LEVEL[] = L"Level";
constexpr wchar_t OTHERPTS_COUNT_LEVELBYEVENT[] = L"LevelByEvent";
} // namespace


void ARBConfigOtherPoints::GetTallyValidValues(std::vector<std::wstring>& outValues)
{
	outValues.clear();
	outValues.push_back(OTHERPTS_COUNT_ALL);
	outValues.push_back(OTHERPTS_COUNT_ALLBYEVENT);
	outValues.push_back(OTHERPTS_COUNT_LEVEL);
	outValues.push_back(OTHERPTS_COUNT_LEVELBYEVENT);
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ARBConfigOtherPoints_concrete : public ARBConfigOtherPoints
{
public:
	ARBConfigOtherPoints_concrete()
	{
	}
	ARBConfigOtherPoints_concrete(ARBConfigOtherPoints const& rhs)
		: ARBConfigOtherPoints(rhs)
	{
	}
};
}; // namespace


ARBConfigOtherPointsPtr ARBConfigOtherPoints::New()
{
	return std::make_shared<ARBConfigOtherPoints_concrete>();
}


ARBConfigOtherPoints::ARBConfigOtherPoints()
	: m_Name()
	, m_Tally(ARBOtherPointsTally::All)
	, m_Desc()
	, m_Default(0.0)
{
}


ARBConfigOtherPoints::ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs)
	: m_Name(rhs.m_Name)
	, m_Tally(rhs.m_Tally)
	, m_Desc(rhs.m_Desc)
	, m_Default(rhs.m_Default)
{
}


ARBConfigOtherPoints::ARBConfigOtherPoints(ARBConfigOtherPoints&& rhs)
	: m_Name(std::move(rhs.m_Name))
	, m_Tally(std::move(rhs.m_Tally))
	, m_Desc(std::move(rhs.m_Desc))
	, m_Default(std::move(rhs.m_Default))
{
}


ARBConfigOtherPoints::~ARBConfigOtherPoints()
{
}


ARBConfigOtherPointsPtr ARBConfigOtherPoints::Clone() const
{
	return std::make_shared<ARBConfigOtherPoints_concrete>(*this);
}


ARBConfigOtherPoints& ARBConfigOtherPoints::operator=(ARBConfigOtherPoints const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Tally = rhs.m_Tally;
		m_Desc = rhs.m_Desc;
		m_Default = rhs.m_Default;
	}
	return *this;
}


ARBConfigOtherPoints& ARBConfigOtherPoints::operator=(ARBConfigOtherPoints&& rhs)
{
	if (this != &rhs)
	{
		m_Name = std::move(rhs.m_Name);
		m_Tally = std::move(rhs.m_Tally);
		m_Desc = std::move(rhs.m_Desc);
		m_Default = std::move(rhs.m_Default);
	}
	return *this;
}


bool ARBConfigOtherPoints::operator==(ARBConfigOtherPoints const& rhs) const
{
	// clang-format off
	return m_Name == rhs.m_Name
		&& m_Tally == rhs.m_Tally
		&& m_Desc == rhs.m_Desc
		&& m_Default == rhs.m_Default;
	// clang-format on
}


void ARBConfigOtherPoints::clear()
{
	m_Name.clear();
	m_Tally = ARBOtherPointsTally::All;
	m_Desc.clear();
	m_Default = 0.0;
}


bool ARBConfigOtherPoints::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_OTHERPTS)
		return false;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_OTHERPTS_NAME, m_Name) || 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_NAME));
		return false;
	}
	inTree->GetAttrib(ATTRIB_OTHERPTS_DEFAULT, m_Default);
	std::wstring attrib;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_OTHERPTS_COUNT, attrib) || 0 == attrib.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT));
		return false;
	}
	if (attrib == OTHERPTS_COUNT_ALL)
		m_Tally = ARBOtherPointsTally::All;
	else if (attrib == OTHERPTS_COUNT_ALLBYEVENT)
		m_Tally = ARBOtherPointsTally::AllByEvent;
	else if (attrib == OTHERPTS_COUNT_LEVEL)
		m_Tally = ARBOtherPointsTally::Level;
	else if (attrib == OTHERPTS_COUNT_LEVELBYEVENT)
		m_Tally = ARBOtherPointsTally::LevelByEvent;
	else
	{
		std::wstring msg(Localization()->ValidValues());
		msg += OTHERPTS_COUNT_ALL;
		msg += L", ";
		msg += OTHERPTS_COUNT_ALLBYEVENT;
		msg += L", ";
		msg += OTHERPTS_COUNT_LEVEL;
		msg += L", ";
		msg += OTHERPTS_COUNT_LEVELBYEVENT;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT, msg.c_str()));
		return false;
	}
	m_Desc = inTree->GetValue();
	return true;
}


bool ARBConfigOtherPoints::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr other = ioTree->AddElementNode(TREE_OTHERPTS);
	other->AddAttrib(ATTRIB_OTHERPTS_NAME, m_Name);
	other->AddAttrib(ATTRIB_OTHERPTS_DEFAULT, m_Default);
	switch (m_Tally)
	{
	case ARBOtherPointsTally::All:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_ALL);
		break;
	case ARBOtherPointsTally::AllByEvent:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_ALLBYEVENT);
		break;
	case ARBOtherPointsTally::Level:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_LEVEL);
		break;
	case ARBOtherPointsTally::LevelByEvent:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_LEVELBYEVENT);
		break;
	}
	if (0 < m_Desc.length())
		other->SetValue(m_Desc);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigOtherPointsList::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	ARBConfigOtherPointsPtr thing(ARBConfigOtherPoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigOtherPointsList::VerifyOtherPoints(std::wstring const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}


bool ARBConfigOtherPointsList::FindOtherPoints(std::wstring const& inName, ARBConfigOtherPointsPtr* outPoints) const
{
	if (outPoints)
		outPoints->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outPoints)
				*outPoints = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigOtherPointsList::AddOtherPoints(ARBConfigOtherPointsPtr const& inOther)
{
	// Global uniqueness must be ensured by the calling function.
	bool bAdded = false;
	if (inOther)
	{
		bAdded = true;
		push_back(inOther);
	}
	return bAdded;
}


bool ARBConfigOtherPointsList::DeleteOtherPoints(std::wstring const& inName)
{
	std::wstring name(inName);
	for (iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == name)
		{
			erase(iter);
			return true;
		}
	}
	return false;
}

} // namespace ARB
} // namespace dconSoft
