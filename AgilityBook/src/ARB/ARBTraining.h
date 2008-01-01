#pragma once

/*
 * Copyright © 2003-2008 David Connet. All Rights Reserved.
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
 * @brief ARBTraining class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-14 DRC Added FindTraining to support importing data.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-21 DRC Created
 */

#include <set>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"

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
	virtual tstring GetGenericName() const
	{
		return m_Date.GetString(ARBDate::eSlashMDY);
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const;

	/**
	 * Load a training entry
	 * @pre inTree is the actual ARBTraining element.
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
	 * @post The ARBTraining element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

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
	tstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(tstring const& inName)
	{
		m_Name = inName;
	}
	tstring const& GetSubName() const
	{
		return m_SubName;
	}
	void SetSubName(tstring const& inName)
	{
		m_SubName = inName;
	}
	tstring const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(tstring const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARBDate m_Date;
	tstring m_Name;
	tstring m_SubName;
	tstring m_Note;
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
			ElementNodePtr inTree,
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
	size_t GetAllNames(std::set<tstring>& outNames) const;

	/**
	 * Get a list of all the different sub-names in the list.
	 * @param outSubNames A list of the unique training sub-names.
	 * @return Number of unique sub-names.
	 */
	size_t GetAllSubNames(std::set<tstring>& outSubNames) const;

	/**
	 * Find a training object.
	 * @param inTraining Object to search for.
	 * @return Whether object is in the list.
	 * @note Equality is tested by value, not pointer.
	 */
	bool FindTraining(ARBTrainingPtr inTraining) const;

	/**
	 * Add a training object to the list.
	 * @param inTraining Object to add.
	 * @return Whether the object was added.
	 */
	bool AddTraining(ARBTrainingPtr inTraining);

	/**
	 * Delete a training object.
	 * @param inTraining Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteTraining(ARBTrainingPtr inTraining);
};
