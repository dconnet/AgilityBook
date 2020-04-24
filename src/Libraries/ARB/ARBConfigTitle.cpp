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
 * 2018-12-16 Convert to fmt.
 * 2018-04-26 Moved ShortToRoman to ARBCommon/ARBMisc.cpp
 * 2015-04-26 Fixed recurring title name on instance 0 (new title dialog)
 * 2013-01-13 Added new recurring title suffix style.
 * 2012-07-30 Added new recurring title style (none).
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-14 Added option to prefix a title.
 * 2005-01-11 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-05 Added LongName.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARB/ARBConfigTitle.h"

#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/Element.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

ARBAttribLookup LoadTitleStyle(
	ElementNodePtr const& inTree,
	wchar_t const* inAttrib,
	ARBVersion const& inVersion,
	ARBTitleStyle& ioStyle)
{
	ioStyle = ARBTitleStyle::Number;
	ARBAttribLookup found = ARBAttribLookup::NotFound;
	if (inVersion < ARBVersion(14, 0))
	{
		short tmp;
		found = inTree->GetAttrib(inAttrib, tmp);
		if (ARBAttribLookup::Found == found)
		{
			switch (tmp)
			{
			default:
				assert(0);
				found = ARBAttribLookup::Invalid;
				break;
			case 0:
				ioStyle = ARBTitleStyle::Number;
				break;
			case 1:
				ioStyle = ARBTitleStyle::Roman;
				break;
			case 2:
				ioStyle = ARBTitleStyle::None;
				break;
			}
		}
	}
	else
	{
		std::wstring tmp;
		found = inTree->GetAttrib(inAttrib, tmp);
		if (ARBAttribLookup::Found == found)
		{
			if (L"0" == tmp)
				ioStyle = ARBTitleStyle::None;
			else if (L"n" == tmp)
				ioStyle = ARBTitleStyle::Number;
			else if (L"r" == tmp)
				ioStyle = ARBTitleStyle::Roman;
			else
			{
				assert(0);
				found = ARBAttribLookup::Invalid;
			}
		}
	}
	return found;
}


void SaveTitleStyle(ElementNodePtr const& ioTree, wchar_t const* inAttrib, ARBTitleStyle inStyle)
{
	std::wstring style;
	switch (inStyle)
	{
	case ARBTitleStyle::None:
		style = L"0";
		break;
	case ARBTitleStyle::Number:
		// default
		break;
	case ARBTitleStyle::Roman:
		style = L"r";
		break;
	}
	if (!style.empty())
		ioTree->AddAttrib(inAttrib, style);
}

/////////////////////////////////////////////////////////////////////////////

ARBAttribLookup LoadTitleSeparator(
	ElementNodePtr const& inTree,
	wchar_t const* inAttrib,
	ARBVersion const& inVersion,
	ARBTitleStyle inStyle,
	ARBTitleSeparator& ioSep)
{
	ioSep = ARBTitleSeparator::None;
	std::wstring tmp;
	ARBAttribLookup found = inTree->GetAttrib(inAttrib, tmp);
	if (ARBAttribLookup::Found == found)
	{
		if (L"n" == tmp)
			ioSep = ARBTitleSeparator::None;
		else if (L"s" == tmp)
			ioSep = ARBTitleSeparator::Space;
		else if (L"h" == tmp)
			ioSep = ARBTitleSeparator::Hyphen;
		else
		{
			assert(0);
			found = ARBAttribLookup::Invalid;
		}
	}

	if (inVersion < ARBVersion(14, 0))
	{
		if (ARBTitleStyle::Roman == inStyle)
		{
			ioSep = ARBTitleSeparator::Hyphen;
		}
	}
	return found;
}


void SaveTitleSeparator(ElementNodePtr const& ioTree, wchar_t const* inAttrib, ARBTitleSeparator inSep)
{
	std::wstring sep;
	switch (inSep)
	{
	case ARBTitleSeparator::None:
		// default
		break;
	case ARBTitleSeparator::Space:
		sep = L"s";
		break;
	case ARBTitleSeparator::Hyphen:
		sep = L"h";
		break;
	}
	if (!sep.empty())
		ioTree->AddAttrib(inAttrib, sep);
}


static std::wstring GetSeparator(ARBTitleSeparator inSep)
{
	std::wstring sep;
	switch (inSep)
	{
	case ARBTitleSeparator::None:
		break;
	case ARBTitleSeparator::Space:
		sep = L" ";
		break;
	case ARBTitleSeparator::Hyphen:
		sep = L"-";
		break;
	}
	return sep;
}

/////////////////////////////////////////////////////////////////////////////

std::wstring ARBTitleInstance::TitleInstance(
	bool bShowInstanceOne,
	short instance,
	short startAt,
	short increment,
	ARBTitleStyle style,
	ARBTitleSeparator sep) const
{
	std::wstring str;
	if (bShowInstanceOne || 1 < instance)
	{
		// If we're showing in the selection menu (new title), there is no
		// instance, so don't show "title0".
		if (bShowInstanceOne && 0 == instance)
			style = ARBTitleStyle::None;
		short value = startAt + (instance - 1) * increment;
		switch (style)
		{
		case ARBTitleStyle::None:
			break;
		case ARBTitleStyle::Number:
			str = fmt::format(L"{}{}", GetSeparator(sep), value);
			break;
		case ARBTitleStyle::Roman:
			str = fmt::format(L"{}{}", GetSeparator(sep), ShortToRoman(value));
			break;
		}
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ARBConfigTitle_concrete : public ARBConfigTitle
{
public:
	ARBConfigTitle_concrete()
	{
	}
	ARBConfigTitle_concrete(ARBConfigTitle const& rhs)
		: ARBConfigTitle(rhs)
	{
	}
};
}; // namespace


ARBConfigTitlePtr ARBConfigTitle::New()
{
	return std::make_shared<ARBConfigTitle_concrete>();
}


ARBConfigTitle::ARBConfigTitle()
	: m_Name()
	, m_LongName()
	, m_Desc()
	, m_Prefix(false)
	, m_ValidFrom()
	, m_ValidTo()
	, m_MultipleStartAt(0)
	, m_MultipleIncrement(1)
	, m_MultipleOnFirst(false)
	, m_MultipleStyle(ARBTitleStyle::Number)
	, m_MultipleSeparator(ARBTitleSeparator::None)
{
}


ARBConfigTitle::ARBConfigTitle(ARBConfigTitle const& rhs)
	: m_Name(rhs.m_Name)
	, m_LongName(rhs.m_LongName)
	, m_Desc(rhs.m_Desc)
	, m_Prefix(rhs.m_Prefix)
	, m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_MultipleStartAt(rhs.m_MultipleStartAt)
	, m_MultipleIncrement(rhs.m_MultipleIncrement)
	, m_MultipleOnFirst(rhs.m_MultipleOnFirst)
	, m_MultipleStyle(rhs.m_MultipleStyle)
	, m_MultipleSeparator(rhs.m_MultipleSeparator)
{
}


ARBConfigTitle::ARBConfigTitle(ARBConfigTitle&& rhs)
	: m_Name(std::move(rhs.m_Name))
	, m_LongName(std::move(rhs.m_LongName))
	, m_Desc(std::move(rhs.m_Desc))
	, m_Prefix(std::move(rhs.m_Prefix))
	, m_ValidFrom(std::move(rhs.m_ValidFrom))
	, m_ValidTo(std::move(rhs.m_ValidTo))
	, m_MultipleStartAt(std::move(rhs.m_MultipleStartAt))
	, m_MultipleIncrement(std::move(rhs.m_MultipleIncrement))
	, m_MultipleOnFirst(std::move(rhs.m_MultipleOnFirst))
	, m_MultipleStyle(std::move(rhs.m_MultipleStyle))
	, m_MultipleSeparator(std::move(rhs.m_MultipleSeparator))
{
}


ARBConfigTitle::~ARBConfigTitle()
{
}


ARBConfigTitlePtr ARBConfigTitle::Clone() const
{
	return std::make_shared<ARBConfigTitle_concrete>(*this);
}


ARBConfigTitle& ARBConfigTitle::operator=(ARBConfigTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_LongName = rhs.m_LongName;
		m_Desc = rhs.m_Desc;
		m_Prefix = rhs.m_Prefix;
		m_ValidFrom = rhs.m_ValidFrom;
		m_ValidTo = rhs.m_ValidTo;
		m_MultipleStartAt = rhs.m_MultipleStartAt;
		m_MultipleIncrement = rhs.m_MultipleIncrement;
		m_MultipleOnFirst = rhs.m_MultipleOnFirst;
		m_MultipleStyle = rhs.m_MultipleStyle;
		m_MultipleSeparator = rhs.m_MultipleSeparator;
	}
	return *this;
}


ARBConfigTitle& ARBConfigTitle::operator=(ARBConfigTitle&& rhs)
{
	if (this != &rhs)
	{
		m_Name = std::move(rhs.m_Name);
		m_LongName = std::move(rhs.m_LongName);
		m_Desc = std::move(rhs.m_Desc);
		m_Prefix = std::move(rhs.m_Prefix);
		m_ValidFrom = std::move(rhs.m_ValidFrom);
		m_ValidTo = std::move(rhs.m_ValidTo);
		m_MultipleStartAt = std::move(rhs.m_MultipleStartAt);
		m_MultipleIncrement = std::move(rhs.m_MultipleIncrement);
		m_MultipleOnFirst = std::move(rhs.m_MultipleOnFirst);
		m_MultipleStyle = std::move(rhs.m_MultipleStyle);
		m_MultipleSeparator = std::move(rhs.m_MultipleSeparator);
	}
	return *this;
}


bool ARBConfigTitle::operator==(ARBConfigTitle const& rhs) const
{
	// clang-format off
	return m_Name == rhs.m_Name
		&& m_LongName == rhs.m_LongName
		&& m_Desc == rhs.m_Desc
		&& m_Prefix == rhs.m_Prefix
		&& m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_MultipleStartAt == rhs.m_MultipleStartAt
		&& m_MultipleIncrement == rhs.m_MultipleIncrement
		&& m_MultipleOnFirst == rhs.m_MultipleOnFirst
		&& m_MultipleStyle == rhs.m_MultipleStyle
		&& m_MultipleSeparator == rhs.m_MultipleSeparator;
	// clang-format on
}


void ARBConfigTitle::clear()
{
	m_Name.clear();
	m_LongName.clear();
	m_Desc.clear();
	m_Prefix = false;
	m_ValidFrom.clear();
	m_ValidTo.clear();
	m_MultipleStartAt = 0;
	m_MultipleIncrement = 1;
	m_MultipleOnFirst = false;
	m_MultipleStyle = ARBTitleStyle::Number;
	m_MultipleSeparator = ARBTitleSeparator::None;
}


bool ARBConfigTitle::Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLES)
		return false;
	if (ARBAttribLookup::Found != inTree->GetAttrib(ATTRIB_TITLES_NAME, m_Name) || 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLES_NAME));
		return false;
	}

	inTree->GetAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	m_Desc = inTree->GetValue();

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_TITLES_PREFIX, m_Prefix))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(
			TREE_TITLES,
			ATTRIB_TITLES_PREFIX,
			Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDFROM, msg.c_str()));
		return false;
	}
	if (ARBAttribLookup::Invalid == inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(
			Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDTO, msg.c_str()));
		return false;
	}

	if (inVersion < ARBVersion(14, 0))
	{
		short multiple;
		if (ARBAttribLookup::Found == inTree->GetAttrib(L"Multiple", multiple))
		{
			if (1 == multiple)
			{
				m_MultipleStartAt = 1;
				m_MultipleOnFirst = true;
			}
			else if (2 == multiple)
			{
				m_MultipleStartAt = 1;
				m_MultipleOnFirst = false;
			}
		}
	}
	else
	{
		inTree->GetAttrib(ATTRIB_TITLES_MULTIPLE_STARTAT, m_MultipleStartAt);
	}

	if (0 < m_MultipleStartAt)
	{
		inTree->GetAttrib(ATTRIB_TITLES_MULTIPLE_INC, m_MultipleIncrement);
		inTree->GetAttrib(ATTRIB_TITLES_MULTIPLE_ONFIRST, m_MultipleOnFirst);
		LoadTitleStyle(inTree, ATTRIB_TITLES_MULTIPLE_STYLE, inVersion, m_MultipleStyle);
		LoadTitleSeparator(inTree, ATTRIB_TITLES_MULTIPLE_SEP, inVersion, m_MultipleStyle, m_MultipleSeparator);
	}

	return true;
}


bool ARBConfigTitle::Save(ElementNodePtr const& ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_TITLES);
	title->AddAttrib(ATTRIB_TITLES_NAME, m_Name);
	if (0 < m_LongName.length())
		title->AddAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	if (0 < m_Desc.length())
		title->SetValue(m_Desc);
	if (m_Prefix)
		title->AddAttrib(ATTRIB_TITLES_PREFIX, m_Prefix);
	if (m_ValidFrom.IsValid())
		title->AddAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		title->AddAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo);

	if (0 < m_MultipleStartAt)
	{
		title->AddAttrib(ATTRIB_TITLES_MULTIPLE_STARTAT, m_MultipleStartAt);
		if (1 < m_MultipleIncrement)
			title->AddAttrib(ATTRIB_TITLES_MULTIPLE_INC, m_MultipleIncrement);
		if (m_MultipleOnFirst)
			title->AddAttrib(ATTRIB_TITLES_MULTIPLE_ONFIRST, m_MultipleOnFirst);
		SaveTitleStyle(title, ATTRIB_TITLES_MULTIPLE_STYLE, m_MultipleStyle);
		SaveTitleSeparator(title, ATTRIB_TITLES_MULTIPLE_SEP, m_MultipleSeparator);
	}

	return true;
}


std::wstring ARBConfigTitle::GetTitleName(short inInstance) const
{
	return fmt::format(
		L"{}{}",
		m_Name,
		TitleInstance(
			0 > inInstance ? false : m_MultipleOnFirst,
			inInstance,
			m_MultipleStartAt,
			m_MultipleIncrement,
			m_MultipleStyle,
			m_MultipleSeparator));
}


std::wstring ARBConfigTitle::GetCompleteName(short inInstance, bool bAbbrevFirst, bool bAddDates) const
{
	fmt::wmemory_buffer buffer;
	fmt::format_to(
		buffer,
		L"{}",
		TitleInstance(
			0 > inInstance ? false : m_MultipleOnFirst,
			inInstance,
			m_MultipleStartAt,
			m_MultipleIncrement,
			m_MultipleStyle,
			m_MultipleSeparator));
	// Special formatting used in configuration dialogs.
	if (0 > inInstance && 0 < m_MultipleStartAt)
	{
		fmt::format_to(buffer, L"+");
	}
	fmt::wmemory_buffer name;
	if (0 < m_LongName.length())
	{
		if (bAbbrevFirst)
		{
			fmt::format_to(name, L"[{}{}] ", m_Name, fmt::to_string(buffer));
		}
		fmt::format_to(name, L"{}", m_LongName);
		if (!bAbbrevFirst)
		{
			fmt::format_to(name, L" [{}{}]", m_Name, fmt::to_string(buffer));
		}
	}
	else
	{
		fmt::format_to(name, L"{}{}", m_Name, fmt::to_string(buffer));
	}
	if (bAddDates)
	{
		std::wstring dates = ARBDate::GetValidDateString(m_ValidFrom, m_ValidTo);
		if (!dates.empty())
			fmt::format_to(name, L" {}", dates);
	}
	return fmt::to_string(name);
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitleList::Load(
	ElementNodePtr const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback,
	bool inCheckDups)
{
	ARBConfigTitlePtr thing(ARBConfigTitle::New());
	if (!thing->Load(inTree, inVersion, ioCallback) || (inCheckDups && FindTitle(thing->GetName())))
	{
		return false;
	}
	push_back(thing);
	return true;
}


void ARBConfigTitleList::ReorderBy(ARBConfigTitleList const& inList)
{
	if (*this != inList)
	{
		ARBConfigTitleList tmp;
		tmp.reserve(size());
		for (ARBConfigTitleList::const_iterator i = inList.begin(); i != inList.end(); ++i)
		{
			ARBConfigTitlePtr title;
			if (FindTitle((*i)->GetName(), &title))
			{
				tmp.push_back(title);
				ARBConfigTitleList::iterator iFound = std::find(begin(), end(), title);
				assert(iFound != end());
				erase(iFound);
			}
		}
		if (0 < size())
			tmp.insert(tmp.end(), begin(), end());
		std::swap(tmp, *this);
	}
}


bool ARBConfigTitleList::FindTitleCompleteName(
	std::wstring const& inName,
	short inInstance,
	bool bAbbrevFirst,
	ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetCompleteName(inInstance, bAbbrevFirst) == inName)
		{
			if (outTitle)
				*outTitle = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigTitleList::FindTitle(std::wstring const& inName, ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
		{
			if (outTitle)
				*outTitle = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigTitleList::AddTitle(std::wstring const& inName, ARBConfigTitlePtr* outTitle)
{
	if (outTitle)
		outTitle->reset();
	if (0 == inName.length())
		return false;
	if (FindTitle(inName))
		return false;
	ARBConfigTitlePtr pTitle(ARBConfigTitle::New());
	pTitle->SetName(inName);
	push_back(pTitle);
	if (outTitle)
		*outTitle = pTitle;
	return true;
}


bool ARBConfigTitleList::AddTitle(ARBConfigTitlePtr const& inTitle)
{
	if (!inTitle || 0 == inTitle->GetName().length())
		return false;
	if (FindTitle(inTitle->GetName()))
		return false;
	push_back(inTitle);
	return true;
}


bool ARBConfigTitleList::DeleteTitle(std::wstring const& inName)
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
