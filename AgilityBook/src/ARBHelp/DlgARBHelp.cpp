/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * 2012-07-10 Fix serialization. Broken in 4/15 wxString checkin.
 * 2011-10-19 Add timestamp info on skipped files. Add file size.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-08-26 Fixed streaming wxString to otstringstream.
 *            Fixed reading binary files. Added some timestamps.
 * 2009-03-01 Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "ARBHelp.h"
#include "DlgPageEncode.h"
#include "DlgPageEncodeFiles.h"
#include "DlgPageEncodeFinish.h"

#include "ARBCommon/BinaryData.h"
#include "ARBCommon/StringUtil.h"
#include <wx/ffile.h>
#include <wx/filename.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgARBHelp::CDlgARBHelp()
	: wxWizard(nullptr, wxID_ANY, L"Agility Record Book Helper", wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxIconBundle icons;
	{
		wxIcon icon;
		icon.CopyFromBitmap(wxBITMAP_PNG(AgilityBook16));
		icons.AddIcon(icon);
	}
	{
		wxIcon icon;
		icon.CopyFromBitmap(wxBITMAP_PNG(AgilityBook32));
		icons.AddIcon(icon);
	}
	{
		wxIcon icon;
		icon.CopyFromBitmap(wxBITMAP_PNG(AgilityBook48));
		icons.AddIcon(icon);
	}
	{
		wxIcon icon;
		icon.CopyFromBitmap(wxBITMAP_PNG(AgilityBook256));
		icons.AddIcon(icon);
	}
	SetIcons(icons);

	m_firstPage = new CDlgPageEncode(this);
	CDlgPageEncodeFiles* page2 = new CDlgPageEncodeFiles(this);
	CDlgPageEncodeFinish* page3 = new CDlgPageEncodeFinish(this);

	wxWizardPageSimple::Chain(m_firstPage, page2);
	wxWizardPageSimple::Chain(page2, page3);

	GetPageAreaSizer()->Add(m_firstPage);
	GetPageAreaSizer()->Add(page2);
	GetPageAreaSizer()->Add(page3);
}


void CDlgARBHelp::AddSysInfo(std::wstring const& inData)
{
	m_SysInfo.clear();
	if (!inData.empty())
		m_SysInfo = inData;
}


void CDlgARBHelp::AddRegistryInfo(wchar_t const* inData)
{
	m_RegInfo.clear();
	if (inData)
		m_RegInfo = inData;
}


void CDlgARBHelp::SetARBFileStatus(std::wstring const& inFileName, bool bInclude)
{
	m_IncFile[inFileName] = bInclude;
}


// Make sure this is synchronized (order of encoding) with
// DlgPageDecode (decoder)
std::wstring CDlgARBHelp::GetEncodedData()
{
	std::wostringstream rawdata;
	wxLogNull logSuppressor;

	// System information.
	{
		std::wstring data;
		BinaryData::EncodeString(m_SysInfo, data);
		rawdata << L"\n" << STREAM_SYSTEM_BEGIN << L"\n"
			<< data
			<< L"\n" << STREAM_SYSTEM_END << L"\n";
	}

	// Registry information.
	{
		std::wstring data;
		BinaryData::EncodeString(m_RegInfo, data);
		rawdata << L"\n" << STREAM_REGISTRY_BEGIN << L"\n"
			<< data
			<< L"\n" << STREAM_REGISTRY_END << L"\n";
	}

	// Data files.
	for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
	{
		rawdata << L"\n" << (*iFile).first;
		wxFileName fileName((*iFile).first.c_str());
		wxDateTime dtMod, dtCreate;
		if (!(*iFile).second)
			rawdata << L": Skipped";
		rawdata << L"\n";
		if (fileName.GetTimes(nullptr, &dtMod, &dtCreate))
		{
			rawdata << L"Created: " << dtCreate.Format().wx_str() << L"\n"
				<< L"Modified: " << dtMod.Format().wx_str() << L"\n";
		}
		wxULongLong size = fileName.GetSize();
		if (wxInvalidSize != size)
			rawdata << L"Size: " << StringUtil::stringW(size.ToString()) << L"\n";
		if ((*iFile).second)
		{
			std::wstring data;
			if (BinaryData::EncodeFile((*iFile).first, data))
			{
				rawdata << STREAM_FILE_BEGIN << L"\n"
					<< data
					<< L"\n" << STREAM_FILE_END << L"\n";
			}
			else
				rawdata << L"Error: Cannot read file\n";
		}
	}

	std::wstring data;
	BinaryData::EncodeString(rawdata.str(), data);

	std::wostringstream finalData;
	finalData << L"\n" << STREAM_DATA_BEGIN << L"\n"
		<< data
		<< L"\n" << STREAM_DATA_END << L"\n";

	return finalData.str();
}
