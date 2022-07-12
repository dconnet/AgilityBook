#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPageEncode class
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include "ARBCommon/ARBTypes.h"
#include <wx/wizard.h>


namespace dconSoft
{
class CDlgARBHelp;


class CDlgPageEncode : public wxWizardPageSimple
{
public:
	CDlgPageEncode(CDlgARBHelp* pParent);

	bool TransferDataFromWindow() override;

private:
	CDlgARBHelp* m_Parent;
	wxRadioBox* m_DiskChoices;
};

} // namespace dconSoft
