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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2004-04-02 DRC Up the ref count to prevent problems.
 */

#include "ARBTypes.h"
#include "ListData.h"
class CDlgConfigVenue;


class CDlgConfigureDataBase : public CTreeData
{
public:
	CDlgConfigureDataBase(CDlgConfigVenue* pDlg);

	virtual wxString OnNeedText() const			{return wxEmptyString;}

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
	virtual CDlgConfigureDataBase* DoMove(bool bUp)	{return NULL;}

protected:
	CDlgConfigVenue* m_pDlg;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataVenue : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataVenue(ARBConfigVenuePtr venue);
	ARBConfigVenuePtr GetVenue() const			{return m_Venue;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
protected:
	ARBConfigVenuePtr m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataFault(ARBConfigFaultPtr fault);
	ARBConfigFaultPtr GetFault() const			{return m_Fault;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
protected:
	ARBConfigFaultPtr m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataOtherPoints(ARBConfigOtherPointsPtr otherPoints);
	ARBConfigOtherPointsPtr GetOtherPoints() const	{return m_OtherPoints;}
	virtual bool CanEdit() const				{return true;}
	virtual bool CanDelete() const				{return true;}
	virtual bool CanCopy() const				{return true;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
protected:
	ARBConfigOtherPointsPtr m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataDivision(
			CDlgConfigVenue* pDlg,
			ARBConfigDivisionPtr div);
	ARBConfigDivisionPtr GetDivision() const	{return m_Div;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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
			ARBConfigDivisionPtr div,
			ARBConfigLevelPtr level);
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const			{return m_Level;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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
			ARBConfigDivisionPtr div,
			ARBConfigLevelPtr level,
			ARBConfigSubLevelPtr subLevel);
	ARBConfigDivisionPtr GetDivision() const	{return m_Division;}
	ARBConfigLevelPtr GetLevel() const			{return m_Level;}
	ARBConfigSubLevelPtr GetSubLevel() const	{return m_SubLevel;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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
			ARBConfigTitlePtr title);
	ARBConfigTitlePtr GetTitle() const			{return m_Title;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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
			ARBConfigEventPtr inEvent);
	ARBConfigEventPtr GetEvent() const			{return m_Event;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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

class CDlgConfigureDataMultiQ : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataMultiQ(
			CDlgConfigVenue* pDlg,
			ARBConfigMultiQPtr multiq);
	ARBConfigMultiQPtr GetMultiQ() const		{return m_MultiQ;}
	virtual wxString OnNeedText() const;
	virtual wxString OnNeedText(int iColumn) const;
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
