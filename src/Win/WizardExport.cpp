/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2011-01-08 DRC Fixed export (was writing unicode instead of utf8)
 *                    Also columns didn't always line up.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-24 DRC Removed (unused) define to export by array.
 * @li 2009-07-14 DRC Fixed group box creation order.
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2004-09-07 DRC Time+Fault scoring shouldn't include time faults.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-05-10 DRC Place quotes are field on output as needed.
 * @li 2004-01-04 DRC Added date format specification.
 * @li 2003-12-30 DRC Fixed a bug exporting the training log.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "WizardExport.h"

#include "AgilityBookCalendarListView.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTrainingView.h"
#include "ARBConfigScoring.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ARBLocalization.h"
#include "BreakLine.h"
#include "DlgAssignColumns.h"
#include "DlgProgress.h"
#include "Globals.h"
#include "ListCtrl.h"
#include "Wizard.h"
#include <wx/valgen.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CWizardExport, wxWizardPageSimple)


CWizardExport::CWizardExport(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc,
		wxWizardPage* prev)
	: wxWizardPageSimple(pSheet, prev)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
	, m_Delim(CAgilityBookOptions::eDelimTab)
	, m_Delimiter(wxT(":"))
	, m_boxDelimiters(NULL)
	, m_ctrlTab(NULL)
	, m_ctrlColon(NULL)
	, m_ctrlComma(NULL)
	, m_ctrlSpace(NULL)
	, m_ctrlSemicolon(NULL)
	, m_ctrlOther(NULL)
	, m_ctrlOtherChar(NULL)
	, m_ctrlAssign(NULL)
	, m_ctrlDateFormat(NULL)
	, m_ctrlPreview(NULL)
{
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGING, wxWizardEventHandler, CWizardExport::OnWizardChanging);
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler, CWizardExport::OnWizardChanged);

	CAgilityBookOptions::GetImportExportDelimiters(false, m_Delim, m_Delimiter);

	// Controls (these are done first to control tab order)

	m_boxDelimiters = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_GROUP"));

	m_ctrlTab = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_TAB"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlTab, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimTab);
	m_ctrlTab->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));
	m_ctrlTab->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));

	m_ctrlColon = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_COLON"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlColon, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimColon);
	m_ctrlColon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));
	m_ctrlColon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));

	m_ctrlComma = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_COMMA"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlComma, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimComma);
	m_ctrlComma->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));
	m_ctrlComma->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));

	m_ctrlSpace = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_SPACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlSpace, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimSpace);
	m_ctrlSpace->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));
	m_ctrlSpace->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));

	m_ctrlSemicolon = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_SEMI"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlSemicolon, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimSemicolon);
	m_ctrlSemicolon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));
	m_ctrlSemicolon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));

	m_ctrlOther = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_OTHER"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlOther, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardExport::OnDelimOther);
	m_ctrlOther->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));
	m_ctrlOther->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));

	switch (m_Delim)
	{
	default:
	case CAgilityBookOptions::eDelimTab:
		m_ctrlTab->SetValue(true);
		break;
	case CAgilityBookOptions::eDelimSpace:
		m_ctrlSpace->SetValue(true);
		break;
	case CAgilityBookOptions::eDelimColon:
		m_ctrlColon->SetValue(true);
		break;
	case CAgilityBookOptions::eDelimSemicolon:
		m_ctrlSemicolon->SetValue(true);
		break;
	case CAgilityBookOptions::eDelimComma:
		m_ctrlComma->SetValue(true);
		break;
	case CAgilityBookOptions::eDelimOther:
		m_ctrlOther->SetValue(true);
		break;
	}

	m_ctrlOtherChar = new CTextCtrl(this, wxID_ANY, wxString(),
		wxDefaultPosition, wxSize(30, -1), 0,
		wxGenericValidator(&m_Delimiter));
	m_ctrlOtherChar->SetMaxLength(1); 
	BIND_OR_CONNECT_CTRL(m_ctrlOtherChar, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler, CWizardExport::OnExportDelim);
	m_ctrlOtherChar->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM"));
	m_ctrlOtherChar->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM"));

	m_ctrlAssign = new wxButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_ASSIGN"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlAssign, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CWizardExport::OnExportAssign);
	m_ctrlAssign->SetHelpText(_("HIDC_WIZARD_EXPORT_ASSIGN"));
	m_ctrlAssign->SetToolTip(_("HIDC_WIZARD_EXPORT_ASSIGN"));

	wxStaticText* textFormat = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textFormat->Wrap(-1);

	m_ctrlDateFormat = new wxComboBox(this, wxID_ANY, wxString(),
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_READONLY); 
	static struct
	{
		wxChar const* uFormat;
		ARBDate::DateFormat format;
		ARBDate::DateFormat extendedFormat;
	} const sc_Dates[] =
	{
		{arbT("IDS_DATEFORMAT_DASH_MDY"),
			ARBDate::eDashMDY, ARBDate::eDashMMDDYYYY},
		{arbT("IDS_DATEFORMAT_SLASH_MDY"),
			ARBDate::eSlashMDY, ARBDate::eSlashMMDDYYYY},
		{arbT("IDS_DATEFORMAT_DASH_YMD"),
			ARBDate::eDashYMD, ARBDate::eDashYYYYMMDD},
		{arbT("IDS_DATEFORMAT_SLASH_YMD"),
			ARBDate::eSlashYMD, ARBDate::eSlashYYYYMMDD},
		{arbT("IDS_DATEFORMAT_DASH_DMY"),
			ARBDate::eDashDMY, ARBDate::eDashDDMMYYYY},
		{arbT("IDS_DATEFORMAT_SLASH_DMY"),
			ARBDate::eSlashDMY, ARBDate::eSlashDDMMYYYY},
	};
	static size_t const sc_nDates = sizeof(sc_Dates) / sizeof(sc_Dates[0]);
	ARBDate::DateFormat format;
	CAgilityBookOptions::GetImportExportDateFormat(false, format);
	for (size_t i = 0; i < sc_nDates; ++i)
	{
		long index = m_ctrlDateFormat->Append(wxGetTranslation(sc_Dates[i].uFormat));
		m_ctrlDateFormat->SetClientData(index, (void*)sc_Dates[i].format);
		if (sc_Dates[i].format == format || sc_Dates[i].extendedFormat == format)
			m_ctrlDateFormat->SetSelection(index);
	}
	if (0 > m_ctrlDateFormat->GetSelection())
		m_ctrlDateFormat->SetSelection(0);
	BIND_OR_CONNECT_CTRL(m_ctrlDateFormat, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CWizardExport::OnSelchangeDate);
	m_ctrlDateFormat->SetHelpText(_("HIDC_WIZARD_EXPORT_DATE"));
	m_ctrlDateFormat->SetToolTip(_("HIDC_WIZARD_EXPORT_DATE"));

	wxStaticText* textPreview = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_PREVIEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPreview->Wrap(-1);

	m_ctrlPreview = new CListCtrl(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT);
	m_ctrlPreview->SetHelpText(_("HIDC_WIZARD_EXPORT_PREVIEW"));
	m_ctrlPreview->SetToolTip(_("HIDC_WIZARD_EXPORT_PREVIEW"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerOptions = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sizerDelimiters = new wxStaticBoxSizer(m_boxDelimiters, wxVERTICAL);

	wxFlexGridSizer* sizerDelim2 = new wxFlexGridSizer(2, 3, 0, 0);
	sizerDelim2->SetFlexibleDirection(wxBOTH);
	sizerDelim2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDelim2->Add(m_ctrlTab, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerDelim2->Add(m_ctrlColon, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerDelim2->Add(m_ctrlComma, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerDelim2->Add(m_ctrlSpace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerDelim2->Add(m_ctrlSemicolon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* sizerOther = new wxBoxSizer(wxHORIZONTAL);
	sizerOther->Add(m_ctrlOther, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerOther->Add(m_ctrlOtherChar, 0, wxALL, 5);

	sizerDelim2->Add(sizerOther, 1, wxEXPAND, 5);

	sizerDelimiters->Add(sizerDelim2, 1, wxEXPAND, 5);

	sizerOptions->Add(sizerDelimiters, 0, wxEXPAND, 5);

	wxBoxSizer* sizerAssign = new wxBoxSizer(wxHORIZONTAL);
	sizerAssign->Add(m_ctrlAssign, 0, wxALL, 5);

	wxBoxSizer* sizerFormat = new wxBoxSizer(wxHORIZONTAL);
	sizerFormat->Add(textFormat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerFormat->Add(m_ctrlDateFormat, 0, wxALL, 5);

	sizerAssign->Add(sizerFormat, 0, wxALIGN_TOP, 5);

	sizerOptions->Add(0, 0, 1, wxEXPAND, 5);
	sizerOptions->Add(sizerAssign, 0, wxEXPAND, 5);

	bSizer->Add(sizerOptions, 0, wxEXPAND, 5);
	bSizer->Add(textPreview, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(m_ctrlPreview, 1, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	bSizer->Fit(this);
}


CAgilityBookOptions::ColumnOrder CWizardExport::GetColumnInfo() const
{
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_EXPORT_RUNS:
		order = CAgilityBookOptions::eRunsExport;
		break;
	case WIZ_EXPORT_CALENDAR:
		order = CAgilityBookOptions::eCalExport;
		break;
	case WIZ_EXPORT_CALENDAR_APPT:
		order = CAgilityBookOptions::eCalExportAppt;
		break;
	case WIZ_EXPORT_CALENDAR_TASK:
		order = CAgilityBookOptions::eCalExportTask;
		break;
	case WIZ_EXPORT_LOG:
		order = CAgilityBookOptions::eLogExport;
		break;
	}
	return order;
}


wxChar CWizardExport::GetDelim() const
{
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		return 0;
	switch (m_Delim)
	{
	default:
	case CAgilityBookOptions::eDelimTab:       return wxT('\t');
	case CAgilityBookOptions::eDelimSpace:     return wxT(' ');
	case CAgilityBookOptions::eDelimColon:     return wxT(':');
	case CAgilityBookOptions::eDelimSemicolon: return wxT(';');
	case CAgilityBookOptions::eDelimComma:     return wxT(',');
	case CAgilityBookOptions::eDelimOther:
		if (1 == m_Delimiter.length())
			return m_Delimiter[0];
		else
			return 0;
	}
}


void CWizardExport::UpdateButtons()
{
	// Some tests to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		bOk = true;
	else if (0 != GetDelim())
		bOk = true;
	bool bEnable = false;
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;
	case WIZ_EXPORT_RUNS:
		order = CAgilityBookOptions::eRunsExport;
		break;
	case WIZ_EXPORT_CALENDAR:
		order = CAgilityBookOptions::eCalExport;
		break;
	case WIZ_EXPORT_CALENDAR_APPT:
		order = CAgilityBookOptions::eCalExportAppt;
		break;
	case WIZ_EXPORT_CALENDAR_TASK:
		order = CAgilityBookOptions::eCalExportTask;
		break;
	case WIZ_EXPORT_LOG:
		order = CAgilityBookOptions::eLogExport;
		break;
	}
	if (CAgilityBookOptions::eUnknown != order)
	{
		bEnable = true;
		if (bOk)
		{
			for (size_t i = 0; bOk && i < IO_TYPE_MAX; ++i)
			{
				std::vector<long> columns;
				if (CDlgAssignColumns::GetColumnOrder(order, i, columns))
				{
					if (0 == columns.size())
						bOk = false;
				}
			}
		}
	}

	m_ctrlAssign->Enable(bEnable);
	m_pSheet->UpdateButtons(bOk);
}


wxString CWizardExport::AddPreviewData(
		long inLine,
		long inCol,
		wxString inData)
{
	wxString data;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		if (0 == inCol)
			m_ctrlPreview->InsertItem(inLine, inData);
		else
			SetListColumnText(m_ctrlPreview, inLine, inCol, inData);
	}
	else
	{
		if (0 < inCol)
			data += GetDelim();
		data += WriteCSVField(GetDelim(), inData);
	}
	return data;
}


void CWizardExport::UpdatePreview()
{
	wxBusyCursor wait;

	m_ctrlPreview->Freeze();

	// Clear existing preview data.
	m_ctrlPreview->DeleteAllItems();
	long nColumnCount = m_ctrlPreview->GetColumnCount();
	long iCol;
	for (iCol = 0; iCol < nColumnCount; ++iCol)
		m_ctrlPreview->DeleteColumn(0);

	// Get export info.
	ARBDate::DateFormat format = ARBDate::eSlashMDY;
	long idxDateFormat = m_ctrlDateFormat->GetSelection();
	if (wxNOT_FOUND != idxDateFormat)
		format = static_cast<ARBDate::DateFormat>((int)m_ctrlDateFormat->GetClientData(idxDateFormat));
	wxChar delim = GetDelim();
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
	&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle()
	&& 0 == delim)
	{
		wxString nodelim(_("IDS_NO_DELIM_SPECIFIED"));
		m_ctrlPreview->InsertColumn(0, wxT(""));
		m_ctrlPreview->InsertItem(0, nodelim);
		m_ctrlPreview->SetColumnWidth(0, wxLIST_AUTOSIZE_USEHEADER);
		m_ctrlPreview->Thaw();
		m_ctrlPreview->Refresh();
		return;
	}

	// Get export columns.
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t index;
	std::vector<long> columns[IO_TYPE_MAX];
	for (index = 0; index < IO_TYPE_MAX; ++index)
	{
		CDlgAssignColumns::GetColumnOrder(order, index, columns[index]);
	}

	// Now generate the header information.
	wxString hdrSep(wxT("/"));
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		hdrSep = wxT("\n");
	std::vector<wxString> cols;

	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;
	case WIZ_EXPORT_RUNS:
		for (index = 0; index < IO_TYPE_MAX; ++index)
		{
			if (0 == columns[index].size())
				continue;
			for (iCol = 0; iCol < static_cast<long>(columns[index].size()); ++iCol)
			{
				wxString str = CDlgAssignColumns::GetNameFromColumnID(columns[index][iCol]);
				if (iCol >= static_cast<long>(cols.size()))
					cols.push_back(str);
				else
				{
					if (cols[iCol] != str && 0 < str.length())
						cols[iCol] += hdrSep + str;
				}
			}
		}
		break;
	case WIZ_EXPORT_CALENDAR:
		for (index = 0; index < columns[IO_TYPE_CALENDAR].size(); ++index)
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][index]);
			cols.push_back(str);
		}
		break;
	case WIZ_EXPORT_CALENDAR_APPT:
		for (index = 0; index < columns[IO_TYPE_CALENDAR_APPT].size(); ++index)
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR_APPT][index]);
			cols.push_back(str);
		}
		break;
	case WIZ_EXPORT_CALENDAR_TASK:
		for (index = 0; index < columns[IO_TYPE_CALENDAR_TASK].size(); ++index)
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR_TASK][index]);
			cols.push_back(str);
		}
		break;

	case WIZ_EXPORT_LOG:
		for (index = 0; index < columns[IO_TYPE_TRAINING].size(); ++index)
		{
			wxString str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][index]);
			cols.push_back(str);
		}
	}

	// Now start writing the data.
	long iLine = 0;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
			m_ctrlPreview->InsertColumn(iCol, wxT(""));
		m_ctrlPreview->InsertItem(iLine, cols[0]);
		for (iCol = 1; iCol < static_cast<long>(cols.size()); ++iCol)
			SetListColumnText(m_ctrlPreview, iLine, iCol, cols[iCol]);
		++iLine;
	}
	else
	{
		m_ctrlPreview->InsertColumn(0, wxT(""));
		wxString data;
		for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
		{
			if (0 < iCol)
				data += delim;
			data += WriteCSVField(delim, cols[iCol]);
		}
		m_ctrlPreview->InsertItem(iLine, data);
		++iLine;
	}

	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;

	case WIZ_EXPORT_RUNS:
		{
			for (ARBDogList::const_iterator iterDog = m_pDoc->Book().GetDogs().begin(); iterDog != m_pDoc->Book().GetDogs().end(); ++iterDog)
			{
				ARBDogPtr pDog = *iterDog;
				for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin(); iterTrial != pDog->GetTrials().end(); ++iterTrial)
				{
					ARBDogTrialPtr pTrial = *iterTrial;
					for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin(); iterRun != pTrial->GetRuns().end(); ++iterRun)
					{
						ARBDogRunPtr pRun = *iterRun;
						ARBConfigScoringPtr pScoring;
						if (pTrial->GetClubs().GetPrimaryClub())
							m_pDoc->Book().GetConfig().GetVenues().FindEvent(
								pTrial->GetClubs().GetPrimaryClubVenue(),
								pRun->GetEvent(),
								pRun->GetDivision(),
								pRun->GetLevel(),
								pRun->GetDate(),
								NULL,
								&pScoring);
						assert(pScoring);
						if (pScoring)
						{
							long idxType = -1;
							switch (pScoring->GetScoringStyle())
							{
							case ARBConfigScoring::eFaultsThenTime:
							case ARBConfigScoring::eFaults100ThenTime:
							case ARBConfigScoring::eFaults200ThenTime:
								idxType = IO_TYPE_RUNS_FAULTS_TIME;
								break;
							case ARBConfigScoring::eOCScoreThenTime:
								idxType = IO_TYPE_RUNS_OPEN_CLOSE;
								break;
							case ARBConfigScoring::eScoreThenTime:
								idxType = IO_TYPE_RUNS_POINTS;
								break;
							case ARBConfigScoring::eTimePlusFaults:
								idxType = IO_TYPE_RUNS_TIME_FAULTS;
								break;
							}
							assert(-1 != idxType);
							if (0 <= idxType)
							{
								wxString data;
								for (long idx = 0; idx < static_cast<long>(columns[idxType].size()); ++idx)
								{
									// Note: All columns must have data written
									// or export columns won't line up.
									switch (columns[idxType][idx])
									{
									default:
										data += AddPreviewData(iLine, idx, wxString());
										break;
									case IO_RUNS_REG_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetRegisteredName());
										break;
									case IO_RUNS_CALL_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetCallName());
										break;
									case IO_RUNS_DATE:
										data += AddPreviewData(iLine, idx, pRun->GetDate().GetString(format));
										break;
									case IO_RUNS_VENUE:
										{
											wxString fld;
											long i = 0;
											for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
												iter != pTrial->GetClubs().end();
												++iter, ++i)
											{
												if (0 < i)
													fld += wxT("/");
												fld += (*iter)->GetVenue();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									case IO_RUNS_CLUB:
										{
											wxString fld;
											long i = 0;
											for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
												iter != pTrial->GetClubs().end();
												++iter, ++i)
											{
												if (0 < i)
													fld += wxT("/");
												fld += (*iter)->GetName();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									case IO_RUNS_LOCATION:
										data += AddPreviewData(iLine, idx, pTrial->GetLocation());
										break;
									case IO_RUNS_TRIAL_NOTES:
										data += AddPreviewData(iLine, idx, pTrial->GetNote());
										break;
									case IO_RUNS_DIVISION:
										data += AddPreviewData(iLine, idx, pRun->GetDivision());
										break;
									case IO_RUNS_LEVEL:
										data += AddPreviewData(iLine, idx, pRun->GetLevel());
										break;
									case IO_RUNS_EVENT:
										data += AddPreviewData(iLine, idx, pRun->GetEvent());
										break;
									case IO_RUNS_HEIGHT:
										data += AddPreviewData(iLine, idx, pRun->GetHeight());
										break;
									case IO_RUNS_JUDGE:
										data += AddPreviewData(iLine, idx, pRun->GetJudge());
										break;
									case IO_RUNS_HANDLER:
										data += AddPreviewData(iLine, idx, pRun->GetHandler());
										break;
									case IO_RUNS_CONDITIONS:
										data += AddPreviewData(iLine, idx, pRun->GetConditions());
										break;
									case IO_RUNS_COURSE_FAULTS:
										{
											wxString str;
											str << pRun->GetScoring().GetCourseFaults();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_TIME:
										data += AddPreviewData(iLine, idx, ARBDouble::ToString(pRun->GetScoring().GetTime()));
										break;
									case IO_RUNS_YARDS:
										data += AddPreviewData(iLine, idx, ARBDouble::ToString(pRun->GetScoring().GetYards(), 3));
										break;
									case IO_RUNS_MIN_YPS:
										{
											double yps;
											if (pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::ToString(yps, 3));
											}
											else
											{
												data += AddPreviewData(iLine, idx, wxString());
											}
										}
										break;
									case IO_RUNS_YPS:
										{
											double yps;
											if (pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::ToString(yps, 3));
											}
											else
											{
												data += AddPreviewData(iLine, idx, wxString());
											}
										}
										break;
									case IO_RUNS_OBSTACLES:
										{
											short ob = pRun->GetScoring().GetObstacles();
											if (0 < ob)
											{
												wxString str;
												str << ob;
												data += AddPreviewData(iLine, idx, str);
											}
											else
											{
												data += AddPreviewData(iLine, idx, wxString());
											}
										}
										break;
									case IO_RUNS_OPS:
										{
											double ops;
											if (pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::ToString(ops, 3));
											}
											else
											{
												data += AddPreviewData(iLine, idx, wxString());
											}
										}
										break;
									case IO_RUNS_SCT:
										data += AddPreviewData(iLine, idx, ARBDouble::ToString(pRun->GetScoring().GetSCT()));
										break;
									case IO_RUNS_TOTAL_FAULTS:
										{
											if (ARBDogRunScoring::eTypeByTime == pRun->GetScoring().GetType())
											{
												double faults = pRun->GetScoring().GetCourseFaults() + pRun->GetScoring().GetTimeFaults(pScoring);
												data += AddPreviewData(iLine, idx, ARBDouble::ToString(faults, 3));
											}
											else
											{
												data += AddPreviewData(iLine, idx, wxString());
											}
										}
										break;
									case IO_RUNS_REQ_OPENING:
										{
											wxString str;
											str << pRun->GetScoring().GetNeedOpenPts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_REQ_CLOSING:
										{
											wxString str;
											str << pRun->GetScoring().GetNeedClosePts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_OPENING:
										{
											wxString str;
											str << pRun->GetScoring().GetOpenPts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_CLOSING:
										{
											wxString str;
											str << pRun->GetScoring().GetClosePts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_REQ_POINTS:
										{
											wxString str;
											str << pRun->GetScoring().GetNeedOpenPts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_POINTS:
										{
											wxString str;
											str << pRun->GetScoring().GetOpenPts();
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_PLACE:
										{
											wxString str;
											short place = pRun->GetPlace();
											if (0 > place)
												str << wxT("?");
											else if (0 == place)
												str << wxT("-");
											else
												str << place;
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_IN_CLASS:
										{
											wxString str;
											short inClass = pRun->GetInClass();
											if (0 >= inClass)
												str << wxT("?");
											else
												str << inClass;
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_DOGSQD:
										{
											wxString str;
											short qd = pRun->GetDogsQd();
											if (0 > qd)
												str << wxT("?");
											else
												str << qd;
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_Q:
										{
											wxString q;
											if (pRun->GetQ().Qualified())
											{
												std::vector<ARBConfigMultiQPtr> multiQs;
												if (0 < pRun->GetMultiQs(multiQs))
												{
													for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin(); iMultiQ != multiQs.end(); ++iMultiQ)
													{
														if (!q.IsEmpty())
															q += wxT("/");
														q += (*iMultiQ)->GetShortName();
													}
												}
												if (ARB_Q::eQ_SuperQ == pRun->GetQ())
												{
													if (!q.IsEmpty())
														q += wxT("/");
													q += _("IDS_SQ");
												}
											}
											if (q.IsEmpty())
												q = pRun->GetQ().str();
											data += AddPreviewData(iLine, idx, q);
										}
										break;
									case IO_RUNS_SCORE:
										if (pRun->GetQ().Qualified()
										|| ARB_Q::eQ_NQ == pRun->GetQ())
										{
											data += AddPreviewData(iLine, idx, ARBDouble::ToString(pRun->GetScore(pScoring)));
										}
										else
										{
											data += AddPreviewData(iLine, idx, wxString());
										}
										break;
									case IO_RUNS_TITLE_POINTS:
										{
											wxString str;
											double pts = 0.0;
											if (pRun->GetQ().Qualified())
												pts = pRun->GetTitlePoints(pScoring);
											str << pts;
											data += AddPreviewData(iLine, idx, str);
										}
										break;
									case IO_RUNS_COMMENTS:
										data += AddPreviewData(iLine, idx, pRun->GetNote());
										break;
									case IO_RUNS_FAULTS:
										{
											wxString fld;
											long i = 0;
											for (ARBDogFaultList::const_iterator iter = pRun->GetFaults().begin();
												iter != pRun->GetFaults().end();
												++iter)
											{
												if (0 < i)
													fld += wxT("/");
												fld += *iter;
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									}
								}
								if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
								&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
									m_ctrlPreview->InsertItem(iLine, data);
								++iLine;
							}
						}
					}
				}
			}
		}
		break;

	case WIZ_EXPORT_CALENDAR:
		{
			for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin(); iterCal != m_pDoc->Book().GetCalendar().end(); ++iterCal)
			{
				wxString data;
				ARBCalendarPtr pCal = *iterCal;
				for (long idx = 0; idx < static_cast<long>(columns[IO_TYPE_CALENDAR].size()); ++idx)
				{
					ARBDate date;
					switch (columns[IO_TYPE_CALENDAR][idx])
					{
					case IO_CAL_START_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetStartDate().GetString(format));
						break;
					case IO_CAL_END_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format));
						break;
					case IO_CAL_TENTATIVE:
						if (pCal->IsTentative())
							data += AddPreviewData(iLine, idx, wxT("?"));
						break;
					case IO_CAL_ENTERED:
						switch (pCal->GetEntered())
						{
						default:
						case ARBCalendar::eNot:
							break;
						case ARBCalendar::eEntered:
							data += AddPreviewData(iLine, idx, Localization()->CalendarEntered());
							break;
						case ARBCalendar::ePending:
							data += AddPreviewData(iLine, idx, Localization()->CalendarPending());
							break;
						case ARBCalendar::ePlanning:
							data += AddPreviewData(iLine, idx, Localization()->CalendarPlanning());
							break;
						}
						break;
					case IO_CAL_LOCATION:
						data += AddPreviewData(iLine, idx, pCal->GetLocation());
						break;
					case IO_CAL_CLUB:
						data += AddPreviewData(iLine, idx, pCal->GetClub());
						break;
					case IO_CAL_VENUE:
						data += AddPreviewData(iLine, idx, pCal->GetVenue());
						break;
					case IO_CAL_OPENS:
						date = pCal->GetOpeningDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format));
						break;
					case IO_CAL_CLOSES:
						date = pCal->GetClosingDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format));
						break;
					case IO_CAL_NOTES:
						data += AddPreviewData(iLine, idx, pCal->GetNote());
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
					m_ctrlPreview->InsertItem(iLine, data);
				++iLine;
			}
		}
		break;

	case WIZ_EXPORT_CALENDAR_APPT:
		{
			std::vector<ARBCalendarPtr> allEntries;
			std::vector<ARBCalendarPtr>* entries = m_pSheet->GetCalendarEntries();
			if (!entries)
			{
				allEntries.reserve(m_pDoc->Book().GetCalendar().size());
				for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin(); iterCal != m_pDoc->Book().GetCalendar().end(); ++iterCal)
				{
					allEntries.push_back(*iterCal);
				}
				entries = &allEntries;
			}
			for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end(); ++iterCal)
			{
				wxString data;
				ARBCalendarPtr pCal = *iterCal;
				for (long idx = 0; idx < static_cast<long>(columns[IO_TYPE_CALENDAR_APPT].size()); ++idx)
				{
					ARBDate date;
					switch (columns[IO_TYPE_CALENDAR_APPT][idx])
					{
					case IO_CAL_APPT_SUBJECT:
						data += AddPreviewData(iLine, idx, pCal->GetGenericName());
						break;
					case IO_CAL_APPT_START_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetStartDate().GetString(format));
						break;
					case IO_CAL_APPT_START_TIME:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_END_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format));
						break;
					case IO_CAL_APPT_END_TIME:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_ALLDAY:
						data += AddPreviewData(iLine, idx, wxT("1"));
						break;
					case IO_CAL_APPT_REMINDER:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_REMINDER_DATE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_REMINDER_TIME:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_ORGANIZER:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_REQ_ATTENDEES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_OPT_ATTENDEES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_RESOURCES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_BILLING:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_CATEGORIES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_DESCRIPTION:
						{
							wxString tmp;
							if (pCal->IsTentative())
							{
								tmp += Localization()->CalendarTentative();
								tmp += wxT(" ");
							}
							switch (pCal->GetEntered())
							{
							default:
							case ARBCalendar::eNot:
								tmp += Localization()->CalendarStatusN();
								tmp += wxT(" ");
								break;
							case ARBCalendar::eEntered:
								tmp += Localization()->CalendarStatusE();
								tmp += wxT(" ");
								break;
							case ARBCalendar::ePending:
								tmp += Localization()->CalendarStatusO();
								tmp += wxT(" ");
								break;
							case ARBCalendar::ePlanning:
								tmp += Localization()->CalendarStatusP();
								tmp += wxT(" ");
								break;
							}
							date = pCal->GetOpeningDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarOpens();
								tmp += wxT(" ");
								tmp += date.GetString(format);
								tmp += wxT(" ");
							}
							date = pCal->GetClosingDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarCloses();
								tmp += wxT(" ");
								tmp += date.GetString(format);
								tmp += wxT(" ");
							}
							tmp += pCal->GetNote();
							data += AddPreviewData(iLine, idx, tmp);
						}
						break;
					case IO_CAL_APPT_LOCATION:
						data += AddPreviewData(iLine, idx, pCal->GetLocation());
						break;
					case IO_CAL_APPT_MILEAGE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_PRIORITY:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_PRIVATE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_SENSITIVITY:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_SHOW_TIME_AS:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
					m_ctrlPreview->InsertItem(iLine, data);
				++iLine;
			}
		}
		break;

	case WIZ_EXPORT_CALENDAR_TASK:
		{
			std::vector<ARBCalendarPtr> allEntries;
			std::vector<ARBCalendarPtr>* entries = m_pSheet->GetCalendarEntries();
			if (!entries)
			{
				allEntries.reserve(m_pDoc->Book().GetCalendar().size());
				for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin(); iterCal != m_pDoc->Book().GetCalendar().end(); ++iterCal)
				{
					allEntries.push_back(*iterCal);
				}
				entries = &allEntries;
			}
			for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end(); ++iterCal)
			{
				wxString data;
				ARBCalendarPtr pCal = *iterCal;
				if (ARBCalendar::ePlanning != pCal->GetEntered())
					continue;
				for (long idx = 0; idx < static_cast<long>(columns[IO_TYPE_CALENDAR_TASK].size()); ++idx)
				{
					ARBDate date;
					ARBDate dateStart = pCal->GetOpeningDate();
					if (!dateStart.IsValid())
						dateStart = pCal->GetStartDate();
					ARBDate dateDue = pCal->GetClosingDate();
					if (!dateDue.IsValid())
						dateDue = pCal->GetStartDate();
					if (dateStart > dateDue)
						dateStart = dateDue;
					dateStart -= CAgilityBookOptions::CalendarOpeningNear();
					switch (columns[IO_TYPE_CALENDAR_TASK][idx])
					{
					case IO_CAL_TASK_SUBJECT:
						data += AddPreviewData(iLine, idx, pCal->GetGenericName());
						break;
					case IO_CAL_TASK_START_DATE:
						data += AddPreviewData(iLine, idx, dateStart.GetString(format));
						break;
					case IO_CAL_TASK_DUE_DATE:
						data += AddPreviewData(iLine, idx, dateDue.GetString(format));
						break;
					case IO_CAL_TASK_REMINDER:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_REMINDER_DATE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_REMINDER_TIME:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_COMPLETED_DATE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_COMPLETE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_TOTAL_WORK:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_ACTUAL_WORK:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_BILLING:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_CATEGORIES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_COMPANIES:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_CONTACTS:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_MILEAGE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_NOTES:
						{
							wxString tmp;
							if (pCal->IsTentative())
							{
								tmp += Localization()->CalendarTentative();
								tmp += wxT(" ");
							}
							date = pCal->GetOpeningDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarOpens();
								tmp += wxT(" ");
								tmp += date.GetString(format);
								tmp += wxT(" ");
							}
							date = pCal->GetClosingDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarCloses();
								tmp += wxT(" ");
								tmp += date.GetString(format);
								tmp += wxT(" ");
							}
							tmp += wxString::Format(_("IDS_TRIAL_DATES"),
								pCal->GetStartDate().GetString(format).c_str(),
								pCal->GetEndDate().GetString(format).c_str());
							tmp += wxT(" ");
							tmp += pCal->GetNote();
							data += AddPreviewData(iLine, idx, tmp);
						}
						break;
					case IO_CAL_TASK_PRIORITY:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_PRIVATE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_ROLE:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_SCH_PRIORITY:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_SENSITIVITY:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_TASK_STATUS:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
					m_ctrlPreview->InsertItem(iLine, data);
				++iLine;
			}
		}
		break;

	case WIZ_EXPORT_LOG:
		{
			for (ARBTrainingList::const_iterator iterLog = m_pDoc->Book().GetTraining().begin(); iterLog != m_pDoc->Book().GetTraining().end(); ++iterLog)
			{
				wxString data;
				ARBTrainingPtr pLog = *iterLog;
				for (long idx = 0; idx < static_cast<long>(columns[IO_TYPE_TRAINING].size()); ++idx)
				{
					switch (columns[IO_TYPE_TRAINING][idx])
					{
					case IO_LOG_DATE:
						data += AddPreviewData(iLine, idx, pLog->GetDate().GetString(format));
						break;
					case IO_LOG_NAME:
						data += AddPreviewData(iLine, idx, pLog->GetName());
						break;
					case IO_LOG_SUBNAME:
						data += AddPreviewData(iLine, idx, pLog->GetSubName());
						break;
					case IO_LOG_NOTES:
						data += AddPreviewData(iLine, idx, pLog->GetNote());
						break;
					}
				}
				if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
					m_ctrlPreview->InsertItem(iLine, data);
				++iLine;
			}
		}
		break;
	}
	for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
		m_ctrlPreview->SetColumnWidth(iCol, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlPreview->Thaw();
	m_ctrlPreview->Refresh();
}


void CWizardExport::OnDelimTab(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimTab;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimSpace(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimSpace;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimColon(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimColon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimSemicolon(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimSemicolon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimComma(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimComma;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimOther(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimOther;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnExportDelim(wxCommandEvent& evt)
{
	if (m_ctrlOtherChar)
		m_Delimiter = m_ctrlOtherChar->GetValue();
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnExportAssign(wxCommandEvent& evt)
{
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;
	case WIZ_EXPORT_RUNS:
		order = CAgilityBookOptions::eRunsExport;
		break;
	case WIZ_EXPORT_CALENDAR:
		order = CAgilityBookOptions::eCalExport;
		break;
	case WIZ_EXPORT_CALENDAR_APPT:
		order = CAgilityBookOptions::eCalExportAppt;
		break;
	case WIZ_EXPORT_CALENDAR_TASK:
		order = CAgilityBookOptions::eCalExportTask;
		break;
	case WIZ_EXPORT_LOG:
		order = CAgilityBookOptions::eLogExport;
		break;
	}
	if (CAgilityBookOptions::eUnknown != order)
	{
		CDlgAssignColumns dlg(order, this);
		if (wxID_OK == dlg.ShowModal())
		{
			UpdateButtons();
			UpdatePreview();
		}
	}
}


void CWizardExport::OnSelchangeDate(wxCommandEvent& evt)
{
	UpdatePreview();
}


void CWizardExport::OnWizardChanging(wxWizardEvent& evt)
{
	if (evt.GetDirection())
	{
		if (!DoWizardFinish())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


void CWizardExport::OnWizardChanged(wxWizardEvent& evt)
{
	if (evt.GetPage() == static_cast<wxWizardPage*>(this))
	{
		m_pSheet->SetLabel(_("IDD_WIZARD_EXPORT"));

		bool showDelims = true;
		if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
		|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
			showDelims = false;
		bool visible = m_boxDelimiters->IsShown();
		if ((showDelims && !visible) || (!showDelims && visible))
		{
			m_boxDelimiters->Show(showDelims);
			m_ctrlTab->Show(showDelims);
			m_ctrlColon->Show(showDelims);
			m_ctrlComma->Show(showDelims);
			m_ctrlSpace->Show(showDelims);
			m_ctrlSemicolon->Show(showDelims);
			m_ctrlOther->Show(showDelims);
			m_ctrlOtherChar->Show(showDelims);
			Layout();
		}

		UpdateButtons();
		UpdatePreview();
	}
	evt.Skip();
}


bool CWizardExport::DoWizardFinish()
{
	long index = m_ctrlDateFormat->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SPECIFY_DATEFORMAT"), wxMessageBoxCaptionStr, wxOK | wxICON_WARNING);
		m_ctrlDateFormat->SetFocus();
		return false;
	}
	UpdatePreview();
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
	&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
	{
		CAgilityBookOptions::SetImportExportDelimiters(false, m_Delim, m_Delimiter);
	}
	ARBDate::DateFormat format = static_cast<ARBDate::DateFormat>((int)m_ctrlDateFormat->GetClientData(index));
	CAgilityBookOptions::SetImportExportDateFormat(false, format);

	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		ISpreadSheetExporterPtr pExporter;
		if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
		&& m_pSheet->ExcelHelper())
			pExporter = m_pSheet->ExcelHelper()->GetExporter();
		else if (WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle()
		&& m_pSheet->CalcHelper())
			pExporter = m_pSheet->CalcHelper()->GetExporter();
		if (pExporter)
		{
			wxBusyCursor wait;
			pExporter->AllowAccess(false);
			long nColumnCount = m_ctrlPreview->GetColumnCount();
			IDlgProgress* pProgress = IDlgProgress::CreateProgress(1, this);
			pProgress->EnableCancel(false);
			pProgress->SetMessage(_("IDS_EXPORTING"));
			pProgress->SetRange(1, m_ctrlPreview->GetItemCount() * nColumnCount);
			pProgress->ShowProgress();

			bool bFailed = false;
			for (long i = 0; !bFailed && i < m_ctrlPreview->GetItemCount(); ++i)
			{
				for (long iCol = 0; !bFailed && iCol < nColumnCount; ++iCol)
				{
					wxString line = GetListColumnText(m_ctrlPreview, i, iCol);
					bFailed = !pExporter->InsertData(i, iCol, line);
					// Calc is started visibly, so steal focus back.
					if (0 == i && 0 == iCol)
						pProgress->SetForegroundWindow();
					pProgress->StepIt(1);
				}
			}
			pExporter->AutoFit(0, nColumnCount-1);
			pProgress->Dismiss();
			pExporter->AllowAccess(true);
			// For testing
			//pExporter->SetBackColor(2,3,RGB(255,0,0));
			//pExporter->SetTextColor(2,3,RGB(0,255,0));
			//pExporter->SetItalic(1,1,true);
			//pExporter->SetBold(1,2,true);
			return true;
		}
		else
		{
			wxMessageBox(_("IDS_EXPORT_FAILED"), wxMessageBoxCaptionStr, wxOK | wxICON_STOP);
			return false;
		}
	}
	else
	{
		wxFileDialog file(this,
			wxString(), // caption
			wxString(), // def dir
			wxString(),
			_("IDS_FILEEXT_FILTER_TXTCSV"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (wxID_OK == file.ShowModal())
		{
			wxBusyCursor wait;
			wxFFileOutputStream output(file.GetPath(), wxT("wb"));
			if (output.IsOk())
			{
				for (long i = 0; i < m_ctrlPreview->GetItemCount(); ++i)
				{
					wxString line = GetListColumnText(m_ctrlPreview, i, 0);
					line << wxT("\n");
					std::string utf8(tstringUtil::tstringA(line));
					output.Write(utf8.c_str(), utf8.length());
				}
				output.Close();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}
