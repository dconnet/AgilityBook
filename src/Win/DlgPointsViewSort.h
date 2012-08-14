#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CDlgPointsViewSort class
 * @author David Connet
 *
 * Revision History
 * @li 2009-02-09 DRC Ported to wxWidgets.
 * @li 2006-07-16 DRC Created
 */

#include "AgilityBookOptions.h"


class CDlgPointsViewSort : public wxDialog
{
public:
	CDlgPointsViewSort(wxWindow* pParent = NULL);

private:
	wxComboBox* m_ctrlPrimary;
	wxComboBox* m_ctrlSecondary;
	wxComboBox* m_ctrlTertiary;
	wxButton* m_ctrlOk;

	CAgilityBookOptions::PointsViewSort m_Primary;
	CAgilityBookOptions::PointsViewSort m_Secondary;
	CAgilityBookOptions::PointsViewSort m_Tertiary;

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
