#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include <set>
#include <vector>
class ARBConfig;
class ARBVersion;
class CElement;

typedef std::vector<std::string> ARBDogFaultList;

class ARBDogNotes
{
public:
	ARBDogNotes();
	ARBDogNotes(const ARBDogNotes& rhs);
	~ARBDogNotes();
	ARBDogNotes& operator=(const ARBDogNotes& rhs);
	bool operator==(const ARBDogNotes& rhs) const;
	bool operator!=(const ARBDogNotes& rhs) const;

	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	const ARBDogFaultList& GetFaults() const;
	ARBDogFaultList& GetFaults();
	const std::string& GetCRCD() const;
	void SetCRCD(const std::string& inCRCD);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);

private:
	ARBDogFaultList m_Faults;
	std::string m_CRCD;
	std::string m_Note;
};

inline const ARBDogFaultList& ARBDogNotes::GetFaults() const
{
	return m_Faults;
}

inline ARBDogFaultList& ARBDogNotes::GetFaults()
{
	return m_Faults;
}

inline const std::string& ARBDogNotes::GetCRCD() const
{
	return m_CRCD;
}

inline void ARBDogNotes::SetCRCD(const std::string& inCRCD)
{
	m_CRCD = inCRCD;
}

inline const std::string& ARBDogNotes::GetNote() const
{
	return m_Note;
}

inline void ARBDogNotes::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}
