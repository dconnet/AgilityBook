#pragma once

/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * @brief ARBConfigLifetimePoints and ARBConfigLifetimePointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-10-06 DRC Added as part of file version 10.
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Number of lifetime points that can be earned based on number of faults.
 * Some venues have a sliding scale, for instance a clean run in NADAC
 * is worth 10 pts, a run with up to 5 faults earns 5 pts.
 *
 * This class works very much like ARBConfigTitlePoints. (In fact, lifetime
 * points used to be titling points until it was discovered that some venues
 * award different values [CPE].)
 */
class ARBConfigLifetimePoints : public ARBBase
{
public:
	ARBConfigLifetimePoints();
	ARBConfigLifetimePoints(short inPoints, short inFaults);
	ARBConfigLifetimePoints(ARBConfigLifetimePoints const& rhs);
	ARBConfigLifetimePoints& operator=(ARBConfigLifetimePoints const& rhs);
	bool operator==(ARBConfigLifetimePoints const& rhs) const;
	bool operator!=(ARBConfigLifetimePoints const& rhs) const;

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
	 * Load a lifetime configuration.
	 * @pre inTree is the actual ARBConfigLifetimePoints element.
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
	 * @post The ARBConfigLifetimePoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Setters/getters.
	 */
	short GetPoints() const;
	void SetPoints(short inPoints);
	short GetFaults() const;
	/**
	 * @attention If faults is set via this API, the caller MUST call ARBConfigLifetimePointsList::sort.
	 */
	void SetFaults(short inFaults);

private:
	~ARBConfigLifetimePoints();
	short m_Points;
	short m_Faults;
};

inline short ARBConfigLifetimePoints::GetPoints() const
{
	return m_Points;
}

inline void ARBConfigLifetimePoints::SetPoints(short inPoints)
{
	m_Points = inPoints;
}

inline short ARBConfigLifetimePoints::GetFaults() const
{
	return m_Faults;
}

inline void ARBConfigLifetimePoints::SetFaults(short inFaults)
{
	m_Faults = inFaults;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigLifetimePoints objects.
 */
class ARBConfigLifetimePointsList : public ARBVectorLoad1<ARBConfigLifetimePoints>
{
public:
	/**
	 * Sort the lifetime point objects by faults.
	 */
	void sort();

	/**
	 * Get the number of lifetime title points earned based on faults.
	 * @param inFaults Number of faults in the run.
	 * @return Number of lifetime titling points.
	 */
	short GetLifetimePoints(double inFaults) const;

	/**
	 * Find a points object.
	 * @param inFaults Number of faults to find.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigLifetimePoints const* FindLifetimePoints(short inFaults) const;

	/**
	 * Add an object.
	 * @param inPoints Number of lifetime points.
	 * @param inFaults Number of faults.
	 * @return Pointer to new object, NULL if it already exists.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigLifetimePoints* AddLifetimePoints(short inPoints, short inFaults);

	/**
	 * Delete an object.
	 * @param inFaults Delete object with the given faults.
	 * @return Whether object was deleted.
	 */
	bool DeleteLifetimePoints(short inFaults);
};
