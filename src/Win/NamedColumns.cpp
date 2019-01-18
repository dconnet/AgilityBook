/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief implementation of the CNamedColumns class
 * @author David Connet
 *
 * Revision History
 * 2018-12-16 Convert to fmt.
 * 2010-03-28 Created.
 */

#include "stdafx.h"
#include "DlgAssignColumns.h"
#include "NamedColumns.h"

#include "AgilityBook.h"
#include "AgilityBookOptions.h"
#include "RegItems.h"

#include "ARB/ARBAgilityRecordBook.h"
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CNamedColumns::CNamedColumns(CAgilityBookOptions::ColumnOrder eOrder)
	: m_eOrder(eOrder)
	, m_Columns()
	, m_numConfigs(0)
	, m_curConfig()
	, m_Configs()
{
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		CDlgAssignColumns::GetColumnOrder(m_eOrder, i, m_Columns[i]);

	m_numConfigs = wxConfig::Get()->Read(CFG_CI_NUMCONFIGS, 0L);
	m_curConfig = wxConfig::Get()->Read(CFG_CI_CURRENTCONFIG, wxEmptyString);

	m_Configs.clear();
	for (int index = 0; index < m_numConfigs; ++index)
	{
		CNamedColumnsData data;
		wxString name(CFG_CI_KEY_CONFIG(index));
		data.configName = wxConfig::Get()->Read(name + CFG_CI_CONFIG_NAME, wxEmptyString);
		for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		{
			CDlgAssignColumns::GetColumnOrder(m_eOrder, i, fmt::format(L"{}{}", name, data.configName), data.m_Columns[i]);
		}
		m_Configs.push_back(data);
	}
}


void CNamedColumns::Save()
{
	if (!m_curConfig.empty())
	{
		std::vector<CNamedColumnsData>::iterator iConfig = FindConfig(m_curConfig);
		if (iConfig != m_Configs.end())
		{
			for (size_t i = 0; i < IO_TYPE_MAX; ++i)
			{
				if ((*iConfig).m_Columns[i] != m_Columns[i])
				{
					m_curConfig.clear();
					break;
				}
			}
		}
	}

	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
	{
		CDlgAssignColumns::SetColumnOrder(m_eOrder, i, wxEmptyString, m_Columns[i]);
	}

	wxConfig::Get()->Write(CFG_CI_CURRENTCONFIG, m_curConfig.c_str());

	int nConfigs = static_cast<int>(m_Configs.size());
	if (nConfigs < m_numConfigs)
	{
		for (int n = nConfigs; n < m_numConfigs; ++n)
		{
			wxString val(CFG_CI_KEY_CONFIG(n, false));
			wxConfig::Get()->DeleteGroup(val);
		}
	}
	int index = 0;
	for (std::vector<CNamedColumnsData>::iterator iConfig = m_Configs.begin();
		iConfig != m_Configs.end();
		++index, ++iConfig)
	{
		wxString name(CFG_CI_KEY_CONFIG(index));
		wxConfig::Get()->Write(name + CFG_CI_CONFIG_NAME, (*iConfig).configName.c_str());
		for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		{
			CDlgAssignColumns::SetColumnOrder(m_eOrder, i, fmt::format(L"{}{}", name, (*iConfig).configName), (*iConfig).m_Columns[i]);
		}
	}
	m_numConfigs = nConfigs;
	wxConfig::Get()->Write(CFG_CI_NUMCONFIGS, m_numConfigs);
}


std::vector<CNamedColumns::CNamedColumnsData>::iterator
CNamedColumns::FindConfig(std::wstring const& inName)
{
	for (std::vector<CNamedColumnsData>::iterator i = m_Configs.begin();
		i != m_Configs.end();
		++i)
	{
		if ((*i).configName == inName)
			return i;
	}
	return m_Configs.end();
}


size_t CNamedColumns::GetAllConfigNames(std::vector<std::wstring>& outNames) const
{
	outNames.clear();
	if (0 < m_Configs.size())
	{
		outNames.reserve(m_Configs.size());
		for (std::vector<CNamedColumnsData>::const_iterator i = m_Configs.begin();
			i != m_Configs.end();
			++i)
		{
			outNames.push_back((*i).configName);
		}
	}
	return outNames.size();
}


bool CNamedColumns::SetCurrentConfig(std::wstring const& inName)
{
	std::vector<CNamedColumnsData>::iterator iter = FindConfig(inName);
	if (iter != m_Configs.end())
	{
		m_curConfig = inName;
		for (size_t i = 0; i < IO_TYPE_MAX; ++i)
			m_Columns[i] = (*iter).m_Columns[i];
		return true;
	}
	else
		return false;
}


bool CNamedColumns::AddConfig(std::wstring const& inName)
{
	bool bAdded = false;
	std::vector<CNamedColumnsData>::iterator iter = FindConfig(inName);
	if (iter == m_Configs.end())
	{
		bAdded = true;
		CNamedColumnsData data;
		data.configName = inName;
		iter = m_Configs.insert(m_Configs.end(), data);
	}
	m_curConfig = inName;
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		(*iter).m_Columns[i] = m_Columns[i];
	return bAdded;
}


bool CNamedColumns::DeleteConfig(std::wstring const& inName)
{
	std::vector<CNamedColumnsData>::iterator iter = FindConfig(inName);
	if (iter != m_Configs.end())
	{
		m_Configs.erase(iter);
		if (inName == m_curConfig)
			m_curConfig.clear();
		return true;
	}
	else
		return false;
}


void CNamedColumns::ResetDefault(int idxType)
{
	CDlgAssignColumns::GetColumnOrder(m_eOrder, idxType, m_Columns[idxType], true);
}


void CNamedColumns::SetColumn(
		int idxType,
		std::vector<long> const& columns)
{
	m_Columns[idxType].clear();
	m_Columns[idxType] = columns;
}
