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
 * @brief ARBConfigOtherPoints and ARBConfigOtherPointsList class.
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

protected:
	ARBConfigOtherPoints();
	ARBConfigOtherPoints(ARBConfigOtherPoints const& rhs);

public:
	~ARBConfigOtherPoints();
	static ARBConfigOtherPointsPtr New();
	ARBConfigOtherPointsPtr Clone() const;

	ARBConfigOtherPoints& operator=(ARBConfigOtherPoints const& rhs);

	bool operator==(ARBConfigOtherPoints const& rhs) const;
	bool operator!=(ARBConfigOtherPoints const& rhs) const;

	/**
	 * Reset the contents of this object and all sub-objects.
	 */
	void clear();

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
	 * Load an otherpoint configuration.
	 * @pre inTree is the actual ARBConfigOtherPoints element.
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
	 * @post The ARBConfigOtherPoints element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBString const& GetName() const;
	void SetName(ARBString const& inName);
	ARBString const& GetDescription() const;
	void SetDescription(ARBString const& inDesc);
	eOtherPointsTally GetTally() const;
	void SetTally(eOtherPointsTally inTally);

private:
	ARBString m_Name;
	eOtherPointsTally m_Tally;
	ARBString m_Desc;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigOtherPoints objects.
 */
class ARBConfigOtherPointsList : public ARBVector<ARBConfigOtherPointsPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
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
	 * Verify the name exists.
	 * @param inName Name to verify.
	 * @return Whether the OtherPoints exists.
	 */
	bool VerifyOtherPoints(ARBString const& inName) const;

	/**
	 * Find an otherpoints object.
	 * @param inName Name of object to find.
	 * @param outPoints Pointer to object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindOtherPoints(
			ARBString const& inName,
			ARBConfigOtherPointsPtr* outPoints = NULL) const;

	/**
	 * Add an otherpoints object.
	 * @param inOther Name of OtherPoints to add.
	 * @return Whether the object was added.
	 */
	bool AddOtherPoints(ARBConfigOtherPointsPtr inOther);

	/**
	 * Delete an object.
	 * @param inName Name of object to delete.
	 * @return Whether the object was deleted or not.
	 */
	bool DeleteOtherPoints(ARBString const& inName);
};
