#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBTraining class.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-12-14 Added FindTraining to support importing data.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-21 Created
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <set>


/**
 * The training logbook
 */
class ARBTraining : public ARBBase
{
protected:
	ARBTraining();
	ARBTraining(ARBTraining const& rhs);

public:
	~ARBTraining();
	static ARBTrainingPtr New();
	ARBTrainingPtr Clone() const;

	ARBTraining& operator=(ARBTraining const& rhs);

	bool operator==(ARBTraining const& rhs) const;
	bool operator!=(ARBTraining const& rhs) const
	{
		return !operator==(rhs);
	}
	bool operator<(ARBTraining const& rhs) const
	{
		return m_Date < rhs.GetDate();
	}
	bool operator>(ARBTraining const& rhs) const
	{
		return m_Date > rhs.GetDate();
	}
	bool operator<(ARBDate const& rhs) const
	{
		return m_Date < rhs;
	}
	bool operator>(ARBDate const& rhs) const
	{
		return m_Date > rhs;
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		return m_Date.GetString(ARBDate::eSlashMDY);
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override;

	/**
	 * Load a training entry
	 * @pre inTree is the actual ARBTraining element.
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
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBTraining element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetDate() const
	{
		return m_Date;
	}
	void SetDate(ARBDate const& inDate)
	{
		m_Date = inDate;
	}
	std::wstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}
	std::wstring const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(std::wstring const& inName)
	{
		m_SubName = inName;
	}
	std::wstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(std::wstring const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDate m_Date;
	std::wstring m_Name;
	std::wstring m_SubName;
	std::wstring m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBTraining objects.
 */
class ARBTrainingList : public ARBVector<ARBTrainingPtr>
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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort the list by date.
	 */
	void sort();

	/**
	 * Get a list of all the different names in the list.
	 * @param outNames A list of the unique training names.
	 * @return Number of unique names.
	 */
	size_t GetAllNames(std::set<std::wstring>& outNames) const;

	/**
	 * Get a list of all the different sub-names in the list.
	 * @param outSubNames A list of the unique training sub-names.
	 * @return Number of unique sub-names.
	 */
	size_t GetAllSubNames(std::set<std::wstring>& outSubNames) const;

	/**
	 * Find a training object.
	 * @param inTraining Object to search for.
	 * @return Whether object is in the list.
	 * @note Equality is tested by value, not pointer.
	 */
	bool FindTraining(ARBTrainingPtr const& inTraining) const;

	/**
	 * Add a training object to the list.
	 * @param inTraining Object to add.
	 * @return Whether the object was added.
	 */
	bool AddTraining(ARBTrainingPtr const& inTraining);

	/**
	 * Delete a training object.
	 * @param inTraining Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteTraining(ARBTrainingPtr const& inTraining);
};
