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
 * @brief ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigScoring;
class ARBDate;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Dog club.
 */
class ARBDogClub : public ARBBase
{
protected:
	ARBDogClub();
	ARBDogClub(ARBDogClub const& rhs);

public:
	~ARBDogClub();
	static ARBDogClubPtr New();
	ARBDogClubPtr Clone() const;

	ARBDogClub& operator=(ARBDogClub const& rhs);

	bool operator==(ARBDogClub const& rhs) const;
	bool operator!=(ARBDogClub const& rhs) const;

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
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load a club.
	 * @pre inTree is the actual ARBDogClub element.
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
	 * @post The ARBDogClub element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);
	ARBString const& GetVenue() const;
	void SetVenue(ARBString const& inVenue);

private:
	ARBString m_Name;
	ARBString m_Venue;
};

inline ARBString ARBDogClub::GetGenericName() const
{
	return m_Name;
}

inline ARBString const& ARBDogClub::GetName() const
{
	return m_Name;
}

inline void ARBDogClub::SetName(ARBString const& inName)
{
	m_Name = inName;
}

inline ARBString const& ARBDogClub::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogClub::SetVenue(ARBString const& inVenue)
{
	m_Venue = inVenue;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogClub objects, used in a trial object.
 * @note The order within the club list is important. It is used to denote
 *       the primary club in a trial. The primary club is the club that is
 *       used to establish the rules used in entering a run.
 */
class ARBDogClubList : public ARBVector<ARBDogClubPtr>
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
	 * Get the primary club, used to establish rules.
	 * @param outClub Pointer to first club.
	 * @return Whether there is a primary club.
	 */
	bool GetPrimaryClub(ARBDogClubPtr* outClub = NULL) const;

	/**
	 * Get the primary club's name.
	 * @return Name of primary club, empty is none.
	 */
	ARBString GetPrimaryClubName() const;

	/**
	 * Get the primary club's venue.
	 * @return Venue of primary club, empty is none.
	 */
	ARBString GetPrimaryClubVenue() const;

	/**
	 * Find the first scoring style to match.
	 * Club order is critical as we'll search the venues by club order.
	 * @param inConfig Configuration for looking up information.
	 * @param inEvent Event to search for.
	 * @param inDivision Division to search for.
	 * @param inLevel Level to search for.
	 * @param inDate Date of event.
	 * @param ioCallback Error processing callback.
	 * @param outScoring Pointer to object, NULL if not found.
	 * @return Whether an event was found.
	 */
	bool FindEvent(
			ARBConfig const& inConfig,
			ARBString const& inEvent,
			ARBString const& inDivision,
			ARBString const& inLevel,
			ARBDate const& inDate,
			ARBErrorCallback& ioCallback,
			ARBConfigScoringPtr* outScoring = NULL) const;

	/**
	 * Find a club that uses the specified venue.
	 * @param inVenue Venue to search for.
	 * @param outClub Found club.
	 * @return Whether the club was found.
	 */
	bool FindVenue(
			ARBString const& inVenue,
			ARBDogClubPtr* outClub = NULL) const;

	/**
	 * Add a club.
	 * @param inName Name of club to add.
	 * @param inVenue Venue of club.
	 * @param outClub Added club.
	 * @return Whether the club was added.
	 */
	bool AddClub(
			ARBString const& inName,
			ARBString const& inVenue,
			ARBDogClubPtr* outClub = NULL);

	/**
	 * Delete a club.
	 * @param inName Name of club to delete.
	 * @param inVenue Venue of club.
	 * @return Whether club was deleted.
	 */
	bool DeleteClub(
			ARBString const& inName,
			ARBString const& inVenue);
};
