/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Import Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2011-01-22 DRC Defer sorting of imported items until the end.
 * @li 2011-01-08 DRC Fix importing dates directly from Excel (time was
 *                    included). Fix importing to group runs into one trial.
 *                    Fix trial sorting (honor user selection).
 * @li 2010-12-05 DRC Fixed crash on Mac when more columns in import data than
 *                    configured columns. Also, set DOB for a new dog.
 * @li 2010-10-30 DRC Implemented proper CSV exporting. Fix init radio check.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-14 DRC Fixed group box creation order.
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-08-08 DRC Added validation during import to make sure names are
 *                    correct.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Only sort runs one way, the UI handles everything else.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-05-10 DRC Process quoted fields in input (like Excel).
 * @li 2004-04-15 DRC If imported data contained a header that we parsed as
 *                    data, the program faulted trying to release a null ptr.
 * @li 2004-03-22 DRC Fixed line/col number reporting (off by one)
 * @li 2004-01-04 DRC Implemented import, except for trials/runs.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "WizardImport.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "BreakLine.h"
#include "DlgAssignColumns.h"
#include "DlgMessage.h"
#include "DlgProgress.h"
#include "Globals.h"
#include "ListCtrl.h"
#include "Wizard.h"
#include <wx/spinctrl.h>
#include <wx/textfile.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


IMPLEMENT_CLASS(CWizardImport, wxWizardPageSimple)


CWizardImport::CWizardImport(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc,
		wxWizardPage* prev)
	: wxWizardPageSimple(pSheet, prev)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
	, m_FileName()
	, m_FileData()
	, m_ExcelData()
	, m_Row(CAgilityBookOptions::GetImportStartRow())
	, m_ctrlRow(NULL)
	, m_Delim(-1)
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
	, m_ctrlPreviewFile(NULL)
	, m_ctrlPreview(NULL)
{
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGING, wxWizardEventHandler, CWizardImport::OnWizardChanging);
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler, CWizardImport::OnWizardChanged);

	CAgilityBookOptions::GetImportExportDelimiters(true, m_Delim, m_Delimiter);

	// Controls (these are done first to control tab order)

	wxStaticText* textRow = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_ROW"),
		wxDefaultPosition, wxDefaultSize, 0);
	textRow->Wrap(-1);

	m_ctrlRow = new wxSpinCtrl(this, wxID_ANY, wxString(),
		wxDefaultPosition, wxSize(50, -1), wxSP_ARROW_KEYS, 0, 100, m_Row);
	BIND_OR_CONNECT_CTRL(m_ctrlRow, wxEVT_COMMAND_SPINCTRL_UPDATED, wxSpinEventHandler, CWizardImport::OnDeltaposImportRowSpin);
	m_ctrlRow->SetHelpText(_("HIDC_WIZARD_IMPORT_ROW"));
	m_ctrlRow->SetToolTip(_("HIDC_WIZARD_IMPORT_ROW"));

	wxStaticText* textFormat = new wxStaticText(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DATE"),
		wxDefaultPosition, wxDefaultSize, 0);
	textFormat->Wrap(-1);

	m_ctrlDateFormat = new wxComboBox(this, wxID_ANY, wxString(),
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxCB_DROPDOWN|wxCB_READONLY); 
	static struct
	{
		wchar_t const* uFormat;
		ARBDate::DateFormat format;
		ARBDate::DateFormat extendedFormat;
	} const sc_Dates[] =
	{
		{arbT("IDS_DATEFORMAT_DASH_MMDDYYYY"),
			ARBDate::eDashMMDDYYYY, ARBDate::eDashMDY},
		{arbT("IDS_DATEFORMAT_SLASH_MMDDYYYY"),
			ARBDate::eSlashMMDDYYYY, ARBDate::eSlashMDY},
		{arbT("IDS_DATEFORMAT_DASH_YYYYMMDD"),
			ARBDate::eDashYYYYMMDD, ARBDate::eDashYMD},
		{arbT("IDS_DATEFORMAT_SLASH_YYYYMMDD"),
			ARBDate::eSlashYYYYMMDD, ARBDate::eSlashYMD},
		{arbT("IDS_DATEFORMAT_DASH_DDMMYYYY"),
			ARBDate::eDashDDMMYYYY, ARBDate::eDashDMY},
		{arbT("IDS_DATEFORMAT_SLASH_DDMMYYYY"),
			ARBDate::eSlashDDMMYYYY, ARBDate::eSlashDMY},
	};
	static size_t const sc_nDates = sizeof(sc_Dates) / sizeof(sc_Dates[0]);
	ARBDate::DateFormat format;
	CAgilityBookOptions::GetImportExportDateFormat(true, format);
	for (size_t i = 0; i < sc_nDates; ++i)
	{
		long index = m_ctrlDateFormat->Append(wxGetTranslation(sc_Dates[i].uFormat));
		m_ctrlDateFormat->SetClientData(index, (void*)sc_Dates[i].format);
		if (sc_Dates[i].format == format || sc_Dates[i].extendedFormat == format)
			m_ctrlDateFormat->SetSelection(index);
	}
	if (0 > m_ctrlDateFormat->GetSelection())
		m_ctrlDateFormat->SetSelection(0);
	m_ctrlDateFormat->SetHelpText(_("HIDC_WIZARD_IMPORT_DATE"));
	m_ctrlDateFormat->SetToolTip(_("HIDC_WIZARD_IMPORT_DATE"));

	m_boxDelimiters = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_IMPORT_DELIM_GROUP"));

	m_ctrlTab = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_TAB"),
		wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
	BIND_OR_CONNECT_CTRL(m_ctrlTab, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimTab);
	m_ctrlTab->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_TAB"));
	m_ctrlTab->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_TAB"));

	m_ctrlColon = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_COLON"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlColon, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimColon);
	m_ctrlColon->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_COLON"));
	m_ctrlColon->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_COLON"));

	m_ctrlComma = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_COMMA"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlComma, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimComma);
	m_ctrlComma->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_COMMA"));
	m_ctrlComma->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_COMMA"));

	m_ctrlSpace = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_SPACE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlSpace, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimSpace);
	m_ctrlSpace->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_SPACE"));
	m_ctrlSpace->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_SPACE"));

	m_ctrlSemicolon = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_SEMI"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlSemicolon, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimSemicolon);
	m_ctrlSemicolon->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_SEMI"));
	m_ctrlSemicolon->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_SEMI"));

	m_ctrlOther = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_DELIM_OTHER"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlOther, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardImport::OnDelimOther);
	m_ctrlOther->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM_OTHER"));
	m_ctrlOther->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM_OTHER"));

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
		wxDefaultPosition, wxSize(30, -1), 0);
	m_ctrlOtherChar->SetMaxLength(1); 
	BIND_OR_CONNECT_CTRL(m_ctrlOtherChar, wxEVT_COMMAND_TEXT_UPDATED, wxCommandEventHandler, CWizardImport::OnImportDelim);
	m_ctrlOtherChar->SetHelpText(_("HIDC_WIZARD_IMPORT_DELIM"));
	m_ctrlOtherChar->SetToolTip(_("HIDC_WIZARD_IMPORT_DELIM"));

	m_ctrlAssign = new wxButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_ASSIGN"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(m_ctrlAssign, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CWizardImport::OnImportAssign);
	m_ctrlAssign->SetHelpText(_("HIDC_WIZARD_IMPORT_ASSIGN"));
	m_ctrlAssign->SetToolTip(_("HIDC_WIZARD_IMPORT_ASSIGN"));

	wxButton* btnFile = new wxButton(this, wxID_ANY,
		_("IDC_WIZARD_IMPORT_FILE"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(btnFile, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler, CWizardImport::OnImportFile);
	btnFile->SetHelpText(_("HIDC_WIZARD_IMPORT_FILE"));
	btnFile->SetToolTip(_("HIDC_WIZARD_IMPORT_FILE"));

	m_ctrlPreviewFile = new wxStaticText(this, wxID_ANY, wxString(),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlPreviewFile->Wrap(-1);

	m_ctrlPreview = new CListCtrl(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize, wxLC_REPORT);
	m_ctrlPreview->SetHelpText(_("HIDC_WIZARD_IMPORT_PREVIEW"));
	m_ctrlPreview->SetToolTip(_("HIDC_WIZARD_IMPORT_PREVIEW"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerStart = new wxBoxSizer(wxHORIZONTAL);
	sizerStart->Add(textRow, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStart->Add(m_ctrlRow, 0, wxALL, 5);

	sizerStart->Add(0, 0, 1, wxEXPAND, 5);
	sizerStart->Add(m_ctrlAssign, 0, wxALL, 5);
	sizerStart->Add(textFormat, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerStart->Add(m_ctrlDateFormat, 0, wxALL, 5);

	bSizer->Add(sizerStart, 0, wxEXPAND, 5);

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

	bSizer->Add(sizerOptions, 0, wxEXPAND, 5);

	wxBoxSizer* sizeFile = new wxBoxSizer(wxHORIZONTAL);
	sizeFile->Add(btnFile, 0, wxALL, 5);
	sizeFile->Add(m_ctrlPreviewFile, 0, wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxTOP, 5);

	bSizer->Add(sizeFile, 0, wxEXPAND, 5);
	bSizer->Add(m_ctrlPreview, 1, wxALL|wxEXPAND, 5);

	SetSizer(bSizer);
	bSizer->Fit(this);
}


void CWizardImport::ResetData()
{
	m_FileName.erase();
	m_FileData.clear();
	m_ExcelData.clear();
}


CAgilityBookOptions::ColumnOrder CWizardImport::GetColumnInfo() const
{
	CAgilityBookOptions::ColumnOrder order = CAgilityBookOptions::eUnknown;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_IMPORT_RUNS: order = CAgilityBookOptions::eRunsImport; break;
	case WIZ_IMPORT_CALENDAR: order = CAgilityBookOptions::eCalImport; break;
	case WIZ_IMPORT_LOG: order = CAgilityBookOptions::eLogImport; break;
	}
	return order;
}


wchar_t CWizardImport::GetDelim() const
{
	switch (m_Delim)
	{
	default:
	case CAgilityBookOptions::eDelimTab:       return L'\t';
	case CAgilityBookOptions::eDelimSpace:     return L' ';
	case CAgilityBookOptions::eDelimColon:     return L':';
	case CAgilityBookOptions::eDelimSemicolon: return L';';
	case CAgilityBookOptions::eDelimComma:     return L',';
	case CAgilityBookOptions::eDelimOther:
		if (1 == m_Delimiter.length())
			return m_Delimiter[0];
		else
			return 0;
	}
}


void CWizardImport::UpdateButtons()
{
	// Some test to make sure things are ready
	bool bOk = false;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		bOk = true;
	else if (0 != GetDelim())
		bOk = true;
	bool bEnable = false;
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
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


bool CWizardImport::DoImportFile()
{
	wxString filter;
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle())
		filter = _("IDS_FILEEXT_FILTER_MSEXCEL");
	else if (WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		filter = _("IDS_FILEEXT_FILTER_OOCALC");
	else
		filter = _("IDS_FILEEXT_FILTER_TXTCSV");
	wxFileDialog dlg(this,
		wxString(), // caption
		wxString(), // def dir
		wxString(),
		filter,
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (wxID_OK == dlg.ShowModal())
	{
		m_FileName = dlg.GetPath();
		wxString str = wxString::Format(_("IDS_FILE_PREVIEW"), m_FileName.c_str());
		m_ctrlPreviewFile->SetLabel(str);
		wxBusyCursor wait;
		m_FileData.clear();
		m_ExcelData.clear();
		if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
		|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
		{
			ISpreadSheetImporterPtr pImporter;
			if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
			&& m_pSheet->ExcelHelper())
				pImporter = m_pSheet->ExcelHelper()->GetImporter();
			else if (WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle()
			&& m_pSheet->CalcHelper())
				pImporter = m_pSheet->CalcHelper()->GetImporter();
			if (pImporter)
			{
				IDlgProgress* pProgress = IDlgProgress::CreateProgress(1, this);
				pProgress->EnableCancel(false);
				if (pImporter->OpenFile(m_FileName))
				{
					pImporter->GetData(m_ExcelData, pProgress);
				}
				pProgress->Dismiss();
			}
			else
				wxMessageBox(_("IDS_IMPORT_FAILED"), wxMessageBoxCaptionStr, wxOK | wxICON_STOP);
		}
		else
		{
			wxTextFile file;
			if (file.Open(m_FileName))
			{
				for (str = file.GetFirstLine(); !file.Eof(); str = file.GetNextLine())
				{
					m_FileData.push_back(StringUtil::TrimRight(StringUtil::stringW(str)));
				}
				file.Close();
			}
		}
		UpdateButtons();
		UpdatePreview();
		return true;
	}
	return false;
}


void CWizardImport::UpdatePreview()
{
	wxBusyCursor wait;
	m_ctrlPreview->Freeze();

	// Clear existing preview data.
	m_ctrlPreview->DeleteAllItems();
	long nColumnCount = m_ctrlPreview->GetColumnCount();
	long iCol;
	for (iCol = 0; iCol < nColumnCount; ++iCol)
		m_ctrlPreview->DeleteColumn(0);

	wchar_t delim = GetDelim();
	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
	}
	else if (0 == delim)
	{
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
	std::vector<std::wstring> cols;
	switch (m_pSheet->GetImportExportItem())
	{
	default: break;
	case WIZ_IMPORT_RUNS:
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
						cols[iCol] += wxT("/") + str;
				}
			}
		}
		break;
	case WIZ_IMPORT_CALENDAR:
		for (index = 0; index < columns[IO_TYPE_CALENDAR].size(); ++index)
		{
			cols.push_back(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_CALENDAR][index]));
		}
		break;
	case WIZ_IMPORT_LOG:
		for (index = 0; index < columns[IO_TYPE_TRAINING].size(); ++index)
		{
			cols.push_back(CDlgAssignColumns::GetNameFromColumnID(columns[IO_TYPE_TRAINING][index]));
		}
		break;
	}
	for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
	{
		m_ctrlPreview->InsertColumn(iCol, cols[iCol]);
	}

	if (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
	|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle())
	{
		long iLine = 0;
		for (std::vector< std::vector<std::wstring> >::const_iterator iter = m_ExcelData.begin();
			iter != m_ExcelData.end();
			++iter, ++iLine)
		{
			if (iLine >= m_Row - 1)
			{
				std::vector<std::wstring> const& rowData = *iter;
				iCol = 0;
				long iCurLine = -1;
				for (std::vector<std::wstring>::const_iterator iter2 = rowData.begin();
					iter2 != rowData.end() && iCol < static_cast<long>(cols.size());
					++iter2, ++iCol)
				{
					std::wstring const& str = *iter2;
					if (0 == iCol)
						iCurLine = m_ctrlPreview->InsertItem(iLine, str);
					else
						SetListColumnText(m_ctrlPreview, iCurLine, iCol, str);
				}
			}
		}
	}
	else
	{
		long iLine = 0;
		for (long iFileLine = m_Row - 1;
			iFileLine < static_cast<long>(m_FileData.size());
			++iLine, ++iFileLine)
		{
			std::vector<std::wstring> fields;
			ReadStatus status = ReadCSV(delim, m_FileData[iFileLine], fields);
			while (DataNeedMore == status && iFileLine + 1 < static_cast<long>(m_FileData.size()))
			{
				++iFileLine;
				status = ReadCSV(delim, m_FileData[iFileLine], fields, true);
			}
			if (DataOk == status && 0 < fields.size())
			{
				m_ctrlPreview->InsertItem(iLine, fields[0]);
				for (iCol = 1;
					iCol < static_cast<long>(fields.size()) && iCol < static_cast<long>(cols.size());
					++iCol)
				{
					SetListColumnText(m_ctrlPreview, iLine, iCol, fields[iCol]);
				}
			}
			// Failed - means a blank row
			else
				m_ctrlPreview->InsertItem(iLine, std::wstring());
		}
	}
	for (iCol = 0; iCol < static_cast<long>(cols.size()); ++iCol)
		m_ctrlPreview->SetColumnWidth(iCol, wxLIST_AUTOSIZE_USEHEADER);
	m_ctrlPreview->Thaw();
	m_ctrlPreview->Refresh();
}


static ARBDogRunPtr CreateRun(
		ARBDogRunPtr pRun,
		ARBConfigScoringPtr pScoring)
{
	if (!pRun)
	{
		pRun = ARBDogRunPtr(ARBDogRun::New());
		pRun->GetScoring().SetType(ARBDogRunScoring::TranslateConfigScoring(pScoring->GetScoringStyle()),
			pScoring->DropFractions());
	}
	return pRun;
}


static std::wstring GetPrimaryVenue(std::wstring const& venues)
{
	std::wstring venue;
	std::wstring::size_type pos = venues.find('/');
	if (std::wstring::npos != pos)
		venue = venues.substr(0, pos);
	else
		venue = venues;
	return venue;
}


static ARBCalendarPtr CreateCal(ARBCalendarPtr pCal)
{
	if (!pCal)
	{
		pCal = ARBCalendarPtr(ARBCalendar::New());
	}
	return pCal;
}


static ARBTrainingPtr CreateLog(ARBTrainingPtr pLog)
{
	if (!pLog)
	{
		pLog = ARBTrainingPtr(ARBTraining::New());
		pLog->SetDate(ARBDate::Today());
	}
	return pLog;
}


void CWizardImport::OnDeltaposImportRowSpin(wxSpinEvent& evt)
{
	if (m_Row != evt.GetPosition() && 0 < evt.GetPosition())
	{
		m_Row = evt.GetPosition();
		UpdatePreview();
	}
}


void CWizardImport::OnDelimTab(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimTab;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnDelimSpace(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimSpace;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnDelimColon(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimColon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnDelimSemicolon(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimSemicolon;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnDelimComma(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimComma;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnDelimOther(wxCommandEvent& evt)
{
	m_Delim = CAgilityBookOptions::eDelimOther;
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnImportDelim(wxCommandEvent& evt)
{
	if (m_ctrlOtherChar)
		m_Delimiter = m_ctrlOtherChar->GetValue();
	UpdateButtons();
	UpdatePreview();
}


void CWizardImport::OnImportAssign(wxCommandEvent& evt)
{
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
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


void CWizardImport::OnImportFile(wxCommandEvent& evt)
{
	DoImportFile();
}


void CWizardImport::OnWizardChanging(wxWizardEvent& evt)
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


void CWizardImport::OnWizardChanged(wxWizardEvent& evt)
{
	if (evt.GetPage() == static_cast<wxWizardPage*>(this))
	{
		m_pSheet->SetLabel(_("IDD_WIZARD_IMPORT"));

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
		if (m_FileName.empty())
		{
			if (!DoImportFile())
				UpdatePreview();
		}
	}
	evt.Skip();
}


bool CWizardImport::DoWizardFinish()
{
	long index = m_ctrlDateFormat->GetSelection();
	if (wxNOT_FOUND == index)
	{
		wxMessageBox(_("IDS_SPECIFY_DATEFORMAT"), wxMessageBoxCaptionStr, wxOK | wxICON_ERROR);
		m_ctrlDateFormat->SetFocus();
		return false;
	}
	CAgilityBookOptions::SetImportStartRow(m_Row);
	if (WIZARD_RADIO_EXCEL != m_pSheet->GetImportExportStyle()
	&& WIZARD_RADIO_CALC != m_pSheet->GetImportExportStyle())
	{
		CAgilityBookOptions::SetImportExportDelimiters(true, m_Delim, m_Delimiter);
	}
	ARBDate::DateFormat format = static_cast<ARBDate::DateFormat>((int)m_ctrlDateFormat->GetClientData(index));
	CAgilityBookOptions::SetImportExportDateFormat(true, format);

	wxBusyCursor wait;
	CAgilityBookOptions::ColumnOrder order = GetColumnInfo();
	size_t iCol;
	std::vector<long> columns[IO_TYPE_MAX];
	for (iCol = 0; iCol < IO_TYPE_MAX; ++iCol)
	{
		CDlgAssignColumns::GetColumnOrder(order, iCol, columns[iCol]);
	}
	std::wstring loadstr;
	std::wostringstream errLog;
	long nAdded = 0;
	long nUpdated = 0;
	long nDuplicate = 0;
	long nSkipped = 0;
	long nColumns = m_ctrlPreview->GetColumnCount();
	std::set<ARBDogPtr> sortTrials;
	bool bSortCal = false;
	bool bSortLog = false;
	for (long nItem = 0; nItem < m_ctrlPreview->GetItemCount(); ++nItem)
	{
		std::vector<std::wstring> entry;
		entry.reserve(nColumns);
		for (long i = 0; i < nColumns; ++i)
		{
			entry.push_back(GetListColumnText(m_ctrlPreview, nItem, i));
		}
		switch (m_pSheet->GetImportExportItem())
		{
		default: break;

		case WIZ_IMPORT_RUNS:
			{
				long const colIdx[4] =
				{
					IO_TYPE_RUNS_FAULTS_TIME,
					IO_TYPE_RUNS_TIME_FAULTS,
					IO_TYPE_RUNS_OPEN_CLOSE,
					IO_TYPE_RUNS_POINTS
				};
				long idxDate[4];
				long idxVenue[4];
				long idxClub[4];
				long idxDiv[4];
				long idxLevel[4];
				long idxEvent[4];
				long i;
				for (i = 0; i < 4; ++i)
				{
					idxDate[i] = idxVenue[i] = idxClub[i] = idxDiv[i] = idxLevel[i] = idxEvent[i] = -1;
				}
				for (i = 0; i < 4; ++i)
				{
					for (iCol = 0; iCol < columns[colIdx[i]].size(); ++iCol)
					{
						if (IO_RUNS_DATE == columns[colIdx[i]][iCol])
							idxDate[i] = static_cast<long>(iCol);
						else if (IO_RUNS_VENUE == columns[colIdx[i]][iCol])
							idxVenue[i] = static_cast<long>(iCol);
						else if (IO_RUNS_CLUB == columns[colIdx[i]][iCol])
							idxClub[i] = static_cast<long>(iCol);
						else if (IO_RUNS_DIVISION == columns[colIdx[i]][iCol])
							idxDiv[i] = static_cast<long>(iCol);
						else if (IO_RUNS_LEVEL == columns[colIdx[i]][iCol])
							idxLevel[i] = static_cast<long>(iCol);
						else if (IO_RUNS_EVENT == columns[colIdx[i]][iCol])
							idxEvent[i] = static_cast<long>(iCol);
					}
				}
				ARBConfigScoringPtr pScoring;
				for (i = 0; !pScoring && i < 4; ++i)
				{
					if (0 <= idxVenue[i] && 0 <= idxEvent[i] && 0 <= idxDiv[i] && 0 <= idxLevel[i] && 0 <= idxDate[i])
					{
						m_pDoc->Book().GetConfig().GetVenues().FindEvent(
							GetPrimaryVenue(entry[idxVenue[i]]),
							entry[idxEvent[i]],
							entry[idxDiv[i]],
							entry[idxLevel[i]],
							ARBDate::FromString(entry[idxDate[i]], format),
							NULL,
							&pScoring);
					}
				}
				// It's conceivable that we could have more than one match.
				// But just don't worry about that... If data from various
				// rows/cols has that much overlap, it's just not worth it.
				if (!pScoring)
				{
					loadstr = wxString::Format(_("IDS_IMPORT_SKIP_NOCONFIG"), static_cast<int>(nItem + 1));
					errLog << loadstr << wxT("\n");
					++nSkipped;
					continue;
				}
				i = -1;
				switch (pScoring->GetScoringStyle())
				{
				case ARBConfigScoring::eFaultsThenTime:
				case ARBConfigScoring::eFaults100ThenTime:
				case ARBConfigScoring::eFaults200ThenTime:
					i = IO_TYPE_RUNS_FAULTS_TIME;
					break;
				case ARBConfigScoring::eOCScoreThenTime:
					i = IO_TYPE_RUNS_OPEN_CLOSE;
					break;
				case ARBConfigScoring::eScoreThenTime:
					i = IO_TYPE_RUNS_POINTS;
					break;
				case ARBConfigScoring::eTimePlusFaults:
					i = IO_TYPE_RUNS_TIME_FAULTS;
					break;
				}
				assert(0 <= i);

				std::wstring nameReg, nameCall;
				std::wstring trialVenue, trialClub, trialLocation, trialNotes;
				ARBDogRunPtr pRun;
				for (iCol = 0; iCol < entry.size() && iCol < columns[i].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[i][iCol])
					{
					case IO_RUNS_REG_NAME:
						nameReg = entry[iCol];
						break;
					case IO_RUNS_CALL_NAME:
						nameCall = entry[iCol];
						break;
					case IO_RUNS_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (!date.IsValid()
							&& (WIZARD_RADIO_EXCEL == m_pSheet->GetImportExportStyle()
							|| WIZARD_RADIO_CALC == m_pSheet->GetImportExportStyle()))
							{
								// Excel mangles the date. It adds time too.
								// Even if you enter "1/8/2011" in excel, it
								// "knows" it's a date so when you access it via
								// COM, you get "1/8/2011 12:00AM" even though
								// you never see that in excel!
								// [Not sure if OOcalc mangles too, leave this]
								wxDateTime d;
#if wxCHECK_VERSION(2, 9, 3)
								wxString::const_iterator end;
								if (d.ParseDateTime(entry[iCol], &end))
#else
								if (d.ParseDateTime(entry[iCol].c_str()))
#endif
									date.SetDate(d.GetYear(), d.GetMonth() + 1, d.GetDay());
							}
							if (date.IsValid())
							{
								pRun = CreateRun(pRun, pScoring);
								pRun->SetDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_RUN"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pRun)
									pRun.reset();
								iCol = columns[i].size();
							}
						}
						break;
					case IO_RUNS_VENUE:
						trialVenue = entry[iCol];
						break;
					case IO_RUNS_CLUB:
						trialClub = entry[iCol];
						break;
					case IO_RUNS_LOCATION:
						trialLocation = entry[iCol];
						break;
					case IO_RUNS_TRIAL_NOTES:
						trialNotes = entry[iCol];
						break;
					case IO_RUNS_DIVISION:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetDivision(entry[iCol]);
						break;
					case IO_RUNS_LEVEL:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetLevel(entry[iCol]);
						break;
					case IO_RUNS_EVENT:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetEvent(entry[iCol]);
						break;
					case IO_RUNS_HEIGHT:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetHeight(entry[iCol]);
						break;
					case IO_RUNS_JUDGE:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetJudge(entry[iCol]);
						break;
					case IO_RUNS_HANDLER:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetHandler(entry[iCol]);
						break;
					case IO_RUNS_CONDITIONS:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetConditions(entry[iCol]);
						break;
					case IO_RUNS_COURSE_FAULTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetCourseFaults(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_TIME:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetTime(StringUtil::ToDouble(entry[iCol]));
						break;
					case IO_RUNS_YARDS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetYards(StringUtil::ToDouble(entry[iCol]));
						break;
					case IO_RUNS_YPS:
						// Computed
						break;
					case IO_RUNS_OBSTACLES:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetObstacles(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_OPS:
						// Computed
						break;
					case IO_RUNS_SCT:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetSCT(StringUtil::ToDouble(entry[iCol]));
						break;
					case IO_RUNS_TOTAL_FAULTS:
						// Computed.
						break;
					case IO_RUNS_REQ_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_REQ_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedClosePts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_OPENING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_CLOSING:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetClosePts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_REQ_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetNeedOpenPts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_POINTS:
						pRun = CreateRun(pRun, pScoring);
						pRun->GetScoring().SetOpenPts(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_PLACE:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetPlace(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_IN_CLASS:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetInClass(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_DOGSQD:
						pRun = CreateRun(pRun, pScoring);
						pRun->SetDogsQd(static_cast<short>(StringUtil::ToLong(entry[iCol])));
						break;
					case IO_RUNS_Q:
						{
							pRun = CreateRun(pRun, pScoring);
							ARB_Q q;
							if (wxT("QQ") == entry[iCol])
								entry[iCol] = wxT("Q");
							CErrorCallback err;
							q.Load(entry[iCol], ARBAgilityRecordBook::GetCurrentDocVersion(), err);
							pRun->SetQ(q);
						}
						break;
					case IO_RUNS_SCORE:
						// This is computed.
						break;
					case IO_RUNS_TITLE_POINTS:
						// These are computed.
						break;
					case IO_RUNS_COMMENTS:
					case IO_RUNS_FAULTS:
						{
							pRun = CreateRun(pRun, pScoring);
							std::wstring str = pRun->GetNote();
							if (0 < str.length())
								str += L"\n";
							str += entry[iCol];
							pRun->SetNote(str);
						}
						break;
					}
				}
				pScoring.reset();

				// Verify data: venue is not necessarily valid!
				// When importing multiple types of data and only one type was
				// actually set, the other types will attempt to import because
				// we matched pScoring on that first entry. But when we
				// actually parse the data, the venue name is not what we
				// matched for pScoring! So check v/d/l/e again.
				if (pRun)
				{
					if (!m_pDoc->Book().GetConfig().GetVenues().FindVenue(trialVenue))
					{
						loadstr = wxString::Format(
							_("IDS_IMPORT_BAD_VENUE"),
							static_cast<int>(nItem + 1),
							trialVenue.c_str());
						errLog << loadstr << wxT("\n");
						pRun.reset();
					}
					else if (!m_pDoc->Book().GetConfig().GetVenues().FindEvent(
						trialVenue,
						pRun->GetEvent(),
						pRun->GetDivision(),
						pRun->GetLevel(),
						pRun->GetDate()))
					{
						loadstr = wxString::Format(_("IDS_IMPORT_SKIP_NOCONFIG"), static_cast<int>(nItem + 1));
						errLog << loadstr << wxT("\n");
						pRun.reset();
					}
				}

				// Now that we've created a run, we have to put it somewhere.
				if (pRun)
				{
					// Find the dog
					ARBDogPtr pDog = m_pDoc->GetCurrentDog();
					if (0 < nameReg.length() || 0 < nameCall.length())
					{
						pDog.reset();
						for (ARBDogList::iterator iterDog = m_pDoc->Book().GetDogs().begin(); iterDog != m_pDoc->Book().GetDogs().end(); ++iterDog)
						{
							ARBDogPtr pDogTmp = *iterDog;
							if ((0 < nameReg.length() && pDogTmp->GetRegisteredName() == nameReg
							&& 0 < nameCall.length() && pDogTmp->GetCallName() == nameCall)
							|| (0 < nameReg.length() && pDogTmp->GetRegisteredName() == nameReg
							&& 0 == nameCall.length())
							|| (0 == nameReg.length()
							&& 0 < nameCall.length() && pDogTmp->GetCallName() == nameCall))
							{
								pDog = pDogTmp;
								break;
							}
						}
						// Not found, create it.
						if (!pDog)
						{
							pDog = ARBDogPtr(ARBDog::New());
							pDog->SetDOB(ARBDate::Today());
							if (0 < nameReg.length())
								pDog->SetRegisteredName(nameReg);
							if (0 < nameCall.length())
								pDog->SetCallName(nameCall);
							if (0 == nameCall.length() && 0 < nameReg.length())
								pDog->SetCallName(nameReg);
							m_pDoc->Book().GetDogs().AddDog(pDog);
						}
					}
					if (!pDog)
					{
						if (m_pDoc->Book().GetDogs().begin() == m_pDoc->Book().GetDogs().end())
						{
							pDog = ARBDogPtr(ARBDog::New());
							pDog->SetCallName(wxT("?"));
							m_pDoc->Book().GetDogs().AddDog(pDog);
						}
						else
							pDog = *(m_pDoc->Book().GetDogs().begin());
					}
					assert(pDog);

					// Find the trial
					std::vector<std::wstring> venues;
					std::vector<std::wstring> clubs;
					if (0 < trialVenue.length())
						BreakLine('/', trialVenue, venues);
					if (0 < trialClub.length())
						BreakLine('/', trialClub, clubs);
					if (clubs.size() < venues.size())
					{
						// Clubs and venues now agree so we can use them together easily.
						if (0 == clubs.size())
							clubs.push_back(wxT("?"));
						while (clubs.size() < venues.size())
							clubs.push_back(clubs[clubs.size()-1]);
					}
					ARBDogTrialPtr pTrial;
					// Loop two times - first time, try to match.
					// Then stradle the date. We're trying to put multiple
					// days into a single trial.
					for (int iLoop = 0; !pTrial && iLoop < 2; ++iLoop)
					{
						for (ARBDogTrialList::iterator iterTrial = pDog->GetTrials().begin();
							iterTrial != pDog->GetTrials().end();
							++iterTrial)
						{
							ARBDogTrialPtr pTrialTmp = *iterTrial;
							ARBDate startDate = pTrialTmp->GetRuns().GetStartDate();
							ARBDate endDate = pTrialTmp->GetRuns().GetEndDate();
							if (1 == iLoop)
							{
								--startDate;
								++endDate;
							}
							if (pTrialTmp->GetClubs().size() == venues.size()
							&& pTrialTmp->GetLocation() == trialLocation
							&& pRun->GetDate().isBetween(startDate, endDate))
							{
								bool bOk = true;
								size_t idx = 0;
								for (ARBDogClubList::iterator iterClub = pTrialTmp->GetClubs().begin();
									iterClub != pTrialTmp->GetClubs().end();
									++iterClub)
								{
									ARBDogClubPtr pClub(ARBDogClub::New());
									pClub->SetName(clubs[idx]);
									pClub->SetVenue(venues[idx]);
									if (*pClub != *(*(iterClub)))
									{
										bOk = false;
										break;
									}
									++idx;
								}
								if (bOk)
								{
									pTrial = pTrialTmp;
									break;
								}
							}
						}
					}
					if (!pTrial)
					{
						// Couldn't find a trial, so make one.
						pTrial = ARBDogTrialPtr(ARBDogTrial::New());
						pDog->GetTrials().AddTrial(pTrial);
						sortTrials.insert(pDog);
						pDog->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
						for (size_t idx = 0; idx < venues.size(); ++idx)
						{
							pTrial->GetClubs().AddClub(clubs[idx], venues[idx]);
						}
						if (0 < trialLocation.length())
							pTrial->SetLocation(trialLocation);
						if (0 < trialNotes.length())
							pTrial->SetNote(trialNotes);
					}
					pTrial->GetRuns().AddRun(pRun);
					pTrial->SetMultiQs(m_pDoc->Book().GetConfig());
					pTrial->GetRuns().sort();
					++nAdded;
				}
				else
				{
					++nSkipped;
				}
			}
			break;

		case WIZ_IMPORT_CALENDAR:
			{
				ARBCalendarPtr pCal;
				for (iCol = 0; iCol < columns[IO_TYPE_CALENDAR].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_CALENDAR][iCol])
					{
					case IO_CAL_START_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetStartDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_CALSTART"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_END_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetEndDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_CALEND"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_TENTATIVE:
						pCal = CreateCal(pCal);
						pCal->SetIsTentative((wxT("?") == entry[iCol] || wxT("y") == entry[iCol] || wxT("Y") == entry[iCol]));
						break;
					case IO_CAL_ENTERED:
						if (wxT("N") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eNot);
						}
						else if (wxT("P") == entry[iCol] || _("IDS_CALENDAR_PLANNING") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::ePlanning);
						}
						else if (wxT("O") == entry[iCol] || _("IDS_CALENDAR_PENDING") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::ePending);
						}
						else if (wxT("E") == entry[iCol] || _("IDS_CALENDAR_ENTERED") == entry[iCol])
						{
							pCal = CreateCal(pCal);
							pCal->SetEntered(ARBCalendar::eEntered);
						}
						else
						{
							loadstr = wxString::Format(
								_("IDS_IMPORT_BAD_CAL_VALUE"),
								static_cast<int>(nItem + 1),
								static_cast<int>(iCol + 1),
								entry[iCol].c_str());
							errLog << loadstr << wxT("\n");
							if (pCal)
								pCal.reset();
							iCol = columns[IO_TYPE_CALENDAR].size();
						}
						break;
					case IO_CAL_LOCATION:
						pCal = CreateCal(pCal);
						pCal->SetLocation(entry[iCol]);
						break;
					case IO_CAL_CLUB:
						pCal = CreateCal(pCal);
						pCal->SetClub(entry[iCol]);
						break;
					case IO_CAL_VENUE:
						pCal = CreateCal(pCal);
						pCal->SetVenue(entry[iCol]);
						break;
					case IO_CAL_OPENS:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetOpeningDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_CALOPEN"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_CLOSES:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pCal = CreateCal(pCal);
								pCal->SetClosingDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_CALCLOSE"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pCal)
									pCal.reset();
								iCol = columns[IO_TYPE_CALENDAR].size();
							}
						}
						break;
					case IO_CAL_NOTES:
						pCal = CreateCal(pCal);
						pCal->SetNote(entry[iCol]);
						break;
					}
				}
				if (pCal)
				{
					ARBCalendarPtr cal;
					if (!m_pDoc->Book().GetCalendar().FindCalendar(pCal, false, &cal))
					{
						m_pDoc->Book().GetCalendar().AddCalendar(pCal);
						bSortCal = true;
						++nAdded;
					}
					else
					{
						if (pCal->Update(cal))
							++nUpdated;
						else
							++nDuplicate;
					}
				}
				else
					++nSkipped;
			}
			break;

		case WIZ_IMPORT_LOG:
			{
				ARBTrainingPtr pLog;
				for (iCol = 0; iCol < entry.size() && iCol < columns[IO_TYPE_TRAINING].size(); ++iCol)
				{
					if (0 == entry[iCol].length())
						continue;
					switch (columns[IO_TYPE_TRAINING][iCol])
					{
					case IO_LOG_DATE:
						{
							ARBDate date = ARBDate::FromString(entry[iCol], format);
							if (date.IsValid())
							{
								pLog = CreateLog(pLog);
								pLog->SetDate(date);
							}
							else
							{
								loadstr = wxString::Format(
									_("IDS_IMPORT_BAD_DATE_LOG"),
									static_cast<int>(nItem + 1),
									static_cast<int>(iCol + 1),
									entry[iCol].c_str());
								errLog << loadstr << wxT("\n");
								if (pLog)
									pLog.reset();
								iCol = columns[IO_TYPE_TRAINING].size();
							}
						}
						break;
					case IO_LOG_NAME:
						pLog = CreateLog(pLog);
						pLog->SetName(entry[iCol]);
						break;
					case IO_LOG_SUBNAME:
						pLog = CreateLog(pLog);
						pLog->SetSubName(entry[iCol]);
						break;
					case IO_LOG_NOTES:
						pLog = CreateLog(pLog);
						pLog->SetNote(entry[iCol]);
						break;
					}
				}
				if (pLog)
				{
					if (!m_pDoc->Book().GetTraining().FindTraining(pLog))
					{
						m_pDoc->Book().GetTraining().AddTraining(pLog);
						bSortLog = true;
						++nAdded;
					}
					else
						++nDuplicate;
				}
				else
					++nSkipped;
			}
			break;
		}
	}
	for (std::set<ARBDogPtr>::iterator iterDog = sortTrials.begin();
		iterDog != sortTrials.end();
		++iterDog)
	{
		(*iterDog)->GetTrials().sort(!CAgilityBookOptions::GetNewestDatesFirst());
	}
	if (bSortCal)
		m_pDoc->Book().GetCalendar().sort();
	if (bSortLog)
		m_pDoc->Book().GetTraining().sort();
	if (!errLog.str().empty())
		errLog << wxT("\n");
	loadstr = wxString::Format(_("IDS_IMPORT_STATS"),
		static_cast<int>(nAdded),
		static_cast<int>(nUpdated),
		static_cast<int>(nDuplicate),
		static_cast<int>(nSkipped));
	errLog << loadstr;
	CDlgMessage dlg(errLog.str(), this);
	dlg.ShowModal();
	if (0 < nAdded)
	{
		m_pDoc->Modify(true);
		CUpdateHint hint(UPDATE_ALL_VIEW);
		m_pDoc->UpdateAllViews(NULL, &hint);
	}
	return true;
}
