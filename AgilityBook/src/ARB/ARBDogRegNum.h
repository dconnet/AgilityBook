#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-11-22 DRC Added FindRegNum().
 */

#include <string>
#include "ARBBase.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigVenue;
class ARBVersion;
class Element;

/**
 * Registration number for a venue.
 */
class ARBDogRegNum : public ARBBase
{
public:
	ARBDogRegNum();
	ARBDogRegNum(const ARBDogRegNum& rhs);
	ARBDogRegNum& operator=(const ARBDogRegNum& rhs);
	bool operator==(const ARBDogRegNum& rhs) const;
	bool operator!=(const ARBDogRegNum& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::string GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogRegNum element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const ARBConfig& inConfig,
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRegNum element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);
	const std::string& GetNumber() const;
	void SetNumber(const std::string& inNumber);
	const std::string& GetHeight() const;
	void SetHeight(const std::string& inHeight);
	bool GetReceived() const;
	void SetReceived(bool inReceived);

private:
	~ARBDogRegNum();
	std::string m_Venue;
	std::string m_Number;
	std::string m_Height;
	bool m_bReceived;
};

inline const std::string& ARBDogRegNum::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogRegNum::SetVenue(const std::string& inVenue)
{
	m_Venue = inVenue;
}

inline const std::string& ARBDogRegNum::GetNumber() const
{
	return m_Number;
}

inline void ARBDogRegNum::SetNumber(const std::string& inNumber)
{
	m_Number = inNumber;
}

inline const std::string& ARBDogRegNum::GetHeight() const
{
	return m_Height;
}

inline void ARBDogRegNum::SetHeight(const std::string& inHeight)
{
	m_Height = inHeight;
}

inline bool ARBDogRegNum::GetReceived() const
{
	return m_bReceived;
}

inline void ARBDogRegNum::SetReceived(bool inReceived)
{
	m_bReceived = inReceived;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRegNum objects.
 */
class ARBDogRegNumList : public ARBVectorLoad2<ARBDogRegNum>
{
public:
	bool operator==(const ARBDogRegNumList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogRegNumList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending = true);

	/**
	 * Number of registration numbers for a venue.
	 * @param inVenue Venue to tally.
	 * @return Number of registration numbers found.
	 */
	int NumRegNumsInVenue(const std::string& inVenue) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(const std::string& inVenue);

	/**
	 * Find a registration number.
	 * @param inVenue Venue to find a number for.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBDogRegNum* FindRegNum(const std::string& inVenue) const;

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBDogRegNum* AddRegNum(
		const std::string& inVenue,
		const std::string& inNumber);

	/**
	 * Delete a registration number.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number.
	 * @return Number of objects deleted.
	 */
	int DeleteRegNum(
		const std::string& inVenue,
		const std::string& inNumber);
};
