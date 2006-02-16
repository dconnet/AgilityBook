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
 * @brief ARBConfigSubLevel and ARBConfigSubLevelList class.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * Sublevel, allows a level to be split into A/B groupings.
 */
class ARBConfigSubLevel : public ARBBase
{
protected:
	ARBConfigSubLevel();
	ARBConfigSubLevel(ARBConfigSubLevel const& rhs);

public:
	~ARBConfigSubLevel();
	static ARBConfigSubLevelPtr New();
	ARBConfigSubLevelPtr Clone() const;

	ARBConfigSubLevel& operator=(ARBConfigSubLevel const& rhs);

	bool operator==(ARBConfigSubLevel const& rhs) const;
	bool operator!=(ARBConfigSubLevel const& rhs) const;

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
	 * Load a sublevel configuration.
	 * @pre inTree is the actual ARBConfigSubLevel element.
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
	 * @post The ARBConfigSubLevel element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);

private:
	ARBString m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigSubLevel objects.
 */
class ARBConfigSubLevelList : public ARBVector<ARBConfigSubLevelPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
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
	 * Find a sublevel.
	 * @param inName Name of sublevel to find.
	 * @return Whether name exists.
	 */
	bool FindSubLevel(ARBString const& inName) const;

	/**
	 * Find a sublevel.
	 * @param inName Name of sublevel to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether the object was added.
	 */
	bool AddSubLevel(
			ARBString const& inName,
			ARBConfigSubLevelPtr* outLevel = NULL);

	/**
	 * Delete a sublevel.
	 * @param inName Name of sublevel to delete.
	 * @return Whether sublevel was deleted or not.
	 */
	bool DeleteSubLevel(ARBString const& inName);
};
