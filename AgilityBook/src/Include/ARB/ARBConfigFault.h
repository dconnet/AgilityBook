#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigFault and ARBConfigFaultList class.
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARB/ARBBase.h"
#include "ARB/ARBTypes2.h"


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
	virtual std::wstring GetGenericName() const
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const
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
	std::wstring const& GetName() const
	{
		return m_Name;
	}
	void SetName(std::wstring const& inName)
	{
		m_Name = inName;
	}

private:
	std::wstring m_Name;
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
			std::wstring const& inName,
			ARBConfigFaultPtr* outFault = NULL) const;

	/**
	 * Add a fault.
	 * @param inName Fault to add.
	 * @param outFault Pointer to object, NULL if name already exists or is empty.
	 * @return Whether the fault was added.
	 */
	bool AddFault(
			std::wstring const& inName,
			ARBConfigFaultPtr* outFault = NULL);

	/**
	 * Delete the fault.
	 * @param inName Name of fault to delete.
	 * @return Whether fault was deleted.
	 */
	bool DeleteFault(std::wstring const& inName);
};
