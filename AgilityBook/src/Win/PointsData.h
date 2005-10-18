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
#include <string>
#include <vector>
#include "ARBDate.h"
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
			std::string const& inDiv,
			std::string const& inLevel,
			int inPoints,
			int inFiltered);
	std::string div;
	std::string level;
	int points;
	int filtered;
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
			int score);
	OtherPtInfo(ARBDogExistingPoints const* pExisting);
	ARBDogTrial const* m_pTrial;
	ARBDogRun const* m_pRun;
	ARBDogExistingPoints const* m_pExisting;
	std::string m_Venue;
	std::string m_Div;
	std::string m_Level;
	std::string m_Event;
	int m_Score;
};

/////////////////////////////////////////////////////////////////////////////

class PointsDataBase
{
public:
	PointsDataBase(CAgilityBookViewPoints* pView);
	void AddRef();
	void Release();

	virtual std::string OnNeedText(size_t index) const = 0;
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

	virtual std::string OnNeedText(size_t index) const;
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

	virtual std::string OnNeedText(size_t index) const;
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

	virtual std::string OnNeedText(size_t index) const;
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
			ARBConfigVenue const* inVenue,
			ARBConfigDivision const* inDiv,
			ARBConfigLevel const* inLevel,
			ARBConfigEvent const* inEvent,
			std::string const& inRunCount,
			std::string const& inQcount,
			std::string const& inPts,
			std::string const& inSuperQ,
			std::string const& inSpeed);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog const* m_Dog;
	std::list<RunInfo> m_Matching;
	ARBConfigVenue const* m_Venue;
	ARBConfigDivision const* m_Div;
	ARBConfigLevel const* m_Level;
	ARBConfigEvent const* m_Event;
	std::string m_RunCount;
	std::string m_Qcount;
	std::string m_Pts;
	std::string m_SuperQ;
	std::string m_Speed;
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
			std::string const& inVenue);
	void AddLifetimeInfo(
			std::string const& inDiv,
			std::string const& inLevel,
			int inLifetime,
			int inFiltered);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	CString m_Venue;
	std::list<LifeTimePointInfo> m_Data;
	int m_Lifetime; //< Total lifetime points.
	int m_Filtered; //< Points that are filtered out.
};

/**
 * Subtotal lifetime points by division.
 */
class PointsDataLifetimeDiv : public PointsDataLifetime
{
public:
	PointsDataLifetimeDiv(
			CAgilityBookViewPoints* pView,
			std::string const& inVenue,
			std::string const& inDiv);

	void AddLifetimeInfo(
			std::string const& inDiv,
			std::string const& inLevel,
			int inLifetime,
			int inFiltered);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	std::string m_Div;
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
			ARBConfigMultiQ const* inMultiQ,
			std::set<MultiQdata> const& inMQs);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	ARBDog* m_Dog;
	ARBConfigVenue* m_Venue;
	ARBConfigMultiQ const* m_MultiQ;
	std::set<MultiQdata> m_MQs;
	int m_ExistingDblQs;
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

	virtual std::string OnNeedText(size_t index) const;
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
	int m_Score;
	std::list<OtherPtInfo> m_RunList;
};

class PointsDataOtherPointsTallyAll : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAll(
			CAgilityBookViewPoints* pView,
			std::string const& inName,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	std::string m_Name;
};

class PointsDataOtherPointsTallyAllByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAllByEvent(
			CAgilityBookViewPoints* pView,
			std::string const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	std::string m_Event;
};

class PointsDataOtherPointsTallyLevel : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevel(
			CAgilityBookViewPoints* pView,
			std::string const& inLevel,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	std::string m_Level;
};

class PointsDataOtherPointsTallyLevelByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevelByEvent(
			CAgilityBookViewPoints* pView,
			std::string const& inLevel,
			std::string const& inEvent,
			std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual bool HasDetails() const {return true;}
	virtual void Details() const;
	virtual bool IsEqual(PointsDataBase const* inData);

protected:
	std::string m_Level;
	std::string m_Event;
};
