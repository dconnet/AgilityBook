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
 * @brief ARBDogTitle and ARBDogTitleList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-13 DRC Added ability to hide titles, including unearned ones.
 *                    An unearned title has an invalid date.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"

/**
 * Titles a dog has earned.
 */
class ARBDogTitle : public ARBBase
{
protected:
	ARBDogTitle();
	ARBDogTitle(ARBDogTitle const& rhs);

public:
	~ARBDogTitle();
	static ARBDogTitlePtr New()
	{
		return ARBDogTitlePtr(new ARBDogTitle());
	}
	ARBDogTitlePtr Clone() const
	{
		return ARBDogTitlePtr(new ARBDogTitle(*this));
	}

	ARBDogTitle& operator=(ARBDogTitle const& rhs);

	bool operator==(ARBDogTitle const& rhs) const;
	bool operator!=(ARBDogTitle const& rhs) const
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
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load a title.
	 * @pre inTree is the actual ARBDogTitle element.
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
	 * @post The ARBDogTitle element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetDate() const
	{
		return m_Date;
	}
	/**
	 * @post Setting the date also affects the hidden value. An invalid date
	 *       will hide the entry, a valid date will unhide.
	 */
	void SetDate(ARBDate const& inDate)
	{
		m_Date = inDate;
		if (!m_Date.IsValid())
			m_bHidden = true;
	}
	ARBString const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(ARBString const& inVenue)
	{
		m_Venue = inVenue;
	}
	ARBString const& GetRawName() const
	{
		return m_Name;
	}
	short GetInstance() const
	{
		return m_Instance;
	}
	void SetName(
			ARBString const& inName,
			short inInstance)
	{
		m_Name = inName;
		m_Instance = inInstance;
	}
	bool GetReceived() const
	{
		return m_bReceived;
	}
	void SetReceived(bool inReceived)
	{
		m_bReceived = inReceived;
	}
	bool IsHidden() const
	{
		return m_bHidden;
	}
	void SetHidden(bool bHidden)
	{
		if (m_Date.IsValid())
			m_bHidden = bHidden;
		else
			m_bHidden = true;
	}

private:
	ARBDate m_Date;
	ARBString m_Venue;
	ARBString m_Name;
	short m_Instance;
	bool m_bReceived;
	bool m_bHidden;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogTitle objects.
 */
class ARBDogTitleList : public ARBVector<ARBDogTitlePtr>
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
	 * Sort the list by date.
	 */
	void sort();

	/**
	 * Get the number of titles in a venue.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInVenue(ARBString const& inVenue) const;

	/**
	 * Find a title
	 * @param inVenue Venue name.
	 * @param inTitle Name of title.
	 * @param outTitle Pointer to found title.
	 */
	bool FindTitle(
			ARBString const& inVenue,
			ARBString const& inTitle,
			ARBDogTitlePtr* outTitle = NULL) const;

	/**
	 * Find the maximum instance of a title
	 * @param inVenue Venue name.
	 * @param inTitle Name of title.
	 */
	short FindMaxInstance(
			ARBString const& inVenue,
			ARBString const& inTitle) const;

	/**
	 * Rename a venue, rename any dependent objects.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items changed.
	 */
	int RenameVenue(
			ARBString const& inOldVenue,
			ARBString const& inNewVenue);

	/**
	 * Delete a venue, remove any dependent objects.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(ARBString const& inVenue);

	/**
	 * Number of titles in use.
	 * Used to warning about impending configuration changes.
	 * @param inVenue Venue title is in.
	 * @param inTitle Title to tally.
	 * @return Number of objects.
	 */
	int NumTitlesInUse(
			ARBString const& inVenue,
			ARBString const& inTitle) const;

	/**
	 * Rename a title, rename any dependent objects.
	 * @param inVenue Venue title is in.
	 * @param inOldTitle Title name being renamed.
	 * @param inNewTitle New title name.
	 * @return Number of items changed.
	 */
	int RenameTitle(
			ARBString const& inVenue,
			ARBString const& inOldTitle,
			ARBString const& inNewTitle);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Whether the object was added.
	 */
	bool AddTitle(ARBDogTitlePtr inTitle);

	/**
	 * Delete a title, remove any dependent objects.
	 * @param inTitle Title being deleted.
	 * @return Was it removed?
	 */
	bool DeleteTitle(ARBDogTitlePtr inTitle);
};
