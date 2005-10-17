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
 * @brief ARBConfigTitle and ARBConfigTitleList class.
 * @author David Connet
 *
 * Revision History
 * @li 2005-10-14 DRC Added option to prefix a title.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-11 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-05 DRC Added LongName.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Title configuration.
 * A title consists of a name (the abbreviation), a long name and a description.
 */
class ARBConfigTitle : public ARBBase
{
public:
	ARBConfigTitle();
	ARBConfigTitle(ARBConfigTitle const& rhs);
	ARBConfigTitle& operator=(ARBConfigTitle const& rhs);
	bool operator==(ARBConfigTitle const& rhs) const;
	bool operator!=(ARBConfigTitle const& rhs) const;

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
	 * @post The ARBConfigTitle element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Get the nice (long) name.
	 * @return the nice (long) name.
	 */
	std::string const& GetNiceName() const;

	/**
	 * Get the complete name (name + nicename).
	 * @param inInstance Instance of the title to allow for multiple.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @return The complete name.
	 */
	std::string GetCompleteName(
			short inInstance = 0,
			bool bAbbrevFirst = true) const;

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);
	std::string const& GetLongName() const;
	void SetLongName(std::string const& inName);
	short GetMultiple() const;
	void SetMultiple(short inMultiple);
	bool GetPrefix() const;
	void SetPrefix(bool inPrefix);
	std::string const& GetDescription() const;
	void SetDescription(std::string const& inDesc);

private:
	~ARBConfigTitle();
	std::string m_Name;
	std::string m_LongName;
	short m_Multiple;
	bool m_Prefix;
	std::string m_Desc;
};

inline std::string ARBConfigTitle::GetGenericName() const
{
	return GetNiceName();
}

inline std::string const& ARBConfigTitle::GetNiceName() const
{
	if (0 == m_LongName.length())
		return m_Name;
	else
		return m_LongName;
}

inline std::string const& ARBConfigTitle::GetName() const
{
	return m_Name;
}

inline void ARBConfigTitle::SetName(std::string const& inName)
{
	m_Name = inName;
}

inline std::string const& ARBConfigTitle::GetLongName() const
{
	return m_LongName;
}

inline void ARBConfigTitle::SetLongName(std::string const& inName)
{
	m_LongName = inName;
}

inline short ARBConfigTitle::GetMultiple() const
{
	return m_Multiple;
}

inline void ARBConfigTitle::SetMultiple(short inMultiple)
{
	m_Multiple = inMultiple;
}

inline bool ARBConfigTitle::GetPrefix() const
{
	return m_Prefix;
}

inline void ARBConfigTitle::SetPrefix(bool inPrefix)
{
	m_Prefix = inPrefix;
}

inline std::string const& ARBConfigTitle::GetDescription() const
{
	return m_Desc;
}

inline void ARBConfigTitle::SetDescription(std::string const& inDesc)
{
	m_Desc = inDesc;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigTitle objects.
 */
class ARBConfigTitleList : public ARBVectorLoad1<ARBConfigTitle>
{
public:
	/**
	 * Find a title by the complete name.
	 * This api is used to fix a problem introduced in v1.0.0.8.
	 * @param inName Complete name of title to find.
	 * @param inInstance Instance of the title to allow for multiple.
	 * @param bAbbrevFirst Name is before or after Longname.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitleCompleteName(
			std::string const& inName,
			short inInstance,
			bool bAbbrevFirst = true,
			ARBConfigTitle** outTitle = NULL) const;

	/**
	 * Find a title.
	 * @param inName Name of title to find.
	 * @param outTitle Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindTitle(
			std::string const& inName,
			ARBConfigTitle** outTitle = NULL) const;

	/**
	 * Add a title.
	 * @param inName Name of title to add.
	 * @param outTitle Pointer to new object, NULL if name already exists or is empty.
	 * @return Whether the object was added.
	 */
	bool AddTitle(
			std::string const& inName,
			ARBConfigTitle** outTitle = NULL);

	/**
	 * Add a title.
	 * @param inTitle Title to add.
	 * @return Whether the object was added.
	 */
	bool AddTitle(ARBConfigTitle* inTitle);

	/**
	 * Delete a title.
	 * @param inName Name of title to delete.
	 * @return Whether title was deleted.
	 */
	bool DeleteTitle(std::string const& inName);
};
