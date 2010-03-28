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
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2004-03-30 DRC Created.
 */

class CTextCtrl;


class CDlgSelectURL : public wxDialog
{
public:
	CDlgSelectURL(
			wxString const& name,
			wxWindow* pParent = NULL);
	wxString GetName() const	{return m_Name;}

private:
	wxString m_Name;
	CTextCtrl* m_textCtrl;

	void OnFilename(wxCommandEvent& evt);
};
