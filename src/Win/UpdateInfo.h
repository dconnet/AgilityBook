#pragma once

/*
 * Copyright © 2004 David Connet. All Rights Reserved.
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
 * Revision History
 * @li 2004-08-03 DRC Created
 */

#include <string>
#include "Element.h"
#include "VersionNum.h"
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
	bool CheckProgram();
	void CheckConfig(CAgilityBookDoc* pDoc, bool bVerbose);

public:
	/**
	 * Called when the program does its monthly auto-check.
	 * This only checks the program version.
	 */
	void AutoUpdateProgram();

	/**
	 * Called when opening a document. This will not load anything
	 * from the internet, it only uses cached data. If it knows the
	 * program needs updating, it will not update the config.
	 */
	void AutoCheckConfiguration(CAgilityBookDoc* pDoc);

	/**
	 * Check the configuration. This will also check the program version.
	 * @param pDoc Document to check configuration against.
	 */
	void UpdateConfiguration(CAgilityBookDoc* pDoc);

private:
	CVersionNum m_VersionNum;
	short m_VerConfig;
	std::string m_FileName;
	std::string m_InfoMsg;
};
