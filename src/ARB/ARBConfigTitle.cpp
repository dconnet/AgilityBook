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
 * @brief The classes that make up the configuration information.
 * @author David Connet
 *
 * Revision History
 * @li 2005-10-14 DRC Added option to prefix a title.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-05 DRC Added LongName.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBConfigTitle.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigTitle::ARBConfigTitle()
	: m_Name()
	, m_LongName()
	, m_Multiple(0)
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
	, m_Prefix(rhs.m_Prefix)
	, m_ValidFrom(rhs.m_ValidFrom)
	, m_ValidTo(rhs.m_ValidTo)
	, m_Desc(rhs.m_Desc)
{
}

ARBConfigTitle::~ARBConfigTitle()
{
}

//static
ARBConfigTitlePtr ARBConfigTitle::New()
{
	return ARBConfigTitlePtr(new ARBConfigTitle());
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
		&& m_Prefix == rhs.m_Prefix
		&& m_ValidFrom == rhs.m_ValidFrom
		&& m_ValidTo == rhs.m_ValidTo
		&& m_Desc == rhs.m_Desc;
}

bool ARBConfigTitle::operator!=(ARBConfigTitle const& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigTitle::clear()
{
	m_Name.erase();
	m_LongName.erase();
	m_Multiple = 0;
	m_Prefix = false;
	m_ValidFrom.clear();
	m_ValidTo.clear();
	m_Desc.erase();
}

size_t ARBConfigTitle::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigTitle::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_TITLES_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_TITLES, ATTRIB_TITLES_NAME));
		return false;
	}

	inTree.GetAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	inTree.GetAttrib(ATTRIB_TITLES_MULTIPLE, m_Multiple);
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLES_PREFIX, m_Prefix))
	{
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_PREFIX, VALID_VALUES_BOOL));
		return false;
	}

	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom))
	{
		ARBString attrib;
		inTree.GetAttrib(ATTRIB_TITLES_VALIDFROM, attrib);
		ARBString msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDFROM, msg.c_str()));
		return false;
	}
	if (Element::eInvalidValue == inTree.GetAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo))
	{
		ARBString attrib;
		inTree.GetAttrib(ATTRIB_TITLES_VALIDTO, attrib);
		ARBString msg(INVALID_DATE);
		msg += attrib;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_TITLES, ATTRIB_TITLES_VALIDTO, msg.c_str()));
		return false;
	}

	m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigTitle::Save(Element& ioTree) const
{
	Element& title = ioTree.AddElement(TREE_TITLES);
	title.AddAttrib(ATTRIB_TITLES_NAME, m_Name);
	if (0 < m_Multiple)
		title.AddAttrib(ATTRIB_TITLES_MULTIPLE, m_Multiple);
	if (m_Prefix)
		title.AddAttrib(ATTRIB_TITLES_PREFIX, m_Prefix);
	if (0 < m_LongName.length())
		title.AddAttrib(ATTRIB_TITLES_LONGNAME, m_LongName);
	if (m_ValidFrom.IsValid())
		title.AddAttrib(ATTRIB_TITLES_VALIDFROM, m_ValidFrom);
	if (m_ValidTo.IsValid())
		title.AddAttrib(ATTRIB_TITLES_VALIDTO, m_ValidTo);
	if (0 < m_Desc.length())
		title.SetValue(m_Desc);
	return true;
}

bool ARBConfigTitle::IsValidOn(ARBDate inDate) const
{
	if (inDate.IsValid()
	&& ((m_ValidFrom.IsValid() && inDate < m_ValidFrom)
	|| (m_ValidTo.IsValid() && inDate > m_ValidTo)))
	{
		return false;
	}
	return true;
}

ARBString ARBConfigTitle::GetCompleteName(
		short inInstance,
		bool bAbbrevFirst,
		bool bAddDates) const
{
	ARBString buffer;
	if (1 < inInstance)
	{
		// Keep sync'd with ARBDogTitle
		ARBostringstream str;
		str << _T(" ") << inInstance;
		buffer = str.str();
	}
	// Special formatting used in configuration dialogs.
	else if (0 > inInstance && 0 < m_Multiple)
	{
		ARBostringstream str;
		str << _T("+");
		buffer = str.str();
	}
	ARBostringstream name;
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
		ARBString dates = ARBDate::GetValidDateString(m_ValidFrom, m_ValidTo);
		if (!dates.empty())
			name << _T(" ") << dates;
	}
	return name.str();
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigTitleList::Load(
		Element const& inTree,
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

bool ARBConfigTitleList::FindTitleCompleteName(
		ARBString const& inName,
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
		ARBString const& inName,
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
		ARBString const& inName,
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

bool ARBConfigTitleList::DeleteTitle(ARBString const& inName)
{
	ARBString name(inName);
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
