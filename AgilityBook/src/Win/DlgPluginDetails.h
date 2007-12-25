#pragma once

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
 * @brief Configure plugins for direct download of calendar data.
 * @author David Connet
 *
 * Revision History
 * @li 2007-12-24 DRC Created
 */

#include "ARBConfigCalSite.h"
#include "DlgBaseDialog.h"
class ARBConfig;

class CDlgPluginDetails : public CDlgBaseDialog
{
	DECLARE_DYNAMIC(CDlgPluginDetails)

public:
	CDlgPluginDetails(ARBConfig const& inConfig, ARBConfigCalSite& calSite, CWnd* pParent = NULL);
	virtual ~CDlgPluginDetails();

private:
// Dialog Data
	enum { IDD = IDD_CALENDAR_PLUGIN_DETAIL };
	CString m_strName;
	CString m_strDesc;
	CString m_strSearch;
	CString m_strHelp;
	CStatic m_ctrlCodes;
	ARBConfig const& m_Config;
	ARBConfigCalSite& m_OrigCalSite;
	ARBConfigCalSite m_CalSite;
	CString m_strCodes;

	void SetCodeText();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnPluginDetailCodes();
	virtual void OnOK();

	DECLARE_MESSAGE_MAP()
};
