#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 * @brief Configuration class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBConfigVenue : public ARBBase
{
public:
	ARBConfigVenue();
	ARBConfigVenue(const ARBConfigVenue& rhs);
	ARBConfigVenue& operator=(const ARBConfigVenue& rhs);
	bool operator==(const ARBConfigVenue& rhs) const;
	bool operator!=(const ARBConfigVenue& rhs) const;
	void clear();

	virtual std::string GetGenericName() const	{return GetName();}
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	/**
	 * @param ioConfig When migrating an old file, we may be updating the config.
	 * @param inTree Data to parse.
	 * @param inVersion Version of file being loaded.
	 * @param ioErrMsg Output string for error accumulated messages.
	 */
	bool Load(
		ARBConfig& ioConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	bool Update(int indent, const ARBConfigVenue* inVenueNew, std::string& ioInfo);

	const std::string GetName() const;
	void SetName(std::string inName);
	const std::string GetDesc() const;
	void SetDesc(std::string inDesc);
	const ARBConfigDivisionList& GetDivisions() const;
	ARBConfigDivisionList& GetDivisions();
	const ARBConfigEventList& GetEvents() const;
	ARBConfigEventList& GetEvents();

private:
	~ARBConfigVenue();
	std::string m_Name;
	std::string m_Desc;
	ARBConfigDivisionList m_Divisions;
	ARBConfigEventList m_Events;
};

inline const std::string ARBConfigVenue::GetName() const
{
	return m_Name;
}

inline void ARBConfigVenue::SetName(std::string inName)
{
	m_Name = inName;
}

inline const std::string ARBConfigVenue::GetDesc() const
{
	return m_Desc;
}

inline void ARBConfigVenue::SetDesc(std::string inDesc)
{
	m_Desc = inDesc;
}

inline const ARBConfigDivisionList& ARBConfigVenue::GetDivisions() const
{
	return m_Divisions;
}

inline ARBConfigDivisionList& ARBConfigVenue::GetDivisions()
{
	return m_Divisions;
}

inline const ARBConfigEventList& ARBConfigVenue::GetEvents() const
{
	return m_Events;
}

inline ARBConfigEventList& ARBConfigVenue::GetEvents()
{
	return m_Events;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigVenueList : public ARBVector<ARBConfigVenue>
{
public:
	bool Load(
		ARBConfig& ioConfig,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBConfigVenueList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigVenueList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending = true);

	bool VerifyVenue(const std::string& inVenue) const;
	bool VerifyLevel(
		const std::string& inVenue,
		const std::string& inDivision,
		const std::string& inLevel
		) const;
	const ARBConfigTitle* FindTitleCompleteName(
		const std::string& inVenue,
		const std::string& inName,
		bool bAbbrevFirst = true) const;
	const ARBConfigTitle* FindTitle(
		const std::string& inVenue,
		const std::string& inTitle) const;

	const ARBConfigVenue* FindVenue(const std::string& inVenue) const;
	ARBConfigVenue* FindVenue(const std::string& inVenue);
	ARBConfigVenue* AddVenue(const std::string& inVenue);
	ARBConfigVenue* AddVenue(ARBConfigVenue* inVenue);
	int DeleteVenue(const std::string& inVenue);

	/**
	 * @param inVenue Venue name
	 * @param inEvent Event name
	 * @param inDivision Division name
	 * @param inLevel True Level name (sublevel)
	 * @param inDate Date for requested scoring.
	 */
	const ARBConfigScoring* FindEvent(
		const std::string& inVenue,
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate) const;
};
