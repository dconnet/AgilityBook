#pragma once

/*
 * Copyright © 2003-2004 David Connet. All Rights Reserved.
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
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-14 DRC Added FindTraining to support importing data.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-21 DRC Created
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBVersion;
class Element;

/**
 * The training logbook
 */
class ARBTraining : public ARBBase
{
public:
	ARBTraining();
	ARBTraining(ARBTraining const& rhs);
	ARBTraining& operator=(ARBTraining const& rhs);
	bool operator==(ARBTraining const& rhs) const;
	bool operator!=(ARBTraining const& rhs) const;
	bool operator<(ARBTraining const& rhs) const;
	bool operator>(ARBTraining const& rhs) const;
	bool operator<(ARBDate const& rhs) const;
	bool operator>(ARBDate const& rhs) const;

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
	 * Load a training entry
	 * @pre inTree is the actual ARBTraining element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBTraining element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBDate const& GetDate() const;
	void SetDate(ARBDate const& inDate);
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetSubName() const;
	void SetSubName(std::string const& inName);
	std::string const& GetNote() const;
	void SetNote(std::string const& inNote);

private:
	~ARBTraining();
	ARBDate m_Date;
	std::string m_Name;
	std::string m_SubName;
	std::string m_Note;
};

inline bool ARBTraining::operator<(ARBTraining const& rhs) const
{
	return m_Date < rhs.GetDate();
}

inline bool ARBTraining::operator>(ARBTraining const& rhs) const
{
	return m_Date > rhs.GetDate();
}

inline bool ARBTraining::operator<(ARBDate const& rhs) const
{
	return m_Date < rhs;
}

inline bool ARBTraining::operator>(ARBDate const& rhs) const
{
	return m_Date > rhs;
}

inline ARBDate const& ARBTraining::GetDate() const
{
	return m_Date;
}

inline void ARBTraining::SetDate(ARBDate const& inDate)
{
	m_Date = inDate;
}

inline std::string const& ARBTraining::GetName() const
{
	return m_Name;
}

inline void ARBTraining::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBTraining::GetSubName() const
{
	return m_SubName;
}

inline void ARBTraining::SetSubName(std::string const& inName)
{
	m_SubName = inName;
}

inline std::string const& ARBTraining::GetNote() const
{
	return m_Note;
}

inline void ARBTraining::SetNote(std::string const& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBTraining objects.
 */
class ARBTrainingList : public ARBVectorLoad1<ARBTraining>
{
public:
	/**
	 * Sort the list by date.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get a list of all the different names in the list.
	 * @param outNames A list of the unique training names.
	 * @return Number of unique names.
	 */
	size_t GetAllNames(std::set<std::string>& outNames) const;

	/**
	 * Get a list of all the different sub-names in the list.
	 * @param outNames A list of the unique training sub-names.
	 * @return Number of unique sub-names.
	 */
	size_t GetAllSubNames(std::set<std::string>& outSubNames) const;

	/**
	 * Find a training object.
	 * @param inTraining Object to search for.
	 * @return Matching object.
	 * @post The returned object is <i>not</i> reference counted.
	 * @note Equality is tested by value, not pointer.
	 */
	ARBTraining const* FindTraining(ARBTraining const* inTraining) const;

	/**
	 * Add a training object to the list.
	 * @param inTraining Object to add.
	 * @return Returns the added object.
	 * @post The pointer is added to the list and its ref count is incremented.
	 */
	ARBTraining* AddTraining(ARBTraining* inTraining);

	/**
	 * Delete a training object.
	 * @param inTraining Object to delete.
	 * @return Object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteTraining(ARBTraining const* inTraining);
};
