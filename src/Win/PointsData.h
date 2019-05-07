#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
 * 2019-05-04 Reworked PointsData usage.
 * 2017-08-20 Add CPointsDataHeader
 * 2011-08-13 Don't copy internal url links to the clipboard.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-26 Ported to wxWidgets.
 * 2006-07-15 Add option to sort by event instead of division.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-18 Remember last selected item when reloading data.
 * 2005-10-14 Added a context menu.
 * 2005-05-04 Added subtotaling by division to lifetime points.
 * 2005-03-14 Show a summary of lifetime points in the list viewer.
 * 2005-01-02 Show existing points in the list viewer.
 * 2005-01-01 Renamed MachPts to SpeedPts.
 * 2004-12-03 Show all lifetime points when filtering.
 * 2004-08-06 Created
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/ListData.h"
#include <list>
#include <set>
#include <vector>
class CAgilityBookDoc;
struct CVenueFilter;

typedef std::pair<ARBDate, ARBDogTrialPtr> MultiQdata;

// See SetupARBPost.h
using std::tuple;
using std::get;

#define TUPLE_DOG	0
#define TUPLE_TRIAL	1
#define TUPLE_RUN	2
typedef tuple<ARBDogPtr, ARBDogTrialPtr, ARBDogRunPtr> RunInfo;

// Special HTML tag for href's
#define ARB_PROTOCOL	L"arb:"

/**
 * Used to accumulate lifetime info. Also for Placement totals.
 */
class LifeTimePointInfo
{
private:
	LifeTimePointInfo() = delete;
public:
	LifeTimePointInfo(
			std::wstring const& inSort1, // Division or event
			std::wstring const& inSort2, // Level or empty
			double inPoints,
			double inFiltered)
		: sort1(inSort1)
		, sort2(inSort2)
		, points(inPoints)
		, filtered(inFiltered)
	{
	}
	std::wstring sort1;
	std::wstring sort2;
	double points;
	double filtered;
};
typedef std::shared_ptr<LifeTimePointInfo> LifeTimePointInfoPtr;

/**
 * Used to accumulate run info into a flat list.
 */
class OtherPtInfo
{
private:
	OtherPtInfo() = delete;
public:
	OtherPtInfo(
			ARBDogTrialPtr const& inTrial,
			ARBDogRunPtr const& inRun,
			double points,
			bool bScore, // m_Score is valid
			double score);
	OtherPtInfo(ARBDogExistingPointsPtr const& inExisting);
	OtherPtInfo(OtherPtInfo const&) = default;
	OtherPtInfo(OtherPtInfo&&) = default;
	OtherPtInfo& operator=(OtherPtInfo const&) = default;
	OtherPtInfo& operator=(OtherPtInfo&&) = default;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
	ARBDogExistingPointsPtr m_pExisting;
	std::wstring m_Venue;
	std::wstring m_Div;
	std::wstring m_Level;
	std::wstring m_Event;
	double m_Points;
	bool m_bScore;
	double m_Score;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all titles in a venue.
 */
class CPointsDataTitle
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataTitle)
public:
	CPointsDataTitle(
			CAgilityBookDoc* pDoc,
			ARBDogTitlePtr pTitle);
	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	bool Details(
			std::wstring const& link,
			ARBDogPtr pDog);

private:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBDogTitlePtr m_pTitle;
};
typedef std::shared_ptr<CPointsDataTitle> CPointsDataTitlePtr;

/////////////////////////////////////////////////////////////////////////////

class CRefTag
{
	size_t m_id;

public:
	CRefTag() : m_id(0) {}

	size_t GetId() { ++m_id; return m_id; }
	void reset() { m_id = 0; }
};

/**
 * Keeps track of all runs in a venue.
 */
class CPointsDataEvent
{
	friend class SortPointItems;
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataEvent)
public:
	CPointsDataEvent(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			std::list<RunInfo>& inMatching,
			ARBConfigVenuePtr const& inVenue,
			ARBConfigDivisionPtr const& inDiv,
			int inDivIdx,
			ARBConfigLevelPtr const& inLevel,
			int inLevelIdx,
			ARBConfigEventPtr const& inEvent,
			int inEventIdx,
			std::wstring const& inRunCount,
			std::wstring const& inQcount,
			std::wstring const& inPts,
			std::wstring const& inSuperQ,
			std::wstring const& inSpeed,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_Dog;
	std::list<RunInfo> m_Matching;
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	int m_DivIdx;
	ARBConfigLevelPtr m_Level;
	int m_LevelIdx;
	ARBConfigEventPtr m_Event;
	int m_EventIdx;
	std::wstring m_RunCount;
	std::wstring m_Qcount;
	std::wstring m_Pts;
	std::wstring m_SuperQ;
	std::wstring m_Speed;
};
typedef std::shared_ptr<CPointsDataEvent> CPointsDataEventPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points in a venue.
 */
class CPointsDataSpeedPts
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataSpeedPts)
public:
	CPointsDataSpeedPts(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue,
			ARBConfigDivisionPtr const& inDiv,
			int inPts,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	int m_Pts;
};
typedef std::shared_ptr<CPointsDataSpeedPts> CPointsDataSpeedPtsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs in a venue.
 */
class CPointsDataMultiQs
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataMultiQs)
public:
	CPointsDataMultiQs(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBConfigVenuePtr const& inVenue,
			ARBConfigMultiQPtr const& inMultiQ,
			std::set<MultiQdata> const& inMQs,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
	std::set<MultiQdata> m_MQs;
	double m_ExistingDblQs;
};
typedef std::shared_ptr<CPointsDataMultiQs> CPointsDataMultiQsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all lifetime points in a venue.
 */
class CPointsDataLifetime
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataLifetime)
public:
	CPointsDataLifetime(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue,
			CRefTag& id);
	CPointsDataLifetime(
			CAgilityBookDoc* pDoc,
			ARBConfigLifetimeNamePtr const& inLifetimeName,
			ARBConfigVenuePtr const& inVenue,
			CRefTag& id);
	virtual void AddLifetimeInfo(
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	virtual bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBConfigLifetimeNamePtr m_LifetimeName;	///< null implies Placement
	ARBConfigVenuePtr m_Venue;
	std::list<LifeTimePointInfoPtr> m_Data;
	double m_Lifetime;	///< Total lifetime points.
	double m_Filtered;	///< Points that are filtered out.
};
typedef std::shared_ptr<CPointsDataLifetime> CPointsDataLifetimePtr;

/**
 * Subtotal lifetime points by division.
 */
class CPointsDataLifetimeByName : public CPointsDataLifetime
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataLifetimeByName)
public:
	CPointsDataLifetimeByName(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue,
			std::wstring const& inName,
			CRefTag& id);
	CPointsDataLifetimeByName(
			CAgilityBookDoc* pDoc,
			ARBConfigLifetimeNamePtr const& inLifetimeName,
			ARBConfigVenuePtr const& inVenue,
			std::wstring const& inName,
			CRefTag& id);

	// Adds are limited by inSort1 == m_Name
	void AddLifetimeInfo(
			std::wstring const& inSort1,
			std::wstring const& inSort2,
			double inLifetime,
			double inFiltered) override;

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) override;

protected:
	std::wstring m_Name;
};
typedef std::shared_ptr<CPointsDataLifetimeByName> CPointsDataLifetimeByNamePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all venue data.
 */
class CPointsDataVenue
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataVenue)
public:
	CPointsDataVenue(
			std::vector<CVenueFilter> const& venues,
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBConfigVenuePtr const inVenue,
			CRefTag& id);

	bool HasData() const;
	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks);
	bool Details(std::wstring const& link);

private:
	struct LifeTimePoint
	{
		std::wstring eventName;
		double points;
		bool bFiltered;
		LifeTimePoint()
			: eventName()
			, points(0.0)
			, bFiltered(false)
		{
		}
		LifeTimePoint(
				std::wstring const& inEvent,
				double inPoints,
				bool inFiltered)
			: eventName(inEvent)
			, points(inPoints)
			, bFiltered(inFiltered)
		{
		}
	};
	typedef std::list<LifeTimePoint> LifeTimePointList;
	struct LifeTimePoints
	{
		ARBConfigDivisionPtr pDiv;
		ARBConfigLevelPtr pLevel;
		std::map<ARBConfigLifetimeNamePtr, LifeTimePointList> ptLifetime;
	};
	typedef std::list<LifeTimePoints> LifeTimePointsList;
	struct PlacementPoints
	{
		ARBConfigDivisionPtr pDiv;
		ARBConfigLevelPtr pLevel;
		LifeTimePointList ptPlacement;
	};
	typedef std::list<PlacementPoints> PlacementPointsList;

	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	ARBConfigVenuePtr m_pVenue;
	std::vector<CPointsDataTitlePtr> m_titles;
	std::vector<CPointsDataEventPtr> m_events;
	std::vector<CPointsDataSpeedPtsPtr> m_speedPts;
	std::vector<CPointsDataMultiQsPtr> m_multiQs;
	std::vector<CPointsDataLifetimePtr> m_lifetime;
};
typedef std::shared_ptr<CPointsDataVenue> CPointsDataVenuePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Other points data base class.
 */
class CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPoints)
public:
	CPointsDataOtherPoints(
			CAgilityBookDoc* pDoc,
			ARBConfigOtherPointsPtr pOther,
			std::list<OtherPtInfo> const& inRunList,
			CRefTag& id);
	virtual void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) = 0;
	virtual bool Details(std::wstring const& link) = 0;
	ARBConfigOtherPointsPtr GetOther() const { return m_pOther; }

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARBConfigOtherPointsPtr m_pOther;
	std::list<OtherPtInfo> m_RunList;
	double m_Points;
};
typedef std::shared_ptr<CPointsDataOtherPoints> CPointsDataOtherPointsPtr;

class CPointsDataOtherPointsTallyAll : public CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPointsTallyAll)
public:
	CPointsDataOtherPointsTallyAll(
			CAgilityBookDoc* pDoc,
			ARBConfigOtherPointsPtr pOther,
			std::list<OtherPtInfo> const& inRunList,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) override;
	bool Details(std::wstring const& link) override;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPointsTallyAllByEvent)
public:
	CPointsDataOtherPointsTallyAllByEvent(
			CAgilityBookDoc* pDoc,
			ARBConfigOtherPointsPtr pOther,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) override;
	bool Details(std::wstring const& link) override;

protected:
	std::wstring m_Event;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyAllByEvent> CPointsDataOtherPointsTallyAllByEventPtr;

class CPointsDataOtherPointsTallyLevel : public CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPointsTallyLevel)
public:
	CPointsDataOtherPointsTallyLevel(
			CAgilityBookDoc* pDoc,
			ARBConfigOtherPointsPtr pOther,
			std::wstring const& inLevel,
			std::list<OtherPtInfo> const& inRunList,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) override;
	bool Details(std::wstring const& link) override;

protected:
	std::wstring m_Level;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyLevel> CPointsDataOtherPointsTallyLevelPtr;

class CPointsDataOtherPointsTallyLevelByEvent : public CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPointsTallyLevelByEvent)
public:
	CPointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookDoc* pDoc,
			ARBConfigOtherPointsPtr pOther,
			std::wstring const& inLevel,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList,
			CRefTag& id);

	void GetHtml(
			fmt::wmemory_buffer& data,
			bool bNoInternalLinks) override;
	bool Details(std::wstring const& link) override;

protected:
	std::wstring m_Level;
	std::wstring m_Event;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyLevelByEvent> CPointsDataOtherPointsTallyLevelByEventPtr;

/////////////////////////////////////////////////////////////////////////////

class CPointsDataItems
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataItems)
public:
	CPointsDataItems(CAgilityBookDoc* pDoc);

	void clear();

	void LoadData(ARBDogPtr const& inDog);

	wxString GetHtml(
			bool bFragment,
			bool bNoInternalLinks);
	bool Details(std::wstring const& link);

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogPtr m_pDog;
	std::vector<CPointsDataVenuePtr> m_venues;
	std::vector< CPointsDataOtherPointsPtr> m_otherPts;
};
