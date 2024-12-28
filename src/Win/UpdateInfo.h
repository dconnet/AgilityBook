#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Parse and cache "version.txt" on www.agilityrecordbook.com
 * @author David Connet
 *
 * Revision History
 * 2021-04-27 Convert libcurl to wxWebRequest.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-06 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2004-08-03 Created
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ReadHttp.h"


namespace dconSoft
{
class CAgilityBookDoc;


/**
 * Check if there is a newer version of the program.
 *
 * Downloads and caches information on first usage.
 * Uses cached information for subsequent checks.
 */
class CUpdateInfo
{
	DECLARE_NO_COPY_IMPLEMENTED(CUpdateInfo)

public:
	/**
	 * Ask the user if they wish to update the configuration. This is a
	 * yes/no/don't-ask-me-again question.
	 * @param ioDoc Configuration/document to update.
	 */
	static bool AllowUpdateConfig(CAgilityBookDoc* ioDoc);

	/**
	 * Cleanup after auto-update.
	 */
	static void CleanupUpdate();

	/**
	 * @param parent Parent window to bind to for notifications.
	 */
	CUpdateInfo(wxWindow* parent);

	/**
	 * Initialize by getting version data.
	 */
	void Initialize();

	/**
	 * A download is currently in progress.
	 */
	bool DownloadInProgress() const;

	/**
	 * Return indicates we're waiting for the cancel to complete.
	 */
	bool CancelDownload();

	/*
	 * Check if we can close.
	 */
	bool CanClose(wxWindow* parent, bool prompt);

	/**
	 * Are we out-of-date? (Does not check internet)
	 */
	bool IsOutOfDate();

	/**
	 * Called when opening a document. This will not load anything
	 * from the internet, it only uses cached data. If it knows the
	 * program needs updating, it will not update the config.
	 * @param pDoc Document to check configuration against.
	 */
	void AutoCheckConfiguration(CAgilityBookDoc* pDoc);

	/**
	 * Check the configuration. This will also check the program version.
	 * @param pDoc Document to check configuration against.
	 * @param outDownloadStarted A download has begun.
	 */
	void UpdateConfiguration(CAgilityBookDoc* pDoc, bool& outDownloadStarted);

private:
	using OnReadComplete = std::function<void()>;

	void clear();
	ARBWin::CReadHttp::ReturnCode ReadVersionFile(bool bVerbose, OnReadComplete callback = nullptr);
	bool ReadVersionFile(std::string const& data, bool bVerbose);
	bool CheckProgram(CAgilityBookDoc* pDoc, wxString const& lang, bool& outDownloadStarted, bool& canInstall);
	void CheckConfig(CAgilityBookDoc* pDoc, bool bVerbose);
	bool CheckConfig(CAgilityBookDoc* pDoc, wxString url, std::string const& strConfig, bool bVerbose);
	bool DownloadFile(wxString const& filename);

	wxWindow* m_parent;
	ARBWin::CReadHttp m_reader;

	ARBCommon::CVersionNum m_VersionNum;
	long m_size;
	wxString m_hash;
	ARBCommon::ARBMsgDigest::ARBDigest m_hashType;
	wxString m_SourceFile;
	short m_VerConfig;
	wxString m_ConfigFileName;
	wxString m_UpdateDownload;
};

} // namespace dconSoft
