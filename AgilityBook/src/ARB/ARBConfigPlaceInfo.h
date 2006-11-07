#pragma once

/*
 * Copyright © 2006 David Connet. All Rights Reserved.
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
 * @brief ARBConfigPlaceInfo and ARBConfigPlaceInfoList class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-11-03 DRC Created
 */

#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * This class is used for 2 purposes:
 * - Speed point multiplier (AKC MACH)
 * - Points (Sweepstakes champion points)
 */
class ARBConfigPlaceInfo : public ARBBase
{
protected:
	ARBConfigPlaceInfo();
	ARBConfigPlaceInfo(short inPlace, double inValue);
	ARBConfigPlaceInfo(ARBConfigPlaceInfo const& rhs);

public:
	~ARBConfigPlaceInfo();
	static ARBConfigPlaceInfoPtr New()
	{
		return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo());
	}
	static ARBConfigPlaceInfoPtr New(short inPlace, double inValue)
	{
		return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo(inPlace, inValue));
	}
	ARBConfigPlaceInfoPtr Clone() const
	{
		return ARBConfigPlaceInfoPtr(new ARBConfigPlaceInfo(*this));
	}

	ARBConfigPlaceInfo& operator=(ARBConfigPlaceInfo const& rhs);

	bool operator==(ARBConfigPlaceInfo const& rhs) const;
	bool operator!=(ARBConfigPlaceInfo const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Load a point info configuration.
	 * @pre inTree is the actual ARBConfigPlaceInfo element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigPlaceInfo element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Setters/getters.
	 */
	short GetPlace() const
	{
		return m_Place;
	}
	double GetValue() const
	{
		return m_Value;
	}
	void SetValue(double inValue)
	{
		m_Value = inValue;
	}
	// There is no SetPlace since this needs to be a unique key in the list.

private:
	short m_Place;
	double m_Value;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigPlaceInfo objects.
 */
class ARBConfigPlaceInfoList : public ARBVector<ARBConfigPlaceInfoPtr>
{
public:
	/**
	 * Load a place info configuration.
	 * @pre inTree is the actual ARBConfigPlaceInfo element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the point objects by place.
	 */
	void sort();

	/**
	 * Get the value based on place.
	 * @param inPlace Placement.
	 * @param outValue Value of placement.
	 * @return inPlace has a value.`
	 */
	bool GetPlaceInfo(short inPlace, double &outValue) const;

	/**
	 * Find a points object.
	 * @param inPlace Placement to find.
	 * @param outPoints Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindPlaceInfo(
			short inPlace,
			ARBConfigPlaceInfoPtr* outPlace = NULL) const;

	/**
	 * Add an object.
	 * @param inPlace Placement.
	 * @param inValue Value.
	 * @param outPlace Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddPlaceInfo(
			short inPlace,
			double inValue,
			ARBConfigPlaceInfoPtr* outPlace = NULL);

	/**
	 * Delete an object.
	 * @param inPlace Delete object with the given placement.
	 * @return Whether object was deleted.
	 */
	bool DeletePlaceInfo(short inPlace);
};
