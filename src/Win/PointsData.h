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
class ARBConfigVenue;
class ARBDog;
class ARBDogExistingPoints;
class ARBDogRun;
class ARBDogTitle;
class ARBDogTrial;
class CAgilityBookViewPoints;

typedef std::pair<ARBDate, ARBDogTrial const*> DoubleQdata;
typedef std::pair<ARBDogTrial const*, ARBDogRun const*> RunInfo;

/////////////////////////////////////////////////////////////////////////////

/**
 * Used to accumulate run info into a flat list.
 */
class OtherPtInfo
{
public:
	OtherPtInfo(ARBDogTrial const* pTrial, ARBDogRun const* pRun, int score);
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
	virtual void OnDblClick() const {}

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
	PointsDataDog(CAgilityBookViewPoints* pView, ARBDog* pDog);
	~PointsDataDog();

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;

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
	PointsDataVenue(CAgilityBookViewPoints* pView, ARBDog* pDog, ARBConfigVenue* pVenue);
	~PointsDataVenue();

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;

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
	PointsDataTitle(CAgilityBookViewPoints* pView, ARBDog* pDog, ARBDogTitle* pTitle);
	~PointsDataTitle();

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;

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
	PointsDataEvent(CAgilityBookViewPoints* pView,
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
	virtual void OnDblClick() const;

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
	PointsDataLifetime(CAgilityBookViewPoints* pView, int inLifetime, int inFiltered);

	virtual std::string OnNeedText(size_t index) const;

protected:
	int m_Lifetime; //< Total lifetime points.
	int m_Filtered; //< Points that are filtered out.
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all double Qs
 */
class PointsDataDoubleQs : public PointsDataBase
{
public:
	PointsDataDoubleQs(CAgilityBookViewPoints* pView, int inExistingDblQs, std::set<DoubleQdata> const& inQQs);

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;

protected:
	std::set<DoubleQdata> m_QQs;
	int m_ExistingDblQs;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Keeps track of all Speed points
 */
class PointsDataSpeedPts : public PointsDataBase
{
public:
	PointsDataSpeedPts(CAgilityBookViewPoints* pView, int inPts);

	virtual std::string OnNeedText(size_t index) const;

protected:
	int m_Pts;
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Other points data.
 */
class PointsDataOtherPoints : public PointsDataBase
{
public:
	PointsDataOtherPoints(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList);

protected:
	int m_Score;
	std::list<OtherPtInfo> m_RunList;
};

class PointsDataOtherPointsTallyAll : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAll(CAgilityBookViewPoints* pView, std::string const& inName, std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;

protected:
	std::string m_Name;
};

class PointsDataOtherPointsTallyAllByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyAllByEvent(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;
};

class PointsDataOtherPointsTallyLevel : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevel(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;
};

class PointsDataOtherPointsTallyLevelByEvent : public PointsDataOtherPoints
{
public:
	PointsDataOtherPointsTallyLevelByEvent(CAgilityBookViewPoints* pView, std::list<OtherPtInfo> const& inRunList);

	virtual std::string OnNeedText(size_t index) const;
	virtual void OnDblClick() const;
};
