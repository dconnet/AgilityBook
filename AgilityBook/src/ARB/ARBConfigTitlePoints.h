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
 * @brief ARBConfigTitlePoints and ARBConfigTitlePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-03-31 DRC Started adding auto-lifetime point accumulation.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-07-12 DRC Added as part of file version 5.
 */

#include "ARBBase.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfigLifetimePointsList;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Number of title points that can be earned based on number of faults.
 * Some venues have a sliding scale, for instance a clean run in NADAC
 * is worth 10 pts, a run with up to 5 faults earns 5 pts.
 */
class ARBConfigTitlePoints : public ARBBase
{
public:
	ARBConfigTitlePoints();
	ARBConfigTitlePoints(short inPoints, short inFaults);
	ARBConfigTitlePoints(ARBConfigTitlePoints const& rhs);
	ARBConfigTitlePoints& operator=(ARBConfigTitlePoints const& rhs);
	bool operator==(ARBConfigTitlePoints const& rhs) const;
	bool operator!=(ARBConfigTitlePoints const& rhs) const;

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
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitlePoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param ioLifetimePoints Used for migrating older files.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			ARBConfigLifetimePointsList& ioLifetimePoints);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigTitlePoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Setters/getters.
	 */
	short GetPoints() const;
	void SetPoints(short inPoints);
	short GetFaults() const;
	/**
	 * @attention If faults is set via this API, the caller MUST call ARBConfigTitlePointsList::sort.
	 */
	void SetFaults(short inFaults);

private:
	~ARBConfigTitlePoints();
	short m_Points;
	short m_Faults;
};

inline short ARBConfigTitlePoints::GetPoints() const
{
	return m_Points;
}

inline void ARBConfigTitlePoints::SetPoints(short inPoints)
{
	m_Points = inPoints;
}

inline short ARBConfigTitlePoints::GetFaults() const
{
	return m_Faults;
}

inline void ARBConfigTitlePoints::SetFaults(short inFaults)
{
	m_Faults = inFaults;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigTitlePoints objects.
 */
class ARBConfigTitlePointsList : public ARBVectorLoad1<ARBConfigTitlePoints>
{
public:
	/**
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitlePoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @param ioLifetimePoints Used for migrating older files.
	 * @return Success
	 */
	bool Load(
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback,
			ARBConfigLifetimePointsList& ioLifetimePoints);

	/**
	 * Sort the title point objects by faults.
	 */
	void sort();

	/**
	 * Get the number of title points earned based on faults.
	 * @param inFaults Number of faults in the run.
	 * @return Number of titling points.
	 */
	short GetTitlePoints(double inFaults) const;

	/**
	 * Find a points object.
	 * @param inFaults Number of faults to find.
	 * @param outPoints Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	bool FindTitlePoints(
			short inFaults,
			ARBConfigTitlePoints** outPoints = NULL) const;

	/**
	 * Add an object.
	 * @param inPoints Number of title points.
	 * @param inFaults Number of faults.
	 * @param outPoints Pointer to new object, NULL if it already exists.
	 * @return Whether the object was added.
	 */
	bool AddTitlePoints(
			short inPoints,
			short inFaults,
			ARBConfigTitlePoints** outPoints = NULL);

	/**
	 * Delete an object.
	 * @param inFaults Delete object with the given faults.
	 * @return Whether object was deleted.
	 */
	bool DeleteTitlePoints(short inFaults);
};
