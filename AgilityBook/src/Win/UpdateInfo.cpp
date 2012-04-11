/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Parse and cache "version2.xml" on www.agilityrecordbook.com
 * @author David Connet
 *
 * File Format: See below.
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2011-07-26 DRC Moved 'Arch' to ARBAgilityRecordBook.
 * @li 2010-05-07 DRC Removed 'lang' from 'Platform'. (Finally figured out how
 *                    to create transforms and embed in msi.)
 * @li 2010-02-07 DRC Changed to version2.xml.
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

/*
 * (post v2.1.4): Changed to version2.xml.
 *  See comments in code (where it's parsed).
 *
 * [v2.1.4 and earlier: version.txt]
 * line 1:
 *  "ARB Version n1.n2.n3.n4"
 * --- as of v2, the real version isn't here - it's in 'Config'
 * line 2-n: xml
 *  <!ELEMENT Data (Config+, Download*, DisableCalPlugin*) >
 *  <!--
 *  Before v1.10:
 *  pcdata is a message that can be displayed to the user explaining
 *  the changes. This is only used when doing a config update, not a
 *  version update. [version update is 1st line in file]
 *  v1.10: Moved PCDATA to 'lang' (We can change the format of the file
 *  since this data is never accessed when there's a pgm version change.)
 *  v2.0: Allow multiple copies of 'Config' for different platforms.
 *  If 'platform' is not set, it's the default. This allows creating
 *  one entry with 'mac', one with none - then all non-macs default to
 *  second. Current recognized values: 'win'/'mac'
 *  v2.0: Added 'version' to 'Config': This replaces the version in the
 *  first line (if not present, line 1 is used).
 *  -->
 *  <!ELEMENT Config (Lang+) >
 *    <!ATTLIST Config
 *      platform CDATA #IMPLIED
 *      version CDATA #IMPLIED
 *      ver CDATA #REQUIRED
 *      file CDATA #REQUIRED
 *      >
 *  <!--
 *  'id' specified the LANGID of the plugin language DLL, '0' is used
 *  for the program default.
 *  v2.0: 'id2' is gettext lang code, id no longer used.
 *  -->
 *  <!ELEMENT Lang (#PCDATA) >
 *    <!-- <!ATTLIST Lang id CDATA #REQUIRED > -->
 *    <!ATTLIST Lang id2 CDATA #REQUIRED >
 *  <!--
 *  if Download is not set, defaults to 'LinkArbDownloadUrl',
 *  which is http://www.agilityrecordbook.com/download.php
 *  -->
 *  <!ELEMENT Download (#PCDATA) >
 *  <!--
 *  When we know a calendar plugin is obsolete due to website changes,
 *  use this to force the plugin to disable. Only disables the specified
 *  version. If we prematurely disabled the plugin, the 'enable' flag
 *  will reenable. [disabling info is stored in the registry]
 *  -->
 *  <!ELEMENT DisableCalPlugin EMPTY >
 *    <!ATTLIST DisableCalPlugin
 *      file CDATA #REQUIRED
 *      ver CDATA #REQUIRED
 *      enable (0|1) '1'
 *      >
 */

#include "stdafx.h"
#include "UpdateInfo.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBMsgDigest.h"
#include "ConfigHandler.h"
#include "DlgProgress.h"
#include "Element.h"
#include "LanguageManager.h"
#include "ReadHttp.h"
#include "RegItems.h"
#include "VersionNum.h"
#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/filesys.h>
#include <wx/stdpaths.h>
#include <wx/wfstream.h>
#include <wx/zipstrm.h>

#ifdef _DEBUG
#define USE_LOCAL
#endif

#ifdef USE_LOCAL
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

static wxString VersionFile()
{
	return wxT("version2.xml");
}


#if defined(__WXMSW__)
static wxString ARBUpdater()
{
	return wxT("ARBUpdater.exe");
}


static wxString UpdateFile()
{
	return wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + ARBUpdater();
}
#endif


#ifdef USE_LOCAL
static wxString FILENAME()
{
#ifdef __WXMSW__
	// Why this? Just cause that's how my local system is setup.
	return wxT("c:/dcon/www/agilityrecordbook/") + VersionFile();
#else
	return wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + VersionFile();
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

	bool b = (wxYES == wxMessageBox(msg, wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_QUESTION));
	/* TODO: Change to include 'never' option
	if (!b)
	{
		ioDoc->GetConfig().SetUpdateStatus(false);
		ioDoc->Modify(true);
	}
	*/
	return b;
}


void CUpdateInfo::CleanupUpdate()
{
#if defined(__WXMSW__)
	wxString updateFile = UpdateFile();
	if (wxFileName::FileExists(updateFile))
	{
		if (!::wxRemoveFile(updateFile))
		{
			// If we get here really quick, the updater may not have finished
			// yet. Go to the Win API now.
			Sleep(3000);
			if (!DeleteFile(updateFile.wx_str()))
			{
				// Ok, delete failed after 3 seconds. Punt (quietly).
				MoveFileEx(updateFile.wx_str(), NULL, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////

CUpdateInfo::CUpdateInfo()
	: m_VersionNum(false)
	, m_VerConfig(0)
	, m_size(0)
	, m_md5()
	, m_NewFile()
	, m_ConfigFileName()
	, m_InfoMsg()
	, m_UpdateDownload()
	, m_usernameHint(wxT("default"))
	, m_CalSiteSuppression()
{
}


/**
 * This will read the version2.xml file and cache it.
 * In addition, it will ask to update if a newer version is found.
 * @param bVerbose Show error message.
 * @param langMgr Language manager.
 */
bool CUpdateInfo::ReadVersionFile(
		bool bVerbose,
		CLanguageManager const& langMgr)
{
	// Clear everything.
	m_VersionNum.clear();
	m_VerConfig = 0;
	m_size = 0;
	m_md5.erase();
	m_NewFile.erase();
	m_ConfigFileName.erase();
	m_InfoMsg.clear();
	m_UpdateDownload.Empty();
	m_usernameHint = wxT("default");
	m_CalSiteSuppression.clear();

	// Set the default values.
	m_UpdateDownload = _("LinkArbDownloadUrl");

	// Read the file.
	wxString url(_("IDS_HELP_UPDATE"));
	url += wxT("/") + VersionFile();
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
			wxString tmp = StringUtil::stringWX(data);
			if (!errMsg.IsEmpty())
				tmp += errMsg;
			wxMessageBox(tmp, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
		}
		return false;
	}
#endif

	bool bLoadedVersion = false;
	if (!data.empty())
	{
		/*
		 * <!ELEMENT Data (Platform, Config, Download*, DisableCalPlugin*) >
		 * <!ELEMENT Platform (EMPTY) >
		 *   <!ATTLIST Platform
		 *     arch CDATA (x86,x64,mac)
		 *     ver CDATA #REQUIRED
		 *     config CDATA #REQUIRED
		 *     md5 CDATA #REQUIRED
		 *     file CDATA #REQUIRED >
		 * <!ELEMENT Config (Lang+) >
		 *   <!ATTLIST Config
		 *     ver CDATA #REQUIRED
		 *     file CDATA #REQUIRED
		 *     >
		 * <!ELEMENT Lang (#PCDATA) >
		 *   <!ATTLIST Lang id2 CDATA #REQUIRED >
		 *  <!--
		 *  if Download is not set, defaults to 'LinkArbDownloadUrl',
		 *  which is http://www.agilityrecordbook.com/download.php
		 *  -->
		 *  <!ELEMENT Download (#PCDATA) >
		 *  <!--
		 *  When we know a calendar plugin is obsolete due to website changes,
		 *  use this to force the plugin to disable. Only disables the specified
		 *  version. If we prematurely disabled the plugin, the 'enable' flag
		 *  will reenable. [disabling info is stored in the registry]
		 *  -->
		 *  <!ELEMENT DisableCalPlugin EMPTY >
		 *    <!ATTLIST DisableCalPlugin
		 *      file CDATA #REQUIRED
		 *      ver CDATA #REQUIRED
		 *      enable (0|1) '1'
		 *      >
		 */
		wxString errMsg2;
		ElementNodePtr tree(ElementNode::New());
		if (!tree->LoadXML(data.c_str(), data.length(), errMsg2))
		{
			if (bVerbose)
			{
				wxString msg = wxString::Format(_("IDS_LOAD_FAILED"), VersionFile().c_str());
				if (0 < errMsg2.length())
				{
					msg += wxT("\n\n");
					msg += errMsg2;
				}
				wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
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
				if (!bLoadedVersion && node->GetName() == wxT("Platform"))
				{
					bool bSkip = true;
					wxString value;
					if (ElementNode::eFound == node->GetAttrib(wxT("arch"), value))
					{
						if (ARBAgilityRecordBook::GetArch() == value)
						{
							bSkip = false;
						}
					}
					// Wrong architecture
					if (bSkip)
						continue;
					if (ElementNode::eFound == node->GetAttrib(wxT("ver"), value))
					{
						m_VersionNum.Parse(value);
						if (m_VersionNum.Valid())
							bLoadedVersion = true;
					}
					if (ElementNode::eFound != node->GetAttrib(wxT("config"), m_VerConfig)
					|| ElementNode::eFound != node->GetAttrib(wxT("size"), m_size)
					|| ElementNode::eFound != node->GetAttrib(wxT("md5"), m_md5)
					|| ElementNode::eFound != node->GetAttrib(wxT("file"), m_NewFile))
					{
						bLoadedVersion = false;
					}
					if (!bLoadedVersion)
					{
						if (bVerbose)
						{
							wxMessageBox(_("IDS_UPDATE_UNKNOWN"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
						}
						return false;
					}
				}
				else if (!bConfigLoaded && node->GetName() == wxT("Config"))
				{
					short config;
					node->GetAttrib(wxT("ver"), config);
					if (config == m_VerConfig)
					{
						wxString value;
						bConfigLoaded = true;
						node->GetAttrib(wxT("file"), m_ConfigFileName);
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

	return bLoadedVersion;
}


/**
 * Check the version against the web.
 */
bool CUpdateInfo::CheckProgram(
		CAgilityBookDoc* pDoc,
		wxString const& lang,
		bool& outClose)
{
	outClose = false;
	bool bNeedsUpdating = false;
	ARBDate today = ARBDate::Today();
	if (IsOutOfDate())
	{
		bNeedsUpdating = true;
		if (pDoc && !pDoc->OnSaveModified())
		{
			// Return that it needs updating, but don't record that we checked.
			return bNeedsUpdating;
		}
		wxConfig::Get()->Write(CFG_SETTINGS_LASTVERCHECK, today.GetString(ARBDate::eISO).c_str());
		wxString msg = wxString::Format(_("IDS_VERSION_AVAILABLE"), m_VersionNum.GetVersionString().c_str());
		if (wxYES == wxMessageBox(msg, wxMessageBoxCaptionStr, wxYES_NO | wxCENTRE | wxICON_QUESTION))
		{
			bool bGotoWeb = false;
			if (m_NewFile.empty())
				bGotoWeb = true;
			else
			{
				wxFileName name(m_NewFile);
				wxString filename;
#if defined(__WXMAC__)
				wxFileDialog dlg(wxGetApp().GetTopWindow(),
					wxFileSelectorPromptStr,
					wxStandardPaths::Get().GetDocumentsDir(),
					name.GetFullName(),
					wxFileSelectorDefaultWildcardStr,
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == dlg.ShowModal())
				{
					filename = dlg.GetPath();
				}
				else
				{
					bGotoWeb = true;
				}
#elif defined(__WXMSW__)
				filename = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + name.GetFullName();
#endif
				if (!bGotoWeb)
				{
					wxString errMsg;
					wxFileOutputStream output(filename);
					if (!output.IsOk())
					{
						bGotoWeb = true;
						errMsg = wxString::Format(_("IDS_CANNOT_OPEN"), filename.c_str());
					}
					else
					{
						IDlgProgress* progress = IDlgProgress::CreateProgress(1, wxGetApp().GetTopWindow());
						progress->SetCaption(_("IDS_DOWNLOADING"));
						progress->SetRange(1, m_size);
						progress->SetMessage(name.GetFullName());
						progress->ShowProgress();
						progress->SetForegroundWindow();
						CReadHttp http(m_NewFile, output, progress);
						wxString err;
						if (!http.ReadHttpFile(err))
						{
							bGotoWeb = true;
							// If user canceled, no message is generated.
							if (!err.empty())
							{
								if (!errMsg.empty())
									errMsg += wxT("\n\n");
								errMsg += err;
							}
						}
						output.Close();
						if (!bGotoWeb && !m_md5.empty())
						{
							progress->SetCaption(_("IDS_VALIDATING"));
							if (ARBMsgDigest::ComputeFile(filename) != m_md5)
							{
								bGotoWeb = true;
								if (!errMsg.empty())
									errMsg += wxT("\n\n");
								errMsg += _("IDS_ERROR_DOWNLOAD");
							}
						}
						if (bGotoWeb)
							::wxRemoveFile(filename);
						progress->Dismiss();
					}
					if (!errMsg.empty() && bGotoWeb)
					{
						wxMessageBox(errMsg);
					}
				}
				if (!bGotoWeb)
				{
#if defined(__WXMAC__)
					outClose = true;
					wxMessageBox(wxString::Format(_("IDS_DOWNLOAD_AND_RESTART"), filename.c_str()));
#elif defined(__WXMSW__)
					wxString msiFilename;
					{ // Can't remove file until stream is closed.
						wxFileInputStream inStream(filename);
						if (!inStream.IsOk())
						{
							bGotoWeb = true;
							wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
						}
						else
						{
							wxZipInputStream inZip(inStream);
							if (!inZip.IsOk())
							{
								bGotoWeb = true;
								wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
							}
							else
							{
								wxZipEntry* entry = inZip.GetNextEntry();
								if (!entry)
								{
									bGotoWeb = true;
									wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
								}
								else
								{
									wxFileName msiName(entry->GetName());
									if (msiName.GetExt().Upper() != wxT("MSI"))
									{
										bGotoWeb = true;
										wxMessageBox(_("IDS_ERROR_UNEXPECTED"));
									}
									else
									{
										msiFilename = wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + msiName.GetFullName();
										wxFileOutputStream output(msiFilename);
										inZip.Read(output);
									}
									delete entry;
								}
								inZip.CloseEntry();
							}
						}
					}
					::wxRemoveFile(filename);
					if (!bGotoWeb)
					{
						wxString updater = UpdateFile();
						{
							wxFileOutputStream output(updater);
							wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
							wxString zipfile = wxFileSystem::FileNameToURL(wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + wxT(".dat"));
							zipfile += wxT("#zip:") + ARBUpdater();
							wxFileSystem filesys;
							wxFSFile* file = filesys.OpenFile(zipfile);
							if (file)
							{
								wxInputStream* input = file->GetStream();
								input->Read(output);
								delete file;
							}
							else
							{
								bGotoWeb = true;
								wxMessageBox(_("IDS_ERROR_AUTOUPDATE"));
							}
						}
						if (!bGotoWeb)
						{
							wxString args = wxString::Format(wxT("-f \"%s\""), msiFilename.c_str());
							SHELLEXECUTEINFO info;
							ZeroMemory(&info, sizeof(info));
							info.cbSize = sizeof(info);
							info.lpVerb = _T("open");
							info.lpFile = updater.wx_str();
							info.lpParameters = args.wx_str();
							if (ShellExecuteEx(&info))
								outClose = true;
							else
							{
								bGotoWeb = true;
								::wxRemoveFile(updater);
								wxMessageBox(_("IDS_ERROR_AUTOUPDATE"));
							}
						}
					}
#endif
				}
			}
			if (bGotoWeb)
			{
				wxString url(m_UpdateDownload);
				wxString suffix = url.Right(4);
				suffix.MakeUpper();
				if (suffix == wxT(".PHP"))
				{
#ifdef __WXMSW__
					OSVERSIONINFO os;
					os.dwOSVersionInfoSize = sizeof(os);
					GetVersionEx(&os);
					switch (os.dwPlatformId)
					{
					default:
					case VER_PLATFORM_WIN32_NT: // NT/Win2000/XP/Vista
						{
#ifdef ARB_HAS_GETSYSTEMINFO
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
	}
	else
		wxConfig::Get()->Write(CFG_SETTINGS_LASTVERCHECK, today.GetString(ARBDate::eISO).c_str());
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
	if (0 < m_ConfigFileName.length() && m_VerConfig > pDoc->Book().GetConfig().GetVersion())
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
			url += m_ConfigFileName;
			std::string strConfig;
			wxString errMsg;
			CReadHttp file(url, &strConfig);
			wxString userName = CAgilityBookOptions::GetUserName(m_usernameHint);
			if (file.ReadHttpFile(m_usernameHint, errMsg, wxGetApp().GetTopWindow()))
			{
				CAgilityBookOptions::SetUserName(m_usernameHint, userName);
				ElementNodePtr tree(ElementNode::New());
				wxString errMsg2;
				if (!tree->LoadXML(strConfig.c_str(), strConfig.length(), errMsg2))
				{
					wxString msg2 = wxString::Format(_("IDS_LOAD_FAILED"), url.c_str());
					if (0 < errMsg2.length())
					{
						msg2 += wxT("\n\n");
						msg2 += errMsg2;
					}
					wxMessageBox(msg2, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
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
								wxMessageBox(err.m_ErrMsg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_WARNING);
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
		wxMessageBox(_("IDS_UPDATE_CURRENT"), wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_INFORMATION);
	}
}


void CUpdateInfo::AutoUpdateProgram(
		CAgilityBookDoc* pDoc,
		CLanguageManager const& langMgr,
		bool& outClose)
{
	outClose = false;
	if (ReadVersionFile(false, langMgr))
	{
		CheckProgram(pDoc, langMgr.CurrentLanguage(), outClose);
	}
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
		CLanguageManager const& langMgr,
		bool& outClose)
{
	outClose = false;
	// Only continue if we parsed the version.txt file
	// AND the version is up-to-date.
	if (!ReadVersionFile(true, langMgr))
		return;
	if (CheckProgram(pDoc, langMgr.CurrentLanguage(), outClose))
		return;
	CheckConfig(pDoc, langMgr, true);
}
