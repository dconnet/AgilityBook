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
 * 2016-09-04 Add a ToString wrapper.
 * 2015-12-22 Changed bAlwaysStripZeros to eStripZeros.
 * 2015-11-01 Added clear and IsSet.
 * 2014-06-09 Move string->arbversion parsing to ARBVersion.
 *            Added missing ARBVersion operator!=.
 * 2013-07-17 Moved SanitizeStringForHTML to ARBMisc.
 * 2012-08-13 Moved ARB_Q to separate file.
 * 2012-05-04 Add bAlwaysStripZeros to ARBDouble::ToString.
 * 2011-08-22 ARBVersion was only using 16 instead of 32bits.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-04-20 Added DNR.
 * 2008-06-29 Moved string stuff to ARBString.
 * 2006-02-16 Cleaned up memory usage with smart pointers.
 * 2005-06-25 Removed ARBDouble.
 * 2004-09-28 Changed how error reporting is done when loading.
 * 2004-04-06 Added op< to ARB_Q.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-09-01 Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <locale>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#define ARB_TYPEDEF(name) \
	class name;\
	typedef std::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF2(name) \
	class name;\
	typedef std::weak_ptr<name> name##WPtr;\
	typedef std::shared_ptr<name> name##Ptr;
#define ARB_TYPEDEF_LIST(name) \
	class name;\
	class name##List;\
	typedef std::shared_ptr<name> name##Ptr;

ARB_TYPEDEF(Element)
ARB_TYPEDEF2(ElementNode)
ARB_TYPEDEF2(ElementText)

// Numeric conversion methods all use wxWidgets by default.
// This is useful for testing.
#define USE_CRT 0


/**
 * Wrapper class to handle locale. Typical use is:
 *
 * ```
 * #if defined(__WXWINDOWS__) && !USE_CRT
 * wxLocale locale(wxLANGUAGE_FRENCH);
 * #else
 * CLocaleWrapper locale(LC_ALL, "french");
 * #endif
 * ```
 *
 * or:
 *
 * ```
 * #if defined(__WXWINDOWS__) && !USE_CRT
 * wxLocale locale(wxLANGUAGE_FRENCH, wxLOCALE_DONT_LOAD_DEFAULT);
 * #else
 * CLocaleWrapper locale(LC_NUMERIC, "french");
 * #endif
 * ```
 *
 * However, use GetDecimalPt regardless of defines.
 */
class CLocaleWrapper
{
	int m_category;
	char* m_pOldLocale;

public:
	CLocaleWrapper(int category, const char* locale)
		: m_category(category)
	{
		m_pOldLocale = setlocale(category, locale);
	}
	~CLocaleWrapper()
	{
		setlocale(m_category, m_pOldLocale);
	}

	static wchar_t GetDecimalPt()
	{
		wchar_t pt = L'.';
#if defined(__WXWINDOWS__) && !USE_CRT
		wxString decimalPt = wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER);
		if (0 < decimalPt.length())
			pt = decimalPt.GetChar(0);
#else
		struct lconv* l = localeconv();
		if (l && l->decimal_point)
			pt = *(l->decimal_point);
#endif
		return pt;
	}
};


/**
 * Helper functions for quickly/easily converting doubles to strings.
 */
class ARBDouble
{
	ARBDouble();
	~ARBDouble();
public:
	/// How to handle trailing zeros
	enum ZeroStrip
	{
		eCompatible,	///< Strip, unless inPrec = 2
		eStrip,			///< Strip
		eAsIs			///< Don't touch
	};
	/**
	 * Trailing zeros are trimmed unless inPrec=2.
	 * Then they are only trimmed if all zero (and inPrec=2).
	 */
	static std::wstring ToString(
			double inValue,
			int inPrec = 2,
			bool bUseCurrentLocale = true,
			ZeroStrip eStripZeros = eCompatible);
	static std::wstring ToString(
			double inValue,
			int inPrec,
			ZeroStrip eStripZeros);
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

	ARBVersion(std::wstring inStr);

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
	bool operator!=(ARBVersion const& rhs) const
	{
		return m_Version != rhs.m_Version;
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

	void clear()
	{
		m_Version = 0;
	}
	bool IsSet()
	{
		return m_Version > 0;
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
