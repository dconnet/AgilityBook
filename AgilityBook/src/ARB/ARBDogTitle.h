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

#include <string>
#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigVenue;
class CElement;

class ARBDogTitle : public ARBBase
{
public:
	ARBDogTitle();
	ARBDogTitle(const ARBDogTitle& rhs);
	ARBDogTitle& operator=(const ARBDogTitle& rhs);
	bool operator==(const ARBDogTitle& rhs) const;
	bool operator!=(const ARBDogTitle& rhs) const;

	virtual std::string GetGenericName() const	{return GetName();}

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		int inVersion);
	bool Save(CElement& ioTree) const;

	const ARBDate& GetDate() const;
	void SetDate(const ARBDate& inDate);
	const std::string& GetVenue() const;
	void SetVenue(const std::string& inVenue);
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const bool GetReceived() const;
	void SetReceived(bool inReceived);

private:
	~ARBDogTitle();
	ARBDate m_Date;
	std::string m_Venue;
	std::string m_Name;
	bool m_bReceived;
};

inline const ARBDate& ARBDogTitle::GetDate() const
{
	return m_Date;
}

inline void ARBDogTitle::SetDate(const ARBDate& inDate)
{
	m_Date = inDate;
}

inline const std::string& ARBDogTitle::GetVenue() const
{
	return m_Venue;
}

inline void ARBDogTitle::SetVenue(const std::string& inVenue)
{
	m_Venue = inVenue;
}

inline const std::string& ARBDogTitle::GetName() const
{
	return m_Name;
}

inline void ARBDogTitle::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const bool ARBDogTitle::GetReceived() const
{
	return m_bReceived;
}

inline void ARBDogTitle::SetReceived(bool inReceived)
{
	m_bReceived = inReceived;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogTitleList : public ARBVectorLoad2<ARBDogTitle>
{
public:
	bool operator==(const ARBDogTitleList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogTitleList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool bDescending = true);

	int NumTitlesInVenue(const std::string& inVenue) const;
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);
	int DeleteVenue(const std::string& inVenue);

	int NumTitlesInDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inDiv) const;
	int RenameDivision(
		const ARBConfigVenue* inVenue,
		const std::string& inOldDiv,
		const std::string& inNewDiv);
	int DeleteDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);

	int NumTitlesInUse(
		const std::string& inVenue,
		const std::string& inTitle) const;
	int RenameTitle(
		const std::string& inVenue,
		const std::string& inOldTitle,
		const std::string& inNewTitle);
	int DeleteTitle(
		const std::string& inVenue,
		const std::string& inTitle);

	ARBDogTitle* AddTitle(ARBDogTitle* inTitle);
};
