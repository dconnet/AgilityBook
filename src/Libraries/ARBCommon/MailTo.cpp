/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Mailto uri generator
 * @author David Connet
 *
 * Revision History
 * 2021-03-16 Created
 */

#include "stdafx.h"
#include "ARBCommon/MailTo.h"

#include "ARBCommon/StringUtil.h"
#include <fmt/xchar.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


CMailTo::CMailTo()
	: m_to()
	, m_cc()
	, m_bcc()
	, m_subject()
	, m_body()
{
}


bool CMailTo::AddTo(std::wstring const& addr)
{
	if (std::find(m_to.begin(), m_to.end(), addr) != m_to.end())
		return false;
	m_to.push_back(addr);
	return true;
}


void CMailTo::ClearTo()
{
	m_to.clear();
}


bool CMailTo::AddCC(std::wstring const& addr)
{
	if (std::find(m_cc.begin(), m_cc.end(), addr) != m_cc.end())
		return false;
	m_cc.push_back(addr);
	return true;
}


void CMailTo::ClearCC()
{
	m_cc.clear();
}


bool CMailTo::AddBCC(std::wstring const& addr)
{
	if (std::find(m_bcc.begin(), m_bcc.end(), addr) != m_bcc.end())
		return false;
	m_bcc.push_back(addr);
	return true;
}


void CMailTo::ClearBCC()
{
	m_bcc.clear();
}


void CMailTo::SetSubject(std::wstring const& subject)
{
	m_subject = subject;
}


void CMailTo::SetBody(std::wstring const& body)
{
	m_body = body;
}


std::string CMailTo::Uri(bool clearText) const
{
	fmt::memory_buffer buffer;
	fmt::format_to(std::back_inserter(buffer), "{}", "mailto:");

	for (size_t i = 0; i < m_to.size(); ++i)
	{
		if (0 < i)
			fmt::format_to(std::back_inserter(buffer), "{}", ",");
		fmt::format_to(std::back_inserter(buffer), "{}", Encode(m_to[i], clearText));
	}

	if (m_cc.empty() && m_bcc.empty() && m_subject.empty() && m_body.empty())
		return fmt::to_string(buffer);

	fmt::format_to(std::back_inserter(buffer), "{}", "?");
	bool needAmp = false;

	for (size_t i = 0; i < m_cc.size(); ++i)
	{
		if (0 == i)
		{
			if (needAmp)
				fmt::format_to(std::back_inserter(buffer), "{}", "&");
			fmt::format_to(std::back_inserter(buffer), "{}", "cc=");
		}
		else
			fmt::format_to(std::back_inserter(buffer), "{}", ",");
		fmt::format_to(std::back_inserter(buffer), "{}", Encode(m_cc[i], clearText));
		needAmp = true;
	}

	for (size_t i = 0; i < m_bcc.size(); ++i)
	{
		if (0 == i)
		{
			if (needAmp)
				fmt::format_to(std::back_inserter(buffer), "{}", "&");
			fmt::format_to(std::back_inserter(buffer), "{}", "bcc=");
		}
		else
			fmt::format_to(std::back_inserter(buffer), "{}", ",");
		fmt::format_to(std::back_inserter(buffer), "{}", Encode(m_bcc[i], clearText));
		needAmp = true;
	}

	if (!m_subject.empty())
	{
		if (needAmp)
			fmt::format_to(std::back_inserter(buffer), "{}", "&");
		fmt::format_to(std::back_inserter(buffer), "subject={}", Encode(m_subject, clearText));
		needAmp = true;
	}

	if (!m_body.empty())
	{
		if (needAmp)
			fmt::format_to(std::back_inserter(buffer), "{}", "&");
		fmt::format_to(std::back_inserter(buffer), "body={}", Encode(m_body, clearText));
		needAmp = true;
	}

	return fmt::to_string(buffer);
}


std::string CMailTo::Encode(std::wstring const& str, bool clearText) const
{
	// This may not be completely correct. Reading RFCs isn't fun.
	// But it seems close enough.
	fmt::memory_buffer buffer;
	std::string s = StringUtil::stringA(str);

	if (clearText)
		return s;

	for (auto const& c : s)
	{
		if (
			// Unreserved (rfc6068)
			(c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '-' || c == '.'
			|| c == '_'
			|| c == '~'
			// SomeDelims
			|| c == '!' || c == '%' || c == '\'' || c == '(' || c == ')' || c == '*' || c == '+' || c == ',' || c == ';'
			|| c == ':' || c == '@')
		{
			fmt::format_to(std::back_inserter(buffer), "{}", c);
		}
		else
		{
			// Must cast or things like 'a3' print as '-5d'
			fmt::format_to(std::back_inserter(buffer), "%{:02x}", (unsigned char)c);
		}
	}
	return fmt::to_string(buffer);
}
