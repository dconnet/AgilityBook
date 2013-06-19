#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
 * @li 2011-08-13 DRC Don't copy internal url links to the clipboard.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-01-26 DRC Ported to wxWidgets.
 * @li 2006-07-15 DRC Add option to sort by event instead of division.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-10-18 DRC Remember last selected item when reloading data.
 * @li 2005-10-14 DRC Added a context menu.
 * @li 2005-05-04 DRC Added subtotaling by division to lifetime points.
 * @li 2005-03-14 DRC Show a summary of lifetime points in the list viewer.
 * @li 2005-01-02 DRC Show existing points in the list viewer.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-03 DRC Show all lifetime points when filtering.
 * @li 2004-08-06 DRC Created
 */

#include "ListData.h"
#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBDate.h"
#include <list>
#include <set>
#include <vector>
class CAgilityBookDoc;

typedef std::pair<ARBDate, ARBDogTrialPtr> MultiQdata;

// See SetupARBPost.h
#if _MSC_VER < 1700 && (!defined(_HAS_TR1) || !_HAS_TR1)
#include <boost/tr1/tuple.hpp>
using std::tr1::tuple;
using std::tr1::get;
#else
#include <tuple>
using std::tuple;
using std::get;
#endif
#define TUPLE_DOG	0
#define TUPLE_TRIAL	1
#define TUPLE_RUN	2
typedef tuple<ARBDogPtr, ARBDogTrialPtr, ARBDogRunPtr> RunInfo;

// Special HTML tag for href's
#define ARB_PROTOCOL	L"arb:"

/**
 * Used to accumulate lifetime info. Also for Placement totals.
 */
class LifeTimePointInfo;
typedef std::tr1::shared_ptr<LifeTimePointInfo> LifeTimePointInfoPtr;
class LifeTimePointInfo
{
protected:
	LifeTimePointInfo(
			std::wstring const& inSort1,
			std::wstring const& inSort2,
			double inPoints,
			double inFiltered);
public:
	static LifeTimePointInfoPtr New(
			std::wstring const& inSort1, // Division or event
			std::wstring const& inSort2, // Level or empty
			double inPoints,
			double inFiltered);
	std::wstring sort1;
	std::wstring sort2;
	double points;
	double filtered;
};

/**
 * Used to accumulate run info into a flat list.
 */
class OtherPtInfo
{
#if _MSC_VER >= 1400
private:
	OtherPtInfo(); // Not implemented, needed for <list>
#endif
public:
	OtherPtInfo(
			ARBDogTrialPtr pTrial,
			ARBDogRunPtr pRun,
			double points,
			bool bScore, // m_Score is valid
			double score);
	OtherPtInfo(ARBDogExistingPointsPtr pExisting);
	~OtherPtInfo();
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

class CPointsDataBase;
typedef std::tr1::shared_ptr<CPointsDataBase> CPointsDataBasePtr;
class CPointsDataBase
{
public:
	CPointsDataBase(CAgilityBookDoc* pDoc);
	virtual ~CPointsDataBase();

	/// Get a particular column of text (used for listctrl)
	virtual std::wstring OnNeedText(int inCol) const = 0;
	/// Get html for a line
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const = 0;
	/// Is this entry visible? (used for special html processing)
	virtual bool IsVisible() const	{return true;}
	/// This entry has details (dbl-click works)
	virtual bool HasDetails() const {return false;}
	virtual void Details() const	{}
	/// Equality testing (to locate previous entries)
	virtual bool IsEqual(CPointsDataBasePtr inData) = 0;

protected:
	CAgilityBookDoc* m_pDoc;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataSeparator : public CPointsDataBase
{
public:
	CPointsDataSeparator(
			CAgilityBookDoc* pDoc,
			std::wstring const& inHtml);

	virtual std::wstring OnNeedText(int inCol) const	{return std::wstring();}
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const
	{
		return m_Html;
	}
	virtual bool IsVisible() const						{return false;}
	virtual bool IsEqual(CPointsDataBasePtr /*inData*/)	{return false;}

protected:
	std::wstring m_Html;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataText : public CPointsDataBase
{
public:
	CPointsDataText(
			CAgilityBookDoc* pDoc,
			bool bUseInHtml,
			wchar_t const* inCol1 = L"",
			wchar_t const* inCol2 = L"");

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

private:
	bool m_UseInHtml;
	std::wstring m_Col1;
	std::wstring m_Col2;
};
typedef std::tr1::shared_ptr<CPointsDataText> CPointsDataTextPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the data for the "header".
 */
class CPointsDataDog : public CPointsDataBase
{
public:
	CPointsDataDog(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
};
typedef std::tr1::shared_ptr<CPointsDataDog> CPointsDataDogPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the venue "header"
 */
class CPointsDataVenue : public CPointsDataBase
{
public:
	CPointsDataVenue(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
	ARBConfigVenuePtr m_pVenue;
};
typedef std::tr1::shared_ptr<CPointsDataVenue> CPointsDataVenuePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is a title item
 */
class CPointsDataTitle : public CPointsDataBase
{
public:
	CPointsDataTitle(
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			ARBDogTitlePtr pTitle);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
	ARBDogTitlePtr m_pTitle;
};
typedef std::tr1::shared_ptr<CPointsDataTitle> CPointsDataTitlePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all runs summarized in an event line.
 */
class CPointsDataEvent : public CPointsDataBase
{
	friend class SortPointItems;
public:
	CPointsDataEvent(
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog,
			std::list<RunInfo>& inMatching,
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			int inDivIdx,
			ARBConfigLevelPtr inLevel,
			int inLevelIdx,
			ARBConfigEventPtr inEvent,
			int inEventIdx,
			std::wstring const& inRunCount,
			std::wstring const& inQcount,
			std::wstring const& inPts,
			std::wstring const& inSuperQ,
			std::wstring const& inSpeed);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
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
typedef std::tr1::shared_ptr<CPointsDataEvent> CPointsDataEventPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all lifetime points
 */
class CPointsDataLifetime : public CPointsDataBase
{
public:
	/**
	 * @param pDoc Document
	 * @param bLifetime Lifetime or Placement?
	 * @param inVenue Associated venue
	 */
	CPointsDataLifetime(
			CAgilityBookDoc* pDoc,
			bool bLifetime,
			ARBConfigVenuePtr inVenue);
	void AddLifetimeInfo(
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	bool m_bLifetime;	///< Lifetime or Placement?
	ARBConfigVenuePtr m_Venue;
	std::list<LifeTimePointInfoPtr> m_Data;
	double m_Lifetime;	///< Total lifetime points.
	double m_Filtered;	///< Points that are filtered out.
};
typedef std::tr1::shared_ptr<CPointsDataLifetime> CPointsDataLifetimePtr;

/**
 * Subtotal lifetime points by division.
 */
class CPointsDataLifetimeByName : public CPointsDataLifetime
{
public:
	CPointsDataLifetimeByName(
			CAgilityBookDoc* pDoc,
			bool bLifetime,
			ARBConfigVenuePtr inVenue,
			std::wstring const& inName);

	// Adds are limited by inSort1 == m_Name
	void AddLifetimeInfo(
			std::wstring const& inSort1,
			std::wstring const& inSort2,
			double inLifetime,
			double inFiltered);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	std::wstring m_Name;
};
typedef std::tr1::shared_ptr<CPointsDataLifetimeByName> CPointsDataLifetimeByNamePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs
 */
class CPointsDataMultiQs : public CPointsDataBase
{
public:
	CPointsDataMultiQs(
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			std::set<MultiQdata> const& inMQs);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
	std::set<MultiQdata> m_MQs;
	double m_ExistingDblQs;
};
typedef std::tr1::shared_ptr<CPointsDataMultiQs> CPointsDataMultiQsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points
 */
class CPointsDataSpeedPts : public CPointsDataBase
{
public:
	CPointsDataSpeedPts(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			int inPts);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	int m_Pts;
};
typedef std::tr1::shared_ptr<CPointsDataSpeedPts> CPointsDataSpeedPtsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Other points data base class.
 */
class CPointsDataOtherPoints : public CPointsDataBase
{
public:
	CPointsDataOtherPoints(
			CAgilityBookDoc* pDoc,
			std::list<OtherPtInfo> const& inRunList);

protected:
	double m_Points;
	std::list<OtherPtInfo> m_RunList;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPoints> CPointsDataOtherPointsPtr;

class CPointsDataOtherPointsTallyAll : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAll(
			CAgilityBookDoc* pDoc,
			std::wstring const& inName,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	std::wstring m_Name;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAllByEvent(
			CAgilityBookDoc* pDoc,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	std::wstring m_Event;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyAllByEvent> CPointsDataOtherPointsTallyAllByEventPtr;

class CPointsDataOtherPointsTallyLevel : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevel(
			CAgilityBookDoc* pDoc,
			std::wstring const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	std::wstring m_Level;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyLevel> CPointsDataOtherPointsTallyLevelPtr;

class CPointsDataOtherPointsTallyLevelByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookDoc* pDoc,
			std::wstring const& inLevel,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::wstring OnNeedText(int inCol) const;
	virtual std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	std::wstring m_Level;
	std::wstring m_Event;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyLevelByEvent> CPointsDataOtherPointsTallyLevelByEventPtr;

/////////////////////////////////////////////////////////////////////////////

class CPointsDataItems
{
public:
	CPointsDataItems();
	~CPointsDataItems();

	void LoadData(
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog);
	void clear();

	size_t NumLines() const;
	CPointsDataBasePtr GetLine(size_t nLine) const;

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
				const std::wstring inEvent,
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
		LifeTimePointList ptLifetime;
		LifeTimePointList ptPlacement;
	};
	typedef std::list<LifeTimePoints> LifeTimePointsList;

	void InsertVenueHeader(
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog,
			ARBConfigVenuePtr pVenue);

	std::vector<CPointsDataBasePtr> m_Lines;
};
