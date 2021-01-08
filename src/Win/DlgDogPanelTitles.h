#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgDog class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated from DlgDog.cpp
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2003-08-18 Added a deceased date for a dog.
 */

#include "DlgDogPanelBase.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/ARBDate.h"
class CAgilityBookDoc;
class CReportListCtrl;
class wxDateEvent;
class wxDatePickerCtrl;

ARB_TYPEDEF(CDlgDogDataTitle)

class CDlgDogPanelTitles : public CDlgDogPanelReportBase
{
	friend class CDlgDogDataTitle;
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDogPanelTitles)
public:
	CDlgDogPanelTitles(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent);

	bool Save(unsigned int& hint) override;

private:
	void DoInitList() override;

	void DoNew() override;
	void DoEdit() override;
	void DoDelete() override;

	CDlgDogDataTitlePtr GetTitleData(long index) const;
	void ListTitles();

	int m_imgTitlesEmpty;
	int m_imgTitlesTitled;
	int m_imgTitlesTitledReceived;
	int m_imgTitlesHidden;
	int m_imgTitlesTitledHidden;
	int m_imgTitlesTitledHiddenReceived;

	ARBDogTitleList m_Titles;
	// CColumnOrder m_sortTitles;
	bool m_ViewHiddenTitles;

	void OnTitleHidden(wxCommandEvent& evt);
};
