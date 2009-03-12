/*
 * Copyright � 2004-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2009-01-28 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-02 DRC Show existing points in the list viewer.
 *                    Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-08-26 DRC Completed.
 * @li 2004-07-29 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgListViewer.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBConfig.h"
#include "ARBDate.h"
#include "ARBDog.h"
#include "ARBDogExistingPoints.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ClipBoard.h"
#include "ListCtrl.h"
#include "FilterOptions.h"

/////////////////////////////////////////////////////////////////////////////

// Run/MQ are shared in the DataExisting class, hence they need to use
// the same column values.
#define COL_RUN_MQ_DATE			0
#define COL_RUN_MQ_STATUS		1
#define COL_RUN_MQ_DOG			2
#define COL_RUN_MQ_Q			3
#define COL_RUN_MQ_TITLE_PTS	4
#define COL_RUN_MQ_VENUE		5
#define COL_RUN_MQ_DIV			6
#define COL_RUN_MQ_LEVEL		7
#define COL_RUN_MQ_EVENT		8
#define COL_RUN_MQ_SUBNAME		9
#define COL_RUN_MQ_LOCATION		10
#define COL_RUN_MQ_CLUB			11
#define COL_RUN_MQ_JUDGE		12
#define COL_RUN_MQ_PLACE		13
#define COL_RUN_MQ_INCLASS		14
#define COL_RUN_MQ_QD			15
#define COL_RUN_MQ_SPEED		16
#define COL_RUN_MQ_PARTNERS		17
#define COL_OTHER_DATE			0
#define COL_OTHER_NAME			1 // Trial, existing pt
#define COL_OTHER_CLUB			2
#define COL_OTHER_VENUE			3
#define COL_OTHER_DIV			4
#define COL_OTHER_LEVEL			5
#define COL_OTHER_EVENT			6
#define COL_OTHER_PTS			7
#define COL_ITEM_TYPE			0
#define COL_ITEM_NAME			1
#define COL_ITEM_COMMENT		2


class CDlgListViewerDataColumns
{
public:
	CDlgListViewerDataColumns(size_t inInitSize = 0)
	{
		if (0 < inInitSize)
			m_Columns.reserve(inInitSize);
	}
	~CDlgListViewerDataColumns() {}

	bool InsertColumn(
			CReportListCtrl* inList,
			int inIndex,
			wxString const& inName)
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
	int NumColumns() const	{return static_cast<int>(m_Columns.size());}

private:
	std::vector<int> m_Columns;
};
typedef tr1::shared_ptr<CDlgListViewerDataColumns> CDlgListViewerDataColumnsPtr;

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerData : public CListData
{
public:
	CDlgListViewerData() {}
	virtual ~CDlgListViewerData() {}

	virtual void OnNeedListItem(long iCol, wxListItem& info) const;

	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const = 0;
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
public:
	CDlgListViewerDataExisting(
			CDlgListViewerDataColumnsPtr inColData,
			ARBDogExistingPointsPtr inExisting)
		: m_ColData(inColData)
		, m_Existing(inExisting)
	{
		assert(m_ColData);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDogExistingPointsPtr m_Existing;
};
typedef tr1::shared_ptr<CDlgListViewerDataExisting> CDlgListViewerDataExistingPtr;


wxString CDlgListViewerDataExisting::OnNeedText(long iCol) const
{
	tstring str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Existing->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
		break;
	case COL_RUN_MQ_TITLE_PTS:
		{
			otstringstream tmp;
			tmp << m_Existing->GetPoints();
			str = tmp.str();
		}
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
	}
	return str.c_str();
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataRun : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
public:
	CDlgListViewerDataRun(
			CDlgListViewerDataColumnsPtr inColData,
			ARBDogPtr inDog,
			ARBDogTrialPtr inTrial,
			ARBDogRunPtr inRun,
			ARBConfigScoringPtr inScoring,
			ScoringRunInfo::eScoringDetail inScoringDetail)
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
		//assert(inScoring);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDogPtr m_Dog;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	ARBConfigScoringPtr m_Scoring;
	ScoringRunInfo::eScoringDetail m_ScoringDetail;
};
typedef tr1::shared_ptr<CDlgListViewerDataRun> CDlgListViewerDataRunPtr;


wxString CDlgListViewerDataRun::OnNeedText(long iCol) const
{
	otstringstream str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str << m_Run->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
		break;
	case COL_RUN_MQ_STATUS: // Only happens for scoring detail items.
		switch (m_ScoringDetail)
		{
		default:
			break;
		case ScoringRunInfo::eScoringDeleted:
			str << _("IDS_DELETED");
			break;
		case ScoringRunInfo::eScoringChanged:
			str << _("IDS_CHANGED");
			break;
		}
		break;
	case COL_RUN_MQ_DOG: // Only happens for scoring detail items.
		if (m_Dog)
			str << m_Dog->GetCallName();
		break;
	case COL_RUN_MQ_Q:
		str << m_Run->GetQ().str();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		if (m_Run->GetQ().Qualified() && m_Scoring)
			str << m_Run->GetTitlePoints(m_Scoring);
		else
			str << '0';
		break;
	case COL_RUN_MQ_VENUE:
		str << m_Trial->GetClubs().GetPrimaryClubVenue();
		break;
	case COL_RUN_MQ_DIV:
		str << m_Run->GetDivision();
		break;
	case COL_RUN_MQ_LEVEL:
		str << m_Run->GetLevel();
		break;
	case COL_RUN_MQ_EVENT:
		str << m_Run->GetEvent();
		break;
	case COL_RUN_MQ_SUBNAME:
		str << m_Run->GetSubName();
		break;
	case COL_RUN_MQ_LOCATION:
		str << m_Trial->GetLocation();
		break;
	case COL_RUN_MQ_CLUB:
		if (m_Trial->GetClubs().GetPrimaryClub())
			str << m_Trial->GetClubs().GetPrimaryClubName();
		break;
	case COL_RUN_MQ_JUDGE:
		str << m_Run->GetJudge();
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
				str << wxT(", ");
			str << (*iter2)->GetHandler().c_str();
			str << wxT("/");
			str << (*iter2)->GetDog().c_str();
		}
		break;
	}
	return str.str().c_str();
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataMultiQ : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
public:
	CDlgListViewerDataMultiQ(
			CDlgListViewerDataColumnsPtr inColData,
			ARBDate const& inDate,
			ARBDogTrialPtr inTrial)
		: m_ColData(inColData)
		, m_Date(inDate)
		, m_Trial(inTrial)
	{
		assert(m_ColData);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	ARBDate m_Date;
	ARBDogTrialPtr m_Trial;
};
typedef tr1::shared_ptr<CDlgListViewerDataMultiQ> CDlgListViewerDataMultiQPtr;


wxString CDlgListViewerDataMultiQ::OnNeedText(long iCol) const
{
	tstring str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Date.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
		break;
	case COL_RUN_MQ_TITLE_PTS:
		str = wxT("1");
		break;
	case COL_RUN_MQ_LOCATION:
		str = m_Trial->GetLocation();
		break;
	case COL_RUN_MQ_CLUB:
		if (m_Trial->GetClubs().GetPrimaryClub())
			str = m_Trial->GetClubs().GetPrimaryClubName();
		break;
	}
	return str.c_str();
}

/////////////////////////////////////////////////////////////////////////////

int CDlgListViewerDataExisting::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting = tr1::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(pRow2);
	CDlgListViewerDataRunPtr pDataRun = tr1::dynamic_pointer_cast<CDlgListViewerDataRun, CDlgListViewerData>(pRow2);
	CDlgListViewerDataMultiQPtr pDataMultiQ = tr1::dynamic_pointer_cast<CDlgListViewerDataMultiQ, CDlgListViewerData>(pRow2);
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
		str2 = pRow2->OnNeedText(inCol);
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

int CDlgListViewerDataRun::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting = tr1::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(pRow2);
	CDlgListViewerDataRunPtr pDataRun = tr1::dynamic_pointer_cast<CDlgListViewerDataRun, CDlgListViewerData>(pRow2);
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
		str2 = pRow2->OnNeedText(inCol);
		break;
	}
	if (str1 < str2)
		return -1;
	else if (str1 > str2)
		return 1;
	else
		return 0;
}


int CDlgListViewerDataMultiQ::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataExistingPtr pDataExisting = tr1::dynamic_pointer_cast<CDlgListViewerDataExisting, CDlgListViewerData>(pRow2);
	CDlgListViewerDataMultiQPtr pData = tr1::dynamic_pointer_cast<CDlgListViewerDataMultiQ, CDlgListViewerData>(pRow2);
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
		str2 = pRow2->OnNeedText(inCol);
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
public:
	CDlgListViewerDataLifetime(
			CDlgListViewerDataColumnsPtr inColData,
			LifeTimePointInfoPtr const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	LifeTimePointInfoPtr m_info;
};
typedef tr1::shared_ptr<CDlgListViewerDataLifetime> CDlgListViewerDataLifetimePtr;


wxString CDlgListViewerDataLifetime::OnNeedText(long iCol) const
{
	otstringstream str;
	if (m_info)
	{
		switch (m_ColData->GetIndex(iCol))
		{
		case COL_OTHER_DIV:
			str << m_info->sort1;
			break;
		case COL_OTHER_LEVEL:
			str << m_info->sort2;
			break;
		case COL_OTHER_PTS:
			if (0 < m_info->filtered)
				str << m_info->points - m_info->filtered << wxT(" (") << m_info->points << ')';
			else
				str << m_info->points;
			break;
		}
	}
	return str.str().c_str();
}


int CDlgListViewerDataLifetime::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataLifetimePtr pData = tr1::dynamic_pointer_cast<CDlgListViewerDataLifetime, CDlgListViewerData>(pRow2);
	if (!pData)
		return 0;
	tstring str1, str2;
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
public:
	CDlgListViewerDataOther(
			CDlgListViewerDataColumnsPtr inColData,
			OtherPtInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	OtherPtInfo m_info;
};
typedef tr1::shared_ptr<CDlgListViewerDataOther> CDlgListViewerDataOtherPtr;


wxString CDlgListViewerDataOther::OnNeedText(long iCol) const
{
	otstringstream str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_OTHER_DATE:
		if (m_info.m_pExisting)
			str << m_info.m_pExisting->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
		else
			str << m_info.m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints));
		break;
	case COL_OTHER_NAME: // Trial/Existing Points
		if (m_info.m_pExisting)
			str << m_info.m_pExisting->GetOtherPoints();
		else
			str << m_info.m_pTrial->GetLocation();
		break;
	case COL_OTHER_CLUB:
		if (m_info.m_pExisting)
			str << wxT('[') << _("IDS_EXISTING_POINTS") << wxT(']');
		else if (m_info.m_pTrial->GetClubs().GetPrimaryClub())
			str << m_info.m_pTrial->GetClubs().GetPrimaryClubName();
		break;
	case COL_OTHER_VENUE:
		str << m_info.m_Venue;
		break;
	case COL_OTHER_DIV:
		str << m_info.m_Div;
		break;
	case COL_OTHER_LEVEL:
		str << m_info.m_Level;
		break;
	case COL_OTHER_EVENT:
		str << m_info.m_Event;
		break;
	case COL_OTHER_PTS:
		str << m_info.m_Score;
		break;
	}
	return str.str().c_str();
}


int CDlgListViewerDataOther::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataOtherPtr pData = tr1::dynamic_pointer_cast<CDlgListViewerDataOther, CDlgListViewerData>(pRow2);
	if (!pData)
		return 0;
	tstring str1, str2;
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
			str1 = m_info.m_pExisting->GetOtherPoints();
		else
			str1 = m_info.m_pTrial->GetLocation();
		if (pData->m_info.m_pExisting)
			str2 = pData->m_info.m_pExisting->GetOtherPoints();
		else
			str2 = pData->m_info.m_pTrial->GetLocation();
		break;
	case COL_OTHER_CLUB:
		if (!m_info.m_pExisting && m_info.m_pTrial->GetClubs().GetPrimaryClub())
			str1 = m_info.m_pTrial->GetClubs().GetPrimaryClubName();
		if (!pData->m_info.m_pExisting && pData->m_info.m_pTrial->GetClubs().GetPrimaryClub())
			str2 = pData->m_info.m_pTrial->GetClubs().GetPrimaryClubName();
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
		if (m_info.m_Score < pData->m_info.m_Score)
			return -1;
		else if (m_info.m_Score > pData->m_info.m_Score)
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
public:
	CDlgListViewerDataItem(
			CDlgListViewerDataColumnsPtr inColData,
			CFindItemInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		assert(m_ColData);
	}
	virtual wxString OnNeedText(long iCol) const;
	virtual int OnNeedIcon() const		{return -1;}
	virtual int Compare(
			CDlgListViewerDataPtr pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumnsPtr m_ColData;
	CFindItemInfo m_info;
};
typedef tr1::shared_ptr<CDlgListViewerDataItem> CDlgListViewerDataItemPtr;


wxString CDlgListViewerDataItem::OnNeedText(long iCol) const
{
	wxString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_ITEM_TYPE:
		{
			switch (m_info.type)
			{
			case ARBInfo::eClubInfo:
				str = _("IDS_COL_CLUB");
				break;
			case ARBInfo::eJudgeInfo:
				str = _("IDS_COL_JUDGE");
				break;
			case ARBInfo::eLocationInfo:
				str = _("IDS_COL_LOCATION");
				break;
			}
		}
		break;

	case COL_ITEM_NAME:
		str = m_info.name.c_str();
		break;

	case COL_ITEM_COMMENT:
		if (m_info.pItem)
			str = m_info.pItem->GetComment().c_str();
		break;
	}
	return str;
}


int CDlgListViewerDataItem::Compare(
		CDlgListViewerDataPtr pRow2,
		int inCol) const
{
	CDlgListViewerDataItemPtr pData = tr1::dynamic_pointer_cast<CDlgListViewerDataItem, CDlgListViewerData>(pRow2);
	if (!pData)
		return 0;
	wxString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_ITEM_TYPE:
		switch (m_info.type)
		{
		case ARBInfo::eClubInfo:
			str1 = _("IDS_COL_CLUB");
			break;
		case ARBInfo::eJudgeInfo:
			str1 = _("IDS_COL_JUDGE");
			break;
		case ARBInfo::eLocationInfo:
			str1 = _("IDS_COL_LOCATION");
			break;
		}
		switch (pData->m_info.type)
		{
		case ARBInfo::eClubInfo:
			str2 = _("IDS_COL_CLUB");
			break;
		case ARBInfo::eJudgeInfo:
			str2 = _("IDS_COL_JUDGE");
			break;
		case ARBInfo::eLocationInfo:
			str2 = _("IDS_COL_LOCATION");
			break;
		}
		break;

	case COL_ITEM_NAME:
		str1 = m_info.name.c_str();
		str2 = pData->m_info.name.c_str();
		break;

	case COL_ITEM_COMMENT:
		if (m_info.pItem)
			str1 = m_info.pItem->GetComment().c_str();
		if (pData->m_info.pItem)
			str1 = pData->m_info.pItem->GetComment().c_str();
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

static struct SORT_COL_INFO
{
	CDlgListViewer* pThis;
	int nCol;
} s_SortInfo;


int wxCALLBACK CompareRows(
		long item1,
		long item2,
		long item3)
{
	CDlgListViewerDataPtr pRow1 = s_SortInfo.pThis->GetDataByData(item1);
	CDlgListViewerDataPtr pRow2 = s_SortInfo.pThis->GetDataByData(item2);
	if (!pRow1 || !pRow2)
		return 0;
	int iCol = abs(s_SortInfo.nCol);
	int nRet = pRow1->Compare(pRow2, iCol - 1);
	if (0 > s_SortInfo.nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////

static void InsertRun(
		CAgilityBookDoc* pDoc,
		CReportListCtrl* ctrlList,
		CDlgListViewerDataColumnsPtr pColData,
		int& iItem,
		ARBDogPtr pDog,
		ARBDogTrialPtr pTrial,
		ARBDogRunPtr pRun,
		ScoringRunInfo::eScoringDetail scoringDetail)
{
	ARBConfigScoringPtr pScoring;
	if (pTrial->GetClubs().GetPrimaryClub())
		 pDoc->Book().GetConfig().GetVenues().FindEvent(
			pTrial->GetClubs().GetPrimaryClubVenue(),
			pRun->GetEvent(),
			pRun->GetDivision(),
			pRun->GetLevel(),
			pRun->GetDate(),
			NULL,
			&pScoring);
	if (pScoring)
	{
		if (pScoring->HasSpeedPts())
			pColData->InsertColumn(ctrlList, COL_RUN_MQ_SPEED, _("IDS_SPEEDPTS"));
	}
	if (0 < pRun->GetPartners().size())
		pColData->InsertColumn(ctrlList, COL_RUN_MQ_PARTNERS, _("IDS_PARTNERS"));

	CDlgListViewerDataRunPtr data(new CDlgListViewerDataRun(pColData, pDog, pTrial, pRun, pScoring, scoringDetail));
	ctrlList->InsertItem(data);
}

/////////////////////////////////////////////////////////////////////////////
// Viewing runs

CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		RunInfoData const* inData,
		std::list<RunInfo> const& inRuns,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
{
	Create(inCaption, pParent);

	std::set<tstring> subNames;
	std::vector<ARBDogExistingPointsPtr> existingRuns;

	std::list<RunInfo>::const_iterator iter;
	for (iter = inRuns.begin(); iter != inRuns.end(); ++iter)
	{
		if ((iter->second)->GetSubName().length())
			subNames.insert((iter->second)->GetSubName());
	}
	if (inData)
	{
		for (ARBDogExistingPointsList::const_iterator iter = inData->m_Dog->GetExistingPoints().begin();
			iter != inData->m_Dog->GetExistingPoints().end();
			++iter)
		{
			ARBDogExistingPointsPtr pExisting = *iter;
			if (ARBDogExistingPoints::eRuns == pExisting->GetType()
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
	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(10));
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
		for (std::vector<ARBDogExistingPointsPtr>::iterator iter = existingRuns.begin();
			iter != existingRuns.end();
			++iter)
		{
			CDlgListViewerDataExistingPtr data(new CDlgListViewerDataExisting(pColData, *iter));
			m_ctrlList->InsertItem(data);
		}
	}
	for (iter = inRuns.begin(); iter != inRuns.end(); ++iter)
	{
		ARBDogTrialPtr pTrial = iter->first;
		ARBDogRunPtr pRun = iter->second;
		if (CFilterOptions::Options().IsFilterEnabled())
		{
			if (pRun->IsFiltered())
				continue;
		}
		InsertRun(m_pDoc, m_ctrlList, pColData, iItem,
			ARBDogPtr(), pTrial, pRun, ScoringRunInfo::eNotScoringDetail);
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
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
{
	Create(inCaption, pParent);

	std::set<tstring> subNames;
	std::list<ScoringRunInfo>::const_iterator iter;
	for (iter = inScoringRuns.begin(); iter != inScoringRuns.end(); ++iter)
	{
		if ((iter->m_Run)->GetSubName().length())
			subNames.insert((iter->m_Run)->GetSubName());
	}
	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(12));
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
		InsertRun(m_pDoc, m_ctrlList, pColData, iItem,
			info.m_Dog, info.m_Trial, info.m_Run, info.m_ScoringDetail);
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
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
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
			if (ARBDogExistingPoints::eMQ == pExisting->GetType()
			&& pExisting->GetVenue() == inData->m_Venue->GetName()
			&& pExisting->GetMultiQ() == inData->m_MultiQ->GetName())
			{
				existingRuns2.push_back(pExisting);
			}
		}
	}

	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(3));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, _("IDS_COL_TITLE_PTS"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, _("IDS_COL_LOCATION"));
	pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, _("IDS_COL_CLUB"));
	if (inData)
	{
		for (std::vector<ARBDogExistingPointsPtr>::iterator iter = existingRuns2.begin();
			iter != existingRuns2.end();
			++iter)
		{
			CDlgListViewerDataExistingPtr data(new CDlgListViewerDataExisting(pColData, *iter));
			m_ctrlList->InsertItem(data);
		}
	}
	for (std::set<MultiQdata>::const_iterator iter = inMQs.begin();
		iter != inMQs.end();
		++iter)
	{
		CDlgListViewerDataMultiQPtr data(new CDlgListViewerDataMultiQ(pColData, iter->first, iter->second));
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
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(3));
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
	for (std::list<LifeTimePointInfoPtr>::const_iterator iter = inLifetime.begin();
		iter != inLifetime.end();
		++iter)
	{
		CDlgListViewerDataLifetimePtr data(new CDlgListViewerDataLifetime(pColData, *iter));
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
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(8));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_DATE, _("IDS_COL_DATE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_OTHER_NAME, _("IDS_COL_TRIAL_EXISTPTS"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_CLUB, _("IDS_COL_CLUB"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_VENUE, _("IDS_COL_VENUE"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, _("IDS_COL_DIVISION"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_LEVEL, _("IDS_COL_LEVEL"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_EVENT, _("IDS_COL_EVENT"));
	pColData->InsertColumn(m_ctrlList, COL_OTHER_PTS, _("IDS_COL_POINTS"));
	for (std::list<OtherPtInfo>::const_iterator iter = inRunList.begin();
		iter != inRunList.end();
		++iter)
	{
		CDlgListViewerDataOtherPtr data(new CDlgListViewerDataOther(pColData, *iter));
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
	, m_ctrlList(NULL)
	, m_pDoc(inDoc)
	, m_SortColumn(1)
{
	Create(inCaption, pParent);

	CDlgListViewerDataColumnsPtr pColData(new CDlgListViewerDataColumns(3));
	pColData->InsertColumn(m_ctrlList, COL_ITEM_TYPE, _("IDS_COL_TYPE"));
	m_SortColumn = pColData->NumColumns();
	pColData->InsertColumn(m_ctrlList, COL_ITEM_NAME, _("IDS_COL_NAME"));
	pColData->InsertColumn(m_ctrlList, COL_ITEM_COMMENT, _("IDS_COL_COMMENTS"));
	for (std::vector<CFindItemInfo>::const_iterator iter = inItems.begin();
		iter != inItems.end();
		++iter)
	{
		CDlgListViewerDataItemPtr data(new CDlgListViewerDataItem(pColData, *iter));
		m_ctrlList->InsertItem(data);
	}
	pColData->SetColumnWidths(m_ctrlList);

	FinishCreate();
}


CDlgListViewerDataPtr CDlgListViewer::GetDataByData(long data) const
{
	return tr1::dynamic_pointer_cast<CDlgListViewerData, CListData>(m_ctrlList->GetDataByData(data));
}


bool CDlgListViewer::Create(
		wxString const& inCaption,
		wxWindow* pParent)
{
	if (!wxDialog::Create(pParent, wxID_ANY, inCaption, wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER))
		return false;

	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP);

	// Controls (these are done first to control tab order)

	m_ctrlList = new CReportListCtrl(this,
		wxDefaultPosition, wxSize(500, 180),
		false, CReportListCtrl::eSortHeader, true);
	m_ctrlList->Connect(wxEVT_COMMAND_LIST_COL_CLICK, wxListEventHandler(CDlgListViewer::OnColumnClick), NULL, this);
	m_ctrlList->Connect(wxEVT_COMMAND_LIST_ITEM_SELECTED, wxListEventHandler(CDlgListViewer::OnItemSelected), NULL, this);
	m_ctrlList->SetHelpText(_("HIDC_LIST_VIEWER"));
	m_ctrlList->SetToolTip(_("HIDC_LIST_VIEWER"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_LIST_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgListViewer::OnCopy), NULL, this);
	m_ctrlCopy->SetHelpText(_("HIDC_LIST_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_LIST_COPY"));

	wxButton* btnClose = new wxButton(this, wxID_OK,
		_("IDC_LIST_CLOSE"),
		wxDefaultPosition, wxDefaultSize, 0);
	btnClose->SetDefault();

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(m_ctrlList, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxHORIZONTAL);
	sizerBtns->Add(0, 0, 1, wxEXPAND, 5);
	sizerBtns->Add(m_ctrlCopy, 0, wxALL, 5);
	sizerBtns->Add(btnClose, 0, wxALL, 5);

	bSizer->Add(sizerBtns, 0, wxALIGN_BOTTOM|wxALIGN_RIGHT|wxEXPAND, 5);

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
	s_SortInfo.pThis = this;
	s_SortInfo.nCol = m_SortColumn;
	m_ctrlList->SortItems(CompareRows, 0);
	m_ctrlList->SetColumnSort(abs(m_SortColumn)-1, m_SortColumn);
	m_ctrlCopy->Enable(0 < m_ctrlList->GetItemCount());
}


void CDlgListViewer::OnColumnClick(wxListEvent& evt)
{
	m_ctrlList->SetColumnSort(abs(m_SortColumn)-1, 0);
	int nBackwards = 1;
	if (m_SortColumn == evt.GetColumn() + 1)
		nBackwards = -1;
	m_SortColumn = (evt.GetColumn() + 1) * nBackwards;
	s_SortInfo.pThis = this;
	s_SortInfo.nCol = m_SortColumn;
	m_ctrlList->SortItems(CompareRows, 0);
	m_ctrlList->SetColumnSort(abs(m_SortColumn)-1, m_SortColumn);
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

		if (1 < indices.size()
		|| indices.size() == static_cast<size_t>(m_ctrlList->GetItemCount()))
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


void CDlgListViewer::OnItemSelected(wxListEvent& evt)
{
	int items = std::max(m_ctrlList->GetSelectedItemCount(), m_ctrlList->GetItemCount());
	m_ctrlCopy->Enable(0 < items);
}
