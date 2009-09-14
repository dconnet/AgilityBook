/*
 * Copyright (c) 2003-2009 David Connet. All Rights Reserved.
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
#include "Element.h"

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
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
		wxString const& name = element->GetName();
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
