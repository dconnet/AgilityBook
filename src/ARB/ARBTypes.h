#pragma once

/*
 * Copyright © 2002-2003 David Connet. All Rights Reserved.
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
 * @brief Utility classes.
 * @author David Connet
 *
 * Revision History
 * @li 2003-09-01 DRC Added 'operator+=' and 'operator-=' to ARBDouble.
 */

#include <string>
#include <vector>
class CElement;

/////////////////////////////////////////////////////////////////////////////
/**
 * In XML/CElement, this class is only used as an attribute, never an element.
 * This serves as a translation mechanism from the string in the XML to
 * something we can use as a basic type.
 */
class ARB_Q
{
public:
	typedef enum
	{
		eQ_NA,
		eQ_Q,
		eQ_NQ,
		eQ_E,
		eQ_SuperQ,
	} eQ;
	static std::string GetValidTypes();
	static void GetValidTypes(std::vector<std::string>& outTypes);
	static int GetNumValidTypes();
	static ARB_Q GetValidType(int inIndex);

	ARB_Q() : m_Q(eQ_NA)
	{
	}
	ARB_Q(const ARB_Q& rhs) : m_Q(rhs.m_Q)
	{
	}
	~ARB_Q()
	{
	}

	ARB_Q& operator=(const ARB_Q& rhs)
	{
		if (this != &rhs)
			m_Q = rhs.m_Q;
		return *this;
	}
	bool operator==(const ARB_Q& rhs) const
	{
		return m_Q == rhs.m_Q;
	}
	bool operator!=(const ARB_Q& rhs) const
	{
		return !operator==(rhs);
	}

	bool Qualified() const
	{
		return eQ_Q == m_Q || eQ_SuperQ == m_Q;
	}

	operator eQ() const
	{
		return m_Q;
	}
	std::string str() const;

	bool Load(
		const std::string& inAttrib,
		int inVersion);
	bool Save(
		CElement& ioTree,
		const char* const inAttribName) const;

private:
	eQ m_Q;
};

/////////////////////////////////////////////////////////////////////////////
/**
 * In XML/CElement, this class is only used as an attribute, never an element.
 */
class ARBDouble
{
public:
	ARBDouble()
		: m_Prec(2)
		, m_Val(0.0)
	{
	}
	ARBDouble(double rhs)
		: m_Prec(2)
		, m_Val(rhs)
	{
	}
	ARBDouble(const ARBDouble& rhs)
		: m_Prec(rhs.m_Prec)
		, m_Val(rhs.m_Val)
	{
	}
	~ARBDouble()
	{
	}

	ARBDouble& operator=(const ARBDouble& rhs)
	{
		if (this != &rhs)
		{
			m_Prec = rhs.m_Prec;
			m_Val = rhs.m_Val;
		}
		return *this;
	}
	ARBDouble& operator+=(const ARBDouble& rhs)
	{
		m_Val += rhs.m_Val;
		return *this;
	}
	ARBDouble& operator-=(const ARBDouble& rhs)
	{
		m_Val -= rhs.m_Val;
		return *this;
	}
	bool operator==(const ARBDouble& rhs) const
	{
		return m_Prec == rhs.m_Prec
			&& m_Val == rhs.m_Val;
	}
	bool operator!=(const ARBDouble& rhs) const
	{
		return !operator==(rhs);
	}

	operator double() const
	{
		return m_Val;
	}

	std::string str() const;

	bool Load(
		const std::string& inAttrib,
		int inVersion);
	bool Save(
		CElement& ioTree,
		const char* const inAttribName) const;

private:
	int m_Prec;		///< Define the precision for displaying.
	double m_Val;	///< Actual value.
};
