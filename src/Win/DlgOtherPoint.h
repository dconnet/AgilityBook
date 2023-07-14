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
#include "LibARBWin/Logger.h"


namespace dconSoft
{
namespace ARBWin
{
class CRichEditCtrl2;
} // namespace ARBWin


class CDlgOtherPoint : public wxDialog
{
public:
	CDlgOtherPoint(ARB::ARBConfig& config, ARB::ARBDogRunOtherPointsPtr const& inRunOther, wxWindow* pParent = nullptr);

private:
	void LoadPoints(ARB::ARBConfigOtherPointsPtr const& inOther);

	ARB::ARBConfig& m_Config;
	ARB::ARBDogRunOtherPointsPtr m_pRunOther;
	wxComboBox* m_ctrlOtherPoints;
	ARBWin::CRichEditCtrl2* m_ctrlDesc;
	double m_Points;
	ARBWin::CStackLogger m_trace;

	DECLARE_EVENT_TABLE()
	void OnSelchangeOtherpoints(wxCommandEvent& evt);
	void OnOtherNew(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
