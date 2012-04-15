#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgSelectURL class
 * @author David Connet
 *
 * Revision History
 * @li 2012-02-24 DRC Changed GetName to Name (GetName is wxDialog api)
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-03-30 DRC Created.
 */

class CTextCtrl;


class CDlgSelectURL : public wxDialog
{
public:
	CDlgSelectURL(
			std::wstring const& name,
			wxWindow* pParent = NULL);

	std::wstring Name() const;

private:
	wxString m_Name;
	CTextCtrl* m_textCtrl;

	DECLARE_ON_INIT()
	void OnFilename(wxCommandEvent& evt);
};
