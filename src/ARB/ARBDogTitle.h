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
 * @brief ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigVenue;
class ARBVersion;
class CElement;

/**
 * Titles a dog has earned.
 */
class ARBDogTitle : public ARBBase
{
public:
	ARBDogTitle();
	ARBDogTitle(const ARBDogTitle& rhs);
	ARBDogTitle& operator=(const ARBDogTitle& rhs);
	bool operator==(const ARBDogTitle& rhs) const;
	bool operator!=(const ARBDogTitle& rhs) const;

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
	 * Load a title.
	 * @pre inTree is the actual ARBDogTitle element.
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
	 * @post The ARBDogTitle element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/*
	 * Getters/setters.
	 */
	const ARBDate& GetDate() const;
	void SetDate(const ARBDate& inDate);
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const bool GetReceived() const;
	void SetReceived(bool inReceived);

private:
	~ARBDogTitle();
	ARBDate m_Date;
	std::string m_Venue;
	std::string m_Name;
	bool m_bReceived;
};

inline std::string ARBDogTitle::GetGenericName() const
{
	return m_Name;
}

inline const ARBDate& ARBDogTitle::GetDate() const
{
	return m_Date;
}

inline void ARBDogTitle::SetDate(const ARBDate& inDate)
{
	m_Date = inDate;
}

inline const std::string& ARBDogTitle::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogTitle::SetVenue(const std::string& inVenue)
{
	m_Venue = inVenue;
}

inline const std::string& ARBDogTitle::GetName() const
{
	return m_Name;
}

inline void ARBDogTitle::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const bool ARBDogTitle::GetReceived() const
{
	return m_bReceived;
}

inline void ARBDogTitle::SetReceived(bool inReceived)
{
	m_bReceived = inReceived;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogTitle objects.
 */
class ARBDogTitleList : public ARBVectorLoad2<ARBDogTitle>
{
public:
	bool operator==(const ARBDogTitleList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogTitleList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Sort the list by date.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get the number of titles in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInVenue(const std::string& inVenue) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(const std::string& inVenue);

	/**
	 * Get the number of titles in a division.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @param inDiv Division to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inDiv) const;

	/**
	 * Rename a division, rename any dependent objects.
	 * @param inVenue Venue division is in.
	 * @param inOldDiv Division name being renamed.
	 * @param inNewDiv New division name.
	 * @return Number of items changed.
	 */
	int RenameDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inOldDiv,
		const std::string& inNewDiv);

	/**
	 * Delete a division, remove any dependent objects.
	 * @param inConfig Configuration for looking up information.
	 * @param inVenue Venue division is in.
	 * @param inDiv Division name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);

	/**
	 * Number of titles in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInUse(
		const std::string& inVenue,
		const std::string& inTitle) const;

	/**
	 * Rename a title, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldTitle Title name being renamed.
	 * @param inNewTitle New title name.
	 * @return Number of items changed.
	 */
	int RenameTitle(
		const std::string& inVenue,
		const std::string& inOldTitle,
		const std::string& inNewTitle);

	/**
	 * Delete a title, remove any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteTitle(
		const std::string& inVenue,
		const std::string& inTitle);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBDogTitle* AddTitle(ARBDogTitle* inTitle);
};
