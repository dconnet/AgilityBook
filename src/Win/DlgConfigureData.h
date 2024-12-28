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


namespace dconSoft
{
class CDlgConfigVenue;


class CDlgConfigureDataBase : public ARBWin::CTreeData
{
public:
	CDlgConfigureDataBase(CDlgConfigVenue* pDlg);

	wxString OnNeedText() const override
	{
		return wxEmptyString;
	}
	virtual wxString OnNeedText(int iColumn) const
	{
		return wxEmptyString;
	}

	// After adding a node, use this to add subchildren in venue (levels/sublevels)
	virtual void AddSubItems()
	{
	}

	virtual bool CanAdd() const
	{
		return false;
	}
	virtual bool CanEdit() const
	{
		return false;
	}
	virtual bool CanDelete() const
	{
		return false;
	}
	virtual bool CanCopy() const
	{
		return false;
	}
	// Is the item allowed to move up/down - does not check for boundaries
	virtual bool CanMove() const
	{
		return false;
	}

	virtual bool DoAdd()
	{
		return false;
	}
	virtual bool DoEdit()
	{
		return false;
	}
	virtual bool DoDelete()
	{
		return false;
	}
	virtual bool DoCopy()
	{
		return false;
	}
	virtual CDlgConfigureDataBase* DoMove(bool bUp)
	{
		return nullptr;
	}

protected:
	CDlgConfigVenue* m_pDlg;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataVenue : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataVenue(ARB::ARBConfigVenuePtr const& inVenue);
	ARB::ARBConfigVenuePtr GetVenue() const
	{
		return m_Venue;
	}
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;

protected:
	ARB::ARBConfigVenuePtr m_Venue;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataFault : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataFault(ARB::ARBConfigFaultPtr const& inFault);
	ARB::ARBConfigFaultPtr GetFault() const
	{
		return m_Fault;
	}
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;

protected:
	ARB::ARBConfigFaultPtr m_Fault;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataOtherPoints : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataOtherPoints(ARB::ARBConfigOtherPointsPtr const& inOtherPoints);
	ARB::ARBConfigOtherPointsPtr GetOtherPoints() const
	{
		return m_OtherPoints;
	}
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;

protected:
	ARB::ARBConfigOtherPointsPtr m_OtherPoints;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataDivision : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataDivision(CDlgConfigVenue* pDlg, ARB::ARBConfigDivisionPtr const& inDiv);
	ARB::ARBConfigDivisionPtr GetDivision() const
	{
		return m_Div;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	void AddSubItems() override;
	bool CanAdd() const override
	{
		return true;
	}
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoAdd() override;
	bool DoEdit() override;
	bool DoDelete() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigDivisionPtr m_Div;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLevel : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataLevel(
		CDlgConfigVenue* pDlg,
		ARB::ARBConfigDivisionPtr const& inDiv,
		ARB::ARBConfigLevelPtr const& inLevel);
	ARB::ARBConfigDivisionPtr GetDivision() const
	{
		return m_Division;
	}
	ARB::ARBConfigLevelPtr GetLevel() const
	{
		return m_Level;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	void AddSubItems() override;
	bool CanAdd() const override
	{
		return true;
	}
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoAdd() override;
	bool DoEdit() override;
	bool DoDelete() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigDivisionPtr m_Division;
	ARB::ARBConfigLevelPtr m_Level;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataSubLevel : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataSubLevel(
		CDlgConfigVenue* pDlg,
		ARB::ARBConfigDivisionPtr const& inDiv,
		ARB::ARBConfigLevelPtr const& inLevel,
		ARB::ARBConfigSubLevelPtr const& inSubLevel);
	ARB::ARBConfigDivisionPtr GetDivision() const
	{
		return m_Division;
	}
	ARB::ARBConfigLevelPtr GetLevel() const
	{
		return m_Level;
	}
	ARB::ARBConfigSubLevelPtr GetSubLevel() const
	{
		return m_SubLevel;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoEdit() override;
	bool DoDelete() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigDivisionPtr m_Division;
	ARB::ARBConfigLevelPtr m_Level;
	ARB::ARBConfigSubLevelPtr m_SubLevel;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataTitle : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataTitle(CDlgConfigVenue* pDlg, ARB::ARBConfigTitlePtr const& inTitle);
	ARB::ARBConfigTitlePtr GetTitle() const
	{
		return m_Title;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoEdit() override;
	bool DoDelete() override;
	bool DoCopy() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigTitlePtr m_Title;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataEvent : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataEvent(CDlgConfigVenue* pDlg, ARB::ARBConfigEventPtr const& inEvent);
	ARB::ARBConfigEventPtr GetEvent() const
	{
		return m_Event;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoEdit() override;
	bool DoDelete() override;
	bool DoCopy() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigEventPtr m_Event;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataLifetimeName : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataLifetimeName(CDlgConfigVenue* pDlg, ARB::ARBConfigLifetimeNamePtr const& inName);
	ARB::ARBConfigLifetimeNamePtr GetLifetimeName() const
	{
		return m_pName;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoEdit() override;
	bool DoDelete() override;
	bool DoCopy() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigLifetimeNamePtr m_pName;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgConfigureDataMultiQ : public CDlgConfigureDataBase
{
public:
	CDlgConfigureDataMultiQ(CDlgConfigVenue* pDlg, ARB::ARBConfigMultiQPtr const& inMultiq);
	ARB::ARBConfigMultiQPtr GetMultiQ() const
	{
		return m_MultiQ;
	}
	wxString OnNeedText() const override;
	wxString OnNeedText(int iColumn) const override;
	bool CanEdit() const override
	{
		return true;
	}
	bool CanDelete() const override
	{
		return true;
	}
	bool CanCopy() const override
	{
		return true;
	}
	bool CanMove() const override
	{
		return true;
	}
	bool DoEdit() override;
	bool DoDelete() override;
	bool DoCopy() override;
	CDlgConfigureDataBase* DoMove(bool bUp) override;

protected:
	ARB::ARBConfigMultiQPtr m_MultiQ;
};

} // namespace dconSoft
