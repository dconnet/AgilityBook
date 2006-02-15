#pragma once

/*
 * Copyright © 2004-2006 David Connet. All Rights Reserved.
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

class CAgilityBookViewPoints;

typedef std::pair<ARBDate, ARBDogTrialPtr> MultiQdata;
typedef std::pair<ARBDogTrialPtr, ARBDogRunPtr> RunInfo;

/////////////////////////////////////////////////////////////////////////////

/**
 * Used to accumulate lifetime info.
 */
class LifeTimePointInfo
{
#if _MSC_VER >= 1400
private:
	LifeTimePointInfo(); // Not implemented, needed for <list>
#endif
public:
	LifeTimePointInfo(
			ARBString const& inDiv,
			ARBString const& inLevel,
			double inPoints,
			double inFiltered);
	ARBString div;
	ARBString level;
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
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_Level;
	ARBString m_Event;
	double m_Score;
};

/////////////////////////////////////////////////////////////////////////////

class PointsDataBase : public CListData
{
public:
	PointsDataBase(CAgilityBookViewPoints* pView);
	void AddRef();
	void Release();

	virtual ARBString OnNeedText(size_t index) const = 0;
	virtual bool HasDetails() const {return false;}
	virtual void Details() const {}
	virtual bool IsEqual(PointsDataBase const* inData) = 0;

protected:
	virtual ~PointsDataBase();
	UINT m_RefCount;
	CAgilityBookViewPoints* m_pView;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the data for the "header".
 */
class PointsDataDog : public PointsDataBase
{
public:
	PointsDataDog(
			CAgilityBookViewPoints* pView,
			ARBDogPtr pDog);
	~PointsDataDog();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDogPtr m_pDog;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * This is the venue "header"
 */
class PointsDataVenue : public PointsDataBase
{
public:
	PointsDataVenue(
			CAgilityBookViewPoints* pView,
			ARBDogPtr pDog,
			ARBConfigVenuePtr pVenue);
	~PointsDataVenue();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDogPtr m_pDog;
	ARBConfigVenuePtr m_pVenue;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * This is a title item
 */
class PointsDataTitle : public PointsDataBase
{
public:
	PointsDataTitle(
			CAgilityBookViewPoints* pView,
			ARBDogPtr pDog,
			ARBDogTitlePtr pTitle);
	~PointsDataTitle();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDogPtr m_pDog;
	ARBDogTitlePtr m_pTitle;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all runs summarized in an event line.
 */
class PointsDataEvent : public PointsDataBase
{
public:
	PointsDataEvent(
			CAgilityBookViewPoints* pView,
			ARBDogPtr inDog,
			std::list<RunInfo>& inMatching,
			ARBConfigVenuePtr inVenue,
			ARBConfigDivisionPtr inDiv,
			ARBConfigLevelPtr inLevel,
			ARBConfigEventPtr inEvent,
			ARBString const& inRunCount,
			ARBString const& inQcount,
			ARBString const& inPts,
			ARBString const& inSuperQ,
			ARBString const& inSpeed);
	~PointsDataEvent();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDogPtr m_Dog;
	std::list<RunInfo> m_Matching;
	ARBConfigVenuePtr m_Venue;
	ARBConfigDivisionPtr m_Div;
	ARBConfigLevelPtr m_Level;
	ARBConfigEventPtr m_Event;
	ARBString m_RunCount;
	ARBString m_Qcount;
	ARBString m_Pts;
	ARBString m_SuperQ;
	ARBString m_Speed;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all lifetime points
 */
class PointsDataLifetime : public PointsDataBase
{
public:
	PointsDataLifetime(
			CAgilityBookViewPoints* pView,
			ARBString const& inVenue);
	void AddLifetimeInfo(
			ARBString const& inDiv,
			ARBString const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	CString m_Venue;
	std::list<LifeTimePointInfo> m_Data;
	double m_Lifetime; //< Total lifetime points.
	double m_Filtered; //< Points that are filtered out.
};

/**
 * Subtotal lifetime points by division.
 */
class PointsDataLifetimeDiv : public PointsDataLifetime
{
public:
	PointsDataLifetimeDiv(
			CAgilityBookViewPoints* pView,
			ARBString const& inVenue,
			ARBString const& inDiv);

	void AddLifetimeInfo(
			ARBString const& inDiv,
			ARBString const& inLevel,
			double inLifetime,
			double inFiltered);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBString m_Div;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs
 */
class PointsDataMultiQs : public PointsDataBase
{
public:
	PointsDataMultiQs(
			CAgilityBookViewPoints* pView,
			ARBDogPtr inDog,
			ARBConfigVenuePtr inVenue,
			ARBConfigMultiQPtr inMultiQ,
			std::set<MultiQdata> const& inMQs);
	~PointsDataMultiQs();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDogPtr m_Dog;
	ARBConfigVenuePtr m_Venue;
	ARBConfigMultiQPtr m_MultiQ;
	std::set<MultiQdata> m_MQs;
	double m_ExistingDblQs;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points
 */
class PointsDataSpeedPts : public PointsDataBase
{
public:
	PointsDataSpeedPts(
			CAgilityBookViewPoints* pView,
			ARBConfigVenuePtr inVenue,
			int inPts);
	~PointsDataSpeedPts();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBConfigVenuePtr m_Venue;
	int m_Pts;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Other points data base class.
 */
class PointsDataOtherPoints : public PointsDataBase
{
public:
	PointsDataOtherPoints(
			CAgilityBookViewPoints* pView,
			std::list<OtherPtInfo> const& inRunList);

protected:
	double m_Score;
	std::list<OtherPtInfo> m_RunList;
};

class PointsDataOtherPointsTallyAll : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAll(
			CAgilityBookViewPoints* pView,
			ARBString const& inName,
			std::list<OtherPtInfo> const& inRunList);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBString m_Name;
};

class PointsDataOtherPointsTallyAllByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAllByEvent(
			CAgilityBookViewPoints* pView,
			ARBString const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBString m_Event;
};

class PointsDataOtherPointsTallyLevel : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevel(
			CAgilityBookViewPoints* pView,
			ARBString const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBString m_Level;
};

class PointsDataOtherPointsTallyLevelByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookViewPoints* pView,
			ARBString const& inLevel,
			ARBString const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBString m_Level;
	ARBString m_Event;
};
