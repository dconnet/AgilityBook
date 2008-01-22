#pragma once

/*
 * Copyright © 2002-2008 David Connet. All Rights Reserved.
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
 * @li 2006-04-04 DRC Added GetMinYPS.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-03-26 DRC Changed Table-in-YPS to hasTable.
 * @li 2004-02-14 DRC Added Table-in-YPS flag.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 */

#include "ARBConfigScoring.h"
#include "ARBTypes.h"

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
	~ARBDogRunScoring();
	ARBDogRunScoring(ARBDogRunScoring const& rhs);
	ARBDogRunScoring& operator=(ARBDogRunScoring const& rhs);

	bool operator==(ARBDogRunScoring const& rhs) const;
	bool operator!=(ARBDogRunScoring const& rhs) const
	{
		return !operator==(rhs);
	}

	/**
	 * Load the scoring.
	 * @pre inTree is the actual ARBDogRunScoring element.
	 * @param inConfigVersion Configuration Version
	 * @param inEvent Configuration for looking up information.
	 * @param inEventScoring Configuration for looking up information.
	 * @param inTree XML structure to convert into ARB.
	 * @param inVersion Version of the document being read.
	 * @param ioCallback Error processing callback.
	 * @return Success
	 */
	bool Load(
			short inConfigVersion,
			ARBConfigEventPtr inEvent,
			ARBConfigScoringPtr inEventScoring,
			ElementNodePtr inTree,
			ARBVersion const& inVersion,
			ARBErrorCallback& ioCallback);

	/**
	 * Save a document.
	 * @param ioTree Parent element.
	 * @return Success
	 * @post The ARBDogRunScoring element will be created in ioTree.
	 */
	bool Save(ElementNodePtr ioTree) const;

	/**
	 * Get the minimum YPS for the run (yards/sct).
	 * @param inTableInYPS Include table in YPS computation.
	 * @param outYPS YPS for the run.
	 * @return Indicates whether outYPS is valid, not all runs have YPS.
	 */
	bool GetMinYPS(
			bool inTableInYPS,
			double& outYPS) const;

	/**
	 * Get the YPS for the run.
	 * @param inTableInYPS Include table in YPS computation.
	 * @param outYPS YPS for the run.
	 * @return Indicates whether outYPS is valid, not all runs have YPS.
	 */
	bool GetYPS(
			bool inTableInYPS,
			double& outYPS) const;

	/**
	 * Get the Obstacles per second for the run.
	 * @param inTableInYPS Include table in computation.
	 * @param outOPS OPS for the run.
	 * @return Indicates whether outOPS is valid (needs obstacle count and time)
	 */
	bool GetObstaclesPS(
			bool inTableInYPS,
			double& outOPS) const;

	/**
	 * Compute the number of time faults based on the scoring configuration.
	 * @param inScoring Configuration to use.
	 */
	double GetTimeFaults(ARBConfigScoringPtr inScoring) const;

	/**
	 * Getters/setters.
	 */
	ScoringType GetType() const
	{
		return m_type;
	}
	void SetType(
			ScoringType inType,
			bool inRound)
	{
		m_type = inType;
		m_bRoundTimeFaults = inRound;
	}
	double GetSCT() const
	{
		return m_SCT;
	}
	void SetSCT(double inSCT)
	{
		m_SCT = inSCT;
	}
	double GetSCT2() const
	{
		return m_SCT2;
	}
	void SetSCT2(double inSCT)
	{
		m_SCT2 = inSCT;
	}
	double GetYards() const
	{
		return m_Yards;
	}
	void SetYards(double inYards)
	{
		m_Yards = inYards;
	}
	short GetObstacles() const
	{
		return m_Obstacles;
	}
	void SetObstacles(short inNumber)
	{
		m_Obstacles = inNumber;
	}
	double GetTime() const
	{
		return m_Time;
	}
	void SetTime(double inTime)
	{
		m_Time = inTime;
	}
	bool HasTable() const
	{
		return m_Table;
	}
	void SetHasTable(bool inInc)
	{
		m_Table = inInc;
		m_ConvertTable = false;
	}
	bool TableNeedsConverting() const
	{
		return m_ConvertTable;
	}
	short GetCourseFaults() const
	{
		return m_CourseFaults;
	}
	void SetCourseFaults(short inFaults)
	{
		m_CourseFaults = inFaults;
	}
	short GetNeedOpenPts() const
	{
		return m_NeedOpenPts;
	}
	void SetNeedOpenPts(short inOpenPts)
	{
		m_NeedOpenPts = inOpenPts;
	}
	short GetNeedClosePts() const
	{
		return m_NeedClosePts;
	}
	void SetNeedClosePts(short inClosePts)
	{
		m_NeedClosePts = inClosePts;
	}
	short GetOpenPts() const
	{
		return m_OpenPts;
	}
	void SetOpenPts(short inOpenPts)
	{
		m_OpenPts = inOpenPts;
	}
	short GetClosePts() const
	{
		return m_ClosePts;
	}
	void SetClosePts(short inClosePts)
	{
		m_ClosePts = inClosePts;
	}
	short GetBonusPts() const
	{
		return m_BonusPts;
	}
	void SetBonusPts(short inBonusPts)
	{
		m_BonusPts = inBonusPts;
	}

private:
	ScoringType m_type;
	bool m_bRoundTimeFaults;
	double m_SCT;
	double m_SCT2;
	double m_Yards;
	short m_Obstacles;
	double m_Time;
	bool m_Table;
	bool m_ConvertTable; ///< Used to update table setting when upgrading
	short m_CourseFaults;
	short m_NeedOpenPts;
	short m_NeedClosePts;
	short m_OpenPts;
	short m_ClosePts;
	short m_BonusPts;
};
