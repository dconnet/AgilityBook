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
 * 2006-07-15 Add a selected-item total.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Created.
 **/

#include "DlgDogPanelBase.h"

#include "ARB/ARBDog.h"
#include "ARBCommon/ARBDate.h"
class CAgilityBookDoc;
class CReportListCtrl;
class wxDateEvent;
class wxDatePickerCtrl;

ARB_TYPEDEF(CDlgDogDataPoint)

class CDlgDogPanelPoints : public CDlgDogPanelReportBase
{
	DECLARE_NO_COPY_IMPLEMENTED(CDlgDogPanelPoints)
public:
	CDlgDogPanelPoints(CAgilityBookDoc* pDoc, ARBDogPtr const& inDog, wxWindow* parent);

	bool Save(unsigned int& hint) override;

private:
	void DoInitList() override;
	void UpdateControls() override;

	void DoNew() override;
	void DoEdit() override;
	void DoDelete() override;

	CDlgDogDataPointPtr GetPointData(long index) const;
	void ListExistingPoints();

	void SetColumnPointsHeaders();
	void UpdatePointsButtons();
	void EditPoints();

	wxStaticText* m_ctrlSelectedPts;
	ARBDogExistingPointsList m_ExistingPoints;
};
