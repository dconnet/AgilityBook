/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * 2016-11-29 Fix sizer assertions.
 * 2014-12-31 Changed pixels to dialog units.
 * 2011-12-22 Switch to using Bind on wx2.9+.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-07-30 Created.
 */

#include "stdafx.h"
#include "DlgEventSelect.h"

#include "AgilityBook.h"

#include "ARB/ARBConfigDivision.h"
#include "ARB/ARBConfigEvent.h"
#include "ARB/ARBConfigLevel.h"
#include "ARB/ARBConfigSubLevel.h"
#include "ARB/ARBConfigVenue.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/ARBWinUtilities.h"
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class CDlgDivSelectData : public wxClientData
{
public:
	CDlgDivSelectData(ARBConfigDivisionPtr const& inDiv)
		: m_pDiv(inDiv)
	{
	}
	ARBConfigDivisionPtr m_pDiv;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgEventSelectData : public wxClientData
{
public:
	CDlgEventSelectData(ARBConfigLevelPtr const& inLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel()
	{
	}
	CDlgEventSelectData(ARBConfigLevelPtr const& inLevel, ARBConfigSubLevelPtr const& inSubLevel)
		: m_pLevel(inLevel)
		, m_pSubLevel(inSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////

wxBEGIN_EVENT_TABLE(CDlgEventSelect, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgEventSelect::OnOk)
wxEND_EVENT_TABLE()


CDlgEventSelect::CDlgEventSelect(
	ARBConfigVenuePtr const& inVenue,
	ARBDate const& inDate,
	std::wstring const& inDivision,
	std::wstring const& inLevel,
	std::wstring const& inEvent,
	wxWindow* pParent)
	: wxDialog()
	, m_ctrlDivisions(nullptr)
	, m_ctrlLevels(nullptr)
	, m_ctrlEvents(nullptr)
	, m_ctrlOk(nullptr)
	, m_Division()
	, m_Level()
	, m_Event()
	, m_inLevel(StringUtil::stringWX(inLevel))
	, m_inEvent(StringUtil::stringWX(inEvent))
	, m_pVenue(inVenue)
	, m_Date(inDate)
{
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_EVENT_SELECT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* textDiv
		= new wxStaticText(this, wxID_ANY, _("IDC_EVENT_SELECT_DIVISION"), wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivisions = new wxComboBox(
		this,
		wxID_ANY,
		m_Division,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_Division));
	m_ctrlDivisions->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgEventSelect::OnSelchangeDivision, this);
	m_ctrlDivisions->SetHelpText(_("HIDC_EVENT_SELECT_DIVISION"));
	m_ctrlDivisions->SetToolTip(_("HIDC_EVENT_SELECT_DIVISION"));

	wxStaticText* textLevel
		= new wxStaticText(this, wxID_ANY, _("IDC_EVENT_SELECT_LEVEL"), wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(
		this,
		wxID_ANY,
		m_Level,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_Level));
	m_ctrlLevels->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgEventSelect::OnSelchangeLevel, this);
	m_ctrlLevels->SetHelpText(_("HIDC_EVENT_SELECT_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_EVENT_SELECT_LEVEL"));

	wxStaticText* textEvent
		= new wxStaticText(this, wxID_ANY, _("IDC_EVENT_SELECT_EVENT"), wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(
		this,
		wxID_ANY,
		m_Event,
		wxDefaultPosition,
		wxDefaultSize,
		0,
		nullptr,
		wxCB_DROPDOWN | wxCB_READONLY,
		wxGenericValidator(&m_Event));
	m_ctrlEvents->Bind(wxEVT_COMMAND_COMBOBOX_SELECTED, &CDlgEventSelect::OnSelchangeEvent, this);
	m_ctrlEvents->SetHelpText(_("HIDC_EVENT_SELECT_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_EVENT_SELECT_EVENT"));

	// Sizers

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizerDiv = new wxBoxSizer(wxHORIZONTAL);
	sizerDiv->Add(textDiv, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerDiv->Add(m_ctrlDivisions, 1, wxEXPAND);

	bSizer->Add(sizerDiv, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerLevel = new wxBoxSizer(wxHORIZONTAL);
	sizerLevel->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerLevel->Add(m_ctrlLevels, 1, wxEXPAND);

	bSizer->Add(sizerLevel, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, wxDLG_UNIT_X(this, 5));

	wxBoxSizer* sizerEvent = new wxBoxSizer(wxHORIZONTAL);
	sizerEvent->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, wxDLG_UNIT_X(this, 3));
	sizerEvent->Add(m_ctrlEvents, 1, wxEXPAND);

	bSizer->Add(sizerEvent, 1, wxEXPAND | wxALL, wxDLG_UNIT_X(this, 5));

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, wxDLG_UNIT_X(this, 3));
	m_ctrlOk = wxDynamicCast(FindWindowInSizer(bSizer, wxID_OK), wxButton);
	assert(!!m_ctrlOk);

	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	std::vector<wxClientData*> data;
	for (auto const& pDiv : m_pVenue->GetDivisions())
	{
		items.Add(pDiv->GetName());
		data.push_back(new CDlgDivSelectData(pDiv));
		if (pDiv->GetName() == inDivision)
			idxCur = static_cast<int>(items.size()) - 1;
	}
	m_ctrlDivisions->Append(items, data.data());
	m_ctrlDivisions->SetSelection(idxCur);
	FillLevels();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, sz);
	CenterOnParent();

	m_ctrlOk->SetFocus();
}


std::wstring CDlgEventSelect::GetDivision() const
{
	return StringUtil::stringW(m_Division);
}


std::wstring CDlgEventSelect::GetLevel() const
{
	return StringUtil::stringW(m_Level);
}


std::wstring CDlgEventSelect::GetEvent() const
{
	return StringUtil::stringW(m_Event);
}


void CDlgEventSelect::UpdateControls()
{
	if (m_ctrlOk)
	{
		bool bEnable = false;
		if (wxNOT_FOUND != m_ctrlDivisions->GetSelection() && wxNOT_FOUND != m_ctrlLevels->GetSelection()
			&& wxNOT_FOUND != m_ctrlEvents->GetSelection())
		{
			bEnable = true;
		}
		m_ctrlOk->Enable(bEnable);
	}
}


void CDlgEventSelect::FillLevels()
{
	std::wstring level;
	if (wxNOT_FOUND != m_ctrlLevels->GetSelection())
	{
		TransferDataFromWindow();
		level = m_Level;
	}
	if (!m_inLevel.empty())
	{
		level = m_inLevel;
		m_inLevel.clear();
	}
	m_ctrlLevels->Clear();

	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	std::vector<wxClientData*> data;
	int index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		CDlgDivSelectData* pData = dynamic_cast<CDlgDivSelectData*>(m_ctrlDivisions->GetClientObject(index));
		for (auto const& pLevel : pData->m_pDiv->GetLevels())
		{
			if (0 < pLevel->GetSubLevels().size())
			{
				for (auto const& pSubLevel : pLevel->GetSubLevels())
				{
					items.Add(pSubLevel->GetName());
					data.push_back(new CDlgEventSelectData(pLevel, pSubLevel));
					if (level == pSubLevel->GetName())
						idxCur = static_cast<int>(items.size()) - 1;
				}
			}
			else
			{
				items.Add(pLevel->GetName());
				data.push_back(new CDlgEventSelectData(pLevel));
				if (level == pLevel->GetName())
					idxCur = static_cast<int>(items.size()) - 1;
			}
		}
	}
	m_ctrlLevels->Append(items, data.data());
	m_ctrlLevels->SetSelection(idxCur);
	FillEvents();
}


void CDlgEventSelect::FillEvents()
{
	std::wstring evt;
	if (wxNOT_FOUND != m_ctrlEvents->GetSelection())
	{
		TransferDataFromWindow();
		evt = m_Event;
	}
	if (!m_inEvent.empty())
	{
		evt = m_inEvent;
		m_inEvent.clear();
	}
	m_ctrlEvents->Clear();

	int idxCur = wxNOT_FOUND;
	wxArrayString items;
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
	{
		CDlgDivSelectData* pData = dynamic_cast<CDlgDivSelectData*>(m_ctrlDivisions->GetClientObject(idxDiv));
		int idxLevel = m_ctrlLevels->GetSelection();
		if (wxNOT_FOUND != idxLevel)
		{
			CDlgEventSelectData* pEvtData = dynamic_cast<CDlgEventSelectData*>(m_ctrlLevels->GetClientObject(idxLevel));
			for (auto const& pEvent : m_pVenue->GetEvents())
			{
				if (pEvent->FindEvent(pData->m_pDiv->GetName(), pEvtData->m_pLevel->GetName(), m_Date))
				{
					items.Add(pEvent->GetName());
					if (evt == pEvent->GetName())
						idxCur = static_cast<int>(items.size()) - 1;
				}
			}
		}
	}
	m_ctrlEvents->Append(items);
	m_ctrlEvents->SetSelection(idxCur);
	UpdateControls();
}


void CDlgEventSelect::OnSelchangeDivision(wxCommandEvent& evt)
{
	FillLevels();
}


void CDlgEventSelect::OnSelchangeLevel(wxCommandEvent& evt)
{
	FillEvents();
}


void CDlgEventSelect::OnSelchangeEvent(wxCommandEvent& evt)
{
	UpdateControls();
}


void CDlgEventSelect::OnOk(wxCommandEvent& evt)
{
	if (!Validate() || !TransferDataFromWindow())
		return;

	if (m_Division.IsEmpty() || m_Level.IsEmpty() || m_Event.IsEmpty())
		return;

	EndDialog(wxID_OK);
}

} // namespace dconSoft
