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


class CDlgPointsViewSort : public wxDialog
{
public:
	CDlgPointsViewSort(wxWindow* pParent = nullptr);

private:
	wxComboBox* m_ctrlPrimary;
	wxComboBox* m_ctrlSecondary;
	wxComboBox* m_ctrlTertiary;
	wxButton* m_ctrlOk;

	ARBPointsViewSort m_Primary;
	ARBPointsViewSort m_Secondary;
	ARBPointsViewSort m_Tertiary;

protected:
	void UpdateControls();
	void FillPrimary();
	void FillSecondary();
	void FillTertiary();

	DECLARE_EVENT_TABLE()
	void OnSelchangePrimary(wxCommandEvent& evt);
	void OnSelchangeSecondary(wxCommandEvent& evt);
	void OnSelchangeTertiary(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
