#pragma once

/*
 * Copyright � 2003-2004 David Connet. All Rights Reserved.
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
class CElement;

/**
 * The training logbook
 */
class ARBTraining : public ARBBase
{
public:
	ARBTraining();
	ARBTraining(const ARBTraining& rhs);
	ARBTraining& operator=(const ARBTraining& rhs);
	bool operator==(const ARBTraining& rhs) const;
	bool operator!=(const ARBTraining& rhs) const;
	bool operator<(const ARBTraining& rhs) const;
	bool operator>(const ARBTraining& rhs) const;
	bool operator<(const ARBDate& rhs) const;
	bool operator>(const ARBDate& rhs) const;

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
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBTraining element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/*
	 * Getters/setters.
	 */
	const ARBDate& GetDate() const;
	void SetDate(const ARBDate& inDate);
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);

private:
	~ARBTraining();
	ARBDate m_Date;
	std::string m_Name;
	std::string m_Note;
};

inline bool ARBTraining::operator<(const ARBTraining& rhs) const
{
	return m_Date < rhs.GetDate();
}

inline bool ARBTraining::operator>(const ARBTraining& rhs) const
{
	return m_Date > rhs.GetDate();
}

inline bool ARBTraining::operator<(const ARBDate& rhs) const
{
	return m_Date < rhs;
}

inline bool ARBTraining::operator>(const ARBDate& rhs) const
{
	return m_Date > rhs;
}

inline const ARBDate& ARBTraining::GetDate() const
{
	return m_Date;
}

inline void ARBTraining::SetDate(const ARBDate& inDate)
{
	m_Date = inDate;
}

inline const std::string& ARBTraining::GetNote() const
{
	return m_Note;
}

inline void ARBTraining::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

inline const std::string& ARBTraining::GetName() const
{
	return m_Name;
}

inline void ARBTraining::SetName(const std::string& inName)
{
	m_Name = inName;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBTraining objects.
 */
class ARBTrainingList : public ARBVectorLoad1<ARBTraining>
{
public:
	bool operator==(const ARBTrainingList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBTrainingList& rhs) const
	{
		return !isEqual(rhs);
	}

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
	 * Find a training object.
	 * @param inTraining Object to search for.
	 * @return Matching object.
	 * @post The returned object is <i>not</i> reference counted.
	 * @note Equality is tested by value, not pointer.
	 */
	const ARBTraining* FindTraining(const ARBTraining* inTraining) const;

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
	bool DeleteTraining(const ARBTraining* inTraining);
};
