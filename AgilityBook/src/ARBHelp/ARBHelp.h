#pragma once

/*
 * Copyright � 2007-2009 David Connet. All Rights Reserved.
 *
 * Permission to use, copy, modify and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies, that both the
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of David Connet, dcon Software,
 * AgilityBook, AgilityRecordBook or "Agility Record Book" not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission. David Connet makes no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * DAVID CONNET DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO
 * EVENT SHALL DAVID CONNET BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * http://opensource.org
 * Open Source Historical Permission Notice and Disclaimer.
 */

/**
 * @file
 *
 * @brief Identifying strings
 * @author David Connet
 *
 * Revision History
 * @li 2009-03-01 DRC Ported to wxWidgets.
 * @li 2007-02-08 DRC Created
 */

#define STREAM_DATA_BEGIN		wxT("ARB Begin Data")
#define STREAM_DATA_END			wxT("ARB End Data")
#define STREAM_SYSTEM_BEGIN		wxT("ARB Begin SysInfo")
#define STREAM_SYSTEM_END		wxT("ARB End SysInfo")
#define STREAM_REGISTRY_BEGIN	wxT("ARB Begin Registry")
#define STREAM_REGISTRY_END		wxT("ARB End Registry")
#define STREAM_FILE_BEGIN		wxT("ARB Begin File")
#define STREAM_FILE_END			wxT("ARB End File")
