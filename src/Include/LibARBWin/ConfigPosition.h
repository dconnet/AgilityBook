#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 */

/**
 * @file
 *
 * Revision History
 * 2021-09-19 Changed default config paths.
 * 2021-03-06 Moved into LibARBWin.
 * 2018-03-31 Added configuration bits.
 * 2017-12-22 Add support for saving dialogs.
 * 2012-08-13 Moved ConfigPosition out of Globals.cpp
 */

#include "LibwxARBWin.h"


/**
 * Set/Save application's location/size.
 * Must have set up wxConfig already.
 */
class ARBWIN_API CConfigPosition
{
protected:
	wxString m_name;
	uint8_t m_flags;
	DECLARE_NO_COPY_IMPLEMENTED(CConfigPosition)
	CConfigPosition() = delete;

public:
	enum
	{
		eConfigPos = 0x1,
		eConfigSize = 0x2,
		eConfigState = 0x4,

		eConfigNoState = eConfigPos | eConfigSize,
		eConfigAll = eConfigNoState | eConfigState
	};

	explicit CConfigPosition(uint8_t flags);
	CConfigPosition(wxString const& name, uint8_t flags);
	virtual ~CConfigPosition();

	/**
	 * Set window position
	 * @param wnd Window to set.
	 * @param bUseExisting Use existing position as default.
	 * @param pPosSet Was the position restored?
	 * @return True if app was last maximized.
	 */
	virtual bool Set(wxWindow* wnd, bool bUseExisting = false, bool* pPosSet = nullptr);

	virtual void Save(wxTopLevelWindow* wnd);
	virtual void SaveWindow(wxWindow* wnd);

protected:
	virtual wxString SectionName() const
	{
		return L"Positions";
	}
	virtual wxString LastX() const
	{
		return (m_flags & eConfigPos) ? wxString::Format(L"%s/%sLastX", SectionName(), m_name) : wxString();
	}
	virtual wxString LastY() const
	{
		return (m_flags & eConfigPos) ? wxString::Format(L"%s/%sLastY", SectionName(), m_name) : wxString();
	}
	virtual wxString LastCX() const
	{
		return (m_flags & eConfigSize) ? wxString::Format(L"%s/%sLastCX", SectionName(), m_name) : wxString();
	}
	virtual wxString LastCY() const
	{
		return (m_flags & eConfigSize) ? wxString::Format(L"%s/%sLastCY", SectionName(), m_name) : wxString();
	}
	virtual wxString LastState() const
	{
		return (m_flags & eConfigState) ? wxString::Format(L"%s/%sLastState", SectionName(), m_name) : wxString();
	}
};
