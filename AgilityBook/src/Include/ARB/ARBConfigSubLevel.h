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
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


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
	bool operator!=(ARBConfigSubLevel const& rhs) const
	{
		return !operator==(rhs);
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
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigSubLevel element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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

private:
	std::wstring m_Name;
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

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
	bool FindSubLevel(
			std::wstring const& inName,
			ARBConfigSubLevelPtr* outLevel = NULL) const;

	/**
	 * Find a sublevel.
	 * @param inName Name of sublevel to find.
	 * @param outLevel Pointer to object, NULL if not found.
	 * @return Whether the object was added.
	 */
	bool AddSubLevel(
			std::wstring const& inName,
			ARBConfigSubLevelPtr* outLevel = NULL);

	/**
	 * Delete a sublevel.
	 * @param inName Name of sublevel to delete.
	 * @return Whether sublevel was deleted or not.
	 */
	bool DeleteSubLevel(std::wstring const& inName);
};
