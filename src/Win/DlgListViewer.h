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
#include "LibARBWin/Logger.h"
#include "PointsData.h"
#include <set>


namespace dconSoft
{
namespace ARBWin
{
class CReportListCtrl;
} // namespace ARBWin
class CAgilityBookDoc;

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
		ARB::ARBDogPtr const& inDog,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		ScoringDetail inScoringDetail)
		: m_Dog(inDog)
		, m_Trial(inTrial)
		, m_Run(inRun)
		, m_ScoringDetail(inScoringDetail)
	{
	}
	ARB::ARBDogPtr m_Dog;
	ARB::ARBDogTrialPtr m_Trial;
	ARB::ARBDogRunPtr m_Run;
	ScoringDetail m_ScoringDetail;
};


class RunInfoData
{
	friend class CDlgListViewer;

public:
	RunInfoData(
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigDivisionPtr const& inDiv,
		ARB::ARBConfigLevelPtr const& inLevel,
		ARB::ARBConfigEventPtr const& inEvent)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_Div(inDiv)
		, m_Level(inLevel)
		, m_Event(inEvent)
	{
	}
	ARB::ARBDogPtr m_Dog;
	ARB::ARBConfigVenuePtr m_Venue;
	ARB::ARBConfigDivisionPtr m_Div;
	ARB::ARBConfigLevelPtr m_Level;
	ARB::ARBConfigEventPtr m_Event;
};


class MultiQInfoData
{
	friend class CDlgListViewer;

public:
	MultiQInfoData(
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigMultiQPtr const& inMultiQ)
		: m_Dog(inDog)
		, m_Venue(inVenue)
		, m_MultiQ(inMultiQ)
	{
	}
	ARB::ARBDogPtr m_Dog;
	ARB::ARBConfigVenuePtr m_Venue;
	ARB::ARBConfigMultiQPtr m_MultiQ;
};


struct CFindItemInfo
{
	ARB::ARBInfoType type;
	wxString name;
	ARB::ARBInfoItemPtr pItem;
	CFindItemInfo()
		: type(ARB::ARBInfoType::Club)
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
		wxString const& inCaption,
		RunInfoData const* inData,
		std::list<RunInfo> const& inRuns,
		wxWindow* pParent = nullptr);
	// Viewing runs affected by configuration changes
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		std::list<ScoringRunInfo> const& inScoringRuns,
		wxWindow* pParent = nullptr);
	// Viewing multi-Qs
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		MultiQInfoData const* inData,
		std::set<MultiQdata> const& inMQs,
		wxWindow* pParent = nullptr);
	// Viewing lifetime data
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		std::list<LifeTimePointInfoPtr> const& inLifetime,
		wxWindow* pParent = nullptr);
	// Viewing other points
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		std::list<OtherPtInfo> const& inRunList,
		wxWindow* pParent = nullptr);
	// Viewing Info
	CDlgListViewer(
		CAgilityBookDoc* inDoc,
		wxString const& inCaption,
		std::vector<CFindItemInfo> const& inItems,
		wxWindow* pParent = nullptr);

private:
	bool Create(wxString const& inCaption, wxWindow* pParent = nullptr);
	void FinishCreate();
	void UpdateControls();

	ARBWin::CReportListCtrl* m_ctrlList;
	wxButton* m_ctrlCopy;
	CAgilityBookDoc* m_pDoc;
	int m_SortColumn;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnItemFocused(wxListEvent& evt);
	void OnColumnClick(wxListEvent& evt);
	void OnCopy(wxCommandEvent& evt);
};

} // namespace dconSoft
