/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Main propertysheet
 * @author David Connet
 *
 * Revision History
 * @li 2012-07-10 DRC Fix serialization. Broken in 4/15 wxString checkin.
 * @li 2011-10-19 DRC Add timestamp info on skipped files. Add file size.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-08-26 DRC Fixed streaming wxString to otstringstream.
 *                    Fixed reading binary files. Added some timestamps.
 * @li 2009-03-01 DRC Ported to wxWidgets.
 */

#include "stdafx.h"
#include "DlgARBHelp.h"

#include "ARBHelp.h"
#include "ARBString.h"
#include "BinaryData.h"
#include "DlgPageEncode.h"
#include "DlgPageEncodeFiles.h"
#include "DlgPageEncodeFinish.h"
#include <wx/ffile.h>
#include <wx/filename.h>

#include "../Win/res/AgilityBook16.xpm"
#include "../Win/res/AgilityBook32.xpm"
#include "../Win/res/AgilityBook48.xpm"
#include "../Win/res/AgilityBook256.xpm"

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CDlgARBHelp::CDlgARBHelp()
	: wxWizard(NULL, wxID_ANY, L"Agility Record Book Helper", wxNullBitmap, wxDefaultPosition, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
{
	wxIconBundle icons;
	icons.AddIcon(wxIcon(AgilityBook16_xpm));
	icons.AddIcon(wxIcon(AgilityBook32_xpm));
	icons.AddIcon(wxIcon(AgilityBook48_xpm));
	icons.AddIcon(wxIcon(AgilityBook256_xpm));
	SetIcons(icons);

	CDlgPageEncode* m_pageEncode = new CDlgPageEncode(this);
	CDlgPageEncodeFiles* page2 = new CDlgPageEncodeFiles(this);
	CDlgPageEncodeFinish* page3 = new CDlgPageEncodeFinish(this);

	wxWizardPageSimple::Chain(m_pageEncode, page2);
	wxWizardPageSimple::Chain(page2, page3);

	GetPageAreaSizer()->Add(m_pageEncode);
	GetPageAreaSizer()->Add(page2);
	GetPageAreaSizer()->Add(page3);

	RunWizard(m_pageEncode);
	Destroy();
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
		if (fileName.GetTimes(NULL, &dtMod, &dtCreate))
		{
			rawdata << L"Created: " << dtCreate.Format().wx_str() << L"\n"
				<< L"Modified: " << dtMod.Format().wx_str() << L"\n";
		}
		wxULongLong size = fileName.GetSize();
		if (wxInvalidSize != size)
			rawdata << L"Size: " << StringUtil::stringW(size.ToString()) << L"\n";
		if ((*iFile).second)
		{
			wxFFile file;
			if (file.Open((*iFile).first.c_str(), L"rb"))
			{
				std::wstring data;
				BinaryData::Encode(file, data);
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
