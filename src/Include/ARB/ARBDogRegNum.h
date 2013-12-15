#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogRegNum and ARBDogRegNumList classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-11-22 Added FindRegNum().
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


/**
 * Registration number for a venue.
 */
class ARBDogRegNum : public ARBBase
{
protected:
	ARBDogRegNum();
	ARBDogRegNum(ARBDogRegNum const& rhs);

public:
	~ARBDogRegNum();
	static ARBDogRegNumPtr New();
	ARBDogRegNumPtr Clone() const;

	ARBDogRegNum& operator=(ARBDogRegNum const& rhs);

	bool operator==(ARBDogRegNum const& rhs) const;
	bool operator!=(ARBDogRegNum const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::wstring GetGenericName() const
	{
		return GetVenue() + L" " + GetNumber();
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const;

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogRegNum element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRegNum element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetVenue() const
	{
		return m_Venue;
	}
	void SetVenue(std::wstring const& inVenue)
	{
		m_Venue = inVenue;
	}
	std::wstring const& GetNumber() const
	{
		return m_Number;
	}
	void SetNumber(std::wstring const& inNumber)
	{
		m_Number = inNumber;
	}
	std::wstring const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(std::wstring const& inHeight)
	{
		m_Height = inHeight;
	}
	bool GetReceived() const
	{
		return m_bReceived;
	}
	void SetReceived(bool inReceived)
	{
		m_bReceived = inReceived;
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
	std::wstring m_Venue;
	std::wstring m_Number;
	std::wstring m_Height;
	bool m_bReceived;
	std::wstring m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRegNum objects.
 */
class ARBDogRegNumList : public ARBVector<ARBDogRegNumPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Sort registration numbers by name.
	 */
	void sort();

	/**
	 * Number of registration numbers for a venue.
	 * @param inVenue Venue to tally.
	 * @return Number of registration numbers found.
	 */
	int NumRegNumsInVenue(std::wstring const& inVenue) const;

	/**
	 * Rename a venue.
	 * @param inOldVenue Venue name being renamed.
	 * @param inNewVenue New venue name.
	 * @return Number of items updated.
	 */
	int RenameVenue(
			std::wstring const& inOldVenue,
			std::wstring const& inNewVenue);

	/**
	 * Delete a venue.
	 * @param inVenue Venue name being deleted.
	 * @return Number of items removed.
	 */
	int DeleteVenue(std::wstring const& inVenue);

	/**
	 * Find a registration number.
	 * @param inVenue Venue to find a number for.
	 * @param outRegNum Pointer to found object.
	 * @return Whether the object was found.
	 */
	bool FindRegNum(
			std::wstring const& inVenue,
			ARBDogRegNumPtr* outRegNum = NULL) const;

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number to add.
	 * @param outRegNum Pointer to new object.
	 * @return Whether the object was added.
	 */
	bool AddRegNum(
			std::wstring const& inVenue,
			std::wstring const& inNumber,
			ARBDogRegNumPtr* outRegNum = NULL);

	/**
	 * Add a registration number, duplicates are allowed.
	 * @param inRegNum Registration number object
	 * @return Whether the object was added.
	 */
	bool AddRegNum(ARBDogRegNumPtr inRegNum);

	/**
	 * Delete a registration number.
	 * @param inVenue Venue of number.
	 * @param inNumber Registration number.
	 * @return Number of objects deleted.
	 */
	int DeleteRegNum(
			std::wstring const& inVenue,
			std::wstring const& inNumber);
};
