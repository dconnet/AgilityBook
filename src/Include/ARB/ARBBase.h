#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBBase class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-15 Added IsAnyFiltered.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-05-20 Added different types of filtering.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-08-24 Optimized filtering by adding boolean into ARBBase to
 *            prevent constant re-evaluation.
 */

#include "ARB/ARBTypes2.h"
#include <set>


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
	virtual std::wstring GetGenericName() const = 0;

	/**
	 * Get all the strings to search in an object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const = 0;

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
	 * Get any filtered state of this object.
	 * @return The filtered state.
	 */
	virtual bool IsAnyFiltered() const
	{
		return IsFiltered(eFilter) || IsFiltered(eIgnoreQ);
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

	ARBBase(ARBBase const&) = delete;
	ARBBase(ARBBase&&) = delete;
	ARBBase& operator=(ARBBase const&) = delete;
	ARBBase& operator=(ARBBase&&) = delete;
};
typedef std::shared_ptr<ARBBase> ARBBasePtr;
