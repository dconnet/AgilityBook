/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-14 DRC Added option to prefix a title.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-05 DRC Added LongName.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "stdafx.h"
#include "ARBConfigTitle.h"

#include <algorithm>
#include <math.h>
#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif


/*
// Roman number conversion (modified from C# code on CodeProject)
static short RomanDigit(TCHAR digit)
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
static short RomanToShort(tstring number)
{
	short result = 0;
	short oldValue = 1000;
	for (tstring::const_iterator index = number.begin(); index != number.end(); ++index)
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
static tstring ShortToRoman(short value)
{
	static const TCHAR* romanDigits[9][4] =
	{
		{_T("M"),   _T("C"),    _T("X"),    _T("I")   },
		{_T("MM"),  _T("CC"),   _T("XX"),   _T("II")  },
		{_T("MMM"), _T("CCC"),  _T("XXX"),  _T("III") },
		{NULL,      _T("CD"),   _T("XL"),   _T("IV")  },
		{NULL,      _T("D"),    _T("L"),    _T("V")   },
		{NULL,      _T("DC"),   _T("LX"),   _T("VI")  },
		{NULL,      _T("DCC"),  _T("LXX"),  _T("VII") },
		{NULL,      _T("DCCC"), _T("LXXX"), _T("VIII")},
		{NULL,      _T("CM"),   _T("XC"),   _T("IX")  }
	};
	otstringstream result;
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


tstring ARBTitleInstance::TitleInstance(
		bool bShowInstanceOne,
		short instance,
		ARBTitleStyle style) const
{
	if (bShowInstanceOne || 1 < instance)
	{
		otstringstream str;
		if (eTitleRoman == style)
			str << ' ' << ShortToRoman(instance);
		else
			str << instance;
		return str.str();
	}
	return _T("");
}

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitlePtr ARBConfigTitle::New()
{
	return ARBConfigTitlePtr(new ARBConfigTitle());
}


ARBConfigTitle::ARBConfigTitle()
	: m_Name()
	, m_LongName()
	, m_Multiple(0)
	, m_MultipleStyle(eTitleNumber)
	, m_Prefix(false)
	, m_ValidFrom()
	, m_ValidTo()
	, m_Desc()
{
}


ARBConfigTitle::ARBConfigTitle(ARBConfigTitle const& rhs)
	: m_Name(rhs.m_Name)
	, m_LongName(rhs.m_LongName)
	, m_Multiple(rhs.m_Multiple)
	, m_MultipleStyle(rhs.m_MultipleStyle)
	, m_Prefix(rhs.m_Prefix)
	, m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Desc(rhs.m_Desc)
{
}


ARBConfigTitle::~ARBConfigTitle()
{
}


ARBConfigTitlePtr ARBConfigTitle::Clone() const
{
	return ARBConfigTitlePtr(new ARBConfigTitle(*this));
}


ARBConfigTitle& ARBConfigTitle::operator=(ARBConfigTitle const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_LongName = rhs.m_LongName;
		m_Multiple = rhs.m_Multiple;
		m_MultipleStyle = rhs.m_MultipleStyle;
		m_Prefix = rhs.m_Prefix;
		m_ValidFrom = rhs.m_ValidFrom;
		m_ValidTo = rhs.m_ValidTo;
		m_Desc = rhs.m_Desc;
	}
	return *this;
}


bool ARBConfigTitle::operator==(ARBConfigTitle const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_LongName == rhs.m_LongName
		&& m_Multiple == rhs.m_Multiple
		&& m_MultipleStyle == rhs.m_MultipleStyle
		&& m_Prefix == rhs.m_Prefix
		&& m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_Desc == rhs.m_Desc;
}


void ARBConfigTitle::clear()
{
	m_Name.erase();
	m_LongName.erase();
	m_Multiple = 0;
	m_MultipleStyle = eTitleNumber;
	m_Prefix = false;
	m_ValidFrom.clear();
	m_ValidTo.clear();
	m_Desc.erase();
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
	inTree->GetAttrib(ATTRIB_TITLES_MULTIPLE, m_Multiple);
	short tmp;
	if (ElementNode::eFound == inTree->GetAttrib(ATTRIB_TITLES_MULTIPLE_STYLE, tmp))
		m_MultipleStyle = static_cast<ARBTitleStyle>(tmp);
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_PREFIX, m_Prefix))
	{
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_PREFIX, Localization()->ValidValuesBool().c_str()));
		return false;
	}

	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDFROM, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDFROM, msg.c_str()));
		return false;
	}
	if (ElementNode::eInvalidValue == inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo))
	{
		tstring attrib;
		inTree->GetAttrib(ATTRIB_TITLES_VALIDTO, attrib);
		tstring msg(Localization()->InvalidDate());
		msg += attrib;
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDTO, msg.c_str()));
		return false;
	}

	m_Desc = inTree->GetValue();
	return true;
}


bool ARBConfigTitle::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr title = ioTree->AddElementNode(TREE_TITLES);
	title->AddAttrib(ATTRIB_TITLES_NAME, m_Name);
	if (0 < m_Multiple)
		title->AddAttrib(ATTRIB_TITLES_MULTIPLE, m_Multiple);
	if (eTitleNumber != m_MultipleStyle)
		title->AddAttrib(ATTRIB_TITLES_MULTIPLE_STYLE, static_cast<short>(m_MultipleStyle));
	if (m_Prefix)
		title->AddAttrib(ATTRIB_TITLES_PREFIX, m_Prefix);
	if (0 < m_LongName.length())
		title->AddAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	if (m_ValidFrom.IsValid())
		title->AddAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		title->AddAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo);
	if (0 < m_Desc.length())
		title->SetValue(m_Desc);
	return true;
}


tstring ARBConfigTitle::GetCompleteName(
		short inInstance,
		bool bShowInstance,
		bool bAbbrevFirst,
		bool bAddDates) const
{
	tstring buffer = TitleInstance(bShowInstance, inInstance, m_MultipleStyle);
	// Special formatting used in configuration dialogs.
	if (0 > inInstance && 0 < m_Multiple)
	{
		otstringstream str;
		str << _T("+");
		buffer = str.str();
	}
	otstringstream name;
	if (0 < m_LongName.length())
	{
		if (bAbbrevFirst)
		{
			name << _T("[") << m_Name;
			if (0 < buffer.length())
				name << buffer;
			name << _T("] ");
		}
		name << m_LongName;
		if (!bAbbrevFirst)
		{
			name << _T(" [") << m_Name;
			if (0 < buffer.length())
				name << buffer;
			name << _T("]");
		}
	}
	else
	{
		name << m_Name;
		if (0 < buffer.length())
			name << buffer;
	}
	if (bAddDates)
	{
		tstring dates = ARBDate::GetValidDateString(m_ValidFrom, m_ValidTo);
		if (!dates.empty())
			name << _T(" ") << dates;
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
		tstring const& inName,
		short inInstance,
		bool bShowInstance,
		bool bAbbrevFirst,
		ARBConfigTitlePtr* outTitle) const
{
	if (outTitle)
		outTitle->reset();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetCompleteName(inInstance, bShowInstance, bAbbrevFirst) == inName)
		{
			if (outTitle)
				*outTitle = *iter;
			return true;
		}
	}
	return false;
}


bool ARBConfigTitleList::FindTitle(
		tstring const& inName,
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
		tstring const& inName,
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


bool ARBConfigTitleList::DeleteTitle(tstring const& inName)
{
	tstring name(inName);
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
