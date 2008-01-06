#pragma once

/*
 * Copyright © 2007-2008 David Connet. All Rights Reserved.
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
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 */

#include <map>
#include "resource.h"
#include "ARBTypes.h"
#include "DlgPageEncode.h"
#include "DlgPageEncodeFiles.h"
#include "DlgPageEncodeFinish.h"
#include "DlgPageDecode.h"
#include "DlgPageDecodeFinish.h"

class CDlgARBHelp : public CPropertySheet
{
	DECLARE_DYNAMIC(CDlgARBHelp)
public:
	CDlgARBHelp(bool decode);

	void AddRegistryInfo(LPCTSTR inData);
	void SetARBFileStatus(CString const& inFileName, bool bInclude = true);
	typedef std::map<CString, bool> FileMap;
	FileMap const& GetARBFiles() const			{return m_IncFile;}
	void SendIt();

// Implementation
protected:
	HICON m_hIcon;
	bool m_Decode;
	CDlgHelpPage1 m_pageEncode;
	CDlgPageEncodeFiles m_pageFiles;
	CDlgPageEncodeFinish m_pageEncodeFinish;
	CDlgPageDecode m_pageDecode;
	CDlgPageDecodeFinish m_pageDecodeFinish;
	tstring m_RegInfo;
	FileMap m_IncFile;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
