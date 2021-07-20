#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgListViewer class
 * @author David Connet
 *
 * Revision History
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-28 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-03-14 Show a summary of lifetime points in the list viewer.
 * 2005-01-02 Show existing points in the list viewer.
 * 2004-07-29 Created
 */

#include "ARB/ARBInfo.h"
#include "PointsData.h"
#include <set>
class ARBDate;
class CAgilityBookDoc;
class CReportListCtrl;

class CDlgListViewerData;
typedef std::shared_ptr<CDlgListViewerData> CDlgListViewerDataPtr;


class ScoringRunInfo
{
public:
	enum class ScoringDetail
	{
		None,
		Deleted,
		Changed
	};
	ScoringRunInfo()
		: m_Dog()
		, m_Trial()
		, m_Run()
		, m_ScoringDetail(ScoringDetail::None)
	{
	}
	ScoringRunInfo(
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		ScoringDetail inScoringDetail)
		: m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_ScoringDetail(inScoringDetail)
	{
	}
	ARBDogPtr m_Dog;
	ARBDogTrialPtr m_Trial;
	ARBDogRunPtr m_Run;
	ScoringDetail m_ScoringDetail;
};


class RunInfoData
{
	friend class CDlgListViewer;

public:
	RunInfoData(
		ARBDogPtr const& inDog,
		ARBConfigVenuePtr const& inVenue,
		ARBConfigDivisionPtr const& inDiv,
		ARBConfigLevelPtr const& inLevel,
		ARBConfigEventPtr const& inEvent)
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
	MultiQInfoData(ARBDogPtr const& inDog, ARBConfigVenuePtr const& inVenue, ARBConfigMultiQPtr const& inMultiQ)
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
	ARBInfoType type;
	std::wstring name;
	ARBInfoItemPtr pItem;
	CFindItemInfo()
		: type(ARBInfoType::Club)
		, name()
		, pItem()
	{
	}
};


class CDlgListViewer : public wxDialog
{
public:
	// Viewing runs
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		RunInfoData const* inData,
		std::list<RunInfo> const& inRuns,
		wxWindow* pParent = nullptr);
	// Viewing runs affected by configuration changes
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		std::list<ScoringRunInfo> const& inScoringRuns,
		wxWindow* pParent = nullptr);
	// Viewing multi-Qs
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		MultiQInfoData const* inData,
		std::set<MultiQdata> const& inMQs,
		wxWindow* pParent = nullptr);
	// Viewing lifetime data
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		std::list<LifeTimePointInfoPtr> const& inLifetime,
		wxWindow* pParent = nullptr);
	// Viewing other points
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		std::list<OtherPtInfo> const& inRunList,
		wxWindow* pParent = nullptr);
	// Viewing Info
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		std::wstring const& inCaption,
		std::vector<CFindItemInfo> const& inItems,
		wxWindow* pParent = nullptr);

private:
	bool Create(std::wstring const& inCaption, wxWindow* pParent = nullptr);
	void FinishCreate();
	void UpdateControls();

	CReportListCtrl* m_ctrlList;
	wxButton* m_ctrlCopy;

	CAgilityBookDoc* m_pDoc;
	int m_SortColumn;

	void OnColumnClick(wxListEvent& evt);
	void OnCopy(wxCommandEvent& evt);
};
