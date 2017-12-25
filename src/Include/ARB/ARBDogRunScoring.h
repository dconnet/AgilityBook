#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief ARBDogRunScoring class.
 * @author David Connet
 *
 * Revision History
 * 2006-04-04 Added GetMinYPS.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-12-04 Added support for NADAC bonus titling points.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-03-26 Changed Table-in-YPS to hasTable.
 * 2004-02-14 Added Table-in-YPS flag.
 * 2003-11-26 Changed version number to a complex value.
 */

#include "ARB/ARBConfigScoring.h"
#include "ARB/ARBTypes2.h"


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
	 * Get the YPS for a run simply given the time (used for RefRun).
	 * @param inTableInYPS Include table in YPS computation.
	 * @param inTime Time of run
	 * @param outYPS YPS for the run.
	 * @return Indicates whether outYPS is valid, not all runs have YPS.
	 */
	bool GetYPS(
			bool inTableInYPS,
			double inTime,
			double& outYPS) const;

	/**
	 * Get the Obstacles per second for the run.
	 * @param inTableInYPS Include table in computation.
	 * @param inRunTimeInOPS Use runtime instead of opening time in OPS.
	 * @param outOPS OPS for the run.
	 * @return Indicates whether outOPS is valid (needs obstacle count and time)
	 */
	bool GetObstaclesPS(
			bool inTableInYPS,
			bool inRunTimeInOPS,
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
	double GetBonusTitlePts() const
	{
		return m_BonusTitlePts;
	}
	void SetBonusTitlePts(double inBonusPts)
	{
		m_BonusTitlePts = inBonusPts;
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
	double m_BonusTitlePts;
};
