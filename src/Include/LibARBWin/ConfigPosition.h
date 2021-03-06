#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 */

/**
 * @file
 *
 * Revision History
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

	CConfigPosition(uint8_t flags);
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
	virtual wchar_t const* const LastX() const
	{
		return (m_flags & eConfigPos) ? L"Settings/lastX" : nullptr;
	}
	virtual wchar_t const* const LastY() const
	{
		return (m_flags & eConfigPos) ? L"Settings/lastY" : nullptr;
	}
	virtual wchar_t const* const LastCX() const
	{
		return (m_flags & eConfigSize) ? L"Settings/lastCX" : nullptr;
	}
	virtual wchar_t const* const LastCY() const
	{
		return (m_flags & eConfigSize) ? L"Settings/lastCY" : nullptr;
	}
	virtual wchar_t const* const LastState() const
	{
		return (m_flags & eConfigState) ? L"Settings/lastState" : nullptr;
	}
};
