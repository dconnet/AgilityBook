/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTreeData classes
 * @author David Connet
 *
 * Revision History
 * @li 2012-09-29 DRC Strip the Runs View.
 * @li 2012-09-09 DRC Added 'titlePts' to 'Placement'.
 * @li 2012-03-03 DRC Fixed new unearned titles from showing up in view.
 * @li 2011-10-14 DRC Modify how reorder dialog is invoked.
 * @li 2010-12-30 DRC After copying/deleting runs, update multi-Q status.
 * @li 2010-12-05 DRC DOB can be invalid (on import). Don't show in tree.
 * @li 2010-01-02 DRC Fix initialization of date for a new run.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-14 DRC Fixed crash (on Mac) when editing dog.
 * @li 2009-02-02 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2004-10-21 DRC When an item is inserted in the tree, the text callback
 *                    can actually occur BEFORE we set the HTREEITEM.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-11 DRC Added verified-trial info to listing
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-06-06 DRC Added duplicate/cut/paste support for dogs.
 * @li 2004-05-09 DRC After adding a title, display dog property dlg.
 * @li 2004-04-15 DRC Added Duplicate menu item.
 * @li 2004-01-04 DRC Changed ARBDate::GetString to take a format code.
 * @li 2003-12-30 DRC Implemented customized text in tree.
 * @li 2003-09-04 DRC When pasting, set document modified flag. Clean up some
 *                    warning messages when creating/pasting items and a filter
 *                    is set. (It was saying they weren't added - they were,
 *                    they just weren't visible.) Also, after adding/editing
 *                    trials/runs, make sure the items get sorted properly.
 * @li 2003-09-03 DRC When pasting trials/runs, select the new item.
 * @li 2003-08-30 DRC Copy a trial/run in the tree to the clipboard in dual
 *                    form. The internal form allows copying of the entry.
 *                    The text form copies what is printed.
 * @li 2003-08-24 DRC Optimized filtering by adding boolean into ARBBase to
 *                    prevent constant re-evaluation.
 * @li 2003-07-24 DRC Added reorder support on all items. Made dogs user-sorted.
 */

#include "stdafx.h"
#include "AgilityBookTreeData.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeModel.h"
#include "AgilityBookTreeView.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "Element.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "IconList.h"
#include "ImageManager.h"
#include "PointsData.h"
#include "Print.h"
#include "StringUtil.h"
#include "TabView.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeData::CAgilityBookTreeData(CAgilityBookTreeModel* pModel)
	: m_pModel(pModel)
	, m_icon()
	, m_parent(NULL)
{
}


CAgilityBookTreeData::~CAgilityBookTreeData()
{
	for (std::vector<CAgilityBookTreeData*>::iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		delete *i;
	}
	m_children.clear();
}


unsigned int CAgilityBookTreeData::GetChildren(
		wxDataViewItemArray& array) const
{
	for (std::vector<CAgilityBookTreeData*>::const_iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		array.Add(wxDataViewItem(*i));
	}
	return static_cast<unsigned int>(m_children.size());
}


void CAgilityBookTreeData::Append(CAgilityBookTreeData* child)
{
	child->m_parent = this;
	m_children.push_back(child);
}


void CAgilityBookTreeData::Remove(CAgilityBookTreeData* child)
{
	std::vector<CAgilityBookTreeData*>::iterator i = std::find(m_children.begin(), m_children.end(), child);
	if (i != m_children.end())
	{
		CAgilityBookTreeData* node = *i;
		m_children.erase(i);
		delete node;
	}
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataDog::CAgilityBookTreeDataDog(
		CAgilityBookTreeModel* pModel,
		ARBDogPtr dog)
	: CAgilityBookTreeData(pModel)
	, m_pDog(dog)
{
	m_icon = CImageManager::Get()->Dog();
}


CAgilityBookTreeDataDog::~CAgilityBookTreeDataDog()
{
}


wxVariant CAgilityBookTreeDataDog::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 == col)
	{
		std::wostringstream str;
		for (size_t idx = 0; idx < columns.size(); ++idx)
		{
			if (0 < idx)
				str << L" ";
			switch (columns[idx])
			{
			case IO_TREE_DOG_REGNAME:
				str << m_pDog->GetRegisteredName();
				break;
			case IO_TREE_DOG_CALLNAME:
				str << m_pDog->GetCallName();
				break;
			case IO_TREE_DOG_BREED:
				str << m_pDog->GetBreed();
				break;
			case IO_TREE_DOG_DOB:
				if (m_pDog->GetDOB().IsValid())
					str << m_pDog->GetDOB().GetString();
				break;
			case IO_TREE_DOG_AGE:
				if (m_pDog->GetDOB().IsValid())
				{
					wxDateTime dob;
					m_pDog->GetDOB().GetDate(dob);
					wxDateTime current = wxDateTime::Now();
					if (m_pDog->GetDeceased().IsValid())
						m_pDog->GetDeceased().GetDate(current);
					wxTimeSpan age = current - dob;
					str << StringUtil::stringW(wxString::Format(_("IDS_YEARS"), ARBDouble::ToString(age.GetDays()/365.0, 1).c_str()));
				}
				break;
			}
		}
		variant = str.str();
	}
	return variant;
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataTrial::CAgilityBookTreeDataTrial(
		CAgilityBookTreeModel* pModel,
		ARBDogTrialPtr trial)
	: CAgilityBookTreeData(pModel)
	, m_pTrial(trial)
{
	CIconList icons;
	m_icon = icons.GetIcon(icons.Trial(m_pModel->GetDocument()->Book().GetConfig(), m_pTrial));
}


CAgilityBookTreeDataTrial::~CAgilityBookTreeDataTrial()
{
}


wxVariant CAgilityBookTreeDataTrial::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 == col)
	{
		std::wostringstream str;
		bool bNeedSpace = false;
		for (size_t idx = 0; idx < columns.size(); ++idx)
		{
			switch (columns[idx])
			{
			case IO_TREE_TRIAL_START:
				if (m_pTrial->GetRuns().GetStartDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_END == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetRuns().GetStartDate().GetString();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_END:
				if (m_pTrial->GetRuns().GetEndDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_START == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetRuns().GetEndDate().GetString();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_VERIFIED:
				if (bNeedSpace)
					str << L" ";
				if (m_pTrial->IsVerified())
					str << L"*";
				else
					str << L"  "; // 2 spaces due to font (variable spacing)
				bNeedSpace = true;
				break;
			case IO_TREE_TRIAL_CLUB:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str << L" ";
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str << L"/";
						str << (*iter)->GetName();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_VENUE:
				{
					if (bNeedSpace && 0 < m_pTrial->GetClubs().size())
						str << L" ";
					int i = 0;
					for (ARBDogClubList::const_iterator iter = m_pTrial->GetClubs().begin();
						iter != m_pTrial->GetClubs().end();
						++iter, ++i)
					{
						if (0 < i)
							str << L"/";
						str << (*iter)->GetVenue();
						bNeedSpace = true;
					}
				}
				break;
			case IO_TREE_TRIAL_LOCATION:
				if (!m_pTrial->GetLocation().empty())
				{
					if (bNeedSpace)
						str << L" ";
					str << m_pTrial->GetLocation();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_NOTES:
				if (!m_pTrial->GetNote().empty())
				{
					if (bNeedSpace)
						str << L" ";
					str << StringUtil::Replace(m_pTrial->GetNote(), L"\n", L" ");
					bNeedSpace = true;
				}
				break;
			}
		}
		variant = str.str();
	}
	return variant;
}


ARBDogPtr CAgilityBookTreeDataTrial::GetDog() const
{
	CAgilityBookTreeDataDog const* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogPtr CAgilityBookTreeDataTrial::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


CAgilityBookTreeDataDog const* CAgilityBookTreeDataTrial::GetDataDog() const
{
	CAgilityBookTreeData const* pData = GetParent();
	CAgilityBookTreeDataDog const* pDog = dynamic_cast<CAgilityBookTreeDataDog const*>(pData);
	assert(pDog);
	return pDog;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataTrial::GetDataDog()
{
	CAgilityBookTreeData* pData = GetParent();
	CAgilityBookTreeDataDog* pDog = dynamic_cast<CAgilityBookTreeDataDog*>(pData);
	assert(pDog);
	return pDog;
}

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeDataRun::CAgilityBookTreeDataRun(
		CAgilityBookTreeModel* pModel,
		ARBDogRunPtr run)
	: CAgilityBookTreeData(pModel)
	, m_pRun(run)
{
	if (0 < m_pRun->GetCRCDRawMetaData().length())
		m_icon = CImageManager::Get()->CRCD();
	else if (0 < m_pRun->GetCRCD().length())
		m_icon = CImageManager::Get()->ARB16();
	else
		m_icon = CImageManager::Get()->Run();
}


CAgilityBookTreeDataRun::~CAgilityBookTreeDataRun()
{
}


wxVariant CAgilityBookTreeDataRun::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col) const
{
	wxVariant variant;
	if (0 <= col && m_pRun)
	{
		ARBDogPtr dog = GetDog();
		ARBDogTrialPtr trial = GetTrial();
		std::wostringstream str;
		short val;
		switch (columns[col])
		{
		default:
			break;

		case IO_RUNS_REG_NAME:
			str << dog->GetRegisteredName();
			break;
		case IO_RUNS_CALL_NAME:
			str << dog->GetCallName();
			break;
		case IO_RUNS_DATE:
			str << m_pRun->GetDate().GetString();
			break;
		case IO_RUNS_VENUE:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = trial->GetClubs().begin();
					iter != trial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetVenue();
				}
			}
			break;
		case IO_RUNS_CLUB:
			{
				int i = 0;
				for (ARBDogClubList::const_iterator iter = trial->GetClubs().begin();
					iter != trial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetName();
				}
			}
			break;
		case IO_RUNS_LOCATION:
			str << trial->GetLocation();
			break;
		case IO_RUNS_TRIAL_NOTES:
			str << trial->GetNote();
			break;
		case IO_RUNS_DIVISION:
			str << m_pRun->GetDivision();
			break;
		case IO_RUNS_LEVEL:
			str << m_pRun->GetLevel();
			break;
		case IO_RUNS_EVENT:
			str << m_pRun->GetEvent();
			break;
		case IO_RUNS_HEIGHT:
			str << m_pRun->GetHeight();
			break;
		case IO_RUNS_JUDGE:
			str << m_pRun->GetJudge();
			break;
		case IO_RUNS_HANDLER:
			str << m_pRun->GetHandler();
			break;
		case IO_RUNS_CONDITIONS:
			str << m_pRun->GetConditions();
			break;
		case IO_RUNS_COURSE_FAULTS:
			str << m_pRun->GetScoring().GetCourseFaults();
			break;
		case IO_RUNS_TIME:
			str << ARBDouble::ToString(m_pRun->GetScoring().GetTime());
			break;
		case IO_RUNS_YARDS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetYards())
			{
				str << ARBDouble::ToString(m_pRun->GetScoring().GetYards(), 0);
			}
			break;
		case IO_RUNS_MIN_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::ToString(yps, 3);
				}
			}
			break;
		case IO_RUNS_YPS:
			{
				double yps;
				if (m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
				{
					str << ARBDouble::ToString(yps, 3);
				}
			}
			break;
		case IO_RUNS_OBSTACLES:
			{
				short obstacles = m_pRun->GetScoring().GetObstacles();
				if (0 < obstacles)
					str << obstacles;
			}
			break;
		case IO_RUNS_OPS:
			{
				double ops;
				if (m_pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), CAgilityBookOptions::GetRunTimeInOPS(), ops))
				{
					str << ARBDouble::ToString(ops, 3);
				}
			}
			break;
		case IO_RUNS_SCT:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType()
			&& 0.0 < m_pRun->GetScoring().GetSCT())
			{
				str << ARBDouble::ToString(m_pRun->GetScoring().GetSCT());
			}
			break;
		case IO_RUNS_TOTAL_FAULTS:
			if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
			{
				ARBConfigScoringPtr pScoring;
				if (trial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						trial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				double faults = m_pRun->GetScoring().GetCourseFaults() + m_pRun->GetScoring().GetTimeFaults(pScoring);
				str << ARBDouble::ToString(faults, 0);
			}
			break;
		case IO_RUNS_REQ_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_REQ_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedClosePts();
			}
			break;
		case IO_RUNS_OPENING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_CLOSING:
			if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetClosePts();
			}
			break;
		case IO_RUNS_REQ_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetNeedOpenPts();
			}
			break;
		case IO_RUNS_POINTS:
			if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
			{
				str << m_pRun->GetScoring().GetOpenPts();
			}
			break;
		case IO_RUNS_PLACE:
			val = m_pRun->GetPlace();
			if (0 > val)
				str << L"?";
			else if (0 == val)
				str << L"-";
			else
				str << val;
			break;
		case IO_RUNS_IN_CLASS:
			val = m_pRun->GetInClass();
			if (0 >= val)
				str << L"?";
			else
				str << val;
			break;
		case IO_RUNS_DOGSQD:
			val = m_pRun->GetDogsQd();
			if (0 > val)
				str << L"?";
			else
				str << m_pRun->GetDogsQd();
			break;
		case IO_RUNS_Q:
			{
				bool bSet = false;
				if (m_pRun->GetQ().Qualified())
				{
					std::wstring q;
					std::vector<ARBConfigMultiQPtr> multiQs;
					if (0 < m_pRun->GetMultiQs(multiQs))
					{
						for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
						{
							if (!q.empty())
								q += L"/";
							q += (*iMultiQ)->GetShortName();
						}
					}
					if (ARB_Q::eQ_SuperQ == m_pRun->GetQ())
					{
						bSet = true;
						if (!q.empty())
							str << q << L"/";
						str << _("IDS_SQ");
					}
					else
					{
						if (!q.empty())
						{
							bSet = true;
							str << q;
						}
					}
				}
				if (!bSet)
					str << m_pRun->GetQ().str();
			}
			break;
		case IO_RUNS_SCORE:
			if (m_pRun->GetQ().Qualified()
			|| ARB_Q::eQ_NQ == m_pRun->GetQ())
			{
				ARBConfigScoringPtr pScoring;
				if (trial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						trial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					str << ARBDouble::ToString(m_pRun->GetScore(pScoring));
				}
			}
			break;
		case IO_RUNS_TITLE_POINTS:
			{
				double pts = 0;
				if (m_pRun->GetQ().Qualified())
				{
					ARBConfigScoringPtr pScoring;
					if (trial->GetClubs().GetPrimaryClub())
						config.GetVenues().FindEvent(
							trial->GetClubs().GetPrimaryClubVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate(),
							NULL,
							&pScoring);
					if (pScoring)
					{
						pts = m_pRun->GetTitlePoints(pScoring);
					}
				}
				str << pts;
			}
			break;
		case IO_RUNS_COMMENTS:
			str << StringUtil::Replace(m_pRun->GetNote(), L"\n", L" ");
			break;
		case IO_RUNS_FAULTS:
			{
				int i = 0;
				for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin();
					iter != m_pRun->GetFaults().end();
					++i, ++iter)
				{
					if (0 < i)
						str << L", ";
					str << (*iter);
				}
			}
			break;
		case IO_RUNS_SPEED:
			{
				ARBConfigScoringPtr pScoring;
				if (trial->GetClubs().GetPrimaryClub())
					config.GetVenues().FindEvent(
						trial->GetClubs().GetPrimaryClubVenue(),
						m_pRun->GetEvent(),
						m_pRun->GetDivision(),
						m_pRun->GetLevel(),
						m_pRun->GetDate(),
						NULL,
						&pScoring);
				if (pScoring)
				{
					if (pScoring->HasSpeedPts() && m_pRun->GetQ().Qualified())
					{
						str << m_pRun->GetSpeedPoints(pScoring);
					}
				}
			}
			break;
		}
		variant = str.str();
	}

	return variant;
}


ARBDogPtr CAgilityBookTreeDataRun::GetDog() const
{
	CAgilityBookTreeDataDog const* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogTrialPtr CAgilityBookTreeDataRun::GetTrial() const
{
	CAgilityBookTreeDataTrial const* pTrial = GetDataTrial();
	assert(pTrial);
	return pTrial->GetTrial();
}


ARBDogPtr CAgilityBookTreeDataRun::GetDog()
{
	CAgilityBookTreeDataDog* pDog = GetDataDog();
	assert(pDog);
	return pDog->GetDog();
}


ARBDogTrialPtr CAgilityBookTreeDataRun::GetTrial()
{
	CAgilityBookTreeDataTrial* pTrial = GetDataTrial();
	assert(pTrial);
	return pTrial->GetTrial();
}


CAgilityBookTreeDataDog const* CAgilityBookTreeDataRun::GetDataDog() const
{
	CAgilityBookTreeData const* pData = GetParent();
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial const* CAgilityBookTreeDataRun::GetDataTrial() const
{
	CAgilityBookTreeData const* pData = GetParent();
	CAgilityBookTreeDataTrial const* pTrial = dynamic_cast<CAgilityBookTreeDataTrial const*>(pData);
	assert(pTrial);
	return pTrial;
}


CAgilityBookTreeDataDog* CAgilityBookTreeDataRun::GetDataDog()
{
	CAgilityBookTreeData* pData = GetParent();
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial->GetDataDog();
}


CAgilityBookTreeDataTrial* CAgilityBookTreeDataRun::GetDataTrial()
{
	CAgilityBookTreeData* pData = GetParent();
	CAgilityBookTreeDataTrial* pTrial = dynamic_cast<CAgilityBookTreeDataTrial*>(pData);
	assert(pTrial);
	return pTrial;
}
