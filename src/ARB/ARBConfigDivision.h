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
 * @brief ARBConfigDivision and ARBConfigDivisionList class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-21 DRC Added DeleteTitle.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigLevel.h"
#include "ARBConfigTitle.h"
#include "ARBVector.h"
class ARBConfigEventList;
class ARBConfigVenue;
class ARBVersion;
class Element;

/**
 * Division configuration information.
 */
class ARBConfigDivision : public ARBBase
{
public:
	ARBConfigDivision();
	ARBConfigDivision(const ARBConfigDivision& rhs);
	ARBConfigDivision& operator=(const ARBConfigDivision& rhs);
	bool operator==(const ARBConfigDivision& rhs) const;
	bool operator!=(const ARBConfigDivision& rhs) const;

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
	 * Load a division configuration.
	 * @pre inTree is the actual ARBConfigDivision element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigDivision element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Update this configuration from inDivNew.
	 * @param indent Indentation level for generating messages.
	 * @param inDivNew Division to merge.
	 * @param ioInfo Accumulated messages about changes that have happened.
	 * @return Whether or not changes have occurred.
	 */
	bool Update(int indent, const ARBConfigDivision* inDivNew, std::string& ioInfo);

	/*
	 * Getters/setters.
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const ARBConfigLevelList& GetLevels() const;
	ARBConfigLevelList& GetLevels();
	const ARBConfigTitleList& GetTitles() const;
	ARBConfigTitleList& GetTitles();

private:
	~ARBConfigDivision();
	std::string m_Name;
	ARBConfigLevelList m_Levels;
	ARBConfigTitleList m_Titles;
};

inline std::string ARBConfigDivision::GetGenericName() const
{
	return m_Name;
}

inline const std::string& ARBConfigDivision::GetName() const
{
	return m_Name;
}

inline void ARBConfigDivision::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const ARBConfigLevelList& ARBConfigDivision::GetLevels() const
{
	return m_Levels;
}

inline ARBConfigLevelList& ARBConfigDivision::GetLevels()
{
	return m_Levels;
}

inline const ARBConfigTitleList& ARBConfigDivision::GetTitles() const
{
	return m_Titles;
}

inline ARBConfigTitleList& ARBConfigDivision::GetTitles()
{
	return m_Titles;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigDivision objects.
 */
class ARBConfigDivisionList : public ARBVector<ARBConfigDivision>
{
public:
	/**
	 * Load a division configuration.
	 * @pre inTree is the actual ARBConfigDivision element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		const Element& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBConfigDivisionList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigDivisionList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Verify a level exists.
	 * @param inDiv Division level exists in.
	 * @param inLevel Level to verify.
	 * @return Level exists.
	 */
	bool VerifyLevel(
		const std::string& inDiv,
		const std::string& inLevel) const;

	/**
	 * Find the named division.
	 * @param inDiv Division to find.
	 * @return Object that was found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigDivision* FindDivision(const std::string& inDiv) const;
	ARBConfigDivision* FindDivision(const std::string& inDiv);

	/**
	 * Add a division.
	 * @param inDiv Name of division to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigDivision* AddDivision(const std::string& inDiv);

	/**
	 * Add a division.
	 * @param inDiv Division to add.
	 * @return Pointer to object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 *       The pointer is added to the list and its ref count is incremented.
	 */
	ARBConfigDivision* AddDivision(ARBConfigDivision* inDiv);

	/**
	 * Delete the division.
	 * @param inDiv Name of division to delete.
	 * @param ioEvents List of events to be updated.
	 * @return Number of divisions deleted (0 or 1).
	 */
	int DeleteDivision(
		const std::string& inDiv,
		ARBConfigEventList& ioEvents);

	/**
	 * Find a title by the complete name.
	 * This api is used to fix a problem introduced in v1.0.0.8.
	 * @param inName Complete name of title to find.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigTitle* FindTitleCompleteName(const std::string& inName, bool bAbbrevFirst = true) const;

	/**
	 * Find a title.
	 * @param inTitle Name of title to find.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigTitle* FindTitle(const std::string& inTitle) const;
	ARBConfigTitle* FindTitle(const std::string& inTitle);

	/**
	 * Delete a title.
	 * @param inTitle Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(const std::string& inTitle);
};
