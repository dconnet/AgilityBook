#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @brief ARBConfigFault and ARBConfigFaultList class.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes.h"

/**
 * Pre-loaded Fault types.
 */
class ARBConfigFault : public ARBBase
{
protected:
	ARBConfigFault();
	ARBConfigFault(ARBConfigFault const& rhs);

public:
	~ARBConfigFault();
	static ARBConfigFaultPtr New();
	ARBConfigFaultPtr Clone() const;

	ARBConfigFault& operator=(ARBConfigFault const& rhs);

	bool operator==(ARBConfigFault const& rhs) const;
	bool operator!=(ARBConfigFault const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual tstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<tstring>& ioStrings) const
	{
		return 0;
	}

	/**
	 * Load a fault.
	 * @pre inTree is the actual ARBConfigFault element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigFault element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/*
	 * Getters/setters.
	 */
	tstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(tstring const& inName)
	{
		m_Name = inName;
	}

private:
	tstring m_Name;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigFault objects.
 */
class ARBConfigFaultList : public ARBVector<ARBConfigFaultPtr>
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
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Find a fault.
	 * @param inName Name of fault to find.
	 * @param outFault Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindFault(
			tstring const& inName,
			ARBConfigFaultPtr* outFault = NULL) const;

	/**
	 * Add a fault.
	 * @param inName Fault to add.
	 * @param outFault Pointer to object, NULL if name already exists or is empty.
	 * @return Whether the fault was added.
	 */
	bool AddFault(
			tstring const& inName,
			ARBConfigFaultPtr* outFault = NULL);

	/**
	 * Delete the fault.
	 * @param inName Name of fault to delete.
	 * @return Whether fault was deleted.
	 */
	bool DeleteFault(tstring const& inName);
};
