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
 * @brief ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-11-22 DRC Added FindRegNum().
 */

#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * Registration number for a venue.
 */
class ARBDogRegNum : public ARBBase
{
protected:
	ARBDogRegNum();
	ARBDogRegNum(ARBDogRegNum const& rhs);

public:
	~ARBDogRegNum();
	static ARBDogRegNumPtr New();
	ARBDogRegNumPtr Clone() const;

	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);

	bool operator==(ARBDogRegNum const& rhs) const;
	bool operator!=(ARBDogRegNum const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		return GetVenue() + _T(" ") + GetNumber();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogRegNum element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

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
	ARBString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(ARBString const& inVenue)
	{
		m_Venue = inVenue;
	}
	ARBString const& GetNumber() const
	{
		return m_Number;
	}
	void SetNumber(ARBString const& inNumber)
	{
		m_Number = inNumber;
	}
	ARBString const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(ARBString const& inHeight)
	{
		m_Height = inHeight;
	}
	bool GetReceived() const
	{
		return m_bReceived;
	}
	void SetReceived(bool inReceived)
	{
		m_bReceived = inReceived;
	}
	ARBString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(ARBString const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBString m_Venue;
	ARBString m_Number;
	ARBString m_Height;
	bool m_bReceived;
	ARBString m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRegNum objects.
 */
class ARBDogRegNumList : public ARBVector<ARBDogRegNumPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort registration numbers by name.
	 */
	void sort();

	/**
	 * Number of registration numbers for a venue.
	 * @param inVenue Venue to tally.
	 * @return Number of registration numbers found.
	 */
	int NumRegNumsInVenue(ARBString const& inVenue) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
			ARBString const& inOldVenue,
			ARBString const& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(ARBString const& inVenue);

	/**
	 * Find a registration number.
	 * @param inVenue Venue to find a number for.
	 * @param outRegNum Pointer to found object.
	 * @return Whether the object was found.
	 */
	bool FindRegNum(
			ARBString const& inVenue,
			ARBDogRegNumPtr* outRegNum = NULL) const;

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number to add.
	 * @param outRegNum Pointer to new object.
	 * @return Whether the object was added.
	 */
	bool AddRegNum(
			ARBString const& inVenue,
			ARBString const& inNumber,
			ARBDogRegNumPtr* outRegNum = NULL);

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inRegNum Registration number object
	 * @return Whether the object was added.
	 */
	bool AddRegNum(ARBDogRegNumPtr inRegNum);

	/**
	 * Delete a registration number.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number.
	 * @return Number of objects deleted.
	 */
	int DeleteRegNum(
			ARBString const& inVenue,
			ARBString const& inNumber);
};
