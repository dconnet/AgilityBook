#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgPageEncodeFiles class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-01-02 DRC Created
 */

#include <wx/wizard.h>
class CDlgARBHelp;


class CDlgPageEncodeFiles : public wxWizardPageSimple
{
public:
	CDlgPageEncodeFiles(CDlgARBHelp* pParent);

	virtual bool TransferDataToWindow();
	virtual bool TransferDataFromWindow();

private:
	CDlgARBHelp* m_Parent;
	wxCheckListBox* m_ctrlList;

	void OnCheckAll(wxCommandEvent& evt);
	void OnCheckNone(wxCommandEvent& evt);
};
