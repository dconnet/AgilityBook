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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
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

ARBConfigOtherPoints::ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs)
	: m_Name(rhs.m_Name)
	, m_Tally(rhs.m_Tally)
	, m_Desc(rhs.m_Desc)
{
}

ARBConfigOtherPoints::~ARBConfigOtherPoints()
{
}

ARBConfigOtherPointsPtr ARBConfigOtherPoints::Clone() const
{
	return ARBConfigOtherPointsPtr(new ARBConfigOtherPoints(*this));
}

ARBConfigOtherPoints& ARBConfigOtherPoints::operator=(ARBConfigOtherPoints const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Tally = rhs.m_Tally;
		m_Desc = rhs.m_Desc;
	}
	return *this;
}

bool ARBConfigOtherPoints::operator==(ARBConfigOtherPoints const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Tally == rhs.m_Tally
		&& m_Desc == rhs.m_Desc;
}

bool ARBConfigOtherPoints::operator!=(ARBConfigOtherPoints const& rhs) const
{
	return !operator==(rhs);
}

void ARBConfigOtherPoints::clear()
{
	m_Name.erase();
	m_Tally = ARBConfigOtherPoints::eTallyAll;
	m_Desc.erase();
}

size_t ARBConfigOtherPoints::GetSearchStrings(std::set<ARBString>& ioStrings) const
{
	size_t nItems = 0;
	return nItems;
}

bool ARBConfigOtherPoints::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	if (Element::eFound != inTree.GetAttrib(ATTRIB_OTHERPTS_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_NAME));
		return false;
	}
	ARBString attrib;
	if (Element::eFound != inTree.GetAttrib(ATTRIB_OTHERPTS_COUNT, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT));
		return false;
	}
	if (attrib == _T("All"))
		m_Tally = eTallyAll;
	else if (attrib == _T("AllByEvent"))
		m_Tally = eTallyAllByEvent;
	else if (attrib == _T("Level"))
		m_Tally = eTallyLevel;
	else if (attrib == _T("LevelByEvent"))
		m_Tally = eTallyLevelByEvent;
	else
	{
		ARBString msg(INVALID_VALUE);
		msg += attrib;
		msg += _T("\n");
		msg += VALID_VALUES_OTHERPT;
		ioCallback.LogMessage(ErrorInvalidAttributeValue(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT, msg.c_str()));
		return false;
	}
	m_Desc = inTree.GetValue();
	return true;
}

bool ARBConfigOtherPoints::Save(Element& ioTree) const
{
	Element& other = ioTree.AddElement(TREE_OTHERPTS);
	other.AddAttrib(ATTRIB_OTHERPTS_NAME, m_Name);
	switch (m_Tally)
	{
	default:
	case eTallyAll:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("All"));
		break;
	case eTallyAllByEvent:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("AllByEvent"));
		break;
	case eTallyLevel:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("Level"));
		break;
	case eTallyLevelByEvent:
		other.AddAttrib(ATTRIB_OTHERPTS_COUNT, _T("LevelByEvent"));
		break;
	}
	if (0 < m_Desc.length())
		other.SetValue(m_Desc);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigOtherPointsList::Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigOtherPointsPtr thing(new ARBConfigOtherPoints());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}

bool ARBConfigOtherPointsList::VerifyOtherPoints(ARBString const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}

bool ARBConfigOtherPointsList::FindOtherPoints(
		ARBString const& inName,
		ARBConfigOtherPointsPtr* outPoints) const
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

bool ARBConfigOtherPointsList::AddOtherPoints(ARBConfigOtherPointsPtr inOther)
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

bool ARBConfigOtherPointsList::DeleteOtherPoints(ARBString const& inName)
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
