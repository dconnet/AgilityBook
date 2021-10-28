#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief interface of the VersionNum class
 * @author David Connet
 *
 * Revision History
 * 2021-10-26 Added 'parts' to GetVersionString.
 * 2018-08-15 Changed VERSION_NUMBER to std::array
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-01-28 Removed Windows VERSIONNUM support (use VersionNumber.h)
 * 2009-01-01 Ported to wxWidgets.
 * 2007-10-24 Changed ctor and added <= operators
 * 2004-03-04 Created
 */

#include "LibwxARBCommon.h"

#include <array>
#include <string>


class ARBCOMMON_API CVersionNum
{
public:
	typedef std::array<unsigned short, 4> VERSION_NUMBER;

	CVersionNum()
		: m_Valid(false)
		, m_Version()
	{
	}
	CVersionNum(unsigned short inMajor, unsigned short inMinor, unsigned short inDot, unsigned short inBuild)
		: m_Valid(true)
		, m_Version({inMajor, inMinor, inDot, inBuild})
	{
	}
	explicit CVersionNum(std::wstring const& inVer)
		: m_Valid(false)
		, m_Version()
	{
		m_Valid = Parse(inVer);
	}
	CVersionNum(CVersionNum const& rhs)
		: m_Valid(rhs.m_Valid)
		, m_Version(rhs.m_Version)
	{
	}
	CVersionNum(CVersionNum&& rhs)
		: m_Valid(std::move(rhs.m_Valid))
		, m_Version(std::move(rhs.m_Version))
	{
	}
	~CVersionNum()
	{
	}
	CVersionNum& operator=(CVersionNum const& rhs)
	{
		if (this != &rhs)
		{
			m_Valid = rhs.m_Valid;
			m_Version = rhs.m_Version;
		}
		return *this;
	}
	CVersionNum& operator=(CVersionNum&& rhs)
	{
		if (this != &rhs)
		{
			m_Valid = std::move(rhs.m_Valid);
			m_Version = std::move(rhs.m_Version);
		}
		return *this;
	}

	/**
	 * Parse a version number, must be in form "n.n.n.n".
	 */
	bool Parse(std::wstring const& inVer);

	void Assign(unsigned short inMajor, unsigned short inMinor, unsigned short inDot, unsigned short inBuild)
	{
		m_Valid = (inMajor > 0 || inMinor > 0 || inDot > 0 || inBuild > 0);
		m_Version = {inMajor, inMinor, inDot, inBuild};
	}

	/**
	 * Equality is based solely on the version number, not any language aspects.
	 */
	bool operator==(CVersionNum const& rhs) const
	{
		return m_Version == rhs.m_Version;
	}
	bool operator<(CVersionNum const& rhs) const
	{
		return m_Version < rhs.m_Version;
	}
	bool operator<=(CVersionNum const& rhs) const
	{
		return operator==(rhs) || rhs.operator>(*this);
	}
	bool operator>(CVersionNum const& rhs) const
	{
		return m_Version > rhs.m_Version;
	}
	bool operator>=(CVersionNum const& rhs) const
	{
		return operator==(rhs) || rhs.operator<(*this);
	}

	void clear()
	{
		m_Valid = false;
		m_Version = {0, 0, 0, 0};
	}

	bool Valid() const
	{
		return m_Valid;
	}

	// Any invalid number (<=0, >4) will return 4 parts
	std::wstring GetVersionString(int parts = 4) const;

	/**
	 * Get version info for the a '-v' cmdline argument.
	 * @param program argv[0]
	 * @param date __DATE__
	 * @param time __TIME__
	 * @param includeNewline Include an ending newline
	 */
	std::wstring GetVersionUsage(
		wxString const& program,
		std::string const& date,
		std::string const& time,
		bool includeNewline = true) const;

	static wxString GetCompiledOn(std::string const& date, std::string const& time);
	static wxString GetCompiledOn(std::wstring const& date, std::wstring const& time);

	void GetVersion(VERSION_NUMBER& outVer) const
	{
		outVer = m_Version;
	}

private:
	bool m_Valid;
	VERSION_NUMBER m_Version;
};
