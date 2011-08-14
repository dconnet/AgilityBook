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

#include <list>
#include <set>
#include <vector>
#include "ARBDate.h"
#include "ARBTypes.h"
#include "ListData.h"
class CAgilityBookDoc;

typedef std::pair<ARBDate, ARBDogTrialPtr> MultiQdata;
typedef std::pair<ARBDogTrialPtr, ARBDogRunPtr> RunInfo;

// Special HTML tag for href's
#define ARB_PROTOCOL	wxT("arb:")

/**
 * Used to accumulate lifetime info. Also for Placement totals.
 */
class LifeTimePointInfo;
typedef std::tr1::shared_ptr<LifeTimePointInfo> LifeTimePointInfoPtr;
class LifeTimePointInfo
{
private:
	LifeTimePointInfo(
			wxString const& inSort1,
			wxString const& inSort2,
			double inPoints,
			double inFiltered);
public:
	static LifeTimePointInfoPtr New(
			wxString const& inSort1, // Division or event
			wxString const& inSort2, // Level or empty
			double inPoints,
			double inFiltered);
	wxString sort1;
	wxString sort2;
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
			double score);
	OtherPtInfo(ARBDogExistingPointsPtr pExisting);
	~OtherPtInfo();
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRun;
	ARBDogExistingPointsPtr m_pExisting;
	wxString m_Venue;
	wxString m_Div;
	wxString m_Level;
	wxString m_Event;
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
	virtual wxString OnNeedText(int inCol) const = 0;
	/// Get html for a line
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const = 0;
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
			wxString const& inHtml);

	virtual wxString OnNeedText(int inCol) const	{return wxT("");}
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const
	{
		return m_Html;
	}
	virtual bool IsVisible() const						{return false;}
	virtual bool IsEqual(CPointsDataBasePtr /*inData*/)	{return false;}

protected:
	wxString m_Html;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataText : public CPointsDataBase
{
public:
	CPointsDataText(
			CAgilityBookDoc* pDoc,
			bool bUseInHtml,
			wxChar const* inCol1 = wxT(""),
			wxChar const* inCol2 = wxT(""));

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

private:
	bool m_UseInHtml;
	wxString m_Col1;
	wxString m_Col2;
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

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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
			wxString const& inRunCount,
			wxString const& inQcount,
			wxString const& inPts,
			wxString const& inSuperQ,
			wxString const& inSpeed);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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
	wxString m_RunCount;
	wxString m_Qcount;
	wxString m_Pts;
	wxString m_SuperQ;
	wxString m_Speed;
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
			wxString const& inDiv,
			wxString const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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
			wxString const& inName);

	// Adds are limited by inSort1 == m_Name
	void AddLifetimeInfo(
			wxString const& inSort1,
			wxString const& inSort2,
			double inLifetime,
			double inFiltered);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	wxString m_Name;
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

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
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
	double m_Score;
	std::list<OtherPtInfo> m_RunList;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPoints> CPointsDataOtherPointsPtr;

class CPointsDataOtherPointsTallyAll : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAll(
			CAgilityBookDoc* pDoc,
			wxString const& inName,
			std::list<OtherPtInfo> const& inRunList);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	wxString m_Name;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAllByEvent(
			CAgilityBookDoc* pDoc,
			wxString const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	wxString m_Event;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyAllByEvent> CPointsDataOtherPointsTallyAllByEventPtr;

class CPointsDataOtherPointsTallyLevel : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevel(
			CAgilityBookDoc* pDoc,
			wxString const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	wxString m_Level;
};
typedef std::tr1::shared_ptr<CPointsDataOtherPointsTallyLevel> CPointsDataOtherPointsTallyLevelPtr;

class CPointsDataOtherPointsTallyLevelByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookDoc* pDoc,
			wxString const& inLevel,
			wxString const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual wxString OnNeedText(int inCol) const;
	virtual wxString GetHtml(size_t nCurLine, bool bNoInternalLinks) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	wxString m_Level;
	wxString m_Event;
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
		wxString eventName;
		double points;
		bool bFiltered;
		LifeTimePoint()
			: eventName()
			, points(0.0)
			, bFiltered(false)
		{
		}
		LifeTimePoint(
				const wxString inEvent,
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
