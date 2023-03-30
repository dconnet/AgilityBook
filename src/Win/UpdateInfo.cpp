/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Parse and cache "version2.xml" on www.agilityrecordbook.com
 * @author David Connet
 *
 * File Format: See below.
 *
 * Revision History
 * 2019-01-18 Change debugging to use ARB_WWW_ROOT instead of hardcoded path.
 *            Fix a config-only update (broken since moving to version2.xml)
 *            Fix loading local config file.
 * 2018-12-16 Convert to fmt.
 * 2014-03-05 Change wxFileSystem usage to CLibArchive.
 * 2013-12-05 Remove "?os=..." from url (website redesign)
 * 2013-10-30 Fixed a problem where arbupdater was spawned hidden.
 * 2013-10-23 Added 'minOS'.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-07-26 Moved 'Arch' to ARBAgilityRecordBook.
 * 2010-05-07 Removed 'lang' from 'Platform'. (Finally figured out how
 *            to create transforms and embed in msi.)
 * 2010-02-07 Changed to version2.xml.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-26 Removed Win98 support.
 * 2009-07-18 Updated version.txt to support different versions/platforms.
 * 2009-01-06 Ported to wxWidgets.
 * 2008-06-29 When looking for language ids, it was searching the wrong node.
 * 2008-01-01 Fix a bug parsing Element data (GetElementNode may return null)
 * 2007-08-03 Separated HTTP reading code from UpdateInfo.cpp
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2005-09-09 Modified URL parsing to handle redirection.
 *            I can now advertise the url as "www.agilityrecordbook.com"
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-08-03 Created
 */

/*
 * (post v2.1.4): Changed to version2.xml.
 *  See comments in code (where it's parsed).
 */

#include "stdafx.h"
#include "UpdateInfo.h"

#include "AgilityBook.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ConfigHandler.h"
#include "RegItems.h"
#include "VersionNumber.h"
#include <fstream>

#include "ARBCommon/ARBMisc.h"
#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/ARBUtils.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/DlgProgress.h"
#include "LibARBWin/Logger.h"
#include "LibARBWin/ReadHttp.h"
#include "LibARBWin/ResourceManager.h"
#include <wx/config.h>
#include <wx/filedlg.h>
#include <wx/filename.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#include <wx/textfile.h>
#include <wx/wfstream.h>

#ifdef WIN32
#include <wuapi.h>
#endif

#include "Platform/arbWarningPush.h"

#include <wx/zipstrm.h>

#include "Platform/arbWarningPop.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
constexpr wchar_t k_versionFile[] = L"version2.xml";


#if defined(__WXMSW__)
constexpr wchar_t k_arbUpdater[] = L"ARBUpdater.exe";

wxString UpdateFile()
{
	return wxFileName::GetTempDir() + wxFileName::GetPathSeparator() + k_arbUpdater;
}
#endif


bool GetVersionsFilename(wxString const& filename, wxString& fullpath)
{
	fullpath.clear();

	bool bUseLocal = false;
#if defined(_DEBUG) || defined(__WXDEBUG__)
	/// ARB_WWW_ROOT points to the directory where version2.xml lives.
	wxString path;
	if (wxGetEnv(L"ARB_WWW_ROOT", &path))
	{
		wxFileName file(path, filename);
		fullpath = file.GetFullPath();
		bUseLocal = true;
	}
	else
	{
		// #pragma PRAGMA_TODO(Remove this)
		// fullpath = L"d:\\dcon\\www\\agilityrecordbook\\version2.xml";
		// bUseLocal = true;
	}
#endif
	if (!bUseLocal)
		fullpath = L"https://www.agilityrecordbook.com/" + filename;

	return bUseLocal;
}


void GotoUrl(wxString const& url)
{
#if defined(_DEBUG) || defined(__WXDEBUG__)
	wxMessageBox(url, L"DEBUG - Goto url");
#else
	wxLaunchDefaultBrowser(url);
#endif
}
} // namespace

/////////////////////////////////////////////////////////////////////////////

bool CUpdateInfo::AllowUpdateConfig(CAgilityBookDoc* ioDoc)
{
	wxString msg(_("IDS_UPDATED_CONFIG"));
	bool b = (wxYES == wxMessageBox(msg, _("Agility Record Book"), wxYES_NO | wxCENTRE | wxICON_QUESTION));
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
		wxLogNull log;
		if (!::wxRemoveFile(updateFile))
		{
			// If we get here really quick, the updater may not have finished
			// yet. Go to the Win API now.
			Sleep(3000);
			if (!DeleteFile(updateFile.wc_str()))
			{
				// Ok, delete failed after 3 seconds. Punt (quietly).
				MoveFileEx(updateFile.wc_str(), nullptr, MOVEFILE_DELAY_UNTIL_REBOOT);
			}
		}
	}
#endif
}

/////////////////////////////////////////////////////////////////////////////

CUpdateInfo::CUpdateInfo(wxWindow* parent)
	: m_parent(parent)
	, m_reader()
	, m_VersionNum()
	, m_size(0)
	, m_hash()
	, m_hashType(ARBMsgDigest::ARBDigest::Unknown)
	, m_SourceFile()
	, m_VerConfig(0)
	, m_ConfigFileName()
	, m_UpdateDownload()
{
	assert(m_parent);
}


void CUpdateInfo::clear()
{
	m_VersionNum.clear();
	m_size = 0;
	m_hash.clear();
	m_hashType = ARBMsgDigest::ARBDigest::Unknown;
	m_SourceFile.clear();
	m_VerConfig = 0;
	m_ConfigFileName.clear();
	m_UpdateDownload = _("LinkArbDownloadUrl");
}


void CUpdateInfo::Initialize()
{
	if (!m_parent || m_VersionNum.Valid() || DownloadInProgress())
		return;

	// Load the version data. When that's complete, do the autoupdate check.
	ReadVersionFile(false, [this]() {
		// Check for updates every 30 days.
		if (CAgilityBookOptions::GetAutoUpdateCheck())
		{
			std::wstring ver = StringUtil::stringW(wxConfig::Get()->Read(CFG_SETTINGS_LASTVERCHECK, L""));
			ARBDate date = ARBDate::FromString(ver, ARBDateFormat::ISO);
			if (date.IsValid())
			{
				ARBDate today = ARBDate::Today();
				date += CAgilityBookOptions::GetAutoUpdateCheck();
				if (date < today)
				{
					if (CReadHttp::ReturnCode::Success == ReadVersionFile(false))
					{
						// When checking if we can install, silently ignore the case when we
						// are due to update, but can't because Windows is updating.
						bool outDownloadStarted, canInstall;
						CheckProgram(nullptr, wxGetApp().CurrentLanguage(), outDownloadStarted, canInstall);
					}
				}
			}
		}
	});
}


bool CUpdateInfo::DownloadInProgress() const
{
	return m_reader.IsOk();
}


bool CUpdateInfo::CancelDownload()
{
	return m_reader.CancelDownload();
}


bool CUpdateInfo::CanClose(wxWindow* parent, bool prompt)
{
	if (!DownloadInProgress())
		return true;
	bool canClose = false;
	if (prompt)
	{
		wxMessageDialog dlg(
			parent,
			_("A file download is in progress. Closing the window will cancel it."),
			_("Confirm"),
			wxYES_NO);
		dlg.SetYesNoLabels(_("Cancel and close"), _("Don't close"));
		if (wxID_YES == dlg.ShowModal())
		{
			canClose = true;
			CancelDownload();
		}
	}
	return canClose;
}


/**
 * Check the version against the web.
 */
bool CUpdateInfo::IsOutOfDate()
{
	if (!m_VersionNum.Valid())
		return true;
	CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
	assert(ver.Valid());
	return ver < m_VersionNum;
}


void CUpdateInfo::AutoCheckConfiguration(CAgilityBookDoc* pDoc)
{
	// If we're opening a doc and we've checked the internet
	// and there is a more current version, do not continue
	// to check the config. This prevents loading a config that
	// uses a newer file version.
	if (IsOutOfDate())
		return;
	CheckConfig(pDoc, false);
}


void CUpdateInfo::UpdateConfiguration(CAgilityBookDoc* pDoc, bool& outDownloadStarted)
{
	outDownloadStarted = false;
	// Only continue if we parsed the version.txt file
	// AND the version is up-to-date.
	if (!m_VersionNum.Valid() && CReadHttp::ReturnCode::Success != ReadVersionFile(true))
		return;
	bool canInstall;
	if (CheckProgram(pDoc, wxGetApp().CurrentLanguage(), outDownloadStarted, canInstall))
	{
		if (!canInstall)
			wxMessageBox(_("IDS_INSTALL_BUSY"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_WARNING);
		return;
	}
	CheckConfig(pDoc, true);
}


/**
 * This will read the version2.xml file and cache it.
 * In addition, it will ask to update if a newer version is found.
 * @param bVerbose Show error message.
 * @param callback Function to call on completion of download.
 */
CReadHttp::ReturnCode CUpdateInfo::ReadVersionFile(bool bVerbose, OnReadComplete callback)
{
	if (!m_parent)
		return CReadHttp::ReturnCode::Failed;

	wxString fullpath;
	if (GetVersionsFilename(k_versionFile, fullpath))
	{
		wxTextFile file;
		if (!file.Open(fullpath))
		{
			if (bVerbose)
				wxMessageBox(_("IDS_UPDATE_UNKNOWN"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return CReadHttp::ReturnCode::Failed;
		}

		std::string data; // must be 'char' for XML parsing
		for (size_t line = 0; line < file.GetLineCount(); ++line)
		{
			data += file[line].ToUTF8();
			data += '\n';
		}
		file.Close();

		if (ReadVersionFile(data, bVerbose))
		{
			if (callback)
				callback();
			return CReadHttp::ReturnCode::Success;
		}
		else
			return CReadHttp::ReturnCode::Failed;
	}

	wxMemoryOutputStream* out = new wxMemoryOutputStream();

	CLogger::Log(wxString::Format(L"UpdateInfo: Reading: %s", fullpath));
	auto rc = m_reader.DownloadFile(
		m_parent,
		fullpath,
		nullptr,
		out,
		[this, out, callback, bVerbose, fullpath](wxWebRequest::State state) {
			if (wxWebRequest::State::State_Completed == state)
			{
				auto stream = out->GetOutputStreamBuffer();
				std::string data(static_cast<const char*>(stream->GetBufferStart()), stream->GetBufferSize());
				if (ReadVersionFile(data, bVerbose))
				{
					CLogger::Log(wxString::Format(L"UpdateInfo: Success: %s", fullpath));
					if (callback)
						callback();
				}
				else
				{
					CLogger::Log(wxString::Format(L"UpdateInfo: Failed: %s", fullpath));
				}
			}
			delete out;
		});
	if (CReadHttp::ReturnCode::Failed == rc)
		delete out;
	return rc;
}

bool CUpdateInfo::ReadVersionFile(std::string const& data, bool bVerbose)
{
	// Clear everything.
	clear();

	bool bLoadedVersion = false;
	if (!data.empty())
	{
		/*
		 * <!ELEMENT Data (Platform, Config, Download*) >
		 * <!ELEMENT Platform (EMPTY) >
		 *   <!--
		 *   Important: 'arch' elements for must be ordered in decending
		 *   order on 'minOS'. On Windows, this is just a coarse check.
		 *   If there is an SP requirement, the installer will catch it. Not
		 *   a good UI experience, but there's only so much that can be done!
		 *   Right now, XP has the SP requirements. The only issue is if a future
		 *   version (let's say 8.1sp3). If that happens, then I can rename the
		 *   'arch' tags again and deal with it then (like adding support for
		 *   'minOS' is happening now).
		 *   -->
		 *   <!ATTLIST Platform
		 *     arch CDATA #REQUIRED (win32,win64,osx,osx-x64,osx-arm64)
		 *     minOS CDATA #IMPLIED (maj.min)
		 *     ver CDATA #REQUIRED
		 *     config CDATA #REQUIRED
		 *     md5 CDATA #REQUIRED [one of md5/sha1/sha256 is required]
		 *     sha1 CDATA #REQUIRED [one of md5/sha1/sha256 is required]
		 *     sha256 CDATA #REQUIRED [one of md5/sha1/sha256 is required]
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
		 *  which is https://www.agilityrecordbook.com/download.php
		 *  -->
		 *  <!ELEMENT Download (#PCDATA) >
		 */
		fmt::wmemory_buffer errMsg2;
		ElementNodePtr tree(ElementNode::New());
		bool bOk = false;
		{
			wxBusyCursor wait;
			bOk = tree->LoadXML(data.c_str(), data.length(), errMsg2);
		}
		if (!bOk)
		{
			if (bVerbose)
			{
				fmt::wmemory_buffer msg;
				fmt::format_to(std::back_inserter(msg), _("IDS_LOAD_FAILED").wc_str(), k_versionFile);
				if (0 < errMsg2.size())
				{
					fmt::format_to(std::back_inserter(msg), L"\n\n{}", fmt::to_string(errMsg2));
				}
				wxMessageBox(fmt::to_string(msg), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			}
		}
		else if (tree->GetName() == L"Data")
		{
			const auto arbArch = GetARBArch();
			for (int nIndex = 0; nIndex < tree->GetElementCount(); ++nIndex)
			{
				ElementNodePtr node = tree->GetElementNode(nIndex);
				if (!node)
					continue;
				if (!bLoadedVersion && node->GetName() == L"Platform")
				{
					bool bSkip = true;
					std::wstring value;
					if (ARBAttribLookup::Found == node->GetAttrib(L"arch", value))
					{
						if (arbArch == value)
						{
							bSkip = false;
							ARBVersion minOS;
							if (ARBAttribLookup::Found == node->GetAttrib(L"minOS", minOS))
							{
								int majVer;
								int minVer;
								GetOSInfo(majVer, minVer);
								ARBVersion ver(
									static_cast<unsigned short>(majVer),
									static_cast<unsigned short>(minVer));
								if (ver < minOS)
									bSkip = true;
							}
							else
							{
								// It's an error, but don't expose this to the user.
								bSkip = true;
							}
						}
					}
					// Wrong architecture
					if (bSkip)
						continue;
					if (ARBAttribLookup::Found == node->GetAttrib(L"ver", value))
					{
						m_VersionNum.Parse(value);
						if (m_VersionNum.Valid())
							bLoadedVersion = true;
					}
					if (ARBAttribLookup::Found != node->GetAttrib(L"config", m_VerConfig)
						|| ARBAttribLookup::Found != node->GetAttrib(L"size", m_size)
						|| ARBAttribLookup::Found != node->GetAttrib(L"file", m_SourceFile))
					{
						bLoadedVersion = false;
					}
					else
					{
						m_ConfigFileName = fmt::format(L"Config{}.txt", m_VerConfig);
						if (ARBAttribLookup::Found == node->GetAttrib(L"sha256", m_hash))
							m_hashType = ARBMsgDigest::ARBDigest::SHA256;
						else if (ARBAttribLookup::Found == node->GetAttrib(L"sha1", m_hash))
							m_hashType = ARBMsgDigest::ARBDigest::SHA1;
						else if (ARBAttribLookup::Found == node->GetAttrib(L"md5", m_hash))
							m_hashType = ARBMsgDigest::ARBDigest::MD5;
						else
							bLoadedVersion = false;
					}

					if (!bLoadedVersion)
					{
						if (bVerbose)
						{
							wxMessageBox(
								_("IDS_UPDATE_UNKNOWN"),
								_("Agility Record Book"),
								wxOK | wxCENTRE | wxICON_EXCLAMATION);
						}
						// If the parsing failed, clear everything.
						clear();
						return false;
					}
				}
				// Note: Ignoring <Config> section - that was pre-v2.4.0.
				// Note: version2.xml doesn't have this. As noted, we default to 'LinkArbDownloadUrl'
				else if (node->GetName() == L"Download")
				{
					m_UpdateDownload = node->GetValue();
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
	std::wstring const& lang,
	bool& outDownloadStarted,
	bool& canInstall)
{
	outDownloadStarted = false;
	canInstall = true;
#ifdef WIN32
	// Can we install? Check to see if Window Update is in process.
	// Note: CoInitialize already called
	IUpdateInstaller* iInstaller = nullptr;
	if (SUCCEEDED(CoCreateInstance(
			CLSID_UpdateInstaller,
			nullptr,
			CLSCTX_INPROC_SERVER,
			IID_IUpdateInstaller,
			(LPVOID*)&iInstaller)))
	{
		VARIANT_BOOL bVal;
		if (SUCCEEDED(iInstaller->get_IsBusy(&bVal)))
		{
			if (bVal)
				canInstall = false;
		}
		/* We don't really care if a reboot is needed.
		if (SUCCEEDED(iInstaller->get_RebootRequiredBeforeInstallation(&bVal)))
		{
			if (bVal)
				canInstall = false;
		}
		*/

		iInstaller->Release();
	}
#endif

	bool bNeedsUpdating = false;
	ARBDate today = ARBDate::Today();
	if (IsOutOfDate())
	{
		bNeedsUpdating = true;
		if ((pDoc && !pDoc->OnSaveModified()) || !canInstall)
		{
			// Return that it needs updating, but don't record that we checked.
			return bNeedsUpdating;
		}
		wxConfig::Get()->Write(CFG_SETTINGS_LASTVERCHECK, today.GetString(ARBDateFormat::ISO).c_str());
		std::wstring msg = fmt::format(_("IDS_VERSION_AVAILABLE").wc_str(), m_VersionNum.GetVersionString());
		if (wxYES == wxMessageBox(msg, _("Agility Record Book"), wxYES_NO | wxCENTRE | wxICON_QUESTION))
		{
			bool bGotoWeb = false;
			// If running in standalone mode, just go to the web.
			if (m_SourceFile.empty() || wxGetApp().IsStandloneMode())
				bGotoWeb = true;
			else
			{
				wxFileName name(StringUtil::stringWX(m_SourceFile));
				wxString filename;
#if defined(__WXMAC__)
				wxFileDialog dlg(
					wxGetApp().GetTopWindow(),
					wxFileSelectorPromptStr,
					wxStandardPaths::Get().GetUserDir(wxStandardPaths::Dir_Downloads),
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
					if (DownloadFile(filename))
						outDownloadStarted = true;
					else
						bGotoWeb = true;
				}
			}
			if (bGotoWeb)
			{
				GotoUrl(StringUtil::stringWX(m_UpdateDownload));
			}
		}
	}
	else
		wxConfig::Get()->Write(CFG_SETTINGS_LASTVERCHECK, today.GetString(ARBDateFormat::ISO).c_str());
	return bNeedsUpdating;
}


void CUpdateInfo::CheckConfig(CAgilityBookDoc* pDoc, bool bVerbose)
{
	// If the parse was successful, check for the posted config version.
	bool bUpToDate = true;
	// Cool! New config!
	if (0 < m_ConfigFileName.length() && m_VerConfig > pDoc->Book().GetConfig().GetVersion())
	{
		bUpToDate = false;
		if (AllowUpdateConfig(pDoc))
		{
			// Load the config.
			wxString url;
			bool bUseLocal = GetVersionsFilename(m_ConfigFileName, url);
			std::string strConfig;

			if (bUseLocal)
			{
				wxTextFile file;
				if (!file.Open(url))
				{
					// TODO: error message
					return;
				}
				for (size_t line = 0; line < file.GetLineCount(); ++line)
				{
					strConfig += file[line].ToUTF8();
					strConfig += '\n';
				}
				file.Close();
				if (!CheckConfig(pDoc, url, strConfig, bVerbose))
				{
				}
			}
			else
			{
				wxMemoryOutputStream* out = new wxMemoryOutputStream();

				if (CReadHttp::ReturnCode::Failed
					== m_reader.DownloadFile(
						m_parent,
						url,
						nullptr,
						out,
						[this, pDoc, url, out, bVerbose](wxWebRequest::State state) {
							if (wxWebRequest::State::State_Completed == state)
							{
								auto stream = out->GetOutputStreamBuffer();
								std::string data(
									static_cast<const char*>(stream->GetBufferStart()),
									stream->GetBufferSize());
								CheckConfig(pDoc, url, data, bVerbose);
							}
							delete out;
						}))
				{
					// TODO: Error handling
					delete out;
				}
			}
		}
	}
	if (bUpToDate && bVerbose)
	{
		wxMessageBox(_("IDS_UPDATE_CURRENT"), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_INFORMATION);
	}
}


bool CUpdateInfo::CheckConfig(CAgilityBookDoc* pDoc, wxString url, std::string const& strConfig, bool bVerbose)
{
	ElementNodePtr tree(ElementNode::New());
	fmt::wmemory_buffer errMsg2;
	{
		wxBusyCursor wait;
		if (!tree->LoadXML(strConfig.c_str(), strConfig.length(), errMsg2))
		{
			fmt::wmemory_buffer msg2;
			fmt::format_to(std::back_inserter(msg2), _("IDS_LOAD_FAILED").wc_str(), url.wc_str());
			if (0 < errMsg2.size())
			{
				fmt::format_to(std::back_inserter(msg2), L"\n\n{}", fmt::to_string(errMsg2));
			}
			wxMessageBox(fmt::to_string(msg2), _("Agility Record Book"), wxOK | wxCENTRE | wxICON_EXCLAMATION);
			return false;
		}
	}
	if (tree->GetName() != L"DefaultConfig")
		return false;

	ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
	tree->GetAttrib(ATTRIB_BOOK_VERSION, version);
	int nConfig = tree->FindElement(TREE_CONFIG);
	if (0 <= nConfig)
	{
		CErrorCallback err;
		ARBAgilityRecordBook book;
		if (!book.GetConfig().Load(tree->GetElementNode(nConfig), version, err))
		{
			if (0 < err.m_ErrMsg.size())
				wxMessageBox(
					StringUtil::stringWX(fmt::to_string(err.m_ErrMsg)),
					_("Agility Record Book"),
					wxOK | wxCENTRE | wxICON_WARNING);
			return false;
		}
		else
		{
			if (pDoc->ImportConfiguration(book.GetConfig()))
				pDoc->Modify(true);
		}
	}
	return true;
}


bool CUpdateInfo::DownloadFile(wxString const& filename)
{
	if (m_hash.empty())
		return false;

	wxFileOutputStream* output = new wxFileOutputStream(filename);
	if (!output->IsOk())
	{
		delete output;
		fmt::wmemory_buffer errMsg;
		fmt::format_to(std::back_inserter(errMsg), _("IDS_CANNOT_OPEN").wc_str(), filename.wc_str());
		wxMessageBox(fmt::to_string(errMsg));
		return false;
	}

	wxFileName name(StringUtil::stringWX(m_SourceFile));

	IDlgProgress* progress = IDlgProgress::CreateProgress(1, wxGetApp().GetTopWindow());
	progress->SetCaption(StringUtil::stringW(_("IDS_DOWNLOADING")));
	progress->SetRange(1, m_size);
	progress->SetMessage(StringUtil::stringW(name.GetFullName()));
	progress->ShowProgress();
	progress->SetForegroundWindow();

	if (CReadHttp::ReturnCode::Failed
		== m_reader.DownloadFile(
			wxGetApp().GetTopWindow(),
			m_SourceFile,
			progress,
			output,
			[this, filename, output, progress](wxWebRequest::State state) {
				bool deleteFile = true;
				bool gotoWeb = false;
				output->Close();
				delete output;
				switch (state)
				{
				case wxWebRequest::State::State_Completed:
				{
#if defined(__WXMAC__)
					deleteFile = false;
					wxMessageBox(fmt::format(_("IDS_DOWNLOAD_AND_RESTART").wc_str(), filename.wc_str()));
#elif defined(__WXMSW__)
					progress->SetCaption(StringUtil::stringW(_("IDS_VALIDATING")));
					wxFileInputStream file(filename);
					wxStdInputStream stdfile(file);
					if (ARBMsgDigest::Compute(stdfile, m_hashType) != m_hash)
					{
						wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
						gotoWeb = true;
					}
					else
					{
						wxString msiFilename;
						{
							wxFileInputStream inStream(filename);
							if (!inStream.IsOk())
							{
								wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
								gotoWeb = true;
							}
							else
							{
								wxZipInputStream inZip(inStream);
								if (!inZip.IsOk())
								{
									wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
									gotoWeb = true;
								}
								else
								{
									wxZipEntry* entry = inZip.GetNextEntry();
									if (!entry)
									{
										wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
										gotoWeb = true;
									}
									else
									{
										wxFileName msiName(entry->GetName());
										if (msiName.GetExt().Upper() != L"MSI")
										{
											wxMessageBox(_("IDS_ERROR_UNEXPECTED"));
											gotoWeb = true;
										}
										else
										{
											msiFilename = wxFileName::GetTempDir() + wxFileName::GetPathSeparator()
														  + msiName.GetFullName();
											wxFileOutputStream outputMsi(msiFilename);
											inZip.Read(outputMsi);
										}
										delete entry;
									}
									inZip.CloseEntry();
								}
							}
						}
						if (!gotoWeb)
						{
							wxString updater = UpdateFile();
							{
								std::wstring outFile(StringUtil::stringW(updater));
#if defined(ARB_HAS_OSTREAM_WCHAR)
								std::ofstream outputUpdater(outFile.c_str(), std::ios::out | std::ios::binary);
#else
								std::string filenameA = StringUtil::stringA(outFile);
								std::ofstream output(filenameA.c_str(), std::ios::out | std::ios::binary);
#endif
								if (!CResourceManager::Get()->LoadFile(k_arbUpdater, outputUpdater))
								{
									gotoWeb = true;
									wxMessageBox(_("IDS_ERROR_AUTOUPDATE"));
								}
							}
							if (!gotoWeb)
							{
								std::wstring args = fmt::format(L"-f \"{}\"", msiFilename.wc_str());
								SHELLEXECUTEINFO info;
								ZeroMemory(&info, sizeof(info));
								info.cbSize = sizeof(info);
								info.lpVerb = L"open";
								info.lpFile = updater.wc_str();
								info.lpParameters = args.c_str();
								info.nShow = SW_SHOWNORMAL;
								if (ShellExecuteEx(&info))
								{
									wxGetApp().GetTopWindow()->Close();
								}
								else
								{
									gotoWeb = true;
									::wxRemoveFile(updater);
									wxMessageBox(_("IDS_ERROR_AUTOUPDATE"));
								}
							}
						}
					}
#endif
				}
				break;
				case wxWebRequest::State::State_Failed:
					wxMessageBox(_("IDS_ERROR_DOWNLOAD"));
					gotoWeb = true;
					break;
				case wxWebRequest::State::State_Active:
				case wxWebRequest::State::State_Cancelled:
				case wxWebRequest::State::State_Idle:
				case wxWebRequest::State::State_Unauthorized:
					break;
				}
				progress->Dismiss();
				if (gotoWeb)
					GotoUrl(StringUtil::stringWX(m_UpdateDownload));
				if (deleteFile)
				{
					wxLogNull log; // In case file is slow in releasing lock (from zip extraction)
					::wxRemoveFile(filename);
				}
			}))
	{
		progress->Dismiss();
		delete output;
		return false;
	}
	return true;
}

} // namespace dconSoft
