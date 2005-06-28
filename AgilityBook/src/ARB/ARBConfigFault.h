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
 * @brief ARBConfigFault and ARBConfigFaultList class.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
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
 * Pre-loaded Fault types.
 */
class ARBConfigFault : public ARBBase
{
public:
	ARBConfigFault();
	ARBConfigFault(ARBConfigFault const& rhs);
	ARBConfigFault& operator=(ARBConfigFault const& rhs);
	bool operator==(ARBConfigFault const& rhs) const;
	bool operator!=(ARBConfigFault const& rhs) const;

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
	 * Load a fault.
	 * @pre inTree is the actual ARBConfigFault element.
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
	 * @post The ARBConfigFault element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::string const& GetName() const;
	void SetName(std::string const& inName);

private:
	~ARBConfigFault();
	std::string m_Name;
};

inline std::string ARBConfigFault::GetGenericName() const
{
	return m_Name;
}

inline std::string const& ARBConfigFault::GetName() const
{
	return m_Name;
}

inline void ARBConfigFault::SetName(std::string const& inName)
{
	m_Name = inName;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBConfigFault objects.
 */
class ARBConfigFaultList : public ARBVectorLoad1<ARBConfigFault>
{
public:
	/**
	 * Find a fault.
	 * @param inName Name of fault to find.
	 * @param outFault Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindFault(
			std::string const& inName,
			ARBConfigFault** outFault = NULL) const;

	/**
	 * Add a fault.
	 * @param inName Fault to add.
	 * @param outFault Pointer to object, NULL if name already exists or is empty.
	 * @return Whether the fault was added.
	 */
	bool AddFault(
			std::string const& inName,
			ARBConfigFault** outFault = NULL);

	/**
	 * Delete the fault.
	 * @param inName Name of fault to delete.
	 * @return Whether fault was deleted.
	 */
	bool DeleteFault(std::string const& inName);
};
