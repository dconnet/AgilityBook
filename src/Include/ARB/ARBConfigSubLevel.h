#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigSubLevel and ARBConfigSubLevelList class.
 * @author David Connet
 *
 * Revision History
 * 2013-09-03 Added short name.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


/**
 * Sublevel, allows a level to be split into A/B groupings.
 */
class ARB_API ARBConfigSubLevel : public ARBBase
{
protected:
	ARBConfigSubLevel();
	ARBConfigSubLevel(ARBConfigSubLevel const& rhs);
	ARBConfigSubLevel(ARBConfigSubLevel&& rhs);

public:
	~ARBConfigSubLevel();
	static ARBConfigSubLevelPtr New();
	ARBConfigSubLevelPtr Clone() const;

	ARBConfigSubLevel& operator=(ARBConfigSubLevel const& rhs);
	ARBConfigSubLevel& operator=(ARBConfigSubLevel&& rhs);

	bool operator==(ARBConfigSubLevel const& rhs) const;
	bool operator!=(ARBConfigSubLevel const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override
	{
		return 0;
	}

	/**
	 * Load a sublevel configuration.
	 * @pre inTree is the actual ARBConfigSubLevel element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigSubLevel element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Update this configuration from inLevelNew.
	 * @param indent Indentation level for generating messages.
	 * @param inLevelNew Level to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, ARBConfigSubLevelPtr const& inLevelNew, std::wstring& ioInfo);

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
	std::wstring const& GetShortName() const
	{
		return m_ShortName;
	}
	void SetShortName(std::wstring const& inName)
	{
		m_ShortName = inName;
	}

private:
	std::wstring m_Name;
	std::wstring m_ShortName;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigSubLevel objects.
 */
class ARB_API ARBConfigSubLevelList : public ARBVector<ARBConfigSubLevelPtr>
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
	bool Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Reorder the list based on 'inList'
	 * @param inList List we want to match order on.
	 */
	void ReorderBy(ARBConfigSubLevelList const& inList);

	/**
	 * Find a sublevel.
	 * @param inName Name of sublevel to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether name exists.
	 */
	bool FindSubLevel(std::wstring const& inName, ARBConfigSubLevelPtr* outLevel = nullptr) const;

	/**
	 * Find a sublevel.
	 * @param inName Name of sublevel to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether the object was added.
	 */
	bool AddSubLevel(std::wstring const& inName, ARBConfigSubLevelPtr* outLevel = nullptr);

	/**
	 * Delete a sublevel.
	 * @param inName Name of sublevel to delete.
	 * @return Whether sublevel was deleted or not.
	 */
	bool DeleteSubLevel(std::wstring const& inName);
};
