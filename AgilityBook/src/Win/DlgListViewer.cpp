/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
 * In all the derived classes and the main dialog, we do not add references
 * onto the used objects. If we did, we'd have const issues! There is no need
 * to do so since this is a modal dialog. Since these things exist when the
 * dialog is launched, by definition they will exist during the lifetime of
 * this dialog.
 *
 * Revision History
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
#include "DlgConfigure.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerData
{
public:
	CDlgListViewerData() : m_RefCount(1) {}
	void AddRef() {++m_RefCount;}
	void Release()
	{
		--m_RefCount;
		if (0 == m_RefCount)
			delete this;
	}
	virtual CString OnNeedText(int iCol) const = 0;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const = 0;
protected:
	virtual ~CDlgListViewerData() {}
	UINT m_RefCount;
};

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
class CDlgListViewerDataColumns : public CDlgListViewerData
{
public:
	CDlgListViewerDataColumns(size_t inInitSize = 0)
	{
		if (0 < inInitSize)
			m_Columns.reserve(inInitSize);
	}
	virtual CString OnNeedText(int iCol) const {return _T("");}
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const
	{
		return 0;
	}
	bool InsertColumn(
			CListCtrl2& inList,
			int inIndex,
			UINT inName)
	{
		CString name;
		name.LoadString(inName);
		return InsertColumn(inList, inIndex, (LPCTSTR)name);
	}
	bool InsertColumn(
			CListCtrl2& inList,
			int inIndex,
			LPCTSTR inName)
	{
		int col = static_cast<int>(m_Columns.size());
		for (int i = 0; i < col; ++i)
			if (m_Columns[i] == inIndex)
				return false;
		inList.InsertColumn(col, inName);
		m_Columns.push_back(inIndex);
		return true;
	}
	void SetColumnWidths(CListCtrl2& inList)
	{
		for (int i = 0; i < static_cast<int>(m_Columns.size()); ++i)
			inList.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
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

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataExisting : public CDlgListViewerData
{
	friend class CDlgListViewerDataRun;
	friend class CDlgListViewerDataMultiQ;
public:
	CDlgListViewerDataExisting(
			CDlgListViewerDataColumns* inColData,
			ARBDogExistingPoints const* inExisting)
		: m_ColData(inColData)
		, m_Existing(inExisting)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
	}
	virtual ~CDlgListViewerDataExisting()
	{
		if (m_ColData)
			m_ColData->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	ARBDogExistingPoints const* m_Existing;
};

CString CDlgListViewerDataExisting::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Existing->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		{
			short pts = m_Existing->GetPoints();
			str.Format(_T("%hd"), pts);
		}
		break;
	case COL_RUN_MQ_VENUE:
		str = m_Existing->GetVenue().c_str();
		break;
	case COL_RUN_MQ_DIV:
		str = m_Existing->GetDivision().c_str();
		break;
	case COL_RUN_MQ_LEVEL:
		str = m_Existing->GetLevel().c_str();
		break;
	case COL_RUN_MQ_EVENT:
		str = m_Existing->GetEvent().c_str();
		break;
	case COL_RUN_MQ_SUBNAME:
		str = m_Existing->GetSubName().c_str();
		break;
	case COL_RUN_MQ_LOCATION:
		str.LoadString(IDS_EXISTING_POINTS);
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataRun : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
public:
	CDlgListViewerDataRun(
			CDlgListViewerDataColumns* inColData,
			ARBDog const* inDog,
			ARBDogTrial const* inTrial,
			ARBDogRun const* inRun,
			ARBConfigScoring* inScoring,
			ScoringRunInfo::eScoringDetail inScoringDetail)
		: m_ColData(inColData)
		, m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_Scoring(inScoring)
		, m_ScoringDetail(inScoringDetail)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
		// Note, some users have changed NADAC to remove Novice A/B and only
		// have Novice (no sublevels). This means during a config update,
		// all hell will break loose. Don't bother asserting here...
		//ASSERT(inScoring);
		if (m_Scoring)
			m_Scoring->AddRef();
	}
	virtual ~CDlgListViewerDataRun()
	{
		if (m_ColData)
			m_ColData->Release();
		if (m_Scoring)
			m_Scoring->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	ARBDog const* m_Dog;
	ARBDogTrial const* m_Trial;
	ARBDogRun const* m_Run;
	ARBConfigScoring* m_Scoring;
	ScoringRunInfo::eScoringDetail m_ScoringDetail;
};

CString CDlgListViewerDataRun::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Run->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case COL_RUN_MQ_STATUS: // Only happens for scoring detail items.
		switch (m_ScoringDetail)
		{
		default:
			break;
		case ScoringRunInfo::eScoringDeleted:
			str = _T("Deleted");
			break;
		case ScoringRunInfo::eScoringChanged:
			str = _T("Changed");
			break;
		}
		break;
	case COL_RUN_MQ_DOG: // Only happens for scoring detail items.
		if (m_Dog)
			str = m_Dog->GetCallName().c_str();
		break;
	case COL_RUN_MQ_Q:
		str = m_Run->GetQ().str().c_str();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		{
			short pts = 0;
			if (m_Run->GetQ().Qualified() && m_Scoring)
				pts = m_Run->GetTitlePoints(m_Scoring);
			str.Format(_T("%hd"), pts);
		}
		break;
	case COL_RUN_MQ_VENUE:
		str = m_Trial->GetClubs().GetPrimaryClubVenue().c_str();
		break;
	case COL_RUN_MQ_DIV:
		str = m_Run->GetDivision().c_str();
		break;
	case COL_RUN_MQ_LEVEL:
		str = m_Run->GetLevel().c_str();
		break;
	case COL_RUN_MQ_EVENT:
		str = m_Run->GetEvent().c_str();
		break;
	case COL_RUN_MQ_SUBNAME:
		str = m_Run->GetSubName().c_str();
		break;
	case COL_RUN_MQ_LOCATION:
		str = m_Trial->GetLocation().c_str();
		break;
	case COL_RUN_MQ_CLUB:
		if (m_Trial->GetClubs().GetPrimaryClub())
			str = m_Trial->GetClubs().GetPrimaryClubName().c_str();
		break;
	case COL_RUN_MQ_JUDGE:
		str = m_Run->GetJudge().c_str();
		break;
	case COL_RUN_MQ_PLACE:
		str.Format(_T("%hd"), m_Run->GetPlace());
		break;
	case COL_RUN_MQ_INCLASS:
		str.Format(_T("%hd"), m_Run->GetInClass());
		break;
	case COL_RUN_MQ_QD:
		str.Format(_T("%hd"), m_Run->GetDogsQd());
		break;
	case COL_RUN_MQ_SPEED:
		if (m_Scoring)
			str.Format(_T("%hd"), m_Run->GetSpeedPoints(m_Scoring));
		break;
	case COL_RUN_MQ_PARTNERS:
		for (ARBDogRunPartnerList::const_iterator iter2 = m_Run->GetPartners().begin();
			iter2 != m_Run->GetPartners().end();
			++iter2)
		{
			if (!str.IsEmpty())
				str += _T(", ");
			str += (*iter2)->GetHandler().c_str();
			str += _T("/");
			str += (*iter2)->GetDog().c_str();
		}
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

class CDlgListViewerDataMultiQ : public CDlgListViewerData
{
	friend class CDlgListViewerDataExisting;
public:
	CDlgListViewerDataMultiQ(
			CDlgListViewerDataColumns* inColData,
			ARBDate const& inDate,
			ARBDogTrial const* inTrial)
		: m_ColData(inColData)
		, m_Date(inDate)
		, m_Trial(inTrial)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
	}
	virtual ~CDlgListViewerDataMultiQ()
	{
		if (m_ColData)
			m_ColData->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	ARBDate m_Date;
	ARBDogTrial const* m_Trial;
};

CString CDlgListViewerDataMultiQ::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_RUN_MQ_DATE:
		str = m_Date.GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case COL_RUN_MQ_TITLE_PTS:
		str = _T("1");
		break;
	case COL_RUN_MQ_LOCATION:
		str = m_Trial->GetLocation().c_str();
		break;
	case COL_RUN_MQ_CLUB:
		if (m_Trial->GetClubs().GetPrimaryClub())
			str = m_Trial->GetClubs().GetPrimaryClubName().c_str();
		break;
	}
	return str;
}

/////////////////////////////////////////////////////////////////////////////

int CDlgListViewerDataExisting::Compare(
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataExisting const* pDataExisting = dynamic_cast<CDlgListViewerDataExisting const*>(pRow2);
	CDlgListViewerDataRun const* pDataRun = dynamic_cast<CDlgListViewerDataRun const*>(pRow2);
	CDlgListViewerDataMultiQ const* pDataMultiQ = dynamic_cast<CDlgListViewerDataMultiQ const*>(pRow2);
	if (!pDataExisting && !pDataRun && !pDataMultiQ)
		return 0;
	ARBString str1, str2;
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
			short pts1 = m_Existing->GetPoints();
			short pts2 = 0;
			if (pDataExisting)
				pts2 = pDataExisting->m_Existing->GetPoints();
			else if (pDataRun)
			{
				if (pDataRun->m_Run->GetQ().Qualified() && pDataRun->m_Scoring)
					pts2 = pDataRun->m_Run->GetTitlePoints(pDataRun->m_Scoring);
			}
			else if (pDataMultiQ)
			{
				pts2 = 1;
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
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataExisting const* pDataExisting = dynamic_cast<CDlgListViewerDataExisting const*>(pRow2);
	CDlgListViewerDataRun const* pDataRun = dynamic_cast<CDlgListViewerDataRun const*>(pRow2);
	if (!pDataExisting && !pDataRun)
		return 0;
	ARBString str1, str2;
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
			short pts1 = 0;
			if (m_Run->GetQ().Qualified() && m_Scoring)
				pts1 = m_Run->GetTitlePoints(m_Scoring);
			short pts2 = 0;
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
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataExisting const* pDataExisting = dynamic_cast<CDlgListViewerDataExisting const*>(pRow2);
	CDlgListViewerDataMultiQ const* pData = dynamic_cast<CDlgListViewerDataMultiQ const*>(pRow2);
	if (!pDataExisting && !pData)
		return 0;
	ARBString str1, str2;
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
			CDlgListViewerDataColumns* inColData,
			LifeTimePointInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
	}
	virtual ~CDlgListViewerDataLifetime()
	{
		if (m_ColData)
			m_ColData->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	LifeTimePointInfo m_info;
};

CString CDlgListViewerDataLifetime::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_OTHER_DIV:
		str = m_info.div.c_str();
		break;
	case COL_OTHER_LEVEL:
		str = m_info.level.c_str();
		break;
	case COL_OTHER_PTS:
		{
			CString str2;
			if (0 < m_info.filtered)
				str2.Format(_T("%d (%d)"), m_info.points - m_info.filtered, m_info.points);
			else
				str2.Format(_T("%d"), m_info.points);
			str = (LPCTSTR)str2;
		}
		break;
	}
	return str;
}

int CDlgListViewerDataLifetime::Compare(
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataLifetime const* pData = dynamic_cast<CDlgListViewerDataLifetime const*>(pRow2);
	if (!pData)
		return 0;
	ARBString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_OTHER_DIV:
		str1 = m_info.div;
		str2 = pData->m_info.div;
		break;
	case COL_OTHER_LEVEL:
		str1 = m_info.level;
		str2 = pData->m_info.level;
		break;
	case COL_OTHER_PTS:
		if (m_info.points < pData->m_info.points)
			return -1;
		else if (m_info.points > pData->m_info.points)
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
			CDlgListViewerDataColumns* inColData,
			OtherPtInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
	}
	virtual ~CDlgListViewerDataOther()
	{
		if (m_ColData)
			m_ColData->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	OtherPtInfo m_info;
};

CString CDlgListViewerDataOther::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_OTHER_DATE:
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		else
			str = m_info.m_pRun->GetDate().GetString(CAgilityBookOptions::GetDateFormat(CAgilityBookOptions::ePoints)).c_str();
		break;
	case COL_OTHER_NAME: // Trial/Existing Points
		if (m_info.m_pExisting)
			str = m_info.m_pExisting->GetOtherPoints().c_str();
		else
			str = m_info.m_pTrial->GetLocation().c_str();
		break;
	case COL_OTHER_CLUB:
		if (!m_info.m_pExisting && m_info.m_pTrial->GetClubs().GetPrimaryClub())
			str = m_info.m_pTrial->GetClubs().GetPrimaryClubName().c_str();
		break;
	case COL_OTHER_VENUE:
		str = m_info.m_Venue.c_str();
		break;
	case COL_OTHER_DIV:
		str = m_info.m_Div.c_str();
		break;
	case COL_OTHER_LEVEL:
		str = m_info.m_Level.c_str();
		break;
	case COL_OTHER_EVENT:
		str = m_info.m_Event.c_str();
		break;
	case COL_OTHER_PTS:
		str.Format(_T("%d"), m_info.m_Score);
		break;
	}
	return str;
}

int CDlgListViewerDataOther::Compare(
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataOther const* pData = dynamic_cast<CDlgListViewerDataOther const*>(pRow2);
	if (!pData)
		return 0;
	ARBString str1, str2;
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
			if (m_info.m_pExisting)
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
			CDlgListViewerDataColumns* inColData,
			CFindItemInfo const& info)
		: m_ColData(inColData)
		, m_info(info)
	{
		ASSERT(m_ColData);
		if (m_ColData)
			m_ColData->AddRef();
	}
	virtual ~CDlgListViewerDataItem()
	{
		if (m_ColData)
			m_ColData->Release();
	}
	virtual CString OnNeedText(int iCol) const;
	virtual int Compare(
			CDlgListViewerData const* pRow2,
			int inCol) const;
private:
	CDlgListViewerDataColumns* m_ColData;
	CFindItemInfo m_info;
};

CString CDlgListViewerDataItem::OnNeedText(int iCol) const
{
	CString str;
	switch (m_ColData->GetIndex(iCol))
	{
	case COL_ITEM_TYPE:
		switch (m_info.type)
		{
		case ARBInfo::eClubInfo:
			str.LoadString(IDS_COL_CLUB);
			break;
		case ARBInfo::eJudgeInfo:
			str.LoadString(IDS_COL_JUDGE);
			break;
		case ARBInfo::eLocationInfo:
			str.LoadString(IDS_COL_LOCATION);
			break;
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
		CDlgListViewerData const* pRow2,
		int inCol) const
{
	CDlgListViewerDataItem const* pData = dynamic_cast<CDlgListViewerDataItem const*>(pRow2);
	if (!pData)
		return 0;
	CString str;
	ARBString str1, str2;
	switch (m_ColData->GetIndex(inCol))
	{
	default:
	case COL_ITEM_TYPE:
		switch (m_info.type)
		{
		case ARBInfo::eClubInfo:
			str.LoadString(IDS_COL_CLUB);
			break;
		case ARBInfo::eJudgeInfo:
			str.LoadString(IDS_COL_JUDGE);
			break;
		case ARBInfo::eLocationInfo:
			str.LoadString(IDS_COL_LOCATION);
			break;
		}
		str1 = (LPCTSTR)str;
		switch (pData->m_info.type)
		{
		case ARBInfo::eClubInfo:
			str.LoadString(IDS_COL_CLUB);
			break;
		case ARBInfo::eJudgeInfo:
			str.LoadString(IDS_COL_JUDGE);
			break;
		case ARBInfo::eLocationInfo:
			str.LoadString(IDS_COL_LOCATION);
			break;
		}
		str2 = (LPCTSTR)str;
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

struct SORT_COL_INFO
{
	CDlgListViewer *pThis;
	int nCol;
};

int CALLBACK CompareRows(
		LPARAM lParam1,
		LPARAM lParam2,
		LPARAM lParam3)
{
	SORT_COL_INFO* sortInfo = reinterpret_cast<SORT_COL_INFO*>(lParam3);
	if (!sortInfo)
		return 0;
	CDlgListViewerData* pRow1 = reinterpret_cast<CDlgListViewerData*>(lParam1);
	CDlgListViewerData* pRow2 = reinterpret_cast<CDlgListViewerData*>(lParam2);
	if (!pRow1 || !pRow2)
		return 0;
	int iCol = abs(sortInfo->nCol);
	int nRet = pRow1->Compare(pRow2, iCol - 1);
	if (0 > sortInfo->nCol)
		nRet *= -1;
	return nRet;
}

/////////////////////////////////////////////////////////////////////////////
// Viewing runs
CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		CString const& inCaption,
		RunInfoData const* inData,
		std::list<RunInfo> const& inRuns,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(inData)
	, m_DataMultiQ(NULL)
	, m_Runs(&inRuns)
	, m_ScoringRuns(NULL)
	, m_MultiQdata(NULL)
	, m_Lifetime(NULL)
	, m_OtherData(NULL)
	, m_Items(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
	//{{AFX_DATA_INIT(CDlgListViewer)
	//}}AFX_DATA_INIT
}

// Viewing runs affected by configuration changes
CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::list<ScoringRunInfo> const& inScoringRuns,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(NULL)
	, m_DataMultiQ(NULL)
	, m_Runs(NULL)
	, m_ScoringRuns(&inScoringRuns)
	, m_MultiQdata(NULL)
	, m_Lifetime(NULL)
	, m_OtherData(NULL)
	, m_Items(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

// Viewing multi-Qs
CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		CString const& inCaption,
		MultiQInfoData const* inData,
		std::set<MultiQdata> const& inMQs,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(NULL)
	, m_DataMultiQ(inData)
	, m_Runs(NULL)
	, m_ScoringRuns(NULL)
	, m_MultiQdata(&inMQs)
	, m_Lifetime(NULL)
	, m_OtherData(NULL)
	, m_Items(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

// Viewing lifetime data
CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
	CString const& inCaption,
	std::list<LifeTimePointInfo> const& inLifetime,
	CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(NULL)
	, m_DataMultiQ(NULL)
	, m_Runs(NULL)
	, m_ScoringRuns(NULL)
	, m_MultiQdata(NULL)
	, m_Lifetime(&inLifetime)
	, m_OtherData(NULL)
	, m_Items(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

// Viewing other points
CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::list<OtherPtInfo> const& inRunList,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(NULL)
	, m_DataMultiQ(NULL)
	, m_Runs(NULL)
	, m_ScoringRuns(NULL)
	, m_MultiQdata(NULL)
	, m_Lifetime(NULL)
	, m_OtherData(&inRunList)
	, m_Items(NULL)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

CDlgListViewer::CDlgListViewer(
		CAgilityBookDoc* inDoc,
		CString const& inCaption,
		std::vector<CFindItemInfo> const& inItems,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgListViewer::IDD, pParent)
	, m_pDoc(inDoc)
	, m_Caption(inCaption)
	, m_DataRun(NULL)
	, m_DataMultiQ(NULL)
	, m_Runs(NULL)
	, m_ScoringRuns(NULL)
	, m_MultiQdata(NULL)
	, m_Lifetime(NULL)
	, m_OtherData(NULL)
	, m_Items(&inItems)
	, m_rWin(0,0,0,0)
	, m_rDlg(0,0,0,0)
	, m_rList(0,0,0,0)
	, m_rOK(0,0,0,0)
	, m_SortColumn(1)
{
}

void CDlgListViewer::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgListViewer)
	DDX_Control(pDX, IDC_LIST_VIEWER, m_ctrlList);
	DDX_Control(pDX, IDC_LIST_COPY, m_ctrlCopy);
	DDX_Control(pDX, IDOK, m_ctrlClose);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgListViewer, CDlgBaseDialog)
	//{{AFX_MSG_MAP(CDlgListViewer)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_VIEWER, OnGetdispinfoList)
	ON_NOTIFY(LVN_DELETEITEM, IDC_LIST_VIEWER, OnDeleteitemList)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LIST_VIEWER, OnColumnclickList)
	ON_WM_GETMINMAXINFO()
	ON_WM_SIZE()
	ON_BN_CLICKED(IDC_LIST_COPY, OnBnClickedListCopy)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgListViewer message handlers

static void InsertRun(
		CAgilityBookDoc* pDoc,
		CListCtrl2& ctrlList,
		CDlgListViewerDataColumns* pColData,
		int& iItem,
		ARBDog const* pDog,
		ARBDogTrial const* pTrial,
		ARBDogRun const* pRun,
		ScoringRunInfo::eScoringDetail scoringDetail)
{
	ARBConfigScoring* pScoring = NULL;
	if (pTrial->GetClubs().GetPrimaryClub())
		 pDoc->GetConfig().GetVenues().FindEvent(
			pTrial->GetClubs().GetPrimaryClubVenue(),
			pRun->GetEvent(),
			pRun->GetDivision(),
			pRun->GetLevel(),
			pRun->GetDate(),
			&pScoring);
	if (pScoring)
	{
		if (pScoring->HasSpeedPts())
			pColData->InsertColumn(ctrlList, COL_RUN_MQ_SPEED, IDS_SPEEDPTS);
	}
	if (0 < pRun->GetPartners().size())
		pColData->InsertColumn(ctrlList, COL_RUN_MQ_PARTNERS, IDS_PARTNERS);

	CDlgListViewerDataRun* pData = new CDlgListViewerDataRun(pColData, pDog, pTrial, pRun, pScoring, scoringDetail);
	LVITEM item;
	item.mask = LVIF_TEXT | LVIF_PARAM;
	item.iItem = iItem++;
	item.iSubItem = 0;
	item.pszText = LPSTR_TEXTCALLBACK;
	item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
	ctrlList.InsertItem(&item);

	if (pScoring)
		pScoring->Release();
}

BOOL CDlgListViewer::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlList.SetExtendedStyle(m_ctrlList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), TRUE);	// Set big icon
	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);	// Set small icon

	GetWindowRect(m_rWin);
	GetClientRect(m_rDlg);
	m_ctrlList.GetWindowRect(m_rList);
	ScreenToClient(m_rList);
	m_ctrlCopy.GetWindowRect(m_rCopy);
	ScreenToClient(m_rCopy);
	m_ctrlClose.GetWindowRect(m_rOK);
	ScreenToClient(m_rOK);

	SetWindowText(m_Caption);

	std::set<ARBString> subNames;
	std::vector<ARBDogExistingPoints const*> existingRuns;
	if (m_Runs || m_ScoringRuns)
	{
		if (m_Runs)
		{
			std::list<RunInfo>::const_iterator iter;
			for (iter = m_Runs->begin(); iter != m_Runs->end(); ++iter)
			{
				if ((iter->second)->GetSubName().length())
					subNames.insert((iter->second)->GetSubName());
			}
		}
		else if (m_ScoringRuns)
		{
			std::list<ScoringRunInfo>::const_iterator iter;
			for (iter = m_ScoringRuns->begin(); iter != m_ScoringRuns->end(); ++iter)
			{
				if ((iter->m_Run)->GetSubName().length())
					subNames.insert((iter->m_Run)->GetSubName());
			}
		}
		if (m_DataRun)
		{
			for (ARBDogExistingPointsList::const_iterator iter = m_DataRun->m_Dog->GetExistingPoints().begin();
				iter != m_DataRun->m_Dog->GetExistingPoints().end();
				++iter)
			{
				ARBDogExistingPoints const* pExisting = *iter;
				if (ARBDogExistingPoints::eRuns == pExisting->GetType()
				&& pExisting->GetVenue() == m_DataRun->m_Venue->GetName()
				&& pExisting->GetDivision() == m_DataRun->m_Div->GetName()
				&& (pExisting->GetLevel() == m_DataRun->m_Level->GetName()
				|| m_DataRun->m_Level->GetSubLevels().FindSubLevel(pExisting->GetLevel()))
				&& pExisting->GetEvent() == m_DataRun->m_Event->GetName())
				{
					existingRuns.push_back(pExisting);
					if (0 < pExisting->GetSubName().length())
						subNames.insert(pExisting->GetSubName());
				}
			}
		}
	}

	if (m_Runs)
	{
		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(10);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, IDS_COL_DATE);
		m_SortColumn = pColData->NumColumns();
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_Q, IDS_COL_Q);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, IDS_COL_TITLE_PTS);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_VENUE, IDS_COL_VENUE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DIV, IDS_COL_DIVISION);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LEVEL, IDS_COL_LEVEL);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_EVENT, IDS_COL_EVENT);
		if (0 < subNames.size())
			pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_SUBNAME, IDS_COL_SUBNAME);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, IDS_COL_LOCATION);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, IDS_COL_CLUB);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_JUDGE, IDS_COL_JUDGE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_PLACE, IDS_COL_PLACE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_INCLASS, IDS_COL_IN_CLASS);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_QD, IDS_COL_Q_D);
		int iItem = 0;
		if (m_DataRun)
		{
			for (std::vector<ARBDogExistingPoints const*>::iterator iter = existingRuns.begin();
				iter != existingRuns.end();
				++iter)
			{
				CDlgListViewerDataExisting* pData = new CDlgListViewerDataExisting(pColData, *iter);
				LVITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				item.iItem = iItem++;
				item.iSubItem = 0;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
				m_ctrlList.InsertItem(&item);
			}
		}
		std::list<RunInfo>::const_iterator iter;
		for (iter = m_Runs->begin(); iter != m_Runs->end(); ++iter)
		{
			ARBDogTrial const* pTrial = iter->first;
			ARBDogRun const* pRun = iter->second;
			if (CAgilityBookOptions::IsFilterEnabled())
			{
				if (pRun->IsFiltered())
					continue;
			}
			InsertRun(m_pDoc, m_ctrlList, pColData, iItem,
				NULL, pTrial, pRun, ScoringRunInfo::eNotScoringDetail);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	else if (m_ScoringRuns)
	{
		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(12);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_STATUS, _T("Status"));
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DOG, IDS_COL_DOG);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, IDS_COL_DATE);
		m_SortColumn = pColData->NumColumns();
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_Q, IDS_COL_Q);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, IDS_COL_TITLE_PTS);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_VENUE, IDS_COL_VENUE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DIV, IDS_COL_DIVISION);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LEVEL, IDS_COL_LEVEL);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_EVENT, IDS_COL_EVENT);
		if (0 < subNames.size())
			pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_SUBNAME, IDS_COL_SUBNAME);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, IDS_COL_LOCATION);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, IDS_COL_CLUB);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_JUDGE, IDS_COL_JUDGE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_PLACE, IDS_COL_PLACE);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_INCLASS, IDS_COL_IN_CLASS);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_QD, IDS_COL_Q_D);
		int iItem = 0;
		for (std::list<ScoringRunInfo>::const_iterator iter = m_ScoringRuns->begin();
			iter != m_ScoringRuns->end();
			++iter)
		{
			ScoringRunInfo const& info = *iter;
			InsertRun(m_pDoc, m_ctrlList, pColData, iItem,
				info.m_Dog, info.m_Trial, info.m_Run, info.m_ScoringDetail);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	else if (m_MultiQdata)
	{
		std::vector<ARBDogExistingPoints const*> existingRuns;
		if (m_DataMultiQ)
		{
			for (ARBDogExistingPointsList::const_iterator iter = m_DataMultiQ->m_Dog->GetExistingPoints().begin();
				iter != m_DataMultiQ->m_Dog->GetExistingPoints().end();
				++iter)
			{
				ARBDogExistingPoints const* pExisting = *iter;
				if (ARBDogExistingPoints::eMQ == pExisting->GetType()
				&& pExisting->GetVenue() == m_DataMultiQ->m_Venue->GetName()
				&& pExisting->GetMultiQ() == m_DataMultiQ->m_MultiQ->GetName())
				{
					existingRuns.push_back(pExisting);
				}
			}
		}

		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(3);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_DATE, IDS_COL_DATE);
		m_SortColumn = pColData->NumColumns();
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_TITLE_PTS, IDS_COL_TITLE_PTS);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_LOCATION, IDS_COL_LOCATION);
		pColData->InsertColumn(m_ctrlList, COL_RUN_MQ_CLUB, IDS_COL_CLUB);
		int iItem = 0;
		if (m_DataMultiQ)
		{
			for (std::vector<ARBDogExistingPoints const*>::iterator iter = existingRuns.begin();
				iter != existingRuns.end();
				++iter)
			{
				CDlgListViewerDataExisting* pData = new CDlgListViewerDataExisting(pColData, *iter);
				LVITEM item;
				item.mask = LVIF_TEXT | LVIF_PARAM;
				item.iItem = iItem++;
				item.iSubItem = 0;
				item.pszText = LPSTR_TEXTCALLBACK;
				item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
				m_ctrlList.InsertItem(&item);
			}
		}
		for (std::set<MultiQdata>::const_iterator iter = m_MultiQdata->begin();
			iter != m_MultiQdata->end();
			++iter)
		{
			CDlgListViewerDataMultiQ* pData = new CDlgListViewerDataMultiQ(pColData, iter->first, iter->second);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
			m_ctrlList.InsertItem(&item);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	else if (m_Lifetime)
	{
		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(3);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, IDS_COL_DIVISION);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_LEVEL, IDS_COL_LEVEL);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_PTS, IDS_COL_POINTS);
		int iItem = 0;
		for (std::list<LifeTimePointInfo>::const_iterator iter = m_Lifetime->begin();
			iter != m_Lifetime->end();
			++iter)
		{
			LifeTimePointInfo const& info = *iter;
			CDlgListViewerDataLifetime* pData = new CDlgListViewerDataLifetime(pColData, info);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
			m_ctrlList.InsertItem(&item);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	else if (m_OtherData)
	{
		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(8);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_DATE, IDS_COL_DATE);
		m_SortColumn = pColData->NumColumns();
		pColData->InsertColumn(m_ctrlList, COL_OTHER_NAME, _T("Trial / Existing Pts"));
		pColData->InsertColumn(m_ctrlList, COL_OTHER_CLUB, IDS_COL_CLUB);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_VENUE, IDS_COL_VENUE);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_DIV, IDS_COL_DIVISION);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_LEVEL, IDS_COL_LEVEL);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_EVENT, IDS_COL_EVENT);
		pColData->InsertColumn(m_ctrlList, COL_OTHER_PTS, IDS_COL_POINTS);
		int iItem = 0;
		for (std::list<OtherPtInfo>::const_iterator iter = m_OtherData->begin();
			iter != m_OtherData->end();
			++iter)
		{
			OtherPtInfo const& info = *iter;
			CDlgListViewerDataOther* pData = new CDlgListViewerDataOther(pColData, info);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
			m_ctrlList.InsertItem(&item);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	else if (m_Items)
	{
		CDlgListViewerDataColumns* pColData = new CDlgListViewerDataColumns(3);
		pColData->InsertColumn(m_ctrlList, COL_ITEM_TYPE, IDS_COL_TYPE);
		m_SortColumn = pColData->NumColumns();
		pColData->InsertColumn(m_ctrlList, COL_ITEM_NAME, IDS_COL_NAME);
		pColData->InsertColumn(m_ctrlList, COL_ITEM_COMMENT, IDS_COL_COMMENTS);
		int iItem = 0;
		for (std::vector<CFindItemInfo>::const_iterator iter = m_Items->begin();
			iter != m_Items->end();
			++iter)
		{
			CFindItemInfo const& info = *iter;
			CDlgListViewerDataItem* pData = new CDlgListViewerDataItem(pColData, info);
			LVITEM item;
			item.mask = LVIF_TEXT | LVIF_PARAM;
			item.iItem = iItem++;
			item.iSubItem = 0;
			item.pszText = LPSTR_TEXTCALLBACK;
			item.lParam = reinterpret_cast<LPARAM>(static_cast<CDlgListViewerData*>(pData));
			m_ctrlList.InsertItem(&item);
		}
		pColData->SetColumnWidths(m_ctrlList);
		pColData->Release();
	}

	SORT_COL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	m_ctrlList.SortItems(CompareRows, reinterpret_cast<LPARAM>(&info));
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1, CHeaderCtrl2::eAscending);

	GetDlgItem(IDOK)->SetFocus();
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgListViewer::OnGetdispinfoList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = reinterpret_cast<LV_DISPINFO*>(pNMHDR);
	if (pDispInfo->item.mask & LVIF_TEXT)
	{
		CDlgListViewerData *pData = reinterpret_cast<CDlgListViewerData*>(pDispInfo->item.lParam);
		if (pData)
		{
			CString str = pData->OnNeedText(pDispInfo->item.iSubItem);
			::lstrcpyn(pDispInfo->item.pszText, str, pDispInfo->item.cchTextMax);
			pDispInfo->item.pszText[pDispInfo->item.cchTextMax-1] = '\0';
		}
		else
			pDispInfo->item.pszText[0] = '\0';
	}
	*pResult = 0;
}

void CDlgListViewer::OnDeleteitemList(
		NMHDR* pNMHDR,
		LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	CDlgListViewerData *pData = reinterpret_cast<CDlgListViewerData*>(pNMListView->lParam);
	if (pData)
		pData->Release();
	pNMListView->lParam = 0;
	*pResult = 0;
}

void CDlgListViewer::OnColumnclickList(
		NMHDR* pNMHDR,
		LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1, CHeaderCtrl2::eNoSort);
	int nBackwards = 1;
	if (m_SortColumn == pNMListView->iSubItem + 1)
		nBackwards = -1;
	m_SortColumn = (pNMListView->iSubItem + 1) * nBackwards;
	SORT_COL_INFO info;
	info.pThis = this;
	info.nCol = m_SortColumn;
	m_ctrlList.SortItems(CompareRows, reinterpret_cast<LPARAM>(&info));
	m_ctrlList.HeaderSort(abs(m_SortColumn) - 1,
		nBackwards > 0 ? CHeaderCtrl2::eAscending : CHeaderCtrl2::eDescending);
	*pResult = 0;
}

void CDlgListViewer::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_rWin.Width();
	lpMMI->ptMinTrackSize.y = m_rWin.Height();
	CDlgBaseDialog::OnGetMinMaxInfo(lpMMI);
}

void CDlgListViewer::OnSize(
		UINT nType,
		int cx,
		int cy)
{
	CDlgBaseDialog::OnSize(nType, cx, cy);
	if (::IsWindow(m_ctrlList.GetSafeHwnd()))
	{
		m_ctrlCopy.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rCopy.left), cy - (m_rDlg.Height() - m_rCopy.bottom) - m_rCopy.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlClose.SetWindowPos(NULL,
			cx - (m_rDlg.Width() - m_rOK.left), cy - (m_rDlg.Height() - m_rOK.bottom) - m_rOK.Height(),
			0, 0, SWP_NOZORDER | SWP_NOSIZE);
		m_ctrlList.SetWindowPos(NULL,
			0, 0,
			cx - (m_rDlg.Width() - m_rList.Width()), cy - (m_rDlg.Height() - m_rList.Height()),
			SWP_NOZORDER | SWP_NOMOVE);
	}
}

void CDlgListViewer::OnBnClickedListCopy()
{
	if (!AfxGetMainWnd()->OpenClipboard())
		return;
	if (!EmptyClipboard())
	{
		CloseClipboard();
		return;
	}
	if (0 < m_ctrlList.GetItemCount())
	{
		CString data;
		CStringArray line;

		m_ctrlList.GetPrintLine(-1, line);
		for (int i = 0; i < line.GetSize(); ++i)
		{
			if (0 < i)
				data += '\t';
			data += line[i];
		}
		data += _T("\r\n");

		// Now all the data.
		for (int idx = 0; idx < m_ctrlList.GetItemCount(); ++idx)
		{
			m_ctrlList.GetPrintLine(idx, line);
			for (int i = 0; i < line.GetSize(); ++i)
			{
				if (0 < i)
					data += '\t';
				data += line[i];
			}
			data += _T("\r\n");
		}

		// alloc mem block & copy text in
		HGLOBAL temp = GlobalAlloc(GHND, data.GetLength()+1);
		if (NULL != temp)
		{
			LPTSTR str = reinterpret_cast<LPTSTR>(GlobalLock(temp));
			lstrcpy(str, (LPCTSTR)data);
			GlobalUnlock(reinterpret_cast<void*>(temp));
			// send data to clipbard
			SetClipboardData(CF_TEXT, temp);
		}
	}
	CloseClipboard();
}
