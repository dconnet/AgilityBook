/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2015-04-22 Specifically use std::abs, on mac it used abs(int).
 * 2014-12-31 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-12-12 Added Score to Other points list.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-28 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-03-14 Show a summary of lifetime points in the list viewer.
 * 2005-01-02 Show existing points in the list viewer. Added subnames to events.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-08-26 Completed.
 * 2004-07-29 Created
 */

#include "stdafx.h"
#include "DlgListViewer.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "FilterOptions.h"

#include "ARB/ARBConfig.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBDogExistingPoints.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/ARBDate.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ReportListCtrl.h"
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

// Run/MQ are shared in the DataExisting class, hence they need to use
// the same column values.
constexpr int COL_RUN_MQ_DATE = 0;
constexpr int COL_RUN_MQ_STATUS = 1;
constexpr int COL_RUN_MQ_DOG = 2;
constexpr int COL_RUN_MQ_Q = 3;
constexpr int COL_RUN_MQ_TITLE_PTS = 4;
constexpr int COL_RUN_MQ_VENUE = 5;
constexpr int COL_RUN_MQ_DIV = 6;
constexpr int COL_RUN_MQ_LEVEL = 7;
constexpr int COL_RUN_MQ_EVENT = 8;
constexpr int COL_RUN_MQ_SUBNAME = 9;
constexpr int COL_RUN_MQ_LOCATION = 10;
constexpr int COL_RUN_MQ_CLUB = 11;
constexpr int COL_RUN_MQ_JUDGE = 12;
constexpr int COL_RUN_MQ_PLACE = 13;
constexpr int COL_RUN_MQ_INCLASS = 14;
constexpr int COL_RUN_MQ_QD = 15;
constexpr int COL_RUN_MQ_SPEED = 16;
constexpr int COL_RUN_MQ_PARTNERS = 17;
constexpr int COL_OTHER_DATE = 0;
constexpr int COL_OTHER_NAME = 1; // Trial, existing pt
constexpr int COL_OTHER_CLUB = 2;
constexpr int COL_OTHER_VENUE = 3;
constexpr int COL_OTHER_DIV = 4;
constexpr int COL_OTHER_LEVEL = 5;
constexpr int COL_OTHER_EVENT = 6;
constexpr int COL_OTHER_PTS = 7;
constexpr int COL_OTHER_SCORE = 8; // Run score
constexpr int COL_ITEM_TYPE = 0;
constexpr int COL_ITEM_NAME = 1;
constexpr int COL_ITEM_COMMENT = 2;


class CDlgListViewerDataColumns
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataColumns)
public:
	CDlgListViewerDataColumns(size_t inInitSize = 0)
	{
		if (0 < inInitSize)
			m_Columns.reserve(inInitSize);
	}
	~CDlgListViewerDataColumns()
	{
	}

	bool InsertColumn(CReportListCtrl* inList, int inIndex, wxString const& inName)
	{
		int col = static_cast<int>(m_Columns.size());
		for (int i = 0; i < col; ++i)
			if (m_Columns[i] == inIndex)
				return false;
		inList->InsertColumn(col, inName);
		m_Columns.push_back(inIndex);
		return true;
	}
	void SetColumnWidths(CReportListCtrl* inList)
	{
		for (int i = 0; i < static_cast<int>(m_Columns.size()); ++i)
			inList->SetColumnWidth(i, wxLIST_AUTOSIZE_USEHEADER);
	}
	int GetIndex(int inColumn)
	{
		if (0 <= inColumn && inColumn < static_cast<int>(m_Columns.size()))
			return m_Columns[inColumn];
		return -1;
	}
	int NumColumns() const
	{
		return static_cast<int>(m_Columns.size());
	}

private:
	std::vector<int> m_Columns;
};
typedef std::shared_ptr<CDlgListViewerDataColumns> CDlgListViewerDataColumnsPtr;

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerData : public CListData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerData)
public:
	CDlgListViewerData()
	{
	}
	~CDlgListViewerData()
	{
	}

	void OnNeedListItem(long iCol, wxListItem& info) const override;

	virtual int OnNeedIcon() const = 0;
	virtual int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const = 0;
};


void CDlgListViewerData::OnNeedListItem(long iCol, wxListItem& info) const
{
	info.SetMask(info.GetMask() | wxLIST_MASK_TEXT);
	info.SetText(OnNeedText(iCol));
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataExisting : public CDlgListViewerData
{
	friend class CDlgListViewerDataRun;
	friend class CDlgListViewerDataMultiQ;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataExisting)
public:
	CDlgListViewerDataExisting(CDlgListViewerDataColumnsPtr const& inColData, ARBDogExistingPointsPtr const& inExisting)
		: m_ColData(inColData)
		, m_Existing(inExisting)
	{
		assert(m_ColData);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDogExistingPointsPtr m_Existing;
};
typedef std::shared_ptr<CDlgListViewerDataExisting> CDlgListViewerDataExistingPtr;


wxString CDlgListViewerDataExisting::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Existing->GetDate().GetString();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		str << m_Existing->GetPoints();
		break;
	case COL_RUN_MQ_VENUE:
		str = m_Existing->GetVenue();
		break;
	case COL_RUN_MQ_DIV:
		str = m_Existing->GetDivision();
		break;
	case COL_RUN_MQ_LEVEL:
		str = m_Existing->GetLevel();
		break;
	case COL_RUN_MQ_EVENT:
		str = m_Existing->GetEvent();
		break;
	case COL_RUN_MQ_SUBNAME:
		str = m_Existing->GetSubName();
		break;
	case COL_RUN_MQ_LOCATION:
		str = _("IDS_EXISTING_POINTS");
		break;
	default:
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataRun : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataRun)
public:
	CDlgListViewerDataRun(
		CDlgListViewerDataColumnsPtr const& inColData,
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		ARBConfigScoringPtr const& inScoring,
		ScoringRunInfo::ScoringDetail inScoringDetail)
		: m_ColData(inColData)
		, m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_Scoring(inScoring)
		, m_ScoringDetail(inScoringDetail)
	{
		assert(m_ColData);
		// Note, some users have changed NADAC to remove Novice A/B and only
		// have Novice (no sublevels). This means during a config update,
		// all hell will break loose. Don't bother asserting here...
		// assert(inScoring);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDogPtr m_Dog;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	ARBConfigScoringPtr m_Scoring;
	ScoringRunInfo::ScoringDetail m_ScoringDetail;
};
typedef std::shared_ptr<CDlgListViewerDataRun> CDlgListViewerDataRunPtr;


wxString CDlgListViewerDataRun::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Run->GetDate().GetString();
		break;
	case COL_RUN_MQ_STATUS: // Only happens for scoring detail items.
		switch (m_ScoringDetail)
		{
		case ScoringRunInfo::ScoringDetail::None:
			break;
		case ScoringRunInfo::ScoringDetail::Deleted:
			str = _("IDS_DELETED");
			break;
		case ScoringRunInfo::ScoringDetail::Changed:
			str = _("IDS_CHANGED");
			break;
		}
		break;
	case COL_RUN_MQ_DOG: // Only happens for scoring detail items.
		if (m_Dog)
			str = m_Dog->GetCallName();
		break;
	case COL_RUN_MQ_Q:
		str = m_Run->GetQ().str();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		if (m_Run->GetQ().Qualified() && m_Scoring)
			str << m_Run->GetTitlePoints(m_Scoring);
		else
			str = L"0";
		break;
	case COL_RUN_MQ_VENUE:
		if (m_Run->GetClub())
			str = m_Run->GetClub()->GetVenue();
		break;
	case COL_RUN_MQ_DIV:
		str = m_Run->GetDivision();
		break;
	case COL_RUN_MQ_LEVEL:
		str = m_Run->GetLevel();
		break;
	case COL_RUN_MQ_EVENT:
		str = m_Run->GetEvent();
		break;
	case COL_RUN_MQ_SUBNAME:
		str = m_Run->GetSubName();
		break;
	case COL_RUN_MQ_LOCATION:
		str = m_Trial->GetLocation();
		break;
	case COL_RUN_MQ_CLUB:
		if (m_Run->GetClub())
			str = m_Run->GetClub()->GetName();
		break;
	case COL_RUN_MQ_JUDGE:
		str = m_Run->GetJudge();
		break;
	case COL_RUN_MQ_PLACE:
		str << m_Run->GetPlace();
		break;
	case COL_RUN_MQ_INCLASS:
		str << m_Run->GetInClass();
		break;
	case COL_RUN_MQ_QD:
		str << m_Run->GetDogsQd();
		break;
	case COL_RUN_MQ_SPEED:
		if (m_Scoring)
			str << m_Run->GetSpeedPoints(m_Scoring);
		break;
	case COL_RUN_MQ_PARTNERS:
		for (ARBDogRunPartnerList::const_iterator iter2 = m_Run->GetPartners().begin();
			 iter2 != m_Run->GetPartners().end();
			 ++iter2)
		{
			if (iter2 != m_Run->GetPartners().begin())
				str << L", ";
			str << (*iter2)->GetHandler() << L"/" << (*iter2)->GetDog();
		}
		break;
	default:
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataMultiQ : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataMultiQ)
public:
	CDlgListViewerDataMultiQ(
		CDlgListViewerDataColumnsPtr const& inColData,
		ARBDate const& inDate,
		ARBDogTrialPtr const& inTrial,
		wxString const& inClub)
		: m_ColData(inColData)
		, m_Date(inDate)
		, m_Trial(inTrial)
		, m_Club(inClub)
	{
		assert(m_ColData);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDate m_Date;
	ARBDogTrialPtr m_Trial;
	wxString m_Club;
};
typedef std::shared_ptr<CDlgListViewerDataMultiQ> CDlgListViewerDataMultiQPtr;


wxString CDlgListViewerDataMultiQ::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Date.GetString();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		str = L"1";
		break;
	case COL_RUN_MQ_LOCATION:
		str = m_Trial->GetLocation();
		break;
	case COL_RUN_MQ_CLUB:
		str = m_Club;
		break;
	default:
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

int CDlgListViewerDataExisting::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting
		= std::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(inRow2);
	CDlgListViewerDataRunPtr pDataRun = std::dynamic_pointer_cast<CDlgListViewerDataRun, CDlgListViewerData>(inRow2);
	CDlgListViewerDataMultiQPtr pDataMultiQ
		= std::dynamic_pointer_cast<CDlgListViewerDataMultiQ, CDlgListViewerData>(inRow2);
	if (!pDataExisting && !pDataRun && !pDataMultiQ)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	case COL_RUN_MQ_DATE:
	{
		ARBDate date1 = m_Existing->GetDate();
		ARBDate date2;
		if (pDataExisting)
			date2 = pDataExisting->m_Existing->GetDate();
		else if (pDataRun)
			date2 = pDataRun->m_Run->GetDate();
		else if (pDataMultiQ)
			date2 = pDataMultiQ->m_Date;
		if (date1 < date2)
			return -1;
		else if (date1 > date2)
			return 1;
		else
			return 0;
	}
	break;
	case COL_RUN_MQ_TITLE_PTS:
	{
		double pts1 = m_Existing->GetPoints();
		double pts2 = 0.0;
		if (pDataExisting)
			pts2 = pDataExisting->m_Existing->GetPoints();
		else if (pDataRun)
		{
			if (pDataRun->m_Run->GetQ().Qualified() && pDataRun->m_Scoring)
				pts2 = pDataRun->m_Run->GetTitlePoints(pDataRun->m_Scoring);
		}
		else if (pDataMultiQ)
		{
			pts2 = 1.0;
		}
		if (pts1 < pts2)
			return -1;
		else if (pts1 > pts2)
			return 1;
		else
			return 0;
	}
	break;

	default:
	case COL_RUN_MQ_STATUS:
	case COL_RUN_MQ_DOG:
	case COL_RUN_MQ_Q:
	case COL_RUN_MQ_VENUE:
	case COL_RUN_MQ_DIV:
	case COL_RUN_MQ_LEVEL:
	case COL_RUN_MQ_EVENT:
	case COL_RUN_MQ_SUBNAME:
	case COL_RUN_MQ_LOCATION:
	case COL_RUN_MQ_CLUB:
	case COL_RUN_MQ_JUDGE:
	case COL_RUN_MQ_PLACE:
	case COL_RUN_MQ_INCLASS:
	case COL_RUN_MQ_QD:
	case COL_RUN_MQ_SPEED:
	case COL_RUN_MQ_PARTNERS:
		str1 = OnNeedText(inCol);
		str2 = inRow2->OnNeedText(inCol);
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////

int CDlgListViewerDataRun::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting
		= std::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(inRow2);
	CDlgListViewerDataRunPtr pDataRun = std::dynamic_pointer_cast<CDlgListViewerDataRun, CDlgListViewerData>(inRow2);
	if (!pDataExisting && !pDataRun)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	case COL_RUN_MQ_DATE:
	{
		ARBDate date1 = m_Run->GetDate();
		ARBDate date2;
		if (pDataExisting)
			date2 = pDataExisting->m_Existing->GetDate();
		else
			date2 = pDataRun->m_Run->GetDate();
		if (date1 < date2)
			return -1;
		else if (date1 > date2)
			return 1;
		else
			return 0;
	}
	break;
	case COL_RUN_MQ_Q:
		if (pDataRun)
		{
			if (m_Run->GetQ() < pDataRun->m_Run->GetQ())
				return -1;
			else if (m_Run->GetQ() > pDataRun->m_Run->GetQ())
				return 1;
			else
				return 0;
		}
		else
			return 1;
		break;
	case COL_RUN_MQ_TITLE_PTS:
	{
		double pts1 = 0.0;
		if (m_Run->GetQ().Qualified() && m_Scoring)
			pts1 = m_Run->GetTitlePoints(m_Scoring);
		double pts2 = 0.0;
		if (pDataExisting)
			pts2 = pDataExisting->m_Existing->GetPoints();
		else
		{
			if (pDataRun->m_Run->GetQ().Qualified() && pDataRun->m_Scoring)
				pts2 = pDataRun->m_Run->GetTitlePoints(pDataRun->m_Scoring);
		}
		if (pts1 < pts2)
			return -1;
		else if (pts1 > pts2)
			return 1;
		else
			return 0;
	}
	break;
	case COL_RUN_MQ_PLACE:
		if (pDataRun)
		{
			if (m_Run->GetPlace() < pDataRun->m_Run->GetPlace())
				return -1;
			else if (m_Run->GetPlace() > pDataRun->m_Run->GetPlace())
				return 1;
			else
				return 0;
		}
		else
			return 1;
		break;
	case COL_RUN_MQ_INCLASS:
		if (pDataRun)
		{
			if (m_Run->GetInClass() < pDataRun->m_Run->GetInClass())
				return -1;
			else if (m_Run->GetInClass() > pDataRun->m_Run->GetInClass())
				return 1;
			else
				return 0;
		}
		else
			return 1;
		break;
	case COL_RUN_MQ_QD:
		if (pDataRun)
		{
			if (m_Run->GetDogsQd() < pDataRun->m_Run->GetDogsQd())
				return -1;
			else if (m_Run->GetDogsQd() > pDataRun->m_Run->GetDogsQd())
				return 1;
			else
				return 0;
		}
		else
			return 1;
		break;
	case COL_RUN_MQ_SPEED:
		if (pDataRun)
		{
			if (m_Scoring && !pDataRun->m_Scoring)
				return 1;
			else if (!m_Scoring && pDataRun->m_Scoring)
				return -1;
			else
			{
				short pts1 = m_Run->GetSpeedPoints(m_Scoring);
				short pts2 = pDataRun->m_Run->GetSpeedPoints(pDataRun->m_Scoring);
				if (pts1 < pts2)
					return -1;
				else if (pts1 > pts2)
					return 1;
				else
					return 0;
			}
		}
		else
			return 1;
		break;
	default:
	case COL_RUN_MQ_VENUE:
	case COL_RUN_MQ_DIV:
	case COL_RUN_MQ_LEVEL:
	case COL_RUN_MQ_EVENT:
	case COL_RUN_MQ_SUBNAME:
	case COL_RUN_MQ_LOCATION:
	case COL_RUN_MQ_CLUB:
	case COL_RUN_MQ_JUDGE:
	case COL_RUN_MQ_PARTNERS:
		str1 = OnNeedText(inCol);
		str2 = inRow2->OnNeedText(inCol);
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}


int CDlgListViewerDataMultiQ::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting
		= std::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(inRow2);
	CDlgListViewerDataMultiQPtr pData = std::dynamic_pointer_cast<CDlgListViewerDataMultiQ, CDlgListViewerData>(inRow2);
	if (!pDataExisting && !pData)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	case COL_RUN_MQ_DATE:
	{
		ARBDate date1 = m_Date;
		ARBDate date2;
		if (pDataExisting)
			date2 = pDataExisting->m_Existing->GetDate();
		else
			date2 = pData->m_Date;
		if (date1 < date2)
			return -1;
		else if (date1 > date2)
			return 1;
		else
			return 0;
	}
	break;
	default:
	case COL_RUN_MQ_CLUB:
	case COL_RUN_MQ_LOCATION:
		str1 = OnNeedText(inCol);
		str2 = inRow2->OnNeedText(inCol);
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataLifetime : public CDlgListViewerData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataLifetime)
public:
	CDlgListViewerDataLifetime(CDlgListViewerDataColumnsPtr const& inColData, LifeTimePointInfoPtr const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	LifeTimePointInfoPtr m_info;
};
typedef std::shared_ptr<CDlgListViewerDataLifetime> CDlgListViewerDataLifetimePtr;


wxString CDlgListViewerDataLifetime::OnNeedText(long iCol) const
{
	wxString str;
	if (m_info)
	{
		switch (m_ColData->GetIndex(iCol))
		{
		case COL_OTHER_DIV:
			str = m_info->sort1;
			break;
		case COL_OTHER_LEVEL:
			str = m_info->sort2;
			break;
		case COL_OTHER_PTS:
			if (0 < m_info->filtered)
				str << m_info->points - m_info->filtered << L" (" << m_info->points << L")";
			else
				str << m_info->points;
			break;
		default:
			break;
		}
	}
	return str;
}


int CDlgListViewerDataLifetime::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataLifetimePtr pData
		= std::dynamic_pointer_cast<CDlgListViewerDataLifetime, CDlgListViewerData>(inRow2);
	if (!pData)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_OTHER_DIV:
		str1 = m_info->sort1;
		str2 = pData->m_info->sort1;
		break;
	case COL_OTHER_LEVEL:
		str1 = m_info->sort2;
		str2 = pData->m_info->sort2;
		break;
	case COL_OTHER_PTS:
		if (m_info->points < pData->m_info->points)
			return -1;
		else if (m_info->points > pData->m_info->points)
			return 1;
		else
			return 0;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataOther : public CDlgListViewerData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataOther)
public:
	CDlgListViewerDataOther(CDlgListViewerDataColumnsPtr const& inColData, OtherPtInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	OtherPtInfo m_info;
};
typedef std::shared_ptr<CDlgListViewerDataOther> CDlgListViewerDataOtherPtr;


wxString CDlgListViewerDataOther::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_OTHER_DATE:
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetDate().GetString();
		else
			str = m_info.m_pRun->GetDate().GetString();
		break;
	case COL_OTHER_NAME: // Trial/Existing Points
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetTypeName();
		else
			str = m_info.m_pTrial->GetLocation();
		break;
	case COL_OTHER_CLUB:
		if (m_info.m_pExisting)
			str << L"[" << _("IDS_EXISTING_POINTS") << L"]";
		else if (m_info.m_pRun && m_info.m_pRun->GetClub())
			str = m_info.m_pRun->GetClub()->GetName();
		break;
	case COL_OTHER_VENUE:
		str = m_info.m_Venue;
		break;
	case COL_OTHER_DIV:
		str = m_info.m_Div;
		break;
	case COL_OTHER_LEVEL:
		str = m_info.m_Level;
		break;
	case COL_OTHER_EVENT:
		str = m_info.m_Event;
		break;
	case COL_OTHER_PTS:
		str << m_info.m_Points;
		break;
	case COL_OTHER_SCORE:
		if (m_info.m_bScore)
			str << m_info.m_Score;
		break;
	default:
		break;
	}
	return str;
}


int CDlgListViewerDataOther::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataOtherPtr pData = std::dynamic_pointer_cast<CDlgListViewerDataOther, CDlgListViewerData>(inRow2);
	if (!pData)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_OTHER_DATE:
	{
		ARBDate date1, date2;
		if (m_info.m_pExisting)
			date1 = m_info.m_pExisting->GetDate();
		else
			date1 = m_info.m_pRun->GetDate();
		if (pData->m_info.m_pExisting)
			date2 = pData->m_info.m_pExisting->GetDate();
		else
			date2 = pData->m_info.m_pRun->GetDate();
		if (date1 < date2)
			return -1;
		else if (date1 > date2)
			return 1;
		else
			return 0;
	}
	case COL_OTHER_NAME: // Trial/Existing Points
		if (m_info.m_pExisting)
			str1 = m_info.m_pExisting->GetTypeName();
		else
			str1 = m_info.m_pTrial->GetLocation();
		if (pData->m_info.m_pExisting)
			str2 = pData->m_info.m_pExisting->GetTypeName();
		else
			str2 = pData->m_info.m_pTrial->GetLocation();
		break;
	case COL_OTHER_CLUB:
		if (!m_info.m_pExisting && m_info.m_pRun && m_info.m_pRun->GetClub())
			str1 = m_info.m_pRun->GetClub()->GetName();
		if (!pData->m_info.m_pExisting && pData->m_info.m_pRun && pData->m_info.m_pRun->GetClub())
			str2 = pData->m_info.m_pRun->GetClub()->GetName();
		break;
	case COL_OTHER_VENUE:
		str1 = m_info.m_Venue;
		str2 = pData->m_info.m_Venue;
		break;
	case COL_OTHER_DIV:
		str1 = m_info.m_Div;
		str2 = pData->m_info.m_Div;
		break;
	case COL_OTHER_LEVEL:
		str1 = m_info.m_Level;
		str2 = pData->m_info.m_Level;
		break;
	case COL_OTHER_EVENT:
		str1 = m_info.m_Event;
		str2 = pData->m_info.m_Event;
		break;
	case COL_OTHER_PTS:
		if (m_info.m_Points < pData->m_info.m_Points)
			return -1;
		else if (m_info.m_Points > pData->m_info.m_Points)
			return 1;
		else
			return 0;
	case COL_OTHER_SCORE:
		if (m_info.m_bScore && pData->m_info.m_bScore)
		{
			if (m_info.m_Score < pData->m_info.m_Score)
				return -1;
			else if (m_info.m_Score > pData->m_info.m_Score)
				return 1;
			else
				return 0;
		}
		else if (!m_info.m_bScore && pData->m_info.m_bScore)
			return -1;
		else if (m_info.m_bScore && !pData->m_info.m_bScore)
			return 1;
		else
			return 0;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataItem : public CDlgListViewerData
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgListViewerDataItem)
public:
	CDlgListViewerDataItem(CDlgListViewerDataColumnsPtr const& inColData, CFindItemInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	wxString OnNeedText(long iCol) const override;
	int OnNeedIcon() const override
	{
		return -1;
	}
	int Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const override;

private:
	CDlgListViewerDataColumnsPtr m_ColData;
	CFindItemInfo m_info;
};
typedef std::shared_ptr<CDlgListViewerDataItem> CDlgListViewerDataItemPtr;


wxString CDlgListViewerDataItem::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_ITEM_TYPE:
	{
		switch (m_info.type)
		{
		case ARBInfoType::Club:
			str = _("IDS_COL_CLUB");
			break;
		case ARBInfoType::Judge:
			str = _("IDS_COL_JUDGE");
			break;
		case ARBInfoType::Location:
			str = _("IDS_COL_LOCATION");
			break;
		}
	}
	break;

	case COL_ITEM_NAME:
		str = m_info.name;
		break;

	case COL_ITEM_COMMENT:
		if (m_info.pItem)
			str = m_info.pItem->GetComment();
		break;

	default:
		break;
	}
	return str;
}


int CDlgListViewerDataItem::Compare(CDlgListViewerDataPtr const& inRow2, int inCol) const
{
	CDlgListViewerDataItemPtr pData = std::dynamic_pointer_cast<CDlgListViewerDataItem, CDlgListViewerData>(inRow2);
	if (!pData)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_ITEM_TYPE:
		switch (m_info.type)
		{
		case ARBInfoType::Club:
			str1 = _("IDS_COL_CLUB");
			break;
		case ARBInfoType::Judge:
			str1 = _("IDS_COL_JUDGE");
			break;
		case ARBInfoType::Location:
			str1 = _("IDS_COL_LOCATION");
			break;
		}
		switch (pData->m_info.type)
		{
		case ARBInfoType::Club:
			str2 = _("IDS_COL_CLUB");
			break;
		case ARBInfoType::Judge:
			str2 = _("IDS_COL_JUDGE");
			break;
		case ARBInfoType::Location:
			str2 = _("IDS_COL_LOCATION");
			break;
		}
		break;

	case COL_ITEM_NAME:
		str1 = m_info.name;
		str2 = pData->m_info.name;
		break;

	case COL_ITEM_COMMENT:
		if (m_info.pItem)
			str1 = m_info.pItem->GetComment();
		if (pData->m_info.pItem)
			str1 = pData->m_info.pItem->GetComment();
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CDlgListViewer dialog

struct ListSortInfo : public SortInfo
{
	CDlgListViewer* pThis;

	ListSortInfo(CDlgListViewer* This, int nCol)
		: SortInfo(nCol)
		, pThis(This)
	{
	}
};


int wxCALLBACK CompareRows(CListDataPtr const& item1, CListDataPtr const& item2, SortInfo const* pSortInfo)
{
	ListSortInfo const* pInfo = dynamic_cast<ListSortInfo const*>(pSortInfo);
	assert(pInfo);

	CDlgListViewerDataPtr pRow1 = std::dynamic_pointer_cast<CDlgListViewerData, CListData>(item1);
	CDlgListViewerDataPtr pRow2 = std::dynamic_pointer_cast<CDlgListViewerData, CListData>(item2);

	if (!pRow1 || !pRow2)
		return 0;
	int iCol = std::abs(pInfo->nCol);
	int nRet = pRow1->Compare(pRow2, iCol - 1);
	if (0 > pInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
void InsertRun(
	CAgilityBookDoc* pDoc,
	CReportListCtrl* ctrlList,
	CDlgListViewerDataColumnsPtr const& pColData,
	int& iItem,
	ARBDogPtr const& pDog,
	ARBDogTrialPtr const& pTrial,
	ARBDogRunPtr const& pRun,
	ScoringRunInfo::ScoringDetail scoringDetail)
{
	ARBConfigScoringPtr pScoring;
	if (pRun->GetClub())
		pDoc->Book().GetConfig().GetVenues().FindEvent(
			pRun->GetClub()->GetVenue(),
			pRun->GetEvent(),
			pRun->GetDivision(),
			pRun->GetLevel(),
			pRun->GetDate(),
			nullptr,
			&pScoring);
	if (pScoring)
	{
		if (pScoring->HasSpeedPts())
			pColData->InsertColumn(ctrlList, COL_RUN_MQ_SPEED, _("IDS_SPEEDPTS"));
	}
	if (0 < pRun->GetPartners().size())
		pColData->InsertColumn(ctrlList, COL_RUN_MQ_PARTNERS, _("IDS_PARTNERS"));

	CDlgListViewerDataRunPtr data(
		std::make_shared<CDlgListViewerDataRun>(pColData, pDog, pTrial, pRun, pScoring, scoringDetail));
	ctrlList->InsertItem(data);
}
} // namespace

/////////////////////////////////////////////////////////////////////////////
// Viewing runs

// TODO: not getting selected when selecting 2nd item via shift-click (wx bug)
// Listen to focused because of https://github.com/wxWidgets/wxWidgets/issues/4541
// Use EventTable because Binding allows a focused event after destroy on mac
constexpr int LIST_CTRL = 1000;
wxBEGIN_EVENT_TABLE(CDlgListViewer, wxDialog)
	EVT_LIST_ITEM_FOCUSED(LIST_CTRL, CDlgListViewer::OnItemFocused)
wxEND_EVENT_TABLE()

CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	RunInfoData const* inData,
	std::list<RunInfo> const& inRuns,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	std::set<wxString> subNames;
	std::vector<ARBDogExistingPointsPtr> existingRuns;

	std::list<RunInfo>::const_iterator iterRuns;
	for (iterRuns = inRuns.begin(); iterRuns != inRuns.end(); ++iterRuns)
	{
		if (get<TUPLE_RUN>(*iterRuns)->GetSubName().length())
			subNames.insert(get<TUPLE_RUN>(*iterRuns)->GetSubName());
	}
	if (inData)
	{
		for (ARBDogExistingPointsList::const_iterator iter = inData->m_Dog->GetExistingPoints().begin();
			 iter != inData->m_Dog->GetExistingPoints().end();
			 ++iter)
		{
			ARBDogExistingPointsPtr pExisting = *iter;
			if (ARBExistingPointType::Title == pExisting->GetType()
				&& pExisting->GetVenue() == inData->m_Venue->GetName()
				&& pExisting->GetDivision() == inData->m_Div->GetName()
				&& (pExisting->GetLevel() == inData->m_Level->GetName()
					|| inData->m_Level->GetSubLevels().FindSubLevel(pExisting->GetLevel()))
				&& pExisting->GetEvent() == inData->m_Event->GetName())
			{
				existingRuns.push_back(pExisting);
				if (0 < pExisting->GetSubName().length())
					subNames.insert(pExisting->GetSubName());
			}
		}
	}
	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(10));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_Q, _("IDS_COL_Q"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, _("IDS_COL_TITLE_PTS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_VENUE, _("IDS_COL_VENUE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DIV, _("IDS_COL_DIVISION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LEVEL, _("IDS_COL_LEVEL"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_EVENT, _("IDS_COL_EVENT"));
	if (0 < subNames.size())
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_SUBNAME, _("IDS_COL_SUBNAME"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, _("IDS_COL_LOCATION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, _("IDS_COL_CLUB"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_JUDGE, _("IDS_COL_JUDGE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_PLACE, _("IDS_COL_PLACE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_INCLASS, _("IDS_COL_IN_CLASS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_QD, _("IDS_COL_Q_D"));
	int iItem = 0;
	if (inData)
	{
		for (std::vector<ARBDogExistingPointsPtr>::iterator iter = existingRuns.begin(); iter != existingRuns.end();
			 ++iter)
		{
			CDlgListViewerDataExistingPtr data(std::make_shared<CDlgListViewerDataExisting>(pColData, *iter));
			m_ctrlList->InsertItem(data);
		}
	}
	for (iterRuns = inRuns.begin(); iterRuns != inRuns.end(); ++iterRuns)
	{
		ARBDogTrialPtr pTrial = get<TUPLE_TRIAL>(*iterRuns);
		ARBDogRunPtr pRun = get<TUPLE_RUN>(*iterRuns);
		if (CFilterOptions::Options().IsFilterEnabled())
		{
			if (pRun->IsFiltered())
				continue;
		}
		InsertRun(m_pDoc, m_ctrlList, pColData, iItem, ARBDogPtr(), pTrial, pRun, ScoringRunInfo::ScoringDetail::None);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


// Viewing runs affected by configuration changes
CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	std::list<ScoringRunInfo> const& inScoringRuns,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	std::set<wxString> subNames;
	std::list<ScoringRunInfo>::const_iterator iter;
	for (iter = inScoringRuns.begin(); iter != inScoringRuns.end(); ++iter)
	{
		if ((iter->m_Run)->GetSubName().length())
			subNames.insert((iter->m_Run)->GetSubName());
	}
	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(12));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_STATUS, _("IDS_COL_STATUS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DOG, _("IDS_COL_DOG"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_Q, _("IDS_COL_Q"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, _("IDS_COL_TITLE_PTS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_VENUE, _("IDS_COL_VENUE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DIV, _("IDS_COL_DIVISION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LEVEL, _("IDS_COL_LEVEL"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_EVENT, _("IDS_COL_EVENT"));
	if (0 < subNames.size())
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_SUBNAME, _("IDS_COL_SUBNAME"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, _("IDS_COL_LOCATION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, _("IDS_COL_CLUB"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_JUDGE, _("IDS_COL_JUDGE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_PLACE, _("IDS_COL_PLACE"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_INCLASS, _("IDS_COL_IN_CLASS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_QD, _("IDS_COL_Q_D"));
	int iItem = 0;
	for (iter = inScoringRuns.begin(); iter != inScoringRuns.end(); ++iter)
	{
		ScoringRunInfo const& info = *iter;
		InsertRun(m_pDoc, m_ctrlList, pColData, iItem, info.m_Dog, info.m_Trial, info.m_Run, info.m_ScoringDetail);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


// Viewing multi-Qs
CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	MultiQInfoData const* inData,
	std::set<MultiQdata> const& inMQs,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	std::vector<ARBDogExistingPointsPtr> existingRuns2;
	if (inData)
	{
		for (ARBDogExistingPointsList::const_iterator iter = inData->m_Dog->GetExistingPoints().begin();
			 iter != inData->m_Dog->GetExistingPoints().end();
			 ++iter)
		{
			ARBDogExistingPointsPtr pExisting = *iter;
			if (ARBExistingPointType::MQ == pExisting->GetType() && pExisting->GetVenue() == inData->m_Venue->GetName()
				&& pExisting->GetMultiQ() == inData->m_MultiQ->GetName())
			{
				existingRuns2.push_back(pExisting);
			}
		}
	}

	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(3));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, _("IDS_COL_TITLE_PTS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, _("IDS_COL_LOCATION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, _("IDS_COL_CLUB"));
	if (inData)
	{
		for (std::vector<ARBDogExistingPointsPtr>::iterator iter = existingRuns2.begin(); iter != existingRuns2.end();
			 ++iter)
		{
			CDlgListViewerDataExistingPtr data(std::make_shared<CDlgListViewerDataExisting>(pColData, *iter));
			m_ctrlList->InsertItem(data);
		}
	}
	for (std::set<MultiQdata>::const_iterator iter = inMQs.begin(); iter != inMQs.end(); ++iter)
	{
		CDlgListViewerDataMultiQPtr data(
			std::make_shared<CDlgListViewerDataMultiQ>(pColData, iter->date, iter->trial, iter->club));
		m_ctrlList->InsertItem(data);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


// Viewing lifetime data
CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	std::list<LifeTimePointInfoPtr> const& inLifetime,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(3));
	if (CAgilityBookOptions::GetViewLifetimePointsByEvent())
	{
		pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, _("IDS_COL_EVENT"));
	}
	else
	{
		pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, _("IDS_COL_DIVISION"));
		pColData->InsertColumn(m_ctrlList, COL_OTHER_LEVEL, _("IDS_COL_LEVEL"));
	}
	pColData->InsertColumn(m_ctrlList, COL_OTHER_PTS, _("IDS_COL_POINTS"));
	for (std::list<LifeTimePointInfoPtr>::const_iterator iter = inLifetime.begin(); iter != inLifetime.end(); ++iter)
	{
		CDlgListViewerDataLifetimePtr data(std::make_shared<CDlgListViewerDataLifetime>(pColData, *iter));
		m_ctrlList->InsertItem(data);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


// Viewing other points
CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	std::list<OtherPtInfo> const& inRunList,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(8));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_OTHER_NAME, _("IDS_COL_TRIAL_EXISTPTS"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_CLUB, _("IDS_COL_CLUB"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_VENUE, _("IDS_COL_VENUE"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, _("IDS_COL_DIVISION"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_LEVEL, _("IDS_COL_LEVEL"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_EVENT, _("IDS_COL_EVENT"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_PTS, _("IDS_COL_POINTS"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_SCORE, _("IDS_COL_SCORE"));
	for (std::list<OtherPtInfo>::const_iterator iter = inRunList.begin(); iter != inRunList.end(); ++iter)
	{
		CDlgListViewerDataOtherPtr data(std::make_shared<CDlgListViewerDataOther>(pColData, *iter));
		m_ctrlList->InsertItem(data);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


CDlgListViewer::CDlgListViewer(
	CAgilityBookDoc* inDoc,
	wxString const& inCaption,
	std::vector<CFindItemInfo> const& inItems,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(nullptr)
	, m_ctrlCopy(nullptr)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
	, m_trace("CDlgListViewer")
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(std::make_shared<CDlgListViewerDataColumns>(3));
	pColData->InsertColumn(m_ctrlList, COL_ITEM_TYPE, _("IDS_COL_TYPE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_ITEM_NAME, _("IDS_COL_NAME"));
	pColData->InsertColumn(m_ctrlList, COL_ITEM_COMMENT, _("IDS_COL_COMMENTS"));
	for (std::vector<CFindItemInfo>::const_iterator iter = inItems.begin(); iter != inItems.end(); ++iter)
	{
		CDlgListViewerDataItemPtr data(std::make_shared<CDlgListViewerDataItem>(pColData, *iter));
		m_ctrlList->InsertItem(data);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


bool CDlgListViewer::Create(wxString const& inCaption, wxWindow* pParent)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	if (!wxDialog::Create(
			pParent,
			wxID_ANY,
			inCaption,
			wxDefaultPosition,
			wxDefaultSize,
			wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	// Controls (these are done first to control tab order)

	m_ctrlList = new CReportListCtrl(
		this,
		wxDefaultPosition,
		wxDLG_UNIT(this, wxSize(300, 100)),
		false,
		CReportListCtrl::SortHeader::Sort,
		true);
	m_ctrlList->SetId(LIST_CTRL);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_COL_CLICK, &CDlgListViewer::OnColumnClick, this);
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_SELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->Bind(wxEVT_COMMAND_LIST_ITEM_DESELECTED, [this](wxListEvent& evt) { UpdateControls(); });
	m_ctrlList->SetHelpText(_("HIDC_LIST_VIEWER"));
	m_ctrlList->SetToolTip(_("HIDC_LIST_VIEWER"));

	m_ctrlCopy = new wxButton(this, wxID_ANY, _("IDC_LIST_COPY"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgListViewer::OnCopy, this);
	m_ctrlCopy->SetHelpText(_("HIDC_LIST_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_LIST_COPY"));

	wxButton* btnClose = new wxButton(this, wxID_OK, _("IDC_LIST_CLOSE"), wxDefaultPosition, wxDefaultSize, 0);
	btnClose->SetDefault();

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlList, 1, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->AddStretchSpacer();
	sizerBtns->Add(m_ctrlCopy, 0, wxRIGHT, padding.Controls());
	sizerBtns->Add(btnClose);

	bSizer->Add(sizerBtns, 0, wxEXPAND | wxALL, padding.Controls());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();

	btnClose->SetFocus();
	return true;
}


void CDlgListViewer::FinishCreate()
{
	ListSortInfo sortInfo(this, m_SortColumn);
	m_ctrlList->SortItems(CompareRows, &sortInfo);
	m_ctrlList->SetColumnSort(std::abs(m_SortColumn) - 1, m_SortColumn);
	m_ctrlCopy->Enable(0 < m_ctrlList->GetItemCount());
}


void CDlgListViewer::UpdateControls()
{
	int items = std::max(m_ctrlList->GetSelectedItemCount(), m_ctrlList->GetItemCount());
	m_ctrlCopy->Enable(0 < items);
}


void CDlgListViewer::OnItemFocused(wxListEvent& evt)
{
	UpdateControls();
}


void CDlgListViewer::OnColumnClick(wxListEvent& evt)
{
	m_ctrlList->SetColumnSort(std::abs(m_SortColumn) - 1, 0);
	int nBackwards = 1;
	if (m_SortColumn == evt.GetColumn() + 1)
		nBackwards = -1;
	m_SortColumn = (evt.GetColumn() + 1) * nBackwards;
	ListSortInfo sortInfo(this, m_SortColumn);
	m_ctrlList->SortItems(CompareRows, &sortInfo);
	m_ctrlList->SetColumnSort(std::abs(m_SortColumn) - 1, m_SortColumn);
}


void CDlgListViewer::OnCopy(wxCommandEvent& evt)
{
	std::vector<long> indices;
	if (0 < m_ctrlList->GetSelectedItemCount())
		m_ctrlList->GetSelection(indices);
	else
	{
		long cnt = m_ctrlList->GetItemCount();
		indices.reserve(cnt);
		for (long i = 0; i < cnt; ++i)
			indices.push_back(i);
	}

	if (0 < indices.size())
	{
		CClipboardDataWriter clpData;
		if (!clpData.isOkay())
			return;

		wxString data;
		wxString html;
		CClipboardDataTable table(data, html);

		if (1 < indices.size() || indices.size() == static_cast<size_t>(m_ctrlList->GetItemCount()))
		{
			std::vector<wxString> line;
			m_ctrlList->GetPrintLine(-1, line);
			table.StartLine();
			for (int i = 0; i < static_cast<int>(line.size()); ++i)
			{
				table.Cell(i, line[i]);
			}
			table.EndLine();
		}

		// Now all the data.
		for (size_t idx = 0; idx < indices.size(); ++idx)
		{
			std::vector<wxString> line;
			m_ctrlList->GetPrintLine(indices[idx], line);
			table.StartLine();
			for (int i = 0; i < static_cast<int>(line.size()); ++i)
			{
				table.Cell(i, line[i]);
			}
			table.EndLine();
		}

		clpData.AddData(table);
		clpData.CommitData();
	}
}

} // namespace dconSoft
