#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBConfigLifetimeName and ARBConfigLifetimeNameList class.
 * @author David Connet
 *
 * Revision History
 * 2016-06-19 Add delete method
 * 2016-04-29 Add VerifyLifetimeName
 * 2016-01-06 Created
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


/**
 * Pre-loaded LifetimeName types.
 */
class ARB_API ARBConfigLifetimeName : public ARBBase
{
protected:
	ARBConfigLifetimeName();
	ARBConfigLifetimeName(ARBConfigLifetimeName const& rhs);
	ARBConfigLifetimeName(ARBConfigLifetimeName&& rhs);

public:
	~ARBConfigLifetimeName();
	static ARBConfigLifetimeNamePtr New();
	ARBConfigLifetimeNamePtr Clone() const;

	ARBConfigLifetimeName& operator=(ARBConfigLifetimeName const& rhs);
	ARBConfigLifetimeName& operator=(ARBConfigLifetimeName&& rhs);

	bool operator==(ARBConfigLifetimeName const& rhs) const;
	bool operator!=(ARBConfigLifetimeName const& rhs) const
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
	 * @pre inTree is the actual ARBConfigLifetimeName element.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBConfigLifetimeName element will be created in ioTree.
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
 * List of ARBConfigLifetimeName objects.
 */
class ARB_API ARBConfigLifetimeNameList : public ARBVector<ARBConfigLifetimeNamePtr>
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
	bool Load(ElementNodePtr const& inTree, ARBVersion const& inVersion, ARBErrorCallback& ioCallback);

	/**
	 * Verify the name exists.
	 * @param inName Name to verify.
	 * @return Whether the name exists.
	 */
	bool VerifyLifetimeName(std::wstring const& inName) const;

	/**
	 * Find a fault.
	 * @param inName Name of fault to find.
	 * @param outLifetimeName Pointer to found object, NULL if not found.
	 * @return Whether the object was found.
	 */
	bool FindLifetimeName(std::wstring const& inName, ARBConfigLifetimeNamePtr* outLifetimeName = nullptr) const;

	/**
	 * Add a Lifetime name.
	 * @param inName LifetimeName to add.
	 * @param outLifetimeName Pointer to object, NULL if name already exists or is empty.
	 * @return Whether the fault was added.
	 */
	bool AddLifetimeName(std::wstring const& inName, ARBConfigLifetimeNamePtr* outLifetimeName = nullptr);

	/**
	 * Delete the Lifetime name.
	 * @param inName Name of fault to delete.
	 * @return Whether fault was deleted.
	 */
	bool DeleteLifetimeName(std::wstring const& inName);
};
