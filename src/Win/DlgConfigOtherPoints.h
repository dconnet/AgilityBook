#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgConfigOtherPoints class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes.h"


class CDlgConfigOtherPoints : public wxDialog
{
public:
	CDlgConfigOtherPoints(
			ARBConfig& config,
			ARBConfigOtherPointsPtr pOther,
			wxWindow* pParent = NULL);

	bool IsNameOkay(std::wstring const& name) const;

private:
	ARBConfig& m_Config;
	ARBConfigOtherPointsPtr m_pOther;
	wxString m_Name;
	wxComboBox* m_ctrlTally;
	double m_Default;
	wxString m_Desc;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnOk(wxCommandEvent& evt);
};
