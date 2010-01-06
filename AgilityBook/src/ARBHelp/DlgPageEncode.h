#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See Licence.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgPageEncode class
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-01-02 DRC Created
 */

#include "ARBTypes.h"
#include <wx/wizard.h>
class CDlgARBHelp;


class CDlgPageEncode : public wxWizardPageSimple
{
public:
	CDlgPageEncode(CDlgARBHelp* pParent);

	virtual bool TransferDataFromWindow();

private:
	void DumpGroup(
			wxString* data,
			wxString const& group,
			std::vector<wxString>* items);

	CDlgARBHelp* m_Parent;
	wxRadioBox* m_DiskChoices;
};
