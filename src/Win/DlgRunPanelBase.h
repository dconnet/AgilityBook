#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Base class for run panels
 * @author David Connet
 */

#include "ARB/ARBTypes2.h"
#include <wx/panel.h>


namespace dconSoft
{
class CAgilityBookDoc;
class CDlgRun;


class CDlgRunPanelBase : public wxPanel
{
public:
	CDlgRunPanelBase(
		CDlgRun* pDlg,
		CAgilityBookDoc* pDoc,
		ARB::ARBDogPtr const& inDog,
		ARB::ARBDogTrialPtr const& inTrial,
		ARB::ARBDogRunPtr const& inRun,
		wxWindow* parent);

	virtual wxWindow* GetInitialControl() = 0;
	virtual void OnActivation() = 0;
	virtual bool Validate() = 0;
	virtual bool Save() = 0;

protected:
	CDlgRun* m_pDlg;
	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogPtr m_pDog;
	ARB::ARBDogTrialPtr m_pTrial;
	ARB::ARBDogRunPtr m_Run;
};

} // namespace dconSoft
