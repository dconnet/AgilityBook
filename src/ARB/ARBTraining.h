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
 * @brief The training logbook
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

	virtual std::string GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	/**
	 * Training date
	 */
	const ARBDate& GetDate() const;
	void SetDate(const ARBDate& inDate);
	/**
	 * A name (trainer, etc)
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	/**
	 * A note...
	 */
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

	void sort(bool inDescending = true);

	size_t GetAllNames(std::set<std::string>& outNames) const;

	const ARBTraining* FindTraining(const ARBTraining* inTraining) const;

	ARBTraining* AddTraining(ARBTraining* inTraining);
	bool DeleteTraining(const ARBTraining* inTraining);
};
