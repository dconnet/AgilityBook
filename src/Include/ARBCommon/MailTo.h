#pragma once

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

#include "ARBTypes.h"
#include "LibwxARBCommon.h"

#include <set>
#include <string>
#include <vector>


class ARBCOMMON_API CMailTo
{
public:
	CMailTo();

	bool AddTo(std::wstring const& addr);
	void ClearTo();

	bool AddCC(std::wstring const& addr);
	void ClearCC();

	bool AddBCC(std::wstring const& addr);
	void ClearBCC();

	void SetSubject(std::wstring const& subject);
	void SetBody(std::wstring const& body);

	// clearText is to a user friendly version can be presented to user
	std::string Uri(bool clearText = false) const;

private:
	std::string Encode(std::wstring const& str, bool clearText) const;

	std::vector<std::wstring> m_to;
	std::vector<std::wstring> m_cc;
	std::vector<std::wstring> m_bcc;
	std::wstring m_subject;
	std::wstring m_body;
};
