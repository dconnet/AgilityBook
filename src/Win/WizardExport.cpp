/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Export Wizard
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-09-04 Change default DogsInClass to -1 (allows for DNR runs with 0 dogs)
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-07-25 Adhere to RFC4180 and use CRLF between records.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2011-01-08 Fixed export (was writing unicode instead of utf8)
 *            Also columns didn't always line up.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-24 Removed (unused) define to export by array.
 * 2009-07-14 Fixed group box creation order.
 * 2009-06-14 Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *            _after_ the dialog is destroyed).
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2004-09-07 Time+Fault scoring shouldn't include time faults.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-05-10 Place quotes are field on output as needed.
 * 2004-01-04 Added date format specification.
 * 2003-12-30 Fixed a bug exporting the training log.
 * 2003-12-10 Created
 */

#include "stdafx.h"
#include "WizardExport.h"

#include "AgilityBookCalendarListView.h"
#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "AgilityBookTrainingView.h"
#include "DlgAssignColumns.h"
#include "Wizard.h"

#include "ARB/ARBConfigScoring.h"
#include "ARB/ARBDog.h"
#include "ARB/ARBDogRun.h"
#include "ARB/ARBDogTrial.h"
#include "ARB/ARBLocalization.h"
#include "ARBCommon/BreakLine.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgProgress.h"
#include "LibARBWin/ReportListCtrl.h"
#include <wx/valgen.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxIMPLEMENT_CLASS(CWizardExport, wxWizardPageSimple)


CWizardExport::CWizardExport(CWizard* pSheet, CAgilityBookDoc* pDoc, wxWizardPage* prev)
	: wxWizardPageSimple(pSheet, prev)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
	, m_Delim(ARBImportExportDelim::Tab)
	, m_Delimiter(L":")
	, m_boxDelimiters(nullptr)
	, m_ctrlTab(nullptr)
	, m_ctrlColon(nullptr)
	, m_ctrlComma(nullptr)
	, m_ctrlSpace(nullptr)
	, m_ctrlSemicolon(nullptr)
	, m_ctrlOther(nullptr)
	, m_ctrlOtherChar(nullptr)
	, m_ctrlAssign(nullptr)
	, m_ctrlDateFormat(nullptr)
	, m_ctrlPreview(nullptr)
{
	Bind(wxEVT_WIZARD_PAGE_CHANGING, &CWizardExport::OnWizardChanging, this);
	Bind(wxEVT_WIZARD_PAGE_CHANGED, &CWizardExport::OnWizardChanged, this);

	std::wstring tmp;
	CAgilityBookOptions::GetImportExportDelimiters(false, m_Delim, tmp);
	m_Delimiter = StringUtil::stringWX(tmp);

	// Controls (these are done first to control tab order)

	m_boxDelimiters = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_GROUP"));

	m_ctrlTab = new wxRadioButton(
		this,
		wxID_ANY,
		_("IDC_WIZARD_EXPORT_DELIM_TAB"),
		wxDefaultPosition,
		wxDefaultSize,
		wxRB_GROUP);
	m_ctrlTab->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimTab, this);
	m_ctrlTab->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));
	m_ctrlTab->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_TAB"));

	m_ctrlColon
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_COLON"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlColon->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimColon, this);
	m_ctrlColon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));
	m_ctrlColon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COLON"));

	m_ctrlComma
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_COMMA"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlComma->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimComma, this);
	m_ctrlComma->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));
	m_ctrlComma->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_COMMA"));

	m_ctrlSpace
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_SPACE"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSpace->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimSpace, this);
	m_ctrlSpace->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));
	m_ctrlSpace->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SPACE"));

	m_ctrlSemicolon
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_SEMI"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlSemicolon->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimSemicolon, this);
	m_ctrlSemicolon->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));
	m_ctrlSemicolon->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_SEMI"));

	m_ctrlOther
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DELIM_OTHER"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlOther->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardExport::OnDelimOther, this);
	m_ctrlOther->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));
	m_ctrlOther->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM_OTHER"));

	switch (m_Delim)
	{
	case ARBImportExportDelim::Tab:
		m_ctrlTab->SetValue(true);
		break;
	case ARBImportExportDelim::Space:
		m_ctrlSpace->SetValue(true);
		break;
	case ARBImportExportDelim::Colon:
		m_ctrlColon->SetValue(true);
		break;
	case ARBImportExportDelim::Semicolon:
		m_ctrlSemicolon->SetValue(true);
		break;
	case ARBImportExportDelim::Comma:
		m_ctrlComma->SetValue(true);
		break;
	case ARBImportExportDelim::Other:
		m_ctrlOther->SetValue(true);
		break;
	}

	m_ctrlOtherChar = new CTextCtrl(
		this,
		wxID_ANY,
		wxString(),
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 20), -1),
		0,
		wxGenericValidator(&m_Delimiter));
	m_ctrlOtherChar->SetMaxLength(1);
	m_ctrlOtherChar->Bind(wxEVT_COMMAND_TEXT_UPDATED, &CWizardExport::OnExportDelim, this);
	m_ctrlOtherChar->SetHelpText(_("HIDC_WIZARD_EXPORT_DELIM"));
	m_ctrlOtherChar->SetToolTip(_("HIDC_WIZARD_EXPORT_DELIM"));

	m_ctrlAssign = new wxButton(this, wxID_ANY, _("IDC_WIZARD_EXPORT_ASSIGN"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlAssign->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CWizardExport::OnExportAssign, this);
	m_ctrlAssign->SetHelpText(_("HIDC_WIZARD_EXPORT_ASSIGN"));
	m_ctrlAssign->SetToolTip(_("HIDC_WIZARD_EXPORT_ASSIGN"));

	wxStaticText* textFormat
		= new wxStaticText(this, wxID_ANY, _("IDC_WIZARD_EXPORT_DATE"), wxDefaultPosition, wxDefaultSize, 0);
	textFormat->Wrap(-1);

	m_ctrlDateFormat = new wxComboBox(
		this,
		wxID_ANY,
		wxString(),
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY);
	static struct
	{
		wchar_t const* uFormat;
		ARBDateFormat format;
		ARBDateFormat extendedFormat;
	} const sc_Dates[] = {
		{arbT("IDS_DATEFORMAT_DASH_MDY"), ARBDateFormat::DashMDY, ARBDateFormat::DashMMDDYYYY},
		{arbT("IDS_DATEFORMAT_SLASH_MDY"), ARBDateFormat::SlashMDY, ARBDateFormat::SlashMMDDYYYY},
		{arbT("IDS_DATEFORMAT_DASH_YMD"), ARBDateFormat::DashYMD, ARBDateFormat::DashYYYYMMDD},
		{arbT("IDS_DATEFORMAT_SLASH_YMD"), ARBDateFormat::SlashYMD, ARBDateFormat::SlashYYYYMMDD},
		{arbT("IDS_DATEFORMAT_DASH_DMY"), ARBDateFormat::DashDMY, ARBDateFormat::DashDDMMYYYY},
		{arbT("IDS_DATEFORMAT_SLASH_DMY"), ARBDateFormat::SlashDMY, ARBDateFormat::SlashDDMMYYYY},
	};
	constexpr size_t sc_nDates = sizeof(sc_Dates) / sizeof(sc_Dates[0]);
	ARBDateFormat format;
	CAgilityBookOptions::GetImportExportDateFormat(false, format);
	for (size_t i = 0; i < sc_nDates; ++i)
	{
		long index = m_ctrlDateFormat->Append(wxGetTranslation(sc_Dates[i].uFormat));
		m_ctrlDateFormat->SetClientData(index, reinterpret_cast<void*>(sc_Dates[i].format));
		if (sc_Dates[i].format == format || sc_Dates[i].extendedFormat == format)
			m_ctrlDateFormat->SetSelection(index);
	}
	if (0 > m_ctrlDateFormat->GetSelection())
		m_ctrlDateFormat->SetSelection(0);
	m_ctrlDateFormat->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CWizardExport::OnSelchangeDate, this);
	m_ctrlDateFormat->SetHelpText(_("HIDC_WIZARD_EXPORT_DATE"));
	m_ctrlDateFormat->SetToolTip(_("HIDC_WIZARD_EXPORT_DATE"));

	wxStaticText* textPreview
		= new wxStaticText(this, wxID_ANY, _("IDC_WIZARD_EXPORT_PREVIEW"), wxDefaultPosition, wxDefaultSize, 0);
	textPreview->Wrap(-1);

	m_ctrlPreview = new CListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_NO_HEADER | wxLC_REPORT);
	m_ctrlPreview->SetHelpText(_("HIDC_WIZARD_EXPORT_PREVIEW"));
	m_ctrlPreview->SetToolTip(_("HIDC_WIZARD_EXPORT_PREVIEW"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerOptions = new wxBoxSizer(wxHORIZONTAL);

	wxStaticBoxSizer* sizerDelimiters = new wxStaticBoxSizer(m_boxDelimiters, wxVERTICAL);

	wxFlexGridSizer* sizerDelim2 = new wxFlexGridSizer(2, 3, 0, 0);
	sizerDelim2->SetFlexibleDirection(wxBOTH);
	sizerDelim2->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerDelim2->Add(m_ctrlTab, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerDelim2->Add(m_ctrlColon, 0, wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerDelim2->Add(m_ctrlComma, 0, 0, 0);
	sizerDelim2->Add(m_ctrlSpace, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 3));
	sizerDelim2->Add(m_ctrlSemicolon, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 3));

	wxBoxSizer* sizerOther = new wxBoxSizer(wxHORIZONTAL);
	sizerOther->Add(m_ctrlOther, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerOther->Add(m_ctrlOtherChar, 0, wxALIGN_CENTER_VERTICAL, 0);

	sizerDelim2->Add(sizerOther, 1, wxEXPAND | wxALIGN_CENTER_VERTICAL | wxTOP, wxDLG_UNIT_X(this, 3));

	sizerDelimiters->Add(sizerDelim2, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 3));

	sizerOptions->Add(sizerDelimiters, 0, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerAssign = new wxBoxSizer(wxHORIZONTAL);
	sizerAssign->Add(m_ctrlAssign, 0, wxRIGHT, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerFormat = new wxBoxSizer(wxHORIZONTAL);
	sizerFormat->Add(textFormat, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerFormat->Add(m_ctrlDateFormat, wxALIGN_CENTER_VERTICAL, 0);

	sizerAssign->Add(sizerFormat, 0, wxALIGN_TOP, 0);

	sizerOptions->Add(0, 0, 1, wxEXPAND, 0);
	sizerOptions->Add(sizerAssign, 0, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 4));

	bSizer->Add(sizerOptions, 0, wxEXPAND, 0);
	bSizer->Add(textPreview, 0, wxTOP, wxDLG_UNIT_X(this, 5));
	bSizer->Add(m_ctrlPreview, 1, wxEXPAND, 0);

	SetSizer(bSizer);
	bSizer->Fit(this);
}


CAgilityBookOptions::ColumnOrder CWizardExport::GetColumnInfo() const
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
	return order;
}


wchar_t CWizardExport::GetDelim() const
{
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		return 0;
	switch (m_Delim)
	{
	case ARBImportExportDelim::Tab:
		return L'\t';
	case ARBImportExportDelim::Space:
		return L' ';
	case ARBImportExportDelim::Colon:
		return L':';
	case ARBImportExportDelim::Semicolon:
		return L';';
	case ARBImportExportDelim::Comma:
		return L',';
	case ARBImportExportDelim::Other:
		if (1 == m_Delimiter.length())
			return m_Delimiter[0];
		else
			return 0;
	}
	// 'enum class' handles all cases via the switch above
	return 0;
}


void CWizardExport::UpdateButtons()
{
	// Some tests to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
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


std::wstring CWizardExport::AddPreviewData(long inLine, long inCol, std::wstring inData)
{
	std::wstring data;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		if (0 == inCol)
			m_ctrlPreview->InsertItem(inLine, StringUtil::stringWX(inData));
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
	ARBDateFormat format = ARBDateFormat::SlashMDY;
	long idxDateFormat = m_ctrlDateFormat->GetSelection();
	if (wxNOT_FOUND != idxDateFormat)
		format = static_cast<ARBDateFormat>(reinterpret_cast<size_t>(m_ctrlDateFormat->GetClientData(idxDateFormat)));
	wchar_t delim = GetDelim();
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle() && WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle()
		&& 0 == delim)
	{
		wxString nodelim(_("IDS_NO_DELIM_SPECIFIED"));
		m_ctrlPreview->InsertColumn(0, L"");
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
	std::wstring hdrSep(L"/");
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		hdrSep = L"\r\n";
	std::vector<std::wstring> cols;

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
				std::wstring str = CDlgAssignColumns::GetNameFromColumnID(columns[index][iCol]);
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
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][index]);
			cols.push_back(str);
		}
		break;
	case WIZ_EXPORT_CALENDAR_APPT:
		for (index = 0; index < columns[IO_TYPE_CALENDAR_APPT].size(); ++index)
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR_APPT][index]);
			cols.push_back(str);
		}
		break;
	case WIZ_EXPORT_CALENDAR_TASK:
		for (index = 0; index < columns[IO_TYPE_CALENDAR_TASK].size(); ++index)
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR_TASK][index]);
			cols.push_back(str);
		}
		break;

	case WIZ_EXPORT_LOG:
		for (index = 0; index < columns[IO_TYPE_TRAINING].size(); ++index)
		{
			std::wstring str = CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][index]);
			cols.push_back(str);
		}
	}

	// Now start writing the data.
	long iLine = 0;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
			m_ctrlPreview->InsertColumn(iCol, L"");
		m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(cols[0]));
		for (iCol = 1; iCol < static_cast<long>(cols.size()); ++iCol)
			SetListColumnText(m_ctrlPreview, iLine, iCol, cols[iCol]);
		++iLine;
	}
	else
	{
		m_ctrlPreview->InsertColumn(0, L"");
		std::wstring data;
		for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
		{
			if (0 < iCol)
				data += delim;
			data += WriteCSVField(delim, cols[iCol]);
		}
		m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
		++iLine;
	}

	switch (m_pSheet->GetImportExportItem())
	{
	default:
		break;

	case WIZ_EXPORT_RUNS:
	{
		for (ARBDogList::const_iterator iterDog = m_pDoc->Book().GetDogs().begin();
			 iterDog != m_pDoc->Book().GetDogs().end();
			 ++iterDog)
		{
			ARBDogPtr pDog = *iterDog;
			for (ARBDogTrialList::const_iterator iterTrial = pDog->GetTrials().begin();
				 iterTrial != pDog->GetTrials().end();
				 ++iterTrial)
			{
				ARBDogTrialPtr pTrial = *iterTrial;
				for (ARBDogRunList::const_iterator iterRun = pTrial->GetRuns().begin();
					 iterRun != pTrial->GetRuns().end();
					 ++iterRun)
				{
					ARBDogRunPtr pRun = *iterRun;
					ARBConfigScoringPtr pScoring;
					if (pRun->GetClub())
						m_pDoc->Book().GetConfig().GetVenues().FindEvent(
							pRun->GetClub()->GetVenue(),
							pRun->GetEvent(),
							pRun->GetDivision(),
							pRun->GetLevel(),
							pRun->GetDate(),
							nullptr,
							&pScoring);
					assert(pScoring);
					if (pScoring)
					{
						long idxType = -1;
						switch (pScoring->GetScoringStyle())
						{
						case ARBScoringStyle::Unknown:
							break;
						case ARBScoringStyle::FaultsThenTime:
						case ARBScoringStyle::Faults100ThenTime:
						case ARBScoringStyle::Faults200ThenTime:
						case ARBScoringStyle::TimeNoPlaces:
							idxType = IO_TYPE_RUNS_FAULTS_TIME;
							break;
						case ARBScoringStyle::OCScoreThenTime:
							idxType = IO_TYPE_RUNS_OPEN_CLOSE;
							break;
						case ARBScoringStyle::ScoreThenTime:
							idxType = IO_TYPE_RUNS_POINTS;
							break;
						case ARBScoringStyle::TimePlusFaults:
							idxType = IO_TYPE_RUNS_TIME_FAULTS;
							break;
						}
						assert(-1 != idxType);
						if (0 <= idxType)
						{
							std::wstring data;
							for (long idx = 0; idx < static_cast<long>(columns[idxType].size()); ++idx)
							{
								// Note: All columns must have data written
								// or export columns won't line up.
								switch (columns[idxType][idx])
								{
								default:
									data += AddPreviewData(iLine, idx, std::wstring());
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
									std::wstring fld;
									long i = 0;
									for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
										 iter != pTrial->GetClubs().end();
										 ++iter, ++i)
									{
										if (0 < i)
											fld += L"/";
										fld += (*iter)->GetVenue();
									}
									data += AddPreviewData(iLine, idx, fld);
								}
								break;
								case IO_RUNS_CLUB:
								{
									std::wstring fld;
									long i = 0;
									for (ARBDogClubList::const_iterator iter = pTrial->GetClubs().begin();
										 iter != pTrial->GetClubs().end();
										 ++iter, ++i)
									{
										if (0 < i)
											fld += L"/";
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
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetCourseFaults()));
									break;
								case IO_RUNS_TIME:
									data += AddPreviewData(
										iLine,
										idx,
										ARBDouble::ToString(pRun->GetScoring().GetTime()));
									break;
								case IO_RUNS_YARDS:
									data += AddPreviewData(
										iLine,
										idx,
										ARBDouble::ToString(pRun->GetScoring().GetYards(), 3));
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
										data += AddPreviewData(iLine, idx, std::wstring());
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
										data += AddPreviewData(iLine, idx, std::wstring());
									}
								}
								break;
								case IO_RUNS_OBSTACLES:
								{
									short ob = pRun->GetScoring().GetObstacles();
									if (0 < ob)
									{
										data += AddPreviewData(iLine, idx, fmt::format(L"{}", ob));
									}
									else
									{
										data += AddPreviewData(iLine, idx, std::wstring());
									}
								}
								break;
								case IO_RUNS_OPS:
								{
									double ops;
									int prec;
									if (pRun->GetScoring().GetObstaclesPS(
											CAgilityBookOptions::GetTableInYPS(),
											CAgilityBookOptions::GetRunTimeInOPS(),
											ops,
											prec))
									{
										data += AddPreviewData(iLine, idx, ARBDouble::ToString(ops, prec));
									}
									else
									{
										data += AddPreviewData(iLine, idx, std::wstring());
									}
								}
								break;
								case IO_RUNS_SCT:
									data
										+= AddPreviewData(iLine, idx, ARBDouble::ToString(pRun->GetScoring().GetSCT()));
									break;
								case IO_RUNS_TOTAL_FAULTS:
								{
									if (ARBScoringType::ByTime == pRun->GetScoring().GetType())
									{
										double faults = pRun->GetScoring().GetCourseFaults()
														+ pRun->GetScoring().GetTimeFaults(pScoring);
										data += AddPreviewData(iLine, idx, ARBDouble::ToString(faults, 3));
									}
									else
									{
										data += AddPreviewData(iLine, idx, std::wstring());
									}
								}
								break;
								case IO_RUNS_REQ_OPENING:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetNeedOpenPts()));
									break;
								case IO_RUNS_REQ_CLOSING:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetNeedClosePts()));
									break;
								case IO_RUNS_OPENING:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetOpenPts()));
									break;
								case IO_RUNS_CLOSING:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetClosePts()));
									break;
								case IO_RUNS_REQ_POINTS:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetNeedOpenPts()));
									break;
								case IO_RUNS_POINTS:
									data += AddPreviewData(
										iLine,
										idx,
										fmt::format(L"{}", pRun->GetScoring().GetOpenPts()));
									break;
								case IO_RUNS_PLACE:
								{
									std::wstring str;
									short place = pRun->GetPlace();
									if (0 > place)
										str = L"?";
									else if (0 == place)
										str = L"-";
									else
										str = fmt::format(L"{}", place);
									data += AddPreviewData(iLine, idx, str);
								}
								break;
								case IO_RUNS_IN_CLASS:
								{
									std::wstring str;
									short inClass = pRun->GetInClass();
									if (0 > inClass)
										str = L"?";
									else
										str = fmt::format(L"{}", inClass);
									data += AddPreviewData(iLine, idx, str);
								}
								break;
								case IO_RUNS_DOGSQD:
								{
									std::wstring str;
									short qd = pRun->GetDogsQd();
									if (0 > qd)
										str = L"?";
									else
										str = fmt::format(L"{}", qd);
									data += AddPreviewData(iLine, idx, str);
								}
								break;
								case IO_RUNS_Q:
								{
									std::wstring q;
									if (pRun->GetQ().Qualified())
									{
										std::vector<ARBConfigMultiQPtr> multiQs;
										if (0 < pRun->GetMultiQs(multiQs))
										{
											for (std::vector<ARBConfigMultiQPtr>::iterator iMultiQ = multiQs.begin();
												 iMultiQ != multiQs.end();
												 ++iMultiQ)
											{
												if (!q.empty())
													q += L"/";
												q += (*iMultiQ)->GetShortName();
											}
										}
										if (Q::SuperQ == pRun->GetQ())
										{
											if (!q.empty())
												q += L"/";
											q += _("IDS_SQ");
										}
									}
									if (q.empty())
										q = pRun->GetQ().str();
									data += AddPreviewData(iLine, idx, q);
								}
								break;
								case IO_RUNS_SCORE:
									if (pRun->GetQ().Qualified() || Q::NQ == pRun->GetQ())
									{
										data += AddPreviewData(
											iLine,
											idx,
											ARBDouble::ToString(pRun->GetScore(pScoring)));
									}
									else
									{
										data += AddPreviewData(iLine, idx, std::wstring());
									}
									break;
								case IO_RUNS_TITLE_POINTS:
								{
									double pts = 0.0;
									if (pRun->GetQ().Qualified())
										pts = pRun->GetTitlePoints(pScoring);
									data += AddPreviewData(iLine, idx, fmt::format(L"{}", pts));
								}
								break;
								case IO_RUNS_COMMENTS:
									data += AddPreviewData(iLine, idx, pRun->GetNote());
									break;
								case IO_RUNS_FAULTS:
								{
									std::wstring fld;
									long i = 0;
									for (ARBDogFaultList::const_iterator iter = pRun->GetFaults().begin();
										 iter != pRun->GetFaults().end();
										 ++iter)
									{
										if (0 < i)
											fld += L"/";
										fld += *iter;
									}
									data += AddPreviewData(iLine, idx, fld);
								}
								break;
								}
							}
							if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
								&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
								m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
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
		for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin();
			 iterCal != m_pDoc->Book().GetCalendar().end();
			 ++iterCal)
		{
			std::wstring data;
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
						data += AddPreviewData(iLine, idx, L"?");
					break;
				case IO_CAL_ENTERED:
					switch (pCal->GetEntered())
					{
					case ARBCalendarEntry::Not:
						break;
					case ARBCalendarEntry::Entered:
						data += AddPreviewData(iLine, idx, Localization()->CalendarEntered());
						break;
					case ARBCalendarEntry::Pending:
						data += AddPreviewData(iLine, idx, Localization()->CalendarPending());
						break;
					case ARBCalendarEntry::Planning:
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
				m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
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
			for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin();
				 iterCal != m_pDoc->Book().GetCalendar().end();
				 ++iterCal)
			{
				allEntries.push_back(*iterCal);
			}
			entries = &allEntries;
		}
		for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end();
			 ++iterCal)
		{
			std::wstring data;
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
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_END_DATE:
					data += AddPreviewData(iLine, idx, pCal->GetEndDate().GetString(format));
					break;
				case IO_CAL_APPT_END_TIME:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_ALLDAY:
					data += AddPreviewData(iLine, idx, L"1");
					break;
				case IO_CAL_APPT_REMINDER:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_REMINDER_DATE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_REMINDER_TIME:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_ORGANIZER:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_REQ_ATTENDEES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_OPT_ATTENDEES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_RESOURCES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_BILLING:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_CATEGORIES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_DESCRIPTION:
				{
					std::wstring tmp;
					if (pCal->IsTentative())
					{
						tmp += Localization()->CalendarTentative();
						tmp += L" ";
					}
					switch (pCal->GetEntered())
					{
					case ARBCalendarEntry::Not:
						tmp += Localization()->CalendarStatusN();
						tmp += L" ";
						break;
					case ARBCalendarEntry::Entered:
						tmp += Localization()->CalendarStatusE();
						tmp += L" ";
						break;
					case ARBCalendarEntry::Pending:
						tmp += Localization()->CalendarStatusO();
						tmp += L" ";
						break;
					case ARBCalendarEntry::Planning:
						tmp += Localization()->CalendarStatusP();
						tmp += L" ";
						break;
					}
					date = pCal->GetOpeningDate();
					if (date.IsValid())
					{
						tmp += Localization()->CalendarOpens();
						tmp += L" ";
						tmp += date.GetString(format);
						tmp += L" ";
					}
					date = pCal->GetClosingDate();
					if (date.IsValid())
					{
						tmp += Localization()->CalendarCloses();
						tmp += L" ";
						tmp += date.GetString(format);
						tmp += L" ";
					}
					tmp += pCal->GetNote();
					data += AddPreviewData(iLine, idx, tmp);
				}
				break;
				case IO_CAL_APPT_LOCATION:
					data += AddPreviewData(iLine, idx, pCal->GetLocation());
					break;
				case IO_CAL_APPT_MILEAGE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_PRIORITY:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_PRIVATE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_SENSITIVITY:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_APPT_SHOW_TIME_AS:
					data += AddPreviewData(iLine, idx, L"");
					break;
				}
			}
			if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
				m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
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
			for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin();
				 iterCal != m_pDoc->Book().GetCalendar().end();
				 ++iterCal)
			{
				allEntries.push_back(*iterCal);
			}
			entries = &allEntries;
		}
		for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end();
			 ++iterCal)
		{
			std::wstring data;
			ARBCalendarPtr pCal = *iterCal;
			if (ARBCalendarEntry::Planning != pCal->GetEntered())
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
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_REMINDER_DATE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_REMINDER_TIME:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_COMPLETED_DATE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_COMPLETE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_TOTAL_WORK:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_ACTUAL_WORK:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_BILLING:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_CATEGORIES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_COMPANIES:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_CONTACTS:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_MILEAGE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_NOTES:
				{
					fmt::wmemory_buffer tmp;
					if (pCal->IsTentative())
					{
						fmt::format_to(tmp, L"{} ", Localization()->CalendarTentative());
					}
					date = pCal->GetOpeningDate();
					if (date.IsValid())
					{
						fmt::format_to(tmp, L"{} {} ", Localization()->CalendarOpens(), date.GetString(format));
					}
					date = pCal->GetClosingDate();
					if (date.IsValid())
					{
						fmt::format_to(tmp, L"{} {} ", Localization()->CalendarCloses(), date.GetString(format));
					}
					fmt::format_to(
						tmp,
						_("IDS_TRIAL_DATES").wx_str(),
						pCal->GetStartDate().GetString(format),
						pCal->GetEndDate().GetString(format));
					fmt::format_to(tmp, L" {}", pCal->GetNote());
					data += AddPreviewData(iLine, idx, fmt::to_string(tmp));
				}
				break;
				case IO_CAL_TASK_PRIORITY:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_PRIVATE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_ROLE:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_SCH_PRIORITY:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_SENSITIVITY:
					data += AddPreviewData(iLine, idx, L"");
					break;
				case IO_CAL_TASK_STATUS:
					data += AddPreviewData(iLine, idx, L"");
					break;
				}
			}
			if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
				&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
				m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
			++iLine;
		}
	}
	break;

	case WIZ_EXPORT_LOG:
	{
		for (ARBTrainingList::const_iterator iterLog = m_pDoc->Book().GetTraining().begin();
			 iterLog != m_pDoc->Book().GetTraining().end();
			 ++iterLog)
		{
			std::wstring data;
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
				m_ctrlPreview->InsertItem(iLine, StringUtil::stringWX(data));
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
	m_Delim = ARBImportExportDelim::Tab;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimSpace(wxCommandEvent& evt)
{
	m_Delim = ARBImportExportDelim::Space;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimColon(wxCommandEvent& evt)
{
	m_Delim = ARBImportExportDelim::Colon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimSemicolon(wxCommandEvent& evt)
{
	m_Delim = ARBImportExportDelim::Semicolon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimComma(wxCommandEvent& evt)
{
	m_Delim = ARBImportExportDelim::Comma;
	UpdateButtons();
	UpdatePreview();
}


void CWizardExport::OnDelimOther(wxCommandEvent& evt)
{
	m_Delim = ARBImportExportDelim::Other;
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
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle() && WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
	{
		std::wstring tmp;
		CAgilityBookOptions::SetImportExportDelimiters(false, m_Delim, tmp);
		m_Delimiter = StringUtil::stringWX(tmp);
	}
	ARBDateFormat format = static_cast<ARBDateFormat>(reinterpret_cast<size_t>(m_ctrlDateFormat->GetClientData(index)));
	CAgilityBookOptions::SetImportExportDateFormat(false, format);

	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() || WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		ISpreadSheetExporterPtr pExporter;
		if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle() && m_pSheet->ExcelHelper())
			pExporter = m_pSheet->ExcelHelper()->GetExporter();
		else if (WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle() && m_pSheet->CalcHelper())
			pExporter = m_pSheet->CalcHelper()->GetExporter();
		if (pExporter)
		{
			pExporter->AllowAccess(false);
			long nColumnCount = m_ctrlPreview->GetColumnCount();
			IDlgProgress* pProgress = IDlgProgress::CreateProgress(1, this);
			pProgress->EnableCancel(false);
			pProgress->SetMessage(StringUtil::stringW(_("IDS_EXPORTING")));
			pProgress->SetRange(1, m_ctrlPreview->GetItemCount() * nColumnCount);
			pProgress->ShowProgress();

			bool bFailed = false;
			for (long i = 0; !bFailed && i < m_ctrlPreview->GetItemCount(); ++i)
			{
				for (long iCol = 0; !bFailed && iCol < nColumnCount; ++iCol)
				{
					std::wstring line = GetListColumnText(m_ctrlPreview, i, iCol);
					bFailed = !pExporter->InsertData(i, iCol, line);
					// Calc is started visibly, so steal focus back.
					if (0 == i && 0 == iCol)
						pProgress->SetForegroundWindow();
					pProgress->StepIt(1);
				}
			}
			pExporter->AutoFit(0, nColumnCount - 1);
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
		wxFileDialog file(
			this,
			wxString(), // caption
			wxString(), // def dir
			wxString(),
			_("IDS_FILEEXT_FILTER_TXTCSV"),
			wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
		if (wxID_OK == file.ShowModal())
		{
			wxBusyCursor wait;
			wxFFileOutputStream output(file.GetPath(), L"wb");
			if (output.IsOk())
			{
				for (long i = 0; i < m_ctrlPreview->GetItemCount(); ++i)
				{
					std::wstring line = GetListColumnText(m_ctrlPreview, i, 0);
					line += L"\r\n";
					std::string utf8(StringUtil::stringA(line));
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
