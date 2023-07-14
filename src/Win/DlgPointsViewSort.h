#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgPointsViewSort class
 * @author David Connet
 *
 * Revision History
 * 2009-02-09 Ported to wxWidgets.
 * 2006-07-16 Created
 */

#include "AgilityBookOptions.h"
#include "LibARBWin/Logger.h"


namespace dconSoft
{


class CDlgPointsViewSort : public wxDialog
{
public:
	CDlgPointsViewSort(wxWindow* pParent = nullptr);

private:
	void UpdateControls();
	void FillPrimary();
	void FillSecondary();
	void FillTertiary();

	wxComboBox* m_ctrlPrimary;
	wxComboBox* m_ctrlSecondary;
	wxComboBox* m_ctrlTertiary;
	wxButton* m_ctrlOk;
	ARBPointsViewSort m_Primary;
	ARBPointsViewSort m_Secondary;
	ARBPointsViewSort m_Tertiary;
	ARBWin::CStackLogger m_trace;

	void OnSelchangePrimary(wxCommandEvent& evt);
	void OnSelchangeSecondary(wxCommandEvent& evt);
	void OnSelchangeTertiary(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
