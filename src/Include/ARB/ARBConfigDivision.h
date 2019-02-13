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
 * 2013-09-03 Added short name.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-14 Moved 'Titles' to 'Venue'.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-01-21 Added DeleteTitle.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBConfigLevel.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


/**
 * Division configuration information.
 */
class ARB_API ARBConfigDivision : public ARBBase
{
protected:
	ARBConfigDivision();
	ARBConfigDivision(ARBConfigDivision const& rhs);
	ARBConfigDivision(ARBConfigDivision&& rhs);

public:
	~ARBConfigDivision();
	static ARBConfigDivisionPtr New();
	ARBConfigDivisionPtr Clone() const;

	ARBConfigDivision& operator=(ARBConfigDivision const& rhs);
	ARBConfigDivision& operator=(ARBConfigDivision&& rhs);

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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/**
	 * Update this configuration from inDivNew.
	 * @param indent Indentation level for generating messages.
	 * @param inDivNew Division to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(
			int indent,
			ARBConfigDivisionPtr const& inDivNew,
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
class ARB_API ARBConfigDivisionList : public ARBVector<ARBConfigDivisionPtr>
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
			ElementNodePtr const& inTree,
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
			ARBConfigDivisionPtr* outDiv = nullptr) const;

	/**
	 * Add a division.
	 * @param inDiv Name of division to add.
	 * @param outDiv Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddDivision(
			std::wstring const& inDiv,
			ARBConfigDivisionPtr* outDiv = nullptr);

	/**
	 * Add a division.
	 * @param inDiv Division to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 */
	bool AddDivision(ARBConfigDivisionPtr const& inDiv);

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
