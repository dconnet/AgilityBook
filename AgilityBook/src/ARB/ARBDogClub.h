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
 * @brief ARBDogClub and ARBDogClubList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigScoring;
class ARBDate;
class ARBVersion;
class CElement;

/**
 * Dog club.
 */
class ARBDogClub : public ARBBase
{
public:
	ARBDogClub();
	ARBDogClub(const ARBDogClub& rhs);
	ARBDogClub& operator=(const ARBDogClub& rhs);
	bool operator==(const ARBDogClub& rhs) const;
	bool operator!=(const ARBDogClub& rhs) const;

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
	 * Load a club.
	 * @pre inTree is the actual ARBDogClub element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogClub element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/*
	 * Getters/setters.
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);

private:
	~ARBDogClub();
	std::string m_Name;
	std::string m_Venue;
};

inline std::string ARBDogClub::GetGenericName() const
{
	return m_Name;
}

inline const std::string& ARBDogClub::GetName() const
{
	return m_Name;
}

inline void ARBDogClub::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBDogClub::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogClub::SetVenue(const std::string& inVenue)
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
class ARBDogClubList : public ARBVectorLoad2<ARBDogClub>
{
public:
	bool operator==(const ARBDogClubList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogClubList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Get the primary club, used to establish rules.
	 * @return Pointer to first club.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBDogClub* GetPrimaryClub() const;

	/**
	 * Find the first scoring style to match.
	 * Club order is critical as we'll search the venues by club order.
	 * @param inConfig Configuration for looking up information.
	 * @param inEvent Event to search for.
	 * @param inDivision Division to search for.
	 * @param inLevel Level to search for.
	 * @param inDate Date of event.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigScoring* FindEvent(
		const ARBConfig* inConfig,
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate,
		std::string& ioErrMsg) const;

	/**
	 * Find a club that uses the specified venue.
	 * @param inVenue Venue to search for.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBDogClub* FindVenue(const std::string& inVenue) const;

	/**
	 * Add a club.
	 * @param inName Name of club to add.
	 * @param inVenue Venue of club.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBDogClub* AddClub(
		const std::string& inName,
		const std::string& inVenue);

	/**
	 * Delete a club.
	 * @param inName Name of club to delete.
	 * @param inVenue Venue of club.
	 * @return Whether club was deleted.
	 */
	bool DeleteClub(
		const std::string& inName,
		const std::string& inVenue);
};
