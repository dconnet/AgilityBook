/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CDlgEventSelect class
 * @author David Connet
 *
 * Revision History
 * @li 2011-12-22 DRC Switch to using Bind on wx2.9+.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-02-11 DRC Ported to wxWidgets.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-07-30 DRC Created.
 */

#include "stdafx.h"
#include "DlgEventSelect.h"

#include "AgilityBook.h"
#include "ARBConfigDivision.h"
#include "ARBConfigEvent.h"
#include "ARBConfigLevel.h"
#include "ARBConfigSubLevel.h"
#include "ARBConfigVenue.h"
#include "Globals.h"
#include <wx/valgen.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

class CDlgDivSelectData : public wxClientData
{
public:
	CDlgDivSelectData(ARBConfigDivisionPtr pDiv)
		: m_pDiv(pDiv)
	{
	}
	ARBConfigDivisionPtr m_pDiv;
};

/////////////////////////////////////////////////////////////////////////////

class CDlgEventSelectData : public wxClientData
{
public:
	CDlgEventSelectData(ARBConfigLevelPtr pLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel()
	{
	}
	CDlgEventSelectData(
			ARBConfigLevelPtr pLevel,
			ARBConfigSubLevelPtr pSubLevel)
		: m_pLevel(pLevel)
		, m_pSubLevel(pSubLevel)
	{
	}
	ARBConfigLevelPtr m_pLevel;
	ARBConfigSubLevelPtr m_pSubLevel;
};

/////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(CDlgEventSelect, wxDialog)
	EVT_BUTTON(wxID_OK, CDlgEventSelect::OnOk)
END_EVENT_TABLE()


CDlgEventSelect::CDlgEventSelect(
		ARBConfigVenuePtr inVenue,
		ARBDate const& inDate,
		wxString const& inDivision,
		wxString const& inLevel,
		wxString const& inEvent,
		wxWindow* pParent)
	: wxDialog()
	, m_ctrlDivisions(NULL)
	, m_ctrlLevels(NULL)
	, m_ctrlEvents(NULL)
	, m_ctrlOk(NULL)
	, m_Division()
	, m_Level()
	, m_Event()
	, m_inLevel(inLevel)
	, m_inEvent(inEvent)
	, m_pVenue(inVenue)
	, m_Date(inDate)
{
	SetExtraStyle(wxDIALOG_EX_CONTEXTHELP | GetExtraStyle());
	if (!pParent)
		pParent = wxGetApp().GetTopWindow();
	Create(pParent, wxID_ANY, _("IDD_EVENT_SELECT"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE);

	// Controls (these are done first to control tab order)

	wxStaticText* textDiv = new wxStaticText(this, wxID_ANY,
		_("IDC_EVENT_SELECT_DIVISION"),
		wxDefaultPosition, wxDefaultSize, 0);
	textDiv->Wrap(-1);

	m_ctrlDivisions = new wxComboBox(this, wxID_ANY, m_Division,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_Division));
	BIND_OR_CONNECT_CTRL(m_ctrlDivisions, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgEventSelect::OnSelchangeDivision);
	m_ctrlDivisions->SetHelpText(_("HIDC_EVENT_SELECT_DIVISION"));
	m_ctrlDivisions->SetToolTip(_("HIDC_EVENT_SELECT_DIVISION"));

	wxStaticText* textLevel = new wxStaticText(this, wxID_ANY,
		_("IDC_EVENT_SELECT_LEVEL"),
		wxDefaultPosition, wxDefaultSize, 0);
	textLevel->Wrap(-1);

	m_ctrlLevels = new wxComboBox(this, wxID_ANY, m_Level,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_Level));
	BIND_OR_CONNECT_CTRL(m_ctrlLevels, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgEventSelect::OnSelchangeLevel);
	m_ctrlLevels->SetHelpText(_("HIDC_EVENT_SELECT_LEVEL"));
	m_ctrlLevels->SetToolTip(_("HIDC_EVENT_SELECT_LEVEL"));

	wxStaticText* textEvent = new wxStaticText(this, wxID_ANY, _("IDC_EVENT_SELECT_EVENT"), wxDefaultPosition, wxDefaultSize, 0);
	textEvent->Wrap(-1);

	m_ctrlEvents = new wxComboBox(this, wxID_ANY, m_Event,
		wxDefaultPosition, wxDefaultSize,
		0, NULL,
		wxCB_DROPDOWN|wxCB_READONLY,
		wxGenericValidator(&m_Event));
	BIND_OR_CONNECT_CTRL(m_ctrlEvents, wxEVT_COMMAND_COMBOBOX_SELECTED, wxCommandEventHandler, CDlgEventSelect::OnSelchangeEvent);
	m_ctrlEvents->SetHelpText(_("HIDC_EVENT_SELECT_EVENT"));
	m_ctrlEvents->SetToolTip(_("HIDC_EVENT_SELECT_EVENT"));

	// Sizers (sizer creation is in same order as wxFormBuilder)

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizer1 = new wxBoxSizer(wxHORIZONTAL);
	sizer1->Add(textDiv, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer1->Add(m_ctrlDivisions, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer1, 0, wxEXPAND, 5);

	wxBoxSizer* sizer2 = new wxBoxSizer(wxHORIZONTAL);
	sizer2->Add(textLevel, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer2->Add(m_ctrlLevels, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer2, 0, wxEXPAND, 5);

	wxBoxSizer* sizer3 = new wxBoxSizer(wxHORIZONTAL);
	sizer3->Add(textEvent, 0, wxALIGN_CENTER_VERTICAL|wxALL, 5);
	sizer3->Add(m_ctrlEvents, 1, wxALIGN_CENTER_VERTICAL|wxALL|wxEXPAND, 5);

	bSizer->Add(sizer3, 1, wxEXPAND, 5);

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK|wxCANCEL);
	bSizer->Add(sdbSizer, 0, wxALL|wxEXPAND, 5);
	m_ctrlOk = wxDynamicCast(FindWindowInSizer(bSizer, wxID_OK), wxButton);
	assert(m_ctrlOk != NULL);

	for (ARBConfigDivisionList::const_iterator iterDiv = m_pVenue->GetDivisions().begin();
		iterDiv != m_pVenue->GetDivisions().end();
		++iterDiv)
	{
		ARBConfigDivisionPtr pDiv = (*iterDiv);
		int index = m_ctrlDivisions->Append(pDiv->GetName());
		m_ctrlDivisions->SetClientObject(index,
			new CDlgDivSelectData(pDiv));
		if (pDiv->GetName() == inDivision)
			m_ctrlDivisions->SetSelection(index);
	}
	FillLevels();

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz(GetSize());
	SetSizeHints(sz, sz);
	CenterOnParent();

	m_ctrlOk->SetFocus();
}


void CDlgEventSelect::UpdateControls()
{
	if (m_ctrlOk)
	{
		bool bEnable = false;
		if (wxNOT_FOUND != m_ctrlDivisions->GetSelection()
		&& wxNOT_FOUND != m_ctrlLevels->GetSelection()
		&& wxNOT_FOUND != m_ctrlEvents->GetSelection())
		{
			bEnable = true;
		}
		m_ctrlOk->Enable(bEnable);
	}
}


void CDlgEventSelect::FillLevels()
{
	wxString level;
	int index = m_ctrlLevels->GetSelection();
	if (wxNOT_FOUND != index)
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
	index = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != index)
	{
		CDlgDivSelectData* pData = dynamic_cast<CDlgDivSelectData*>(m_ctrlDivisions->GetClientObject(index));
		for (ARBConfigLevelList::const_iterator iter = pData->m_pDiv->GetLevels().begin();
			iter != pData->m_pDiv->GetLevels().end();
			++iter)
		{
			ARBConfigLevelPtr pLevel = (*iter);
			if (0 < pLevel->GetSubLevels().size())
			{
				for (ARBConfigSubLevelList::const_iterator iterSub = pLevel->GetSubLevels().begin();
					iterSub != pLevel->GetSubLevels().end();
					++iterSub)
				{
					ARBConfigSubLevelPtr pSubLevel = (*iterSub);
					int idx = m_ctrlLevels->Append(pSubLevel->GetName());
					m_ctrlLevels->SetClientObject(idx,
						new CDlgEventSelectData(pLevel, pSubLevel));
					if (level == pSubLevel->GetName())
						m_ctrlLevels->SetSelection(idx);
				}
			}
			else
			{
				int idx = m_ctrlLevels->Append(pLevel->GetName());
				m_ctrlLevels->SetClientObject(idx,
					new CDlgEventSelectData(pLevel));
				if (level == pLevel->GetName())
					m_ctrlLevels->SetSelection(idx);
			}
		}
	}
	FillEvents();
}


void CDlgEventSelect::FillEvents()
{
	wxString evt;
	int index = m_ctrlEvents->GetSelection();
	if (wxNOT_FOUND != index)
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
	int idxDiv = m_ctrlDivisions->GetSelection();
	if (wxNOT_FOUND != idxDiv)
	{
		CDlgDivSelectData* pData = dynamic_cast<CDlgDivSelectData*>(m_ctrlDivisions->GetClientObject(idxDiv));
		int idxLevel = m_ctrlLevels->GetSelection();
		if (wxNOT_FOUND != idxLevel)
		{
			CDlgEventSelectData* pEvtData = dynamic_cast<CDlgEventSelectData*>(m_ctrlLevels->GetClientObject(idxLevel));
			for (ARBConfigEventList::const_iterator iter = m_pVenue->GetEvents().begin();
				iter != m_pVenue->GetEvents().end();
				++iter)
			{
				ARBConfigEventPtr pEvent = (*iter);
				if (pEvent->FindEvent(pData->m_pDiv->GetName(), pEvtData->m_pLevel->GetName(), m_Date))
				{
					int idx = m_ctrlEvents->Append(pEvent->GetName());
					if (evt == pEvent->GetName())
					{
						m_ctrlEvents->SetSelection(idx);
					}
				}
			}
		}
	}
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

	if (m_Division.IsEmpty()
	|| m_Level.IsEmpty()
	|| m_Event.IsEmpty())
		return;

	EndDialog(wxID_OK);
}
