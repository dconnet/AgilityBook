#pragma once

/*
 * Copyright (c) 2004-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-28 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-02 DRC Show existing points in the list viewer.
 * @li 2004-07-29 DRC Created
 */

#include "ARBInfo.h"
#include "PointsData.h"
#include <set>
class ARBDate;
class CAgilityBookDoc;
class CReportListCtrl;

class CDlgListViewerData;
typedef tr1::shared_ptr<CDlgListViewerData> CDlgListViewerDataPtr;


class ScoringRunInfo
{
public:
	typedef enum
	{
		eNotScoringDetail,
		eScoringDeleted,
		eScoringChanged
	} eScoringDetail;
	ScoringRunInfo()
		: m_Dog()
		, m_Trial()
		, m_Run()
		, m_ScoringDetail(eNotScoringDetail)
	{
	}
	ScoringRunInfo(
			ARBDogPtr inDog,
			ARBDogTrialPtr inTrial,
			ARBDogRunPtr inRun,
			eScoringDetail inScoringDetail)
		: m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_ScoringDetail(inScoringDetail)
	{
	}
	ARBDogPtr m_Dog;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	eScoringDetail m_ScoringDetail;
};


class RunInfoData
{
	friend class CDlgListViewer;
public:
	RunInfoData(
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_Div(inDiv)
		, m_Level(inLevel)
		, m_Event(inEvent)
	{
	}
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	ARBConfigLevelPtr m_Level;
	ARBConfigEventPtr m_Event;
};


class MultiQInfoData
{
	friend class CDlgListViewer;
public:
	MultiQInfoData(
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_MultiQ(inMultiQ)
	{
	}
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
};


struct CFindItemInfo
{
	ARBInfo::eInfoType type;
	wxString name;
	ARBInfoItemPtr pItem;
};


class CDlgListViewer : public wxDialog
{
public:
	// Viewing runs
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			RunInfoData const* inData,
			std::list<RunInfo> const& inRuns,
			wxWindow* pParent = NULL);
	// Viewing runs affected by configuration changes
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			std::list<ScoringRunInfo> const& inScoringRuns,
			wxWindow* pParent = NULL);
	// Viewing multi-Qs
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			MultiQInfoData const* inData,
			std::set<MultiQdata> const& inMQs,
			wxWindow* pParent = NULL);
	// Viewing lifetime data
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			std::list<LifeTimePointInfoPtr> const& inLifetime,
			wxWindow* pParent = NULL);
	// Viewing other points
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			std::list<OtherPtInfo> const& inRunList,
			wxWindow* pParent = NULL);
	// Viewing Info
	CDlgListViewer(
			CAgilityBookDoc* inDoc,
			wxString const& inCaption,
			std::vector<CFindItemInfo> const& inItems,
			wxWindow* pParent = NULL);

	CDlgListViewerDataPtr GetDataByData(long data) const;

private:
	bool Create(
			wxString const& inCaption,
			wxWindow* pParent = NULL);
	void FinishCreate();

	CReportListCtrl* m_ctrlList;
	wxButton* m_ctrlCopy;

	CAgilityBookDoc* m_pDoc;
	int m_SortColumn;

	void OnColumnClick(wxListEvent& evt);
	void OnItemSelected(wxListEvent& evt);
	void OnCopy(wxCommandEvent& evt);
};
