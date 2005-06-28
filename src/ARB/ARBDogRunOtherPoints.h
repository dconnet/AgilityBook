#pragma once

/*
 * Copyright © 2002-2005 David Connet. All Rights Reserved.
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
 * @brief ARBDogRunOtherPoints and ARBDogRunOtherPointsList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Other points are accumulated on a per-run basis.
 */
class ARBDogRunOtherPoints : public ARBBase
{
public:
	ARBDogRunOtherPoints();
	ARBDogRunOtherPoints(ARBDogRunOtherPoints const& rhs);
	ARBDogRunOtherPoints& operator=(ARBDogRunOtherPoints const& rhs);
	bool operator==(ARBDogRunOtherPoints const& rhs) const;
	bool operator!=(ARBDogRunOtherPoints const& rhs) const;

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
	 * Load an other point.
	 * @pre inTree is the actual ARBDogOtherPoints element.
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
	 * @post The ARBDogOtherPoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	short GetPoints() const;
	void SetPoints(short inPts);

private:
	~ARBDogRunOtherPoints();
	std::string m_Name;
	short m_Points;
};

inline std::string ARBDogRunOtherPoints::GetGenericName() const
{
	return m_Name;
}

inline std::string const& ARBDogRunOtherPoints::GetName() const
{
	return m_Name;
}

inline void ARBDogRunOtherPoints::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline short ARBDogRunOtherPoints::GetPoints() const
{
	return m_Points;
}

inline void ARBDogRunOtherPoints::SetPoints(short inPts)
{
	m_Points = inPts;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRunOtherPoints objects.
 */
class ARBDogRunOtherPointsList : public ARBVectorLoad2<ARBDogRunOtherPoints>
{
public:
	/**
	 * Add an other point.
	 * @param inOther OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBDogRunOtherPoints* inOther);
};
