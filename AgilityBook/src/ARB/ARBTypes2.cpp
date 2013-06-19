/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Utility classes
 * @author David Connet
 *
 * Revision History
 */

#include "stdafx.h"
#include "ARBTypes2.h"

#include "ARBCommon/StringUtil.h"

#if defined(__WXWINDOWS__)
#include <wx/string.h>
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

/////////////////////////////////////////////////////////////////////////////

std::wstring ARBVersion::str() const
{
	std::wostringstream buffer;
	buffer << Major() << L"." << Minor();
	return buffer.str();
}


ElementNode::AttribLookup ARBVersion::GetAttrib(
		ElementNodePtr element,
		std::wstring const& inName)
{
	assert(element);

	std::wstring value;
	ElementNode::AttribLookup rc = element->GetAttrib(inName, value);
	if (ElementNode::eFound == rc)
	{
		unsigned short major = 0;
		unsigned short minor = 0;
		std::wstring::size_type pos = value.find('.');
		if (std::wstring::npos != pos)
		{
			major = static_cast<unsigned short>(StringUtil::ToCLong(value));
			value = value.substr(pos+1);
			minor = static_cast<unsigned short>(StringUtil::ToCLong(value));
		}
		else
		{
			major = static_cast<unsigned short>(StringUtil::ToCLong(value));
		}
		m_Version = MakeVersion(major, minor);
	}
	return rc;
}


bool ARBVersion::AddAttrib(
		ElementNodePtr element,
		std::wstring const& inName) const
{
	assert(element);
	return element->AddAttrib(inName, str());
}
