/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBConfigOtherPoints.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigOtherPoints::ARBConfigOtherPoints()
	: m_Name()
	, m_Tally(ARBConfigOtherPoints::eTallyAll)
	, m_Desc()
{
}

ARBConfigOtherPoints::ARBConfigOtherPoints(const ARBConfigOtherPoints& rhs)
	: m_Name(rhs.m_Name)
	, m_Tally(rhs.m_Tally)
	, m_Desc(rhs.m_Desc)
{
}

ARBConfigOtherPoints::~ARBConfigOtherPoints()
{
}

ARBConfigOtherPoints& ARBConfigOtherPoints::operator=(const ARBConfigOtherPoints& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Tally = rhs.m_Tally;
		m_Desc = rhs.m_Desc;
	}
	return *this;
}

bool ARBConfigOtherPoints::operator==(const ARBConfigOtherPoints& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Tally == rhs.m_Tally
		&& m_Desc == rhs.m_Desc;
}

bool ARBConfigOtherPoints::operator!=(const ARBConfigOtherPoints& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigOtherPoints::clear()
{
	m_Name.erase();
	m_Tally = ARBConfigOtherPoints::eTallyAll;
	m_Desc.erase();
}

size_t ARBConfigOtherPoints::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigOtherPoints::Load(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_OTHERPTS_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_NAME);
		return false;
	}
	std::string attrib;
	if (CElement::eFound != inTree.GetAttrib(ATTRIB_OTHERPTS_COUNT, attrib)
	|| 0 == attrib.length())
	{
		ioErrMsg += ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT);
		return false;
	}
	if (attrib == "All")
		m_Tally = eTallyAll;
	else if (attrib == "AllByEvent")
		m_Tally = eTallyAllByEvent;
	else if (attrib == "Level")
		m_Tally = eTallyLevel;
	else if (attrib == "LevelByEvent")
		m_Tally = eTallyLevelByEvent;
	else
	{
		std::string msg(INVALID_VALUE);
		msg += attrib;
		msg += "\n";
		msg += VALID_VALUES_OTHERPT;
		ioErrMsg += ErrorInvalidAttributeValue(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT, msg.c_str());
		return false;
	}
	m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigOtherPoints::Save(CElement& ioTree) const
{
	CElement& other = ioTree.AddElement(TREE_OTHERPTS);
	other.AddAttrib(ATTRIB_OTHERPTS_NAME, m_Name);
	switch (m_Tally)
	{
	default:
	case eTallyAll:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, "All");
		break;
	case eTallyAllByEvent:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, "AllByEvent");
		break;
	case eTallyLevel:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, "Level");
		break;
	case eTallyLevelByEvent:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, "LevelByEvent");
		break;
	}
	if (0 < m_Desc.length())
		other.SetValue(m_Desc);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigOtherPointsList::VerifyOtherPoints(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}

const ARBConfigOtherPoints* ARBConfigOtherPointsList::FindOtherPoints(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
		if ((*iter)->GetName() == inName)
			return (*iter);
	return NULL;
}

ARBConfigOtherPoints* ARBConfigOtherPointsList::FindOtherPoints(const std::string& inName)
{
	for (iterator iter = begin(); iter != end(); ++iter)
		if ((*iter)->GetName() == inName)
			return (*iter);
	return NULL;
}

ARBConfigOtherPoints* ARBConfigOtherPointsList::AddOtherPoints(ARBConfigOtherPoints* inOther)
{
	// Global uniqueness must be ensured by the calling function.
	if (inOther)
	{
		inOther->AddRef();
		push_back(inOther);
	}
	return inOther;
}

bool ARBConfigOtherPointsList::DeleteOtherPoints(const std::string& inName)
{
	std::string name(inName);
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
