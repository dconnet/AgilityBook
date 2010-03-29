#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief interface of the CNamedColumns class
 * @author David Connet
 *
 * Revision History
 * @li 2010-03-28 DRC Created.
 */

#include "AgilityBookOptions.h"
#include <vector>


class CNamedColumns
{
private:
	CAgilityBookOptions::ColumnOrder m_eOrder;
	std::vector<long> m_Columns[IO_TYPE_MAX];

	int m_numConfigs;
	wxString m_curConfig;
	struct CNamedColumnsData
	{
		wxString configName;
		std::vector<long> m_Columns[IO_TYPE_MAX];
	};
	std::vector<CNamedColumnsData> m_Configs;

	std::vector<CNamedColumnsData>::iterator FindConfig(wxString const& inName);

public:
	CNamedColumns(CAgilityBookOptions::ColumnOrder eOrder);
	void Save();

	CAgilityBookOptions::ColumnOrder Order() const	{return m_eOrder;}
	wxString GetCurrentConfig() const				{return m_curConfig;}
	std::vector<long> const& Columns(int idx) const	{return m_Columns[idx];}

	size_t GetAllConfigNames(std::vector<wxString>& outNames) const;
	bool SetCurrentConfig(wxString const& inName);
	bool AddConfig(wxString const& inName); ///< Returns if a new one was added
	bool DeleteConfig(wxString const& inName);

	void ResetDefault(int idxType);
	void SetColumn(
			int idxType,
			std::vector<long> const& columns);
};
