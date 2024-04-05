/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Generate a checksum for the given file.
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2017-04-09 Fixed sizer flags, look for config on D:, then fallback to C:.
 * 2015-11-28 Cleaned up UI, added copy command.
 * 2010-02-09 Created
 */

#include "stdafx.h"
#include "DlgDigest.h"

#include "VersionNumber.h"

#include "../Win/ImageHelper.h"
#include "ARB/ARBAgilityRecordBook.h"
#include "ARB/ARBConfig.h"
#include "ARB/ARBStructure.h"
#include "ARBCommon/ARBMsgDigest.h"
#include "ARBCommon/Element.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ARBWinUtilities.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/ResourceManager.h"
#include "LibARBWin/Validators.h"

#include <wx/filedlg.h>
#include <wx/wfstream.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARB;
using namespace ARBCommon;
using namespace ARBWin;

CDlgDigest::CDlgDigest(wxString const& inFile)
	: m_Localization()
	, m_ctrlInit(nullptr)
	, m_ctrlConfig(nullptr)
	, m_ctrlConfigVersion(nullptr)
	, m_ctrlCopy(nullptr)
	, m_Config()
	, m_File(inFile)
	, m_MD5()
	, m_SHA1()
	, m_SHA256()
	, m_Size(0)
	, m_ConfigVersion(0)
{
	Create(
		nullptr,
		wxID_ANY,
		wxString::Format(
			L"MD5/SHA1/SHA256 Checksum %s",
			CVersionNum(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD).GetVersionString(VER_PARTS).c_str()),
		wxDefaultPosition,
		wxDefaultSize,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

	wxString baseConfig(L"\\AgilityBook\\AgilityBook\\src\\Win\\res\\DefaultConfig.xml");
	m_Config = L"D:" + baseConfig;
	if (!wxFile::Access(m_Config, wxFile::read))
		m_Config = L"C:" + baseConfig;

	wxIconBundle icons = CResourceManager::Get()->CreateIconBundle(ImageMgrAppBundle, wxART_OTHER);
	SetIcons(icons);

	m_Localization.Load();
	IARBLocalization::Init(&m_Localization);

	if (!m_File.empty())
	{
		wxBusyCursor wait;
		size_t size = 0;

		struct
		{
			ARBMsgDigest::ARBDigest type;
			wxString* pHash;
			size_t* pSize;
		} types[] = {
			{ARBMsgDigest::ARBDigest::MD5, &m_MD5, &size},
			{ARBMsgDigest::ARBDigest::SHA1, &m_SHA1, nullptr},
			{ARBMsgDigest::ARBDigest::SHA256, &m_SHA256, nullptr},
		};
		for (size_t i = 0; i < _countof(types); ++i)
		{
			wxFileInputStream file(m_File);
			wxStdInputStream stdfile(file);
			*(types[i].pHash) = ARBMsgDigest::Compute(stdfile, types[i].type, types[i].pSize);
		}
		m_Size = static_cast<long>(size);

		if (m_MD5.empty())
			m_File.clear();
		else
			m_Size = static_cast<long>(size);
	}

	m_ctrlInit = new wxButton(this, wxID_ANY, L"Init", wxDefaultPosition, wxDefaultSize, 0);
	m_ctrlInit->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDigest::OnInit, this);

	m_ctrlConfig = new wxStaticText(this, wxID_ANY, m_Config, wxDefaultPosition, wxDefaultSize, 0);

	wxStaticText* ctrlVersion
		= new wxStaticText(this, wxID_ANY, L"Config Version:", wxDefaultPosition, wxDefaultSize, 0);

	m_ctrlConfigVersion = new wxStaticText(this, wxID_ANY, L"?", wxDefaultPosition, wxDefaultSize, 0);

	wxTextCtrl* ctrlFile = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 260), -1),
		wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_File));

	wxButton* ctrlFind = new wxButton(this, wxID_ANY, L"Browse...", wxDefaultPosition, wxDefaultSize, 0);
	ctrlFind->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDigest::OnBrowse, this);

	wxStaticText* txtMD5 = new wxStaticText(this, wxID_ANY, L"MD5", wxDefaultPosition, wxDefaultSize, 0);

	wxTextCtrl* ctrlMD5 = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_MD5));
	ctrlMD5->SetBackgroundColour(GetBackgroundColour());

	wxStaticText* txtSHA1 = new wxStaticText(this, wxID_ANY, L"SHA1", wxDefaultPosition, wxDefaultSize, 0);

	wxTextCtrl* ctrlSHA1 = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_SHA1));
	ctrlSHA1->SetBackgroundColour(GetBackgroundColour());

	wxStaticText* txtSHA256 = new wxStaticText(this, wxID_ANY, L"SHA256", wxDefaultPosition, wxDefaultSize, 0);

	wxTextCtrl* ctrlSHA256 = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxDefaultSize,
		wxTE_READONLY,
		wxTextValidator(wxFILTER_NONE, &m_SHA256));
	ctrlSHA256->SetBackgroundColour(GetBackgroundColour());

	wxStaticText* txtSize = new wxStaticText(this, wxID_ANY, L"Size", wxDefaultPosition, wxDefaultSize, 0);

	wxTextCtrl* ctrlSize = new wxTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(wxDLG_UNIT_X(this, 115), -1),
		wxTE_READONLY,
		CGenericValidator(&m_Size));
	ctrlSize->SetBackgroundColour(GetBackgroundColour());

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* sizeConfigData = new wxBoxSizer(wxVERTICAL);
	sizeConfigData->Add(m_ctrlConfig, 0, wxBOTTOM, padding.Tight());

	wxBoxSizer* sizeConfigVersion = new wxBoxSizer(wxHORIZONTAL);
	sizeConfigVersion->Add(ctrlVersion, 0, wxRIGHT, padding.Inner());
	sizeConfigVersion->Add(m_ctrlConfigVersion);
	sizeConfigData->Add(sizeConfigVersion);

	wxBoxSizer* sizeConfig = new wxBoxSizer(wxHORIZONTAL);
	sizeConfig->Add(m_ctrlInit, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizeConfig->Add(sizeConfigData, 0, wxEXPAND);
	bSizer->Add(sizeConfig, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizeFile = new wxBoxSizer(wxHORIZONTAL);
	sizeFile->Add(ctrlFile, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, padding.Controls());
	sizeFile->Add(ctrlFind, 0, wxALIGN_CENTER_VERTICAL);
	bSizer->Add(sizeFile, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxFlexGridSizer* sizerGrid = new wxFlexGridSizer(4, 2, padding.Inner(), padding.Inner());
	sizerGrid->AddGrowableCol(1);
	sizerGrid->SetFlexibleDirection(wxBOTH);
	sizerGrid->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);
	sizerGrid->Add(txtMD5, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerGrid->Add(ctrlMD5, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	sizerGrid->Add(txtSHA1, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerGrid->Add(ctrlSHA1, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	sizerGrid->Add(txtSHA256, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerGrid->Add(ctrlSHA256, 0, wxALIGN_CENTER_VERTICAL | wxEXPAND);
	sizerGrid->Add(txtSize, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
	sizerGrid->Add(ctrlSize, 0, wxALIGN_CENTER_VERTICAL);
	bSizer->Add(sizerGrid, 1, wxEXPAND | wxALL, padding.Controls());

	wxSizer* sdbSizer = CreateSeparatedButtonSizer(wxOK | wxCANCEL);
	m_ctrlCopy = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_OK), wxButton);
	m_ctrlCopy->SetLabel(L"Copy");
	m_ctrlCopy->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgDigest::OnCopy, this);
	m_ctrlCopy->Enable(false);
	wxButton* cancel = wxDynamicCast(FindWindowInSizer(sdbSizer, wxID_CANCEL), wxButton);
	cancel->SetLabel(L"Close");
	bSizer->Add(sdbSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, padding.ButtonSizer());

	SetSizer(bSizer);
	Layout();
	GetSizer()->Fit(this);
	wxSize sz = GetSize();
	SetSizeHints(sz, wxSize(-1, sz.y));
	ctrlFind->SetFocus();
	CenterOnParent();

	// Auto-init
	if (wxFile::Access(m_Config, wxFile::read))
		LoadConfig();
}


bool CDlgDigest::InitConfig()
{
	if (m_ConfigVersion != 0)
		return true;

	wxFileDialog file(
		this,
		L"", // caption
		L"", // default dir
		m_Config,
		L"XML Files (*.xml)|*.xml|All Files (*.*)|*.*||",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (wxID_OK != file.ShowModal())
		return false;
	if (m_Config != file.GetPath())
	{
		m_Config = file.GetPath();
		m_ctrlConfig->SetLabel(m_Config);
	}
	return LoadConfig();
}


bool CDlgDigest::LoadConfig()
{
	wxBusyCursor wait;

	ElementNodePtr tree(ElementNode::New());

	fmt::wmemory_buffer errMsg;
	bool bOk = tree->LoadXML(m_Config.c_str(), errMsg);
	if (!bOk)
	{
		wxMessageBox(fmt::to_string(errMsg));
		return false;
	}
	if (tree->GetName() != L"DefaultConfig")
		return false;

	ARBConfig config;
	int configIdx = tree->FindElement(TREE_CONFIG);
	ARBErrorCallback err(errMsg);
	if (!config.Load(tree->GetElementNode(configIdx), ARBAgilityRecordBook::GetCurrentDocVersion(), err))
	{
		wxMessageBox(fmt::to_string(errMsg));
		return false;
	}

	m_ConfigVersion = config.GetVersion();
	m_ctrlConfigVersion->SetLabel(fmt::format(L"{}", m_ConfigVersion));

	m_ctrlCopy->Enable(true);

	return true;
}


void CDlgDigest::OnInit(wxCommandEvent& evt)
{
	InitConfig();
}


void CDlgDigest::OnBrowse(wxCommandEvent& evt)
{
	wxFileDialog dlg(nullptr, wxFileSelectorPromptStr, wxEmptyString, m_File);
	if (wxID_OK == dlg.ShowModal())
	{
		wxBusyCursor wait;
		m_File = dlg.GetPath();
		size_t size = 0;
		struct
		{
			ARBMsgDigest::ARBDigest type;
			wxString* pHash;
			size_t* pSize;
		} types[] = {
			{ARBMsgDigest::ARBDigest::MD5, &m_MD5, &size},
			{ARBMsgDigest::ARBDigest::SHA1, &m_SHA1, nullptr},
			{ARBMsgDigest::ARBDigest::SHA256, &m_SHA256, nullptr},
		};
		for (size_t i = 0; i < _countof(types); ++i)
		{
			wxFileInputStream file(m_File);
			wxStdInputStream stdfile(file);
			*(types[i].pHash) = ARBMsgDigest::Compute(stdfile, types[i].type, types[i].pSize);
		}
		m_Size = static_cast<long>(size);
		TransferDataToWindow();
	}
}


void CDlgDigest::OnCopy(wxCommandEvent& evt)
{
	if (m_File.empty())
	{
		wxBell();
		return;
	}

	if (!InitConfig())
		return;

	wxBusyCursor busy;

	// Copy data in XML form needed for version2.xml
	// <Platform arch = "osx" minOS = "10.4" ver = "2.4.5.93" config = "49"
	//   file = "https://www.agilityrecordbook.com/files/AgilityBook-2_4_5_93-mac.dmg"
	//   md5 = "0ed4c6dfbcfdd764ac877a25a8c19b2e"
	//   sha1 = "80d7faaefa9680a783c126c3918a74dc4e46e7bb"
	//   size = "10942007"
	//   / >

	wxFileName filename(m_File);

	fmt::wmemory_buffer str;
	fmt::format_to(
		std::back_inserter(str),
		L"<Platform arch=\"?\" minOS=\"?\" ver=\"{}.{}.{}.{}\" config=\"{}\"\n",
		ARB_VER_MAJOR,
		ARB_VER_MINOR,
		ARB_VER_DOT,
		ARB_VER_BUILD,
		m_ConfigVersion);
	fmt::format_to(
		std::back_inserter(str),
		L"\tfile=\"https://www.agilityrecordbook.com/files/{}\"\n",
		filename.GetFullName().wx_str());
	fmt::format_to(std::back_inserter(str), L"\tmd5=\"{}\"\n", m_MD5.wx_str());
	fmt::format_to(std::back_inserter(str), L"\tsha1=\"{}\"\n", m_SHA1.wx_str());
	fmt::format_to(std::back_inserter(str), L"\tsize=\"{}\"\n\t/>\n", m_Size);

	if (wxClipboard::Get()->Open())
	{
		auto* dataObj = new wxTextDataObject(fmt::to_string(str));
		if (!wxClipboard::Get()->SetData(dataObj))
			delete dataObj;
		wxClipboard::Get()->Flush();
		wxClipboard::Get()->Close();
		wxMessageBox(L"Copied!", _("Agility Record Book"), wxOK, this);
	}
	else
		wxBell();
}

} // namespace dconSoft
