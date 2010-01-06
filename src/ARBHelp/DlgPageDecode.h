#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgPageDecode class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-01-02 DRC Created
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
