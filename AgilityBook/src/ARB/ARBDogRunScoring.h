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
 * @brief ARBDogRunScoring class.
 * @author David Connet
 *
 * Revision History
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-03-26 DRC Changed Table-in-YPS to hasTable.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBTypes.h"
#include "ARBConfigScoring.h"
class ARBVersion;
class Element;

/**
 * Keeps track of the scoring for a run.
 */
class ARBDogRunScoring
{
public:
	/**
	 * Types of scoring
	 */
	typedef enum
	{
		eTypeUnknown = -1,		///< Unknown
		eTypeByTime = 0,		///< Scoring based on time.
		eTypeByOpenClose = 1,	///< Scoring based on Opening/Closing points.
		eTypeByPoints = 2		///< Scoring based on points.
	} ScoringType;

	static ARBDogRunScoring::ScoringType TranslateConfigScoring(ARBConfigScoring::ScoringStyle inType);

	ARBDogRunScoring();
	ARBDogRunScoring(ARBDogRunScoring const& rhs);
	~ARBDogRunScoring();
	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);
	bool operator==(ARBDogRunScoring const& rhs) const;
	bool operator!=(ARBDogRunScoring const& rhs) const;

	/**
	 * Load the scoring.
	 * @pre inTree is the actual ARBDogRunScoring element.
	 * @param inEvent Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioErrMsg Accumulated error messages.
	 * @return Success
	 */
	bool Load(
		ARBConfigScoring const* inEvent,
		Element const& inTree,
		ARBVersion const& inVersion,
		std::string& ioErrMsg);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRunScoring element will be created in ioTree.
	 */
	bool Save(Element& ioTree) const;

	/**
	 * Get the YPS for the run.
	 * @param inTableInYPS Include table in YPS computation.
	 * @param outYPS YPS for the run.
	 * @return Indicates whether outYPS is valid, not all runs have YPS.
	 */
	bool GetYPS(bool inTableInYPS, double& outYPS) const;

	/**
	 * Getters/setters.
	 */
	ScoringType GetType() const;
	void SetType(ScoringType inType, bool inRound);
	ARBDouble GetSCT() const;
	void SetSCT(ARBDouble inSCT);
	double GetYards() const;
	void SetYards(double inYards);
	ARBDouble GetTime() const;
	void SetTime(ARBDouble inTime);
	bool HasTable() const;
	void SetHasTable(bool inInc);
	bool TableNeedsConverting() const;
	double GetTimeFaults(ARBConfigScoring const* inScoring) const;
	short GetCourseFaults() const;
	void SetCourseFaults(short inFaults);
	short GetNeedOpenPts() const;
	void SetNeedOpenPts(short inOpenPts);
	short GetNeedClosePts() const;
	void SetNeedClosePts(short inClosePts);
	short GetOpenPts() const;
	void SetOpenPts(short inOpenPts);
	short GetClosePts() const;
	void SetClosePts(short inClosePts);

private:
	ScoringType m_type;
	bool m_bRoundTimeFaults;
	ARBDouble m_SCT;
	double m_Yards;
	ARBDouble m_Time;
	bool m_Table;
	bool m_ConvertTable; ///< Used to update table setting when upgrading
	short m_CourseFaults;
	short m_NeedOpenPts;
	short m_NeedClosePts;
	short m_OpenPts;
	short m_ClosePts;
};

inline ARBDogRunScoring::ScoringType ARBDogRunScoring::GetType() const
{
	return m_type;
}

inline void ARBDogRunScoring::SetType(
	ARBDogRunScoring::ScoringType inType,
	bool inRound)
{
	m_type = inType;
	m_bRoundTimeFaults = inRound;
}

inline ARBDouble ARBDogRunScoring::GetSCT() const
{
	return m_SCT;
}

inline void ARBDogRunScoring::SetSCT(ARBDouble inSCT)
{
	m_SCT = inSCT;
}

inline double ARBDogRunScoring::GetYards() const
{
	return m_Yards;
}

inline void ARBDogRunScoring::SetYards(double inYards)
{
	m_Yards = inYards;
}

inline ARBDouble ARBDogRunScoring::GetTime() const
{
	return m_Time;
}

inline void ARBDogRunScoring::SetTime(ARBDouble inTime)
{
	m_Time = inTime;
}

inline bool ARBDogRunScoring::HasTable() const
{
	return m_Table;
}

inline void ARBDogRunScoring::SetHasTable(bool inInc)
{
	m_Table = inInc;
	m_ConvertTable = false;
}

inline bool ARBDogRunScoring::TableNeedsConverting() const
{
	return m_ConvertTable;
}

inline short ARBDogRunScoring::GetCourseFaults() const
{
	return m_CourseFaults;
}

inline void ARBDogRunScoring::SetCourseFaults(short inFaults)
{
	m_CourseFaults = inFaults;
}

inline short ARBDogRunScoring::GetNeedOpenPts() const
{
	return m_NeedOpenPts;
}

inline void ARBDogRunScoring::SetNeedOpenPts(short inOpenPts)
{
	m_NeedOpenPts = inOpenPts;
}

inline short ARBDogRunScoring::GetNeedClosePts() const
{
	return m_NeedClosePts;
}

inline void ARBDogRunScoring::SetNeedClosePts(short inClosePts)
{
	m_NeedClosePts = inClosePts;
}

inline short ARBDogRunScoring::GetOpenPts() const
{
	return m_OpenPts;
}

inline void ARBDogRunScoring::SetOpenPts(short inOpenPts)
{
	m_OpenPts = inOpenPts;
}

inline short ARBDogRunScoring::GetClosePts() const
{
	return m_ClosePts;
}

inline void ARBDogRunScoring::SetClosePts(short inClosePts)
{
	m_ClosePts = inClosePts;
}
