#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 *
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-08 DRC Changed AddPartner usage.
 */

#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBVersion;
class CElement;

class ARBDogRunPartner : public ARBBase
{
public:
	ARBDogRunPartner();
	ARBDogRunPartner(const ARBDogRunPartner& rhs);
	ARBDogRunPartner& operator=(const ARBDogRunPartner& rhs);
	bool operator==(const ARBDogRunPartner& rhs) const;
	bool operator!=(const ARBDogRunPartner& rhs) const;

	virtual std::string GetGenericName() const	{return GetDog();}

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		const ARBVersion& inVersion);
	bool Save(CElement& ioTree) const;

	const std::string& GetHandler() const;
	void SetHandler(const std::string& inHandler);
	const std::string& GetDog() const;
	void SetDog(const std::string& inDog);
	const std::string& GetRegNum() const;
	void SetRegNum(const std::string& inRegNum);

private:
	~ARBDogRunPartner();
	std::string m_Handler;
	std::string m_Dog;
	std::string m_RegNum;
};

inline const std::string& ARBDogRunPartner::GetHandler() const
{
	return m_Handler;
}

inline void ARBDogRunPartner::SetHandler(const std::string& inHandler)
{
	m_Handler = inHandler;
}

inline const std::string& ARBDogRunPartner::GetDog() const
{
	return m_Dog;
}

inline void ARBDogRunPartner::SetDog(const std::string& inDog)
{
	m_Dog = inDog;
}

inline const std::string& ARBDogRunPartner::GetRegNum() const
{
	return m_RegNum;
}

inline void ARBDogRunPartner::SetRegNum(const std::string& inRegNum)
{
	m_RegNum = inRegNum;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogRunPartnerList : public ARBVectorLoad2<ARBDogRunPartner>
{
public:
	bool operator==(const ARBDogRunPartnerList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogRunPartnerList& rhs) const
	{
		return !isEqual(rhs);
	}

	ARBDogRunPartner* AddPartner(ARBDogRunPartner* inPartner);
};
