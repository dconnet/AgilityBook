/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Implementation of IARBConfigHandler
 * @author David Connet
 *
 * Revision History
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-04 DRC Ported to wxWidgets.
 * @li 2008-10-31 DRC Created
 */

#include "stdafx.h"
#include "ConfigHandler.h"

#include "Element.h"
#include "StringUtil.h"
#if defined(__WXWINDOWS__)
#include <wx/filesys.h>
#include <wx/mstream.h>
#include <wx/stdpaths.h>
#else
#include "zlib/contrib/minizip/unzip.h"
#endif

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


bool CConfigHandler::LoadWxFile(
		std::wstring const& zipFile,
		std::wstring const& archiveFile,
		std::string& outData)
{
	outData.erase();
#if defined(__WXWINDOWS__)
	wxString zipfile = wxFileSystem::FileNameToURL(wxString(zipFile.c_str()));
	zipfile += L"#zip:" + StringUtil::stringWX(archiveFile);
	wxFileSystem filesys;
	wxFSFile* file = filesys.OpenFile(zipfile);
	if (file)
	{
		size_t size = 0;
		wxInputStream* input = file->GetStream();
		while (input->CanRead())
		{
			char buffer[1024];
			size_t num = 1024;
			input->Read(buffer, num);
			outData.append(buffer, input->LastRead());
			size += input->LastRead();
		}
		delete file;
		return true;
	}
	else
	{
		wxLogMessage(zipfile);
		assert(file);
	}
#else

    bool rc = false;
#pragma PRAGMA_TODO(Write LoadWxFile)
#ifdef USEWIN32IOAPI
    zlib_filefunc64_def ffunc;
    fill_win32_filefunc64A(&ffunc);
    unzFile uf = unzOpen2_64(zipfilename,&ffunc);
#else
    //unzFile uf = unzOpen64(zipFile.c_str());
    unzFile uf = unzOpen("D:\\AgilityBook\\trunk\\AgilityBook\\bin\\testVC100x64\\Debug\\TestARB.dat");
#endif
    if (uf)
    {
        std::string s = StringUtil::stringA(archiveFile);
        if (UNZ_OK == unzLocateFile(uf, s.c_str(), 0))
        {
            char filename_inzip[256];
            unz_file_info64 file_info;
            if (UNZ_OK == unzGetCurrentFileInfo64(uf, &file_info, filename_inzip, sizeof(filename_inzip), NULL, 0, NULL, 0))
            {
                uInt size_buf = 2048;
                void* buf = (void*)malloc(size_buf);
                char* p = filename_inzip;
                char* filename_withoutpath = filename_inzip;
                while ((*p) != '\0')
                {
                    if (((*p)=='/') || ((*p)=='\\'))
                        filename_withoutpath = p+1;
                    p++;
                }
                if (UNZ_OK == unzOpenCurrentFile(uf))
                {
                    rc = true;
                    int err;
                    do
                    {
                        err = unzReadCurrentFile(uf, buf, size_buf);
                        if (err < 0)
                            break;
                        else if (err > 0)
                        {
                            outData.append((char*)buf, err);
                        }
                    } while (err > 0);
                }
                free(buf);
            }
        }
        unzClose(uf);
    }
#endif
	return rc;
}


CConfigHandler::CConfigHandler()
{
}


ElementNodePtr CConfigHandler::LoadDefaultConfig() const
{
	bool bOk = false;
	std::wostringstream errMsg;
	ARBErrorCallback err(errMsg);
	ElementNodePtr tree(ElementNode::New());

#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	std::wstring datafile = L"./testarb.dat";
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = StringUtil::stringW(wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat");
#endif
#else
	std::wstring datafile = L"./testarb.dat";
#endif
	std::string data;
	if (LoadWxFile(datafile, L"DefaultConfig.xml", data))
		bOk = tree->LoadXML(data.c_str(), data.length(), errMsg);

	return bOk ? tree : ElementNodePtr();
}


std::string CConfigHandler::LoadDTD(bool bNormalizeCRNL) const
{
#if defined(__WXWINDOWS__)
#ifdef __WXMAC__
	// Command line programs on Mac are acting like unix. GetResourcesDir
	// returns /usr/local/share. And GetExecutablePath is returning nothing.
	std::wstring datafile = L"./testarb.dat";
#else
	wxFileName fileName(wxStandardPaths::Get().GetExecutablePath());
	std::wstring datafile = StringUtil::stringW(wxStandardPaths::Get().GetResourcesDir() + wxFileName::GetPathSeparator() + fileName.GetName() + L".dat");
#endif
#else
	std::wstring datafile = L"./testarb.dat";
#endif

	std::string dtd;
	LoadWxFile(datafile, L"AgilityRecordBook.dtd", dtd);

	if (bNormalizeCRNL)
		dtd = StringUtil::Replace(dtd, "\r\n", "\n");
	return dtd;
}
