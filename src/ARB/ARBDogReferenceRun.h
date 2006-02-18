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
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * How did other dogs perform on the run we did?
 */
class ARBDogReferenceRun : public ARBBase
{
protected:
	ARBDogReferenceRun();
	ARBDogReferenceRun(ARBDogReferenceRun const& rhs);

public:
	~ARBDogReferenceRun();
	static ARBDogReferenceRunPtr New()
	{
		return ARBDogReferenceRunPtr(new ARBDogReferenceRun());
	}
	ARBDogReferenceRunPtr Clone() const
	{
		return ARBDogReferenceRunPtr(new ARBDogReferenceRun(*this));
	}

	ARBDogReferenceRun& operator=(ARBDogReferenceRun const& rhs);

	bool operator==(ARBDogReferenceRun const& rhs) const;
	bool operator!=(ARBDogReferenceRun const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const
	{
		return m_Name;
	}

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
	ARB_Q GetQ() const
	{
		return m_Q;
	}
	void SetQ(ARB_Q inQ)
	{
		m_Q = inQ;
	}
	short GetPlace() const
	{
		return m_Place;
	}
	void SetPlace(short inPlace)
	{
		m_Place = inPlace;
	}
	ARBString const& GetName() const
	{
		return m_Name;
	}
	void SetName(ARBString const& inName)
	{
		m_Name = inName;
	}
	ARBString const& GetHeight() const
	{
		return m_Height;
	}
	void SetHeight(ARBString const& inHeight)
	{
		m_Height = inHeight;
	}
	ARBString const& GetBreed() const
	{
		return m_Breed;
	}
	void SetBreed(ARBString const& inBreed)
	{
		m_Breed = inBreed;
	}
	double GetTime() const
	{
		return m_Time;
	}
	void SetTime(double inTime)
	{
		m_Time = inTime;
	}
	ARBString const& GetScore() const
	{
		return m_Score;
	}
	void SetScore(ARBString const& inScore)
	{
		m_Score = inScore;
	}
	ARBString const& GetNote() const
	{
		return m_Note;
	}
	void SetNote(ARBString const& inNote)
	{
		m_Note = inNote;
	}

private:
	ARB_Q m_Q;
	short m_Place;
	ARBString m_Name;
	ARBString m_Height;
	ARBString m_Breed;
	double m_Time;
	ARBString m_Score;
	ARBString m_Note;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogReferenceRun objects.
 */
class ARBDogReferenceRunList : public ARBVector<ARBDogReferenceRunPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
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
	 * Add a reference run.
	 * @param inRef Object to add.
	 * @return Whether the object was added.
	 */
	bool AddReferenceRun(ARBDogReferenceRunPtr inRef);

	/**
	 * Delete a reference run.
	 * @param inRef Object to delete.
	 * @return Whether object was deleted.
	 * @note Equality is tested by value, not pointer.
	 */
	bool DeleteReferenceRun(ARBDogReferenceRunPtr inRef);
};
