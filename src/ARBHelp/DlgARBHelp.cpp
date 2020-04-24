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
 * 2018-12-16 Convert to fmt.
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

#include "../Win/ImageHelper.h"
#include "ARBCommon/BinaryData.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/format.h"
#include <wx/ffile.h>
#include <wx/filename.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

// Copy test from gdicmn.h for wxBITMAP_PNG
#if !((defined(__WINDOWS__) && wxUSE_WXDIB) || defined(__WXOSX__))
#include "images/AgilityBook16_png.c"
#include "images/AgilityBook256_png.c"
#include "images/AgilityBook32_png.c"
#include "images/AgilityBook48_png.c"
#endif


CDlgARBHelp::CDlgARBHelp()
	: wxWizard(
		nullptr,
		wxID_ANY,
		L"Agility Record Book Helper",
		wxNullBitmap,
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxIconBundle icons;
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook16)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook32)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook48)));
	icons.AddIcon(ImageHelper::CreateIconFromBitmap(wxBITMAP_PNG(AgilityBook256)));
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
	fmt::wmemory_buffer rawdata;
	wxLogNull logSuppressor;

	// System information.
	{
		std::wstring data;
		BinaryData::EncodeString(m_SysInfo, data);
		fmt::format_to(rawdata, L"\n{}\n{}\n{}\n", STREAM_SYSTEM_BEGIN, data, STREAM_SYSTEM_END);
	}

	// Registry information.
	{
		std::wstring data;
		BinaryData::EncodeString(m_RegInfo, data);
		fmt::format_to(rawdata, L"\n{}\n{}\n{}\n", STREAM_REGISTRY_BEGIN, data, STREAM_REGISTRY_END);
	}

	// Data files.
	for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
	{
		fmt::format_to(rawdata, L"\n{}", (*iFile).first);
		wxFileName fileName((*iFile).first.c_str());
		wxDateTime dtMod, dtCreate;
		if (!(*iFile).second)
			fmt::format_to(rawdata, L": Skipped");
		fmt::format_to(rawdata, L"\n");
		if (fileName.GetTimes(nullptr, &dtMod, &dtCreate))
		{
			fmt::format_to(rawdata, L"Created: {}\n", dtCreate.Format().wx_str());
			fmt::format_to(rawdata, L"Modified: {}\n", dtMod.Format().wx_str());
		}
		wxULongLong size = fileName.GetSize();
		if (wxInvalidSize != size)
			fmt::format_to(rawdata, L"Size: {}\n", StringUtil::stringW(size.ToString()));
		if ((*iFile).second)
		{
			std::wstring data;
			if (BinaryData::EncodeFile((*iFile).first, data))
			{
				fmt::format_to(rawdata, L"{}\n{}\n{}\n", STREAM_FILE_BEGIN, data, STREAM_FILE_END);
			}
			else
				fmt::format_to(rawdata, L"Error: Cannot read file\n");
		}
	}

	std::wstring data;
	BinaryData::EncodeString(fmt::to_string(rawdata), data);

	return fmt::format(L"\n{}\n{}\n{}\n", STREAM_DATA_BEGIN, data, STREAM_DATA_END);
}
