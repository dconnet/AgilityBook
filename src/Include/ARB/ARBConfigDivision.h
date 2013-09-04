#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigDivision and ARBConfigDivisionList class.
 * @author David Connet
 *
 * Revision History
 * @li 2013-09-03 DRC Added short name.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-14 DRC Moved 'Titles' to 'Venue'.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Added DeleteTitle.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBConfigLevel.h"
#include "ARB/ARBTypes2.h"


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
	bool operator!=(ARBConfigDivision const& rhs) const
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
			std::wstring& ioInfo);

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
	ARBConfigLevelList const& GetLevels() const
	{
		return m_Levels;
	}
	ARBConfigLevelList& GetLevels()
	{
		return m_Levels;
	}

private:
	std::wstring m_Name;
	std::wstring m_ShortName;
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Reorder the list based on 'inList'
	 * @param inList List we want to match order on.
	 */
	void ReorderBy(ARBConfigDivisionList const& inList);

	/**
	 * Verify a level exists.
	 * @param inDiv Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
			std::wstring const& inDiv,
			std::wstring const& inLevel) const;

	/**
	 * Find the named division.
	 * @param inDiv Division to find.
	 * @param outDiv Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindDivision(
			std::wstring const& inDiv,
			ARBConfigDivisionPtr* outDiv = NULL) const;

	/**
	 * Add a division.
	 * @param inDiv Name of division to add.
	 * @param outDiv Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddDivision(
			std::wstring const& inDiv,
			ARBConfigDivisionPtr* outDiv = NULL);

	/**
	 * Add a division.
	 * @param inDiv Division to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 */
	bool AddDivision(ARBConfigDivisionPtr inDiv);

	/**
	 * Delete the division.
	 * @param inDiv Name of division to delete.
	 * @param ioEvents List of events to be updated.
	 * @return Number of divisions deleted (0 or 1).
	 */
	int DeleteDivision(
			std::wstring const& inDiv,
			ARBConfigEventList& ioEvents);
};
