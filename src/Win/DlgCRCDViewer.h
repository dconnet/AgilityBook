#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the CDlgCRCDViewer class
 * @author David Connet
 *
 * Revision History
 * 2009-02-11 Ported to wxWidgets.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2004-03-06 Created
 */

#ifdef __WXMSW__

// Metafiles are only available on Windows
class CDlgCRCDViewer : public wxDialog
{
public:
	CDlgCRCDViewer(
			HENHMETAFILE inMetaFile,
			wxWindow* pParent = nullptr);
};

#endif
