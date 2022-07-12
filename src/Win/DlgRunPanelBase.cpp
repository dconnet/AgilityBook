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

#include "stdafx.h"
#include "DlgRunPanelBase.h"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;

CDlgRunPanelBase::CDlgRunPanelBase(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: wxPanel(parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
	, m_pDlg(pDlg)
	, m_pDoc(pDoc)
	, m_pDog(inDog)
	, m_pTrial(inTrial)
	, m_Run(inRun)
{
}

} // namespace dconSoft
