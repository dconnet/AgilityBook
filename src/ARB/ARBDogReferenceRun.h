#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief ARBDogReferenceRun and ARBDogReferenceRunList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBVersion;
class Element;

/**
 * How did other dogs perform on the run we did?
 */
class ARBDogReferenceRun : public ARBBase
{
public:
	ARBDogReferenceRun();
	ARBDogReferenceRun(ARBDogReferenceRun const& rhs);
	ARBDogReferenceRun& operator=(ARBDogReferenceRun const& rhs);
	bool operator==(ARBDogReferenceRun const& rhs) const;
	bool operator!=(ARBDogReferenceRun const& rhs) const;

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
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogReferenceRun element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		Element const& inTree,
		ARBVersion const& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogReferenceRun element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Getters/setters.
	 */
	ARB_Q GetQ() const;
	void SetQ(ARB_Q inQ);
	short GetPlace() const;
	void SetPlace(short inPlace);
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetBreed() const;
	void SetBreed(std::string const& inBreed);
	ARBDouble GetTime() const;
	void SetTime(ARBDouble inTime);
	std::string const& GetScore() const;
	void SetScore(std::string const& inScore);
	std::string const& GetNote() const;
	void SetNote(std::string const& inNote);

private:
	~ARBDogReferenceRun();
	ARB_Q m_Q;
	short m_Place;
	std::string m_Name;
	std::string m_Breed;
	ARBDouble m_Time;
	std::string m_Score;
	std::string m_Note;
};

inline std::string ARBDogReferenceRun::GetGenericName() const
{
	return m_Name;
}

inline ARB_Q ARBDogReferenceRun::GetQ() const
{
	return m_Q;
}

inline void ARBDogReferenceRun::SetQ(ARB_Q inQ)
{
	m_Q = inQ;
}

inline short ARBDogReferenceRun::GetPlace() const
{
	return m_Place;
}

inline void ARBDogReferenceRun::SetPlace(short inPlace)
{
	m_Place = inPlace;
}

inline std::string const& ARBDogReferenceRun::GetName() const
{
	return m_Name;
}

inline void ARBDogReferenceRun::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBDogReferenceRun::GetBreed() const
{
	return m_Breed;
}

inline void ARBDogReferenceRun::SetBreed(std::string const& inBreed)
{
	m_Breed = inBreed;
}

inline ARBDouble ARBDogReferenceRun::GetTime() const
{
	return m_Time;
}

inline void ARBDogReferenceRun::SetTime(ARBDouble inTime)
{
	m_Time = inTime;
}

inline std::string const& ARBDogReferenceRun::GetScore() const
{
	return m_Score;
}

inline void ARBDogReferenceRun::SetScore(std::string const& inScore)
{
	m_Score = inScore;
}

inline std::string const& ARBDogReferenceRun::GetNote() const
{
	return m_Note;
}

inline void ARBDogReferenceRun::SetNote(std::string const& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogReferenceRun objects.
 */
class ARBDogReferenceRunList : public ARBVectorLoad2<ARBDogReferenceRun>
{
public:
	/**
	 * Add a reference run.
	 * @param inRef Object to add.
	 * @return Pointer to object.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBDogReferenceRun* AddReferenceRun(ARBDogReferenceRun* inRef);

	/**
	 * Delete a reference run.
	 * @param inRef Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteReferenceRun(ARBDogReferenceRun const* inRef);
};
