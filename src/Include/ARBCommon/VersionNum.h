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
 * 2018-08-15 Changed VERSION_NUMBER to std::array
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2009-01-28 Removed Windows VERSIONNUM support (use VersionNumber.h)
 * 2009-01-01 Ported to wxWidgets.
 * 2007-10-24 Changed ctor and added <= operators
 * 2004-03-04 Created
 */

#include <array>
#include <string>


class CVersionNum
{
public:
	typedef std::array<unsigned short, 4> VERSION_NUMBER;

	CVersionNum()
		: m_Valid(false)
		, m_Version()
	{
	}
	~CVersionNum()
	{
	}
	CVersionNum(
			unsigned short inMajor,
			unsigned short inMinor,
			unsigned short inDot,
			unsigned short inBuild)
		: m_Valid(true)
		, m_Version({ inMajor, inMinor, inDot, inBuild })
	{
	}
	CVersionNum(std::wstring const& inVer)
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
		: m_Valid(rhs.m_Valid)
		, m_Version(rhs.m_Version)
	{
		rhs.clear();
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
			m_Valid = rhs.m_Valid;
			m_Version = rhs.m_Version;
			rhs.clear();
		}
		return *this;
	}

	/**
	 * Parse a version number, must be in form "n.n.n.n".
	 */
	bool Parse(std::wstring const& inVer);

	void Assign(
			unsigned short inMajor,
			unsigned short inMinor,
			unsigned short inDot,
			unsigned short inBuild)
	{
		m_Valid = (inMajor > 0 || inMinor > 0 || inDot > 0 || inBuild > 0);
		m_Version = { inMajor, inMinor, inDot, inBuild };
	}

	/**
	 * Equality is based solely on the version number, not any language aspects.
	 */
	bool operator==(CVersionNum const& rhs) const { return m_Version == rhs.m_Version; }
	bool operator<(CVersionNum const& rhs) const { return m_Version < rhs.m_Version; }
	bool operator<=(CVersionNum const& rhs) const { return operator==(rhs) || rhs.operator>(*this); }
	bool operator>(CVersionNum const& rhs) const { return m_Version > rhs.m_Version; }
	bool operator>=(CVersionNum const& rhs) const { return operator==(rhs) || rhs.operator<(*this); }

	void clear()
	{
		m_Valid = false;
		m_Version = { 0, 0, 0, 0 };
	}

	bool Valid() const
	{
		return m_Valid;
	}

	std::wstring GetVersionString() const;

	void GetVersion(VERSION_NUMBER& outVer) const
	{
		outVer = m_Version;
	}

private:
	bool m_Valid;
	VERSION_NUMBER m_Version;
};
