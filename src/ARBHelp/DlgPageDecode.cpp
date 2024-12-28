/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CDlgPageDecode class
 * @author David Connet
 *
 * Revision History
 * 2012-07-10 Fix serialization. Broken in 4/15 wxString checkin.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-26 Fixed streaming wxString to otstringstream.
 *            Fixed decoding binary files.
 * 2009-03-01 Ported to wxWidgets.
 * 2007-01-02 Created
 */

#include "stdafx.h"
#include "DlgPageDecode.h"

#include "ARBHelp.h"
#include "DlgARBHelp.h"

#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/BinaryData.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/DlgPadding.h"
#include "LibARBWin/Widgets.h"
#include <wx/ffile.h>
#include <wx/filename.h>
#include <algorithm>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
using namespace ARBWin;


CDlgPageDecode::CDlgPageDecode()
	: wxDialog(
		  nullptr,
		  wxID_ANY,
		  L"Decode",
		  wxDefaultPosition,
		  wxDefaultSize,
		  wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER)
{
	// Controls (these are done first to control tab order)

	wxString str(L"In order to decode the gathered information, first select all the text from '");
	str += STREAM_DATA_BEGIN;
	str += L"' to '";
	str += STREAM_DATA_END;
	str += L"' and copy here. Make sure to include those strings! Then press 'Decode'.";

	wxStaticText* staticText = new wxStaticText(this, wxID_ANY, str, wxDefaultPosition, wxDefaultSize, 0);
	staticText->Wrap(wxDLG_UNIT_X(this, 345));

	m_ctrlEncoded = new CTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE);
	m_ctrlEncoded->SetMaxLength(0);
	wxFont font = m_ctrlEncoded->GetFont();
	m_ctrlEncoded->SetFont(wxFont(font.GetPointSize(), wxFONTFAMILY_MODERN, font.GetStyle(), font.GetWeight()));

	m_ctrlDecoded = new CTextCtrl(
		this,
		wxID_ANY,
		wxEmptyString,
		wxDefaultPosition,
		wxSize(-1, wxDLG_UNIT_Y(this, 110)),
		wxHSCROLL | wxTE_MULTILINE | wxTE_READONLY);
	m_ctrlDecoded->SetMaxLength(0);
	font = m_ctrlDecoded->GetFont();
	m_ctrlDecoded->SetFont(wxFont(font.GetPointSize(), wxFONTFAMILY_MODERN, font.GetStyle(), font.GetWeight()));

	wxButton* btnDecode = new wxButton(this, wxID_ANY, L"Decode", wxDefaultPosition, wxDefaultSize, 0);
	btnDecode->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPageDecode::OnDecode, this);

	wxButton* btnClose = new wxButton(this, wxID_CANCEL, L"Close", wxDefaultPosition, wxDefaultSize, 0);
	btnClose->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CDlgPageDecode::OnCancel, this);

	// Sizers
	const ARBWin::CDlgPadding padding(this);

	wxBoxSizer* bSizer = new wxBoxSizer(wxVERTICAL);
	bSizer->Add(staticText, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_ctrlEncoded, 2, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());
	bSizer->Add(m_ctrlDecoded, 3, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, padding.Controls());

	wxBoxSizer* sizeBtns = new wxBoxSizer(wxHORIZONTAL);
	sizeBtns->Add(btnDecode, 0, wxRIGHT, padding.Controls());
	sizeBtns->Add(0, 0, 1, wxEXPAND);
	sizeBtns->Add(btnClose);

	bSizer->Add(sizeBtns, 0, wxEXPAND | wxALL, padding.Controls());

	SetSizer(bSizer);
	Layout();
	bSizer->Fit(this);
	SetSizeHints(GetSize(), wxDefaultSize);
	CenterOnScreen();
}


void CDlgPageDecode::OnDecode(wxCommandEvent& evt)
{
	wxBusyCursor wait;
	wxString editData;
	std::string data = m_ctrlEncoded->GetValue().utf8_string();

	// Make sure this is synchronized (order of decoding) with
	// DlgARBHelp (encoder)
	std::string::size_type pos = data.find(STREAM_DATA_BEGIN);
	if (std::string::npos != pos)
	{
		data = data.substr(pos + strlen(STREAM_DATA_BEGIN));
		pos = data.find(STREAM_DATA_END);
		if (std::string::npos != pos)
			data = data.substr(0, pos);
		data = StringUtil::Trim(data);

		std::string dataIn(data);
		data.clear();
		BinaryData::DecodeString(dataIn, data);
		dataIn.clear();
		data = StringUtil::Trim(data);

		editData << "Any temporary files created will be deleted upon closing this window.\n\n";

		static const struct
		{
			std::string begin;
			std::string end;
		} sc_sections[] = {
			{STREAM_SYSTEM_BEGIN, STREAM_SYSTEM_END},
			{STREAM_REGISTRY_BEGIN, STREAM_REGISTRY_END},
			{"", ""},
		};
		for (int idx = 0; !sc_sections[idx].begin.empty(); ++idx)
		{
			pos = data.find(sc_sections[idx].begin);
			if (std::string::npos != pos)
			{
				std::string::size_type posEnd = data.find(sc_sections[idx].end);
				if (pos < posEnd)
				{
					size_t posData = pos + sc_sections[idx].begin.length();
					// Dump the preceding data.
					editData << data.substr(0, posData) << "\n";
					// Trim preceding
					data = data.substr(posData);
					data = StringUtil::TrimLeft(data);
					// Get the data to decode
					posEnd = data.find(sc_sections[idx].end); // Recompute - we just changed the string
					dataIn = data.substr(0, posEnd);
					// Strip that from main data.
					data = data.substr(posEnd + sc_sections[idx].end.length());
					data = StringUtil::TrimLeft(data);
					// Now decode
					std::string dataOut;
					BinaryData::DecodeString(dataIn, dataOut);
					dataIn.clear();
					editData << dataOut << sc_sections[idx].end << "\n\n";
					dataOut.clear();
				}
			}
		}

		while ((std::string::npos != (pos = data.find(STREAM_FILE_BEGIN))))
		{
			std::string::size_type posEnd = data.find(STREAM_FILE_END);
			if (0 < posEnd && posEnd > pos)
			{
				std::string::size_type posData = pos + strlen(STREAM_FILE_BEGIN);
				// Dump the preceding data (but not identifier.
				editData << data.substr(0, pos); // New line included
				// Trim preceding
				data = data.substr(posData);
				data = StringUtil::TrimLeft(data);
				// Get the data to decode
				posEnd = data.find(STREAM_FILE_END); // Recompute - we just changed the string
				dataIn = data.substr(0, posEnd);
				// Strip that from main data.
				data = data.substr(posEnd + strlen(STREAM_FILE_END));
				data = StringUtil::TrimLeft(data);
				// Now decode
				std::vector<unsigned char> binData;
				BinaryData::Decode(dataIn, binData);
				dataIn.clear();
				// Generate a temp file name
				wxString tempname = wxFileName::CreateTempFileName(L"arb");
				m_TmpFiles.push_back(tempname);
				wxFFile output;
				if (output.Open(tempname, L"wb"))
				{
					output.Write(binData.data(), binData.size());
					output.Close();
					editData << "File written to: " << tempname << "\n\n";
				}
				else
				{
					std::string tmp(binData.begin(), binData.end());
					editData << STREAM_FILE_BEGIN << "\n" << tmp << STREAM_FILE_END << "\n\n";
				}
			}
		}
		editData << data;
	}

	else
	{
		editData << "Error in data: Unable to find " << STREAM_DATA_BEGIN;
	}

	m_ctrlDecoded->SetValue(editData.ToStdString());
}


void CDlgPageDecode::OnCancel(wxCommandEvent& evt)
{
	std::for_each(m_TmpFiles.begin(), m_TmpFiles.end(), wxRemoveFile);
	Destroy();
}

} // namespace dconSoft
