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
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


/**
 * Pre-loaded Fault types.
 */
class ARB_API ARBConfigFault : public ARBBase
{
protected:
	ARBConfigFault();
	ARBConfigFault(ARBConfigFault const& rhs);
	ARBConfigFault(ARBConfigFault&& rhs);

public:
	~ARBConfigFault();
	static ARBConfigFaultPtr New();
	ARBConfigFaultPtr Clone() const;

	ARBConfigFault& operator=(ARBConfigFault const& rhs);
	ARBConfigFault& operator=(ARBConfigFault&& rhs);

	bool operator==(ARBConfigFault const& rhs) const;
	bool operator!=(ARBConfigFault const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		return m_Name;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override
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
			ElementNodePtr const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigFault element will be created in ioTree.
	 */
	bool Save(ElementNodePtr const& ioTree) const;

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
class ARB_API ARBConfigFaultList : public ARBVector<ARBConfigFaultPtr>
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
			ElementNodePtr const& inTree,
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
			ARBConfigFaultPtr* outFault = nullptr) const;

	/**
	 * Add a fault.
	 * @param inName Fault to add.
	 * @param outFault Pointer to object, NULL if name already exists or is empty.
	 * @return Whether the fault was added.
	 */
	bool AddFault(
			std::wstring const& inName,
			ARBConfigFaultPtr* outFault = nullptr);

	/**
	 * Delete the fault.
	 * @param inName Name of fault to delete.
	 * @return Whether fault was deleted.
	 */
	bool DeleteFault(std::wstring const& inName);
};
