#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgExistingPoints class
 * @author David Connet
 *
 * Revision History
 * 2016-04-29 Separate lifetime points from title (run) points.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-02-03 Created
 */

#include "ARB/ARBDogExistingPoints.h"
#include "ARBCommon/ARBDate.h"
#include "LibARBWin/Logger.h"
class wxDateEvent;
class wxDatePickerCtrl;


namespace dconSoft
{
class CAgilityBookDoc;
class CDlgPointsDivisionData;
class CDlgPointsEventData;
class CDlgPointsLevelData;
class CDlgPointsMultiQData;
class CDlgPointsTypeNameData;
class CDlgPointsVenueData;


class CDlgExistingPoints : public wxDialog
{
public:
	CDlgExistingPoints(
		CAgilityBookDoc* pDoc,
		ARB::ARBDogExistingPointsList& points,
		ARB::ARBDogExistingPointsPtr const& inExistingPoints,
		wxWindow* pParent = nullptr);

private:
	ARB::ARBExistingPointType GetCurrentType() const;
	CDlgPointsVenueData* GetVenueData(int index) const;
	CDlgPointsDivisionData* GetDivisionData(int index) const;
	CDlgPointsLevelData* GetLevelData(int index) const;
	CDlgPointsEventData* GetEventData(int index) const;
	CDlgPointsTypeNameData* GetTypeNameData(int index) const;
	CDlgPointsMultiQData* GetMultiQData(int index) const;
	ARB::ARBConfigScoringPtr GetConfigScoring() const;
	void GetEnableLists(
		ARB::ARBExistingPointType type,
		bool& outVenue,
		bool& outDivMQ,
		bool& outLevel,
		bool& outEvent,
		bool& outSubName,
		bool& outTypeName);
	void SetEnableLists(
		bool& outVenue,
		bool& outDivMQ,
		bool& outLevel,
		bool& outEvent,
		bool& outSubName,
		bool& outTypeName);
	void UpdateControls();
	void FillVenues();
	void FillDivMultiQ();
	void FillLevels();
	void FillEvents();
	void FillSubNames();
	void FillTypeNames();

	CAgilityBookDoc* m_pDoc;
	ARB::ARBDogExistingPointsList& m_PointsList;
	ARB::ARBDogExistingPointsPtr m_pExistingPoints;
	ARBCommon::ARBDate m_Date;
	double m_Points;
	wxString m_Comments;
	wxDatePickerCtrl* m_ctrlDate;
	wxComboBox* m_ctrlType;
	wxComboBox* m_ctrlVenues;
	wxString m_TextVenue;
	wxStaticText* m_textDivMultiQs;
	wxComboBox* m_ctrlDivMultiQs;
	wxString m_TextDivMultiQ;
	wxComboBox* m_ctrlLevels;
	wxString m_TextLevel;
	wxComboBox* m_ctrlEvents;
	wxString m_TextEvent;
	wxComboBox* m_ctrlSubNames;
	wxString m_TextSubName;
	wxComboBox* m_ctrlTypeNames;
	wxString m_TextTypeName;
	wxSizer* m_sdbSizer;
	ARBWin::CStackLogger m_trace;

	void OnDateChanged(wxDateEvent& evt);
	void OnSelchangeType(wxCommandEvent& evt);
	void OnSelchangeVenue(wxCommandEvent& evt);
	void OnSelchangeDivMultiQ(wxCommandEvent& evt);
	void OnSelchangeLevel(wxCommandEvent& evt);
	void OnSelchangeEvent(wxCommandEvent& evt);
	void OnOk(wxCommandEvent& evt);
};

} // namespace dconSoft
