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
#include "ARBDogClub.h"
#include "ARBDogRun.h"
#include "ARBVector.h"
class ARBCalendar;
class ARBConfig;
class ARBConfigVenue;
class ARBDate;
class CElement;

class ARBDogTrial : public ARBBase
{
public:
	ARBDogTrial();
	ARBDogTrial(const ARBCalendar& inCal);
	ARBDogTrial(const ARBDogTrial& rhs);
	ARBDogTrial& operator=(const ARBDogTrial& rhs);
	bool operator==(const ARBDogTrial& rhs) const;
	bool operator!=(const ARBDogTrial& rhs) const;

	virtual std::string GetGenericName() const;

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		int inVersion);
	bool Save(CElement& ioTree) const;

	int NumQs(
		const ARBDate& inDate,
		const ARBConfig& inConfig,
		const std::string& inDiv,
		const std::string& inLevel) const;
	short GetMachPoints(
		const ARBConfig& inConfig,
		const std::string& inDiv,
		const std::string& inLevel) const;

	bool HasVenue(const std::string inVenue) const;

	const std::string& GetLocation() const;
	void SetLocation(const std::string& inLoc);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);
	bool IsVerified() const;
	void SetVerified(bool inVerified);
	const ARBDogClubList& GetClubs() const;
	ARBDogClubList& GetClubs();
	const ARBDogRunList& GetRuns() const;
	ARBDogRunList& GetRuns();

private:
	~ARBDogTrial();
	std::string m_Location;
	std::string m_Note;
	bool m_Verified;
	ARBDogClubList m_Clubs;
	ARBDogRunList m_Runs;
};

inline const std::string& ARBDogTrial::GetLocation() const
{
	return m_Location;
}

inline void ARBDogTrial::SetLocation(const std::string& inLoc)
{
	m_Location = inLoc;
}

inline const std::string& ARBDogTrial::GetNote() const
{
	return m_Note;
}

inline void ARBDogTrial::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

inline bool ARBDogTrial::IsVerified() const
{
	return m_Verified;
}

inline void ARBDogTrial::SetVerified(bool inVerified)
{
	m_Verified = inVerified;
}

inline const ARBDogClubList& ARBDogTrial::GetClubs() const
{
	return m_Clubs;
}

inline ARBDogClubList& ARBDogTrial::GetClubs()
{
	return m_Clubs;
}

inline const ARBDogRunList& ARBDogTrial::GetRuns() const
{
	return m_Runs;
}

inline ARBDogRunList& ARBDogTrial::GetRuns()
{
	return m_Runs;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogTrialList : public ARBVectorLoad2<ARBDogTrial>
{
public:
	bool operator==(const ARBDogTrialList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogTrialList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending);

	int NumTrialsInVenue(const std::string& inVenue) const;
	int RenameVenue(
		const std::string& inOldVenue,
		const std::string& inNewVenue);
	int DeleteVenue(const std::string& inVenue);

	int NumOtherPointsInUse(const std::string& inOther) const;
	int RenameOtherPoints(
		const std::string& inOldOther,
		const std::string& inNewOther);
	int DeleteOtherPoints(const std::string& inOther);

	int NumMultiHostedTrialsInDivision(
		const ARBConfig& inConfig,
		const std::string& inVenue,
		const std::string& inDiv);
	int NumRunsInDivision(
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

	int NumLevelsInUse(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inLevel) const;
	int RenameLevel(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inOldLevel,
		const std::string& inNewLevel);
	int DeleteLevel(
		const std::string& inVenue,
		const std::string& inDiv,
		const std::string& inLevel);

	int NumEventsInUse(
		const std::string& inVenue,
		const std::string& inEvent) const;
	int RenameEvent(
		const std::string& inVenue,
		const std::string& inOldEvent,
		const std::string& inNewEvent);
	int DeleteEvent(
		const std::string& inVenue,
		const std::string& inEvent);

	ARBDogTrial* AddTrial(ARBDogTrial* inTrial);
	bool DeleteTrial(const ARBDogTrial* inTrial);
};
