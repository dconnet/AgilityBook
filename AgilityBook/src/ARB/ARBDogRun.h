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
 * @brief The classes that make up the dog's information.
 * @author David Connet
 *
 * Revision History
 * @li 2003-12-28 DRC Added GetSearchStrings.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBBase.h"
#include "ARBDate.h"
#include "ARBDogNotes.h"
#include "ARBDogReferenceRun.h"
#include "ARBDogRunOtherPoints.h"
#include "ARBDogRunPartner.h"
#include "ARBDogRunScoring.h"
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class ARBConfigScoring;
class ARBDogClubList;
class ARBDogTrial;
class ARBVersion;
class CElement;

class ARBDogRun : public ARBBase
{
public:
	ARBDogRun();
	ARBDogRun(const ARBDogRun& rhs);
	ARBDogRun& operator=(const ARBDogRun& rhs);
	bool operator==(const ARBDogRun& rhs) const;
	bool operator!=(const ARBDogRun& rhs) const;

	virtual std::string GetGenericName() const;
	virtual size_t GetSearchStrings(std::set<std::string>& ioStrings) const;

	bool Load(
		const ARBConfig& inConfig,
		const ARBDogClubList& inClubs,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);
	bool Save(CElement& ioTree) const;

	int NumOtherPointsInUse(const std::string& inOther) const;
	int RenameOtherPoints(
		const std::string& inOldName,
		const std::string& inNewName);
	int DeleteOtherPoints(const std::string& inName);

	short GetMachPoints(const ARBConfigScoring* inScoring) const;
	short GetTitlePoints(
		const ARBConfigScoring* inScoring,
		bool* bClean = NULL) const;
	ARBDouble GetScore(const ARBConfigScoring* inScoring) const;

	const ARBDate& GetDate() const;
	void SetDate(const ARBDate& inDate);
	const std::string& GetDivision() const;
	void SetDivision(const std::string& inDiv);
	const std::string& GetLevel() const;
	void SetLevel(const std::string& inLevel);
	const std::string& GetHeight() const;
	void SetEvent(const std::string& inEvent);
	const std::string& GetConditions() const;
	void SetHeight(const std::string& inHeight);
	const std::string& GetEvent() const;
	void SetConditions(const std::string& inConditions);
	const std::string& GetJudge() const;
	void SetJudge(const std::string& inJudge);
	const std::string& GetHandler() const;
	void SetHandler(const std::string& inHandler);
	const ARBDogRunPartnerList& GetPartners() const;
	ARBDogRunPartnerList& GetPartners();
	const ARBDogRunScoring& GetScoring() const;
	ARBDogRunScoring& GetScoring();
	ARB_Q GetQ() const;
	void SetQ(ARB_Q inQ);
	short GetPlace() const;
	void SetPlace(short inPlace);
	short GetInClass() const;
	void SetInClass(short inInClass);
	short GetDogsQd() const;
	void SetDogsQd(short inDogsQd);
	const ARBDogRunOtherPointsList& GetOtherPoints() const;
	ARBDogRunOtherPointsList& GetOtherPoints();
	const ARBDogFaultList& GetFaults() const;
	ARBDogFaultList& GetFaults();
	const std::string& GetCRCD() const;
	void SetCRCD(const std::string& inCRCD);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);
	const ARBDogReferenceRunList& GetReferenceRuns() const;
	ARBDogReferenceRunList& GetReferenceRuns();

private:
	~ARBDogRun();
	ARBDate m_Date;
	std::string m_Division;
	std::string m_Level;
	std::string m_Height;
	std::string m_Event;
	std::string m_Conditions;
	std::string m_Judge;
	std::string m_Handler;
	ARBDogRunPartnerList m_Partners;
	ARBDogRunScoring m_Scoring;
	ARB_Q m_Q;
	short m_Place;
	short m_InClass;
	short m_DogsQd;
	ARBDogRunOtherPointsList m_OtherPoints;
	ARBDogNotes m_Notes;
	ARBDogReferenceRunList m_RefRuns;
};

inline const ARBDate& ARBDogRun::GetDate() const
{
	return m_Date;
}

inline void ARBDogRun::SetDate(const ARBDate& inDate)
{
	m_Date = inDate;
}

inline const std::string& ARBDogRun::GetDivision() const
{
	return m_Division;
}

inline void ARBDogRun::SetDivision(const std::string& inDiv)
{
	m_Division = inDiv;
}

inline const std::string& ARBDogRun::GetLevel() const
{
	return m_Level;
}

inline void ARBDogRun::SetLevel(const std::string& inLevel)
{
	m_Level = inLevel;
}

inline const std::string& ARBDogRun::GetEvent() const
{
	return m_Event;
}

inline void ARBDogRun::SetEvent(const std::string& inEvent)
{
	m_Event = inEvent;
}

inline const std::string& ARBDogRun::GetHeight() const
{
	return m_Height;
}

inline void ARBDogRun::SetHeight(const std::string& inHeight)
{
	m_Height = inHeight;
}

inline const std::string& ARBDogRun::GetConditions() const
{
	return m_Conditions;
}

inline void ARBDogRun::SetConditions(const std::string& inConditions)
{
	m_Conditions = inConditions;
}

inline const std::string& ARBDogRun::GetJudge() const
{
	return m_Judge;
}

inline void ARBDogRun::SetJudge(const std::string& inJudge)
{
	m_Judge = inJudge;
}

inline const std::string& ARBDogRun::GetHandler() const
{
	return m_Handler;
}

inline void ARBDogRun::SetHandler(const std::string& inHandler)
{
	m_Handler = inHandler;
}

inline const ARBDogRunPartnerList& ARBDogRun::GetPartners() const
{
	return m_Partners;
}

inline ARBDogRunPartnerList& ARBDogRun::GetPartners()
{
	return m_Partners;
}

inline const ARBDogRunScoring& ARBDogRun::GetScoring() const
{
	return m_Scoring;
}

inline ARBDogRunScoring& ARBDogRun::GetScoring()
{
	return m_Scoring;
}

inline ARB_Q ARBDogRun::GetQ() const
{
	return m_Q;
}

inline void ARBDogRun::SetQ(ARB_Q inQ)
{
	m_Q = inQ;
}

inline short ARBDogRun::GetPlace() const
{
	return m_Place;
}

inline void ARBDogRun::SetPlace(short inPlace)
{
	m_Place = inPlace;
}

inline short ARBDogRun::GetInClass() const
{
	return m_InClass;
}

inline void ARBDogRun::SetInClass(short inInClass)
{
	m_InClass = inInClass;
}

inline short ARBDogRun::GetDogsQd() const
{
	return m_DogsQd;
}

inline void ARBDogRun::SetDogsQd(short inDogsQd)
{
	m_DogsQd = inDogsQd;
}

inline const ARBDogFaultList& ARBDogRun::GetFaults() const
{
	return m_Notes.GetFaults();
}

inline ARBDogFaultList& ARBDogRun::GetFaults()
{
	return m_Notes.GetFaults();
}

inline const std::string& ARBDogRun::GetCRCD() const
{
	return m_Notes.GetCRCD();
}

inline void ARBDogRun::SetCRCD(const std::string& inCRCD)
{
	m_Notes.SetCRCD(inCRCD);
}

inline const std::string& ARBDogRun::GetNote() const
{
	return m_Notes.GetNote();
}

inline void ARBDogRun::SetNote(const std::string& inNote)
{
	m_Notes.SetNote(inNote);
}

inline const ARBDogReferenceRunList& ARBDogRun::GetReferenceRuns() const
{
	return m_RefRuns;
}

inline ARBDogReferenceRunList& ARBDogRun::GetReferenceRuns()
{
	return m_RefRuns;
}

inline const ARBDogRunOtherPointsList& ARBDogRun::GetOtherPoints() const
{
	return m_OtherPoints;
}

inline ARBDogRunOtherPointsList& ARBDogRun::GetOtherPoints()
{
	return m_OtherPoints;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogRunList : public ARBVector<ARBDogRun>
{
public:
	bool Load(
		const ARBConfig& inConfig,
		const ARBDogClubList& inClubs,
		const CElement& inTree,
		const ARBVersion& inVersion,
		std::string& ioErrMsg);

	bool operator==(const ARBDogRunList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogRunList& rhs) const
	{
		return !isEqual(rhs);
	}

	void sort(bool inDescending);

	ARBDate GetStartDate() const;
	ARBDate GetEndDate() const;

	ARBDogRun* AddRun(ARBDogRun* inRun);
	bool DeleteRun(const ARBDogRun* inRun);
};
