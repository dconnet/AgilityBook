#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Identifying strings
 * @author David Connet
 *
 * Revision History
 * 2009-03-01 Ported to wxWidgets.
 * 2007-02-08 Created
 */

namespace dconSoft
{
constexpr wchar_t STREAM_DATA_BEGIN[] = L"ARB Begin Data";
constexpr wchar_t STREAM_DATA_END[] = L"ARB End Data";
constexpr wchar_t STREAM_SYSTEM_BEGIN[] = L"ARB Begin SysInfo";
constexpr wchar_t STREAM_SYSTEM_END[] = L"ARB End SysInfo";
constexpr wchar_t STREAM_REGISTRY_BEGIN[] = L"ARB Begin Registry";
constexpr wchar_t STREAM_REGISTRY_END[] = L"ARB End Registry";
constexpr wchar_t STREAM_FILE_BEGIN[] = L"ARB Begin File";
constexpr wchar_t STREAM_FILE_END[] = L"ARB End File";
} // namespace dconSoft
