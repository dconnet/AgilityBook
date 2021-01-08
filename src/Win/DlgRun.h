#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2017-11-21 Update title points when InClass changes.
 * 2012-05-07 Added autocompletion to combo boxes.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-16 Merged DlgRun* into here.
 * 2009-02-09 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 */

#include "ARB/ARBTypes2.h"
class CAgilityBookDoc;
class CDlgRunPanelBase;
class CDlgRunPanelScore;


class CDlgRun : public wxDialog
{
	friend class CLinkDropTarget;

public:
	CDlgRun(
		CAgilityBookDoc* pDoc,
		ARBDogPtr const& inDog,
		ARBDogTrialPtr const& inTrial,
		ARBDogRunPtr const& inRun,
		wxWindow* pParent = nullptr,
		int iSelectPage = 0);

	std::wstring GetCurrentVenueName() const;

private:
	CAgilityBookDoc* m_pDoc;
	ARBDogTrialPtr m_pTrial;
	ARBDogRunPtr m_pRealRun;
	ARBDogRunPtr m_Run;

	CDlgRunPanelScore* m_panelScore;
	std::vector<CDlgRunPanelBase*> m_panels;

	DECLARE_ON_INIT()
	DECLARE_EVENT_TABLE()
	void OnPageChanging(wxBookCtrlEvent& evt);
	void OnPageChanged(wxBookCtrlEvent& evt);
	void OnOk(wxCommandEvent& evt);
};
