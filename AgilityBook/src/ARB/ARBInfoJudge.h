#pragma once

/*
 * Copyright © 2003 David Connet. All Rights Reserved.
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
 *
 * @brief Comments about judges
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-07 DRC Created
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBInfoJudge : public ARBBase
{
public:
	ARBInfoJudge();
	ARBInfoJudge(const ARBInfoJudge& rhs);
	ARBInfoJudge& operator=(const ARBInfoJudge& rhs);
	bool operator==(const ARBInfoJudge& rhs) const;
	bool operator!=(const ARBInfoJudge& rhs) const;
	bool operator<(const ARBInfoJudge& rhs) const;
	bool operator>(const ARBInfoJudge& rhs) const;

	virtual std::string GetGenericName() const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	/**
	 * Judge's name
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	/**
	 * A comment...
	 */
	const std::string& GetComment() const;
	void SetComment(const std::string& inComment);

private:
	~ARBInfoJudge();
	std::string m_Name;
	std::string m_Comment;
};

inline bool ARBInfoJudge::operator<(const ARBInfoJudge& rhs) const
{
	return m_Name < rhs.GetName();
}

inline bool ARBInfoJudge::operator>(const ARBInfoJudge& rhs) const
{
	return m_Name > rhs.GetName();
}

inline const std::string& ARBInfoJudge::GetName() const
{
	return m_Name;
}

inline void ARBInfoJudge::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBInfoJudge::GetComment() const
{
	return m_Comment;
}

inline void ARBInfoJudge::SetComment(const std::string& inComment)
{
	m_Comment = inComment;
}

/////////////////////////////////////////////////////////////////////////////

class ARBInfoJudgeList : public ARBVectorLoad1<ARBInfoJudge>
{
public:
	bool operator==(const ARBInfoJudgeList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBInfoJudgeList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending = true);

	size_t GetAllJudges(std::set<std::string>& outNames) const;

	void CondenseContent(const std::set<std::string>& inNamesInUse);

	ARBInfoJudge* FindJudge(const std::string& inName) const;
	/**
	 * Add a new judge.
	 * This will fail if the judge already exists or the name is empty.
	 */
	ARBInfoJudge* AddJudge(const std::string& inJudge);
	bool DeleteJudge(const ARBInfoJudge* inJudge);
};
