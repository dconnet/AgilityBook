#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief ARBInfoClub class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-11-18 DRC Created
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Comments about clubs
 */
class ARBInfoClub : public ARBBase
{
public:
	ARBInfoClub();
	ARBInfoClub(ARBInfoClub const& rhs);
	ARBInfoClub& operator=(ARBInfoClub const& rhs);
	bool operator==(ARBInfoClub const& rhs) const;
	bool operator!=(ARBInfoClub const& rhs) const;
	bool operator<(ARBInfoClub const& rhs) const;
	bool operator>(ARBInfoClub const& rhs) const;

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
	 * Load a clubs entry
	 * @pre inTree is the actual ARBInfoClub element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		Element const& inTree,
		ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBInfoClub element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetComment() const;
	void SetComment(std::string const& inComment);

private:
	~ARBInfoClub();
	std::string m_Name;
	std::string m_Comment;
};

inline bool ARBInfoClub::operator<(ARBInfoClub const& rhs) const
{
	return m_Name < rhs.GetName();
}

inline bool ARBInfoClub::operator>(ARBInfoClub const& rhs) const
{
	return m_Name > rhs.GetName();
}

inline std::string const& ARBInfoClub::GetName() const
{
	return m_Name;
}

inline void ARBInfoClub::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBInfoClub::GetComment() const
{
	return m_Comment;
}

inline void ARBInfoClub::SetComment(std::string const& inComment)
{
	m_Comment = inComment;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBInfoClub objects.
 */
class ARBInfoClubList : public ARBVectorLoad1<ARBInfoClub>
{
public:
	/**
	 * Sort the list by name.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get the names of all the clubs.
	 * @param outNames All the names.
	 * @return Number of clubs in list.
	 */
	size_t GetAllClubs(std::set<std::string>& outNames) const;

	/**
	 * Remove entries from list that are in use but have no associated comments.
	 * @param inNamesInUse Names of clubs from runs.
	 */
	void CondenseContent(std::set<std::string> const& inNamesInUse);

	/**
	 * Find a club.
	 * @param inName Club to find.
	 * @return Object matching club.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoClub* FindClub(std::string const& inName) const;

	/**
	 * Add a new club.
	 * @param inClub Name of club to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoClub* AddClub(std::string const& inClub);

	/**
	 * Add a new club.
	 * @param inClub Club to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoClub* AddClub(ARBInfoClub* inClub);

	/**
	 * Delete a club.
	 * @param inClub Object to delete.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteClub(ARBInfoClub const* inClub);
};
