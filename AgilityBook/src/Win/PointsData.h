#pragma once

/*
 * Copyright © 2004-2008 David Connet. All Rights Reserved.
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
 * @brief Data classes behind the points view items.
 * @author David Connet
 *
 * Revision History
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
#define ARB_PROTOCOL	_T("arb:")

/**
 * Used to accumulate lifetime info. Also for Placement totals.
 */
class LifeTimePointInfo
{
#if _MSC_VER >= 1400
private:
	LifeTimePointInfo(); // Not implemented, needed for <list>
#endif
public:
	LifeTimePointInfo(
			tstring const& inDiv,
			tstring const& inLevel,
			double inPoints,
			double inFiltered);
	tstring div;
	tstring level;
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
	tstring m_Venue;
	tstring m_Div;
	tstring m_Level;
	tstring m_Event;
	double m_Score;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataBase;
typedef boost::shared_ptr<CPointsDataBase> CPointsDataBasePtr;
class CPointsDataBase
{
public:
	CPointsDataBase(
			CWnd* pParent,
			CAgilityBookDoc* pDoc);
	virtual ~CPointsDataBase();

	/// Get a particular column of text (used for listctrl)
	virtual tstring OnNeedText(size_t inCol) const = 0;
	/// Get html for a line
	virtual tstring GetHtml(size_t nCurLine) const = 0;
	/// Is this entry visible? (used for special html processing)
	virtual bool IsVisible() const	{return true;}
	/// This entry has details (dbl-click works)
	virtual bool HasDetails() const {return false;}
	virtual void Details() const	{}
	/// Equality testing (to locate previous entries)
	virtual bool IsEqual(CPointsDataBasePtr inData) = 0;

protected:
	CWnd* m_pParent;
	CAgilityBookDoc* m_pDoc;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataSeparator : public CPointsDataBase
{
public:
	CPointsDataSeparator(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			tstring const& inHtml);

	virtual tstring OnNeedText(size_t inCol) const	{return _T("");}
	virtual tstring GetHtml(size_t nCurLine) const	{return m_Html;}
	virtual bool IsVisible() const						{return false;}
	virtual bool IsEqual(CPointsDataBasePtr /*inData*/)	{return false;}

protected:
	tstring m_Html;
};

/////////////////////////////////////////////////////////////////////////////

class CPointsDataText : public CPointsDataBase
{
public:
	CPointsDataText(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			bool bUseInHtml,
			LPCTSTR inCol1 = _T(""),
			LPCTSTR inCol2 = _T(""));

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

private:
	bool m_UseInHtml;
	tstring m_Col1;
	tstring m_Col2;
};
typedef boost::shared_ptr<CPointsDataText> CPointsDataTextPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the data for the "header".
 */
class CPointsDataDog : public CPointsDataBase
{
public:
	CPointsDataDog(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
};
typedef boost::shared_ptr<CPointsDataDog> CPointsDataDogPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the venue "header"
 */
class CPointsDataVenue : public CPointsDataBase
{
public:
	CPointsDataVenue(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
	ARBConfigVenuePtr m_pVenue;
};
typedef boost::shared_ptr<CPointsDataVenue> CPointsDataVenuePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * This is a title item
 */
class CPointsDataTitle : public CPointsDataBase
{
public:
	CPointsDataTitle(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr pDog,
			ARBDogTitlePtr pTitle);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBDogPtr m_pDog;
	ARBDogTitlePtr m_pTitle;
};
typedef boost::shared_ptr<CPointsDataTitle> CPointsDataTitlePtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all runs summarized in an event line.
 */
class CPointsDataEvent : public CPointsDataBase
{
	friend class SortPointItems;
public:
	CPointsDataEvent(
			CWnd* pParent,
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
			tstring const& inRunCount,
			tstring const& inQcount,
			tstring const& inPts,
			tstring const& inSuperQ,
			tstring const& inSpeed);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
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
	tstring m_RunCount;
	tstring m_Qcount;
	tstring m_Pts;
	tstring m_SuperQ;
	tstring m_Speed;
};
typedef boost::shared_ptr<CPointsDataEvent> CPointsDataEventPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all lifetime points
 */
class CPointsDataLifetime : public CPointsDataBase
{
public:
	CPointsDataLifetime(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			bool bLifetime, ///< Lifetime or Placement?
			ARBConfigVenuePtr inVenue);
	void AddLifetimeInfo(
			tstring const& inDiv,
			tstring const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	bool m_bLifetime;	///< Lifetime or Placement?
	ARBConfigVenuePtr m_Venue;
	std::list<LifeTimePointInfo> m_Data;
	double m_Lifetime;	//< Total lifetime points.
	double m_Filtered;	//< Points that are filtered out.
};
typedef boost::shared_ptr<CPointsDataLifetime> CPointsDataLifetimePtr;

/**
 * Subtotal lifetime points by division.
 */
class CPointsDataLifetimeDiv : public CPointsDataLifetime
{
public:
	CPointsDataLifetimeDiv(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			bool bLifetime,
			ARBConfigVenuePtr inVenue,
			tstring const& inDiv);

	void AddLifetimeInfo(
			tstring const& inDiv,
			tstring const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	tstring m_Div;
};
typedef boost::shared_ptr<CPointsDataLifetimeDiv> CPointsDataLifetimeDivPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs
 */
class CPointsDataMultiQs : public CPointsDataBase
{
public:
	CPointsDataMultiQs(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			std::set<MultiQdata> const& inMQs);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
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
typedef boost::shared_ptr<CPointsDataMultiQs> CPointsDataMultiQsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points
 */
class CPointsDataSpeedPts : public CPointsDataBase
{
public:
	CPointsDataSpeedPts(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBConfigVenuePtr inVenue,
			int inPts);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	ARBConfigVenuePtr m_Venue;
	int m_Pts;
};
typedef boost::shared_ptr<CPointsDataSpeedPts> CPointsDataSpeedPtsPtr;

/////////////////////////////////////////////////////////////////////////////

/**
 * Other points data base class.
 */
class CPointsDataOtherPoints : public CPointsDataBase
{
public:
	CPointsDataOtherPoints(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			std::list<OtherPtInfo> const& inRunList);

protected:
	double m_Score;
	std::list<OtherPtInfo> m_RunList;
};
typedef boost::shared_ptr<CPointsDataOtherPoints> CPointsDataOtherPointsPtr;

class CPointsDataOtherPointsTallyAll : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAll(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			tstring const& inName,
			std::list<OtherPtInfo> const& inRunList);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	tstring m_Name;
};
typedef boost::shared_ptr<CPointsDataOtherPointsTallyAll> CPointsDataOtherPointsTallyAllPtr;

class CPointsDataOtherPointsTallyAllByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyAllByEvent(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			tstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	tstring m_Event;
};
typedef boost::shared_ptr<CPointsDataOtherPointsTallyAllByEvent> CPointsDataOtherPointsTallyAllByEventPtr;

class CPointsDataOtherPointsTallyLevel : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevel(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			tstring const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	tstring m_Level;
};
typedef boost::shared_ptr<CPointsDataOtherPointsTallyLevel> CPointsDataOtherPointsTallyLevelPtr;

class CPointsDataOtherPointsTallyLevelByEvent : public CPointsDataOtherPoints
{
public:
	CPointsDataOtherPointsTallyLevelByEvent(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			tstring const& inLevel,
			tstring const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual tstring OnNeedText(size_t inCol) const;
	virtual tstring GetHtml(size_t nCurLine) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(CPointsDataBasePtr inData);

protected:
	tstring m_Level;
	tstring m_Event;
};
typedef boost::shared_ptr<CPointsDataOtherPointsTallyLevelByEvent> CPointsDataOtherPointsTallyLevelByEventPtr;

/////////////////////////////////////////////////////////////////////////////

class CPointsDataItems
{
public:
	CPointsDataItems();
	~CPointsDataItems();

	void LoadData(
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog);
	void clear();

	size_t NumLines() const;
	CPointsDataBasePtr GetLine(size_t nLine) const;

private:
	struct LifeTimePoint
	{
		tstring eventName;
		double points;
		bool bFiltered;
		LifeTimePoint()
			: eventName()
			, points(0.0)
			, bFiltered(false)
		{
		}
		LifeTimePoint(
				const tstring inEvent,
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
			CWnd* pParent,
			CAgilityBookDoc* pDoc,
			ARBDogPtr inDog,
			ARBConfigVenuePtr pVenue);

	std::vector<CPointsDataBasePtr> m_Lines;
};
