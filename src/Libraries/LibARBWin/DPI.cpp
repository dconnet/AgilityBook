/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief DPI handling
 * @author David Connet
 *
 * call only on per-monitor aware
 * Win10AU: BOOL WINAPI EnableNonClientDpiScaling(_In_ HWND hwnd);
 * user32.dll, winuser.h
 *
 * Revision History
 * 2021-12-21 Use wxDisplay to determine scaling.
 * 2020-05-20 Change GetScale from uint to int (otherwise a negative coord is munged)
 * 2019-01-01 GetContentScaleFactor is causing a problem on Mac (wx3.1.2)
 * 2018-10-11 Moved to Win LibARBWin
 * 2014-12-27 Created.
 */

#include "stdafx.h"
#include "LibARBWin/DPI.h"

#include "LibARBWin/ImageHelperBase.h"

#include <wx/display.h>

//#ifdef EVT_DPI_CHANGED
// wx3.1.3 adds much better dpi support
// TODO: Use it!

#if _MSC_VER >= 1800
#include <shellscalingapi.h>

#else
typedef enum PROCESS_DPI_AWARENESS
{
	PROCESS_DPI_UNAWARE = 0,
	PROCESS_SYSTEM_DPI_AWARE = 1,
	PROCESS_PER_MONITOR_DPI_AWARE = 2
} PROCESS_DPI_AWARENESS;
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#pragma warning(disable : 4191) // unsafe conversion from 'FARPROC' to 'DPI::CDPI::GETPROCESSDPIAWARENESS'
#endif

#if !defined(__WXMSW__)
#define FORCE_SCALE_100
#endif

namespace DPI
{

// Based Microsoft's DPITutorial
class CDPI
{
	unsigned int m_nScaleFactor;
	unsigned int m_nScaleFactorSDA;
	PROCESS_DPI_AWARENESS m_Awareness;
#ifdef WIN32
	typedef HRESULT(WINAPI* GETPROCESSDPIAWARENESS)(HANDLE, PROCESS_DPI_AWARENESS*);
	typedef BOOL(WINAPI* ISPROCESSDPIAWARE)(void);
	GETPROCESSDPIAWARENESS m_pGetProcessDpiAwareness;
	ISPROCESSDPIAWARE m_pIsIsProcessDPIAware;
#endif

public:
	CDPI()
		: m_nScaleFactor(0)
		, m_nScaleFactorSDA(0)
		, m_Awareness(PROCESS_DPI_UNAWARE)
#ifdef WIN32
		, m_pGetProcessDpiAwareness(nullptr)
		, m_pIsIsProcessDPIAware(nullptr)
#endif
	{
#ifdef WIN32
		HMODULE hCore = LoadLibrary(L"shcore.dll");
		if (hCore)
		{
			m_pGetProcessDpiAwareness = (GETPROCESSDPIAWARENESS)GetProcAddress(hCore, "GetProcessDpiAwareness");
		}

		HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
		if (m_pGetProcessDpiAwareness)
			m_pGetProcessDpiAwareness(hProcess, &m_Awareness);
		else
		{
			HMODULE hUser = LoadLibrary(L"user32.dll");
			if (hUser)
				m_pIsIsProcessDPIAware = (ISPROCESSDPIAWARE)GetProcAddress(hUser, "IsProcessDPIAware");
			if (m_pIsIsProcessDPIAware && m_pIsIsProcessDPIAware())
				m_Awareness = PROCESS_SYSTEM_DPI_AWARE;
			else
				m_Awareness = PROCESS_DPI_UNAWARE;
		}
#else
		// I'm using this, so I must be aware!
		m_Awareness = PROCESS_SYSTEM_DPI_AWARE;
#endif

		// Set scaling of main monitor for system dpi-aware apps.
		SetScale(1, 1);
	}

	void SetScale(int x, int y)
	{
#ifdef FORCE_SCALE_100
		SetScale(100);
#else
		// Multimon TODO: determine with display it is on.
		wxDisplay display;
		SetScale(static_cast<unsigned int>(display.GetScaleFactor() * 100));
#endif
	}

	void SetScale(unsigned int scale)
	{
		m_nScaleFactor = scale;
		if (m_nScaleFactorSDA == 0)
			m_nScaleFactorSDA = m_nScaleFactor; // Save the first scale factor, which is all that SDA apps know about
	}

	int Scale(int x)
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE || GetScale() == 100)
			return x;
		return x * GetScale() / 100;
	}

	int UnScale(int x)
	{
		if (m_Awareness == PROCESS_DPI_UNAWARE || GetScale() == 100)
			return x;
		return x * 100 / GetScale();
	}

	unsigned int GetScale()
	{
		// DPI Unaware: Return the input value with no scaling.
		// These apps are always virtualized to 96 DPI and scaled by the system for the DPI of the monitor where shown.
		if (m_Awareness == PROCESS_DPI_UNAWARE)
			return 100;

		// System DPI Aware: Return the input value scaled by the factor determined by the system DPI when the app was
		// launched. These apps render themselves according to the DPI of the display where they are launched, and they
		// expect that scaling to remain constant for all displays on the system. These apps are scaled up or down when
		// moved to a display with a different DPI from the system DPI.
		if (m_Awareness == PROCESS_SYSTEM_DPI_AWARE)
			return m_nScaleFactorSDA;

		// Per-Monitor DPI Aware: Return the input value scaled by the factor for the display which contains most of the
		// window. These apps render themselves for any DPI, and re-render when the DPI changes (as indicated by the
		// WM_DPICHANGED window message).
		return m_nScaleFactor;
	}
};


static CDPI& GetDPI()
{
	static CDPI dpi;
	return dpi;
}

/*
void SetScale(int x, int y)
{
	GetDPI().SetScale(x, y);
}


void SetScale(unsigned int scale)
{
	GetDPI().SetScale(scale);
}
*/

int Scale(wxWindow const* pWindow, int x)
{
	return x * GetScale(pWindow) / 100;
}


int Scale(int x)
{
	return GetDPI().Scale(x);
}


int UnScale(wxWindow const* pWindow, int x)
{
	// return GetDPI().UnScale(x);
	return x * 100 / GetScale(pWindow);
}


int unsigned GetScale(wxWindow const* pWindow)
{
	if (!pWindow)
		return GetDPI().GetScale();
	else
	{
#ifdef FORCE_SCALE_100
		// For non-windows, this will be unaware and return 100. This is a
		// kludge because of the LOAD_BITMAP_PNG macro in ImageHelperHBase.h
		// On 200% Ubuntu, we can only load the 100% images. So make sure we
		// scale appropriately.
		return GetDPI().GetScale();
#else
		return static_cast<unsigned int>(pWindow->GetDPIScaleFactor() * 100);
#endif
	}
}

}; // namespace DPI
