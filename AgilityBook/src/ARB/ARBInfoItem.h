#pragma once

/*
 * Copyright © 2003-2005 David Connet. All Rights Reserved.
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
 * @li 2004-12-11 DRC Merged separate club/judge/location classes.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Comments about items
 */
class ARBInfoItem : public ARBBase
{
public:
	ARBInfoItem();
	ARBInfoItem(ARBInfoItem const& rhs);
	ARBInfoItem& operator=(ARBInfoItem const& rhs);
	bool operator==(ARBInfoItem const& rhs) const;
	bool operator!=(ARBInfoItem const& rhs) const;
	bool operator<(ARBInfoItem const& rhs) const;
	bool operator>(ARBInfoItem const& rhs) const;

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
		std::string const& inItemName);

	/**
	 * Save a items entry
	 * @param ioTree Parent element.
	 * @param inItemName Name of collection being saved.
	 */
	bool ARBInfoItem::Save(Element& ioTree,
		std::string const& inItemName) const;

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
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetComment() const;
	void SetComment(std::string const& inComment);

private:
	~ARBInfoItem();
	std::string m_Name;
	std::string m_Comment;
};

inline bool ARBInfoItem::operator<(ARBInfoItem const& rhs) const
{
	return m_Name < rhs.GetName();
}

inline bool ARBInfoItem::operator>(ARBInfoItem const& rhs) const
{
	return m_Name > rhs.GetName();
}

inline std::string const& ARBInfoItem::GetName() const
{
	return m_Name;
}

inline void ARBInfoItem::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBInfoItem::GetComment() const
{
	return m_Comment;
}

inline void ARBInfoItem::SetComment(std::string const& inComment)
{
	m_Comment = inComment;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBInfoItem objects.
 */
class ARBInfoItemList : public ARBVector<ARBInfoItem>
{
public:
	/**
	 * Construct a club/judge/location info object.
	 * @param inItemName Name of elements to be loaded.
	 */
	ARBInfoItemList(std::string const& inItemName);
	ARBInfoItemList(ARBInfoItemList const& rhs);
	ARBInfoItemList& operator=(ARBInfoItemList const& rhs);

	std::string const& GetItemName() const;

	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual item element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a items entry
	 * @param ioTree Parent element.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Sort the list by name.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get the names of all the items.
	 * @param outNames All the names.
	 * @return Number of items in list.
	 */
	size_t GetAllItems(std::set<std::string>& outNames) const;

	/**
	 * Remove entries from list that are in use but have no associated comments.
	 * @param inNamesInUse Names of items from runs.
	 */
	void CondenseContent(std::set<std::string> const& inNamesInUse);

	/**
	 * Find a item.
	 * @param inName Item to find.
	 * @return Object matching item.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoItem* FindItem(std::string const& inName) const;

	/**
	 * Add a new item.
	 * @param inItem Name of item to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoItem* AddItem(std::string const& inItem);

	/**
	 * Add a new item.
	 * @param inItem Item to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoItem* AddItem(ARBInfoItem* inItem);

	/**
	 * Delete a item.
	 * @param inItem Object to delete.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteItem(ARBInfoItem const* inItem);

private:
	std::string m_ItemName;
};

inline std::string const& ARBInfoItemList::GetItemName() const
{
	return m_ItemName;
}
