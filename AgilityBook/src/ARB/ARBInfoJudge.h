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
 * @brief ARBInfoJudge class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-07 DRC Created
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class Element;

/**
 * Comments about judges
 */
class ARBInfoJudge : public ARBBase
{
public:
	ARBInfoJudge();
	ARBInfoJudge(ARBInfoJudge const& rhs);
	ARBInfoJudge& operator=(ARBInfoJudge const& rhs);
	bool operator==(ARBInfoJudge const& rhs) const;
	bool operator!=(ARBInfoJudge const& rhs) const;
	bool operator<(ARBInfoJudge const& rhs) const;
	bool operator>(ARBInfoJudge const& rhs) const;

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
	 * Load a judges entry
	 * @pre inTree is the actual ARBInfoJudge element.
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
	 * @post The ARBInfoJudge element will be created in ioTree.
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
	~ARBInfoJudge();
	std::string m_Name;
	std::string m_Comment;
};

inline bool ARBInfoJudge::operator<(ARBInfoJudge const& rhs) const
{
	return m_Name < rhs.GetName();
}

inline bool ARBInfoJudge::operator>(ARBInfoJudge const& rhs) const
{
	return m_Name > rhs.GetName();
}

inline std::string const& ARBInfoJudge::GetName() const
{
	return m_Name;
}

inline void ARBInfoJudge::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBInfoJudge::GetComment() const
{
	return m_Comment;
}

inline void ARBInfoJudge::SetComment(std::string const& inComment)
{
	m_Comment = inComment;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBInfoJudge objects.
 */
class ARBInfoJudgeList : public ARBVectorLoad1<ARBInfoJudge>
{
public:
	/**
	 * Sort the list by name.
	 * @param inDescending Sort in descending or ascending order.
	 */
	void sort(bool inDescending = true);

	/**
	 * Get the names of all the judges.
	 * @param outNames All the names.
	 * @return Number of judges in list.
	 */
	size_t GetAllJudges(std::set<std::string>& outNames) const;

	/**
	 * Remove entries from list that are in use but have no associated comments.
	 * @param inNamesInUse Names of judges from runs.
	 */
	void CondenseContent(std::set<std::string> const& inNamesInUse);

	/**
	 * Find a judge.
	 * @param inName Judge to find.
	 * @return Object matching judge.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoJudge* FindJudge(std::string const& inName) const;

	/**
	 * Add a new judge.
	 * @param inJudge Name of judge to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoJudge* AddJudge(std::string const& inJudge);

	/**
	 * Add a new judge.
	 * @param inJudge Judge to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBInfoJudge* AddJudge(ARBInfoJudge* inJudge);

	/**
	 * Delete a judge.
	 * @param inJudge Object to delete.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteJudge(ARBInfoJudge const* inJudge);
};
