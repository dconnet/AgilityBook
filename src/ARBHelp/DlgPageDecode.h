#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPageDecode class
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include <vector>
class CTextCtrl;


class CDlgPageDecode : public wxDialog
{
public:
	CDlgPageDecode();

private:
	CTextCtrl* m_ctrlEncoded;
	CTextCtrl* m_ctrlDecoded;
	std::vector<wxString> m_TmpFiles;

	DECLARE_EVENT_TABLE()
	void OnDecode(wxCommandEvent& evt);
	void OnCancel(wxCommandEvent& evt);
};
