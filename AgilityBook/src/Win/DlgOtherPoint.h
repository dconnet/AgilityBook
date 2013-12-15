#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgOtherPoint class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
class CRichEditCtrl2;


class CDlgOtherPoint : public wxDialog
{
public:
	CDlgOtherPoint(
			ARBConfig& config,
			ARBDogRunOtherPointsPtr pRunOther,
			wxWindow* pParent = NULL);

private:
	void LoadPoints(ARBConfigOtherPointsPtr inOther);

	ARBConfig& m_Config;
	ARBDogRunOtherPointsPtr m_pRunOther;
	wxComboBox* m_ctrlOtherPoints;
	CRichEditCtrl2* m_ctrlDesc;
	double m_Points;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnSelchangeOtherpoints(wxCommandEvent& evt);
	void OnOtherNew(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
