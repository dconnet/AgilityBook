/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @brief Parse and cache "version.txt" on www.agilityrecordbook.com
 * @author David Connet
 *
 * The format of the version.txt file is:
 * line 1:
 *  "ARB Version n1.n2.n3.n4"
 * line 2-n: xml (see below)
 *
 * Revision History
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 * @li 2005-10-26 DRC Added option to prevent auto-update user query.
 * @li 2005-09-09 DRC Modified URL parsing to handle redirection.
 *                    I can now advertise the url as "www.agilityrecordbook.com"
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-03 DRC Created
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "UpdateInfo.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "Element.h"
#include "HyperLink.h"
#include "ReadHttp.h"
#include "Splash.h"
#include "VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

bool CUpdateInfo::UpdateConfig(
		CAgilityBookDoc* ioDoc,
		TCHAR const* inMsg)
{
	CSplashWnd::HideSplashScreen();

	CString msg;
	msg.LoadString(IDS_UPDATED_CONFIG);
	if (inMsg && *inMsg)
	{
		msg += _T("\n\n");
		msg += inMsg;
	}

	bool b = (IDYES == AfxMessageBox(msg, MB_ICONQUESTION | MB_YESNO));
	/* TODO: Change to include 'never' option
	if (!b)
	{
		ioDoc->GetConfig().SetUpdateStatus(false);
		ioDoc->SetModifiedFlag(TRUE);
	}
	*/
	return b;
}

/////////////////////////////////////////////////////////////////////////////

CUpdateInfo::CUpdateInfo()
	: m_VersionNum()
	, m_VerConfig(0)
	, m_FileName()
	, m_InfoMsg()
	, m_UpdateDownload()
	, m_usernameHint(_T("default"))
	, m_CalSiteSuppression()
{
}


/**
 * This will read the version.txt file and cache it.
 * In addition, it will ask to update if a newer version is found.
 * @param bVerbose Show error message.
 */
bool CUpdateInfo::ReadVersionFile(bool bVerbose)
{
	// Clear everything.
	m_VersionNum.clear();
	m_VerConfig = 0;
	m_FileName.erase();
	m_InfoMsg.clear();
	m_UpdateDownload.Empty();
	m_usernameHint = _T("default");
	m_CalSiteSuppression.clear();

	// Set the default values.
	m_UpdateDownload.LoadString(IDS_ABOUT_LINK_ARB_DOWNLOAD);
	int nTab = m_UpdateDownload.Find('\t');
	if (0 < nTab)
		m_UpdateDownload = m_UpdateDownload.Mid(nTab+1);

	// Read the file.
	CString url;
	url.LoadString(IDS_HELP_UPDATE);
	url += _T("/version.txt");
	CStringA data;
	CString errMsg;

	CReadHttp file(url, data);
	CString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
	if (file.ReadHttpFile(userName, errMsg))
		CAgilityBookOptions::SetUserName(m_usernameHint, userName);
	else
	{
		if (bVerbose)
		{
			CSplashWnd::HideSplashScreen();
			data.LoadString(IDS_UPDATE_UNKNOWN);
#ifdef UNICODE
			CString tmp(data);
			if (!errMsg.IsEmpty())
				tmp += errMsg;
			AfxMessageBox(tmp, MB_ICONEXCLAMATION);
#else
			if (!errMsg.IsEmpty())
				data += errMsg;
			AfxMessageBox(data, MB_ICONEXCLAMATION);
#endif
		}
		return false;
	}
	file.Close();

	// Now parse it into the object.
	CString tmp(data);
	// This is a static string in "version.txt"
	static CString const idStr(_T("ARB Version "));
	if (0 == tmp.Find(idStr))
	{
		tmp = tmp.Mid(idStr.GetLength());
		m_VersionNum.Parse(_T("version.txt"), tmp);
	}

	if (!m_VersionNum.Valid())
	{
		if (bVerbose)
		{
			CSplashWnd::HideSplashScreen();
			AfxMessageBox(IDS_UPDATE_UNKNOWN, MB_ICONEXCLAMATION);
		}
		return false;
	}

	// Skip the first line, that's the version.
	int n = data.Find('\n');
	if (0 < n)
		data = data.Mid(n+1);
	else
		data.Empty();
	if (!data.IsEmpty())
	{
		// The rest of the file is xml:
		/*
		<!ELEMENT Data (Config, Download*, DisableCalPlugin*) >

		<!--
		pcdata is a message that can be displayed to the user explaining
		the changes. This is only used when doing a config update, not a
		version update. [version update is 1st line in file]
		Note: 'Config' is maintained only for back-compatibility with older
		versions of the program. 'Config2' is used to support languages.
		-->
		<!ELEMENT Config (#PCDATA) >
		  <!ATTLIST Config
		    ver CDATA #REQUIRED
		    file CDATA #REQUIRED
		    >
		<!ELEMENT Config2 (Lang+) >
		  <!ATTLIST Config2
		    ver CDATA #REQUIRED
		    file CDATA #REQUIRED
		    >
		<!--
		'id' specified the LANGID of the plugin language DLL, '0' is used
		for the program default.
		-->
		<!ELEMENT Lang (#PCDATA) >
		  <!ATTLIST Lang id CDATA #REQUIRED >

		<!--
		if Download is not set, defaults to IDS_ABOUT_LINK_ARB_DOWNLOAD,
		which is http://www.agilityrecordbook.com/download.php
		-->
		<!ELEMENT Download (#PCDATA) >

		<!--
		When we know a calendar plugin is obsolete due to website changes,
		use this to force the plugin to disable. Only disables the specified
		version. If we prematurely disabled the plugin, the 'enable' flag
		will reenable. [disabling info is stored in the registry]
		-->
		<!ELEMENT DisableCalPlugin EMPTY >
		  <!ATTLIST DisableCalPlugin
		    file CDATA #REQUIRED
		    ver CDATA #REQUIRED
		    enable (0|1) '1'
		    >
		*/
		tstring errMsg2;
		ElementNodePtr tree(ElementNode::New());
		if (!tree->LoadXMLBuffer((LPCSTR)data, data.GetLength(), errMsg2))
		{
			if (bVerbose)
			{
				CString msg;
				msg.FormatMessage(IDS_LOAD_FAILED, _T("version.txt"));
				if (0 < errMsg2.length())
				{
					msg += _T("\n\n");
					msg += errMsg2.c_str();
				}
				CSplashWnd::HideSplashScreen();
				AfxMessageBox(msg, MB_ICONEXCLAMATION);
				// Even tho we failed, we'll still report success.
				// The return code is really whether we loaded the pgm verno.
			}
		}
		else if (tree->GetName() == _T("Data"))
		{
			for (int nIndex = 0; nIndex < tree->GetElementCount(); ++nIndex)
			{
				ElementNodePtr node = tree->GetElementNode(nIndex);
				// Ignore 'Config'
				if (node->GetName() == _T("Config2"))
				{
					node->GetAttrib(_T("ver"), m_VerConfig);
					node->GetAttrib(_T("file"), m_FileName);
					for (int nLang = 0; nLang < node->GetElementCount(); ++nLang)
					{
						ElementNodePtr lang = node->GetElementNode(nLang);
						if (lang->GetName() == _T("Lang"))
						{
							tstring langIdStr;
							node->GetAttrib(_T("id"), langIdStr);
							LANGID langId = static_cast<LANGID>(_tcstol(langIdStr.c_str(), NULL, 16));
							m_InfoMsg[langId] = lang->GetValue();
						}
					}
				}
				else if (node->GetName() == _T("Download"))
				{
					m_UpdateDownload = node->GetValue().c_str();
				}
				else if (node->GetName() == _T("DisableCalPlugin"))
				{
					tstring file, ver;
					node->GetAttrib(_T("file"), file);
					node->GetAttrib(_T("ver"), ver);
					// The 'enable' attribute is in case we prematurely disable
					bool bEnable = false;
					node->GetAttrib(_T("enable"), bEnable);
					CVersionNum vernum;
					vernum.Parse(file.c_str(), ver.c_str());
					if (vernum.Valid())
					{
						CAgilityBookOptions::SuppressCalSitePermanently(file.c_str(), vernum, !bEnable);
					}
				}
			}
		}
	}

	return true;
}


/**
 * Check the version against the web.
 */
bool CUpdateInfo::CheckProgram()
{
	bool bNeedsUpdating = false;
	ARBDate today = ARBDate::Today();
	if (IsOutOfDate())
	{
		bNeedsUpdating = true;
		CSplashWnd::HideSplashScreen();
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("lastVerCheck"), today.GetString(ARBDate::eDashYMD).c_str());
		CString msg;
		msg.FormatMessage(IDS_VERSION_AVAILABLE, (LPCTSTR)m_VersionNum.GetVersionString());
		if (IDYES == AfxMessageBox(msg, MB_ICONQUESTION | MB_YESNO))
		{
			CString url(m_UpdateDownload);
			CString suffix = url.Right(4);
			suffix.MakeUpper();
			if (suffix == ".PHP")
			{
				OSVERSIONINFO os;
				os.dwOSVersionInfoSize = sizeof(os);
				GetVersionEx(&os);
				switch (os.dwPlatformId)
				{
				default:
				case VER_PLATFORM_WIN32_NT: // NT/Win2000/XP/Vista
					{
#if _MSC_VER >= 1300
						SYSTEM_INFO info;
						GetSystemInfo(&info);
						if (PROCESSOR_ARCHITECTURE_AMD64 == info.wProcessorArchitecture)
							url += "?os=x64";
						else
#endif
							url += "?os=win";
					}
					break;
				case VER_PLATFORM_WIN32_WINDOWS: // Win95/98/Me
					url += "?os=win98";
					break;
				}
			}
			CHyperLink::GotoURL(url);
		}
	}
	else
		AfxGetApp()->WriteProfileString(_T("Settings"), _T("lastVerCheck"), today.GetString(ARBDate::eDashYMD).c_str());
	return bNeedsUpdating;
}


/**
 * Check the version against the web.
 */
bool CUpdateInfo::IsOutOfDate()
{
	// If we haven't parsed the internet file yet, assume we're out-of-date.
	if (!m_VersionNum.Valid())
		return true;
	CVersionNum verThis(NULL); // This auto-loads the versioninfo.
	ASSERT(verThis.Valid());
	if (verThis < m_VersionNum)
		return true;
	else
		return false;
}


void CUpdateInfo::CheckConfig(
		CAgilityBookDoc* pDoc,
		CLanguageManager const& langMgr,
		bool bVerbose)
{
	// If the parse was successful, check for the posted config version.
	bool bUpToDate = true;
	// Cool! New config!
	if (0 < m_FileName.length() && m_VerConfig > pDoc->GetConfig().GetVersion())
	{
		bUpToDate = false;
		CString msg;
		if (0 < m_InfoMsg.size())
		{
			std::map<LANGID, tstring>::iterator iMsg = m_InfoMsg.find(langMgr.CurrentLanguage());
			if (iMsg == m_InfoMsg.end())
				iMsg = m_InfoMsg.find(0);
			if (iMsg != m_InfoMsg.end() && 0 < iMsg->second.length())
			{
				// If the info contains a note, append it.
				// A note will often give a brief description of things
				// the user must do. For instance, from v4->5, USDAA
				// titling pts were removed from Tournament Jumpers and
				// Snooker to allow for non-titling runs. In case the
				// user saved some that way, we need to warn them.
				msg += iMsg->second.c_str();
			}
		}
		if (UpdateConfig(pDoc, msg))
		{
			// Load the config.
			CString url;
			url.LoadString(IDS_HELP_UPDATE);
			url += _T("/");
			url += m_FileName.c_str();
			CStringA strConfig;
			CString errMsg;
			CReadHttp file(url, strConfig);
			CString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
			if (file.ReadHttpFile(m_usernameHint, errMsg))
			{
				CAgilityBookOptions::SetUserName(m_usernameHint, userName);
				file.Close();
				ElementNodePtr tree(ElementNode::New());
				tstring errMsg2;
				if (!tree->LoadXMLBuffer((LPCSTR)strConfig, strConfig.GetLength(), errMsg2))
				{
					msg.FormatMessage(IDS_LOAD_FAILED, (LPCTSTR)url);
					if (0 < errMsg2.length())
					{
						msg += _T("\n\n");
						msg += errMsg2.c_str();
					}
					AfxMessageBox(msg, MB_ICONEXCLAMATION);
				}
				else if (tree->GetName() == _T("DefaultConfig"))
				{
					strConfig.Empty();
					ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
					tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
					int nConfig = tree->FindElement(TREE_CONFIG);
					if (0 <= nConfig)
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						if (!book.GetConfig().Load(tree->GetElementNode(nConfig), version, err))
						{
							if (0 < err.m_ErrMsg.length())
								AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
						}
						else
						{
							pDoc->ImportConfiguration(book.GetConfig());
							pDoc->SetModifiedFlag(TRUE);
						}
					}
				}
			}
		}
	}
	if (bUpToDate && bVerbose)
	{
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(IDS_UPDATE_CURRENT, MB_ICONINFORMATION);
	}
}


void CUpdateInfo::AutoUpdateProgram()
{
	if (ReadVersionFile(false))
		CheckProgram();
}


void CUpdateInfo::AutoCheckConfiguration(
		CAgilityBookDoc* pDoc,
		CLanguageManager const& langMgr)
{
	// If we're opening a doc and we've checked the internet
	// and there is a more current version, do not continue
	// to check the config. This prevents loading a config that
	// uses a newer file version.
	if (IsOutOfDate())
		return;
	CheckConfig(pDoc, langMgr, false);
}


void CUpdateInfo::UpdateConfiguration(
		CAgilityBookDoc* pDoc,
		CLanguageManager const& langMgr)
{
	// Only continue if we parsed the version.txt file
	// AND the version is up-to-date.
	if (!ReadVersionFile(true))
		return;
	if (CheckProgram())
		return;
	CheckConfig(pDoc, langMgr, true);
}
