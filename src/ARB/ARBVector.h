#pragma once

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
 * @brief Templates to simplify some vector operations.
 * @author David Connet
 *
 * Revision History
 * @li 2005-07-01 DRC Separated ARBVector into ref counting / other stuff.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-16 DRC Made copy ctor and operator= safer.
 * @li 2004-01-25 DRC Added Move().
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <set>
#include <vector>
#include "ARBTypes.h"
class ARBConfig;
class Element;

/**
 * Extend some common functionality.
 */
template <typename T>
class ARBVector : public std::vector<T>
{
public:
	ARBVector()
	{
	}

	/**
	 * Equality test.
	 */
	bool operator==(ARBVector<T> const& rhs) const
	{
		if (this == &rhs)
			return true;
		if (size() != rhs.size())
			return false;
		const_iterator iter1, iter2;
		for (iter1 = begin(), iter2 = rhs.begin(); iter1 != end(); ++iter1, ++iter2)
		{
			if (*(*iter1) != *(*iter2))
				return false;
		}
		return true;
	}
	bool operator!=(ARBVector<T> const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Make a copy of everything.
	 * @param outList Object being copied to.
	 */
	size_t Clone(ARBVector<T>& outList) const
	{
		outList.clear();
		outList.reserve(size());
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			T pItem = *iter;
			if (pItem)
				outList.push_back(pItem->Clone());
		}
		return outList.size();
	}

	/**
	 * Get all the strings to search in this list.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		size_t nItems = 0;
		for (const_iterator iter = begin(); iter != end(); ++iter)
			nItems += (*iter)->GetSearchStrings(ioStrings);
		return nItems;
	}

	/**
	 * Reorder a list.
	 * @param inItem Object to be moved.
	 * @param inMove Number of positions to move object.
	 * @return Whether or not object was moved.
	 */
	bool Move(
			T inItem,
			int inMove)
	{
		bool bOk = false;
		if (inItem)
		{
			int n = 0;
			for (iterator iter = begin(); iter != end(); ++iter, ++n)
			{
				if (inItem == *iter)
				{
					int offset = n + inMove;
					if (offset < 0)
						offset = 0;
					if (offset >= static_cast<int>(size()))
						offset = static_cast<int>(size()) - 1;
					if (offset != n)
					{
						bOk = true;
						std::swap(at(n), at(offset));
						break;
					}
				}
			}
		}
		return bOk;
	}

	/**
	 * Save a document.
	 * In general, the vectors themselves are never stored, only the elements.
	 * (It flattens the tree a little.)
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The T element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const
	{
		for (const_iterator iter = begin(); iter != end(); ++iter)
		{
			if (!(*iter)->Save(ioTree))
				return false;
		}
		return true;
	}
};
