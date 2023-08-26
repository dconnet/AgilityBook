/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgRun class
 * @author David Connet
 *
 * Revision History
 * 2021-01-07 Separated into multiple files.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-05-04 Added ability to suppress metafile.
 * 2004-04-27 Added some error recovery.
 */

#include "stdafx.h"
#include "DlgRunPanelCRCD.h"

#include "AgilityBookOptions.h"
#include "ClipBoard.h"
#include "DlgCRCDViewer.h"

#include "ARB/ARBDogRun.h"
#include "ARBCommon/StringUtil.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Widgets.h"
#include <wx/dcbuffer.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

#ifdef __WXMSW__
// You can currently only enable this on Win32. I directly use the win32
// apis to play the metafile into a dc. In theory, I could use wxWidgets
// to play them - but I still need the direct win32 access to copy a
// metafile from the clipboard - wxWidgets only has support to copy >to<
// the clipboard.
#define HAS_ENHMETAFILE
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

class CMetaDataDisplay : public CTextCtrl
{
	DECLARE_NO_COPY_IMPLEMENTED(CMetaDataDisplay)
public:
	CMetaDataDisplay(wxWindow* parent, ARBDogRunPtr const& inRun);
	~CMetaDataDisplay();

	bool HasMetafileSupport() const
	{
#ifdef HAS_ENHMETAFILE
		return true;
#else
		return false;
#endif
	}
	bool HasMetafile()
	{
#ifdef HAS_ENHMETAFILE
		return !!m_metaFile;
#else
		return false;
#endif
	}
	bool ViewText() const
	{
		return m_ViewText;
	}
	bool HasCourse() const
	{
		return !m_Insert;
	}
	void SetCRCDData();
	void ToggleView();
	void OnCopy();
	void OnCRCDImage(bool checked);
	void Display();

private:
	void DeleteMetaFile();

	ARBDogRunPtr m_Run;
#ifdef HAS_ENHMETAFILE
	HENHMETAFILE m_metaFile;
	HENHMETAFILE m_metaFileBack;
#endif
	bool m_ViewText;
	bool m_Insert;

	void OnPaint(wxPaintEvent& evt);
};


CMetaDataDisplay::CMetaDataDisplay(wxWindow* parent, ARBDogRunPtr const& inRun)
	: m_Run(inRun)
#ifdef HAS_ENHMETAFILE
	, m_metaFile(nullptr)
	, m_metaFileBack(nullptr)
#endif
	, m_ViewText(true)
	, m_Insert(inRun->GetCRCD().empty())
{
	CTextCtrl::Create(
		parent,
		wxID_ANY,
		StringUtil::stringWX(m_Run->GetCRCD()),
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_MULTILINE | wxTE_READONLY);
	SetBackgroundStyle(wxBG_STYLE_PAINT);
	SetAllowReadonlyTabstop();

#ifdef HAS_ENHMETAFILE
	if (0 < m_Run->GetCRCDRawMetaData().length())
	{
		ARBMetaDataPtr metaData = m_Run->GetCRCDMetaData();
		m_metaFile = SetEnhMetaFileBits(static_cast<UINT>(metaData->data().size()), metaData->data().data());
		m_metaFileBack = m_metaFile;
		if (m_metaFile)
		{
			m_ViewText = false;
			Enable(m_ViewText);
		}
	}
#endif

	Bind(wxEVT_PAINT, &CMetaDataDisplay::OnPaint, this);
}


CMetaDataDisplay::~CMetaDataDisplay()
{
	DeleteMetaFile();
}


void CMetaDataDisplay::DeleteMetaFile()
{
#ifdef HAS_ENHMETAFILE
	if (!m_metaFile && m_metaFileBack)
		m_metaFile = m_metaFileBack;
	if (m_metaFile)
	{
		DeleteEnhMetaFile(m_metaFile);
		m_metaFile = nullptr;
		m_metaFileBack = nullptr;
	}
#endif
}


void CMetaDataDisplay::SetCRCDData()
{
	std::vector<unsigned char> bits;
#ifdef HAS_ENHMETAFILE
	if (m_metaFile)
	{
		ENHMETAHEADER header;
		GetEnhMetaFileHeader(m_metaFile, sizeof(header), &header);
		UINT nSize = GetEnhMetaFileBits(m_metaFile, 0, nullptr);
		bits = std::vector<unsigned char>(nSize + 1);
		GetEnhMetaFileBits(m_metaFile, nSize, bits.data());
		assert(sizeof(BYTE) == sizeof(char));
		m_Run->SetCRCDMetaData(bits);
	}
	else
#endif
		m_Run->SetCRCDMetaData(bits);
}


void CMetaDataDisplay::ToggleView()
{
	m_ViewText = !m_ViewText;
	Enable(m_ViewText);
	Refresh();
}


void CMetaDataDisplay::OnCopy()
{
	if (m_Insert)
	{
		bool bText = false;
		if (wxClipboard::Get()->Open())
		{
			bText = wxClipboard::Get()->IsSupported(wxDF_TEXT);
			wxClipboard::Get()->Close();
		}
		bool bMeta = false;
#ifdef HAS_ENHMETAFILE
		bMeta = ::IsClipboardFormatAvailable(CF_ENHMETAFILE) ? true : false;
		if (!CAgilityBookOptions::GetIncludeCRCDImage())
			bMeta = FALSE;
#endif
		if (bText || bMeta)
		{
			CClipboardDataReader clpData;
			if (clpData.Open())
			{
				SetLabel(L"");
				DeleteMetaFile();
				if (bText)
				{
					m_ViewText = true;
					std::wstring str;
					clpData.GetData(str);
					str = StringUtil::Trim(str);
					// We do the replace since CRCD3 has "\n\nhdrs\r\netc"
					// Standardize to \n.
					str = StringUtil::Replace(str, L"\r\n", L"\n");
					m_Run->SetCRCD(str);
					SetLabel(StringUtil::stringWX(str));
					if (0 < str.length())
						m_Insert = false;
					// Only create the metafile if we pasted text. Otherwise
					// we could end up with a non-CRCD metafile. It's still
					// possible this may not be CRCD data - the user can
					// just clear it then.
					clpData.Close();
#ifdef HAS_ENHMETAFILE
					if (bMeta)
					{
						// Note, wxClipboard may be using the OLE clipboard
						// which doesn't actually open the windows one.
						wxClipboard::Get()->Open();
						HENHMETAFILE hData = static_cast<HENHMETAFILE>(GetClipboardData(CF_ENHMETAFILE));
						m_metaFile = CopyEnhMetaFile(hData, nullptr);
						m_metaFileBack = m_metaFile;
						SetCRCDData();
						if (m_metaFile)
						{
							m_ViewText = false;
							m_Insert = false;
						}
						wxClipboard::Get()->Close();
					}
#endif
				}
			}
		}
	}
	else
	{
		m_Insert = true;
		m_ViewText = true;
		SetLabel(L"");
		DeleteMetaFile();
		m_Run->SetCRCD(L"");
		m_Run->SetCRCDMetaData(std::vector<unsigned char>());
	}
	Enable(m_ViewText);
	Refresh();
}


void CMetaDataDisplay::OnCRCDImage(bool checked)
{
#ifdef HAS_ENHMETAFILE
	CAgilityBookOptions::SetIncludeCRCDImage(checked);
	if (checked)
	{
		if (!m_metaFile && m_metaFileBack)
			m_metaFile = m_metaFileBack;
	}
	else
	{
		if (m_metaFile)
		{
			m_metaFileBack = m_metaFile;
			m_metaFile = nullptr;
		}
	}
	SetCRCDData();
	if (m_metaFile)
	{
		m_ViewText = false;
		m_Insert = false;
	}
	else
		m_ViewText = true;
	Enable(m_ViewText);
	Refresh();
#endif
}


void CMetaDataDisplay::Display()
{
#ifdef HAS_ENHMETAFILE
	if (m_metaFile)
	{
		CDlgCRCDViewer viewer(m_metaFile, this);
		viewer.ShowModal();
	}
#endif
}


void CMetaDataDisplay::OnPaint(wxPaintEvent& evt)
{
	if (m_ViewText)
		evt.Skip(true);
	else
	{
		wxAutoBufferedPaintDC dc(this);
		dc.SetBackground(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
		dc.Clear();
#ifdef HAS_ENHMETAFILE
		wxSize sz = GetClientSize();
		RECT r;
		r.left = 0;
		r.top = 0;
		r.right = std::min(sz.x, sz.y);
		r.bottom = std::min(sz.x, sz.y);
		WXHDC hdc = dc.GetHDC();
		PlayEnhMetaFile(hdc, m_metaFile, &r);
#endif
	}
}

/////////////////////////////////////////////////////////////////////////////

CDlgRunPanelCRCD::CDlgRunPanelCRCD(
	CDlgRun* pDlg,
	CAgilityBookDoc* pDoc,
	ARBDogPtr const& inDog,
	ARBDogTrialPtr const& inTrial,
	ARBDogRunPtr const& inRun,
	wxWindow* parent)
	: CDlgRunPanelBase(pDlg, pDoc, inDog, inTrial, inRun, parent)
	, m_CRCDDisplay(nullptr)
	, m_ctrlCourse(nullptr)
	, m_ctrlCRCDView(nullptr)
	, m_ctrlCRCDCopy(nullptr)
	, m_ctrlIncImage(nullptr)
{
	// Controls (these are done first to control tab order)

	wxStaticText* textCRCD = new wxStaticText(this, wxID_ANY, _("IDC_RUN_CRCD"), wxDefaultPosition, wxDefaultSize, 0);
	textCRCD->Wrap(wxDLG_UNIT_X(this, 300));

	m_CRCDDisplay = new CMetaDataDisplay(this, m_Run);

	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlCourse = new wxButton(this, wxID_ANY, _("IDC_RUNCRCD_EDIT"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCourse->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelCRCD::OnCRCDEdit, this);
		m_ctrlCourse->SetHelpText(_("HIDC_RUNCRCD_EDIT"));
		m_ctrlCourse->SetToolTip(_("HIDC_RUNCRCD_EDIT"));

		m_ctrlCRCDView = new wxButton(this, wxID_ANY, _("IDC_RUNCRCD_VIEW"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlCRCDView->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelCRCD::OnCRCDView, this);
		m_ctrlCRCDView->SetHelpText(_("HIDC_RUNCRCD_VIEW"));
		m_ctrlCRCDView->SetToolTip(_("HIDC_RUNCRCD_VIEW"));
	}

	m_ctrlCRCDCopy = new wxButton(this, wxID_ANY, _("IDC_RUNCRCD_COPY"), wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlCRCDCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgRunPanelCRCD::OnCRCDCopy, this);
	m_ctrlCRCDCopy->SetHelpText(_("HIDC_RUNCRCD_COPY"));
	m_ctrlCRCDCopy->SetToolTip(_("HIDC_RUNCRCD_COPY"));

	wxStaticText* textImageDesc = nullptr;
	if (m_CRCDDisplay->HasMetafileSupport())
	{
		m_ctrlIncImage = new wxCheckBox(this, wxID_ANY, _("IDC_RUNCRCD_IMAGE"), wxDefaultPosition, wxDefaultSize, 0);
		m_ctrlIncImage->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &CDlgRunPanelCRCD::OnCRCDImage, this);
		m_ctrlIncImage->SetHelpText(_("HIDC_RUNCRCD_IMAGE"));
		m_ctrlIncImage->SetToolTip(_("HIDC_RUNCRCD_IMAGE"));
		bool setCheck = CAgilityBookOptions::GetIncludeCRCDImage();
		if (m_CRCDDisplay->HasMetafile())
			setCheck = true;
		m_ctrlIncImage->SetValue(setCheck);

		textImageDesc
			= new wxStaticText(this, wxID_ANY, _("IDC_RUNCRCD_IMAGE_TEXT"), wxDefaultPosition, wxDefaultSize, 0);
		textImageDesc->Wrap(wxDLG_UNIT_X(this, 50));
	}

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* sizerCRCD = new wxBoxSizer(wxHORIZONTAL);

	wxBoxSizer* sizerDisplay = new wxBoxSizer(wxVERTICAL);
	sizerDisplay->Add(textCRCD, 0, wxEXPAND, 0);
	sizerDisplay->Add(m_CRCDDisplay, 1, wxEXPAND | wxTOP, padding.Controls());

	sizerCRCD->Add(sizerDisplay, 1, wxEXPAND | wxALL, padding.Controls());

	wxBoxSizer* sizerBtnsCRCD = new wxBoxSizer(wxVERTICAL);
	if (m_ctrlCourse)
		sizerBtnsCRCD->Add(m_ctrlCourse, 0, wxBOTTOM, padding.Inner());
	if (m_ctrlCRCDView)
		sizerBtnsCRCD->Add(m_ctrlCRCDView, 0, wxBOTTOM, padding.Inner());
	sizerBtnsCRCD->Add(m_ctrlCRCDCopy, 0, wxBOTTOM, padding.Inner());
	if (m_ctrlIncImage)
		sizerBtnsCRCD->Add(m_ctrlIncImage, 0, wxBOTTOM, padding.Inner());
	if (textImageDesc)
		sizerBtnsCRCD->Add(textImageDesc, 0, wxBOTTOM, padding.Inner());

	sizerCRCD->Add(sizerBtnsCRCD, 0, wxEXPAND | wxRIGHT | wxTOP | wxBOTTOM, padding.Controls());

	SetSizer(sizerCRCD);
	Layout();
	sizerCRCD->Fit(this);

	UpdateCRCDButtons();
}

wxWindow* CDlgRunPanelCRCD::GetInitialControl()
{
	return m_CRCDDisplay;
}


bool CDlgRunPanelCRCD::Validate()
{
	return true;
}


bool CDlgRunPanelCRCD::Save()
{
	return true;
}


void CDlgRunPanelCRCD::UpdateCRCDButtons()
{
	if (m_ctrlCRCDView)
	{
		if (m_CRCDDisplay->ViewText())
			m_ctrlCRCDView->SetLabel(_("IDS_CRCD_BTN_VIEWIMAGE"));
		else
			m_ctrlCRCDView->SetLabel(_("IDS_CRCD_BTN_VIEWTEXT"));
	}
	if (m_CRCDDisplay->HasCourse())
		m_ctrlCRCDCopy->SetLabel(_("IDS_CRCD_BTN_CLEARCOURSE"));
	else
		m_ctrlCRCDCopy->SetLabel(_("IDS_CRCD_BTN_INSERTCOURSE"));

	if (m_ctrlCourse)
		m_ctrlCourse->Enable(m_CRCDDisplay->HasMetafile());
	if (m_ctrlCRCDView)
		m_ctrlCRCDView->Enable(m_CRCDDisplay->HasMetafile());
}

void CDlgRunPanelCRCD::OnCRCDEdit(wxCommandEvent& evt)
{
	m_CRCDDisplay->Display();
}


void CDlgRunPanelCRCD::OnCRCDView(wxCommandEvent& evt)
{
	m_CRCDDisplay->ToggleView();
	UpdateCRCDButtons();
}


void CDlgRunPanelCRCD::OnCRCDCopy(wxCommandEvent& evt)
{
	m_CRCDDisplay->OnCopy();
	UpdateCRCDButtons();
}


void CDlgRunPanelCRCD::OnCRCDImage(wxCommandEvent& evt)
{
	m_CRCDDisplay->OnCRCDImage(m_ctrlIncImage->GetValue());
	UpdateCRCDButtons();
}

} // namespace dconSoft
