#pragma once

/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Type definitions.
 * @author David Connet
 *
 * Revision History
 * @li 2013-07-17 DRC Moved SanitizeStringForHTML to ARBMisc.
 * @li 2012-08-13 DRC Moved ARB_Q to separate file.
 * @li 2012-05-04 DRC Add bAlwaysStripZeros to ARBDouble::ToString.
 * @li 2011-08-22 DRC ARBVersion was only using 16 instead of 32bits.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-04-20 DRC Added DNR.
 * @li 2008-06-29 DRC Moved string stuff to ARBString.
 * @li 2006-02-16 DRC Cleaned up memory usage with smart pointers.
 * @li 2005-06-25 DRC Removed ARBDouble.
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-04-06 DRC Added op< to ARB_Q.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-09-01 DRC Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <set>
#include <sstream>
#include <string>
#include <vector>

#define ARB_TYPEDEF(name) \
	class name;\
	typedef std::tr1::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF2(name) \
	class name;\
	typedef std::tr1::weak_ptr<name> name##WPtr;\
	typedef std::tr1::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF_LIST(name) \
	class name;\
	class name##List;\
	typedef std::tr1::shared_ptr<name> name##Ptr;

ARB_TYPEDEF(Element)
ARB_TYPEDEF2(ElementNode)
ARB_TYPEDEF2(ElementText)


/**
 * Helper functions for quickly/easily converting doubles to strings.
 */
class ARBDouble
{
	ARBDouble();
	~ARBDouble();
public:
	enum LocaleType
	{
		eNone,		///< Use '.' in numbers always.
		eCurrent,	///< Use current locale.
		eDefault	///< Use default (system) locale.
	};
	/**
	 * Trailing zeros are trimmed unless inPrec=2.
	 * Then they are only trimmed if all zero (and inPrec=2).
	 */
	static std::wstring ToString(
			double inValue,
			int inPrec = 2,
			LocaleType eUseDefaultLocale = eDefault,
			bool bAlwaysStripZeros = false);
	/**
	 * Compare two doubles, allowing for 'prec' error.
	 */
	static bool equal(
			double const& inVal1,
			double const& inVal2,
			double inPrec = 1e-9);
};

/////////////////////////////////////////////////////////////////////////////
/**
 * A version has a major and minor number.
 * Differences between major numbers prevents older versions of the program
 * from reading newer files. Differences in minor numbers allows an older
 * version to read the newer file, but warns that some information may be
 * lost if the file is saved.
 *
 * This class is assuming that a short==16 bits and long==32 bits.
 */
class ARBVersion
{
public:
	ARBVersion()
		: m_Version(0)
	{
	}

	ARBVersion(
			unsigned short major,
			unsigned short minor)
		: m_Version(MakeVersion(major, minor))
	{
	}

	ARBVersion(ARBVersion const& inVer)
		: m_Version(inVer.m_Version)
	{
	}

	ARBVersion& operator=(ARBVersion const& rhs)
	{
		if (this != &rhs)
		{
			m_Version = rhs.m_Version;
		}
		return *this;
	}
	bool operator==(ARBVersion const& rhs) const
	{
		return m_Version == rhs.m_Version;
	}
	bool operator<(ARBVersion const& rhs) const
	{
		return m_Version < rhs.m_Version;
	}
	bool operator<=(ARBVersion const& rhs) const
	{
		return m_Version <= rhs.m_Version;
	}
	bool operator>(ARBVersion const& rhs) const
	{
		return m_Version > rhs.m_Version;
	}
	bool operator>=(ARBVersion const& rhs) const
	{
		return m_Version >= rhs.m_Version;
	}

	unsigned short Major() const
	{
		return static_cast<unsigned short>(m_Version >> 16);
	}
	unsigned short Minor() const
	{
		return static_cast<unsigned short>(m_Version & 0xffff);
	}
	std::wstring str() const;

private:
	unsigned long MakeVersion(
			unsigned short major,
			unsigned short minor)
	{
		return (major<<16) | minor;
	}

	unsigned long m_Version;
};
