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
 * @brief Comments about judges
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include "StdAfx.h"
#include "ARBInfoJudge.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfoJudge::ARBInfoJudge()
	: m_Name()
	, m_Comment()
{
}

ARBInfoJudge::ARBInfoJudge(const ARBInfoJudge& rhs)
	: m_Name(rhs.m_Name)
	, m_Comment(rhs.m_Comment)
{
}

ARBInfoJudge::~ARBInfoJudge()
{
}

ARBInfoJudge& ARBInfoJudge::operator=(const ARBInfoJudge& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Comment = rhs.m_Comment;
	}
	return *this;
}

bool ARBInfoJudge::operator==(const ARBInfoJudge& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Comment == rhs.m_Comment;
}

bool ARBInfoJudge::operator!=(const ARBInfoJudge& rhs) const
{
	return !operator==(rhs);
}

std::string ARBInfoJudge::GetGenericName() const
{
	return m_Name;
}

size_t ARBInfoJudge::GetSearchStrings(std::set<std::string>& ioStrings) const
{
	size_t nItems = 0;

	ioStrings.insert(m_Name);
	++nItems;

	if (0 < m_Comment.length())
	{
		ioStrings.insert(m_Comment);
		++nItems;
	}

	return nItems;
}

bool ARBInfoJudge::Load(
	const CElement& inTree,
	const ARBVersion& inVersion,
	std::string& ioErrMsg)
{
	if (CElement::eNotFound == inTree.GetAttrib(ATTRIB_JUDGEINFO_NAME, m_Name))
	{
		ioErrMsg += ErrorMissingAttribute(TREE_JUDGEINFO, ATTRIB_JUDGEINFO_NAME);
		return false;
	}
	m_Comment = inTree.GetValue();
	return true;
}

bool ARBInfoJudge::Save(CElement& ioTree) const
{
	CElement& info = ioTree.AddElement(TREE_JUDGEINFO);
	info.AddAttrib(ATTRIB_JUDGEINFO_NAME, m_Name);
	info.SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortInfoJudge
{
public:
	SortInfoJudge(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBInfoJudge* one, ARBInfoJudge* two) const
	{
		if (one->GetName() < two->GetName())
			return m_bDescending;
		else if (one->GetName() > two->GetName())
			return !m_bDescending;
		else
			return !m_bDescending;
	}
private:
	bool m_bDescending;
};

void ARBInfoJudgeList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortInfoJudge(inDescending));
}

size_t ARBInfoJudgeList::GetAllJudges(std::set<std::string>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		const ARBInfoJudge* info = *iter;
		outNames.insert(info->GetName());
	}
	return outNames.size();
}

void ARBInfoJudgeList::CondenseContent(const std::set<std::string>& inNamesInUse)
{
	// Remove any entries that have empty comments for judges that we have
	// shown under. This is simply to keep the file size down.
	for (iterator iter = begin(); iter != end(); )
	{
		const ARBInfoJudge* judge = *iter;
		if (0 == judge->GetComment().length())
		{
			if (inNamesInUse.end() == inNamesInUse.find(judge->GetName()))
				++iter;
			else
				iter = erase(iter);
		}
		else
			++iter;
	}
}

ARBInfoJudge* ARBInfoJudgeList::FindJudge(const std::string& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoJudge* info = *iter;
		if (info->GetName() == inName)
			return info;
	}
	return NULL;
}

ARBInfoJudge* ARBInfoJudgeList::AddJudge(const std::string& inJudge)
{
	if (NULL != FindJudge(inJudge))
		return NULL;
	ARBInfoJudge* judge = NULL;
	if (0 < inJudge.length())
	{
		judge = new ARBInfoJudge();
		judge->SetName(inJudge);
		push_back(judge);
	}
	return judge;
}

bool ARBInfoJudgeList::DeleteJudge(const ARBInfoJudge* inJudge)
{
	if (inJudge)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inJudge)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
