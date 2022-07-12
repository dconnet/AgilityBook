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
 * 2021-06-08 Fix double Q counting when 2 shows are on one day.
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


namespace dconSoft
{
class CAgilityBookDoc;
struct CVenueFilter;


// See SetupARBPost.h
using std::get;
using std::tuple;

constexpr int TUPLE_DOG = 0;
constexpr int TUPLE_TRIAL = 1;
constexpr int TUPLE_RUN = 2;
typedef tuple<ARB::ARBDogPtr, ARB::ARBDogTrialPtr, ARB::ARBDogRunPtr> RunInfo;

// Special HTML tag for href's
#define ARB_PROTOCOL L"arb:"


/**
 * Used to accumulate multiQ info.
 */
class MultiQdata
{
private:
	MultiQdata() = delete;

public:
	MultiQdata(ARBCommon::ARBDate inDate, ARB::ARBDogTrialPtr inTrial, std::wstring const& inClub)
		: date(inDate)
		, trial(inTrial)
		, club(inClub)
	{
	}
	MultiQdata(MultiQdata const&) = default;
	MultiQdata(MultiQdata&&) = default;
	~MultiQdata()
	{
	}
	MultiQdata& operator=(MultiQdata const&) = default;
	MultiQdata& operator=(MultiQdata&&) = default;

	bool operator<(MultiQdata const& rhs) const
	{
		if (date == rhs.date)
		{
			// Comparing pointers is fine - just want 2 trials to sort different.
			return trial.get() < rhs.trial.get();
		}
		return date < rhs.date;
	}

	ARBCommon::ARBDate date;
	ARB::ARBDogTrialPtr trial;
	std::wstring club;
};

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
	LifeTimePointInfo(LifeTimePointInfo const&) = default;
	LifeTimePointInfo(LifeTimePointInfo&&) = default;
	LifeTimePointInfo& operator=(LifeTimePointInfo const&) = default;
	LifeTimePointInfo& operator=(LifeTimePointInfo&&) = default;
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
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		double points,
		bool bScore, // m_Score is valid
		double score);
	OtherPtInfo(ARB::ARBDogExistingPointsPtr const& inExisting);
	OtherPtInfo(OtherPtInfo const&) = default;
	OtherPtInfo(OtherPtInfo&&) = default;
	~OtherPtInfo()
	{
	}
	OtherPtInfo& operator=(OtherPtInfo const&) = default;
	OtherPtInfo& operator=(OtherPtInfo&&) = default;
	ARB::ARBDogTrialPtr m_pTrial;
	ARB::ARBDogRunPtr m_pRun;
	ARB::ARBDogExistingPointsPtr m_pExisting;
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
	CPointsDataTitle(CAgilityBookDoc* pDoc, ARB::ARBDogTitlePtr pTitle);
	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
	bool Details(std::wstring const& link, ARB::ARBDogPtr pDog);

private:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogTitlePtr m_pTitle;
};
typedef std::shared_ptr<CPointsDataTitle> CPointsDataTitlePtr;

/////////////////////////////////////////////////////////////////////////////

class CRefTag
{
	size_t m_id;

public:
	CRefTag()
		: m_id(0)
	{
	}

	size_t GetId()
	{
		++m_id;
		return m_id;
	}
	void reset()
	{
		m_id = 0;
	}
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
		ARB::ARBDogPtr const& inDog,
		std::list<RunInfo>& inMatching,
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigDivisionPtr const& inDiv,
		int inDivIdx,
		ARB::ARBConfigLevelPtr const& inLevel,
		int inLevelIdx,
		ARB::ARBConfigEventPtr const& inEvent,
		int inEventIdx,
		std::wstring const& inRunCount,
		std::wstring const& inQcount,
		std::wstring const& inPts,
		std::wstring const& inSuperQ,
		std::wstring const& inSpeed,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogPtr m_Dog;
	std::list<RunInfo> m_Matching;
	ARB::ARBConfigVenuePtr m_Venue;
	ARB::ARBConfigDivisionPtr m_Div;
	int m_DivIdx;
	ARB::ARBConfigLevelPtr m_Level;
	int m_LevelIdx;
	ARB::ARBConfigEventPtr m_Event;
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
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigDivisionPtr const& inDiv,
		int inPts,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBConfigVenuePtr m_Venue;
	ARB::ARBConfigDivisionPtr m_Div;
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
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const& inVenue,
		ARB::ARBConfigMultiQPtr const& inMultiQ,
		std::set<MultiQdata> const& inMQs,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
	bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogPtr m_Dog;
	ARB::ARBConfigVenuePtr m_Venue;
	ARB::ARBConfigMultiQPtr m_MultiQ;
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
	CPointsDataLifetime(CAgilityBookDoc* pDoc, ARB::ARBConfigVenuePtr const& inVenue, CRefTag& id);
	CPointsDataLifetime(
		CAgilityBookDoc* pDoc,
		ARB::ARBConfigLifetimeNamePtr const& inLifetimeName,
		ARB::ARBConfigVenuePtr const& inVenue,
		CRefTag& id);
	virtual ~CPointsDataLifetime()
	{
	}
	virtual void AddLifetimeInfo(
		std::wstring const& inDiv,
		std::wstring const& inLevel,
		double inLifetime,
		double inFiltered);

	virtual void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
	virtual bool Details(std::wstring const& link);

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBConfigLifetimeNamePtr m_LifetimeName; ///< null implies Placement
	ARB::ARBConfigVenuePtr m_Venue;
	std::list<LifeTimePointInfoPtr> m_Data;
	double m_Lifetime; ///< Total lifetime points.
	double m_Filtered; ///< Points that are filtered out.
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
		ARB::ARBConfigVenuePtr const& inVenue,
		std::wstring const& inName,
		CRefTag& id);
	CPointsDataLifetimeByName(
		CAgilityBookDoc* pDoc,
		ARB::ARBConfigLifetimeNamePtr const& inLifetimeName,
		ARB::ARBConfigVenuePtr const& inVenue,
		std::wstring const& inName,
		CRefTag& id);

	// Adds are limited by inSort1 == m_Name
	void AddLifetimeInfo(std::wstring const& inSort1, std::wstring const& inSort2, double inLifetime, double inFiltered)
		override;

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) override;

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
		ARB::ARBDogPtr const& inDog,
		ARB::ARBConfigVenuePtr const inVenue,
		CRefTag& id);

	bool HasData() const;
	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks);
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
		LifeTimePoint(std::wstring const& inEvent, double inPoints, bool inFiltered)
			: eventName(inEvent)
			, points(inPoints)
			, bFiltered(inFiltered)
		{
		}
	};
	typedef std::list<LifeTimePoint> LifeTimePointList;
	struct LifeTimePoints
	{
		ARB::ARBConfigDivisionPtr pDiv;
		ARB::ARBConfigLevelPtr pLevel;
		std::map<ARB::ARBConfigLifetimeNamePtr, LifeTimePointList> ptLifetime;
	};
	typedef std::list<LifeTimePoints> LifeTimePointsList;
	struct PlacementPoints
	{
		ARB::ARBConfigDivisionPtr pDiv;
		ARB::ARBConfigLevelPtr pLevel;
		LifeTimePointList ptPlacement;
	};
	typedef std::list<PlacementPoints> PlacementPointsList;

	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogPtr m_pDog;
	ARB::ARBConfigVenuePtr m_pVenue;
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
		ARB::ARBConfigOtherPointsPtr pOther,
		std::list<OtherPtInfo> const& inRunList,
		CRefTag& id);
	virtual ~CPointsDataOtherPoints()
	{
	}
	virtual void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) = 0;
	virtual bool Details(std::wstring const& link) = 0;
	ARB::ARBConfigOtherPointsPtr GetOther() const
	{
		return m_pOther;
	}

protected:
	std::wstring m_refTag;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBConfigOtherPointsPtr m_pOther;
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
		ARB::ARBConfigOtherPointsPtr pOther,
		std::list<OtherPtInfo> const& inRunList,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) override;
	bool Details(std::wstring const& link) override;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
	DECLARE_NO_COPY_IMPLEMENTED(CPointsDataOtherPointsTallyAllByEvent)
public:
	CPointsDataOtherPointsTallyAllByEvent(
		CAgilityBookDoc* pDoc,
		ARB::ARBConfigOtherPointsPtr pOther,
		std::wstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) override;
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
		ARB::ARBConfigOtherPointsPtr pOther,
		std::wstring const& inLevel,
		std::list<OtherPtInfo> const& inRunList,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) override;
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
		ARB::ARBConfigOtherPointsPtr pOther,
		std::wstring const& inLevel,
		std::wstring const& inEvent,
		std::list<OtherPtInfo> const& inRunList,
		CRefTag& id);

	void GetHtml(fmt::wmemory_buffer& data, bool bNoInternalLinks) override;
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

	void LoadData(ARB::ARBDogPtr const& inDog);

	wxString GetHtml(bool bFragment, bool bNoInternalLinks);
	bool Details(std::wstring const& link);

private:
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogPtr m_pDog;
	std::vector<CPointsDataVenuePtr> m_venues;
	std::vector<CPointsDataOtherPointsPtr> m_otherPts;
};

} // namespace dconSoft
