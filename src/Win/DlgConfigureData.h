#pragma once

/*
 * Copyright © 2002-2004 David Connet. All Rights Reserved.
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
 */

#include <string>
#include <vector>
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigFault;
class ARBConfigLevel;
class ARBConfigOtherPoints;
class ARBConfigScoring;
class ARBConfigSubLevel;
class ARBConfigTitle;
class ARBConfigVenue;

/////////////////////////////////////////////////////////////////////////////

/**
 * This class and its derivatives are used to keep data associated with controls.
 */
class CDlgConfigureData
{
public:
	CDlgConfigureData()
	{
	}
	virtual ~CDlgConfigureData()			{}
	virtual CString OnNeedText(int iColumn) const = 0;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataVenue : public CDlgConfigureData
{
public:
	CDlgConfigureDataVenue(ARBConfigVenue* venue)
		: CDlgConfigureData()
		, m_Venue(venue)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigVenue* GetVenue() const		{return m_Venue;}
protected:
	ARBConfigVenue* m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CDlgConfigureData
{
public:
	CDlgConfigureDataFault(ARBConfigFault* fault)
		: CDlgConfigureData()
		, m_Fault(fault)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigFault* GetFault() const		{return m_Fault;}
protected:
	ARBConfigFault* m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CDlgConfigureData
{
public:
	CDlgConfigureDataOtherPoints(ARBConfigOtherPoints* otherPoints)
		: CDlgConfigureData()
		, m_OtherPoints(otherPoints)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigOtherPoints* GetOtherPoints() const	{return m_OtherPoints;}
protected:
	ARBConfigOtherPoints* m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CDlgConfigureData
{
public:
	CDlgConfigureDataDivision(ARBConfigDivision* div)
		: CDlgConfigureData()
		, m_Div(div)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigDivision* GetDivision() const	{return m_Div;}
protected:
	ARBConfigDivision* m_Div;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLevel : public CDlgConfigureData
{
public:
	CDlgConfigureDataLevel(ARBConfigDivision* div, ARBConfigLevel* level)
		: CDlgConfigureData()
		, m_Division(div)
		, m_Level(level)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigDivision* GetDivision() const	{return m_Division;}
	ARBConfigLevel* GetLevel() const		{return m_Level;}
protected:
	ARBConfigDivision* m_Division;
	ARBConfigLevel* m_Level;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataSubLevel : public CDlgConfigureData
{
public:
	CDlgConfigureDataSubLevel(ARBConfigDivision* div, ARBConfigLevel* level, ARBConfigSubLevel* subLevel)
		: CDlgConfigureData()
		, m_Division(div)
		, m_Level(level)
		, m_SubLevel(subLevel)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigDivision* GetDivision() const	{return m_Division;}
	ARBConfigLevel* GetLevel() const		{return m_Level;}
	ARBConfigSubLevel* GetSubLevel() const	{return m_SubLevel;}
protected:
	ARBConfigDivision* m_Division;
	ARBConfigLevel* m_Level;
	ARBConfigSubLevel* m_SubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataTitle : public CDlgConfigureData
{
public:
	CDlgConfigureDataTitle(ARBConfigDivision* div, ARBConfigTitle* title)
		: CDlgConfigureData()
		, m_Division(div)
		, m_Title(title)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigDivision* GetDivision() const	{return m_Division;}
	ARBConfigTitle* GetTitle() const		{return m_Title;}
protected:
	ARBConfigDivision* m_Division;
	ARBConfigTitle* m_Title;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataEvent : public CDlgConfigureData
{
public:
	CDlgConfigureDataEvent(ARBConfigEvent* event)
		: CDlgConfigureData()
		, m_Event(event)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigEvent* GetEvent() const		{return m_Event;}
protected:
	ARBConfigEvent* m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataScoring : public CDlgConfigureData
{
public:
	CDlgConfigureDataScoring(ARBConfigEvent* event, ARBConfigScoring* scoring)
		: CDlgConfigureData()
		, m_Event(event)
		, m_Scoring(scoring)
	{
	}
	virtual CString OnNeedText(int iColumn) const;
	ARBConfigEvent* GetEvent() const		{return m_Event;}
	ARBConfigScoring* GetScoring() const	{return m_Scoring;}
protected:
	ARBConfigEvent* m_Event;
	ARBConfigScoring* m_Scoring;
};
