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
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-01-06 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-10-26 Added option to prevent auto-update user query.
 * 2004-08-03 Created
 */

#include "ARB/ARBTypes2.h"
#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/VersionNum.h"
#include <map>
#include <vector>
class CAgilityBookDoc;


/**
 * This class manages checking to see if there are newer versions of the
 * program or configuration.
 */
class CUpdateInfo
{
protected:
	friend class CAgilityBookApp;

	CUpdateInfo();
	bool ReadVersionFile(bool bVerbose);
	bool IsOutOfDate();
	bool CheckProgram(
			CAgilityBookDoc* pDoc,
			std::wstring const& lang,
			bool& outClose);
	void CheckConfig(
			CAgilityBookDoc* pDoc,
			bool bVerbose);

public:
	/**
	 * Ask the user if they wish to update the configuration. This is a
	 * yes/no/don't-ask-me-again question.
	 * @param ioDoc Configuration/document to update.
	 * @param inMsg Addition text to append to the question. If specified,
	 *              2 newlines will be inserted first.
	 */
	static bool UpdateConfig(
			CAgilityBookDoc* ioDoc,
			wchar_t const* inMsg = nullptr);

	/**
	 * Cleanup after auto-update.
	 */
	static void CleanupUpdate();

	/**
	 * Called when the program does its monthly auto-check.
	 * This only checks the program version.
	 * @param pDoc Document to check configuration against.
	 * @param outClose The program must close.
	 */
	void AutoUpdateProgram(
			CAgilityBookDoc* pDoc,
			bool& outClose);

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
	 * @param outClose The program must close.
	 */
	void UpdateConfiguration(
			CAgilityBookDoc* pDoc,
			bool& outClose);

private:
	CVersionNum m_VersionNum;
	short m_VerConfig;
	long m_size;
	std::wstring m_hash;
	ARBMsgDigest::ARBDigest m_hashType;
	std::wstring m_NewFile;
	std::wstring m_ConfigFileName;
	std::map<std::wstring, std::wstring> m_InfoMsg;
	std::wstring m_UpdateDownload;
	std::wstring m_usernameHint;
	std::vector<CVersionNum> m_CalSiteSuppression;
};
