/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief implementation of the CDlgConfigEvent class
 * @author David Connet
 *
 * There's one caveat to this dialog - there is no verification done to
 * make sure that a division/level pair isn't multiply defined. If that
 * happens, when we go to check things, the first match in the list will
 * be the only one used.
 * Note: The UI checks when creating div/level, so this shouldn't happen.
 * If the file is hand-edited, it could... But just how paranoid do we get?!
 * (Plus, the paranoia checking should be done when the file is loaded.)
 *
 * Revision History
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2008-07-29 DRC Method overlap detection was wrong.
 * @li 2007-05-08 DRC Fixed bug when deleting a method.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-12-04 DRC Added support for NADAC bonus titling points.
 * @li 2005-06-25 DRC Cleaned up reference counting when returning a pointer.
 * @li 2005-01-02 DRC Added subnames to events.
 * @li 2005-01-01 DRC Renamed MachPts to SpeedPts.
 * @li 2004-12-18 DRC Added a time fault multiplier.
 * @li 2004-12-10 DRC Changes weren't saved when lifetime pts were modified.
 * @li 2004-11-15 DRC Added time fault computation option on T+F.
 * @li 2004-04-01 DRC Fixed? the memory access fault.
 * @li 2003-12-27 DRC Added support for from/to dates for the scoring method.
 */

#include "stdafx.h"
#include "DlgConfigEvent.h"

#include "ARBAgilityRecordBook.h"
#include "ARBConfigEvent.h"
#include "ARBConfigVenue.h"
#include "DlgConfigEventMethod.h"
#include "DlgConfigTitlePoints.h"
#include "DlgConfigure.h"
#include "DlgName.h"
#include "ListData.h"
#include "Validators.h"
#include <algorithm>
#include <wx/statline.h>

class CConfigEventDataLifetimePoints : public wxClientData
{
public:
	CConfigEventDataLifetimePoints(ARBConfigLifetimePointsPtr data)
		: m_Data(data)
	{
	}
	ARBConfigLifetimePointsPtr GetData() const	{return m_Data;}
private:
	ARBConfigLifetimePointsPtr m_Data;
};


class CConfigEventDataPlaceInfo : public wxClientData
{
public:
	CConfigEventDataPlaceInfo(ARBConfigPlaceInfoPtr data)
		: m_Data(data)
	{
	}
	ARBConfigPlaceInfoPtr GetData() const	{return m_Data;}
private:
	ARBConfigPlaceInfoPtr m_Data;
};


class CConfigEventDataScoring : public wxClientData
{
public:
	CConfigEventDataScoring(ARBConfigScoringPtr data)
		: m_Data(data)
	{
	}
	ARBConfigScoringPtr GetData() const	{return m_Data;}
private:
	ARBConfigScoringPtr m_Data;
};


class CConfigEventDataTitlePoints : public wxClientData
{
public:
	CConfigEventDataTitlePoints(ARBConfigTitlePointsPtr data)
		: m_Data(data)
	{
	}
	ARBConfigTitlePointsPtr GetData() const	{return m_Data;}
private:
	ARBConfigTitlePointsPtr m_Data;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgConfigEvent, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgConfigEvent::OnOk)
END_EVENT_TABLE()


CDlgConfigEvent::CDlgConfigEvent(
		bool bNewEntry,
		ARBConfigVenuePtr pVenue,
		ARBConfigEventPtr pEvent,
		wxWindow* pParent)
	: wxDialog(pParent, wxID_ANY, _("IDD_CONFIG_EVENT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE)
	, m_bNewEntry(bNewEntry)
	, m_pVenue(pVenue)
	, m_pEvent(pEvent)
	, m_DlgFixup()
	, m_Scorings()
	, m_Name(m_pEvent->GetName().c_str())
	, m_bHasTable(m_pEvent->HasTable())
	, m_bHasPartners(m_pEvent->HasPartner())
	, m_bHasSubNames(m_pEvent->HasSubNames())
	, m_Desc(m_pEvent->GetDesc().c_str())
	, m_ctrlName(NULL)
	, m_ctrlSubNames(NULL)
	, m_ctrlSubNamesNew(NULL)
	, m_ctrlSubNamesEdit(NULL)
	, m_ctrlSubNamesDelete(NULL)
	, m_ctrlNew(NULL)
	, m_ctrlEdit(NULL)
	, m_ctrlDelete(NULL)
	, m_ctrlCopy(NULL)
	, m_ctrlUp(NULL)
	, m_ctrlDown(NULL)
	, m_ctrlMethods(NULL)
	, m_ctrlUnused(NULL)
	, m_ctrlInfo(NULL)
	, m_ctrlPointsList(NULL)
	, m_ctrlPointsNew(NULL)
	, m_ctrlPointsEdit(NULL)
	, m_ctrlPointsDelete(NULL)
	, m_ctrlNote(NULL)
	, m_idxMethod(-1)
{
	assert(m_pVenue);
	assert(m_pEvent);
	// Copy the existing scorings.
	pEvent->GetScorings().Clone(m_Scorings);

	// Controls (these are done first to control tab order)

	wxStaticText* textName = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT"),
		wxDefaultPosition, wxDefaultSize, 0);
	textName->Wrap(-1);

	m_ctrlName = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, 0,
		CTrimValidator(&m_Name));
	m_ctrlName->SetHelpText(_("HIDC_CONFIG_EVENT"));
	m_ctrlName->SetToolTip(_("HIDC_CONFIG_EVENT"));

	wxCheckBox* ctrlTable = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_TABLE"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHasTable));
	ctrlTable->SetHelpText(_("HIDC_CONFIG_EVENT_TABLE"));
	ctrlTable->SetToolTip(_("HIDC_CONFIG_EVENT_TABLE"));

	wxCheckBox* ctrlPartners = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_PARTNER"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHasPartners));
	ctrlPartners->SetHelpText(_("HIDC_CONFIG_EVENT_PARTNER"));
	ctrlPartners->SetToolTip(_("HIDC_CONFIG_EVENT_PARTNER"));

	wxCheckBox* ctrlHasSubnames = new wxCheckBox(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_HAS_SUBNAMES"),
		wxDefaultPosition, wxDefaultSize, 0,
		wxGenericValidator(&m_bHasSubNames));
	ctrlHasSubnames->Connect(wxEVT_COMMAND_CHECKBOX_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnClickedSubNames), NULL, this);
	ctrlHasSubnames->SetHelpText(_("HIDC_CONFIG_EVENT_HAS_SUBNAMES"));
	ctrlHasSubnames->SetToolTip(_("HIDC_CONFIG_EVENT_HAS_SUBNAMES"));

	wxStaticText* textNote = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_DESC"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNote->Wrap(-1);

	wxTextCtrl* ctrlNote = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP,
		CTrimValidator(&m_Desc));
	ctrlNote->SetHelpText(_("HIDC_CONFIG_EVENT_DESC"));
	ctrlNote->SetToolTip(_("HIDC_CONFIG_EVENT_DESC"));

	m_ctrlSubNames = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxLB_SINGLE|wxLB_SORT);
	m_ctrlSubNames->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgConfigEvent::OnLbnSelchangeSubnames), NULL, this);
	m_ctrlSubNames->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(CDlgConfigEvent::OnLbnDblclkSubnames), NULL, this);
	m_ctrlSubNames->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES"));
	m_ctrlSubNames->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES"));

	m_ctrlSubNamesNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_NEW"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedSubNamesNew), NULL, this);
	m_ctrlSubNamesNew->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_NEW"));
	m_ctrlSubNamesNew->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_NEW"));

	m_ctrlSubNamesEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_EDIT"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedSubNamesEdit), NULL, this);
	m_ctrlSubNamesEdit->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_EDIT"));
	m_ctrlSubNamesEdit->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_EDIT"));

	m_ctrlSubNamesDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_SUBNAMES_DELETE"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlSubNamesDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedSubNamesDelete), NULL, this);
	m_ctrlSubNamesDelete->SetHelpText(_("HIDC_CONFIG_EVENT_SUBNAMES_DELETE"));
	m_ctrlSubNamesDelete->SetToolTip(_("HIDC_CONFIG_EVENT_SUBNAMES_DELETE"));

	wxStaticText* textDefined = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_METHODS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDefined->Wrap(-1);

	m_ctrlMethods = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	m_ctrlMethods->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(CDlgConfigEvent::OnLbnDblclkMethods), NULL, this);
	m_ctrlMethods->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgConfigEvent::OnLbnSelchangeMethods), NULL, this);
	m_ctrlMethods->SetHelpText(_("HIDC_CONFIG_EVENT_METHODS"));
	m_ctrlMethods->SetToolTip(_("HIDC_CONFIG_EVENT_METHODS"));

	m_ctrlNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_NEW"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedNew), NULL, this);
	m_ctrlNew->SetHelpText(_("HIDC_CONFIG_EVENT_NEW"));
	m_ctrlNew->SetToolTip(_("HIDC_CONFIG_EVENT_NEW"));

	m_ctrlEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_EDIT"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedEdit), NULL, this);
	m_ctrlEdit->SetHelpText(_("HIDC_CONFIG_EVENT_EDIT"));
	m_ctrlEdit->SetToolTip(_("HIDC_CONFIG_EVENT_EDIT"));

	m_ctrlDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_DELETE"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedDelete), NULL, this);
	m_ctrlDelete->SetHelpText(_("HIDC_CONFIG_EVENT_DELETE"));
	m_ctrlDelete->SetToolTip(_("HIDC_CONFIG_EVENT_DELETE"));

	m_ctrlCopy = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_COPY"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCopy->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedCopy), NULL, this);
	m_ctrlCopy->SetHelpText(_("HIDC_CONFIG_EVENT_COPY"));
	m_ctrlCopy->SetToolTip(_("HIDC_CONFIG_EVENT_COPY"));
 
	m_ctrlUp = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_MOVE_UP"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlUp->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedUp), NULL, this);
	m_ctrlUp->SetHelpText(_("HIDC_CONFIG_EVENT_MOVE_UP"));
	m_ctrlUp->SetToolTip(_("HIDC_CONFIG_EVENT_MOVE_UP"));

	m_ctrlDown = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_MOVE_DOWN"),
		wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlDown->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnBnClickedDown), NULL, this);
	m_ctrlDown->SetHelpText(_("HIDC_CONFIG_EVENT_MOVE_DOWN"));
	m_ctrlDown->SetToolTip(_("HIDC_CONFIG_EVENT_MOVE_DOWN"));

	wxStaticText* textNotDefined = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_UNUSED"),
		wxDefaultPosition, wxDefaultSize, 0);
	textNotDefined->Wrap(-1);

	m_ctrlUnused = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, 0);
	m_ctrlUnused->SetHelpText(_("HIDC_CONFIG_EVENT_UNUSED"));
	m_ctrlUnused->SetToolTip(_("HIDC_CONFIG_EVENT_UNUSED"));
	m_ctrlUnused->Enable(false);

	m_ctrlInfo = new wxStaticText(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxST_NO_AUTORESIZE|wxSUNKEN_BORDER);
	m_ctrlInfo->Connect(wxEVT_COMMAND_LEFT_DCLICK, wxMouseEventHandler(CDlgConfigEvent::OnDblclickConfigInfo), NULL, this);
	m_ctrlInfo->Wrap(-1);
	m_ctrlInfo->SetHelpText(_("HIDC_CONFIG_EVENT_INFO"));
	m_ctrlInfo->SetToolTip(_("HIDC_CONFIG_EVENT_INFO"));

	wxStaticText* textTitlePoints = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS"),
		wxDefaultPosition, wxDefaultSize, 0);
	textTitlePoints->Wrap( -1 );

	m_ctrlPointsList = new wxListBox(this, wxID_ANY,
		wxDefaultPosition, wxDefaultSize,
		0, NULL, wxLB_SINGLE);
	m_ctrlPointsList->Connect(wxEVT_COMMAND_LISTBOX_SELECTED, wxCommandEventHandler(CDlgConfigEvent::OnSelchangePoints), NULL, this);
	m_ctrlPointsList->Connect(wxEVT_COMMAND_LISTBOX_DOUBLECLICKED, wxCommandEventHandler(CDlgConfigEvent::OnDblclkPoints), NULL, this);
	m_ctrlPointsList->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS"));
	m_ctrlPointsList->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS"));

	m_ctrlPointsNew = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_NEW"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsNew->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnPointsNew), NULL, this);
	m_ctrlPointsNew->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_NEW"));
	m_ctrlPointsNew->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_NEW"));

	m_ctrlPointsEdit = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_EDIT"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsEdit->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnPointsEdit), NULL, this);
	m_ctrlPointsEdit->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_EDIT"));
	m_ctrlPointsEdit->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_EDIT"));

	m_ctrlPointsDelete = new wxButton(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_POINTS_DELETE"),
		wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
	m_ctrlPointsDelete->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CDlgConfigEvent::OnPointsDelete), NULL, this);
	m_ctrlPointsDelete->SetHelpText(_("HIDC_CONFIG_EVENT_POINTS_DELETE"));
	m_ctrlPointsDelete->SetToolTip(_("HIDC_CONFIG_EVENT_POINTS_DELETE"));

	wxStaticText* textMethodNote = new wxStaticText(this, wxID_ANY,
		_("IDC_CONFIG_EVENT_NOTES"),
		wxDefaultPosition, wxDefaultSize, 0);
	textMethodNote->Wrap(-1);

	m_ctrlNote = new wxTextCtrl(this, wxID_ANY, wxEmptyString,
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE|wxTE_WORDWRAP);
	m_ctrlNote->SetHelpText(_("HIDC_CONFIG_EVENT_NOTES"));
	m_ctrlNote->SetToolTip(_("HIDC_CONFIG_EVENT_NOTES"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* sizerTop = new wxFlexGridSizer(2, 2, 0, 0);
	sizerTop->SetFlexibleDirection(wxBOTH);
	sizerTop->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	wxBoxSizer* sizerName = new wxBoxSizer(wxHORIZONTAL);
	sizerName->Add(textName, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(m_ctrlName, 0, wxALL, 5);
	sizerName->Add(ctrlTable, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizerName->Add(ctrlPartners, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	sizerTop->Add(sizerName, 1, wxEXPAND, 5);
	sizerTop->Add(ctrlHasSubnames, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);

	wxBoxSizer* sizerNotes = new wxBoxSizer(wxHORIZONTAL);
	sizerNotes->Add(textNote, 0, wxALL, 5);
	sizerNotes->Add(ctrlNote, 1, wxALL|wxEXPAND, 5);

	sizerTop->Add(sizerNotes, 1, wxEXPAND, 5);

	wxBoxSizer* sizerSubname = new wxBoxSizer(wxHORIZONTAL);
	sizerSubname->Add(m_ctrlSubNames, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtnSub = new wxBoxSizer(wxVERTICAL);
	sizerBtnSub->Add(m_ctrlSubNamesNew, 0, wxALIGN_CENTER|wxALL, 5);
	sizerBtnSub->Add(m_ctrlSubNamesEdit, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtnSub->Add(m_ctrlSubNamesDelete, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerSubname->Add(sizerBtnSub, 0, wxEXPAND, 5);

	sizerTop->Add(sizerSubname, 1, wxEXPAND, 5);

	bSizer->Add(sizerTop, 0, wxEXPAND, 5);

	wxStaticBoxSizer* sizerMethods = new wxStaticBoxSizer(new wxStaticBox(this, wxID_ANY, _("IDC_CONFIG_EVENT_SCORING")), wxVERTICAL);

	wxBoxSizer* sizerDefinitions = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDefined = new wxBoxSizer(wxVERTICAL);
	sizerDefined->Add(textDefined, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerDefined->Add(m_ctrlMethods, 1, wxALL|wxEXPAND, 5);

	sizerDefinitions->Add(sizerDefined, 1, wxEXPAND, 5);

	wxBoxSizer* sizerBtns = new wxBoxSizer(wxVERTICAL);
	sizerBtns->Add(m_ctrlNew, 0, wxALL, 5);
	sizerBtns->Add(m_ctrlEdit, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlDelete, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlCopy, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlUp, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtns->Add(m_ctrlDown, 0, wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerDefinitions->Add(sizerBtns, 0, wxEXPAND, 5);

	wxBoxSizer* sizerNotDefined = new wxBoxSizer(wxVERTICAL);
	sizerNotDefined->Add(textNotDefined, 0, wxLEFT|wxRIGHT|wxTOP, 5);
	sizerNotDefined->Add(m_ctrlUnused, 1, wxALL|wxEXPAND, 5);

	sizerDefinitions->Add(sizerNotDefined, 1, wxEXPAND, 5);

	sizerMethods->Add(sizerDefinitions, 1, wxEXPAND, 5);

	wxStaticLine* staticLine = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	sizerMethods->Add(staticLine, 0, wxBOTTOM|wxEXPAND|wxTOP, 5);
	sizerMethods->Add(m_ctrlInfo, 0, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerPoints = new wxBoxSizer(wxHORIZONTAL);
	sizerPoints->Add(textTitlePoints, 0, wxALL, 5);
	sizerPoints->Add(m_ctrlPointsList, 1, wxALL|wxEXPAND, 5);

	wxBoxSizer* sizerBtnPts = new wxBoxSizer(wxVERTICAL);
	sizerBtnPts->Add(m_ctrlPointsNew, 0, wxALIGN_CENTER|wxALL, 5);
	sizerBtnPts->Add(m_ctrlPointsEdit, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5);
	sizerBtnPts->Add(m_ctrlPointsDelete, 0, wxALIGN_CENTER|wxBOTTOM|wxLEFT|wxRIGHT, 5);

	sizerPoints->Add(sizerBtnPts, 0, wxEXPAND, 5);
	sizerPoints->Add(textMethodNote, 0, wxALL, 5);
	sizerPoints->Add(m_ctrlNote, 1, wxALL|wxEXPAND, 5);

	sizerMethods->Add(sizerPoints, 0, wxEXPAND, 5);

	bSizer->Add(sizerMethods, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);

	FillSubNames(true);
	FillMethodList();
	FillControls();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnParent();
}


CDlgConfigEvent::~CDlgConfigEvent()
{
	ClearFixups();
}


void CDlgConfigEvent::GetFixups(std::vector<ARBConfigActionPtr>& ioFixups)
{
	ioFixups.insert(ioFixups.end(), m_DlgFixup.begin(), m_DlgFixup.end());
	m_DlgFixup.clear();
}


void CDlgConfigEvent::ClearFixups()
{
	m_DlgFixup.clear();
}


void CDlgConfigEvent::FillSubNames(bool bInit)
{
	if (m_bHasSubNames)
	{
		m_ctrlSubNames->Show(true);
		m_ctrlSubNamesNew->Show(true);
		m_ctrlSubNamesEdit->Show(true);
		m_ctrlSubNamesDelete->Show(true);
		if (bInit)
		{
			m_ctrlSubNames->Clear();
			std::set<tstring> subNames;
			m_pEvent->GetSubNames(subNames);
			for (std::set<tstring>::const_iterator iter = subNames.begin();
				iter != subNames.end();
				++iter)
			{
				m_ctrlSubNames->Append(iter->c_str());
			}
		}
	}
	else
	{
		m_ctrlSubNames->Show(false);
		m_ctrlSubNamesNew->Show(false);
		m_ctrlSubNamesEdit->Show(false);
		m_ctrlSubNamesDelete->Show(false);
	}
}


CConfigEventDataScoring* CDlgConfigEvent::GetScoringData(int index) const
{
	return dynamic_cast<CConfigEventDataScoring*>(m_ctrlMethods->GetClientObject(index));
}


CConfigEventDataTitlePoints* CDlgConfigEvent::GetTitleData(int index) const
{
	return dynamic_cast<CConfigEventDataTitlePoints*>(m_ctrlPointsList->GetClientObject(index));
}


CConfigEventDataLifetimePoints* CDlgConfigEvent::GetLifetimeData(int index) const
{
	return dynamic_cast<CConfigEventDataLifetimePoints*>(m_ctrlPointsList->GetClientObject(index));
}


CConfigEventDataPlaceInfo* CDlgConfigEvent::GetPlacementData(int index) const
{
	return dynamic_cast<CConfigEventDataPlaceInfo*>(m_ctrlPointsList->GetClientObject(index));
}


wxString CDlgConfigEvent::GetListName(ARBConfigScoringPtr pScoring) const
{
	wxString all = _("IDS_ALL");
	wxString str;
	if (pScoring->GetDivision() == WILDCARD_DIVISION)
		str = all;
	else
		str = pScoring->GetDivision().c_str();
	str += wxT(" / ");
	if (pScoring->GetLevel() == WILDCARD_LEVEL)
		str += all;
	else
		str += pScoring->GetLevel().c_str();
	tstring validStr = ARBDate::GetValidDateString(pScoring->GetValidFrom(), pScoring->GetValidTo());
	if (0 < validStr.length())
	{
		str += ' ';
		str += validStr.c_str();
	}
	return str;
}


void CDlgConfigEvent::EnableSubnameControls()
{
	bool bEnable = false;
	if (wxNOT_FOUND != m_ctrlSubNames->GetSelection())
		bEnable = true;
	m_ctrlSubNamesEdit->Enable(bEnable);
	m_ctrlSubNamesDelete->Enable(bEnable);
}


void CDlgConfigEvent::EnablePointsControls()
{
	bool bEnable = false;
	if (wxNOT_FOUND != m_ctrlPointsList->GetSelection())
		bEnable = true;
	m_ctrlPointsEdit->Enable(bEnable);
	m_ctrlPointsDelete->Enable(bEnable);
}


void CDlgConfigEvent::FillControls()
{
	bool bEnable = false;
	int idxMethod = m_ctrlMethods->GetSelection();
	m_ctrlInfo->SetLabel(wxT(""));
	m_ctrlPointsList->Clear();
	m_ctrlNote->SetValue(wxT(""));
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		if (pScoringData)
		{
			bEnable = true;
			ARBConfigScoringPtr pScoring = pScoringData->GetData();
			// Get info
			{
				wxString str1, str2;
				otstringstream info;
				ARBConfigScoring::ScoringStyle style = pScoring->GetScoringStyle();
				str1 = _("IDS_CONFIGEVENT_STYLE");
				info << str1 << wxT(": ")
					<< ARBConfigScoring::GetScoringStyleStr(style)
					<< wxT("\n");
				// The following strings should be the same as they are in
				// the Method Configuration dialog.
				switch (style)
				{
				default:
					break;
				case ARBConfigScoring::eFaultsThenTime:
				case ARBConfigScoring::eFaults100ThenTime:
				case ARBConfigScoring::eFaults200ThenTime:
					str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
					info << str1 << wxT(": ")
						<< pScoring->TimeFaultMultiplier();
					break;
				case ARBConfigScoring::eOCScoreThenTime:
    				str1 = _("IDS_CONFIGEVENT_REQOPEN");
    				str2 = _("IDS_CONFIGEVENT_REQCLOSE");
					info << str1 << wxT(": ")
						<< pScoring->GetRequiredOpeningPoints()
						<< wxT("; ") << str2 << wxT(": ")
						<< pScoring->GetRequiredClosingPoints();
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1 = _("IDS_CONFIGEVENT_TF_FROMSCORE");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1 = _("IDS_CONFIGEVENT_TF_UNDER");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1 = _("IDS_CONFIGEVENT_TF_OVER");
						info << wxT("; ") << str1;
					}
					break;
				case ARBConfigScoring::eScoreThenTime:
					str1 = _("IDS_POINTS");
					info << str1 << wxT(": ")
						<< pScoring->GetRequiredOpeningPoints();
					if (pScoring->SubtractTimeFaultsFromScore())
					{
						str1 = _("IDS_CONFIGEVENT_TF_FROMSCORE");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1 = _("IDS_CONFIGEVENT_TF_UNDER");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1 = _("IDS_CONFIGEVENT_TF_OVER");
						info << wxT("; ") << str1;
					}
					break;
				case ARBConfigScoring::eTimePlusFaults:
					str1 = _("IDS_CONFIGEVENT_TIMEFAULTMULT");
					info << str1 << wxT(": ")
						<< pScoring->TimeFaultMultiplier();
					if (pScoring->QsMustBeClean())
					{
						// This string is slightly different: Just dropped
						// the 'Time+Fault' at start.
						str1 = _("IDS_CONFIGEVENT_CLEANQ");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsUnder())
					{
						str1 = _("IDS_CONFIGEVENT_TF_UNDER");
						info << wxT("; ") << str1;
					}
					if (pScoring->ComputeTimeFaultsOver())
					{
						str1 = _("IDS_CONFIGEVENT_TF_OVER");
						info << wxT("; ") << str1;
					}
					break;
				}
				if (pScoring->DropFractions())
				{
					str1 = _("IDS_CONFIGEVENT_DROPFRAC");
					info << wxT("; ") << str1;
				}
				if (pScoring->HasBonusPts())
				{
					str1 = _("IDS_CONFIGEVENT_BONUS");
					info << wxT("; ") << str1;
				}
				if (pScoring->HasSuperQ())
				{
					str1 = _("IDS_CONFIGEVENT_SUPERQ");
					info << wxT("; ") << str1;
				}
				if (pScoring->HasSpeedPts())
				{
					str1 = _("IDS_CONFIGEVENT_SPEEDPTS");
					info << wxT("; ") << str1;
					if (0 < pScoring->GetPlaceInfo().size())
					{
						info << wxT(" [");
						int idx = 0;
						for (ARBConfigPlaceInfoList::iterator iter = pScoring->GetPlaceInfo().begin();
							iter != pScoring->GetPlaceInfo().end();
							++idx, ++iter)
						{
							if (0 < idx)
								info << wxT(", ");
							info << (*iter)->GetPlace()
								<< wxT("=")
								<< (*iter)->GetValue();
						}
						info << wxT("]");
					}
				}
				m_ctrlInfo->SetLabel(info.str().c_str());
			}
			// Take care of title points
			FillTitlePoints(pScoring);
			// And the note.
			wxString str = pScoring->GetNote().c_str();
			m_ctrlNote->SetValue(str);
		}
	}
	m_ctrlEdit->Enable(bEnable);
	m_ctrlDelete->Enable(bEnable);
	m_ctrlCopy->Enable(bEnable);
	m_ctrlUp->Enable(bEnable && 1 < m_ctrlMethods->GetCount() && 0 != idxMethod);
	m_ctrlDown->Enable(bEnable && 1 < m_ctrlMethods->GetCount() && static_cast<int>(m_ctrlMethods->GetCount()) - 1 != idxMethod);
	int idxTitle = m_ctrlPointsList->GetSelection();
	m_ctrlPointsList->Enable(bEnable);
	m_ctrlPointsNew->Enable(bEnable);
	m_ctrlPointsEdit->Enable(bEnable && 0 <= idxTitle);
	m_ctrlPointsDelete->Enable(bEnable && 0 <= idxTitle);
	m_ctrlNote->Enable(bEnable);
}


void CDlgConfigEvent::FillMethodList()
{
	m_idxMethod = m_ctrlMethods->GetSelection();
	m_ctrlMethods->Clear();
	wxString str;
	for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
		iter != m_Scorings.end();
		++iter)
	{
		ARBConfigScoringPtr pScoring = (*iter);
		str = GetListName(pScoring);
		int index = m_ctrlMethods->Append(str);
		m_ctrlMethods->SetClientObject(index, new CConfigEventDataScoring(pScoring));
	}
	m_ctrlMethods->SetSelection(m_idxMethod);

	m_ctrlUnused->Clear();
	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		for (ARBConfigLevelList::const_iterator iterLevel = (*iterDiv)->GetLevels().begin();
			iterLevel != (*iterDiv)->GetLevels().end();
			++iterLevel)
		{
			if (!m_Scorings.FindEvent((*iterDiv)->GetName(), (*iterLevel)->GetName(), ARBDate::Today()))
			{
				str = (*iterDiv)->GetName().c_str();
				str += wxT(" / ");
				str += (*iterLevel)->GetName().c_str();
				m_ctrlUnused->Append(str);
			}
			// Remember, configuration doesn't do sublevels.
		}
	}
}


void CDlgConfigEvent::FillTitlePoints(ARBConfigScoringPtr pScoring)
{
	ARBConfigTitlePointsPtr pOldTitle;
	ARBConfigLifetimePointsPtr pOldLifetime;
	ARBConfigPlaceInfoPtr pOldPlacement;
	int idxTitle = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxTitle)
	{
		CConfigEventDataTitlePoints* pTitleData = GetTitleData(idxTitle);
		if (pTitleData)
		{
			pOldTitle = pTitleData->GetData();
		}
		CConfigEventDataLifetimePoints* pLifeData = GetLifetimeData(idxTitle);
		if (pLifeData)
		{
			pOldLifetime = pLifeData->GetData();
		}
		CConfigEventDataPlaceInfo* pPlacement = GetPlacementData(idxTitle);
		if (pPlacement)
		{
			pOldPlacement = pPlacement->GetData();
		}
	}
	m_ctrlPointsList->Clear();
	for (ARBConfigTitlePointsList::const_iterator iter = pScoring->GetTitlePoints().begin();
		iter != pScoring->GetTitlePoints().end();
		++iter)
	{
		ARBConfigTitlePointsPtr pTitle = (*iter);
		int idx = m_ctrlPointsList->Append(pTitle->GetGenericName().c_str());
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataTitlePoints(pTitle));
			if (pOldTitle == pTitle)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	for (ARBConfigLifetimePointsList::const_iterator iter2 = pScoring->GetLifetimePoints().begin();
		iter2 != pScoring->GetLifetimePoints().end();
		++iter2)
	{
		ARBConfigLifetimePointsPtr pLife = (*iter2);
		int idx = m_ctrlPointsList->Append(pLife->GetGenericName().c_str());
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataLifetimePoints(pLife));
			if (pOldLifetime == pLife)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	for (ARBConfigPlaceInfoList::const_iterator iter3 = pScoring->GetPlacements().begin();
		iter3 != pScoring->GetPlacements().end();
		++iter3)
	{
		ARBConfigPlaceInfoPtr pPlace = (*iter3);
		int idx = m_ctrlPointsList->Append(pPlace->GetGenericName().c_str());
		if (wxNOT_FOUND != idx)
		{
			m_ctrlPointsList->SetClientObject(idx, new CConfigEventDataPlaceInfo(pPlace));
			if (pOldPlacement == pPlace)
				m_ctrlPointsList->SetSelection(idx);
		}
	}
	m_ctrlPointsEdit->Enable(false);
	m_ctrlPointsDelete->Enable(false);
}


bool CDlgConfigEvent::SaveControls()
{
	// Save the last selected method.
	if (0 <= m_idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(m_idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		// Point/faults are already up-to-date.
		wxString str;
		m_ctrlNote->SetValue(str);
		str.Trim(true);
		pScoring->SetNote(str.c_str());
	}
	return true;
}


void CDlgConfigEvent::EditMethod()
{
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		CDlgConfigEventMethod dlg(m_pVenue, pScoringData->GetData(), this);
		if (wxID_OK == dlg.ShowModal())
		{
			FillMethodList();
			FillControls();
		}
	}
}


void CDlgConfigEvent::EditPoints()
{
	int idxMethod = m_ctrlMethods->GetSelection();
	int idx = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxMethod && wxNOT_FOUND != idx)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CConfigEventDataTitlePoints* pData1 = GetTitleData(idx);
		CConfigEventDataLifetimePoints* pData2 = GetLifetimeData(idx);
		CConfigEventDataPlaceInfo* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			double value;
			double points;
			CDlgConfigTitlePoints::ETitlePointType type;
			if (pTitle)
			{
				value = pTitle->GetFaults();
				points = pTitle->GetPoints();
				type = CDlgConfigTitlePoints::eTitleNormal;
			}
			else if (pLife)
			{
				value = pLife->GetFaults();
				points = pLife->GetPoints();
				type = CDlgConfigTitlePoints::eTitleLifetime;
			}
			else
			{
				value = pPlace->GetPlace();
				points = pPlace->GetValue();
				type = CDlgConfigTitlePoints::eTitlePlacement;
			}
			CDlgConfigTitlePoints dlg(m_pVenue, value, points, type);
			if (wxID_OK == dlg.ShowModal())
			{
				if (type != dlg.Type()
				|| (pTitle && pTitle->GetFaults() != dlg.Faults())
				|| (pLife && pLife->GetFaults() != dlg.Faults())
				|| (pPlace && pPlace->GetPlace() != dlg.Place()))
				{
					// Clean up.
					if (type == dlg.Type())
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
						else if (pLife)
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
					bool bOk = false;
					switch (dlg.Type())
					{
					default:
						assert(0);
					case CDlgConfigTitlePoints::eTitleNormal:
						bOk = pScoring->GetTitlePoints().AddTitlePoints(dlg.Points(), dlg.Faults());
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
						break;
					case CDlgConfigTitlePoints::eTitleLifetime:
						bOk = pScoring->GetLifetimePoints().AddLifetimePoints(dlg.Points(), dlg.Faults());
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
						break;
					case CDlgConfigTitlePoints::eTitlePlacement:
						bOk = pScoring->GetPlacements().AddPlaceInfo(dlg.Place(), dlg.Points(), true);
						if (!bOk)
							wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
						break;
					}
					if (bOk && type != dlg.Type())
					{
						if (pTitle)
							pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
						else if (pLife)
							pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
						else
							pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
					}
				}
				else
				{
					if (pTitle)
						pTitle->SetPoints(dlg.Points());
					else if (pLife)
						pLife->SetPoints(dlg.Points());
					else
						pPlace->SetValue(dlg.Points());
				}
				FillTitlePoints(pScoring);
				EnablePointsControls();
			}
		}
	}
}


void CDlgConfigEvent::OnClickedSubNames(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	FillSubNames();
	EnableSubnameControls();
	Layout();
	GetSizer()->Fit(this);
}


void CDlgConfigEvent::OnLbnSelchangeSubnames(wxCommandEvent& evt)
{
	EnableSubnameControls();
}


void CDlgConfigEvent::OnLbnDblclkSubnames(wxCommandEvent& evt)
{
}


void CDlgConfigEvent::OnBnClickedSubNamesNew(wxCommandEvent& evt)
{
	CDlgName dlg(wxT(""), this);
	if (wxID_OK == dlg.ShowModal())
	{
		int idx = m_ctrlSubNames->Append(dlg.GetName());
		m_ctrlSubNames->SetSelection(idx);
		EnableSubnameControls();
	}
}


void CDlgConfigEvent::OnBnClickedSubNamesEdit(wxCommandEvent& evt)
{
	int idx = m_ctrlSubNames->GetSelection();
	if (wxNOT_FOUND != idx)
	{
		wxString name = m_ctrlSubNames->GetString(idx);
		CDlgName dlg(name, this);
		if (wxID_OK == dlg.ShowModal())
		{
			m_ctrlSubNames->Delete(idx);
			m_ctrlSubNames->Insert(dlg.GetName(), idx);
			m_ctrlSubNames->SetSelection(idx);
			EnableSubnameControls();
		}
	}
}


void CDlgConfigEvent::OnBnClickedSubNamesDelete(wxCommandEvent& evt)
{
	unsigned int idx = m_ctrlSubNames->GetSelection();
	if (wxNOT_FOUND != idx)
	{
		m_ctrlSubNames->Delete(idx);
		if (idx == m_ctrlSubNames->GetCount())
			--idx;
		m_ctrlSubNames->SetSelection(idx);
		m_ctrlSubNamesEdit->Enable(false);
		m_ctrlSubNamesDelete->Enable(false);
	}
}


void CDlgConfigEvent::OnLbnDblclkMethods(wxCommandEvent& evt)
{
	EditMethod();
}


void CDlgConfigEvent::OnLbnSelchangeMethods(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	SaveControls();
	m_idxMethod = m_ctrlMethods->GetSelection();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedNew(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	SaveControls();
	ARBConfigScoringPtr pScoring = m_Scorings.AddScoring();
	wxString str = GetListName(pScoring);
	m_idxMethod = m_ctrlMethods->Append(str);
	m_ctrlMethods->SetClientObject(m_idxMethod, new CConfigEventDataScoring(pScoring));
	m_ctrlMethods->SetSelection(m_idxMethod);
	FillMethodList();
	FillControls();
}


void CDlgConfigEvent::OnBnClickedEdit(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	EditMethod();
}


void CDlgConfigEvent::OnBnClickedDelete(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		if (m_idxMethod == idxMethod)
		{
			m_idxMethod = -1;
			m_ctrlMethods->SetSelection(-1);
		}
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				m_Scorings.erase(iter);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedCopy(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		ARBConfigScoringPtr pNewScoring = m_Scorings.AddScoring();
		*pNewScoring = *pScoring;
		wxString str = GetListName(pNewScoring);
		m_idxMethod = m_ctrlMethods->Append(str);
		m_ctrlMethods->SetClientObject(m_idxMethod, new CConfigEventDataScoring(pNewScoring));
		m_ctrlMethods->SetSelection(m_idxMethod);
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedUp(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod && 0 != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				iter = m_Scorings.erase(iter);
				--iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods->SetSelection(idxMethod-1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnBnClickedDown(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod && idxMethod < static_cast<int>(m_ctrlMethods->GetCount()) - 1)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		for (ARBConfigScoringList::iterator iter = m_Scorings.begin();
			iter != m_Scorings.end();
			++iter)
		{
			if ((*iter) == pScoring)
			{
				iter = m_Scorings.erase(iter);
				++iter;
				m_Scorings.insert(iter, pScoring);
				m_ctrlMethods->SetSelection(idxMethod+1);
				break;
			}
		}
		FillMethodList();
		FillControls();
	}
}


void CDlgConfigEvent::OnDblclickConfigInfo(wxMouseEvent& evt)
{
	EditMethod();
}


void CDlgConfigEvent::OnSelchangePoints(wxCommandEvent& evt)
{
	EnablePointsControls();
}


void CDlgConfigEvent::OnDblclkPoints(wxCommandEvent& evt)
{
	EditPoints();
}


void CDlgConfigEvent::OnPointsNew(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	if (wxNOT_FOUND != idxMethod)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		if (pScoring)
		{
			CDlgConfigTitlePoints dlg(m_pVenue, 0.0, 0.0, CDlgConfigTitlePoints::eTitleNormal, this);
			if (wxID_OK == dlg.ShowModal())
			{
				// The only reason this fails is if the faults entry exists.
				switch (dlg.Type())
				{
				default:
					assert(0);
				case CDlgConfigTitlePoints::eTitleNormal:
					if (!pScoring->GetTitlePoints().AddTitlePoints(dlg.Points(), dlg.Faults()))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					break;
				case CDlgConfigTitlePoints::eTitleLifetime:
					if (!pScoring->GetLifetimePoints().AddLifetimePoints(dlg.Points(), dlg.Faults()))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					break;
				case CDlgConfigTitlePoints::eTitlePlacement:
					if (!pScoring->GetPlacements().AddPlaceInfo(dlg.Place(), dlg.Points(), true))
						wxMessageBox(_("IDS_TITLEPTS_EXISTS"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_EXCLAMATION);
					break;
				}
				FillTitlePoints(pScoring);
				EnablePointsControls();
			}
		}
	}
}


void CDlgConfigEvent::OnPointsEdit(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	EditPoints();
}


void CDlgConfigEvent::OnPointsDelete(wxCommandEvent& evt)
{
	TransferDataFromWindow();
	int idxMethod = m_ctrlMethods->GetSelection();
	unsigned int idx = m_ctrlPointsList->GetSelection();
	if (wxNOT_FOUND != idxMethod && wxNOT_FOUND != idx)
	{
		CConfigEventDataScoring* pScoringData = GetScoringData(idxMethod);
		ARBConfigScoringPtr pScoring = pScoringData->GetData();
		CConfigEventDataTitlePoints* pData1 = GetTitleData(idx);
		CConfigEventDataLifetimePoints* pData2 = GetLifetimeData(idx);
		CConfigEventDataPlaceInfo* pData3 = GetPlacementData(idx);
		ARBConfigTitlePointsPtr pTitle;
		ARBConfigLifetimePointsPtr pLife;
		ARBConfigPlaceInfoPtr pPlace;
		if (pData1)
			pTitle = pData1->GetData();
		if (pData2)
			pLife = pData2->GetData();
		if (pData3)
			pPlace = pData3->GetData();
		if (pScoring && (pTitle || pLife || pPlace))
		{
			if (pTitle)
				pScoring->GetTitlePoints().DeleteTitlePoints(pTitle->GetFaults());
			else if (pLife)
				pScoring->GetLifetimePoints().DeleteLifetimePoints(pLife->GetFaults());
			else
				pScoring->GetPlacements().DeletePlaceInfo(pPlace->GetPlace());
			m_ctrlPointsList->Delete(idx);
			if (idx == m_ctrlPointsList->GetCount())
				--idx;
			m_ctrlPointsList->SetSelection(idx);
			EnablePointsControls();
		}
	}
}


static ARBDate DateEndPoints(ARBDate from1, ARBDate to1, ARBDate from2, ARBDate to2, bool bFirst)
{
	std::vector<ARBDate> dates;
	if (from1.IsValid())
		dates.push_back(from1);
	if (to1.IsValid())
		dates.push_back(to1);
	if (from2.IsValid())
		dates.push_back(from2);
	if (to2.IsValid())
		dates.push_back(to2);
	ARBDate date;
	if (1 <= dates.size())
	{
		date = dates[0];
		for (size_t n = 1; n < dates.size(); ++n)
		{
			if (bFirst)
			{
				if (dates[n] < date)
					date = dates[n];
			}
			else
			{
				if (dates[n] > date)
					date = dates[n];
			}
		}
	}
	return date;
}


void CDlgConfigEvent::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;
	if (!SaveControls())
		return;

	// Check if there is any overlap.
	bool bOverlap = false;
	{
		// No need to clone, we can just copy the pointers
		ARBConfigScoringList scorings(m_Scorings);
		while (!bOverlap && 1 < scorings.size())
		{
			// Extract all similar methods.
			ARBConfigScoringList items;
			ARBConfigScoringList::iterator iter = scorings.begin();
			ARBConfigScoringPtr pScoring = items.AddScoring();
			*pScoring = *(*iter);
			scorings.erase(scorings.begin());
			iter = scorings.begin();
			while (iter != scorings.end())
			{
				ARBConfigScoringPtr pScoring2 = *iter;
				if (pScoring->GetDivision() == pScoring2->GetDivision()
				&& pScoring->GetLevel() == pScoring2->GetLevel())
				{
					pScoring2 = items.AddScoring();
					*pScoring2 = *(*iter);
					iter = scorings.erase(iter);
				}
				else
					++iter;
			}
			// If we have more than one, check overlap.
			while (!bOverlap && 1 < items.size())
			{
				iter = items.begin();
				ARBConfigScoringPtr pScore = *iter;
				for (++iter; !bOverlap && iter != items.end(); ++iter)
				{
					ARBConfigScoringPtr pScoring2 = *iter;
					ARBDate from1 = pScore->GetValidFrom();
					ARBDate to1 = pScore->GetValidTo();
					ARBDate from2 = pScoring2->GetValidFrom();
					ARBDate to2 = pScoring2->GetValidTo();
					ARBDate low = DateEndPoints(from1, to1, from2, to2, true);
					if (!low.IsValid())
					{
						// If date isn't valid, none of the dates are.
						bOverlap = true;
						break;
					}
					ARBDate hi = DateEndPoints(from1, to1, from2, to2, false);
					--low; // Push valid dates off by 1 so they are not in current range
					++hi;
					if (!from1.IsValid())
						from1 = low;
					if (!to1.IsValid())
						to1 = hi;
					if (!from2.IsValid())
						from2 = low;
					if (!to2.IsValid())
						to2 = hi;
					// Now that we've tweaked the dates, we can do plain old
					// isBetween comparisons. This also means that if both
					// methods have no date set on either the from or to dates,
					// it will be properly flagged (since they are equal)
					if (from1.isBetween(from2, to2) || to1.isBetween(from2, to2))
					{
						bOverlap = true;
					}
				}
				items.erase(items.begin());
			}
		}
	}
	if (bOverlap)
	{
		if (wxYES != wxMessageBox(_("IDS_CONFIGEVENT_OVERLAPDATES"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING | wxYES_NO | wxNO_DEFAULT))
			return;
	}

	ClearFixups();
	if (m_pEvent->GetName() != m_Name)
	{
		if (m_pVenue->GetEvents().FindEvent(m_Name.c_str()))
		{
			wxMessageBox(_("IDS_NAME_IN_USE"), wxMessageBoxCaptionStr, wxCENTRE | wxICON_WARNING);
			m_ctrlName->SetFocus();
			return;
		}
		if (!m_bNewEntry)
			m_DlgFixup.push_back(ARBConfigActionRenameEvent::New(m_pVenue->GetName(), m_pEvent->GetName(), m_Name.c_str()));
		m_pEvent->SetName(m_Name.c_str());
	}
	// m_Book is only valid when editing an existing entry.
	/*
	if (!m_bNewEntry)
	{
		switch (CDlgConfigure::CheckExistingRuns(m_pDoc, m_Book->GetDogs(),
			m_pVenue, m_pEvent->GetName(), m_Scorings, m_DlgFixup))
		{
		case CDlgConfigure::eCancelChanges:
			EndDialog(IDCANCEL);
			return;
		default:
		case CDlgConfigure::eDoNotDoIt:
			return;
		case CDlgConfigure::eNoChange:
		case CDlgConfigure::eDoIt:
			break;
		}
	}
	*/

	m_pEvent->SetDesc(m_Desc.c_str());
	m_pEvent->SetHasTable(m_bHasTable);
	m_pEvent->SetHasPartner(m_bHasPartners);
	m_pEvent->SetHasSubNames(m_bHasSubNames);
	if (m_bHasSubNames)
	{
		std::set<tstring> subNames;
		int nCount = m_ctrlSubNames->GetCount();
		for (int i = 0; i < nCount; ++i)
		{
			wxString str = m_ctrlSubNames->GetString(i);
			str.Trim(true);
			str.Trim(false);
			if (!str.IsEmpty())
				subNames.insert(str.c_str());
		}
		m_pEvent->SetSubNames(subNames);
	}
	// No need to clone them, just move them.
	m_pEvent->GetScorings() = m_Scorings;
	EndDialog(wxID_OK);
}
