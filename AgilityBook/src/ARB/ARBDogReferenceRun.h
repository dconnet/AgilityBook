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
#include "ARBTypes.h"
#include "ARBVector.h"
class ARBConfig;
class CElement;

class ARBDogReferenceRun : public ARBBase
{
public:
	ARBDogReferenceRun();
	ARBDogReferenceRun(const ARBDogReferenceRun& rhs);
	ARBDogReferenceRun& operator=(const ARBDogReferenceRun& rhs);
	bool operator==(const ARBDogReferenceRun& rhs) const;
	bool operator!=(const ARBDogReferenceRun& rhs) const;

	virtual std::string GetGenericName() const	{return GetName();}

	bool Load(
		const ARBConfig& inConfig,
		const CElement& inTree,
		int inVersion);
	bool Save(CElement& ioTree) const;

	ARB_Q GetQ() const;
	void SetQ(ARB_Q inQ);
	short GetPlace() const;
	void SetPlace(short inPlace);
	const std::string& GetName() const;
	void SetName(const std::string& inName);
	const std::string& GetBreed() const;
	void SetBreed(const std::string& inBreed);
	ARBDouble GetTime() const;
	void SetTime(ARBDouble inTime);
	const std::string& GetScore() const;
	void SetScore(const std::string& inScore);
	const std::string& GetNote() const;
	void SetNote(const std::string& inNote);

private:
	~ARBDogReferenceRun();
	ARB_Q m_Q;
	short m_Place;
	std::string m_Name;
	std::string m_Breed;
	ARBDouble m_Time;
	std::string m_Score;
	std::string m_Note;
};

inline ARB_Q ARBDogReferenceRun::GetQ() const
{
	return m_Q;
}

inline void ARBDogReferenceRun::SetQ(ARB_Q inQ)
{
	m_Q = inQ;
}

inline short ARBDogReferenceRun::GetPlace() const
{
	return m_Place;
}

inline void ARBDogReferenceRun::SetPlace(short inPlace)
{
	m_Place = inPlace;
}

inline const std::string& ARBDogReferenceRun::GetName() const
{
	return m_Name;
}

inline void ARBDogReferenceRun::SetName(const std::string& inName)
{
	m_Name = inName;
}

inline const std::string& ARBDogReferenceRun::GetBreed() const
{
	return m_Breed;
}

inline void ARBDogReferenceRun::SetBreed(const std::string& inBreed)
{
	m_Breed = inBreed;
}

inline ARBDouble ARBDogReferenceRun::GetTime() const
{
	return m_Time;
}

inline void ARBDogReferenceRun::SetTime(ARBDouble inTime)
{
	m_Time = inTime;
}

inline const std::string& ARBDogReferenceRun::GetScore() const
{
	return m_Score;
}

inline void ARBDogReferenceRun::SetScore(const std::string& inScore)
{
	m_Score = inScore;
}

inline const std::string& ARBDogReferenceRun::GetNote() const
{
	return m_Note;
}

inline void ARBDogReferenceRun::SetNote(const std::string& inNote)
{
	m_Note = inNote;
}

/////////////////////////////////////////////////////////////////////////////

class ARBDogReferenceRunList : public ARBVectorLoad2<ARBDogReferenceRun>
{
public:
	bool operator==(const ARBDogReferenceRunList& rhs) const
	{
		return isEqual(rhs);
	}
	bool operator!=(const ARBDogReferenceRunList& rhs) const
	{
		return !isEqual(rhs);
	}

	ARBDogReferenceRun* AddReferenceRun(const ARBDogReferenceRun* inRef);
	bool DeleteReferenceRun(const ARBDogReferenceRun* inRef);
};
