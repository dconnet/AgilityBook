#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief ARBBase class
 * @author David Connet
 *
 * Revision History
 * @li 2004-05-20 DRC Added different types of filtering.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include <set>
#include <string>

/**
 * @brief Base class for reference counting and other common functionality.
 *
 * The idea is to remove some of the efforts of memory management.
 * Note, there are a number of APIs that return a non-reference counted pointer.
 * Ideally, everything would, but that gets things rather messy for coding.
 * I have actually played around with creating a smart pointer class to work
 * around this. In time I might complete that work - though I may just toss it
 * in favor of migrating to C#.
 */
class ARBBase
{
public:
	/**
	 * @post Reference count starts at 1.
	 */
	ARBBase();

	/**
	 * Add a reference to the object.
	 */
	void AddRef();

	/**
	 * Remove a reference.
	 * @post When the reference count goes to 0, the object will be deleted.
	 */
	void Release();

	/**
	 * Get the generic name of an object.
	 * This allows us to display a list of ARBBase objects to the user
	 * without knowing what the objects actually are.
	 * @return Name of the object.
	 */
	virtual std::string GetGenericName() const = 0;

	/**
	 * Get all the strings to search in an object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const = 0;

	// Filtered state
	typedef enum
	{
		eFilter = 0,	// Full filter
		eIgnoreQ = 1,	// Ignore Q status when filtering
		eNumFilters = 2	// Number of filter types
	} FilterType;

	/**
	 * Get the filtered state of this object.
	 * @param inFilterType The filtered type to check.
	 * @return The filtered state.
	 */
	virtual bool IsFiltered(FilterType inFilterType) const;

	/**
	 * Set the filtered state of this object.
	 * This attribute is not persistent. It is up to the UI to manage this.
	 * @param inFilterType The filtered type to set.
	 * @param bFiltered Filtered state of the object.
	 */
	virtual void SetFiltered(
			FilterType inFilterType,
			bool bFiltered);

	/**
	 * Get the filtered state of this object.
	 * @return The filtered state.
	 */
	virtual bool IsFiltered() const;

	/**
	 * Set the filtered state of this object.
	 * This attribute is not persistent. It is up to the UI to manage this.
	 * @param bFiltered Filtered state of the object, set all types.
	 */
	virtual void SetFiltered(bool bFiltered);

protected:
	/**
	 * Protect the dtor to make sure no one can delete this.
	 * @note Derived classes should not make their dtors public.
	 */
	virtual ~ARBBase();
	unsigned int m_RefCount;
	bool m_bFiltered[eNumFilters];
};

inline bool ARBBase::IsFiltered(FilterType inFilterType) const
{
	return m_bFiltered[inFilterType];
}

inline void ARBBase::SetFiltered(
		FilterType inFilterType,
		bool bFiltered)
{
	m_bFiltered[inFilterType] = bFiltered;
}

inline bool ARBBase::IsFiltered() const
{
	return IsFiltered(eFilter);
}

inline void ARBBase::SetFiltered(bool bFiltered)
{
	SetFiltered(eFilter, bFiltered);
	SetFiltered(eIgnoreQ, bFiltered);
}
