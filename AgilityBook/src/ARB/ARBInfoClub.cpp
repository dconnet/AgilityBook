/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief Comments about clubs
 * @author David Connet
 *
 * Revision History
 * @li 2004-11-18 DRC Created
 */

#include "StdAfx.h"
#include "ARBInfoClub.h"
#include <algorithm>

#include "ARBAgilityRecordBook.h"
#include "Element.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

ARBInfoClub::ARBInfoClub()
	: m_Name()
	, m_Comment()
{
}

ARBInfoClub::ARBInfoClub(ARBInfoClub const& rhs)
	: m_Name(rhs.m_Name)
	, m_Comment(rhs.m_Comment)
{
}

ARBInfoClub::~ARBInfoClub()
{
}

ARBInfoClub& ARBInfoClub::operator=(ARBInfoClub const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Comment = rhs.m_Comment;
	}
	return *this;
}

bool ARBInfoClub::operator==(ARBInfoClub const& rhs) const
{
	return m_Name == rhs.m_Name
		&& m_Comment == rhs.m_Comment;
}

bool ARBInfoClub::operator!=(ARBInfoClub const& rhs) const
{
	return !operator==(rhs);
}

std::string ARBInfoClub::GetGenericName() const
{
	return m_Name;
}

size_t ARBInfoClub::GetSearchStrings(std::set<std::string>& ioStrings) const
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

bool ARBInfoClub::Load(
	Element const& inTree,
	ARBVersion const& inVersion,
	ARBErrorCallback& ioCallback)
{
	if (Element::eNotFound == inTree.GetAttrib(ATTRIB_CLUBINFO_NAME, m_Name))
	{
		ioCallback.LogMessage(ErrorMissingAttribute(TREE_CLUBINFO, ATTRIB_CLUBINFO_NAME));
		return false;
	}
	m_Comment = inTree.GetValue();
	return true;
}

bool ARBInfoClub::Save(Element& ioTree) const
{
	Element& info = ioTree.AddElement(TREE_CLUBINFO);
	info.AddAttrib(ATTRIB_CLUBINFO_NAME, m_Name);
	info.SetValue(m_Comment);
	return true;
}

/////////////////////////////////////////////////////////////////////////////

class SortInfoClub
{
public:
	SortInfoClub(bool bDescending) : m_bDescending(bDescending) {}
	bool operator()(ARBInfoClub* one, ARBInfoClub* two) const
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

void ARBInfoClubList::sort(bool inDescending)
{
	if (2 > size())
		return;
	std::stable_sort(begin(), end(), SortInfoClub(inDescending));
}

size_t ARBInfoClubList::GetAllClubs(std::set<std::string>& outNames) const
{
	outNames.clear();
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoClub const* info = *iter;
		outNames.insert(info->GetName());
	}
	return outNames.size();
}

void ARBInfoClubList::CondenseContent(std::set<std::string> const& inNamesInUse)
{
	// Remove any entries that have empty comments for clubs that we have
	// shown under. This is simply to keep the file size down.
	for (iterator iter = begin(); iter != end(); )
	{
		ARBInfoClub const* club = *iter;
		if (0 == club->GetComment().length())
		{
			if (inNamesInUse.end() == inNamesInUse.find(club->GetName()))
				++iter;
			else
				iter = erase(iter);
		}
		else
			++iter;
	}
}

ARBInfoClub* ARBInfoClubList::FindClub(std::string const& inName) const
{
	for (const_iterator iter = begin(); iter != end(); ++iter)
	{
		ARBInfoClub* info = *iter;
		if (info->GetName() == inName)
			return info;
	}
	return NULL;
}

ARBInfoClub* ARBInfoClubList::AddClub(std::string const& inClub)
{
	if (NULL != FindClub(inClub))
		return NULL;
	ARBInfoClub* club = NULL;
	if (0 < inClub.length())
	{
		club = new ARBInfoClub();
		club->SetName(inClub);
		push_back(club);
	}
	return club;
}

ARBInfoClub* ARBInfoClubList::AddClub(ARBInfoClub* inClub)
{
	if (!inClub)
		return NULL;
	if (FindClub(inClub->GetName()))
		return NULL;
	inClub->AddRef();
	push_back(inClub);
	return inClub;
}

bool ARBInfoClubList::DeleteClub(ARBInfoClub const* inClub)
{
	if (inClub)
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			if ((*iter) && *(*iter) == *inClub)
			{
				erase(iter);
				return true;
			}
		}
	}
	return false;
}
