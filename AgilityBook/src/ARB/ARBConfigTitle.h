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
 * @brief ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-03-11 DRC Added 'Requires', moved desc to 'Desc'.
 * @li 2004-01-05 DRC Added LongName.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <set>
#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

typedef std::set<std::string> ARBConfigTitleRequiresList;

/**
 * Title configuration.
 * A title consists of a name (the abbreviation), a long name and a description.
 */
class ARBConfigTitle : public ARBBase
{
public:
	ARBConfigTitle();
	ARBConfigTitle(const ARBConfigTitle& rhs);
	ARBConfigTitle& operator=(const ARBConfigTitle& rhs);
	bool operator==(const ARBConfigTitle& rhs) const;
	bool operator!=(const ARBConfigTitle& rhs) const;

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
	 * Load a title configuration.
	 * @pre inTree is the actual ARBConfigTitle element.
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
	 * @post The ARBConfigTitle element will be created in ioTree.
	 */
	bool Save(CElement& ioTree) const;

	/**
	 * Get the nice (long) name.
	 * @return the nice (long) name.
	 */
	const std::string& GetNiceName() const;

	/**
	 * Get the complete name (name + nicename).
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @return The complete name.
	 */
	std::string GetCompleteName(bool bAbbrevFirst = true) const;

	/*
	 * Getters/setters.
	 */
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetLongName() const;
	void SetLongName(const std::string& inName);
	const std::string& GetDescription() const;
	void SetDescription(const std::string& inDesc);
	const ARBConfigTitleRequiresList& GetRequires() const;
	ARBConfigTitleRequiresList& GetRequires();

private:
	~ARBConfigTitle();
	std::string m_Name;
	std::string m_LongName;
	std::string m_Desc;
	ARBConfigTitleRequiresList m_Requires;
};

inline std::string ARBConfigTitle::GetGenericName() const
{
	return GetNiceName();
}

inline const std::string& ARBConfigTitle::GetNiceName() const
{
	if (0 == m_LongName.length())
		return m_Name;
	else
		return m_LongName;
}

inline const std::string& ARBConfigTitle::GetName() const
{
	return m_Name;
}

inline void ARBConfigTitle::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBConfigTitle::GetLongName() const
{
	return m_LongName;
}

inline void ARBConfigTitle::SetLongName(const std::string& inName)
{
	m_LongName = inName;
}

inline const std::string& ARBConfigTitle::GetDescription() const
{
	return m_Desc;
}

inline void ARBConfigTitle::SetDescription(const std::string& inDesc)
{
	m_Desc = inDesc;
}

inline const ARBConfigTitleRequiresList& ARBConfigTitle::GetRequires() const
{
	return m_Requires;
}

inline ARBConfigTitleRequiresList& ARBConfigTitle::GetRequires()
{
	return m_Requires;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigTitle objects.
 */
class ARBConfigTitleList : public ARBVectorLoad1<ARBConfigTitle>
{
public:
	bool operator==(const ARBConfigTitleList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigTitleList& rhs) const
	{
		return !isEqual(rhs);
	}

	/**
	 * Verify all titles Requires items are within this list
	 * (they are within a division)
	 * @post Any required items not in the title list will be removed.
	 */
	bool VerifyRequiresLists();

	/**
	 * Determine whether a title requires the specified name.
	 * This searches thru all the linkings.
	 * @param inTitle Title to check.
	 * @param inName Name of requirement to check.
	 */
	bool Requires(const ARBConfigTitle* inTitle, const std::string& inName) const;

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
	 * @param inName Name of title to find.
	 * @return Pointer to found object, NULL if not found.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	const ARBConfigTitle* FindTitle(const std::string& inName) const;
	ARBConfigTitle* FindTitle(const std::string& inName);

	/**
	 * Add a title.
	 * @param inName Name of title to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigTitle* AddTitle(const std::string& inName);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Pointer to new object, NULL if name already exists or is empty.
	 * @post Returned pointer is not ref counted, do <b><i>not</i></b> release.
	 */
	ARBConfigTitle* AddTitle(ARBConfigTitle* inTitle);

	/**
	 * Rename a title.
	 * @param inOldName Name of title to rename.
	 * @param inNewName New name of title.
	 * @return Number of titles changed.
	 */
	int RenameTitle(const std::string& inOldName, const std::string& inNewName);

	/**
	 * Delete a title.
	 * @param inName Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(const std::string& inName);
};
