#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Generate a checksum for the given file.
 * @author David Connet
 *
 * Revision History
 * 2010-02-09 Created
 */

#include "../Win/Localization.h"


class CDlgDigest : public wxDialog
{
public:
	CDlgDigest(wxString const& inFile);

private:
	CLocalization m_Localization;
	wxButton* m_ctrlInit;
	wxStaticText* m_ctrlConfig;
	wxStaticText* m_ctrlConfigVersion;
	wxButton* m_ctrlCopy;
	wxString m_Config;
	wxString m_File;
	wxString m_MD5;
	wxString m_SHA1;
	wxString m_SHA256;
	long m_Size;
	short m_ConfigVersion;

	bool Init();

	void OnInit(wxCommandEvent& evt);
	void OnBrowse(wxCommandEvent& evt);
	void OnCopy(wxCommandEvent& evt);
};
