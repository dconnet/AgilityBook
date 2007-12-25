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

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgPluginDetails.h"

#include "DlgCalendarQueryDetail.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDlgPluginDetails, CDlgBaseDialog)


CDlgPluginDetails::CDlgPluginDetails(
		ARBConfig const& inConfig,
		ARBConfigCalSite& calSite,
		CWnd* pParent)
	: CDlgBaseDialog(CDlgPluginDetails::IDD, pParent)
	, m_Config(inConfig)
	, m_OrigCalSite(calSite)
	, m_CalSite(calSite)
	, m_strName(calSite.GetName().c_str())
	, m_strDesc(calSite.GetDescription().c_str())
	, m_strSearch(calSite.GetSearchURL().c_str())
	, m_strHelp(calSite.GetHelpURL().c_str())
{
}


CDlgPluginDetails::~CDlgPluginDetails()
{
}


void CDlgPluginDetails::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_PLUGINDETAIL_NAME, m_strName);
	DDX_Text(pDX, IDC_PLUGINDETAIL_DESC, m_strDesc);
	DDX_Text(pDX, IDC_PLUGINDETAIL_SEARCH, m_strSearch);
	DDX_Text(pDX, IDC_PLUGINDETAIL_HELP, m_strHelp);
	DDX_Control(pDX, IDC_PLUGINDETAIL_CODES_TEXT, m_ctrlCodes);
}


BEGIN_MESSAGE_MAP(CDlgPluginDetails, CDlgBaseDialog)
	ON_BN_CLICKED(IDC_PLUGINDETAIL_CODES, OnPluginDetailCodes)
END_MESSAGE_MAP()


void CDlgPluginDetails::SetCodeText()
{
	CString str;
	str.FormatMessage(m_strCodes, static_cast<int>(m_CalSite.LocationCodes().size()), static_cast<int>(m_CalSite.VenueCodes().size()));
	m_ctrlCodes.SetWindowText(str);
}

// CDlgPluginDetails message handlers

BOOL CDlgPluginDetails::OnInitDialog() 
{
	CDlgBaseDialog::OnInitDialog();
	m_ctrlCodes.GetWindowText(m_strCodes);
	SetCodeText();
	return TRUE;	// return TRUE unless you set the focus to a control
					// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgPluginDetails::OnPluginDetailCodes()
{
	CDlgCalendarQueryDetail dlg(m_Config, m_CalSite.LocationCodes(), m_CalSite.VenueCodes(), this);
	if (IDOK == dlg.DoModal())
	{
		m_CalSite.RemoveAllLocationCodes();
		std::map<tstring, tstring>::const_iterator i;
		for (i = dlg.GetLocationCodes().begin(); i != dlg.GetLocationCodes().end(); ++i)
		{
			m_CalSite.AddLocationCode(i->first, i->second);
		}
		m_CalSite.RemoveAllVenueCodes();
		for (i = dlg.GetVenueCodes().begin(); i != dlg.GetVenueCodes().end(); ++i)
		{
			m_CalSite.AddVenueCode(i->first, i->second);
		}
		SetCodeText();
	}
}


void CDlgPluginDetails::OnOK() 
{
	if (!UpdateData(TRUE))
		return;
	m_CalSite.SetName((LPCTSTR)m_strName);
	m_CalSite.SetDescription((LPCTSTR)m_strDesc);
	m_CalSite.SetSearchURL((LPCTSTR)m_strSearch);
	m_CalSite.SetHelpURL((LPCTSTR)m_strHelp);
	m_OrigCalSite = m_CalSite;
	CDlgBaseDialog::OnOK();
}
