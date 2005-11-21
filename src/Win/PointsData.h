#pragma once

/*
 * Copyright © 2004-2005 David Connet. All Rights Reserved.
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
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigLevel;
class ARBConfigMultiQ;
class ARBConfigVenue;
class ARBDog;
class ARBDogExistingPoints;
class ARBDogRun;
class ARBDogTitle;
class ARBDogTrial;
class CAgilityBookViewPoints;

typedef std::pair<ARBDate, ARBDogTrial const*> MultiQdata;
typedef std::pair<ARBDogTrial const*, ARBDogRun const*> RunInfo;

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
			ARBDogTrial const* pTrial,
			ARBDogRun const* pRun,
			double score);
	OtherPtInfo(ARBDogExistingPoints const* pExisting);
	~OtherPtInfo();
	ARBDogTrial const* m_pTrial;
	ARBDogRun const* m_pRun;
	ARBDogExistingPoints const* m_pExisting;
	ARBString m_Venue;
	ARBString m_Div;
	ARBString m_Level;
	ARBString m_Event;
	double m_Score;
};

/////////////////////////////////////////////////////////////////////////////

class PointsDataBase
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
			ARBDog* pDog);
	~PointsDataDog();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog* m_pDog;
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
			ARBDog* pDog,
			ARBConfigVenue* pVenue);
	~PointsDataVenue();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog* m_pDog;
	ARBConfigVenue* m_pVenue;
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
			ARBDog* pDog,
			ARBDogTitle* pTitle);
	~PointsDataTitle();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog* m_pDog;
	ARBDogTitle* m_pTitle;
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
			ARBDog const* inDog,
			std::list<RunInfo>& inMatching,
			ARBConfigVenue* inVenue,
			ARBConfigDivision* inDiv,
			ARBConfigLevel* inLevel,
			ARBConfigEvent* inEvent,
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
	ARBDog const* m_Dog;
	std::list<RunInfo> m_Matching;
	ARBConfigVenue* m_Venue;
	ARBConfigDivision* m_Div;
	ARBConfigLevel* m_Level;
	ARBConfigEvent* m_Event;
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
			ARBDog* inDog,
			ARBConfigVenue* inVenue,
			ARBConfigMultiQ* inMultiQ,
			std::set<MultiQdata> const& inMQs);
	~PointsDataMultiQs();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog* m_Dog;
	ARBConfigVenue* m_Venue;
	ARBConfigMultiQ* m_MultiQ;
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
			ARBConfigVenue* inVenue,
			int inPts);
	~PointsDataSpeedPts();

	virtual ARBString OnNeedText(size_t index) const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBConfigVenue* m_Venue;
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
