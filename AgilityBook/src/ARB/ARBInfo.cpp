/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-07 DRC Created
 */

#include "StdAfx.h"
#include "ARBInfo.h"

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfo::ARBInfo()
	: m_JudgeInfo()
{
}

ARBInfo::ARBInfo(ARBInfo const& rhs)
	: m_JudgeInfo(rhs.m_JudgeInfo)
{
}

ARBInfo::~ARBInfo()
{
	clear();
}

ARBInfo& ARBInfo::operator=(ARBInfo const& rhs)
{
	if (this != &rhs)
	{
		m_JudgeInfo = rhs.m_JudgeInfo;
	}
	return *this;
}

bool ARBInfo::operator==(ARBInfo const& rhs) const
{
	return m_JudgeInfo == rhs.m_JudgeInfo;
}

bool ARBInfo::operator!=(ARBInfo const& rhs) const
{
	return !operator==(rhs);
}

void ARBInfo::clear()
{
	m_JudgeInfo.clear();
}

bool ARBInfo::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	for (int i = 0; i < inTree.GetElementCount(); ++i)
	{
		Element const& element = inTree.GetElement(i);
		std::string const& name = element.GetName();
		if (name == TREE_JUDGEINFO)
		{
			// Ignore any errors.
			m_JudgeInfo.Load(element, inVersion, ioCallback);
		}
	}
	return true;
}

bool ARBInfo::Save(Element& ioTree) const
{
	if (0 < m_JudgeInfo.size())
	{
		Element& info = ioTree.AddElement(TREE_INFO);
		if (!m_JudgeInfo.Save(info))
			return false;
	}
	return true;
}
