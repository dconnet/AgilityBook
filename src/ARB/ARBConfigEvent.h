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
 * @li 2004-02-02 DRC Added VerifyEvent.
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-12-27 DRC Changed FindEvent to take a date.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include <string>
#include "ARBBase.h"
#include "ARBConfigDivision.h"
#include "ARBConfigScoring.h"
#include "ARBVector.h"
class ARBVersion;
class CElement;

class ARBConfigEvent : public ARBBase
{
public:
	ARBConfigEvent();
	ARBConfigEvent(const ARBConfigEvent& rhs);
	ARBConfigEvent& operator=(const ARBConfigEvent& rhs);
	bool operator==(const ARBConfigEvent& rhs) const;
	bool operator!=(const ARBConfigEvent& rhs) const;

	virtual std::string GetGenericName() const	{return GetName();}
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const ARBConfigDivisionList& inDivisions,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	bool Update(int indent, const ARBConfigEvent* inEventNew, std::string& ioInfo);

	size_t FindAllEvents(
		const std::string& inDivision,
		const std::string& inLevel,
		bool inTitlePoints,
		std::vector<const ARBConfigScoring*>& outList) const;
	bool VerifyEvent(
		const std::string& inLevel,
		const std::string& inEvent) const;
	const ARBConfigScoring* FindEvent(
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate) const;

	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetDesc() const;
	void SetDesc(const std::string& inDesc);
	bool HasPartner() const;
	void SetHasPartner(bool inHas);
	const ARBConfigScoringList& GetScorings() const;
	ARBConfigScoringList& GetScorings();

private:
	~ARBConfigEvent();
	std::string m_Name;
	std::string m_Desc;
	bool m_bHasPartner;
	ARBConfigScoringList m_Scoring;
};

inline const std::string& ARBConfigEvent::GetName() const
{
	return m_Name;
}

inline void ARBConfigEvent::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBConfigEvent::GetDesc() const
{
	return m_Desc;
}

inline void ARBConfigEvent::SetDesc(const std::string& inDesc)
{
	m_Desc = inDesc;
}

inline bool ARBConfigEvent::HasPartner() const
{
	return m_bHasPartner;
}

inline void ARBConfigEvent::SetHasPartner(bool inHas)
{
	m_bHasPartner = inHas;
}

inline const ARBConfigScoringList& ARBConfigEvent::GetScorings() const
{
	return m_Scoring;
}

inline ARBConfigScoringList& ARBConfigEvent::GetScorings()
{
	return m_Scoring;
}

/////////////////////////////////////////////////////////////////////////////

class ARBConfigEventList : public ARBVector<ARBConfigEvent>
{
public:
	bool Load(
		const ARBConfigDivisionList& inDivisions,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBConfigEventList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBConfigEventList& rhs) const
	{
		return !isEqual(rhs);
	}

	bool VerifyEvent(
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel) const;
	const ARBConfigScoring* FindEvent(
		const std::string& inEvent,
		const std::string& inDivision,
		const std::string& inLevel,
		const ARBDate& inDate) const;

	int RenameDivision(
		const std::string& inOldDiv,
		const std::string& inNewDiv);
	int DeleteDivision(const std::string& inDiv);
	int RenameLevel(
		const std::string& inOldLevel,
		const std::string& inNewLevel);
	int DeleteLevel(const std::string& inLevel);

	const ARBConfigEvent* FindEvent(const std::string& inEvent) const;
	ARBConfigEvent* FindEvent(const std::string& inEvent);
	ARBConfigEvent* AddEvent(ARBConfigEvent* inEvent);
	bool DeleteEvent(const std::string& inEvent);
};
