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
 * @brief The main configuration class.
 * @author David Connet
 *
 * Revision History
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added static GetDTD() method.
 */

#include "ARBConfigAction.h"
#include "ARBConfigFault.h"
#include "ARBConfigOtherPoints.h"
#include "ARBConfigVenue.h"
class ARBVersion;
class CElement;

class ARBConfig
{
public:
	ARBConfig();
	ARBConfig(const ARBConfig& rhs);
	~ARBConfig();
	ARBConfig& operator=(const ARBConfig& rhs);
	bool operator==(const ARBConfig& rhs) const;
	bool operator!=(const ARBConfig& rhs) const;
	void clear();

	// These functions enable backwards file compatibility.
	bool LoadFault(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool LoadOtherPoints(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool Load(
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;
	void Default();
	static std::string GetDTD();

	std::string Update(int indent, const ARBConfig& inConfigNew);

	const ARBConfigActionList& GetActions() const;
	ARBConfigActionList& GetActions();
	const ARBConfigVenueList& GetVenues() const;
	ARBConfigVenueList& GetVenues();
	const ARBConfigFaultList& GetFaults() const;
	ARBConfigFaultList& GetFaults();
	const ARBConfigOtherPointsList& GetOtherPoints() const;
	ARBConfigOtherPointsList& GetOtherPoints();

private:
	ARBConfigActionList m_Actions;
	ARBConfigVenueList m_Venues;
	ARBConfigFaultList m_FaultTypes;
	ARBConfigOtherPointsList m_OtherPoints;
};

inline const ARBConfigActionList& ARBConfig::GetActions() const
{
	return m_Actions;
}

inline ARBConfigActionList& ARBConfig::GetActions()
{
	return m_Actions;
}

inline const ARBConfigVenueList& ARBConfig::GetVenues() const
{
	return m_Venues;
}

inline ARBConfigVenueList& ARBConfig::GetVenues()
{
	return m_Venues;
}

inline const ARBConfigFaultList& ARBConfig::GetFaults() const
{
	return m_FaultTypes;
}

inline ARBConfigFaultList& ARBConfig::GetFaults()
{
	return m_FaultTypes;
}

inline const ARBConfigOtherPointsList& ARBConfig::GetOtherPoints() const
{
	return m_OtherPoints;
}

inline ARBConfigOtherPointsList& ARBConfig::GetOtherPoints()
{
	return m_OtherPoints;
}
