#pragma once

/*
 * Copyright © 2007-2009 David Connet. All Rights Reserved.
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
 * @brief interface of the CDlgQueryDetail class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2007-12-24 DRC Created
 */

//#include "ComboBox.h"
//#include "DlgBaseDialog.h"
//#include <map>
class ARBConfig;


class CDlgQueryDetail
{
public:
	CDlgQueryDetail(
			wxChar const* inCode,
			wxChar const* inName,
			wxWindow* pParent = NULL,
			ARBConfig const* inConfig = NULL); // Only set when using venues
	int ShowModal()
	{
		wxMessageBox(wxT("CDlgQueryDetail"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_INFORMATION);
		return wxID_CANCEL;
	}
};

#if 0
class CDlgQueryDetail : public CDlgBaseDialog
{
public:
	CDlgQueryDetail(
			const wxChar* inCode,
			const wxChar* inName,
			CWnd* pParent = NULL,
			ARBConfig const* inConfig = NULL); // Only set when using venues

	tstring const& GetCode() const		{return m_Code;}
	tstring const& GetName() const		{return m_Name;}

private:
// Dialog Data
	//{{AFX_DATA(CDlgQueryDetail)
	enum { IDD = IDD_QUERY_DETAIL };
	CEdit m_ctrlCode;
	CString m_strCode;
	CStatic m_ctrlNameText;
	CEdit m_ctrlName;
	CString m_strName;
	CVenueComboBox m_ctrlVenues;
	CString m_strVenue;
	//}}AFX_DATA
	ARBConfig const* m_Config;
	tstring m_Code;
	tstring m_Name;

	//{{AFX_VIRTUAL(CDlgQueryDetail)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CDlgQueryDetail)
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeQuerydetailCombo();
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif
