#pragma once

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
 * @brief ARBBase class
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-05-20 DRC Added different types of filtering.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 */

#include <set>
#include "ARBTypes.h"

/**
 * @brief Base class for common functionality.
 */
class ARBBase
{
public:
	ARBBase();
	virtual ~ARBBase();

	/**
	 * Get the generic name of an object.
	 * This allows us to display a list of ARBBase objects to the user
	 * without knowing what the objects actually are.
	 * @return Name of the object.
	 */
	virtual ARBString GetGenericName() const = 0;

	/**
	 * Get all the strings to search in an object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const = 0;

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
	virtual bool IsFiltered(FilterType inFilterType) const
	{
		return m_bFiltered[inFilterType];
	}

	/**
	 * Set the filtered state of this object.
	 * This attribute is not persistent. It is up to the UI to manage this.
	 * @param inFilterType The filtered type to set.
	 * @param bFiltered Filtered state of the object.
	 */
	virtual void SetFiltered(
			FilterType inFilterType,
			bool bFiltered)
	{
		m_bFiltered[inFilterType] = bFiltered;
	}

	/**
	 * Get the filtered state of this object.
	 * @return The filtered state.
	 */
	virtual bool IsFiltered() const
	{
		return IsFiltered(eFilter);
	}

	/**
	 * Set the filtered state of this object.
	 * This attribute is not persistent. It is up to the UI to manage this.
	 * @param bFiltered Filtered state of the object, set all types.
	 */
	virtual void SetFiltered(bool bFiltered)
	{
		SetFiltered(eFilter, bFiltered);
		SetFiltered(eIgnoreQ, bFiltered);
	}

protected:
	bool m_bFiltered[eNumFilters];
};
typedef boost::shared_ptr<ARBBase> ARBBasePtr;
