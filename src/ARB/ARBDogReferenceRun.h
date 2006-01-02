#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBErrorCallback;
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
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load an existing point.
	 * @pre inTree is the actual ARBDogReferenceRun element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ARBConfig const& inConfig,
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

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
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);
	ARBString const& GetHeight() const;
	void SetHeight(ARBString const& inHeight);
	ARBString const& GetBreed() const;
	void SetBreed(ARBString const& inBreed);
	double GetTime() const;
	void SetTime(double inTime);
	ARBString const& GetScore() const;
	void SetScore(ARBString const& inScore);
	ARBString const& GetNote() const;
	void SetNote(ARBString const& inNote);

private:
	~ARBDogReferenceRun();
	ARB_Q m_Q;
	short m_Place;
	ARBString m_Name;
	ARBString m_Height;
	ARBString m_Breed;
	double m_Time;
	ARBString m_Score;
	ARBString m_Note;
};

inline ARBString ARBDogReferenceRun::GetGenericName() const
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

inline ARBString const& ARBDogReferenceRun::GetName() const
{
	return m_Name;
}

inline void ARBDogReferenceRun::SetName(ARBString const& inName)
{
	m_Name = inName;
}

inline ARBString const& ARBDogReferenceRun::GetHeight() const
{
	return m_Height;
}

inline void ARBDogReferenceRun::SetHeight(ARBString const& inHeight)
{
	m_Height = inHeight;
}

inline ARBString const& ARBDogReferenceRun::GetBreed() const
{
	return m_Breed;
}

inline void ARBDogReferenceRun::SetBreed(ARBString const& inBreed)
{
	m_Breed = inBreed;
}

inline double ARBDogReferenceRun::GetTime() const
{
	return m_Time;
}

inline void ARBDogReferenceRun::SetTime(double inTime)
{
	m_Time = inTime;
}

inline ARBString const& ARBDogReferenceRun::GetScore() const
{
	return m_Score;
}

inline void ARBDogReferenceRun::SetScore(ARBString const& inScore)
{
	m_Score = inScore;
}

inline ARBString const& ARBDogReferenceRun::GetNote() const
{
	return m_Note;
}

inline void ARBDogReferenceRun::SetNote(ARBString const& inNote)
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
	 * @return Whether the object was added.
	 */
	bool AddReferenceRun(ARBDogReferenceRun* inRef);

	/**
	 * Delete a reference run.
	 * @param inRef Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteReferenceRun(ARBDogReferenceRun const* inRef);
};
