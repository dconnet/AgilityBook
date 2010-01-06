/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
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
 * --- as of v2, the real version isn't here - see below.
 *
 * Revision History
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-26 DRC Removed Win98 support.
 * @li 2009-07-18 DRC Updated version.txt to support different versions/platforms.
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
#include <wx/filesys.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
static wxString FILENAME()
{
#ifdef WIN32
	// Why this? Just cause that's how my local system is setup.
	return wxT("c:/AgilityBook/www/version.txt");
#else
	return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + wxT("version.txt");
#endif
}
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
	: m_VersionNum(false)
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
	if (file.Open(FILENAME()))
	{
		for (size_t line = 0; line < file.GetLineCount(); ++line)
		{
			data += file[line].ToUTF8();
			data += '\n';
		}
		file.Close();
	}
#else
	CReadHttp file(url, &data);
	wxString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
	if (file.ReadHttpFile(userName, errMsg, wxGetApp().GetTopWindow()))
		CAgilityBookOptions::SetUserName(m_usernameHint, userName);
	else
	{
		if (bVerbose)
		{
			data = wxString(_("IDS_UPDATE_UNKNOWN")).mb_str(wxMBConvUTF8());
			wxString tmp = tstringUtil::TString(data);
			if (!errMsg.IsEmpty())
				tmp += errMsg;
			wxMessageBox(tmp, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
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
		m_VersionNum.Parse(tmp);
	}

	// We'll always leave a valid version on line 1.
	// As of v2, that will be the current windows version (so v1 programs
	// can correctly show the new version number to the user). The real
	// version will be in the 'Config' object below.
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
		data.erase();
	if (!data.empty())
	{
		// The rest of the file is xml:
		/*
		<!ELEMENT Data (Config+, Download*, DisableCalPlugin*) >

		<!--
		Before v1.10:
		pcdata is a message that can be displayed to the user explaining
		the changes. This is only used when doing a config update, not a
		version update. [version update is 1st line in file]
		v1.10: Moved PCDATA to 'lang' (We can change the format of the file
		since this data is never accessed when there's a pgm version change.)
		v2.0: Allow multiple copies of 'Config' for different platforms.
		If 'platform' is not set, it's the default. This allows creating
		one entry with 'mac', one with none - then all non-macs default to
		second. Current recognized values: 'win'/'mac'
		v2.0: Added 'version' to 'Config': This replaces the version in the
		first line (if not present, line 1 is used).
		-->
		<!ELEMENT Config (Lang+) >
		  <!ATTLIST Config
		    platform CDATA #IMPLIED
			version CDATA #IMPLIED
		    ver CDATA #REQUIRED
		    file CDATA #REQUIRED
		    >
		<!--
		'id' specified the LANGID of the plugin language DLL, '0' is used
		for the program default.
		v2.0: 'id2' is gettext lang code, id no longer used.
		-->
		<!ELEMENT Lang (#PCDATA) >
		  <!-- <!ATTLIST Lang id CDATA #REQUIRED > -->
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
		wxString errMsg2;
		ElementNodePtr tree(ElementNode::New());
		if (!tree->LoadXMLBuffer(data.c_str(), data.length(), errMsg2))
		{
			if (bVerbose)
			{
				wxString msg = wxString::Format(_("IDS_LOAD_FAILED"), wxT("version.txt"));
				if (0 < errMsg2.length())
				{
					msg += wxT("\n\n");
					msg += errMsg2;
				}
				wxMessageBox(msg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				// Even tho we failed, we'll still report success.
				// The return code is really whether we loaded the pgm verno.
			}
		}
		else if (tree->GetName() == wxT("Data"))
		{
			bool bConfigLoaded = false;
			for (int nIndex = 0; nIndex < tree->GetElementCount(); ++nIndex)
			{
				ElementNodePtr node = tree->GetElementNode(nIndex);
				if (!node)
					continue;
				if (!bConfigLoaded && node->GetName() == wxT("Config"))
				{
					wxString value;
					// If not set, all platforms are the same. This 
					if (ElementNode::eFound == node->GetAttrib(wxT("platform"), value))
					{
#ifdef WIN32
						if (value != wxT("win"))
						{
							continue; 
						}
#elif defined(__WXMAC__)
						if (value != wxT("mac"))
						{
							continue;
						}
#else
						// Unknown platform
						continue;
#endif
					}
					if (ElementNode::eFound == node->GetAttrib(wxT("version"), value))
					{
						m_VersionNum.Parse(value);
						if (!m_VersionNum.Valid())
						{
							if (bVerbose)
							{
								wxMessageBox(_("IDS_UPDATE_UNKNOWN"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
							}
							return false;
						}
					}
					bConfigLoaded = true;
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
							wxString langIdStr;
							lang->GetAttrib(wxT("id2"), langIdStr);
							m_InfoMsg[langIdStr] = lang->GetValue();
						}
					}
				}
				else if (node->GetName() == wxT("Download"))
				{
					m_UpdateDownload = node->GetValue();
				}
				else if (node->GetName() == wxT("DisableCalPlugin"))
				{
					wxString filename, ver;
					node->GetAttrib(wxT("file"), filename);
					node->GetAttrib(wxT("ver"), ver);
					// The 'enable' attribute is in case we prematurely disable
					bool bEnable = false;
					node->GetAttrib(wxT("enable"), bEnable);
					CVersionNum vernum(false);
					vernum.Parse(ver);
					if (vernum.Valid())
					{
						CAgilityBookOptions::SuppressCalSitePermanently(filename, vernum, !bEnable);
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
bool CUpdateInfo::CheckProgram(wxString const& lang)
{
	bool bNeedsUpdating = false;
	ARBDate today = ARBDate::Today();
	if (IsOutOfDate())
	{
		bNeedsUpdating = true;
		wxConfig::Get()->Write(wxT("Settings/lastVerCheck"), today.GetString(ARBDate::eISO));
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
						if (!lang.empty())
						{
							url += wxT("-");
							url += lang;
						}
					}
					break;
				}
#elif defined(__WXMAC__)
// comments from include/wx/platform.h
//__WXMAC__
//    __WXMAC_CLASSIC__ means ppc non-carbon builds,
//    __WXMAC_CARBON__ means carbon API available (mach or cfm builds),
//    __WXMAC_OSX__ means mach-o builds, running under 10.2 + only
//
//__WXOSX__ is a common define to wxMac (Carbon) and wxCocoa ports under OS X.
				// We currently compile for Universal OSX 10.4. At this time,
				// there's no need to further determine the OS.
				url += wxT("?os=mac");
#else
#pragma PRAGMA_TODO("Add 'os' tag for URL download")
				// @todo Add appropriate 'os' tag for other OS's
				// Must agree with website's download.php
#endif
			}
			wxLaunchDefaultBrowser(url);
		}
	}
	else
		wxConfig::Get()->Write(wxT("Settings/lastVerCheck"), today.GetString(ARBDate::eISO));
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
	CVersionNum verThis(true);
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
			std::map<wxString, wxString>::iterator iMsg = m_InfoMsg.find(langMgr.CurrentLanguage());
			if (iMsg == m_InfoMsg.end())
				iMsg = m_InfoMsg.begin();
			if (iMsg != m_InfoMsg.end() && 0 < iMsg->second.length())
			{
				// If the info contains a note, append it.
				// A note will often give a brief description of things
				// the user must do. For instance, from v4->5, USDAA
				// titling pts were removed from Tournament Jumpers and
				// Snooker to allow for non-titling runs. In case the
				// user saved some that way, we need to warn them.
				msg += iMsg->second;
			}
		}
		if (UpdateConfig(pDoc, msg))
		{
			// Load the config.
			wxString url = _("IDS_HELP_UPDATE");
			url += wxT("/");
			url += m_FileName;
			std::string strConfig;
			wxString errMsg;
			CReadHttp file(url, &strConfig);
			wxString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
			if (file.ReadHttpFile(m_usernameHint, errMsg, wxGetApp().GetTopWindow()))
			{
				CAgilityBookOptions::SetUserName(m_usernameHint, userName);
				ElementNodePtr tree(ElementNode::New());
				wxString errMsg2;
				if (!tree->LoadXMLBuffer(strConfig.c_str(), strConfig.length(), errMsg2))
				{
					wxString msg2 = wxString::Format(_("IDS_LOAD_FAILED"), url.c_str());
					if (0 < errMsg2.length())
					{
						msg2 += wxT("\n\n");
						msg2 += errMsg2;
					}
					wxMessageBox(msg2, wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
				}
				else if (tree->GetName() == wxT("DefaultConfig"))
				{
					strConfig.erase();
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
								wxMessageBox(err.m_ErrMsg, wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
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


void CUpdateInfo::AutoUpdateProgram(
		CLanguageManager const& langMgr)
{
	if (ReadVersionFile(false))
		CheckProgram(langMgr.CurrentLanguage());
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
	if (CheckProgram(langMgr.CurrentLanguage()))
		return;
	CheckConfig(pDoc, langMgr, true);
}
