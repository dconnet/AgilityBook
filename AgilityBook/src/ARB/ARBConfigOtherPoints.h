#pragma once

/*
 * Copyright � 2002-2004 David Connet. All Rights Reserved.
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
 * @brief ARBConfigOtherPoints and ARBConfigOtherPointsList class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

/**
 * A way to tally points that aren't inheritantly known.
 */
class ARBConfigOtherPoints : public ARBBase
{
public:
	/**
	 * How to accumulate points.
	 */
	typedef enum
	{
		eTallyAll,			///< Accumulate all runs together.
		eTallyAllByEvent,	///< Separate runs by event.
		eTallyLevel,		///< Separate runs by level.
		eTallyLevelByEvent	///< Separate runs by event and level.
	} eOtherPointsTally;

	ARBConfigOtherPoints();
	ARBConfigOtherPoints(const ARBConfigOtherPoints& rhs);
	ARBConfigOtherPoints& operator=(const ARBConfigOtherPoints& rhs);
	bool operator==(const ARBConfigOtherPoints& rhs) const;
	bool operator!=(const ARBConfigOtherPoints& rhs) const;

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

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
	 * Load an otherpoint configuration.
	 * @pre inTree is the actual ARBConfigOtherPoints element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/*
	 * Getters/setters.
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetDescription() const;
	void SetDescription(const std::string& inDesc);
	eOtherPointsTally GetTally() const;
	void SetTally(eOtherPointsTally inTally);

private:
	~ARBConfigOtherPoints();
	std::string m_Name;
	eOtherPointsTally m_Tally;
	std::string m_Desc;
};

inline std::string ARBConfigOtherPoints::GetGenericName() const
{
	return GetName();
}

inline const std::string& ARBConfigOtherPoints::GetName() const
{
	return m_Name;
}

inline void ARBConfigOtherPoints::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBConfigOtherPoints::GetDescription() const
{
	return m_Desc;
}

inline void ARBConfigOtherPoints::SetDescription(const std::string& inDesc)
{
	m_Desc = inDesc;
}

inline ARBConfigOtherPoints::eOtherPointsTally ARBConfigOtherPoints::GetTally() const
{
	return m_Tally;
}

inline void ARBConfigOtherPoints::SetTally(ARBConfigOtherPoints::eOtherPointsTally inTally)
{
	m_Tally = inTally;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigOtherPoints objects.
 */
class ARBConfigOtherPointsList : public ARBVectorLoad1<ARBConfigOtherPoints>
{
public:
	bool operator==(const ARBConfigOtherPointsList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigOtherPointsList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Verify the name exists.
	 * @param inName Name to verify.
	 * @return Whether the OtherPoints exists.
	 */
	bool VerifyOtherPoints(const std::string& inName) const;

	/**
	 * Find an otherpoints object.
	 * @param inName Name of object to find.
	 * @return Pointer to object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigOtherPoints* FindOtherPoints(const std::string& inName) const;
	ARBConfigOtherPoints* FindOtherPoints(const std::string& inName);

	/**
	 * Add an otherpoints object.
	 * @param inOther Name of OtherPoints to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBConfigOtherPoints* AddOtherPoints(ARBConfigOtherPoints* inOther);

	/**
	 * Delete an object.
	 * @param inName Name of object to delete.
	 * @return Whether the object was deleted or not.
	 */
	bool DeleteOtherPoints(const std::string& inName);
};
