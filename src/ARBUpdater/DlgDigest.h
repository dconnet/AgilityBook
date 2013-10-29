#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Generate a checksum for the given file.
 * @author David Connet
 *
 * Revision History
 * @li 2010-02-09 DRC Created
 */


class CDlgDigest : public wxDialog
{
public:
	CDlgDigest(wxString const& inFile);

private:
	wxString m_File;
	wxString m_MD5;
	wxString m_SHA1;
	wxString m_SHA256;
	long m_Size;

	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
