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
#include "ARBConfigFault.h"
#include "ARBConfigSubLevel.h"
class ARBAgilityRecordBook;
class ARBConfigDivision;
class ARBConfigEvent;
class ARBConfigLevel;
class ARBConfigOtherPoints;
class ARBConfigScoring;
class ARBConfigTitle;
class ARBConfigVenue;
class CDlgConfigure;
class CDlgConfigureDataDivision;
class CDlgConfigureDataEvent;
class CDlgConfigureDataVenue;
class CDlgFixup;

/////////////////////////////////////////////////////////////////////////////

/**
 * This class and its derivatives are used to keep data associated with a tree item.
 */
class CDlgConfigureData
{
public:
	typedef enum
	{
		eUserData,
		eVenues,
		eFaultTypes,
		eOtherPoints,
		eDivisions,
		eLevels,
		eTitles,
		eEvents,
		eScoring,
		eScoringInfo
	} eType;
	CDlgConfigureData(CDlgConfigure* pDlg, eType type)
		: m_pDlg(pDlg)
		, m_Type(type)
		, m_hItem(NULL)
	{
	}
	virtual ~CDlgConfigureData()			{}
	eType GetType() const					{return m_Type;}
	HTREEITEM GetHTreeItem() const			{return m_hItem;}
	void SetHTreeItem(HTREEITEM hItem)		{m_hItem = hItem;}
	void SetItemCurrent(HTREEITEM hItem = NULL, bool bExpand = false);
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData();
	virtual BOOL CanAdd() const;
	virtual void Add();
	virtual BOOL CanDuplicate() const		{return FALSE;}
	virtual void Duplicate()				{}
	virtual BOOL CanEdit() const;
	virtual void Edit();
	virtual BOOL CanDelete() const;
	virtual void Delete();
protected:
	CDlgConfigureData* GetParentData() const;
	BOOL CanParentAdd() const;
	void ParentAdd();
	// These keep me from having to declare friendship in the dialog for all derived classes.
	CTreeCtrl& GetTree() const;
	ARBAgilityRecordBook& GetRecordBook() const;
	ARBConfig& GetConfig() const;
	std::vector<CDlgFixup*>& GetFixupList() const;
	void InsertScorings(HTREEITEM hParent, ARBConfigEvent* event);
	CDlgConfigure* m_pDlg;
	eType m_Type;
	HTREEITEM m_hItem;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataVenue : public CDlgConfigureData
{
public:
	CDlgConfigureDataVenue(CDlgConfigure* pDlg, ARBConfigVenue* venue)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Venue(venue)
	{
	}
	ARBConfigVenue* GetVenue() const		{return m_Venue;}
	virtual CDlgConfigureDataVenue* GetVenueData()			{return this;}
	virtual CDlgConfigureDataDivision* GetDivisionData()	{return NULL;}
	virtual CDlgConfigureDataEvent* GetEventData()			{return NULL;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanDuplicate() const		{return TRUE;}
	virtual void Duplicate();
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigVenue* m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CDlgConfigureData
{
public:
	CDlgConfigureDataFault(CDlgConfigure* pDlg, ARBConfigFault* fault)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Fault(fault)
	{
	}
	ARBConfigFault* GetFault() const		{return m_Fault;}
	virtual CDlgConfigureDataVenue* GetVenueData()			{return NULL;}
	virtual CDlgConfigureDataDivision* GetDivisionData()	{return NULL;}
	virtual CDlgConfigureDataEvent* GetEventData()			{return NULL;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanDuplicate() const		{return TRUE;}
	virtual void Duplicate();
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigFault* m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CDlgConfigureData
{
public:
	CDlgConfigureDataOtherPoints(CDlgConfigure* pDlg, ARBConfigOtherPoints* otherPoints)
		: CDlgConfigureData(pDlg, eUserData)
		, m_OtherPoints(otherPoints)
	{
	}
	ARBConfigOtherPoints* GetOtherPoints() const	{return m_OtherPoints;}
	virtual CDlgConfigureDataVenue* GetVenueData()			{return NULL;}
	virtual CDlgConfigureDataDivision* GetDivisionData()	{return NULL;}
	virtual CDlgConfigureDataEvent* GetEventData()			{return NULL;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanDuplicate() const		{return TRUE;}
	virtual void Duplicate();
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigOtherPoints* m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CDlgConfigureData
{
public:
	CDlgConfigureDataDivision(CDlgConfigure* pDlg, ARBConfigDivision* div)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Div(div)
	{
	}
	ARBConfigDivision* GetDivision() const	{return m_Div;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData()	{return this;}
	virtual CDlgConfigureDataEvent* GetEventData()			{return NULL;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigDivision* m_Div;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLevel : public CDlgConfigureData
{
public:
	CDlgConfigureDataLevel(CDlgConfigure* pDlg, ARBConfigLevel* level)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Level(level)
	{
	}
	ARBConfigLevel* GetLevel() const		{return m_Level;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData()	{return NULL;}
	virtual BOOL CanAdd() const				{return TRUE;}
	virtual void Add();
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigLevel* m_Level;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataSubLevel : public CDlgConfigureData
{
public:
	CDlgConfigureDataSubLevel(CDlgConfigure* pDlg, ARBConfigSubLevel* subLevel)
		: CDlgConfigureData(pDlg, eUserData)
		, m_SubLevel(subLevel)
	{
	}
	ARBConfigSubLevel* GetSubLevel() const	{return m_SubLevel;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData()	{return NULL;}
	CDlgConfigureDataLevel* GetLevelData();
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigSubLevel* m_SubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataTitle : public CDlgConfigureData
{
public:
	CDlgConfigureDataTitle(CDlgConfigure* pDlg, ARBConfigTitle* title)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Title(title)
	{
	}
	ARBConfigTitle* GetTitle() const		{return m_Title;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData()	{return NULL;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigTitle* m_Title;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataEvent : public CDlgConfigureData
{
public:
	CDlgConfigureDataEvent(CDlgConfigure* pDlg, ARBConfigEvent* event)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Event(event)
	{
	}
	ARBConfigEvent* GetEvent() const		{return m_Event;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData()	{return this;}
	virtual BOOL CanAdd() const				{return CanParentAdd();}
	virtual void Add()						{ParentAdd();}
	virtual BOOL CanDuplicate() const		{return TRUE;}
	virtual void Duplicate();
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return TRUE;}
	virtual void Delete();
protected:
	ARBConfigEvent* m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataScoring : public CDlgConfigureData
{
public:
	CDlgConfigureDataScoring(CDlgConfigure* pDlg, ARBConfigScoring* scoring)
		: CDlgConfigureData(pDlg, eUserData)
		, m_Scoring(scoring)
	{
	}
	ARBConfigScoring* GetScoring() const	{return m_Scoring;}
	virtual CDlgConfigureDataVenue* GetVenueData();
	virtual CDlgConfigureDataDivision* GetDivisionData();
	virtual CDlgConfigureDataEvent* GetEventData();
	// All editing of scoring methods is done in the event edit dialog
	virtual BOOL CanAdd() const				{return FALSE;}
	virtual void Add()						{}
	virtual BOOL CanEdit() const			{return TRUE;}
	virtual void Edit();
	virtual BOOL CanDelete() const			{return FALSE;}
	virtual void Delete()					{}
protected:
	ARBConfigScoring* m_Scoring;
};
