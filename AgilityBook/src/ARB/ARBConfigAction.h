#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * @brief ARBConfigAction class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-01-21 DRC Created
 */

#include <string>
#include "ARBBase.h"
#include "ARBVector.h"
class ARBVersion;
class Element;

/**
 * Special class that performs actions during ARBConfig::Update.
 *
 * This class is not created anywhere (hence, there are only getters, no
 * setters). During document revisions, it is sometimes necessary to do
 * additional work - hence when we create the default configuration, we
 * must create these entries in that by hand. As of file version 8.2, the
 * default configuration now has some entries.
 *
 * During the update process, all actions are run in order. The configuration
 * is then updated.
 *
 * Currently, the only action supported is renaming an existing title.
 */
class ARBConfigAction : public ARBBase
{
public:
	ARBConfigAction();
	ARBConfigAction(const ARBConfigAction& rhs);
	ARBConfigAction& operator=(const ARBConfigAction& rhs);
	bool operator==(const ARBConfigAction& rhs) const;
	bool operator!=(const ARBConfigAction& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual std::string GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	/**
	 * Load a calendar entry
	 * @pre inTree is the actual ARBConfigAction element.
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
	 * @post The ARBConfigAction element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters.
	 */
	const std::string& GetVerb() const;
	const std::string& GetVenue() const;
	const std::string& GetOldName() const;
	const std::string& GetNewName() const;

private:
	~ARBConfigAction();
	std::string m_Verb;
	std::string m_Venue;
	std::string m_OldName;
	std::string m_NewName;
};

inline std::string ARBConfigAction::GetGenericName() const
{
	return m_Verb;
}

inline const std::string& ARBConfigAction::GetVerb() const
{
	return m_Verb;
}

inline const std::string& ARBConfigAction::GetVenue() const
{
	return m_Venue;
}

inline const std::string& ARBConfigAction::GetOldName() const
{
	return m_OldName;
}

inline const std::string& ARBConfigAction::GetNewName() const
{
	return m_NewName;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigAction objects.
 */
class ARBConfigActionList : public ARBVectorLoad1<ARBConfigAction>
{
public:
	bool operator==(const ARBConfigActionList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigActionList& rhs) const
	{
		return !isEqual(rhs);
	}
};
