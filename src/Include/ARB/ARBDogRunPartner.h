#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogRunPartner and ARBDogRunPartnerList classes.
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2003-12-28 Added GetSearchStrings.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-08 Changed AddPartner usage.
 */

#include "ARBBase.h"
#include "ARBTypes2.h"
#include "LibwxARB.h"


namespace dconSoft
{
namespace ARB
{

/**
 * Keeps track of partners in pairs runs.
 */
class ARB_API ARBDogRunPartner : public ARBBase
{
protected:
	ARBDogRunPartner();
	ARBDogRunPartner(ARBDogRunPartner const& rhs);
	ARBDogRunPartner(ARBDogRunPartner&& rhs);

public:
	~ARBDogRunPartner();
	static ARBDogRunPartnerPtr New();
	ARBDogRunPartnerPtr Clone() const;

	ARBDogRunPartner& operator=(ARBDogRunPartner const& rhs);
	ARBDogRunPartner& operator=(ARBDogRunPartner&& rhs);

	bool operator==(ARBDogRunPartner const& rhs) const;
	bool operator!=(ARBDogRunPartner const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	std::wstring GetGenericName() const override
	{
		return m_Dog;
	}

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	size_t GetSearchStrings(std::set<std::wstring>& ioStrings) const override;

	/**
	 * Load a partner.
	 * @pre inTree is the actual ARBDogRunPartner element.
	 * @param inConfig Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRun element will be created in ioTree.
	 */
	bool Save(ARBCommon::ElementNodePtr const& ioTree) const;

	/*
	 * Getters/setters.
	 */
	std::wstring const& GetHandler() const
	{
		return m_Handler;
	}
	void SetHandler(std::wstring const& inHandler)
	{
		m_Handler = inHandler;
	}
	std::wstring const& GetDog() const
	{
		return m_Dog;
	}
	void SetDog(std::wstring const& inDog)
	{
		m_Dog = inDog;
	}
	std::wstring const& GetRegNum() const
	{
		return m_RegNum;
	}
	void SetRegNum(std::wstring const& inRegNum)
	{
		m_RegNum = inRegNum;
	}

private:
	std::wstring m_Handler;
	std::wstring m_Dog;
	std::wstring m_RegNum;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRunPartner objects.
 */
class ARB_API ARBDogRunPartnerList : public ARBVector<ARBDogRunPartnerPtr>
{
public:
	/**
	 * Load the information from XML (the tree).
	 * @pre inTree is the actual T element.
	 * @param inConfig Configuration information to verify data to load against.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
		ARBConfig const& inConfig,
		ARBCommon::ElementNodePtr const& inTree,
		ARBCommon::ARBVersion const& inVersion,
		ARBErrorCallback& ioCallback);

	/**
	 * Add a partner.
	 * @param inPartner Partner to add.
	 * @return Whether the object was added.
	 */
	bool AddPartner(ARBDogRunPartnerPtr const& inPartner);
};

} // namespace ARB
} // namespace dconSoft
