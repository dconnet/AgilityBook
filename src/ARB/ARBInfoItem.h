#pragma once

/*
 * Copyright © 2003-2006 David Connet. All Rights Reserved.
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
 * @brief ARBInfoItem class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-12-11 DRC Merged separate club/judge/location classes.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include <set>
#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * Comments about items
 */
class ARBInfoItem : public ARBBase
{
protected:
	ARBInfoItem();
	ARBInfoItem(ARBInfoItem const& rhs);

public:
	~ARBInfoItem();
	static ARBInfoItemPtr New();
	ARBInfoItemPtr Clone() const;

	ARBInfoItem& operator=(ARBInfoItem const& rhs);

	bool operator==(ARBInfoItem const& rhs) const;
	bool operator!=(ARBInfoItem const& rhs) const;
	bool operator<(ARBInfoItem const& rhs) const;
	bool operator>(ARBInfoItem const& rhs) const;

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
	 * Load a items entry
	 * @pre inTree is the actual ARBInfoItem element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param inItemName Name of collection being loaded.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			ARBString const& inItemName);

	/**
	 * Save a items entry
	 * @param ioTree Parent element.
	 * @param inItemName Name of collection being saved.
	 */
	bool ARBInfoItem::Save(
			Element& ioTree,
			ARBString const& inItemName) const;

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBInfoItem element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);
	ARBString const& GetComment() const;
	void SetComment(ARBString const& inComment);

private:
	ARBString m_Name;
	ARBString m_Comment;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBInfoItem objects.
 */
class ARBInfoItemList : public ARBVector<ARBInfoItemPtr>
{
public:
	/**
	 * Construct a club/judge/location info object.
	 * @param inItemName Name of elements to be loaded.
	 */
	ARBInfoItemList(ARBString const& inItemName);
	ARBInfoItemList(ARBInfoItemList const& rhs);
	ARBInfoItemList& operator=(ARBInfoItemList const& rhs);

	ARBString const& GetItemName() const;

	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual item element.
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
	 * Save a items entry
	 * @param ioTree Parent element.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Sort the list by name.
	 */
	void sort();

	/**
	 * Get the names of all the items.
	 * @param outNames All the names.
	 * @return Number of items in list.
	 */
	size_t GetAllItems(std::set<ARBString>& outNames) const;

	/**
	 * Remove entries from list that are in use but have no associated comments.
	 * @param inNamesInUse Names of items from runs.
	 */
	void CondenseContent(std::set<ARBString> const& inNamesInUse);

	/**
	 * Find a item.
	 * @param inName Item to find.
	 * @param outItem Found item.
	 * @return Whether the object was found.
	 */
	bool FindItem(
			ARBString const& inName,
			ARBInfoItemPtr* outItem = NULL) const;

	/**
	 * Add a new item.
	 * @param inItem Name of item to add.
	 * @param outItem New object, NULL if name already exists or is empty.
	 * @return Whether object was added.
	 */
	bool AddItem(
			ARBString const& inItem,
			ARBInfoItemPtr* outItem = NULL);

	/**
	 * Add a new item.
	 * @param inItem Item to add.
	 * @return Whether the object was added.
	 */
	bool AddItem(ARBInfoItemPtr inItem);

	/**
	 * Delete a item.
	 * @param inItem Object to delete.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteItem(ARBInfoItemPtr inItem);

private:
	ARBString m_ItemName;
};
