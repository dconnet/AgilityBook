#pragma once

/*
 * Copyright (c) 2002-2003 David Connet.
 * All Rights Reserved.
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
 */

#include "ARBBase.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigScoring;
class CElement;

class ARBDogClub : public ARBBase
{
public:
	ARBDogClub();
	ARBDogClub(const ARBDogClub& rhs);
	ARBDogClub& operator=(const ARBDogClub& rhs);
	bool operator==(const ARBDogClub& rhs) const;
	bool operator!=(const ARBDogClub& rhs) const;

	virtual std::string GetGenericName() const	{return GetName();}

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		int inVersion);
	bool Save(CElement& ioTree) const;

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);

private:
	~ARBDogClub();
	std::string m_Name;
	std::string m_Venue;
};

inline const std::string& ARBDogClub::GetName() const
{
	return m_Name;
}

inline void ARBDogClub::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBDogClub::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogClub::SetVenue(const std::string& inVenue)
{
	m_Venue = inVenue;
}

/////////////////////////////////////////////////////////////////////////////

// Note, the order within the club list is important.
class ARBDogClubList : public ARBVectorLoad2<ARBDogClub>
{
public:
	bool operator==(const ARBDogClubList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogClubList& rhs) const
	{
		return !isEqual(rhs);
	}

	const ARBDogClub* GetPrimaryClub() const;

	const ARBConfigScoring* FindEvent(
		const ARBConfig* inConfig,
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel) const;
	const ARBDogClub* FindVenue(const std::string& inVenue) const;

	ARBDogClub* AddClub(
		const std::string& inName,
		const std::string& inVenue);
	bool DeleteClub(
		const std::string& inName,
		const std::string& inVenue);
};
