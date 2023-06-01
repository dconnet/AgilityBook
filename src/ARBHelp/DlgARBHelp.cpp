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
#include "VersionNumber.h"

#include "../Win/ImageHelper.h"
#include "ARBCommon/BinaryData.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ResourceManager.h"
#include "fmt/xchar.h"
#include <wx/ffile.h>
#include <wx/filename.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


namespace dconSoft
{
using namespace ARBCommon;
using namespace ARBWin;

CDlgARBHelp::CDlgARBHelp()
	: wxWizard(
		nullptr,
		wxID_ANY,
		wxString::Format(
			L"Agility Record Book Helper %s",
			CVersionNum(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD).GetVersionString(VER_PARTS).c_str()),
		wxNullBitmap,
		wxDefaultPosition,
		wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxIconBundle icons = CResourceManager::Get()->CreateIconBundle(ImageMgrAppBundle, wxART_OTHER);
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
		m_SysInfo = StringUtil::stringA(inData);
}


void CDlgARBHelp::AddRegistryInfo(wchar_t const* inData)
{
	m_RegInfo.clear();
	if (inData)
		m_RegInfo = StringUtil::stringA(inData, wcslen(inData));
}


void CDlgARBHelp::SetARBFileStatus(std::wstring const& inFileName, bool bInclude)
{
	m_IncFile[inFileName] = bInclude;
}


// Make sure this is synchronized (order of encoding) with
// DlgPageDecode (decoder)
std::string CDlgARBHelp::GetEncodedData()
{
	fmt::memory_buffer rawdata;
	wxLogNull logSuppressor;

	// System information.
	{
		std::string data;
		BinaryData::EncodeString(m_SysInfo, data);
		fmt::format_to(std::back_inserter(rawdata), "\n{}\n{}\n{}\n", STREAM_SYSTEM_BEGIN, data, STREAM_SYSTEM_END);
	}

	// Registry information.
	{
		std::string data;
		BinaryData::EncodeString(m_RegInfo, data);
		fmt::format_to(std::back_inserter(rawdata), "\n{}\n{}\n{}\n", STREAM_REGISTRY_BEGIN, data, STREAM_REGISTRY_END);
	}

	// Data files.
	for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
	{
		fmt::format_to(std::back_inserter(rawdata), "\n{}", StringUtil::stringA((*iFile).first));
		wxFileName fileName((*iFile).first.c_str());
		wxDateTime dtMod, dtCreate;
		if (!(*iFile).second)
			fmt::format_to(std::back_inserter(rawdata), "{}", ": Skipped");
		fmt::format_to(std::back_inserter(rawdata), "{}", "\n");
		if (fileName.GetTimes(nullptr, &dtMod, &dtCreate))
		{
			fmt::format_to(std::back_inserter(rawdata), "Created: {}\n", dtCreate.Format().utf8_string());
			fmt::format_to(std::back_inserter(rawdata), "Modified: {}\n", dtMod.Format().utf8_string());
		}
		wxULongLong size = fileName.GetSize();
		if (wxInvalidSize != size)
			fmt::format_to(std::back_inserter(rawdata), "Size: {}\n", StringUtil::stringA(size.ToString()));
		if ((*iFile).second)
		{
			std::string data;
			if (BinaryData::EncodeFile((*iFile).first, data))
			{
				fmt::format_to(std::back_inserter(rawdata), "{}\n{}\n{}\n", STREAM_FILE_BEGIN, data, STREAM_FILE_END);
			}
			else
				fmt::format_to(std::back_inserter(rawdata), "{}", "Error: Cannot read file\n");
		}
	}

	std::string data;
	BinaryData::EncodeString(fmt::to_string(rawdata), data);

	return fmt::format("\n{}\n{}\n{}\n", STREAM_DATA_BEGIN, data, STREAM_DATA_END);
}

} // namespace dconSoft
