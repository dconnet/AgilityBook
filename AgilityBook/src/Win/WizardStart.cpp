/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Import/Export Wizard
 * @author David Connet
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2010-03-28 DRC Added ability to import/export program settings.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-07-14 DRC Fixed group box creation order.
 * @li 2009-06-14 DRC Fix wizard finish (wxEVT_WIZARD_FINISHED is only invoked
 *                    _after_ the dialog is destroyed).
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-08-09 DRC When importing ARB files, update regnum and titles
 *                    that are different.
 * @li 2005-07-14 DRC Remember selected export item.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-10 DRC Allow titles to be optionally entered multiple times.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-21 DRC Added ability to import dog/trial/run/judge info.
 * @li 2004-02-26 DRC Moved configuration update logic to the doc.
 * @li 2004-02-18 DRC Added 'DeleteTitle' configuration action.
 * @li 2004-01-26 DRC Display errors on non-fatal load.
 * @li 2004-01-21 DRC Implemented Action items in configuration update.
 * @li 2003-12-10 DRC Created
 */

#include "stdafx.h"
#include "WizardStart.h"

#include "AgilityBookDoc.h"
#include "AgilityBookOptions.h"
#include "ARBString.h"
#include "ConfigHandler.h"
#include "DlgMessage.h"
#include "Element.h"
#include "Wizard.h"
#include "VersionNum.h"
#include <wx/config.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


// Registry settings in "Last"
#define LAST_STYLE		wxT("Last/WizStyle")
#define LAST_STYLEITEM	wxT("Last/WizSubStyle") // A number will be appended
// Note: LAST_STYLE is a fixed number, regardless of UI order.
// LAST_STYLEITEM actually uses the UI order (as of v1.10). If items are
// reordered, this _will_ cause a problem. As of v2.0, this will change to
// the registry key to use the actual style, not the UI order. There will be
// no backwards compatibility - this simply means that the wrong item may be
// initially selected the first time this is run (if the user ran the wizard
// in v1)


static struct
{
	long regValue;
	wchar_t const* id;
} s_ImportExportChoices[] =
{
	{WIZARD_RADIO_EXCEL, wxT("IDC_WIZARD_START_EXCEL")},
	{WIZARD_RADIO_CALC, wxT("IDC_WIZARD_START_CALC")},
	{WIZARD_RADIO_SPREADSHEET, wxT("IDC_WIZARD_START_SPREADSHEET")},
	{WIZARD_RADIO_ARB, wxT("IDC_WIZARD_START_ARB")}
};
static const long s_numImportExportChoices = sizeof(s_ImportExportChoices) / sizeof(s_ImportExportChoices[0]);

/////////////////////////////////////////////////////////////////////////////
// CWizardStart property page

IMPLEMENT_CLASS(CWizardStart, wxWizardPage)


CWizardStart::CWizardStart(
		CWizard* pSheet,
		CAgilityBookDoc* pDoc)
	: wxWizardPage(pSheet)
	, m_pSheet(pSheet)
	, m_pDoc(pDoc)
	, m_ctrlList(NULL)
	, m_ctrlDesc(NULL)
	, m_Style(-1)
	, m_Next(NULL)
{
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGING, wxWizardEventHandler, CWizardStart::OnWizardChanging);
	BIND_OR_CONNECT(wxEVT_WIZARD_PAGE_CHANGED, wxWizardEventHandler, CWizardStart::OnWizardChanged);

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
	wxRadioButton* radioExcel = NULL;
	if (m_pSheet->ExcelHelper())
	{
		radioExcel = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_START_EXCEL"),
			wxDefaultPosition, wxDefaultSize, wxRB_GROUP);
		BIND_OR_CONNECT_CTRL(radioExcel, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardStart::OnWizardStyleExcel);
		radioExcel->SetHelpText(_("HIDC_WIZARD_START_EXCEL"));
		radioExcel->SetToolTip(_("HIDC_WIZARD_START_EXCEL"));
	}

	wxRadioButton* radioCalc = NULL;
	if (m_pSheet->CalcHelper())
	{
		radioCalc = new wxRadioButton(this, wxID_ANY,
			_("IDC_WIZARD_START_CALC"),
			wxDefaultPosition, wxDefaultSize, 0);
		BIND_OR_CONNECT_CTRL(radioCalc, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardStart::OnWizardStyleCalc);
		radioCalc->SetHelpText(_("HIDC_WIZARD_START_CALC"));
		radioCalc->SetToolTip(_("HIDC_WIZARD_START_CALC"));
	}

	wxRadioButton* radioSpread = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_START_SPREADSHEET"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(radioSpread, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardStart::OnWizardStyleSpread);
	radioSpread->SetHelpText(_("HIDC_WIZARD_START_SPREADSHEET"));
	radioSpread->SetToolTip(_("HIDC_WIZARD_START_SPREADSHEET"));

	wxRadioButton* radioArb = new wxRadioButton(this, wxID_ANY,
		_("IDC_WIZARD_START_ARB"),
		wxDefaultPosition, wxDefaultSize, 0);
	BIND_OR_CONNECT_CTRL(radioArb, wxEVT_COMMAND_RADIOBUTTON_SELECTED, wxCommandEventHandler, CWizardStart::OnWizardStyleArb);
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

	m_ctrlList = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxSize(200, -1),
		0, NULL, wxLB_HSCROLL|wxLB_SINGLE);
	BIND_OR_CONNECT_CTRL(m_ctrlList, wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler, CWizardStart::OnSelchangeExportList);
	m_ctrlList->SetHelpText(_("HIDC_WIZARD_START_LIST"));
	m_ctrlList->SetToolTip(_("HIDC_WIZARD_START_LIST"));

	wxStaticBox* boxDesc = new wxStaticBox(this, wxID_ANY, _("IDC_WIZARD_START_DESCRIPTION"));

	m_ctrlDesc = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxSize(200, -1), wxST_NO_AUTORESIZE);
	m_ctrlDesc->Wrap(200);

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxStaticBoxSizer* sizerImportExport = new wxStaticBoxSizer(boxImportExport, wxVERTICAL);
	if (radioExcel)
		sizerImportExport->Add(radioExcel, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	if (radioCalc)
		sizerImportExport->Add(radioCalc, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerImportExport->Add(radioSpread, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerImportExport->Add(radioArb, 0, wxBOTTOM|wxLEFT|wxRIGHT|wxTOP, 5);
	bSizer->Add(sizerImportExport, 0, wxALL, 5);

	wxBoxSizer* sizerAction = new wxBoxSizer(wxHORIZONTAL);
	sizerAction->Add(m_ctrlList, 1, wxALL|wxEXPAND, 5);

	wxStaticBoxSizer* sizerDesc = new wxStaticBoxSizer(boxDesc, wxVERTICAL);
	sizerDesc->Add(m_ctrlDesc, 1, wxALL|wxEXPAND, 5);

	sizerAction->Add(sizerDesc, 1, wxEXPAND|wxLEFT|wxRIGHT, 5);

	bSizer->Add(sizerAction, 1, wxEXPAND, 5);

	SetSizer(bSizer);
	bSizer->Fit(this);
}

/////////////////////////////////////////////////////////////////////////////

enum ePage
{
	ePageNone,
	ePageFinish,
	ePageImport,
	ePageExport,
};

// Note, order of 'data' changed between v1 and v2. In v1, order was UI based.
// In v2, it's id based.
static struct
{
	// For integrity checking. See Wizard.h.
	int index;
	struct
	{
		ePage nextPage;
		// Listing (NULL denotes no entry)
		wchar_t const* item;
		// Description shown when listing is selected.
		wchar_t const* desc;
	} data[4]; // Data must agree with radio buttons. [WIZARD_RADIO_*]
	// excel, spread, arb, calc
} const sc_Items[] =
{
	{WIZ_IMPORT_RUNS,
	{
		{ePageImport, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_EXCEL")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_SPREAD")},
		{ePageFinish, arbT("IDS_WIZ_IMPORT_RUNS_PLUS"), arbT("IDS_WIZ_IMPORT_RUNS_ARB")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_RUNS"), arbT("IDS_WIZ_IMPORT_RUNS_CALC")},
	} },
	{WIZ_EXPORT_RUNS,
	{
		{ePageExport, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_EXCEL")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_SPREAD")},
		{ePageNone, NULL, NULL},
		{ePageExport, arbT("IDS_WIZ_EXPORT_RUNS"), arbT("IDS_WIZ_EXPORT_RUNS_CALC")},
	} },
	{WIZ_IMPORT_CALENDAR,
	{
		{ePageImport, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_EXCEL")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_SPREAD")},
		{ePageFinish, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_ARB")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_CAL"), arbT("IDS_WIZ_IMPORT_CAL_CALC")},
	} },
	{WIZ_EXPORT_CALENDAR,
	{
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_EXCEL")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_SPREAD")},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_ARB")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL"), arbT("IDS_WIZ_EXPORT_CAL_CALC")},
	} },
	{WIZ_EXPORT_CALENDAR_VCAL,
	{
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_CAL_VCAL"), arbT("IDS_WIZ_EXPORT_CAL_VCAL_SPREAD")},
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_CALENDAR_ICAL,
	{
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_CAL_ICAL"), arbT("IDS_WIZ_EXPORT_CAL_ICAL_SPREAD")},
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_CALENDAR_APPT,
	{
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL_APPT"), arbT("IDS_WIZ_EXPORT_CAL_APPT_EXCEL")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL_APPT"), arbT("IDS_WIZ_EXPORT_CAL_APPT_SPREAD")},
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_CALENDAR_TASK,
	{
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL_TASK"), arbT("IDS_WIZ_EXPORT_CAL_TASK_EXCEL")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_CAL_TASK"), arbT("IDS_WIZ_EXPORT_CAL_TASK_SPREAD")},
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_IMPORT_LOG,
	{
		{ePageImport, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_EXCEL")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_SPREAD")},
		{ePageFinish, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_ARB")},
		{ePageImport, arbT("IDS_WIZ_IMPORT_LOG"), arbT("IDS_WIZ_IMPORT_LOG_CALC")},
	} },
	{WIZ_EXPORT_LOG,
	{
		{ePageExport, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_EXCEL")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_SPREAD")},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_ARB")},
		{ePageExport, arbT("IDS_WIZ_EXPORT_LOG"), arbT("IDS_WIZ_EXPORT_LOG_CALC")},
	} },
	{WIZ_IMPORT_CONFIGURATION,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_IMPORT_CONFIG"), arbT("IDS_WIZ_IMPORT_CONFIG_ARB")},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_CONFIGURATION,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_CONFIG"), arbT("IDS_WIZ_EXPORT_CONFIG_ARB")},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_DTD,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_DTD"), arbT("IDS_WIZ_EXPORT_DTD_ARB")},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_XML,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_XML"), arbT("IDS_WIZ_EXPORT_XML_ARB")},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_IMPORT_SETTINGS,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_IMPORT_SETTINGS"), arbT("IDS_WIZ_IMPORT_SETTINGS_ARB")},
		{ePageNone, NULL, NULL},
	} },
	{WIZ_EXPORT_SETTINGS,
	{
		{ePageNone, NULL, NULL},
		{ePageNone, NULL, NULL},
		{ePageFinish, arbT("IDS_WIZ_EXPORT_SETTINGS"), arbT("IDS_WIZ_EXPORT_SETTINGS_ARB")},
		{ePageNone, NULL, NULL},
	} },
};
static int const sc_nItems = sizeof(sc_Items) / sizeof(sc_Items[0]);


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
		if (m_pSheet->GetCalendarEntries()
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_VCAL
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_ICAL
		&& sc_Items[i].index != WIZ_EXPORT_CALENDAR_APPT
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
			m_ctrlList->SetClientData(index, (void*)i);
	}
	wxString str;
	str << LAST_STYLEITEM << m_Style;
	long idx = wxConfig::Get()->Read(str, -1L);
	m_ctrlList->SetSelection(idx);
	DoUpdateExportList(bInit);
}


void CWizardStart::DoUpdateExportList(bool bInit)
{
	int index = m_ctrlList->GetSelection();
	if (wxNOT_FOUND == index)
		return;

	int idx = (int)m_ctrlList->GetClientData(index);

	bool bEnableNext = true;
	switch (sc_Items[idx].data[m_Style].nextPage)
	{
	default:
		assert(0);
	case ePageNone:
		bEnableNext = false;
		// fallthru
	case ePageFinish:
		m_Next = NULL;
		break;
	case ePageImport:
		m_Next = m_pSheet->GetImportPage();
		break;
	case ePageExport:
		m_Next = m_pSheet->GetExportPage();
		break;
	}
	if (!bInit)
		m_pSheet->UpdateButtons(bEnableNext);

	wxString msg = wxGetTranslation(sc_Items[idx].data[m_Style].desc);
	m_ctrlDesc->SetLabel(msg);

	wxString str;
	str << LAST_STYLEITEM << m_Style;
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
		int data = (int)m_ctrlList->GetClientData(index);
		m_pSheet->SetImportExportItem(data, m_Style);
		if (ePageFinish == sc_Items[data].data[m_Style].nextPage
		&& !DoWizardFinish())
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
		int data = (int)m_ctrlList->GetClientData(index);
		switch (data)
		{
		default:
			break;

		case WIZ_IMPORT_RUNS:
			{
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_OPEN | wxFD_FILE_MUST_EXIST);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					std::wostringstream errMsg;
					ElementNodePtr tree(ElementNode::New());
					if (!tree->LoadXML(file.GetPath(), errMsg))
					{
						wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
						if (0 < errMsg.str().length())
						{
							msg << wxT("\n\n") << errMsg.str().c_str();
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
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_OPEN | wxFD_FILE_MUST_EXIST);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					std::wostringstream errMsg;
					ElementNodePtr tree(ElementNode::New());
					if (!tree->LoadXML(file.GetPath(), errMsg))
					{
						wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
						if (0 < errMsg.str().length())
						{
							msg << wxT("\n\n") << errMsg.str().c_str();
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
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					CVersionNum ver(true);
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
					fname = _("IDS_FILEEXT_FNAME_VCS");
					filter = _("IDS_FILEEXT_FILTER_VCS");
				}
				else
				{
					fname = _("IDS_FILEEXT_FNAME_ICS");
					filter = _("IDS_FILEEXT_FILTER_ICS");
				}
				wxFileDialog file(this,
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
						for (ARBCalendarList::const_iterator iterCal = m_pDoc->Book().GetCalendar().begin(); iterCal != m_pDoc->Book().GetCalendar().end(); ++iterCal)
						{
							allEntries.push_back(*iterCal);
						}
						entries = &allEntries;
					}
					wxFFileOutputStream output(file.GetPath(), wxT("wb"));
					if (output.IsOk())
					{
						int nWarning = CAgilityBookOptions::CalendarOpeningNear();
						ICalendar* iCalendar = ICalendar::iCalendarBegin(output, (WIZ_EXPORT_CALENDAR_VCAL == data) ? 1 : 2);
						for (std::vector<ARBCalendarPtr>::const_iterator iterCal = entries->begin(); iterCal != entries->end(); ++iterCal)
						{
							ARBCalendarPtr pCal = *iterCal;
							pCal->iCalendar(iCalendar, nWarning);
						}
						iCalendar->Release();
						output.Close();
					}
					bOk = true;
				}
			}
			break;

		case WIZ_IMPORT_LOG:
			{
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_OPEN | wxFD_FILE_MUST_EXIST);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					std::wostringstream errMsg;
					ElementNodePtr tree(ElementNode::New());
					if (!tree->LoadXML(file.GetPath(), errMsg))
					{
						wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
						if (0 < errMsg.str().length())
						{
							msg << wxT("\n\n") << errMsg.str().c_str();
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
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					CVersionNum ver(true);
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
			bOk = m_pDoc->ImportConfiguration(false);
			if (bOk)
				m_pDoc->Modify(true);
			break;

		case WIZ_EXPORT_CONFIGURATION:
			{
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					_("IDS_FILEEXT_FNAME_ARB"),
					_("IDS_FILEEXT_FILTER_ARB"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					CVersionNum ver(true);
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
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					wxT("AgilityRecordBook.dtd"),
					_("IDS_FILEEXT_FILTER_DTD"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					wxFFileOutputStream output(file.GetPath(), wxT("wb"));
					if (output.IsOk())
					{
						CConfigHandler handler;
						std::string dtd = ARBConfig::GetDTD(&handler, false);
						output.Write(dtd.c_str(), dtd.length());
						output.Close();
					}
					bOk = true;
				}
			}
			break;

		case WIZ_EXPORT_XML:
			{
				wxString name = m_pDoc->GetFilename();
				if (name.empty())
				{
					name = wxT("AgilityRecordBook.");
					name += _("IDS_FILEEXT_DEF_XML");
				}
				else
				{
					int iDot = name.Find('.', true);
					if (0 <= iDot)
						name = name.Left(iDot+1) + _("IDS_FILEEXT_DEF_XML");
					else
					{
						name += wxT(".");
						name += _("IDS_FILEEXT_DEF_XML");
					}
				}
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					name,
					_("IDS_FILEEXT_FILTER_XML"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					CVersionNum ver(true);
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
				wxString name = wxT("AgilityRecordBook.");
				name += _("IDS_FILEEXT_DEF_SETTINGS");
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					name,
					_("IDS_FILEEXT_FILTER_SETTINGS"),
					wxFD_OPEN | wxFD_FILE_MUST_EXIST);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					std::wostringstream errMsg;
					ElementNodePtr tree(ElementNode::New());
					if (tree->LoadXML(file.GetPath(), errMsg)
					&& CAgilityBookOptions::ImportSettings(tree))
					{
						bOk = true;
					}
					else
					{
						wxString msg(_("AFX_IDP_FAILED_TO_OPEN_DOC"));
						if (0 < errMsg.str().length())
						{
							msg << wxT("\n\n") << errMsg.str().c_str();
						}
						wxMessageBox(msg, wxMessageBoxCaptionStr, wxOK | wxCENTRE | wxICON_EXCLAMATION);
					}
				}
			}
			break;

		case WIZ_EXPORT_SETTINGS:
			{
				wxString name = wxT("AgilityRecordBook.");
				name += _("IDS_FILEEXT_DEF_SETTINGS");
				wxFileDialog file(this,
					wxEmptyString, // caption
					wxEmptyString, // def dir
					name,
					_("IDS_FILEEXT_FILTER_SETTINGS"),
					wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
				if (wxID_OK == file.ShowModal())
				{
					wxBusyCursor wait;
					CVersionNum ver(true);
					wxString verstr = ver.GetVersionString();
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
