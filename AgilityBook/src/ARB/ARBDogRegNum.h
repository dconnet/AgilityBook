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
 * @brief ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
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
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Registration number for a venue.
 */
class ARBDogRegNum : public ARBBase
{
public:
	ARBDogRegNum();
	ARBDogRegNum(ARBDogRegNum const& rhs);
	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);
	bool operator==(ARBDogRegNum const& rhs) const;
	bool operator!=(ARBDogRegNum const& rhs) const;

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
	std::string const& GetVenue() const;
	void SetVenue(std::string const& inVenue);
	std::string const& GetNumber() const;
	void SetNumber(std::string const& inNumber);
	std::string const& GetHeight() const;
	void SetHeight(std::string const& inHeight);
	bool GetReceived() const;
	void SetReceived(bool inReceived);
	std::string const& GetNote() const;
	void SetNote(std::string const& inNote);

private:
	~ARBDogRegNum();
	std::string m_Venue;
	std::string m_Number;
	std::string m_Height;
	bool m_bReceived;
	std::string m_Note;
};

inline std::string const& ARBDogRegNum::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogRegNum::SetVenue(std::string const& inVenue)
{
	m_Venue = inVenue;
}

inline std::string const& ARBDogRegNum::GetNumber() const
{
	return m_Number;
}

inline void ARBDogRegNum::SetNumber(std::string const& inNumber)
{
	m_Number = inNumber;
}

inline std::string const& ARBDogRegNum::GetHeight() const
{
	return m_Height;
}

inline void ARBDogRegNum::SetHeight(std::string const& inHeight)
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

inline std::string const& ARBDogRegNum::GetNote() const
{
	return m_Note;
}

inline void ARBDogRegNum::SetNote(std::string const& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRegNum objects.
 */
class ARBDogRegNumList : public ARBVectorLoad2<ARBDogRegNum>
{
public:
	void sort(bool inDescending = true);

	/**
	 * Number of registration numbers for a venue.
	 * @param inVenue Venue to tally.
	 * @return Number of registration numbers found.
	 */
	int NumRegNumsInVenue(std::string const& inVenue) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
		std::string const& inOldVenue,
		std::string const& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::string const& inVenue);

	/**
	 * Find a registration number.
	 * @param inVenue Venue to find a number for.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBDogRegNum const* FindRegNum(std::string const& inVenue) const;

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBDogRegNum* AddRegNum(
		std::string const& inVenue,
		std::string const& inNumber);

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inRegNum Registration number object
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBDogRegNum* AddRegNum(ARBDogRegNum* inRegNum);

	/**
	 * Delete a registration number.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number.
	 * @return Number of objects deleted.
	 */
	int DeleteRegNum(
		std::string const& inVenue,
		std::string const& inNumber);
};
