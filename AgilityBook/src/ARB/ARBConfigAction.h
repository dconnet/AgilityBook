#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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
 * @li 2005-10-26 DRC Added optional 'Div' to DeleteTitle.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-01-21 DRC Created
 */

#include "ARBBase.h"
#include "ARBTypes.h"

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
protected:
	ARBConfigAction();
	ARBConfigAction(ARBConfigAction const& rhs);

public:
	~ARBConfigAction();
	static ARBConfigActionPtr New();
	ARBConfigActionPtr Clone() const;

	ARBConfigAction& operator=(ARBConfigAction const& rhs);

	bool operator==(ARBConfigAction const& rhs) const;
	bool operator!=(ARBConfigAction const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 * @note There are no strings to search in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

	/**
	 * Load a calendar entry
	 * @pre inTree is the actual ARBConfigAction element.
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
	 * @post The ARBConfigAction element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters.
	 */
	ARBString const& GetVerb() const;
	ARBString const& GetVenue() const;
	ARBString const& GetDivision() const;
	ARBString const& GetOldName() const;
	ARBString const& GetNewName() const;

private:
	ARBString m_Verb;
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_OldName;
	ARBString m_NewName;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigAction objects.
 */
class ARBConfigActionList : public ARBVector<ARBConfigActionPtr>
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
};
