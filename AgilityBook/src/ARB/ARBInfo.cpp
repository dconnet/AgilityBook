/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief General information to be tracked.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-07 DRC Created
 */

#include "stdafx.h"
#include "ARBInfo.h"

#include "ARBAgilityRecordBook.h"

#include "ARBCommon/Element.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfo::ARBInfo()
	: m_ClubInfo(TREE_CLUBINFO)
	, m_JudgeInfo(TREE_JUDGEINFO)
	, m_LocationInfo(TREE_LOCATIONINFO)
{
}


ARBInfo::ARBInfo(ARBInfo const& rhs)
	: m_ClubInfo(TREE_CLUBINFO)
	, m_JudgeInfo(TREE_JUDGEINFO)
	, m_LocationInfo(TREE_LOCATIONINFO)
{
	rhs.m_ClubInfo.Clone(m_ClubInfo);
	rhs.m_JudgeInfo.Clone(m_JudgeInfo);
	rhs.m_LocationInfo.Clone(m_LocationInfo);
}


ARBInfo::~ARBInfo()
{
	clear();
}


ARBInfo& ARBInfo::operator=(ARBInfo const& rhs)
{
	if (this != &rhs)
	{
		rhs.m_ClubInfo.Clone(m_ClubInfo);
		rhs.m_JudgeInfo.Clone(m_JudgeInfo);
		rhs.m_LocationInfo.Clone(m_LocationInfo);
	}
	return *this;
}


bool ARBInfo::operator==(ARBInfo const& rhs) const
{
	return m_ClubInfo == rhs.m_ClubInfo
		&& m_JudgeInfo == rhs.m_JudgeInfo
		&& m_LocationInfo == rhs.m_LocationInfo;
}


void ARBInfo::clear()
{
	m_ClubInfo.clear();
	m_JudgeInfo.clear();
	m_LocationInfo.clear();
}


bool ARBInfo::Load(
		ElementNodePtr inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback)
{
	assert(inTree);
	if (!inTree || inTree->GetName() != TREE_INFO)
		return false;
	for (int i = 0; i < inTree->GetElementCount(); ++i)
	{
		ElementNodePtr element = inTree->GetElementNode(i);
		if (!element)
			continue;
		std::wstring const& name = element->GetName();
		if (name == TREE_CLUBINFO)
		{
			// Ignore any errors.
			m_ClubInfo.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_JUDGEINFO)
		{
			// Ignore any errors.
			m_JudgeInfo.Load(element, inVersion, ioCallback);
		}
		else if (name == TREE_LOCATIONINFO)
		{
			// Ignore any errors.
			m_LocationInfo.Load(element, inVersion, ioCallback);
		}
	}
	return true;
}


bool ARBInfo::Save(ElementNodePtr ioTree) const
{
	assert(ioTree);
	if (!ioTree)
		return false;
	ElementNodePtr info = ioTree->AddElementNode(TREE_INFO);
	if (!m_ClubInfo.Save(info))
		return false;
	if (!m_JudgeInfo.Save(info))
		return false;
	if (!m_LocationInfo.Save(info))
		return false;
	return true;
}
