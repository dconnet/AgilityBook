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
#include "ARBCommon/Element.h"
#include <algorithm>
#include <math.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


/*
// Roman number conversion (modified from C# code on CodeProject)
static short RomanDigit(wchar_t digit)
{
	switch (digit)
	{
	case 'I':
		return 1;
	case 'V':
		return 5;
	case 'X':
		return 10;
	case 'L':
		return 50;
	case 'C':
		return 100;
	case 'D':
		return 500;
	case 'M':
		return 1000;
	default :
		//We're in a limited world - don't worry about out-of-range
		//throw new ArgumentOutOfRangeException("digit");
		return 0;
	}
}
static short RomanToShort(std::wstring number)
{
	short result = 0;
	short oldValue = 1000;
	for (std::wstring::const_iterator index = number.begin(); index != number.end(); ++index)
	{
		short newValue = RomanDigit(*index);
		if (newValue > oldValue)
			result = result + newValue - 2 * oldValue;
		else
			result += newValue;
		oldValue = newValue;
	}
	return result;
}
*/
static std::wstring ShortToRoman(short value)
{
	static const wchar_t* romanDigits[9][4] =
	{
		{L"M",    L"C",    L"X",    L"I"   },
		{L"MM",   L"CC",   L"XX",   L"II"  },
		{L"MMM",  L"CCC",  L"XXX",  L"III" },
		{nullptr, L"CD",   L"XL",   L"IV"  },
		{nullptr, L"D",    L"L",    L"V"   },
		{nullptr, L"DC",   L"LX",   L"VI"  },
		{nullptr, L"DCC",  L"LXX",  L"VII" },
		{nullptr, L"DCCC", L"LXXX", L"VIII"},
		{nullptr, L"CM",   L"XC",   L"IX"  }
	};
	std::wostringstream result;
	for (int index = 0; index < 4; ++index)
	{
		short power = static_cast<short>(pow(10.0, 3 - index));
		short digit = value / power;
		value -= digit * power;
		if (digit > 0)
			result << romanDigits[digit-1][index];
	}
	return result.str();
}

/////////////////////////////////////////////////////////////////////////////

ElementNode::AttribLookup LoadTitleStyle(
		ElementNodePtr inTree,
		wchar_t const* inAttrib,
		ARBVersion const& inVersion,
		ARBTitleStyle& ioStyle)
{
	ioStyle = eTitleStyleNumber;
	ElementNode::AttribLookup found = ElementNode::eNotFound;
	if (inVersion < ARBVersion(14, 0))
	{
		short tmp;
		found = inTree->GetAttrib(inAttrib, tmp);
		if (ElementNode::eFound == found)
		{
			switch (tmp)
			{
			default:
				assert(0);
				found = ElementNode::eInvalidValue;
				break;
			case 0:
				ioStyle = eTitleStyleNumber;
				break;
			case 1:
				ioStyle = eTitleStyleRoman;
				break;
			case 2:
				ioStyle = eTitleStyleNone;
				break;
			}
		}
	}
	else
	{
		std::wstring tmp;
		found = inTree->GetAttrib(inAttrib, tmp);
		if (ElementNode::eFound == found)
		{
			if (L"0" == tmp)
				ioStyle = eTitleStyleNone;
			else if (L"n" == tmp)
				ioStyle = eTitleStyleNumber;
			else if (L"r" == tmp)
				ioStyle = eTitleStyleRoman;
			else
			{
				assert(0);
				found = ElementNode::eInvalidValue;
			}
		}
	}
	return found;
}


void SaveTitleStyle(
		ElementNodePtr ioTree,
		wchar_t const* inAttrib,
		ARBTitleStyle inStyle)
{
	std::wstring style;
	switch (inStyle)
	{
	default:
		assert(0);
		break;
	case eTitleStyleNone:
		style = L"0";
		break;
	case eTitleStyleNumber:
		// default
		break;
	case eTitleStyleRoman:
		style = L"r";
		break;
	}
	if (!style.empty())
		ioTree->AddAttrib(inAttrib, style);
}

/////////////////////////////////////////////////////////////////////////////

ElementNode::AttribLookup LoadTitleSeparator(
		ElementNodePtr inTree,
		wchar_t const* inAttrib,
		ARBVersion const& inVersion,
		ARBTitleStyle inStyle,
		ARBTitleSeparator& ioSep)
{
	ioSep = eTitleSeparatorNone;
	std::wstring tmp;
	ElementNode::AttribLookup found = inTree->GetAttrib(inAttrib, tmp);
	if (ElementNode::eFound == found)
	{
		if (L"n" == tmp)
			ioSep = eTitleSeparatorNone;
		else if (L"s" == tmp)
			ioSep = eTitleSeparatorSpace;
		else if (L"h" == tmp)
			ioSep = eTitleSeparatorHyphen;
		else
		{
			assert(0);
			found = ElementNode::eInvalidValue;
		}
	}

	if (inVersion < ARBVersion(14, 0))
	{
		if (eTitleStyleRoman == inStyle)
		{
			ioSep = eTitleSeparatorHyphen;
		}
	}
	return found;
}


void SaveTitleSeparator(
		ElementNodePtr ioTree,
		wchar_t const* inAttrib,
		ARBTitleSeparator inSep)
{
	std::wstring sep;
	switch (inSep)
	{
	default:
		assert(0);
		break;
	case eTitleSeparatorNone:
		// default
		break;
	case eTitleSeparatorSpace:
		sep = L"s";
		break;
	case eTitleSeparatorHyphen:
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
	default:
		break;
	case eTitleSeparatorSpace:
		sep = L" ";
		break;
	case eTitleSeparatorHyphen:
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
	std::wostringstream str;
	if (bShowInstanceOne || 1 < instance)
	{
		short value = startAt + (instance - 1) * increment;
		switch (style)
		{
		default:
			assert(0);
			// fall thru
		case eTitleStyleNone:
			break;
		case eTitleStyleNumber:
			str << GetSeparator(sep) << value;
			break;
		case eTitleStyleRoman:
			str << GetSeparator(sep) << ShortToRoman(value);
			break;
		}
	}
	return str.str();
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
	class ARBConfigTitle_concrete : public ARBConfigTitle
	{
	public:
		ARBConfigTitle_concrete() {}
		ARBConfigTitle_concrete(ARBConfigTitle const& rhs)
			: ARBConfigTitle(rhs)
		{
		}
	};
};


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
	, m_MultipleStyle(eTitleStyleNumber)
	, m_MultipleSeparator(eTitleSeparatorNone)
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


bool ARBConfigTitle::operator==(ARBConfigTitle const& rhs) const
{
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
	m_MultipleStyle = eTitleStyleNumber;
	m_MultipleSeparator = eTitleSeparatorNone;
}


bool ARBConfigTitle::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_TITLES)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_TITLES_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLES_NAME));
		return false;
	}

	inTree->GetAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	m_Desc = inTree->GetValue();

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_PREFIX, m_Prefix))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_PREFIX, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDFROM, msg.c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo))
	{
		std::wstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, attrib);
		std::wstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDTO, msg.c_str()));
		return false;
	}

	if (inVersion < ARBVersion(14, 0))
	{
		short multiple;
		if (ElementNode::eFound == inTree->GetAttrib(L"Multiple", multiple))
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


bool ARBConfigTitle::Save(ElementNodePtr ioTree) const
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
	std::wostringstream buffer;
	buffer << m_Name << TitleInstance(0 > inInstance ? false : m_MultipleOnFirst, inInstance, m_MultipleStartAt, m_MultipleIncrement, m_MultipleStyle, m_MultipleSeparator);
	return buffer.str();
}


std::wstring ARBConfigTitle::GetCompleteName(
		short inInstance,
		bool bAbbrevFirst,
		bool bAddDates) const
{
	std::wostringstream buffer;
	buffer << TitleInstance(0 > inInstance ? false : m_MultipleOnFirst, inInstance, m_MultipleStartAt, m_MultipleIncrement, m_MultipleStyle, m_MultipleSeparator);
	// Special formatting used in configuration dialogs.
	if (0 > inInstance && 0 < m_MultipleStartAt)
	{
		buffer << L"+";
	}
	std::wostringstream name;
	if (0 < m_LongName.length())
	{
		if (bAbbrevFirst)
		{
			name << L"[" << m_Name
				<< buffer.str()
				<< L"] ";
		}
		name << m_LongName;
		if (!bAbbrevFirst)
		{
			name << L" [" << m_Name
				<< buffer.str()
				<< L"]";
		}
	}
	else
	{
		name << m_Name
			<< buffer.str();
	}
	if (bAddDates)
	{
		std::wstring dates = ARBDate::GetValidDateString(m_ValidFrom, m_ValidTo);
		if (!dates.empty())
			name << L" " << dates;
	}
	return name.str();
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitleList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback,
		bool inCheckDups)
{
	ARBConfigTitlePtr thing(ARBConfigTitle::New());
	if (!thing->Load(inTree, inVersion, ioCallback)
	|| (inCheckDups && FindTitle(thing->GetName())))
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
		for (ARBConfigTitleList::const_iterator i = inList.begin();
			i != inList.end();
			++i)
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


bool ARBConfigTitleList::FindTitle(
		std::wstring const& inName,
		ARBConfigTitlePtr* outTitle) const
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


bool ARBConfigTitleList::AddTitle(
		std::wstring const& inName,
		ARBConfigTitlePtr* outTitle)
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


bool ARBConfigTitleList::AddTitle(ARBConfigTitlePtr inTitle)
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
