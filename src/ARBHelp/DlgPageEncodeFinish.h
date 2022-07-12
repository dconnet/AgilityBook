#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPageEncodeFinish class
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include <wx/wizard.h>


namespace dconSoft
{
class CDlgARBHelp;
namespace ARBWin
{
class CTextCtrl;
} // namespace ARBWin


class CDlgPageEncodeFinish : public wxWizardPageSimple
{
public:
	CDlgPageEncodeFinish(CDlgARBHelp* pParent);

	bool TransferDataToWindow() override;

private:
	CDlgARBHelp* m_Parent;
	ARBWin::CTextCtrl* m_ctrlText;
};

} // namespace dconSoft
