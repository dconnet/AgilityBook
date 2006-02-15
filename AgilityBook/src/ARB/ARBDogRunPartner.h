#pragma once

/*
 * Copyright © 2002-2006 David Connet. All Rights Reserved.
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
 * @brief ARBDogRunPartner and ARBDogRunPartnerList classes.
 * @author David Connet
 *
 * Revision History
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-08 DRC Changed AddPartner usage.
 */

#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBErrorCallback;
class ARBVersion;
class Element;

/**
 * Keeps track of partners in pairs runs.
 */
class ARBDogRunPartner : public ARBBase
{
protected:
	ARBDogRunPartner();
	ARBDogRunPartner(ARBDogRunPartner const& rhs);

public:
	~ARBDogRunPartner();
	static ARBDogRunPartnerPtr New();
	ARBDogRunPartnerPtr Clone() const;

	ARBDogRunPartner& operator=(ARBDogRunPartner const& rhs);

	bool operator==(ARBDogRunPartner const& rhs) const;
	bool operator!=(ARBDogRunPartner const& rhs) const;

	/**
	 * Get the generic name of this object.
	 * @return The generic name of this object.
	 */
	virtual ARBString GetGenericName() const;

	/**
	 * Get all the strings to search in this object.
	 * @param ioStrings Accumulated list of strings to be used during a search.
	 * @return Number of strings accumulated in this object.
	 */
	virtual size_t GetSearchStrings(std::set<ARBString>& ioStrings) const;

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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRun element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/*
	 * Getters/setters.
	 */
	ARBString const& GetHandler() const;
	void SetHandler(ARBString const& inHandler);
	ARBString const& GetDog() const;
	void SetDog(ARBString const& inDog);
	ARBString const& GetRegNum() const;
	void SetRegNum(ARBString const& inRegNum);

private:
	ARBString m_Handler;
	ARBString m_Dog;
	ARBString m_RegNum;
};

inline ARBString ARBDogRunPartner::GetGenericName() const
{
	return m_Dog;
}

inline ARBString const& ARBDogRunPartner::GetHandler() const
{
	return m_Handler;
}

inline void ARBDogRunPartner::SetHandler(ARBString const& inHandler)
{
	m_Handler = inHandler;
}

inline ARBString const& ARBDogRunPartner::GetDog() const
{
	return m_Dog;
}

inline void ARBDogRunPartner::SetDog(ARBString const& inDog)
{
	m_Dog = inDog;
}

inline ARBString const& ARBDogRunPartner::GetRegNum() const
{
	return m_RegNum;
}

inline void ARBDogRunPartner::SetRegNum(ARBString const& inRegNum)
{
	m_RegNum = inRegNum;
}

/////////////////////////////////////////////////////////////////////////////

/**
 * List of ARBDogRunPartner objects.
 */
class ARBDogRunPartnerList : public ARBVector<ARBDogRunPartnerPtr>
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
			Element const& inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Add a partner.
	 * @param inPartner Partner to add.
	 * @return Whether the object was added.
	 */
	bool AddPartner(ARBDogRunPartnerPtr inPartner);
};
