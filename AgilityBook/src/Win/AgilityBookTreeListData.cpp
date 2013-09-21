/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CAgilityBookTreeListData classes
 * @author David Connet
 *
 * Revision History
 * @li 2013-04-22 DRC Converted tree+list into single control.
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
#if USE_TREELIST
#include "AgilityBookTreeListData.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookMenu.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTreeListModel.h"
#include "AgilityBookTreeView.h"
#include "ClipBoard.h"
#include "DlgAssignColumns.h"
#include "FilterOptions.h"
#include "Globals.h"
#include "IconList.h"
#include "ImageManager.h"
#include "PointsData.h"
#include "Print.h"
#include "TabView.h"

#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeListData::CAgilityBookTreeListData(CAgilityBookTreeListModel* pModel)
	: m_pModel(pModel)
	, m_icon()
	, m_parent(NULL)
{
}


CAgilityBookTreeListData::~CAgilityBookTreeListData()
{
	for (std::vector<CAgilityBookTreeListData*>::iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		delete *i;
	}
	m_children.clear();
}


unsigned int CAgilityBookTreeListData::GetChildren(
		wxDataViewItemArray& array) const
{
	for (std::vector<CAgilityBookTreeListData*>::const_iterator i = m_children.begin();
		i != m_children.end();
		++i)
	{
		array.Add(wxDataViewItem(*i));
	}
	return static_cast<unsigned int>(m_children.size());
}


void CAgilityBookTreeListData::Append(CAgilityBookTreeListData* child)
{
	child->m_parent = this;
	m_children.push_back(child);
}


void CAgilityBookTreeListData::Remove(CAgilityBookTreeListData* child)
{
	std::vector<CAgilityBookTreeListData*>::iterator i = std::find(m_children.begin(), m_children.end(), child);
	if (i != m_children.end())
	{
		CAgilityBookTreeListData* node = *i;
		m_children.erase(i);
		delete node;
	}
}

////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeListDataDog::CAgilityBookTreeListDataDog(
		CAgilityBookTreeListModel* pModel,
		ARBDogPtr dog)
	: CAgilityBookTreeListData(pModel)
	, m_pDog(dog)
{
	m_icon = CImageManager::Get()->Dog();
}


CAgilityBookTreeListDataDog::~CAgilityBookTreeListDataDog()
{
}


wxVariant CAgilityBookTreeListDataDog::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col,
		bool bInCompare) const
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

CAgilityBookTreeListDataTrial::CAgilityBookTreeListDataTrial(
		CAgilityBookTreeListModel* pModel,
		ARBDogPtr dog,
		ARBDogTrialPtr trial)
	: CAgilityBookTreeListData(pModel)
	, m_pDog(dog)
	, m_pTrial(trial)
{
	CIconList icons;
	m_icon = icons.GetIcon(icons.Trial(m_pModel->GetDocument()->Book().GetConfig(), m_pTrial));
}


CAgilityBookTreeListDataTrial::~CAgilityBookTreeListDataTrial()
{
}


wxVariant CAgilityBookTreeListDataTrial::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col,
		bool bInCompare) const
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
				if (m_pTrial->GetStartDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_END == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetStartDate().GetString();
					bNeedSpace = true;
				}
				break;
			case IO_TREE_TRIAL_END:
				if (m_pTrial->GetEndDate().IsValid())
				{
					if (bNeedSpace)
					{
						if (IO_TREE_TRIAL_START == columns[idx-1])
							str << L"-";
						else
							str << L" ";
					}
					str << m_pTrial->GetEndDate().GetString();
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

/////////////////////////////////////////////////////////////////////////////

CAgilityBookTreeListDataRun::CAgilityBookTreeListDataRun(
		CAgilityBookTreeListModel* pModel,
		ARBDogPtr dog,
		ARBDogTrialPtr trial,
		ARBDogRunPtr run)
	: CAgilityBookTreeListData(pModel)
	, m_pDog(dog)
	, m_pTrial(trial)
	, m_pRun(run)
{
	if (0 < m_pRun->GetCRCDRawMetaData().length())
		m_icon = CImageManager::Get()->CRCD();
	else if (0 < m_pRun->GetCRCD().length())
		m_icon = CImageManager::Get()->ARB16();
	else
		m_icon = CImageManager::Get()->Run();
}


CAgilityBookTreeListDataRun::~CAgilityBookTreeListDataRun()
{
}


wxVariant CAgilityBookTreeListDataRun::GetColumn(
		ARBConfig const& config,
		std::vector<long> const& columns,
		unsigned int col,
		bool bInCompare) const
{
	wxVariant variant;
	if (0 <= col && m_pRun)
	{
		ARBDogPtr dog = GetDog();
		ARBDogTrialPtr trial = GetTrial();
		switch (columns[col])
		{
		default:
			break;

		case IO_RUNS_REG_NAME:
			variant = dog->GetRegisteredName().c_str();
			break;
		case IO_RUNS_CALL_NAME:
			variant = dog->GetCallName().c_str();
			break;
		case IO_RUNS_DATE:
			if (bInCompare)
			{
				wxDateTime date;
				m_pRun->GetDate().GetDate(date);
				variant = date;
			}
			else
				variant = m_pRun->GetDate().GetString().c_str();
			break;
		case IO_RUNS_VENUE:
			{
				std::wostringstream str;
				int i = 0;
				for (ARBDogClubList::const_iterator iter = trial->GetClubs().begin();
					iter != trial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetVenue();
				}
				variant = str.str();
			}
			break;
		case IO_RUNS_CLUB:
			{
				std::wostringstream str;
				int i = 0;
				for (ARBDogClubList::const_iterator iter = trial->GetClubs().begin();
					iter != trial->GetClubs().end();
					++iter, ++i)
				{
					if (0 < i)
						str << L"/";
					str << (*iter)->GetName();
				}
				variant = str.str();
			}
			break;
		case IO_RUNS_LOCATION:
			variant = trial->GetLocation().c_str();
			break;
		case IO_RUNS_TRIAL_NOTES:
			variant = trial->GetNote().c_str();
			break;
		case IO_RUNS_DIVISION:
			variant = m_pRun->GetDivision().c_str();
			break;
		case IO_RUNS_LEVEL:
			variant = m_pRun->GetLevel().c_str();
			break;
		case IO_RUNS_EVENT:
			variant = m_pRun->GetEvent().c_str();
			break;
		case IO_RUNS_HEIGHT:
			variant = m_pRun->GetHeight().c_str();
			break;
		case IO_RUNS_JUDGE:
			variant = m_pRun->GetJudge().c_str();
			break;
		case IO_RUNS_HANDLER:
			variant = m_pRun->GetHandler().c_str();
			break;
		case IO_RUNS_CONDITIONS:
			variant = m_pRun->GetConditions().c_str();
			break;
		case IO_RUNS_COURSE_FAULTS:
			if (bInCompare)
				variant = long(m_pRun->GetScoring().GetCourseFaults());
			else
			{
				std::wostringstream str;
				str << m_pRun->GetScoring().GetCourseFaults();
				variant = str.str();
			}
			break;
		case IO_RUNS_TIME:
			if (bInCompare)
				variant = m_pRun->GetScoring().GetTime();
			else
				variant = ARBDouble::ToString(m_pRun->GetScoring().GetTime());
			break;
		case IO_RUNS_YARDS:
			{
				double yds = -1.0;
				if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
				{
					yds = m_pRun->GetScoring().GetYards();
					if (!bInCompare && 0.0 < yds)
					{
						variant = ARBDouble::ToString(yds, 0);
					}
				}
				if (bInCompare)
					variant = yds;
			}
			break;
		case IO_RUNS_MIN_YPS:
			{
				double yps = 0.0;
				bool bSet = m_pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps);
				if (bInCompare)
					variant = yps;
				else if (bSet)
					variant = ARBDouble::ToString(yps, 3);
			}
			break;
		case IO_RUNS_YPS:
			{
				double yps = 0.0;
				bool bSet = m_pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps);
				if (bInCompare)
					variant = yps;
				else if (bSet)
					variant = ARBDouble::ToString(yps, 3);
			}
			break;
		case IO_RUNS_OBSTACLES:
			{
				long obstacles = m_pRun->GetScoring().GetObstacles();
				if (bInCompare)
					variant = obstacles;
				else if (0 < obstacles)
				{
					std::wostringstream str;
					str << obstacles;
					variant = str.str();
				}
			}
			break;
		case IO_RUNS_OPS:
			{
				double ops = 0.0;
				bool bSet = m_pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), CAgilityBookOptions::GetRunTimeInOPS(), ops);
				if (bInCompare)
					variant = ops;
				else if (bSet)
					variant = ARBDouble::ToString(ops, 3);
			}
			break;
		case IO_RUNS_SCT:
			{
				double sct = -1.0;
				if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
				{
					sct = m_pRun->GetScoring().GetSCT();
					if (!bInCompare && 0.0 < sct)
					{
						variant = ARBDouble::ToString(sct);
					}
				}
				if (bInCompare)
					variant = sct;
			}
			break;
		case IO_RUNS_TOTAL_FAULTS:
			{
				double faults = -1.0;
				if (ARBDogRunScoring::eTypeByTime == m_pRun->GetScoring().GetType())
				{
					ARBConfigScoringPtr pScoring;
					if (trial->GetClubs().GetPrimaryClub())
					{
						config.GetVenues().FindEvent(
							trial->GetClubs().GetPrimaryClubVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate(),
							NULL,
							&pScoring);
					}
					faults = m_pRun->GetScoring().GetCourseFaults() + m_pRun->GetScoring().GetTimeFaults(pScoring);
					if (!bInCompare)
						variant = ARBDouble::ToString(faults, 0);
				}
				if (bInCompare)
					variant = faults;
			}
			break;
		case IO_RUNS_REQ_OPENING:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetNeedOpenPts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_REQ_CLOSING:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetNeedClosePts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_OPENING:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetOpenPts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_CLOSING:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByOpenClose == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetClosePts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_REQ_POINTS:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetNeedOpenPts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_POINTS:
			{
				long pts = -1;
				if (ARBDogRunScoring::eTypeByPoints == m_pRun->GetScoring().GetType())
				{
					pts = m_pRun->GetScoring().GetOpenPts();
					if (!bInCompare)
					{
						std::wostringstream str;
						str << pts;
						variant = str.str();
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		case IO_RUNS_PLACE:
			{
				long val = m_pRun->GetPlace();
				if (bInCompare)
				{
					variant = val;
				}
				else
				{
					if (0 > val)
						variant = L"?";
					else if (0 == val)
						variant = L"-";
					else
					{
						std::wostringstream str;
						str << val;
						variant = str.str();
					}
				}
			}
			break;
		case IO_RUNS_IN_CLASS:
			{
				long val = m_pRun->GetInClass();
				if (bInCompare)
					variant = val;
				else
				{
					if (0 >= val)
						variant = L"?";
					else
					{
						std::wostringstream str;
						str << val;
						variant = str.str();
					}
				}
			}
			break;
		case IO_RUNS_DOGSQD:
			{
				long val = m_pRun->GetDogsQd();
				if (bInCompare)
					variant = val;
				else
				{
					if (0 > val)
						variant = L"?";
					else
					{
						std::wostringstream str;
						str << val;
						variant = str.str();
					}
				}
			}
			break;
		case IO_RUNS_Q:
			{
				bool bSet = false;
				std::wostringstream str;
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
				if (bSet)
					variant = str.str();
				else
					variant = m_pRun->GetQ().str();
			}
			break;
		case IO_RUNS_SCORE:
			{
				double score = -2.0;
				if (m_pRun->GetQ().Qualified()
				|| ARB_Q::eQ_NQ == m_pRun->GetQ())
				{
					ARBConfigScoringPtr pScoring;
					if (trial->GetClubs().GetPrimaryClub())
					{
						config.GetVenues().FindEvent(
							trial->GetClubs().GetPrimaryClubVenue(),
							m_pRun->GetEvent(),
							m_pRun->GetDivision(),
							m_pRun->GetLevel(),
							m_pRun->GetDate(),
							NULL,
							&pScoring);
					}
					if (pScoring)
					{
						score = m_pRun->GetScore(pScoring);
						if (!bInCompare)
							variant = ARBDouble::ToString(score);
					}
					else if (bInCompare)
						score = -1.0;
				}
				if (bInCompare)
					variant = score;
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
				if (bInCompare)
					variant = pts;
				else
				{
					std::wostringstream str;
					str << pts;
					variant = str.str();
				}
			}
			break;
		case IO_RUNS_COMMENTS:
			variant = StringUtil::Replace(m_pRun->GetNote(), L"\n", L" ");
			break;
		case IO_RUNS_FAULTS:
			{
				std::wostringstream str;
				int i = 0;
				for (ARBDogFaultList::const_iterator iter = m_pRun->GetFaults().begin();
					iter != m_pRun->GetFaults().end();
					++i, ++iter)
				{
					if (0 < i)
						str << L", ";
					str << (*iter);
				}
				variant = str.str();
			}
			break;
		case IO_RUNS_SPEED:
			{
				long pts = -1;
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
						pts = m_pRun->GetSpeedPoints(pScoring);
						if (!bInCompare)
						{
							std::wostringstream str;
							str << pts;
							variant = str.str();
						}
					}
				}
				if (bInCompare)
					variant = pts;
			}
			break;
		}
	}

	if (variant.IsNull())
		variant = L"";

	return variant;
}

#endif // USE_TREELIST
