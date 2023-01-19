/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2015-01-01 Changed pixels to dialog units.
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2010-03-28 Added ability to import/export program settings.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-07-14 Fixed group box creation order.
 * 2009-06-14 Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *            _after_ the dialog is destroyed).
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-08-09 When importing ARB files, update regnum and titles
 *            that are different.
 * 2005-07-14 Remember selected export item.
 * 2005-06-25 Cleaned up reference counting when returning a pointer.
 * 2005-01-10 Allow titles to be optionally entered multiple times.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-08-21 Added ability to import dog/trial/run/judge info.
 * 2004-02-26 Moved configuration update logic to the doc.
 * 2004-02-18 Added 'DeleteTitle' configuration action.
 * 2004-01-26 Display errors on non-fatal load.
 * 2004-01-21 Implemented Action items in configuration update.
 * 2003-12-10 Created
 */

#include "stdafx.h"
#include "WizardStart.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ConfigHandler.h"
#include "VersionNumber.h"
#include "Wizard.h"

#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/DlgMessage.h"
#include "LibARBWin/Logger.h"
#include <wx/config.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

namespace
{
// Registry settings in "Last"
constexpr wchar_t LAST_STYLE[] = L"Last/WizStyle";
constexpr wchar_t LAST_STYLEITEM[] = L"Last/WizSubStyle"; // A number will be appended
														  // Note: LAST_STYLE is a fixed number, regardless of UI order.
// LAST_STYLEITEM actually uses the UI order (as of v1.10). If items are
// reordered, this _will_ cause a problem. As of v2.0, this will change to
// the registry key to use the actual style, not the UI order. There will be
// no backwards compatibility - this simply means that the wrong item may be
// initially selected the first time this is run (if the user ran the wizard
// in v1)
} // namespace

/////////////////////////////////////////////////////////////////////////////
// CWizardStart property page

wxIMPLEMENT_CLASS(CWizardStart, wxWizardPage)


CWizardStart::CWizardStart(CWizard* pSheet, CAgilityBookDoc* pDoc)
	: wxWizardPage(pSheet)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
	, m_ctrlList(nullptr)
	, m_ctrlDesc(nullptr)
	, m_Style(-1)
	, m_Next(nullptr)
{
	Bind(wxEVT_WIZARD_PAGE_CHANGING, &CWizardStart::OnWizardChanging, this);
	Bind(wxEVT_WIZARD_PAGE_CHANGED, &CWizardStart::OnWizardChanged, this);

	// Get the last selected choice
	m_Style = wxConfig::Get()->Read(LAST_STYLE, WIZARD_RADIO_EXCEL);
	if (-1 == m_Style)
		m_Style = WIZARD_RADIO_EXCEL;
	if (WIZARD_RADIO_EXCEL == m_Style && !m_pSheet->ExcelHelper())
		m_Style = WIZARD_RADIO_SPREADSHEET;
	if (WIZARD_RADIO_CALC == m_Style && !m_pSheet->CalcHelper())
		m_Style = WIZARD_RADIO_SPREADSHEET;

	// Controls (these are done first to control tab order)

	wxStaticBox* boxImportExport = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_RADIO_BOX"));
	wxRadioButton* radioExcel = nullptr;
	if (m_pSheet->ExcelHelper())
	{
		radioExcel = new wxRadioButton(
			this,
			wxID_ANY,
			_("IDC_WIZARD_START_EXCEL"),
			wxDefaultPosition,
			wxDefaultSize,
			wxRB_GROUP);
		radioExcel->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardStart::OnWizardStyleExcel, this);
		radioExcel->SetHelpText(_("HIDC_WIZARD_START_EXCEL"));
		radioExcel->SetToolTip(_("HIDC_WIZARD_START_EXCEL"));
	}

	wxRadioButton* radioCalc = nullptr;
	if (m_pSheet->CalcHelper())
	{
		radioCalc = new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_START_CALC"), wxDefaultPosition, wxDefaultSize, 0);
		radioCalc->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardStart::OnWizardStyleCalc, this);
		radioCalc->SetHelpText(_("HIDC_WIZARD_START_CALC"));
		radioCalc->SetToolTip(_("HIDC_WIZARD_START_CALC"));
	}

	wxRadioButton* radioSpread
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_START_SPREADSHEET"), wxDefaultPosition, wxDefaultSize, 0);
	radioSpread->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardStart::OnWizardStyleSpread, this);
	radioSpread->SetHelpText(_("HIDC_WIZARD_START_SPREADSHEET"));
	radioSpread->SetToolTip(_("HIDC_WIZARD_START_SPREADSHEET"));

	wxRadioButton* radioArb
		= new wxRadioButton(this, wxID_ANY, _("IDC_WIZARD_START_ARB"), wxDefaultPosition, wxDefaultSize, 0);
	radioArb->Bind(wxEVT_COMMAND_RADIOBUTTON_SELECTED, &CWizardStart::OnWizardStyleArb, this);
	radioArb->SetHelpText(_("HIDC_WIZARD_START_ARB"));
	radioArb->SetToolTip(_("HIDC_WIZARD_START_ARB"));

	switch (m_Style)
	{
	case WIZARD_RADIO_EXCEL:
		if (radioExcel)
			radioExcel->SetValue(true);
		else
			radioSpread->SetValue(true);
		break;
	case WIZARD_RADIO_CALC:
		if (radioCalc)
			radioCalc->SetValue(true);
		else
			radioSpread->SetValue(true);
		break;
	case WIZARD_RADIO_SPREADSHEET:
		radioSpread->SetValue(true);
		break;
	case WIZARD_RADIO_ARB:
		radioArb->SetValue(true);
		break;
	}

	m_ctrlList = new wxListBox(
		this,
		wxID_ANY,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 115), -1),
		0,
		nullptr,
		wxLB_HSCROLL | wxLB_SINGLE);
	m_ctrlList->Bind(wxEVT_COMMAND_LISTBOX_SELECTED, &CWizardStart::OnSelchangeExportList, this);
	m_ctrlList->SetHelpText(_("HIDC_WIZARD_START_LIST"));
	m_ctrlList->SetToolTip(_("HIDC_WIZARD_START_LIST"));

	wxStaticBox* boxDesc = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_START_DESCRIPTION"));

	m_ctrlDesc = new wxStaticText(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE);
	m_ctrlDesc->Wrap(wxDLG_UNIT_X(this, 115));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerImportExport = new wxStaticBoxSizer(boxImportExport, wxVERTICAL);
	if (radioExcel)
		sizerImportExport->Add(radioExcel, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 3));
	if (radioCalc)
		sizerImportExport->Add(radioCalc, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 3));
	sizerImportExport->Add(radioSpread, 0, wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 3));
	sizerImportExport->Add(radioArb, 0, wxALL, wxDLG_UNIT_X(this, 3));
	bSizer->Add(sizerImportExport, 0, wxDLG_UNIT_X(this, 3));

	wxStaticBoxSizer* sizerDesc = new wxStaticBoxSizer(boxDesc, wxVERTICAL);
	sizerDesc->Add(m_ctrlDesc, 1, wxEXPAND);

	wxBoxSizer* sizerAction = new wxBoxSizer(wxHORIZONTAL);
	sizerAction->Add(m_ctrlList, 1, wxEXPAND | wxRIGHT, wxDLG_UNIT_X(this, 5));
	sizerAction->Add(sizerDesc, 1, wxEXPAND);

	bSizer->Add(sizerAction, 1, wxEXPAND | wxTOP, wxDLG_UNIT_X(this, 5));

	SetSizer(bSizer);
	bSizer->Fit(this);
}

/////////////////////////////////////////////////////////////////////////////

enum class WizardPage
{
	None,
	Finish,
	Import,
	Export,
};

// Note, order of 'data' changed between v1 and v2. In v1, order was UI based.
// In v2, it's id based.
namespace
{
constexpr struct
{
	// For integrity checking. See Wizard.h.
	int index;
	struct
	{
		WizardPage nextPage;
		// Listing (NULL denotes no entry)
		wchar_t const* item;
		// Description shown when listing is selected.
		wchar_t const* desc;
	} data[4]; // Data must agree with radio buttons. [WIZARD_RADIO_*]
			   // excel, spread, arb, calc
} const sc_Items[] = {
	{
		WIZ_IMPORT_RUNS,
		{
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_EXCEL")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_SPREAD")},
			{WizardPage::Finish, arbT("IDS_WIZ_IMPORT_RUNS_PLUS"), arbT("IDS_WIZ_IMPORT_RUNS_ARB")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_CALC")},
		},
	},
	{
		WIZ_EXPORT_RUNS,
		{
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_EXCEL")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_SPREAD")},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_CALC")},
		},
	},
	{
		WIZ_IMPORT_CALENDAR,
		{
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_EXCEL")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_SPREAD")},
			{WizardPage::Finish, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_ARB")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_CALC")},
		},
	},
	{
		WIZ_EXPORT_CALENDAR,
		{
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_EXCEL")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_SPREAD")},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_ARB")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_CALC")},
		},
	},
	{
		WIZ_EXPORT_CALENDAR_VCAL,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_CAL_VCAL"), arbT("IDS_WIZ_EXPORT_CAL_VCAL_SPREAD")},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_CALENDAR_ICAL,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_CAL_ICAL"), arbT("IDS_WIZ_EXPORT_CAL_ICAL_SPREAD")},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_CALENDAR_APPT,
		{
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL_APPT"), arbT("IDS_WIZ_EXPORT_CAL_APPT_EXCEL")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL_APPT"), arbT("IDS_WIZ_EXPORT_CAL_APPT_SPREAD")},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_CALENDAR_TASK,
		{
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL_TASK"), arbT("IDS_WIZ_EXPORT_CAL_TASK_EXCEL")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_CAL_TASK"), arbT("IDS_WIZ_EXPORT_CAL_TASK_SPREAD")},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_IMPORT_LOG,
		{
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_EXCEL")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_SPREAD")},
			{WizardPage::Finish, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_ARB")},
			{WizardPage::Import, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_CALC")},
		},
	},
	{
		WIZ_EXPORT_LOG,
		{
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_EXCEL")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_SPREAD")},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_ARB")},
			{WizardPage::Export, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_CALC")},
		},
	},
	{
		WIZ_IMPORT_CONFIGURATION,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_IMPORT_CONFIG"), arbT("IDS_WIZ_IMPORT_CONFIG_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_CONFIGURATION,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_CONFIG"), arbT("IDS_WIZ_EXPORT_CONFIG_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_DTD,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_DTD"), arbT("IDS_WIZ_EXPORT_DTD_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_XML,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_XML"), arbT("IDS_WIZ_EXPORT_XML_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_IMPORT_SETTINGS,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_IMPORT_SETTINGS"), arbT("IDS_WIZ_IMPORT_SETTINGS_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
	{
		WIZ_EXPORT_SETTINGS,
		{
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::None, nullptr, nullptr},
			{WizardPage::Finish, arbT("IDS_WIZ_EXPORT_SETTINGS"), arbT("IDS_WIZ_EXPORT_SETTINGS_ARB")},
			{WizardPage::None, nullptr, nullptr},
		},
	},
};
constexpr int sc_nItems = sizeof(sc_Items) / sizeof(sc_Items[0]);
} // namespace


void CWizardStart::UpdateList(bool bInit)
{
	wxConfig::Get()->Write(LAST_STYLE, m_Style);
	m_pSheet->ResetData();

	m_ctrlList->Clear();
	m_ctrlDesc->SetLabel(wxEmptyString);
	for (int i = 0; i < sc_nItems; ++i)
	{
		assert(sc_Items[i].index == i);
		bool bAdd = true;
		if (m_pSheet->GetCalendarEntries() && sc_Items[i].index != WIZ_EXPORT_CALENDAR_VCAL
			&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_ICAL && sc_Items[i].index != WIZ_EXPORT_CALENDAR_APPT
			&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_TASK)
			bAdd = false;
		if (bAdd && !sc_Items[i].data[m_Style].item)
			bAdd = false;
		int index = wxNOT_FOUND;
		if (bAdd)
		{
			wxString str = wxGetTranslation(sc_Items[i].data[m_Style].item);
			index = m_ctrlList->Append(str);
		}
		if (wxNOT_FOUND != index)
			m_ctrlList->SetClientData(index, reinterpret_cast<void*>(static_cast<size_t>(i)));
	}
	std::wstring str = fmt::format(L"{}{}", LAST_STYLEITEM, m_Style);
	long idx = wxConfig::Get()->Read(str, -1L);
	m_ctrlList->SetSelection(idx);
	DoUpdateExportList(bInit);
}


void CWizardStart::DoUpdateExportList(bool bInit)
{
	int index = m_ctrlList->GetSelection();
	if (wxNOT_FOUND == index)
		return;

	size_t idx = reinterpret_cast<size_t>(m_ctrlList->GetClientData(index));

	bool bEnableNext = true;
	switch (sc_Items[idx].data[m_Style].nextPage)
	{
	case WizardPage::None:
		bEnableNext = false;
		// fallthru
	case WizardPage::Finish:
		m_Next = nullptr;
		break;
	case WizardPage::Import:
		m_Next = m_pSheet->GetImportPage();
		break;
	case WizardPage::Export:
		m_Next = m_pSheet->GetExportPage();
		break;
	}
	if (!bInit)
		m_pSheet->UpdateButtons(bEnableNext);

	wxString msg = wxGetTranslation(sc_Items[idx].data[m_Style].desc);
	m_ctrlDesc->SetLabel(msg);

	std::wstring str = fmt::format(L"{}{}", LAST_STYLEITEM, m_Style);
	wxConfig::Get()->Write(str, index);
}


void CWizardStart::OnWizardStyleExcel(wxCommandEvent& evt)
{
	m_Style = WIZARD_RADIO_EXCEL;
	UpdateList();
}


void CWizardStart::OnWizardStyleCalc(wxCommandEvent& evt)
{
	m_Style = WIZARD_RADIO_CALC;
	UpdateList();
}


void CWizardStart::OnWizardStyleSpread(wxCommandEvent& evt)
{
	m_Style = WIZARD_RADIO_SPREADSHEET;
	UpdateList();
}


void CWizardStart::OnWizardStyleArb(wxCommandEvent& evt)
{
	m_Style = WIZARD_RADIO_ARB;
	UpdateList();
}


void CWizardStart::OnSelchangeExportList(wxCommandEvent& evt)
{
	DoUpdateExportList();
}


void CWizardStart::OnWizardChanging(wxWizardEvent& evt)
{
	if (evt.GetDirection())
	{
		long index = m_ctrlList->GetSelection();
		if (-1 != m_Style && wxNOT_FOUND == index)
		{
			evt.Veto();
			return;
		}
		int data = static_cast<int>(reinterpret_cast<size_t>(m_ctrlList->GetClientData(index)));
		m_pSheet->SetImportExportItem(data, m_Style);
		if (WizardPage::Finish == sc_Items[data].data[m_Style].nextPage && !DoWizardFinish())
		{
			evt.Veto();
			return;
		}
	}
	evt.Skip();
}


void CWizardStart::OnWizardChanged(wxWizardEvent& evt)
{
	if (evt.GetPage() == static_cast<wxWizardPage*>(this))
	{
		CLogger::Log(L"WIZARD: CWizardStart::OnWizardChanged");
		m_pSheet->SetLabel(_("IDD_WIZARD_START"));
		UpdateList();
	}
	evt.Skip();
}


bool CWizardStart::DoWizardFinish()
{
	bool bOk = false;
	int index = m_ctrlList->GetSelection();
	if (-1 != m_Style && wxNOT_FOUND != index)
	{
		fmt::wmemory_buffer errMsg;
		size_t data = reinterpret_cast<size_t>(m_ctrlList->GetClientData(index));
		switch (data)
		{
		default:
			break;

		case WIZ_IMPORT_RUNS:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_IMPORT_RUNS");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (wxID_OK == file.ShowModal())
			{
				ElementNodePtr tree(ElementNode::New());
				bool bLoadOk = false;
				{
					wxBusyCursor wait;
					bLoadOk = tree->LoadXML(file.GetPath().wc_str(), errMsg);
				}
				if (!bLoadOk)
				{
					wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
					if (0 < errMsg.size())
					{
						msg << L"\n\n" << fmt::to_string(errMsg);
					}
					wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				}
				else
					bOk = m_pDoc->ImportARBRunData(tree, this);
			}
		}
		break;

		case WIZ_IMPORT_CALENDAR:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_IMPORT_CALENDAR");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (wxID_OK == file.ShowModal())
			{
				ElementNodePtr tree(ElementNode::New());
				bool bLoadOk = false;
				{
					wxBusyCursor wait;
					bLoadOk = tree->LoadXML(file.GetPath().wc_str(), errMsg);
				}
				if (!bLoadOk)
				{
					wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
					if (0 < errMsg.size())
					{
						msg << L"\n\n" << fmt::to_string(errMsg);
					}
					wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				}
				else
					bOk = m_pDoc->ImportARBCalData(tree, this);
			}
		}
		break;

		case WIZ_EXPORT_CALENDAR:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_CALENDAR");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
				std::wstring verstr = ver.GetVersionString();
				ElementNodePtr tree(ElementNode::New());
				if (m_pDoc->Book().Save(tree, verstr, true, false, false, false, false))
				{
					tree->SaveXML(StringUtil::stringW(file.GetPath()));
				}
				bOk = true;
			}
		}
		break;

		case WIZ_EXPORT_CALENDAR_VCAL:
		case WIZ_EXPORT_CALENDAR_ICAL:
		{
			wxString fname, filter;
			if (WIZ_EXPORT_CALENDAR_VCAL == data)
			{
				CLogger::Log(L"WIZARD: WIZ_EXPORT_CALENDAR_VCAL");
				fname = _("IDS_FILEEXT_FNAME_VCS");
				filter = _("IDS_FILEEXT_FILTER_VCS");
			}
			else
			{
				CLogger::Log(L"WIZARD: WIZ_EXPORT_CALENDAR_ICAL");
				fname = _("IDS_FILEEXT_FNAME_ICS");
				filter = _("IDS_FILEEXT_FILTER_ICS");
			}
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				fname,
				filter,
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
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
				std::stringstream outData;
				int nWarning = CAgilityBookOptions::CalendarOpeningNear();
				ICalendarPtr iCalendar = ICalendar::iCalendarBegin(outData, (WIZ_EXPORT_CALENDAR_VCAL == data) ? 1 : 2);
				for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end();
					 ++iterCal)
				{
					ARBCalendarPtr pCal = *iterCal;
					pCal->iCalendar(iCalendar, nWarning);
				}
				iCalendar.reset();
				wxFFileOutputStream output(file.GetPath(), L"wb");
				if (output.IsOk())
				{
					std::string const& str = outData.str();
					output.Write(str.c_str(), str.length());
					output.Close();
				}
				bOk = true;
			}
		}
		break;

		case WIZ_IMPORT_LOG:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_IMPORT_LOG");
			CLogger::Log(L"WIZARD: CWizardStart ");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (wxID_OK == file.ShowModal())
			{
				ElementNodePtr tree(ElementNode::New());
				bool bLoadOk = false;
				{
					wxBusyCursor wait;
					bLoadOk = tree->LoadXML(file.GetPath().wc_str(), errMsg);
				}
				if (!bLoadOk)
				{
					wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
					if (0 < errMsg.size())
					{
						msg << L"\n\n" << fmt::to_string(errMsg);
					}
					wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				}
				else
					bOk = m_pDoc->ImportARBLogData(tree, this);
			}
		}
		break;

		case WIZ_EXPORT_LOG:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_LOG");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
				std::wstring verstr = ver.GetVersionString();
				ElementNodePtr tree(ElementNode::New());
				if (m_pDoc->Book().Save(tree, verstr, false, true, false, false, false))
				{
					tree->SaveXML(StringUtil::stringW(file.GetPath()));
				}
				bOk = true;
			}
		}
		break;

		case WIZ_IMPORT_CONFIGURATION:
			CLogger::Log(L"WIZARD: CWizardStart WIZ_IMPORT_CONFIGURATION");
			bOk = m_pDoc->ImportConfiguration(false);
			if (bOk)
				m_pDoc->Modify(true);
			break;

		case WIZ_EXPORT_CONFIGURATION:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_CONFIGURATION");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				_("IDS_FILEEXT_FNAME_ARB"),
				_("IDS_FILEEXT_FILTER_ARB"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
				std::wstring verstr = ver.GetVersionString();
				ElementNodePtr tree(ElementNode::New());
				if (m_pDoc->Book().Save(tree, verstr, false, false, true, false, false))
				{
					tree->SaveXML(StringUtil::stringW(file.GetPath()));
				}
				bOk = true;
			}
		}
		break;

		case WIZ_EXPORT_DTD:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_DTD");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				L"AgilityRecordBook.dtd",
				_("IDS_FILEEXT_FILTER_DTD"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				wxFFileOutputStream output(file.GetPath(), L"wb");
				if (output.IsOk())
				{
					CConfigHandler handler;
					std::string dtd = ARBConfig::GetDTD(&handler);
					output.Write(dtd.c_str(), dtd.length());
					output.Close();
				}
				bOk = true;
			}
		}
		break;

		case WIZ_EXPORT_XML:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_XML");
			wxString name = m_pDoc->GetFilename();
			if (name.empty())
			{
				name = L"AgilityRecordBook.";
				name += _("IDS_FILEEXT_DEF_XML");
			}
			else
			{
				int iDot = name.Find('.', true);
				if (0 <= iDot)
					name = name.Left(iDot + 1) + _("IDS_FILEEXT_DEF_XML");
				else
				{
					name += L".";
					name += _("IDS_FILEEXT_DEF_XML");
				}
			}
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				name,
				_("IDS_FILEEXT_FILTER_XML"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
				std::wstring verstr = ver.GetVersionString();
				ElementNodePtr tree(ElementNode::New());
				if (m_pDoc->Book().Save(tree, verstr, true, true, true, true, true))
				{
					CConfigHandler handler;
					tree->SaveXML(StringUtil::stringW(file.GetPath()), ARBConfig::GetDTD(&handler));
				}
				bOk = true;
			}
		}
		break;

		case WIZ_IMPORT_SETTINGS:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_IMPORT_SETTINGS");
			wxString name = L"AgilityRecordBook.";
			name += _("IDS_FILEEXT_DEF_SETTINGS");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				name,
				_("IDS_FILEEXT_FILTER_SETTINGS"),
				wxFD_OPEN | wxFD_FILE_MUST_EXIST);
			if (wxID_OK == file.ShowModal())
			{
				ElementNodePtr tree(ElementNode::New());
				bool bLoadOk = false;
				{
					wxBusyCursor wait;
					bLoadOk = tree->LoadXML(file.GetPath().wc_str(), errMsg);
				}
				if (bLoadOk && CAgilityBookOptions::ImportSettings(tree))
				{
					bOk = true;
				}
				else
				{
					wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
					if (0 < errMsg.size())
					{
						msg << L"\n\n" << fmt::to_string(errMsg);
					}
					wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
				}
			}
		}
		break;

		case WIZ_EXPORT_SETTINGS:
		{
			CLogger::Log(L"WIZARD: CWizardStart WIZ_EXPORT_SETTINGS");
			wxString name = L"AgilityRecordBook.";
			name += _("IDS_FILEEXT_DEF_SETTINGS");
			wxFileDialog file(
				this,
				wxEmptyString, // caption
				wxEmptyString, // def dir
				name,
				_("IDS_FILEEXT_FILTER_SETTINGS"),
				wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
			if (wxID_OK == file.ShowModal())
			{
				wxBusyCursor wait;
				CVersionNum ver(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD);
				std::wstring verstr = ver.GetVersionString();
				ElementNodePtr settings = CAgilityBookOptions::ExportSettings();
				if (settings)
				{
					settings->SaveXML(StringUtil::stringW(file.GetPath()));
					bOk = true;
				}
			}
		}
		break;
		}
	}
	return bOk;
}

} // namespace dconSoft
