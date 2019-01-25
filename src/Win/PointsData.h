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
class LifeTimePointInfo;
typedef std::shared_ptr<LifeTimePointInfo> LifeTimePointInfoPtr;
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
	OtherPtInfo() = delete; // Not implemented, needed for <list>
#endif
public:
	OtherPtInfo(
			ARBDogTrialPtr const& inTrial,
			ARBDogRunPtr const& inRun,
			double points,
			bool bScore, // m_Score is valid
			double score);
	OtherPtInfo(ARBDogExistingPointsPtr const& inExisting);
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
typedef std::shared_ptr<CPointsDataBase> CPointsDataBasePtr;
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
	virtual bool IsEqual(CPointsDataBasePtr const& inData) = 0;

protected:
	CAgilityBookDoc* m_pDoc;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Adds raw html into the HTML view.
 */
class CPointsDataSeparator : public CPointsDataBase
{
public:
	CPointsDataSeparator(
			CAgilityBookDoc* pDoc,
			std::wstring const& inHtml);

	std::wstring OnNeedText(int inCol) const override {return std::wstring();}
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override
	{
		return m_Html;
	}
	bool IsVisible() const override {return false;}
	bool IsEqual(CPointsDataBasePtr const& /*inData*/) override {return false;}

protected:
	std::wstring m_Html;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Inserts a line
 * - List view:
 *   - Blank line
 *   - Other points header (in html, CPointsDataSeparator is used)
 * - Both:
 *   - Name of Other Points data
 */
class CPointsDataText : public CPointsDataBase
{
private:
	CPointsDataText(
			CAgilityBookDoc* pDoc,
			bool bUseInHtml,
			wchar_t const* inCol1 = L"",
			wchar_t const* inCol2 = L"");
public:
	static CPointsDataBasePtr CreateListBlankLine(CAgilityBookDoc* pDoc);
	static CPointsDataBasePtr CreateListOtherPoints(CAgilityBookDoc* pDoc);
	static CPointsDataBasePtr CreateDataLine(
			CAgilityBookDoc* pDoc,
			std::wstring const& str);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

private:
	bool m_UseInHtml;
	std::wstring m_Col1;
	std::wstring m_Col2;
};
typedef std::shared_ptr<CPointsDataText> CPointsDataTextPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the data for the "header".
 */
class CPointsDataHeader : public CPointsDataBase
{
public:
	CPointsDataHeader(CAgilityBookDoc* pDoc);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBDate m_today;
};
typedef std::shared_ptr<CPointsDataHeader> CPointsDataHeaderPtr;

/**
 * This is the data for the dog.
 */
class CPointsDataDog : public CPointsDataBase
{
public:
	CPointsDataDog(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBDogPtr m_pDog;
};
typedef std::shared_ptr<CPointsDataDog> CPointsDataDogPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the venue "header"
 */
class CPointsDataVenue : public CPointsDataBase
{
public:
	CPointsDataVenue(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBConfigVenuePtr const& inVenue);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBDogPtr m_pDog;
	ARBConfigVenuePtr m_pVenue;
};
typedef std::shared_ptr<CPointsDataVenue> CPointsDataVenuePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is a title item
 */
class CPointsDataTitle : public CPointsDataBase
{
public:
	CPointsDataTitle(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBDogTitlePtr const& inTitle);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBDogPtr m_pDog;
	ARBDogTitlePtr m_pTitle;
};
typedef std::shared_ptr<CPointsDataTitle> CPointsDataTitlePtr;

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
			std::wstring const& inSpeed);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

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
typedef std::shared_ptr<CPointsDataEvent> CPointsDataEventPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all lifetime points
 */
class CPointsDataLifetime : public CPointsDataBase
{
public:
	CPointsDataLifetime(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue);
	CPointsDataLifetime(
			CAgilityBookDoc* pDoc,
			ARBConfigLifetimeNamePtr const& inLifetimeName,
			ARBConfigVenuePtr const& inVenue);
	void AddLifetimeInfo(
			std::wstring const& inDiv,
			std::wstring const& inLevel,
			double inLifetime,
			double inFiltered);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
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
public:
	/**
	* @param pDoc Document
	* @param inVenue Associated venue
	* @param inName Division name
	*/
	CPointsDataLifetimeByName(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue,
			std::wstring const& inName);
	CPointsDataLifetimeByName(
			CAgilityBookDoc* pDoc,
			ARBConfigLifetimeNamePtr const& inLifetimeName,
			ARBConfigVenuePtr const& inVenue,
			std::wstring const& inName);

	// Adds are limited by inSort1 == m_Name
	void AddLifetimeInfo(
			std::wstring const& inSort1,
			std::wstring const& inSort2,
			double inLifetime,
			double inFiltered);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	std::wstring m_Name;
};
typedef std::shared_ptr<CPointsDataLifetimeByName> CPointsDataLifetimeByNamePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs
 */
class CPointsDataMultiQs : public CPointsDataBase
{
public:
	CPointsDataMultiQs(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBConfigVenuePtr const& inVenue,
			ARBConfigMultiQPtr const& inMultiQ,
			std::set<MultiQdata> const& inMQs);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
	std::set<MultiQdata> m_MQs;
	double m_ExistingDblQs;
};
typedef std::shared_ptr<CPointsDataMultiQs> CPointsDataMultiQsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points
 */
class CPointsDataSpeedPts : public CPointsDataBase
{
public:
	CPointsDataSpeedPts(
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr const& inVenue,
			ARBConfigDivisionPtr const& inDiv,
			int inPts);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	int m_Pts;
};
typedef std::shared_ptr<CPointsDataSpeedPts> CPointsDataSpeedPtsPtr;

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
typedef std::shared_ptr<CPointsDataOtherPoints> CPointsDataOtherPointsPtr;

class CPointsDataOtherPointsTallyAll : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAll(
			CAgilityBookDoc* pDoc,
			std::wstring const& inName,
			std::list<OtherPtInfo> const& inRunList);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	std::wstring m_Name;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAllByEvent(
			CAgilityBookDoc* pDoc,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	std::wstring m_Event;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyAllByEvent> CPointsDataOtherPointsTallyAllByEventPtr;

class CPointsDataOtherPointsTallyLevel : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevel(
			CAgilityBookDoc* pDoc,
			std::wstring const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	std::wstring m_Level;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyLevel> CPointsDataOtherPointsTallyLevelPtr;

class CPointsDataOtherPointsTallyLevelByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookDoc* pDoc,
			std::wstring const& inLevel,
			std::wstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	std::wstring OnNeedText(int inCol) const override;
	std::wstring GetHtml(size_t nCurLine, bool bNoInternalLinks) const override;
	bool HasDetails() const override {return true;}
	void Details() const override;
	bool IsEqual(CPointsDataBasePtr const& inData) override;

protected:
	std::wstring m_Level;
	std::wstring m_Event;
};
typedef std::shared_ptr<CPointsDataOtherPointsTallyLevelByEvent> CPointsDataOtherPointsTallyLevelByEventPtr;

/////////////////////////////////////////////////////////////////////////////

class CPointsDataItems
{
public:
	CPointsDataItems();
	~CPointsDataItems();

	void LoadData(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog);
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

	void InsertVenueHeader(
			CAgilityBookDoc* pDoc,
			ARBDogPtr const& inDog,
			ARBConfigVenuePtr const& inVenue);

	std::vector<CPointsDataBasePtr> m_Lines;
};
