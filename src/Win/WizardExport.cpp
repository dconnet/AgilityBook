/*
 * Copyright © 2003-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
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

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookCalendarListView.h"
#include "AgilityBookTrainingView.h"
#include "ARBConfigScoring.h"
#include "ARBDog.h"
#include "ARBDogRun.h"
#include "ARBDogTrial.h"
#include "ARBLocalization.h"
#include "DlgAssignColumns.h"
#include "DlgProgress.h"
#include "Globals.h"
#include "Wizard.h"
#include <fstream>


// Exporting by array is much faster, but formatting is better by cell.
//#define EXPORT_BY_ARRAY


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
	, m_ctrlAssign(NULL)
	, m_ctrlDateFormat(NULL)
	, m_ctrlPreview(NULL)
{
	Connect(wxEVT_WIZARD_PAGE_CHANGING, wxWizardEventHandler(CWizardExport::OnWizardChanging));
	Connect(wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler(CWizardExport::OnWizardChanged));

	CAgilityBookOptions::GetImportExportDelimiters(false, m_Delim, m_Delimiter);

	bool showDelims = true;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		showDelims = false;

	// Controls (these are done first to control tab order)

	wxRadioButton* ctrlTab = NULL;
	wxRadioButton* ctrlColon = NULL;
	wxRadioButton* ctrlComma = NULL;
	wxRadioButton* ctrlSpace = NULL;
	wxRadioButton* ctrlSemicolon = NULL;
	wxRadioButton* ctrlOther = NULL;
	wxTextCtrl* ctrlOtherChar = NULL;

	if (showDelims)
	{
		ctrlTab = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_TAB"),
			wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		ctrlTab->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimTab), NULL, this);
		ctrlTab->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));
		ctrlTab->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));

		ctrlColon = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_COLON"),
			wxDefaultPosition, wxDefaultSize, 0);
		ctrlColon->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimColon), NULL, this);
		ctrlColon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));
		ctrlColon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));

		ctrlComma = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_COMMA"),
			wxDefaultPosition, wxDefaultSize, 0);
		ctrlComma->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimComma), NULL, this);
		ctrlComma->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));
		ctrlComma->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));

		ctrlSpace = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_SPACE"),
			wxDefaultPosition, wxDefaultSize, 0);
		ctrlSpace->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimSpace), NULL, this);
		ctrlSpace->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));
		ctrlSpace->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));

		ctrlSemicolon = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_SEMI"),
			wxDefaultPosition, wxDefaultSize, 0);
		ctrlSemicolon->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimSemicolon), NULL, this);
		ctrlSemicolon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));
		ctrlSemicolon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));

		ctrlOther = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_EXPORT_DELIM_OTHER"),
			wxDefaultPosition, wxDefaultSize, 0);
		ctrlOther->Connect(wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler(CWizardExport::OnDelimOther), NULL, this);
		ctrlOther->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));
		ctrlOther->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));

		ctrlOtherChar = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
			wxDefaultPosition, wxSize(30, -1), 0,
			wxGenericValidator(&m_Delimiter));
		ctrlOtherChar->SetMaxLength(1); 
		ctrlOtherChar->Connect(wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler(CWizardExport::OnExportDelim), NULL, this);
		ctrlOtherChar->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM"));
		ctrlOtherChar->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM"));
	}

	m_ctrlAssign = new wxButton(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_ASSIGN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAssign->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CWizardExport::OnExportAssign), NULL, this);
	m_ctrlAssign->SetHelpText(_("HIDC_WIZARD_EXPORT_ASSIGN"));
	m_ctrlAssign->SetToolTip(_("HIDC_WIZARD_EXPORT_ASSIGN"));

	wxStaticText* textFormat = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_DATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textFormat->Wrap(-1);

	m_ctrlDateFormat = new wxComboBox(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_READONLY); 
	static struct
	{
		wxChar const* uFormat;
		ARBDate::DateFormat format;
		ARBDate::DateFormat extendedFormat;
	} const sc_Dates[] =
	{
		{wxT("IDS_DATEFORMAT_DASH_MDY"),
			ARBDate::eDashMDY, ARBDate::eDashMMDDYYYY},
		{wxT("IDS_DATEFORMAT_SLASH_MDY"),
			ARBDate::eSlashMDY, ARBDate::eSlashMMDDYYYY},
		{wxT("IDS_DATEFORMAT_DASH_YMD"),
			ARBDate::eDashYMD, ARBDate::eDashYYYYMMDD},
		{wxT("IDS_DATEFORMAT_SLASH_YMD"),
			ARBDate::eSlashYMD, ARBDate::eSlashYYYYMMDD},
		{wxT("IDS_DATEFORMAT_DASH_DMY"),
			ARBDate::eDashDMY, ARBDate::eDashDDMMYYYY},
		{wxT("IDS_DATEFORMAT_SLASH_DMY"),
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
	m_ctrlDateFormat->Connect(wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler(CWizardExport::OnSelchangeDate), NULL, this);
	m_ctrlDateFormat->SetHelpText(_("HIDC_WIZARD_EXPORT_DATE"));
	m_ctrlDateFormat->SetToolTip(_("HIDC_WIZARD_EXPORT_DATE"));

	wxStaticText* textPreview = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_EXPORT_PREVIEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	textPreview->Wrap(-1);

	m_ctrlPreview = new wxListCtrl(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER|wxLC_REPORT);
	m_ctrlPreview->SetHelpText(_("HIDC_WIZARD_EXPORT_PREVIEW"));
	m_ctrlPreview->SetToolTip(_("HIDC_WIZARD_EXPORT_PREVIEW"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerOptions = new wxBoxSizer(wxHORIZONTAL);

	if (showDelims)
	{
		wxStaticBoxSizer* sizerDelimiters = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_GROUP")), wxVERTICAL);

		wxFlexGridSizer* sizerDelim2 = new wxFlexGridSizer(2, 3, 0, 0);
		sizerDelim2->SetFlexibleDirection(wxBOTH);
		sizerDelim2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
		sizerDelim2->Add(ctrlTab, 0, wxLEFT|wxRIGHT|wxTOP, 5);
		sizerDelim2->Add(ctrlColon, 0, wxLEFT|wxRIGHT|wxTOP, 5);
		sizerDelim2->Add(ctrlComma, 0, wxLEFT|wxRIGHT|wxTOP, 5);
		sizerDelim2->Add(ctrlSpace, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
		sizerDelim2->Add(ctrlSemicolon, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

		wxBoxSizer* sizerOther = new wxBoxSizer(wxHORIZONTAL);
		sizerOther->Add(ctrlOther, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
		sizerOther->Add(ctrlOtherChar, 0, wxALL, 5);

		sizerDelim2->Add(sizerOther, 1, wxEXPAND, 5);

		sizerDelimiters->Add(sizerDelim2, 1, wxEXPAND, 5);

		sizerOptions->Add(sizerDelimiters, 0, wxEXPAND, 5);
	}

	wxBoxSizer* sizerAssign = new wxBoxSizer(wxHORIZONTAL);
	sizerAssign->Add(m_ctrlAssign, 0, wxALIGN_BOTTOM|wxALL, 5);

	wxBoxSizer* sizerFormat = new wxBoxSizer(wxHORIZONTAL);
	sizerFormat->Add(textFormat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerFormat->Add(m_ctrlDateFormat, 0, wxALL, 5);

	sizerAssign->Add(sizerFormat, 0, wxALIGN_TOP, 5);

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


wxString CWizardExport::GetDelim() const
{
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		return wxEmptyString;
	wxString delim;
	switch (m_Delim)
	{
	default:
	case CAgilityBookOptions::eDelimTab: delim = wxT("\t"); break;
	case CAgilityBookOptions::eDelimSpace: delim = wxT(" "); break;
	case CAgilityBookOptions::eDelimColon: delim = wxT(":"); break;
	case CAgilityBookOptions::eDelimSemicolon: delim = wxT(";"); break;
	case CAgilityBookOptions::eDelimComma: delim = wxT(","); break;
	case CAgilityBookOptions::eDelimOther: delim = m_Delimiter; break;
	}
	return delim;
}


wxString CWizardExport::PrepFieldOutput(wxChar const* inStr) const
{
	wxString delim = GetDelim();
	bool bAddQuotes = false;
	wxString fld(inStr);
	if (!delim.IsEmpty() && (0 <= fld.Find(delim) || 0 <= fld.Find('"')))
	{
		bAddQuotes = true;
		if (0 <= fld.Find('"'))
			fld.Replace(wxT("\""), wxT("\"\""));
	}
	if (bAddQuotes)
	{
		fld = wxT("\"") + fld;
		fld += wxT("\"");
	}
	return fld;
}


void CWizardExport::UpdateButtons()
{
	// Some tests to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		bOk = true;
	else if (1 == GetDelim().length())
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
	// TODO: Add option to allow CRs?
	inData.Replace(wxT("\n"), wxT(" "));
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
		data += PrepFieldOutput(inData);
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
	wxString delim = GetDelim();
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
	&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle()
	&& delim.IsEmpty())
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
			data += PrepFieldOutput(cols[iCol]);
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
									switch (columns[idxType][idx])
									{
									case IO_RUNS_REG_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetRegisteredName().c_str());
										break;
									case IO_RUNS_CALL_NAME:
										data += AddPreviewData(iLine, idx, pDog->GetCallName().c_str());
										break;
									case IO_RUNS_DATE:
										data += AddPreviewData(iLine, idx, pRun->GetDate().GetString(format).c_str());
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
												fld += (*iter)->GetVenue().c_str();
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
												fld += (*iter)->GetName().c_str();
											}
											data += AddPreviewData(iLine, idx, fld);
										}
										break;
									case IO_RUNS_LOCATION:
										data += AddPreviewData(iLine, idx, pTrial->GetLocation().c_str());
										break;
									case IO_RUNS_TRIAL_NOTES:
										data += AddPreviewData(iLine, idx, pTrial->GetNote().c_str());
										break;
									case IO_RUNS_DIVISION:
										data += AddPreviewData(iLine, idx, pRun->GetDivision().c_str());
										break;
									case IO_RUNS_LEVEL:
										data += AddPreviewData(iLine, idx, pRun->GetLevel().c_str());
										break;
									case IO_RUNS_EVENT:
										data += AddPreviewData(iLine, idx, pRun->GetEvent().c_str());
										break;
									case IO_RUNS_HEIGHT:
										data += AddPreviewData(iLine, idx, pRun->GetHeight().c_str());
										break;
									case IO_RUNS_JUDGE:
										data += AddPreviewData(iLine, idx, pRun->GetJudge().c_str());
										break;
									case IO_RUNS_HANDLER:
										data += AddPreviewData(iLine, idx, pRun->GetHandler().c_str());
										break;
									case IO_RUNS_CONDITIONS:
										data += AddPreviewData(iLine, idx, pRun->GetConditions().c_str());
										break;
									case IO_RUNS_COURSE_FAULTS:
										{
											otstringstream str;
											str << pRun->GetScoring().GetCourseFaults();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_TIME:
										data += AddPreviewData(iLine, idx, ARBDouble::str(pRun->GetScoring().GetTime()).c_str());
										break;
									case IO_RUNS_YARDS:
										data += AddPreviewData(iLine, idx, ARBDouble::str(pRun->GetScoring().GetYards(), 3).c_str());
										break;
									case IO_RUNS_MIN_YPS:
										{
											double yps;
											if (pRun->GetScoring().GetMinYPS(CAgilityBookOptions::GetTableInYPS(), yps))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::str(yps, 3).c_str());
											}
										}
										break;
									case IO_RUNS_YPS:
										{
											double yps;
											if (pRun->GetScoring().GetYPS(CAgilityBookOptions::GetTableInYPS(), yps))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::str(yps, 3).c_str());
											}
										}
										break;
									case IO_RUNS_OBSTACLES:
										{
											short ob = pRun->GetScoring().GetObstacles();
											if (0 < ob)
											{
												otstringstream str;
												str << ob;
												data += AddPreviewData(iLine, idx, str.str().c_str());
											}
										}
										break;
									case IO_RUNS_OPS:
										{
											double ops;
											if (pRun->GetScoring().GetObstaclesPS(CAgilityBookOptions::GetTableInYPS(), ops))
											{
												data += AddPreviewData(iLine, idx, ARBDouble::str(ops, 3).c_str());
											}
										}
										break;
									case IO_RUNS_SCT:
										data += AddPreviewData(iLine, idx, ARBDouble::str(pRun->GetScoring().GetSCT()).c_str());
										break;
									case IO_RUNS_TOTAL_FAULTS:
										{
											if (ARBDogRunScoring::eTypeByTime == pRun->GetScoring().GetType())
											{
												double faults = pRun->GetScoring().GetCourseFaults() + pRun->GetScoring().GetTimeFaults(pScoring);
												data += AddPreviewData(iLine, idx, ARBDouble::str(faults, 3).c_str());
											}
										}
										break;
									case IO_RUNS_REQ_OPENING:
										{
											otstringstream str;
											str << pRun->GetScoring().GetNeedOpenPts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_REQ_CLOSING:
										{
											otstringstream str;
											str << pRun->GetScoring().GetNeedClosePts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_OPENING:
										{
											otstringstream str;
											str << pRun->GetScoring().GetOpenPts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_CLOSING:
										{
											otstringstream str;
											str << pRun->GetScoring().GetClosePts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_REQ_POINTS:
										{
											otstringstream str;
											str << pRun->GetScoring().GetNeedOpenPts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_POINTS:
										{
											otstringstream str;
											str << pRun->GetScoring().GetOpenPts();
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_PLACE:
										{
											otstringstream str;
											short place = pRun->GetPlace();
											if (0 > place)
												str << '?';
											else if (0 == place)
												str << '-';
											else
												str << place;
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_IN_CLASS:
										{
											otstringstream str;
											short inClass = pRun->GetInClass();
											if (0 >= inClass)
												str << '?';
											else
												str << inClass;
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_DOGSQD:
										{
											otstringstream str;
											short qd = pRun->GetDogsQd();
											if (0 > qd)
												str << '?';
											else
												str << qd;
											data += AddPreviewData(iLine, idx, str.str().c_str());
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
															q += wxT('/');
														q += (*iMultiQ)->GetShortName().c_str();
													}
												}
												if (ARB_Q::eQ_SuperQ == pRun->GetQ())
												{
													if (!q.IsEmpty())
														q += wxT('/');
													q += _("IDS_SQ");
												}
											}
											if (q.IsEmpty())
												q = pRun->GetQ().str().c_str();
											data += AddPreviewData(iLine, idx, q);
										}
										break;
									case IO_RUNS_SCORE:
										if (pRun->GetQ().Qualified()
										|| ARB_Q::eQ_NQ == pRun->GetQ())
										{
											data += AddPreviewData(iLine, idx, ARBDouble::str(pRun->GetScore(pScoring)).c_str());
										}
										break;
									case IO_RUNS_TITLE_POINTS:
										{
											otstringstream str;
											double pts = 0.0;
											if (pRun->GetQ().Qualified())
												pts = pRun->GetTitlePoints(pScoring);
											str << pts;
											data += AddPreviewData(iLine, idx, str.str().c_str());
										}
										break;
									case IO_RUNS_COMMENTS:
										data += AddPreviewData(iLine, idx, pRun->GetNote().c_str());
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
												fld += (*iter).c_str();
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
						data += AddPreviewData(iLine, idx, pCal->GetStartDate().GetString(format).c_str());
						break;
					case IO_CAL_END_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format).c_str());
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
							data += AddPreviewData(iLine, idx, Localization()->CalendarEntered().c_str());
							break;
						case ARBCalendar::ePlanning:
							data += AddPreviewData(iLine, idx, Localization()->CalendarPlanning().c_str());
							break;
						}
						break;
					case IO_CAL_LOCATION:
						data += AddPreviewData(iLine, idx, pCal->GetLocation().c_str());
						break;
					case IO_CAL_CLUB:
						data += AddPreviewData(iLine, idx, pCal->GetClub().c_str());
						break;
					case IO_CAL_VENUE:
						data += AddPreviewData(iLine, idx, pCal->GetVenue().c_str());
						break;
					case IO_CAL_OPENS:
						date = pCal->GetOpeningDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format).c_str());
						break;
					case IO_CAL_CLOSES:
						date = pCal->GetClosingDate();
						if (date.IsValid())
							data += AddPreviewData(iLine, idx, date.GetString(format).c_str());
						break;
					case IO_CAL_NOTES:
						data += AddPreviewData(iLine, idx, pCal->GetNote().c_str());
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
						data += AddPreviewData(iLine, idx, pCal->GetGenericName().c_str());
						break;
					case IO_CAL_APPT_START_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetStartDate().GetString(format).c_str());
						break;
					case IO_CAL_APPT_START_TIME:
						data += AddPreviewData(iLine, idx, wxT(""));
						break;
					case IO_CAL_APPT_END_DATE:
						data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format).c_str());
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
								tmp += Localization()->CalendarTentative().c_str();
								tmp += wxT(" ");
							}
							switch (pCal->GetEntered())
							{
							default:
							case ARBCalendar::eNot:
								tmp += Localization()->CalendarStatusN().c_str();
								tmp += wxT(" ");
								break;
							case ARBCalendar::eEntered:
								tmp += Localization()->CalendarStatusE().c_str();
								tmp += wxT(" ");
								break;
							case ARBCalendar::ePlanning:
								tmp += Localization()->CalendarStatusP().c_str();
								tmp += wxT(" ");
								break;
							}
							date = pCal->GetOpeningDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarOpens().c_str();
								tmp += wxT(" ");
								tmp += date.GetString(format).c_str();
								tmp += wxT(" ");
							}
							date = pCal->GetClosingDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarCloses().c_str();
								tmp += wxT(" ");
								tmp += date.GetString(format).c_str();
								tmp += wxT(" ");
							}
							tmp += pCal->GetNote().c_str();
							data += AddPreviewData(iLine, idx, tmp);
						}
						break;
					case IO_CAL_APPT_LOCATION:
						data += AddPreviewData(iLine, idx, pCal->GetLocation().c_str());
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
						data += AddPreviewData(iLine, idx, pCal->GetGenericName().c_str());
						break;
					case IO_CAL_TASK_START_DATE:
						data += AddPreviewData(iLine, idx, dateStart.GetString(format).c_str());
						break;
					case IO_CAL_TASK_DUE_DATE:
						data += AddPreviewData(iLine, idx, dateDue.GetString(format).c_str());
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
								tmp += Localization()->CalendarTentative().c_str();
								tmp += wxT(" ");
							}
							date = pCal->GetOpeningDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarOpens().c_str();
								tmp += wxT(" ");
								tmp += date.GetString(format).c_str();
								tmp += wxT(" ");
							}
							date = pCal->GetClosingDate();
							if (date.IsValid())
							{
								tmp += Localization()->CalendarCloses().c_str();
								tmp += wxT(" ");
								tmp += date.GetString(format).c_str();
								tmp += wxT(" ");
							}
							tmp += wxString::Format(_("IDS_TRIAL_DATES"),
								pCal->GetStartDate().GetString(format).c_str(),
								pCal->GetEndDate().GetString(format).c_str());
							tmp += wxT(" ");
							tmp += pCal->GetNote().c_str();
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
						data += AddPreviewData(iLine, idx, pLog->GetDate().GetString(format).c_str());
						break;
					case IO_LOG_NAME:
						data += AddPreviewData(iLine, idx, pLog->GetName().c_str());
						break;
					case IO_LOG_SUBNAME:
						data += AddPreviewData(iLine, idx, pLog->GetSubName().c_str());
						break;
					case IO_LOG_NOTES:
						data += AddPreviewData(iLine, idx, pLog->GetNote().c_str());
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
	TransferDataFromWindow();
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
		wxMessageBox(_("IDS_SPECIFY_DATEFORMAT"), wxMessageBoxCaptionStr, wxICON_WARNING);
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
		IWizardExporterPtr pExporter;
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
#ifdef EXPORT_BY_ARRAY
			if (pExporter->CreateArray(m_ctrlPreview->GetItemCount(), nColumnCount))
			{
#else
				IDlgProgress* pProgress = IDlgProgress::CreateProgress(1, this);
				pProgress->EnableCancel(false);
				pProgress->SetMessage(_("IDS_EXPORTING"));
				pProgress->SetRange(1, m_ctrlPreview->GetItemCount() * nColumnCount);
				pProgress->ShowProgress();

#endif
				for (long i = 0; i < m_ctrlPreview->GetItemCount(); ++i)
				{
					for (long iCol = 0; iCol < nColumnCount; ++iCol)
					{
						wxString line = GetListColumnText(m_ctrlPreview, i, iCol);
#ifdef EXPORT_BY_ARRAY
						pExporter->InsertArrayData(i, iCol, line);
#else
						pExporter->InsertData(i, iCol, line);
						// Calc is started visibly, so steal focus back.
						if (0 == i && 0 == iCol)
							pProgress->SetForegroundWindow();
						pProgress->StepIt(1);
#endif
					}
				}
#ifdef EXPORT_BY_ARRAY
				if (!pExporter->ExportDataArray())
				{
					wxMessageBox(_("IDS_ERRORS_DURING_EXPORT"), wxMessageBoxCaptionStr, wxICON_EXCLAMATION);
				}
			}
			else
			{
				wxMessageBox(_("IDS_ERRORS_DURING_EXPORT"), wxMessageBoxCaptionStr, wxICON_EXCLAMATION);
			}
#else
			pExporter->AutoFit(0, nColumnCount-1);
			pProgress->Dismiss();
#endif
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
			wxMessageBox(_("IDS_EXPORT_FAILED"), wxMessageBoxCaptionStr, wxICON_STOP);
			return false;
		}
	}
	else
	{
		wxFileDialog file(this,
			wxEmptyString, // caption
			wxEmptyString, // def dir
			wxEmptyString,
			_("IDS_FILEEXT_FILTER_TXTCSV"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (wxID_OK == file.ShowModal())
		{
			wxBusyCursor wait;
			std::string filename(tstringUtil::tstringA(file.GetPath().c_str()));
			std::ofstream output(filename.c_str(), std::ios::out);
			output.exceptions(std::ios_base::badbit);
			if (output.is_open())
			{
				for (long i = 0; i < m_ctrlPreview->GetItemCount(); ++i)
				{
					wxString line = GetListColumnText(m_ctrlPreview, i, 0);
					output << tstringUtil::tstringA(line.c_str()) << std::endl;
				}
				output.close();
			}
			return true;
		}
		else
		{
			return false;
		}
	}
}
