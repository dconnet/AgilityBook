/*
 * Copyright (c) David Connet. All Rights Reserved.
 */

/**
 * @file
 * @brief DlgLocations
 * @author David Connet
 *
 * Revision History
 * 2021-09-23 Fix reading of last state.
 * 2021-09-19 Changed default config paths.
 * 2021-03-06 Moved into LibARBWin.
 * 2017-12-22 Add support for saving dialogs.
 * 2015-08-11 Add HiDPI support.
 * 2013-12-14 2.0.0 Standardized History
 * 2012-08-13       Moved ConfigPosition out of Globals.cpp
 */

#include "stdafx.h"
#include "LibARBWin/ConfigPosition.h"

#include "LibARBWin/DPI.h"
#include <wx/config.h>
#include <wx/display.h>

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif


CConfigPosition::CConfigPosition(uint8_t flags)
	: CConfigPosition(wxString(), flags)
{
}


CConfigPosition::CConfigPosition(wxString const& name, uint8_t flags)
	: m_name(name)
	, m_flags(flags)
{
}


CConfigPosition::~CConfigPosition()
{
}


bool CConfigPosition::Set(wxWindow* wnd, bool bUseExisting, bool* pPosSet)
{
	if (!wnd || !wxConfig::Get())
		return false;

	int x = wxDefaultCoord, y = wxDefaultCoord, width = 0, height = 0;
	wnd->GetSize(&width, &height);
	int defWidth = width;
	int defHeight = height;

	if (bUseExisting)
	{
		wxRect r = wnd->GetScreenRect();
		x = r.GetLeft();
		y = r.GetTop();
	}

	if (pPosSet)
		*pPosSet = false;

	if (!LastX().empty())
	{
		if (wxConfig::Get()->Read(LastX(), &x, x))
		{
			x = DPI::Scale(x);
			if (pPosSet)
				*pPosSet = true;
		}
	}
	if (!LastY().empty())
	{
		if (wxConfig::Get()->Read(LastY(), &y, y))
		{
			y = DPI::Scale(y);
			if (pPosSet)
				*pPosSet = true;
		}
	}
	if (!LastCX().empty())
	{
		if (wxConfig::Get()->Read(LastCX(), &width, width))
			width = DPI::Scale(width);
	}
	if (!LastCY().empty())
	{
		if (wxConfig::Get()->Read(LastCY(), &height, height))
			height = DPI::Scale(height);
	}

	long state = 0;
	if (!LastState().empty())
		wxConfig::Get()->Read(LastState(), &state, state);

	bool bCompute = false;
	wxMouseState mouseState = ::wxGetMouseState();
	if (wxDefaultCoord != x)
	{
		bCompute = true;
		mouseState.SetX(x);
	}
	if (wxDefaultCoord != y)
	{
		bCompute = true;
		mouseState.SetY(y);
	}
	wxSize curSize(defWidth, defHeight);
	if (defWidth != width)
	{
		bCompute = true;
		curSize.SetWidth(width);
	}
	if (defHeight != height)
	{
		bCompute = true;
		curSize.SetHeight(height);
	}
	if (bCompute)
	{
		wxPoint curPt(mouseState.GetX(), mouseState.GetY());
		int display = wxDisplay::GetFromPoint(curPt);
		if (wxNOT_FOUND == display)
			display = 0; // If the display can't be found, use the primary.
		wxDisplay monitor(display);
		wxRect rWorkSpace = monitor.GetClientArea();
		wxRect rect(curPt, curSize);
		// Make sure window is not bigger.
		if (rect.GetWidth() > rWorkSpace.GetWidth())
			rect.SetRight(rect.GetLeft() + rWorkSpace.GetWidth());
		if (rect.GetHeight() > rWorkSpace.GetHeight())
			rect.SetBottom(rect.GetTop() + rWorkSpace.GetHeight());
		// Make sure the window is fully visible in the screen.
		if (!rWorkSpace.Contains(rect.GetTopLeft()))
		{
			if (rect.GetLeft() < rWorkSpace.GetLeft())
				rect.Offset(rWorkSpace.GetLeft() - rect.GetLeft(), 0);
			if (rect.GetTop() < rWorkSpace.GetTop())
				rect.Offset(0, rWorkSpace.GetTop() - rect.GetTop());
		}
		// Only check the bottom-right if the rect has size.
		if (!rect.IsEmpty() && !rWorkSpace.Contains(rect.GetBottomRight()))
		{
			if (rect.GetRight() > rWorkSpace.GetRight())
				rect.Offset(rWorkSpace.GetRight() - rect.GetRight(), 0);
			if (rect.GetBottom() > rWorkSpace.GetBottom())
				rect.Offset(0, rWorkSpace.GetBottom() - rect.GetBottom());
		}
		if (wxDefaultCoord != x)
			x = rect.GetLeft();
		if (wxDefaultCoord != y)
			y = rect.GetTop();
		if (defWidth != width)
			width = rect.GetWidth();
		if (defHeight != height)
			height = rect.GetHeight();
	}
	wnd->SetSize(x, y, width, height);

	return 0 < state;
}


void CConfigPosition::Save(wxTopLevelWindow* wnd)
{
	if (!wnd || !wxConfig::Get())
		return;

	long state = 0;
	if (wnd->IsMaximized() || wnd->IsFullScreen())
	{
		state = 1;
	}
	else if (wnd->IsIconized())
	{
		state = -1;
	}
	else
	{
		SaveWindow(wnd);
	}
	if (!LastState().empty())
		wxConfig::Get()->Write(LastState(), state);
}


void CConfigPosition::SaveWindow(wxWindow* wnd)
{
	wxRect r = wnd->GetScreenRect();
	if (!LastX().empty())
		wxConfig::Get()->Write(LastX(), DPI::UnScale(wnd, r.x));
	if (!LastY().empty())
		wxConfig::Get()->Write(LastY(), DPI::UnScale(wnd, r.y));
	if (!LastCX().empty())
		wxConfig::Get()->Write(LastCX(), DPI::UnScale(wnd, r.width));
	if (!LastCY().empty())
		wxConfig::Get()->Write(LastCY(), DPI::UnScale(wnd, r.height));
}
