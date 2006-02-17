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
 * @brief ARBConfigDivision and ARBConfigDivisionList class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Added DeleteTitle.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigLevel.h"
#include "ARBTypes.h"

/**
 * Division configuration information.
 */
class ARBConfigDivision : public ARBBase
{
protected:
	ARBConfigDivision();
	ARBConfigDivision(ARBConfigDivision const& rhs);

public:
	~ARBConfigDivision();
	static ARBConfigDivisionPtr New();
	ARBConfigDivisionPtr Clone() const;

	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);

	bool operator==(ARBConfigDivision const& rhs) const;
	bool operator!=(ARBConfigDivision const& rhs) const;

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

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
	 * Load a division configuration.
	 * @pre inTree is the actual ARBConfigDivision element.
	 * @param ioVenue Venue to update when migrating on old file.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigVenue& ioVenue,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Update this configuration from inDivNew.
	 * @param indent Indentation level for generating messages.
	 * @param inDivNew Division to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigDivisionPtr inDivNew,
			ARBString& ioInfo);

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);
	ARBConfigLevelList const& GetLevels() const;
	ARBConfigLevelList& GetLevels();

private:
	ARBString m_Name;
	ARBConfigLevelList m_Levels;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigDivision objects.
 */
class ARBConfigDivisionList : public ARBVector<ARBConfigDivisionPtr>
{
public:
	/**
	 * Load a division configuration.
	 * @pre inTree is the actual ARBConfigDivision element.
	 * @param ioVenue Venue to update when migrating on old file.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfigVenue& ioVenue,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Verify a level exists.
	 * @param inDiv Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
			ARBString const& inDiv,
			ARBString const& inLevel) const;

	/**
	 * Find the named division.
	 * @param inDiv Division to find.
	 * @param outDiv Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindDivision(
			ARBString const& inDiv,
			ARBConfigDivisionPtr* outDiv = NULL) const;

	/**
	 * Add a division.
	 * @param inDiv Name of division to add.
	 * @param outDiv Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddDivision(
			ARBString const& inDiv,
			ARBConfigDivisionPtr* outDiv = NULL);

	/**
	 * Add a division.
	 * @param inDiv Division to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	bool AddDivision(ARBConfigDivisionPtr inDiv);

	/**
	 * Delete the division.
	 * @param inDiv Name of division to delete.
	 * @param ioEvents List of events to be updated.
	 * @return Number of divisions deleted (0 or 1).
	 */
	int DeleteDivision(
			ARBString const& inDiv,
			ARBConfigEventList& ioEvents);
};
