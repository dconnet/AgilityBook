/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief implementation of the CNamedColumns class
 * @author David Connet
 *
 * Revision History
 * @li 2010-03-28 DRC Created.
 */

#include "stdafx.h"
#include "AgilityBook.h"
#include "DlgAssignColumns.h"
#include "NamedColumns.h"

#include "ARBAgilityRecordBook.h"
#include "AgilityBookOptions.h"
#include <wx/config.h>


CNamedColumns::CNamedColumns(CAgilityBookOptions::ColumnOrder eOrder)
	: m_eOrder(eOrder)
	, m_Columns()
	, m_numConfigs(0)
	, m_curConfig()
	, m_Configs()
{
	for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		CDlgAssignColumns::GetColumnOrder(m_eOrder, i, m_Columns[i]);

	m_numConfigs = wxConfig::Get()->Read(wxT("ColumnInfo/numConfigs"), 0L);
	m_curConfig = wxConfig::Get()->Read(wxT("ColumnInfo/CurrentConfig"), wxEmptyString);

	m_Configs.clear();
	for (int index = 0; index < m_numConfigs; ++index)
	{
		CNamedColumnsData data;
		wxString name(wxT("ColumnInfo/Config"));
		name << index << wxT('/');
		data.configName = wxConfig::Get()->Read(name + wxT("name"), wxEmptyString);
		for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		{
			wxString configname(name);
			configname << data.configName;
			CDlgAssignColumns::GetColumnOrder(m_eOrder, i, configname, data.m_Columns[i]);
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

	wxConfig::Get()->Write(wxT("ColumnInfo/CurrentConfig"), m_curConfig);

	int nConfigs = static_cast<int>(m_Configs.size());
	if (nConfigs < m_numConfigs)
	{
		for (int n = nConfigs; n < m_numConfigs; ++n)
		{
			wxString val(wxT("ColumnInfo/Config"));
			val << n;
			wxConfig::Get()->DeleteGroup(val);
		}
	}
	int index = 0;
	for (std::vector<CNamedColumnsData>::iterator iConfig = m_Configs.begin();
		iConfig != m_Configs.end();
		++index, ++iConfig)
	{
		wxString name(wxT("ColumnInfo/Config"));
		name << index << wxT('/');
		wxConfig::Get()->Write(name + wxT("name"), (*iConfig).configName);
		for (size_t i = 0; i < IO_TYPE_MAX; ++i)
		{
			wxString configname(name);
			configname << (*iConfig).configName;
			CDlgAssignColumns::SetColumnOrder(m_eOrder, i, configname, (*iConfig).m_Columns[i]);
		}
	}
	m_numConfigs = nConfigs;
	wxConfig::Get()->Write(wxT("ColumnInfo/numConfigs"), m_numConfigs);
}


std::vector<CNamedColumns::CNamedColumnsData>::iterator
CNamedColumns::FindConfig(wxString const& inName)
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


size_t CNamedColumns::GetAllConfigNames(std::vector<wxString>& outNames) const
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


bool CNamedColumns::SetCurrentConfig(wxString const& inName)
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


bool CNamedColumns::AddConfig(wxString const& inName)
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


bool CNamedColumns::DeleteConfig(wxString const& inName)
{
	std::vector<CNamedColumnsData>::iterator iter = FindConfig(inName);
	if (iter != m_Configs.end())
	{
		m_Configs.erase(iter);
		if (inName == m_curConfig)
			m_curConfig.erase();
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
