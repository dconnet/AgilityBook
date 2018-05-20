#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBInfoItem class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-12-11 Merged separate club/judge/location classes.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-07 Created
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"
#include <set>


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
	bool operator!=(ARBInfoItem const& rhs) const
	{
		return !operator==(rhs);
	}
	bool operator<(ARBInfoItem const& rhs) const
	{
		return m_Name < rhs.GetName();
	}
	bool operator>(ARBInfoItem const& rhs) const
	{
		return m_Name > rhs.GetName();
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const;

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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			std::wstring const& inItemName);

	/**
	 * Save a items entry
	 * @param ioTree Parent element.
	 * @param inItemName Name of collection being saved.
	 */
	bool Save(
			ElementNodePtr const& ioTree,
			std::wstring const& inItemName) const;

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}
	std::wstring const& GetComment() const
	{
		return m_Comment;
	}
	void SetComment(std::wstring const& inComment)
	{
		m_Comment = inComment;
	}
	bool IsVisible() const
	{
		return m_Visible;
	}
	void SetIsVisible(bool bVisible)
	{
		m_Visible = bVisible;
	}

private:
	std::wstring m_Name;
	std::wstring m_Comment;
	bool m_Visible;
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
	ARBInfoItemList(std::wstring const& inItemName);
	ARBInfoItemList(ARBInfoItemList const& rhs);
	ARBInfoItemList& operator=(ARBInfoItemList const& rhs);

	std::wstring const& GetItemName() const
	{
		return m_ItemName;
	}

	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual item element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a items entry
	 * @param ioTree Parent element.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Sort the list by name.
	 */
	void sort();

	/**
	 * Get the names of all the items.
	 * @param outNames All the names.
	 * @param inVisibleOnly Only get the visible names.
	 * @return Number of items in list.
	 */
	size_t GetAllItems(
			std::set<std::wstring>& outNames,
			bool inVisibleOnly) const;

	/**
	 * Remove entries from list that are in use but have no associated comments.
	 * @param inNamesInUse Names of items from runs.
	 */
	void CondenseContent(std::set<std::wstring> const& inNamesInUse);

	/**
	 * Find a item.
	 * @param inName Item to find.
	 * @param outItem Found item.
	 * @return Whether the object was found.
	 */
	bool FindItem(
			std::wstring const& inName,
			ARBInfoItemPtr* outItem = nullptr) const;

	/**
	 * Add a new item.
	 * @param inItem Name of item to add.
	 * @param outItem New object, NULL if name already exists or is empty.
	 * @return Whether object was added.
	 */
	bool AddItem(
			std::wstring const& inItem,
			ARBInfoItemPtr* outItem = nullptr);

	/**
	 * Add a new item.
	 * @param inItem Item to add.
	 * @return Whether the object was added.
	 */
	bool AddItem(ARBInfoItemPtr const& inItem);

	/**
	 * Delete a item.
	 * @param inItem Object to delete.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteItem(ARBInfoItemPtr const& inItem);

private:
	std::wstring m_ItemName;
};
