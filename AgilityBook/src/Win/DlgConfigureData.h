#pragma once

/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Data classes to support DlgConfigure operations.
 * @author David Connet
 *
 * Revision History
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-04-02 DRC Up the ref count to prevent problems.
 */

#include <vector>
#include "ListData.h"

class CDlgConfigureDataVenue : public CListDataDispInfo
{
public:
	CDlgConfigureDataVenue(ARBConfigVenuePtr venue);
	~CDlgConfigureDataVenue();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigVenuePtr GetVenue() const		{return m_Venue;}
protected:
	ARBConfigVenuePtr m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CListDataDispInfo
{
public:
	CDlgConfigureDataFault(ARBConfigFaultPtr fault);
	~CDlgConfigureDataFault();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigFaultPtr GetFault() const		{return m_Fault;}
protected:
	ARBConfigFaultPtr m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CListDataDispInfo
{
public:
	CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr otherPoints);
	~CDlgConfigureDataOtherPoints();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigOtherPointsPtr GetOtherPoints() const	{return m_OtherPoints;}
protected:
	ARBConfigOtherPointsPtr m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CListDataDispInfo
{
public:
	CDlgConfigureDataDivision(ARBConfigDivisionPtr div);
	~CDlgConfigureDataDivision();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigDivisionPtr GetDivision() const	{return m_Div;}
protected:
	ARBConfigDivisionPtr m_Div;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLevel : public CListDataDispInfo
{
public:
	CDlgConfigureDataLevel(
			ARBConfigDivisionPtr div,
			ARBConfigLevelPtr level);
	~CDlgConfigureDataLevel();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const		{return m_Level;}
protected:
	ARBConfigDivisionPtr m_Division;
	ARBConfigLevelPtr m_Level;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataSubLevel : public CListDataDispInfo
{
public:
	CDlgConfigureDataSubLevel(
			ARBConfigDivisionPtr div,
			ARBConfigLevelPtr level,
			ARBConfigSubLevelPtr subLevel);
	~CDlgConfigureDataSubLevel();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const			{return m_Level;}
	ARBConfigSubLevelPtr GetSubLevel() const	{return m_SubLevel;}
protected:
	ARBConfigDivisionPtr m_Division;
	ARBConfigLevelPtr m_Level;
	ARBConfigSubLevelPtr m_SubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataTitle : public CListDataDispInfo
{
public:
	CDlgConfigureDataTitle(
			ARBConfigTitlePtr title);
	~CDlgConfigureDataTitle();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigTitlePtr GetTitle() const		{return m_Title;}
protected:
	ARBConfigTitlePtr m_Title;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataEvent : public CListDataDispInfo
{
public:
	CDlgConfigureDataEvent(ARBConfigEventPtr inEvent);
	~CDlgConfigureDataEvent();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigEventPtr GetEvent() const		{return m_Event;}
protected:
	ARBConfigEventPtr m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataMultiQ : public CListDataDispInfo
{
public:
	CDlgConfigureDataMultiQ(ARBConfigMultiQPtr multiq);
	~CDlgConfigureDataMultiQ();
	virtual tstring OnNeedText() const;
	virtual tstring OnNeedText(int iColumn) const;
	ARBConfigMultiQPtr GetMultiQ() const		{return m_MultiQ;}
protected:
	ARBConfigMultiQPtr m_MultiQ;
};
