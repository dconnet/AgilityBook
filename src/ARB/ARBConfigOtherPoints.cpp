/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "StdAfx.h"
#include "ARBConfigOtherPoints.h"

#include "ARBAgilityRecordBook.h"
#include "ARBLocalization.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBConfigOtherPointsPtr ARBConfigOtherPoints::New()
{
	return ARBConfigOtherPointsPtr(new ARBConfigOtherPoints());
}


ARBConfigOtherPoints::ARBConfigOtherPoints()
	: m_Name()
	, m_Tally(ARBConfigOtherPoints::eTallyAll)
	, m_Desc()
	, m_Default(0)
{
}


ARBConfigOtherPoints::ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs)
	: m_Name(rhs.m_Name)
	, m_Tally(rhs.m_Tally)
	, m_Desc(rhs.m_Desc)
	, m_Default(rhs.m_Default)
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
		m_Default = rhs.m_Default;
	}
	return *this;
}


bool ARBConfigOtherPoints::operator==(ARBConfigOtherPoints const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Tally == rhs.m_Tally
		&& m_Desc == rhs.m_Desc
		&& m_Default == rhs.m_Default;
}


void ARBConfigOtherPoints::clear()
{
	m_Name.erase();
	m_Tally = ARBConfigOtherPoints::eTallyAll;
	m_Desc.erase();
	m_Default = 0;
}


bool ARBConfigOtherPoints::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ASSERT(inTree);
	if (!inTree)
		return false;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_OTHERPTS_NAME, m_Name)
	|| 0 == m_Name.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_NAME));
		return false;
	}
	inTree->GetAttrib(ATTRIB_OTHERPTS_DEFAULT, m_Default);
	tstring attrib;
	if (ElementNode::eFound != inTree->GetAttrib(ATTRIB_OTHERPTS_COUNT, attrib)
	|| 0 == attrib.length())
	{
		ioCallback.LogMessage(Localization()->ErrorMissingAttribute(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT));
		return false;
	}
	if (attrib == OTHERPTS_COUNT_ALL)
		m_Tally = eTallyAll;
	else if (attrib == OTHERPTS_COUNT_ALLBYEVENT)
		m_Tally = eTallyAllByEvent;
	else if (attrib == OTHERPTS_COUNT_LEVEL)
		m_Tally = eTallyLevel;
	else if (attrib == OTHERPTS_COUNT_LEVELBYEVENT)
		m_Tally = eTallyLevelByEvent;
	else
	{
		tstring msg(Localization()->InvalidValue());
		msg += attrib;
		msg += _T("\n");
		msg += Localization()->ValidValuesOtherPt();
		ioCallback.LogMessage(Localization()->ErrorInvalidAttributeValue(TREE_OTHERPTS, ATTRIB_OTHERPTS_COUNT, msg.c_str()));
		return false;
	}
	m_Desc = inTree->GetValue();
	return true;
}


bool ARBConfigOtherPoints::Save(ElementNodePtr ioTree) const
{
	ASSERT(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr other = ioTree->AddElementNode(TREE_OTHERPTS);
	other->AddAttrib(ATTRIB_OTHERPTS_NAME, m_Name);
	other->AddAttrib(ATTRIB_OTHERPTS_DEFAULT, m_Default);
	switch (m_Tally)
	{
	default:
	case eTallyAll:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_ALL);
		break;
	case eTallyAllByEvent:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_ALLBYEVENT);
		break;
	case eTallyLevel:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_LEVEL);
		break;
	case eTallyLevelByEvent:
		other->AddAttrib(ATTRIB_OTHERPTS_COUNT, OTHERPTS_COUNT_LEVELBYEVENT);
		break;
	}
	if (0 < m_Desc.length())
		other->SetValue(m_Desc);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

bool ARBConfigOtherPointsList::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	ARBConfigOtherPointsPtr thing(ARBConfigOtherPoints::New());
	if (!thing->Load(inTree, inVersion, ioCallback))
		return false;
	push_back(thing);
	return true;
}


bool ARBConfigOtherPointsList::VerifyOtherPoints(tstring const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		if ((*iter)->GetName() == inName)
			return true;
	}
	return false;
}


bool ARBConfigOtherPointsList::FindOtherPoints(
		tstring const& inName,
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


bool ARBConfigOtherPointsList::DeleteOtherPoints(tstring const& inName)
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
