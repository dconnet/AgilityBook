#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgConfigOtherPoints class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
#include "LibARBWin/Logger.h"


namespace dconSoft
{


class CDlgConfigOtherPoints : public wxDialog
{
public:
	CDlgConfigOtherPoints(
		ARB::ARBConfig& config,
		ARB::ARBConfigOtherPointsPtr const& inOther,
		wxWindow* pParent = nullptr);

	bool IsNameOkay(wxString const& name) const;

private:
	ARB::ARBConfig& m_Config;
	ARB::ARBConfigOtherPointsPtr m_pOther;
	wxString m_Name;
	wxComboBox* m_ctrlTally;
	double m_Default;
	wxString m_Desc;
	ARBWin::CStackLogger m_trace;

	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
