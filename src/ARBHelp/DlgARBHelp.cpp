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
#include "VersionNumber.h"

#include "../Win/ImageHelper.h"
#include "ARBCommon/BinaryData.h"
#include "ARBCommon/StringUtil.h"
#include "ARBCommon/VersionNum.h"
#include "LibARBWin/ResourceManager.h"
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
			  CVersionNum(ARB_VER_MAJOR, ARB_VER_MINOR, ARB_VER_DOT, ARB_VER_BUILD).GetVersionString(VER_PARTS)),
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


void CDlgARBHelp::AddSysInfo(wxString const& inData)
{
	m_SysInfo.clear();
	if (!inData.empty())
		m_SysInfo = inData.utf8_string();
}


void CDlgARBHelp::AddRegistryInfo(wchar_t const* inData)
{
	m_RegInfo.clear();
	if (inData)
		m_RegInfo = wxString(inData, wcslen(inData)).utf8_string();
}


void CDlgARBHelp::SetARBFileStatus(wxString const& inFileName, bool bInclude)
{
	m_IncFile[inFileName] = bInclude;
}


// Make sure this is synchronized (order of encoding) with
// DlgPageDecode (decoder)
std::string CDlgARBHelp::GetEncodedData()
{
	wxString rawdata;
	wxLogNull logSuppressor;

	// System information.
	{
		std::string data;
		BinaryData::EncodeString(m_SysInfo, data);
		rawdata << "\n" << STREAM_SYSTEM_BEGIN << "\n" << data << "\n" << STREAM_SYSTEM_END << "\n";
	}

	// Registry information.
	{
		std::string data;
		BinaryData::EncodeString(m_RegInfo, data);
		rawdata << "\n" << STREAM_REGISTRY_BEGIN << "\n" << data << "\n" << STREAM_REGISTRY_END << "\n";
	}

	// Data files.
	for (FileMap::iterator iFile = m_IncFile.begin(); iFile != m_IncFile.end(); ++iFile)
	{
		rawdata << "\n" << (*iFile).first;
		wxFileName fileName((*iFile).first);
		wxDateTime dtMod, dtCreate;
		if (!(*iFile).second)
			rawdata << ": Skipped";
		rawdata << "\n";
		if (fileName.GetTimes(nullptr, &dtMod, &dtCreate))
		{
			rawdata << "Created: " << dtCreate.Format() << "\n";
			rawdata << "Modified: " << dtMod.Format() << "\n";
		}
		wxULongLong size = fileName.GetSize();
		if (wxInvalidSize != size)
			rawdata << "Size: " << size.ToString() << "\n";
		if ((*iFile).second)
		{
			std::string data;
			if (BinaryData::EncodeFile((*iFile).first, data))
			{
				rawdata << STREAM_FILE_BEGIN << "\n" << data << "\n" << STREAM_FILE_END << "\n";
			}
			else
				rawdata << "Error: Cannot read file\n";
		}
	}

	std::string data;
	BinaryData::EncodeString(rawdata.utf8_string(), data);

	return wxString::Format("\n%s\n%s\n%s\n", STREAM_DATA_BEGIN, data, STREAM_DATA_END).utf8_string();
}

} // namespace dconSoft
