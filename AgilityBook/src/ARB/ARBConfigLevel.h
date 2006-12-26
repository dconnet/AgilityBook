#pragma once

/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @brief ARBConfigLevel and ARBConfigLevel classes.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigSubLevel.h"
#include "ARBTypes.h"

/**
 * Levels within a division.
 */
class ARBConfigLevel : public ARBBase
{
protected:
	ARBConfigLevel();
	ARBConfigLevel(ARBConfigLevel const& rhs);

public:
	~ARBConfigLevel();
	static ARBConfigLevelPtr New();
	ARBConfigLevelPtr Clone() const;

	ARBConfigLevel& operator=(ARBConfigLevel const& rhs);

	bool operator==(ARBConfigLevel const& rhs) const;
	bool operator!=(ARBConfigLevel const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		return m_Name;
	}

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
	 * Load a level configuration.
	 * @pre inTree is the actual ARBConfigLevel element.
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
	 * @post The ARBConfigLevel element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Update this configuration from inLevelNew.
	 * @param indent Indentation level for generating messages.
	 * @param inLevelNew Level to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigLevelPtr inLevelNew,
			ARBString& ioInfo);

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const
	{
		return m_Name;
	}
	void SetName(ARBString const& inName)
	{
		m_Name = inName;
	}
	ARBConfigSubLevelList const& GetSubLevels() const
	{
		return m_SubLevels;
	}
	ARBConfigSubLevelList& GetSubLevels()
	{
		return m_SubLevels;
	}

private:
	ARBString m_Name;
	ARBConfigSubLevelList m_SubLevels;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigLevel objects.
 */
class ARBConfigLevelList : public ARBVector<ARBConfigLevelPtr>
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
	 * Verify this is a valid level (not sublevel).
	 * Used to verify configuration stuff.
	 * @param inName Name of level to verify.
	 * @param inAllowWildCard Do we allow wildcards during verification?
	 * @return Whether there is a level that matches.
	 */
	bool VerifyLevel(
			ARBString const& inName,
			bool inAllowWildCard = true) const;

	/**
	 * Find a level.
	 * @param inName Name of level to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindLevel(
			ARBString const& inName,
			ARBConfigLevelPtr* outLevel = NULL);

	/**
	 * Find a level, only looks at leaf nodes.
	 * (if there are sub-levels, the level name is ignored).
	 * @param inName Name of level/sublevel to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindSubLevel(
			ARBString const& inName,
			ARBConfigLevelPtr* outLevel = NULL) const;

	/**
	 * Add a level.
	 * @param inName Level to add.
	 * @param outLevel Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddLevel(
			ARBString const& inName,
			ARBConfigLevelPtr* outLevel = NULL);

	/**
	 * Add a level.
	 * @param inLevel Level to add.
	 * @return Whether the object was added.
	 */
	bool AddLevel(ARBConfigLevelPtr inLevel);

	/**
	 * Delete a level.
	 * @param inName Name of level to delete.
	 * @param ioEvents List of events to be updated.
	 * @return Whether level was deleted or not.
	 */
	bool DeleteLevel(
			ARBString const& inName,
			ARBConfigEventList& ioEvents);

	/**
	 * Delete a sublevel.
	 * @param inName Name of sublevel to delete.
	 * @param outLevelModified Level containing the deleted sublevel has been renamed.
	 * @return Whether a sublevel was deleted.
	 * @post Deleting a sublevel may cause the parent level name to change.
	 */
	bool DeleteSubLevel(
			ARBString const& inName,
			bool& outLevelModified);
};
