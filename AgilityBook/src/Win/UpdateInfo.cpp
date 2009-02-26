/*
 * Copyright © 2004-2009 David Connet. All Rights Reserved.
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
 * @li 2009-01-06 DRC Ported to wxWidgets.
 * @li 2008-06-29 DRC When looking for language ids, it was searching the wrong node.
 * @li 2008-01-01 DRC Fix a bug parsing Element data (GetElementNode may return null)
 * @li 2007-08-03 DRC Separated HTTP reading code from UpdateInfo.cpp
 * @li 2005-10-26 DRC Added option to prevent auto-update user query.
 * @li 2005-09-09 DRC Modified URL parsing to handle redirection.
 *                    I can now advertise the url as "www.agilityrecordbook.com"
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-03 DRC Created
 */

#include "stdafx.h"
#include "UpdateInfo.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "Element.h"
#include "LanguageManager.h"
#include "ReadHttp.h"
#include "VersionNum.h"
#include <wx/config.h>

#ifdef _DEBUG
#define USE_LOCAL
#endif

#ifdef USE_LOCAL
#include <wx/textfile.h>
#ifdef WIN32
#define FILENAME	wxT("c:/AgilityBook/www/version.txt")
#else
#define FILENAME	wxT("/AgilityBook/www/version.txt")
#endif
#endif

/////////////////////////////////////////////////////////////////////////////

bool CUpdateInfo::UpdateConfig(
		CAgilityBookDoc* ioDoc,
		wxChar const* inMsg)
{
	wxString msg(_("IDS_UPDATED_CONFIG"));
	if (inMsg && *inMsg)
	{
		msg += wxT("\n\n");
		msg += inMsg;
	}

	bool b = (wxYES == wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO));
	/* TODO: Change to include 'never' option
	if (!b)
	{
		ioDoc->GetConfig().SetUpdateStatus(false);
		ioDoc->Modify(true);
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
	, m_usernameHint(wxT("default"))
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
	m_usernameHint = wxT("default");
	m_CalSiteSuppression.clear();

	// Set the default values.
	m_UpdateDownload = _("LinkArbDownloadUrl");

	// Read the file.
	wxString url(_("IDS_HELP_UPDATE"));
	url += wxT("/version.txt");
	std::string data; // must be 'char' for XML parsing
	wxString errMsg;

#ifdef USE_LOCAL
	wxTextFile file;
	if (file.Open(FILENAME))
	{
		for (size_t line = 0; line < file.GetLineCount(); ++line)
		{
#ifdef UNICODE
			data += tstringUtil::Convert(file[line].c_str());
#else
			data += file[line];
#endif
			data += '\n';
		}
		file.Close();
	}
#else
	CReadHttp file(url, data);
	wxString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
	if (file.ReadHttpFile(userName, errMsg))
		CAgilityBookOptions::SetUserName(m_usernameHint, userName);
	else
	{
		if (bVerbose)
		{
			data = wxString(_("IDS_UPDATE_UNKNOWN")).mb_str(wxMBConvUTF8());
#ifdef UNICODE
			wxString tmp(data.c_str(), wxMBConvUTF8());
			if (!errMsg.IsEmpty())
				tmp += errMsg;
			wxMessageBox(tmp, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
#else
			if (!errMsg.IsEmpty())
				data += errMsg;
			wxMessageBox(data, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
#endif
		}
		return false;
	}
#endif

	// Now parse it into the object.
	wxString tmp(data.c_str(), wxMBConvUTF8());
	// This is a static string in "version.txt"
	static wxString const idStr(wxT("ARB Version "));
	if (0 == tmp.Find(idStr))
	{
		tmp = tmp.Mid(idStr.length());
		m_VersionNum.Parse(wxT("version.txt"), tmp);
	}

	if (!m_VersionNum.Valid())
	{
		if (bVerbose)
		{
			wxMessageBox(_("IDS_UPDATE_UNKNOWN"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
		}
		return false;
	}

	// Skip the first line, that's the version.
	std::string::size_type n = data.find('\n');
	if (std::string::npos != n)
		data = data.substr(n+1);
	else
		data.clear();
	if (!data.empty())
	{
		// The rest of the file is xml:
		/*
		<!ELEMENT Data (Config, Download*, DisableCalPlugin*) >

		<!--
		pcdata is a message that can be displayed to the user explaining
		the changes. This is only used when doing a config update, not a
		version update. [version update is 1st line in file]
		Note: Prior to v1.10, Config had PCDATA. We can change the format
		of the file since this data is never accessed when there's a pgm
		version change.
		-->
		<!ELEMENT Config (Lang+) >
		  <!ATTLIST Config
		    ver CDATA #REQUIRED
		    file CDATA #REQUIRED
		    >
		<!--
		'id' specified the LANGID of the plugin language DLL, '0' is used
		for the program default. (mfc)
		'id2' is gettext lang code. (wxWidgets)
		-->
		<!ELEMENT Lang (#PCDATA) >
		  <!ATTLIST Lang id CDATA #REQUIRED >
		  <!ATTLIST Lang id2 CDATA #REQUIRED >

		<!--
		if Download is not set, defaults to 'LinkArbDownloadUrl',
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
		if (!tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg2))
		{
			if (bVerbose)
			{
				wxString msg = wxString::Format(_("IDS_LOAD_FAILED"), wxT("version.txt"));
				if (0 < errMsg2.length())
				{
					msg += wxT("\n\n");
					msg += errMsg2.c_str();
				}
				wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				// Even tho we failed, we'll still report success.
				// The return code is really whether we loaded the pgm verno.
			}
		}
		else if (tree->GetName() == wxT("Data"))
		{
			for (int nIndex = 0; nIndex < tree->GetElementCount(); ++nIndex)
			{
				ElementNodePtr node = tree->GetElementNode(nIndex);
				if (!node)
					continue;
				if (node->GetName() == wxT("Config"))
				{
					node->GetAttrib(wxT("ver"), m_VerConfig);
					node->GetAttrib(wxT("file"), m_FileName);
					// Note, before v1.10, we did "m_InfoMsg = node->GetValue();"
					// In 1.10, we changed the format of version.txt.
					for (int nLang = 0; nLang < node->GetElementCount(); ++nLang)
					{
						ElementNodePtr lang = node->GetElementNode(nLang);
						if (!lang)
							continue;
						if (lang->GetName() == wxT("Lang"))
						{
							tstring langIdStr;
							lang->GetAttrib(wxT("id2"), langIdStr);
							m_InfoMsg[langIdStr] = lang->GetValue();
						}
					}
				}
				else if (node->GetName() == wxT("Download"))
				{
					m_UpdateDownload = node->GetValue().c_str();
				}
				else if (node->GetName() == wxT("DisableCalPlugin"))
				{
					tstring filename, ver;
					node->GetAttrib(wxT("file"), filename);
					node->GetAttrib(wxT("ver"), ver);
					// The 'enable' attribute is in case we prematurely disable
					bool bEnable = false;
					node->GetAttrib(wxT("enable"), bEnable);
					CVersionNum vernum;
					vernum.Parse(filename.c_str(), ver.c_str());
					if (vernum.Valid())
					{
						CAgilityBookOptions::SuppressCalSitePermanently(filename.c_str(), vernum, !bEnable);
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
		wxConfig::Get()->Write(wxT("Settings/lastVerCheck"), today.GetString(ARBDate::eDashYMD).c_str());
		wxString msg = wxString::Format(_("IDS_VERSION_AVAILABLE"), m_VersionNum.GetVersionString().c_str());
		if (wxYES == wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_QUESTION | wxYES_NO))
		{
			wxString url(m_UpdateDownload);
			wxString suffix = url.Right(4);
			suffix.MakeUpper();
			if (suffix == wxT(".PHP"))
			{
#ifdef WIN32
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
							url += wxT("?os=x64");
						else
#endif
							url += wxT("?os=win");
					}
					break;
				case VER_PLATFORM_WIN32_WINDOWS: // Win95/98/Me
					url += wxT("?os=win98");
					break;
				}
#else
#pragma message PRAGMA_MESSAGE("TODO: Add 'os' tag for URL download")
				// @todo Add appropriate 'os' tag for other OS's
				// Must agree with website's download.php
#endif
			}
			wxLaunchDefaultBrowser(url);
		}
	}
	else
		wxConfig::Get()->Write(wxT("Settings/lastVerCheck"), today.GetString(ARBDate::eDashYMD).c_str());
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
	CVersionNum verThis;
	assert(verThis.Valid());
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
	if (0 < m_FileName.length() && m_VerConfig > pDoc->Book().GetConfig().GetVersion())
	{
		bUpToDate = false;
		wxString msg;
		if (0 < m_InfoMsg.size())
		{
			std::map<tstring, tstring>::iterator iMsg = m_InfoMsg.find(langMgr.CurrentLanguage());
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
			wxString url = _("IDS_HELP_UPDATE");
			url += wxT("/");
			url += m_FileName.c_str();
			std::string strConfig;
			wxString errMsg;
			CReadHttp file(url, &strConfig);
			wxString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
			if (file.ReadHttpFile(m_usernameHint, errMsg))
			{
				CAgilityBookOptions::SetUserName(m_usernameHint, userName);
				ElementNodePtr tree(ElementNode::New());
				tstring errMsg2;
				if (!tree->LoadXMLBuffer(strConfig.c_str(), strConfig.length(), errMsg2))
				{
					wxString msg = wxString::Format(_("IDS_LOAD_FAILED"), url.c_str());
					if (0 < errMsg2.length())
					{
						msg += wxT("\n\n");
						msg += errMsg2.c_str();
					}
					wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				}
				else if (tree->GetName() == wxT("DefaultConfig"))
				{
					strConfig.clear();
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
								wxMessageBox(err.m_ErrMsg.c_str(), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
						}
						else
						{
							pDoc->ImportConfiguration(book.GetConfig());
							pDoc->Modify(true);
						}
					}
				}
			}
		}
	}
	if (bUpToDate && bVerbose)
	{
		wxMessageBox(_("IDS_UPDATE_CURRENT"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
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
