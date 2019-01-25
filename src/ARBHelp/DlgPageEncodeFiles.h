#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPageEncodeFiles class
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include <wx/wizard.h>
class CDlgARBHelp;


class CDlgPageEncodeFiles : public wxWizardPageSimple
{
public:
	CDlgPageEncodeFiles(CDlgARBHelp* pParent);

	bool TransferDataToWindow() override;
	bool TransferDataFromWindow() override;

private:
	CDlgARBHelp* m_Parent;
	wxCheckListBox* m_ctrlList;

	void OnCheckAll(wxCommandEvent& evt);
	void OnCheckNone(wxCommandEvent& evt);
};
