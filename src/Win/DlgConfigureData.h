#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Data classes to support DlgConfigure operations.
 * @author David Connet
 *
 * Revision History
 * 2016-06-17 Add support for Lifetime names.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-04-02 Up the ref count to prevent problems.
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/ListData.h"
class CDlgConfigVenue;


class CDlgConfigureDataBase : public CTreeData
{
public:
	CDlgConfigureDataBase(CDlgConfigVenue* pDlg);

	virtual std::wstring OnNeedText() const			{return wxEmptyString;}

	// After adding a node, use this to add subchildren in venue (levels/sublevels)
	virtual void AddSubItems()					{}

	virtual bool CanAdd() const					{return false;}
	virtual bool CanEdit() const				{return false;}
	virtual bool CanDelete() const				{return false;}
	virtual bool CanCopy() const				{return false;}
	// Is the item allowed to move up/down - does not check for boundaries
	virtual bool CanMove() const				{return false;}

	virtual bool DoAdd()						{return false;}
	virtual bool DoEdit()						{return false;}
	virtual bool DoDelete()						{return false;}
	virtual bool DoCopy()						{return false;}
	virtual CDlgConfigureDataBase* DoMove(bool bUp)	{return nullptr;}

protected:
	CDlgConfigVenue* m_pDlg;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataVenue : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataVenue(ARBConfigVenuePtr const& inVenue);
	ARBConfigVenuePtr GetVenue() const			{return m_Venue;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
protected:
	ARBConfigVenuePtr m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataFault(ARBConfigFaultPtr const& inFault);
	ARBConfigFaultPtr GetFault() const			{return m_Fault;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
protected:
	ARBConfigFaultPtr m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr const& inOtherPoints);
	ARBConfigOtherPointsPtr GetOtherPoints() const	{return m_OtherPoints;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
protected:
	ARBConfigOtherPointsPtr m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataDivision(
			CDlgConfigVenue* pDlg,
			ARBConfigDivisionPtr const& inDiv);
	ARBConfigDivisionPtr GetDivision() const	{return m_Div;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual void AddSubItems();
	virtual bool CanAdd() const					{return true;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoAdd();
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigDivisionPtr m_Div;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLevel : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataLevel(
			CDlgConfigVenue* pDlg,
			ARBConfigDivisionPtr const& inDiv,
			ARBConfigLevelPtr const& inLevel);
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const			{return m_Level;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual void AddSubItems();
	virtual bool CanAdd() const					{return true;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoAdd();
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigDivisionPtr m_Division;
	ARBConfigLevelPtr m_Level;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataSubLevel : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataSubLevel(
			CDlgConfigVenue* pDlg,
			ARBConfigDivisionPtr const& inDiv,
			ARBConfigLevelPtr const& inLevel,
			ARBConfigSubLevelPtr const& inSubLevel);
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const			{return m_Level;}
	ARBConfigSubLevelPtr GetSubLevel() const	{return m_SubLevel;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigDivisionPtr m_Division;
	ARBConfigLevelPtr m_Level;
	ARBConfigSubLevelPtr m_SubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataTitle : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataTitle(
			CDlgConfigVenue* pDlg,
			ARBConfigTitlePtr const& inTitle);
	ARBConfigTitlePtr GetTitle() const			{return m_Title;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual bool DoCopy();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigTitlePtr m_Title;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataEvent : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataEvent(
			CDlgConfigVenue* pDlg,
			ARBConfigEventPtr const& inEvent);
	ARBConfigEventPtr GetEvent() const			{return m_Event;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual bool DoCopy();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigEventPtr m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLifetimeName : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataLifetimeName(
			CDlgConfigVenue* pDlg,
			ARBConfigLifetimeNamePtr const& inName);
	ARBConfigLifetimeNamePtr GetLifetimeName() const	{return m_pName;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual bool DoCopy();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigLifetimeNamePtr m_pName;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataMultiQ : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataMultiQ(
			CDlgConfigVenue* pDlg,
			ARBConfigMultiQPtr const& inMultiq);
	ARBConfigMultiQPtr GetMultiQ() const		{return m_MultiQ;}
	virtual std::wstring OnNeedText() const;
	virtual std::wstring OnNeedText(int iColumn) const;
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual bool CanMove() const				{return true;}
	virtual bool DoEdit();
	virtual bool DoDelete();
	virtual bool DoCopy();
	virtual CDlgConfigureDataBase* DoMove(bool bUp);
protected:
	ARBConfigMultiQPtr m_MultiQ;
};
