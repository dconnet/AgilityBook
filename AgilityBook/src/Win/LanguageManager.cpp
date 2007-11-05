/*
 * Copyright © 2007 David Connet. All Rights Reserved.
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
 * @brief implementation of the CLanguageManager class
 * @author David Connet
 *
 * Revision History
 * @li 2007-09-22 DRC Created
 */

#include "stdafx.h"
#include "LanguageManager.h"
#include <imm.h>
#include <winnls.h>

#include "resource.h"
#include "DlgLanguage.h"
#include "VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Based on code in the VC8 satdll sample.

// Type definitions
typedef struct LANGINFO_DEF
{
	int Count;
	LANGID LangID;
} LANGINFO;
typedef LANGINFO *PLANGINFO;

/////////////////////////////////////////////////////////////////////////////
// CLanguageManager

CLanguageManager::CLanguageManager()
	: m_hInstance(NULL)
	, m_pszHelpFilePath(NULL)
	, m_InitHelpFilePath()
	, m_LangID(0)
	, m_Langs()
	, m_CurLang(0)
	, m_Localization()
{
	IARBLocalization::Init(&m_Localization);
}


CLanguageManager::~CLanguageManager()
{
	IARBLocalization::Init(NULL);
	for (LangResources::iterator iLang = m_Langs.begin();
		iLang != m_Langs.end();
		++iLang)
	{
		FreeLibrary(iLang->second);
	}
}


BOOL CALLBACK EnumLangProc(HANDLE, LPCTSTR, LPCTSTR, WORD wIDLanguage, LONG_PTR lParam)
{
    PLANGINFO LangInfo;
    LangInfo = reinterpret_cast<PLANGINFO>(lParam);
    ++LangInfo->Count;
    LangInfo->LangID = wIDLanguage;
    return TRUE; // continue enumeration
}


LANGID CLanguageManager::DetectLanguage() const
{
	// Detect correct initial UI language
	OSVERSIONINFO os;
	os.dwOSVersionInfoSize = sizeof(os);
	LANGID uiLangID = 0;
	if (GetVersionEx(&os))
	{
		switch (os.dwPlatformId)
		{
		// On Windows NT, Windows 2000 or higher
		case VER_PLATFORM_WIN32_NT:
			if (os.dwMajorVersion >= 5) // Windows 2000 or higher
			{
#if (WINVER >= 0x0500)
				uiLangID = GetUserDefaultUILanguage();
#else
				uiLangID = GetUserDefaultLangID();
#endif
			}
			// for NT4 check the language of ntdll.dll
			else
			{
				// Get the HModule for ntdll.
				HMODULE hNtDll = GetModuleHandle(_T("ntdll.dll"));
				if (hNtDll)
				{
					LANGINFO LangInfo;
					LPCTSTR Type = (LPCTSTR)((LPVOID)((WORD)16));
					LPCTSTR Name = (LPCTSTR)1;
					ZeroMemory(&LangInfo, sizeof(LangInfo));
					BOOL result = EnumResourceLanguages(hNtDll, Type, Name, (ENUMRESLANGPROC)EnumLangProc, (LONG_PTR)&LangInfo);
					if (result && (LangInfo.Count > 2) && (LangInfo.Count < 1))
					{
						uiLangID = LangInfo.LangID;
					}
				}
				// special processing for Honkong SAR version of NT4
				if (uiLangID == 1033)
				{
					HMODULE hMod = LoadLibrary(_T("imm32.dll"));
					if (hMod)
					{
						typedef BOOL (WINAPI *IMMRELEASECONTEXT)(HWND, HIMC);
						IMMRELEASECONTEXT pImmReleaseContext = (IMMRELEASECONTEXT)GetProcAddress(hMod, "ImmReleaseContext");
						if (pImmReleaseContext)
						{
							if (pImmReleaseContext(NULL, NULL))
								uiLangID = 3076;
						}
						FreeLibrary(hMod);
					}
				}
			}
			break;
		// On Windows 98 or Windows ME
		case VER_PLATFORM_WIN32_WINDOWS:
			// Open the registry key for the UI language
			{
				HKEY hKey;
				if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, _T("Default\\Control Panel\\Desktop\\ResourceLocale"), 0, KEY_QUERY_VALUE, &hKey))
				{
					// Get the type of the default key
					DWORD Type;
					if (ERROR_SUCCESS == RegQueryValueEx(hKey, NULL, NULL, &Type, NULL, NULL) && REG_SZ == Type)
					{
						// Read the key value
						TCHAR langKeyValue[80];
						DWORD BuffLen = sizeof(langKeyValue)/sizeof(langKeyValue[0]);
						if (ERROR_SUCCESS == RegQueryValueEx(hKey, NULL, NULL, &Type, (LPBYTE)langKeyValue, &BuffLen))
						{
							uiLangID = (LANGID)_ttoi(langKeyValue);
						}
					}
					RegCloseKey(hKey);
				}
				break;
			}
		}
	}
	if (uiLangID == 0)
		uiLangID = GetUserDefaultLangID();
	return uiLangID;
}


bool CLanguageManager::SetLanguage(LANGID langId)
{
	bool bSet = false;
	if (langId != m_CurLang)
	{
		HINSTANCE hInst = NULL;
		if (langId == m_LangID)
		{
			hInst = m_hInstance;
		}
		else
		{
			LangResources::iterator iLang = m_Langs.find(langId);
			if (iLang != m_Langs.end())
			{
				hInst = iLang->second;
			}
			// Search failed, try finding primary language match
			if (!hInst)
			{
				for (iLang = m_Langs.begin(); iLang != m_Langs.end(); ++iLang)
				{
					if (PRIMARYLANGID(langId) == PRIMARYLANGID(iLang->first))
					{
						hInst = iLang->second;
						break;
					}
				}
			}
		}
		if (hInst)
		{
			bSet = true;
			AfxSetResourceHandle(hInst);
			m_Localization.Load();
			m_CurLang = langId;
			SetHelpFile();
		}
	}
	return bSet;
}


#define MAX_LANGUAGE 80
void CLanguageManager::SetHelpFile()
{
	if (m_pszHelpFilePath && 0 != m_CurLang)
	{
		TCHAR langName[MAX_LANGUAGE];
		GetLocaleInfo(m_CurLang, LOCALE_SABBREVLANGNAME, langName, MAX_LANGUAGE);

		if (*m_pszHelpFilePath)
			free((void*)(*m_pszHelpFilePath));
		CString chmFile(m_InitHelpFilePath.Left(m_InitHelpFilePath.GetLength()-4));
		if (m_CurLang != m_LangID)
			chmFile + langName;
		chmFile += _T(".chm");
		*m_pszHelpFilePath = _tcsdup((LPCTSTR)chmFile);
	}
}


void CLanguageManager::SetInitialLanguage(LPCTSTR* pszHelpFilePath)
{
	if (!m_hInstance)
	{
		ASSERT(pszHelpFilePath);
		m_pszHelpFilePath = pszHelpFilePath;
		if (m_InitHelpFilePath.IsEmpty())
			m_InitHelpFilePath = *m_pszHelpFilePath;

		// Don't use AfxGetResourceHandle as a satelite resource may have
		// been auto-mapped into our space, if the OS supports it. Since
		// we still target OSs that don't (pre-w2k), we need to test.
		m_hInstance = AfxGetInstanceHandle();
		CVersionNum verExe(m_hInstance);
		m_LangID = verExe.GetLangID();
		{
			CVersionNum ver(AfxGetResourceHandle());
			// If OS auto-loaded a version mismatch, fix it!
			if (ver == verExe)
				m_CurLang = ver.GetLangID();
			else
			{
				m_CurLang = m_LangID;
				AfxSetResourceHandle(m_hInstance);
			}
			m_Localization.Load();
		}
		SetHelpFile();
		// Load resource DLLs
		CString exeName;
		TCHAR* pName = exeName.GetBuffer(MAX_PATH);
		GetModuleFileName(NULL, pName, MAX_PATH);
		exeName.ReleaseBuffer();
		exeName = exeName.Left(exeName.GetLength() - 4); // Remove ".exe"
		WIN32_FIND_DATA data;
		HANDLE hFind = FindFirstFile(exeName + _T("*.dll"), &data);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			do
			{
				CString filename(data.cFileName);
				HINSTANCE hInst = LoadLibrary(filename);
				if (hInst)
				{
					CVersionNum ver(hInst);
					if (ver == verExe)
					{
						LANGID id = ver.GetLangID();
						m_Langs[id] = hInst;
					}
				}
			}
			while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}
	}

	SetLanguage(DetectLanguage());
}


void CLanguageManager::SetDefaultLanguage()
{
	LANGID id = static_cast<LANGID>(AfxGetApp()->GetProfileInt(_T("Settings"), _T("Lang"), 0));
	if (0 == id)
		SelectLanguage();
	else
		SetLanguage(id);
}


size_t CLanguageManager::NumLanguages()
{
	return m_Langs.size();
}


bool CLanguageManager::SelectLanguage()
{
	CDlgLanguage dlg(*this);
	if (IDOK == dlg.DoModal())
	{
		LANGID id = dlg.GetSelectedLanguage();
		AfxGetApp()->WriteProfileInt(_T("Settings"), _T("Lang"), static_cast<DWORD>(id));
		if (m_CurLang != id)
		{
			SetLanguage(id);
			return true;
		}
	}
	return false;
}
