/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Tree structure to store XML.
 * @author David Connet
 *
 * Actual reading and writing of XML is done using wxWidgets
 *
 * Revision History
 * 2022-01-31 Add wxDateTime support.
 * 2018-12-16 Convert to fmt.
 * 2017-08-03 Added initial expat support (reader, not write)
 * 2015-04-04 Add support for C99 printf formats. (Breaking change)
 * 2014-06-09 Move string->arbversion parsing to ARBVersion.
 * 2014-03-31 Fixed POCO xml load.
 * 2014-02-26 Add support for POCO xml.
 * 2013-03-23 Implement libxml LoadXML stream api.
 * 2012-11-25 Add libxml support back in.
 * 2012-09-29 Trap wx generated xml parsing errors into our buffer.
 * 2012-04-10 Based on wx-group thread, use std::string for internal use
 * 2012-03-16 Renamed LoadXML functions, added stream version.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-12 Converting all TCHAR stuff to wxWidgets
 * 2009-02-08 Fix wxWidget xml creation
 * 2009-01-05 Added libxml2 support
 * 2008-12-27 Added wxWidget support (xml)
 * 2008-11-02 Added xerces 3.0 support
 * 2007-09-06 Added GetNthElementNode
 * 2007-08-15 Modified to support mixed text/nodes.
 * 2007-08-08 Moved initialization here, so all XML usage is contained.
 * 2007-03-37 Fixed a problem releasing transcoded data.
 * 2005-06-09 Numbers were being stored/shown in scientific notation.
 * 2004-06-16 Changed ARBDate::GetString to put leadingzero into format.
 * 2004-01-04 Moved date parsing code to ARBDate::FromString.
 * 2003-11-26 Changed version number to a complex value.
 * 2003-10-22 Added a DTD parameter to SaveXML.
 * Code evolved to AgilityRecordBook Element
 * 2002-11-27 Added additional SetValue methods.
 * 2002-11-22 Fixed a problem between unicode/mbcs compiles.
 * 2002-11-19 Simplified to Element/Tree to a single class.
 * 2002-07-24 Convert to Xerces 2.0.0. Takeadvantage that
 *            XMLCh == wchar_t and MFC7's CString[AW]
 * 2001-09-13 Created CStringTree
 */

#include "stdafx.h"
#include "ARBCommon/Element.h"

#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"
#include "fmt/xchar.h"
#include <fstream>
#include <list>
#include <map>
#include <sstream>

#if defined(USE_LIBXML2)
#define __USE_LIBXML2 1
#define __USE_EXPAT   0
#define __USE_POCO    0
#define __USE_WX      0

#elif defined(USE_EXPAT)
#define __USE_LIBXML2 0
#define __USE_EXPAT   1
#define __USE_POCO    0
#define __USE_WX      0

#elif defined(USE_POCO)
#define __USE_LIBXML2 0
#define __USE_EXPAT   0
#define __USE_POCO    1
#define __USE_WX      0

#elif defined(__WXWINDOWS__)
#define __USE_LIBXML2 0
#define __USE_EXPAT   0
#define __USE_POCO    0
#define __USE_WX      1

#else
#error No idea what XML parser you want!
#endif

#if __USE_LIBXML2
#include "libxml/encoding.h"
#include "libxml/parser.h"
#include "libxml/xmlIO.h"
#include "libxml/xmlreader.h"
#include "libxml/xmlstring.h"
#include "libxml/xmlversion.h"
#include "libxml/xmlwriter.h"

#if defined(_WINDLL)
#pragma comment(lib, "libxml2.lib")
#else
#pragma comment(lib, "libxml2_a.lib")
#pragma message("Compiling Element with libxml2 " LIBXML_DOTTED_VERSION)
#endif

#elif __USE_EXPAT
//#define XML_STATIC -> defined in .props file
#include "expat.h"
#pragma comment(lib, "libexpatMT.lib")
#pragma message("Compiling Element with expat " STRING(XML_MAJOR_VERSION) "." STRING(XML_MINOR_VERSION) "." STRING( \
	XML_MICRO_VERSION))

#elif __USE_POCO
#include "Poco/DOM/AutoPtr.h"
#include "Poco/DOM/DOMParser.h"
#include "Poco/DOM/DOMWriter.h"
#include "Poco/DOM/Document.h"
#include "Poco/DOM/Element.h"
#include "Poco/DOM/NamedNodeMap.h"
#include "Poco/DOM/Node.h"
#include "Poco/DOM/Text.h"
#include "Poco/DOM/TreeWalker.h"
#include "Poco/SAX/InputSource.h"
#include "Poco/UTF8Encoding.h"
#include "Poco/Version.h"
#include "Poco/XML/XMLWriter.h"
#pragma message("Compiling Element with POCO " STRING(POCO_VERSION))

#else
#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>
#pragma message("Compiling Element with wxWidgets " wxVERSION_NUM_DOT_STRING)
#endif

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

#if !defined(_WIN32) && !defined(__WXWINDOWS__)
#include <iostream>
static void OutputDebugString(wchar_t const* msg)
{
	if (msg)
		fmt::print(std::wcout, L"{}", msg);
}
#endif

////////////////////////////////////////////////////////////////////////////

#if defined(__WXWINDOWS__)

class wxInputStdStream : public wxInputStream
{
public:
	wxInputStdStream(std::istream& stream)
		: m_stream(stream)
	{
	}

	size_t OnSysRead(void* buffer, size_t size) override
	{
		size_t count = 0;
		if (m_stream.good())
		{
			m_stream.read(static_cast<char*>(buffer), size);
			count = static_cast<size_t>(m_stream.gcount());

			if (m_stream.eof())
			{
				m_lasterror = wxSTREAM_EOF;
			}
			else if (m_stream.good())
			{
				m_lasterror = wxSTREAM_NO_ERROR;
			}
			else
			{
				m_lasterror = wxSTREAM_READ_ERROR;
			}
		}
		return count;
	}

protected:
	std::istream& m_stream;
};


class wxOutputStdStream : public wxOutputStream
{
public:
	wxOutputStdStream(std::ostream& stream)
		: m_stream(stream)
	{
	}

	size_t OnSysWrite(const void* buffer, size_t size) override
	{
		size_t count = 0;
		if (m_stream.good())
		{
			m_stream.write(static_cast<const char*>(buffer), size);
			count = size;

			if (m_stream.eof())
			{
				m_lasterror = wxSTREAM_EOF;
			}
			else if (m_stream.good())
			{
				m_lasterror = wxSTREAM_NO_ERROR;
			}
			else
			{
				m_lasterror = wxSTREAM_WRITE_ERROR;
			}
		}
		return count;
	}

protected:
	std::ostream& m_stream;
};

#endif // __WXWINDOWS__

////////////////////////////////////////////////////////////////////////////

bool Element::Initialize(std::wstring& outMsg)
{
	outMsg.clear();
#if __USE_LIBXML2
	xmlInitParser();
	xmlSubstituteEntitiesDefault(1);
#endif
	return true;
}


void Element::Terminate()
{
#if __USE_LIBXML2
	xmlCleanupParser();
#endif
}


Element::Element()
{
}


Element::~Element()
{
}


////////////////////////////////////////////////////////////////////////////

#if __USE_LIBXML2

class XMLstring : public std::basic_string<xmlChar>
{
public:
	XMLstring()
	{
	}
	XMLstring(std::string const& inStr)
		: std::basic_string<xmlChar>((xmlChar*)inStr.c_str())
	{
	}
	XMLstring(std::wstring const& inStr)
	{
		std::string str = StringUtil::stringA(inStr);
		operator=(str.c_str());
	}
	XMLstring& operator=(std::string const& inStr)
	{
		std::basic_string<xmlChar>::operator=((xmlChar*)inStr.c_str());
		return *this;
	}
	XMLstring& operator=(std::wstring const& inStr)
	{
		std::string str = StringUtil::stringA(inStr);
		operator=(str.c_str());
		return *this;
	}
	XMLstring& operator=(const char* inStr)
	{
		std::basic_string<xmlChar>::operator=((xmlChar*)inStr);
		return *this;
	}
	XMLstring& operator=(const wchar_t* inStr)
	{
		std::string str = StringUtil::stringA(inStr);
		operator=(str.c_str());
		return *this;
	}
};


class StringDOM : public std::wstring
{
public:
	StringDOM()
	{
	}
	StringDOM(const xmlChar* inStr)
#ifndef UNICODE
		: std::string((const char*)inStr)
#endif
	{
#ifdef UNICODE
		std::string s((const char*)inStr);
		std::wstring::operator=(StringUtil::stringW(s));
#endif
	}
};


static void ReadDoc(xmlNode const* inNode, ElementNode& tree)
{
	if (!inNode)
		return;
	for (xmlAttr* attrib = inNode->properties; attrib; attrib = attrib->next)
	{
		tree.AddAttrib(StringDOM(attrib->name), StringDOM(attrib->children->content));
	}
	for (xmlNode* child = inNode->children; child; child = child->next)
	{
		if (child->type == XML_ELEMENT_NODE)
		{
			if (tree.HasTextNodes())
				tree.RemoveAllElements();
			ElementNodePtr subtree = tree.AddElementNode(StringDOM(child->name));
			ReadDoc(child, *subtree);
		}
		else if (child->type == XML_TEXT_NODE)
		{
			if (tree.HasTextNodes() || 0 == tree.GetElementCount())
				tree.SetValue(StringDOM(child->content));
		}
	}
}


static void CreateDoc(xmlTextWriterPtr formatter, xmlOutputBufferPtr outputBuffer, ElementNode const& toWrite)
{
	int i;
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		std::wstring name, value;
		toWrite.GetNthAttrib(i, name, value);
		XMLstring name2(name);
		XMLstring value2(value);
		xmlTextWriterWriteAttribute(formatter, name2.c_str(), value2.c_str());
		xmlOutputBufferFlush(outputBuffer);
	}
	int count = toWrite.GetElementCount();
	for (i = 0; i < count; ++i)
	{
		ElementPtr element = toWrite.GetElement(i);
		switch (element->GetType())
		{
		case ARBElementType::Node:
		{
			XMLstring name(element->GetName());
			xmlTextWriterStartElement(formatter, name.c_str());
			xmlOutputBufferFlush(outputBuffer);
			CreateDoc(formatter, outputBuffer, *(dynamic_cast<ElementNode*>(element.get())));
			xmlTextWriterEndElement(formatter);
			xmlOutputBufferFlush(outputBuffer);
		}
		break;
		case ARBElementType::Text:
		{
			XMLstring value(element->GetValue());
			xmlTextWriterWriteString(formatter, value.c_str());
			xmlOutputBufferFlush(outputBuffer);
		}
		break;
		}
	}
}

#elif __USE_EXPAT

#elif __USE_POCO

static void ReadDoc(Poco::XML::Document* pDoc, Poco::XML::Node* inNode, ElementNode& tree)
{
	if (!inNode)
		return;

	Poco::XML::AutoPtr<Poco::XML::NamedNodeMap> attribs = inNode->attributes();
	if (attribs)
	{
		for (unsigned long i = 0; i < attribs->length(); ++i)
		{
			Poco::XML::Node* pItem = attribs->item(i);
			tree.AddAttrib(StringUtil::stringW(pItem->nodeName()), StringUtil::stringW(pItem->nodeValue()));
		}
	}

	for (Poco::XML::Node* pChild = inNode->firstChild(); pChild; pChild = pChild->nextSibling())
	{
		switch (pChild->nodeType())
		{
		case Poco::XML::Node::ELEMENT_NODE:
		{
			if (tree.HasTextNodes())
				tree.RemoveAllElements();
			ElementNodePtr subtree = tree.AddElementNode(StringUtil::stringW(pChild->nodeName()));
			ReadDoc(pDoc, pChild, *subtree);
		}
		break;
		case Poco::XML::Node::TEXT_NODE:
			if (tree.HasTextNodes() || 0 == tree.GetElementCount())
			{
				std::wstring content = StringUtil::stringW(pChild->nodeValue());
				if (!content.empty())
					tree.SetValue(content);
			}
			break;
		}
	}
}

static void CreateDoc(Poco::XML::Document* pDoc, Poco::XML::Element* node, ElementNode const& toWrite)
{
	int i;
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		std::wstring name, value;
		toWrite.GetNthAttrib(i, name, value);
		node->setAttributeNS("", StringUtil::stringA(name), StringUtil::stringA(value));
	}

	int count = toWrite.GetElementCount();
	for (i = 0; i < count; ++i)
	{
		ElementPtr element = toWrite.GetElement(i);
		switch (element->GetType())
		{
		case ARBElementType::Node:
		{
			Poco::XML::XMLString name(StringUtil::stringA(element->GetName()));
			Poco::XML::AutoPtr<Poco::XML::Element> pChild = pDoc->createElement(name);
			node->appendChild(pChild);
			CreateDoc(pDoc, pChild, *(dynamic_cast<ElementNode*>(element.get())));
		}
		break;
		case ARBElementType::Text:
		{
			Poco::XML::XMLString value(StringUtil::stringA(element->GetValue()));
			Poco::XML::AutoPtr<Poco::XML::Text> pText = pDoc->createTextNode(value);
			node->appendChild(pText);
		}
		break;
		}
	}
}

#elif __USE_WX

static void ReadDoc(wxXmlNode const* node, ElementNode& tree)
{
	wxXmlAttribute* attribs = node->GetAttributes();
	while (attribs)
	{
		tree.AddAttrib(StringUtil::stringW(attribs->GetName()), StringUtil::stringW(attribs->GetValue()));
		attribs = attribs->GetNext();
	}
	std::wstring content = StringUtil::stringW(node->GetNodeContent());
	if (!content.empty())
		tree.SetValue(content);
	wxXmlNode* child = node->GetChildren();
	while (child)
	{
		if (wxXML_ELEMENT_NODE == child->GetType())
		{
			ElementNodePtr subtree = tree.AddElementNode(StringUtil::stringW(child->GetName()));
			ReadDoc(child, *subtree);
		}
		child = child->GetNext();
	}
}


static void CreateDoc(wxXmlNode* node, ElementNode const& toWrite)
{
	int i;
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		std::wstring name, value;
		toWrite.GetNthAttrib(i, name, value);
		node->AddAttribute(StringUtil::stringWX(name), StringUtil::stringWX(value));
	}
	int count = toWrite.GetElementCount();
	wxXmlNode* lastChild = nullptr;
	for (i = 0; i < count; ++i)
	{
		ElementPtr element = toWrite.GetElement(i);
		switch (element->GetType())
		{
		case ARBElementType::Node:
		{
			wxXmlNode* child = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, StringUtil::stringWX(element->GetName()));
			if (lastChild)
				lastChild->SetNext(child);
			else
				node->AddChild(child);
			lastChild = child;
			CreateDoc(child, *(dynamic_cast<ElementNode*>(element.get())));
		}
		break;
		case ARBElementType::Text:
		{
			wxXmlNode* child = new wxXmlNode(
				nullptr,
				wxXML_TEXT_NODE,
				StringUtil::stringWX(element->GetName()),
				StringUtil::stringWX(element->GetValue()));
			node->AddChild(child);
		}
		break;
		}
	}
}

#else
#error No idea what XML parser you want!
#endif

/////////////////////////////////////////////////////////////////////////////

static std::wstring GetIndentBuffer(int indent)
{
	std::wstring str;
	if (0 < indent)
	{
		str = fmt::format(L"{:{}s}", L" ", indent);
	}
	return str;
}

static void LogMessage(fmt::wmemory_buffer& msg)
{
#if defined(__WXWINDOWS__)
	wxLogMessage(L"%s", fmt::to_string(msg).c_str());
#else
	fmt::format_to(std::back_inserter(msg), L"{}", L"\n");
	OutputDebugString(fmt::to_string(msg).c_str());
#endif
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ElementNode_concrete : public ElementNode
{
public:
	ElementNode_concrete()
	{
	}
	ElementNode_concrete(std::wstring const& inName)
		: ElementNode(inName)
	{
	}
};
} // namespace


ElementNodePtr ElementNode::New()
{
	return std::make_shared<ElementNode_concrete>();
}


ElementNodePtr ElementNode::New(std::wstring const& inText)
{
	return std::make_shared<ElementNode_concrete>(inText);
}


ElementNode::ElementNode()
{
}


ElementNode::ElementNode(std::wstring const& inName)
	: m_Name(inName)
{
}


void ElementNode::RemoveAllTextNodes()
{
	for (std::vector<ElementPtr>::iterator i = m_Elements.begin(); i != m_Elements.end();)
	{
		if (ARBElementType::Text == (*i)->GetType())
			i = m_Elements.erase(i);
		else
			++i;
	}
}


void ElementNode::Dump(int inLevel) const
{
	int i;
	fmt::wmemory_buffer msg;
	fmt::format_to(std::back_inserter(msg), L"{}{}", GetIndentBuffer(inLevel), m_Name);
	for (i = 0; i < GetAttribCount(); ++i)
	{
		std::wstring name, value;
		GetNthAttrib(i, name, value);
		fmt::format_to(std::back_inserter(msg), L" {}=\"{}\"", name, value);
	}
	LogMessage(msg);
	for (i = 0; i < GetElementCount(); ++i)
	{
		GetElement(i)->Dump(inLevel + 1);
	}
}


ARBElementType ElementNode::GetType() const
{
	return ARBElementType::Node;
}


std::wstring const& ElementNode::GetName() const
{
	return m_Name;
}


void ElementNode::SetName(std::wstring const& inName)
{
	m_Name = inName;
}


std::wstring ElementNode::GetValue() const
{
	std::wstring value;
	for (int i = 0; i < GetElementCount(); ++i)
	{
		if (ARBElementType::Text == GetElement(i)->GetType())
			value += GetElement(i)->GetValue();
	}
	return value;
}


void ElementNode::SetValue(std::wstring const& inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(wchar_t const* const inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(short inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(unsigned short inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(long inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(unsigned long inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(double inValue, int inPrec)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue, inPrec);
	m_Elements.push_back(pText);
}


void ElementNode::clear()
{
	m_Name.clear();
	m_Attribs.clear();
	m_Elements.clear();
}


int ElementNode::GetAttribCount() const
{
	return static_cast<int>(m_Attribs.size());
}


ARBAttribLookup ElementNode::GetNthAttrib(int inIndex, std::wstring& outName, std::wstring& outValue) const
{
	MyAttributes::const_iterator iter = m_Attribs.begin();
	while (0 < inIndex)
	{
		++iter;
		--inIndex;
	}
	if (iter != m_Attribs.end())
	{
		outName = (*iter).first;
		outValue = (*iter).second;
		return ARBAttribLookup::Found;
	}
	else
		return ARBAttribLookup::NotFound;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, std::wstring& outValue) const
{
	MyAttributes::const_iterator iter = m_Attribs.find(inName);
	if (iter != m_Attribs.end())
	{
		outValue = (*iter).second;
		return ARBAttribLookup::Found;
	}
	else
		return ARBAttribLookup::NotFound;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, ARBVersion& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		outValue = ARBVersion(value);
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, ARBDate& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		ARBDate date = ARBDate::FromString(value, ARBDateFormat::DashYMD);
		if (date.IsValid())
			outValue = date;
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


#if defined(__WXWINDOWS__)
ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, wxDateTime& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		wxDateTime date;
		if (date.ParseISOCombined(value, ' '))
			outValue = date;
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}
#endif


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, bool& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (value == L"y")
			outValue = true;
		else if (value == L"n")
			outValue = false;
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, short& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (0 < value.length())
			outValue = static_cast<short>(StringUtil::ToCLong(value));
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, unsigned short& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (0 < value.length())
			outValue = static_cast<unsigned short>(StringUtil::ToCULong(value));
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, long& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCLong(value, outValue))
				rc = ARBAttribLookup::Invalid;
		}
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, unsigned long& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCULong(value, outValue))
				rc = ARBAttribLookup::Invalid;
		}
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


ARBAttribLookup ElementNode::GetAttrib(std::wstring const& inName, double& outValue) const
{
	std::wstring value;
	ARBAttribLookup rc = GetAttrib(inName, value);
	if (ARBAttribLookup::Found == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCDouble(value, outValue))
				rc = ARBAttribLookup::Invalid;
		}
		else
			rc = ARBAttribLookup::Invalid;
	}
	return rc;
}


bool ElementNode::AddAttrib(std::wstring const& inName, std::wstring const& inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = inValue;
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, wchar_t const* const inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = inValue;
	else
		m_Attribs[inName] = std::wstring();
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, ARBVersion const& inValue)
{
	return AddAttrib(inName, inValue.str());
}


bool ElementNode::AddAttrib(std::wstring const& inName, ARBDate const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.GetString(ARBDateFormat::DashYMD));
	return false;
}


#if defined(__WXWINDOWS__)
bool ElementNode::AddAttrib(std::wstring const& inName, wxDateTime const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.FormatISOCombined(' ').wc_str());
	return false;
}
#endif


bool ElementNode::AddAttrib(std::wstring const& inName, bool inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = L"y";
	else
		m_Attribs[inName] = L"n";
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, short inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = fmt::format(L"{}", inValue);
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, unsigned short inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = fmt::format(L"{}", inValue);
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, long inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = fmt::format(L"{}", inValue);
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, unsigned long inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = fmt::format(L"{}", inValue);
	return true;
}


bool ElementNode::AddAttrib(std::wstring const& inName, double inValue, int inPrec)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = ARBDouble::ToString(inValue, inPrec, false);
	return true;
}


bool ElementNode::RemoveAttrib(std::wstring const& inName)
{
	MyAttributes::iterator iter = m_Attribs.find(inName);
	if (iter != m_Attribs.end())
	{
		m_Attribs.erase(iter);
		return true;
	}
	else
		return false;
}


void ElementNode::RemoveAllAttribs()
{
	m_Attribs.clear();
}


int ElementNode::GetElementCount() const
{
	return static_cast<int>(m_Elements.size());
}


int ElementNode::GetNodeCount(ARBElementType type) const
{
	int nCount = 0;
	for (std::vector<ElementPtr>::const_iterator iter = m_Elements.begin(); iter != m_Elements.end(); ++iter)
	{
		if (type == (*iter)->GetType())
			++nCount;
	}
	return nCount;
}


bool ElementNode::HasTextNodes() const
{
	for (std::vector<ElementPtr>::const_iterator iter = m_Elements.begin(); iter != m_Elements.end(); ++iter)
	{
		if (ARBElementType::Text == (*iter)->GetType())
			return true;
	}
	return false;
}


ElementPtr ElementNode::GetElement(int inIndex) const
{
	return m_Elements[inIndex];
}


ElementPtr ElementNode::GetElement(int inIndex)
{
	return m_Elements[inIndex];
}


ElementNodePtr ElementNode::GetElementNode(int inIndex) const
{
	return std::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
}


ElementNodePtr ElementNode::GetElementNode(int inIndex)
{
	return std::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
}


ElementNodePtr ElementNode::GetNthElementNode(int inIndex) const
{
	int index = -1;
	int nElements = static_cast<int>(m_Elements.size());
	for (int iElement = 0; iElement < nElements; ++iElement)
	{
		if (ARBElementType::Node == m_Elements[iElement]->GetType())
		{
			++index;
			if (index == inIndex)
				return GetElementNode(iElement);
		}
	}
	return ElementNodePtr();
}


ElementNodePtr ElementNode::GetNthElementNode(int inIndex)
{
	ElementNode const* constThis = this;
	return constThis->GetNthElementNode(inIndex);
}


ElementNodePtr ElementNode::AddElementNode(std::wstring const& inName, int inAt)
{
	size_t index = 0;
	std::vector<ElementPtr>::iterator iter = m_Elements.begin();
	if (0 < inAt)
	{
		index = 0;
		for (; 0 < inAt && iter != m_Elements.end(); ++index, ++iter, --inAt)
			;
	}
	else if (0 > inAt)
	{
		index = m_Elements.size();
		iter = m_Elements.end();
	}
	ElementNodePtr pNode = ElementNode::New(inName);
	m_Elements.insert(iter, pNode);
	return pNode;
}


ElementTextPtr ElementNode::AddElementText(std::wstring const& inText, int inAt)
{
	assert(0 == m_Value.length());
	size_t index = 0;
	std::vector<ElementPtr>::iterator iter = m_Elements.begin();
	if (0 < inAt)
	{
		index = 0;
		for (; 0 < inAt && iter != m_Elements.end(); ++index, ++iter, --inAt)
			;
	}
	else
	{
		index = m_Elements.size();
		iter = m_Elements.end();
	}
	ElementTextPtr pText = ElementText::New(inText);
	m_Elements.insert(iter, pText);
	return pText;
}


bool ElementNode::RemoveElement(int inIndex)
{
	bool bOk = false;
	if (0 <= inIndex && inIndex < static_cast<int>(m_Elements.size()))
	{
		std::vector<ElementPtr>::iterator iter = m_Elements.begin();
		iter += inIndex;
		m_Elements.erase(iter);
		bOk = true;
	}
	return bOk;
}


void ElementNode::RemoveAllElements()
{
	m_Elements.clear();
}


int ElementNode::FindElement(std::wstring const& inName, int inStartFrom) const
{
	if (0 > inStartFrom)
		inStartFrom = 0;
	for (; inStartFrom < static_cast<int>(m_Elements.size()); ++inStartFrom)
	{
		if (m_Elements[inStartFrom]->GetName() == inName)
			return inStartFrom;
	}
	return -1;
}


bool ElementNode::FindElementDeep(
	ElementNode const*& outParentNode,
	int& outElementIndex,
	std::wstring const& inName,
	std::wstring const* inValue) const
{
	int nCount = GetElementCount();
	for (int i = 0; i < nCount; ++i)
	{
		ElementNodePtr element = GetElementNode(i);
		if (!element)
			continue;
		if (element->GetName() == inName && (!inValue || (inValue && element->GetValue() == *inValue)))
		{
			outParentNode = this;
			outElementIndex = i;
			return true;
		}
		else if (element->FindElementDeep(outParentNode, outElementIndex, inName, inValue))
			return true;
	}
	return false;
}


#if __USE_LIBXML2

static bool LoadXMLNode(ElementNode& node, xmlDocPtr inSource, fmt::wmemory_buffer& ioErrMsg)
{
	node.clear();

	xmlNode* root = xmlDocGetRootElement(inSource);
	if (!root)
		return false;
	node.SetName(StringDOM(root->name));
	ReadDoc(root, node);
	return true;
}

#elif __USE_EXPAT

struct CExpatData
{
	ElementNode* m_root;
	std::list<ElementNode*> m_elements;
	std::string m_data;
};

static void XMLCALL ExpatStart(void* data, const char* el, const char** attr)
{
	CExpatData* pData = reinterpret_cast<CExpatData*>(data);
	pData->m_data.clear();

	ElementNode* node;
	if (pData->m_elements.size() == 0)
	{
		node = pData->m_root;
		node->SetName(StringUtil::stringW(std::string(el)));
	}
	else
		node = pData->m_elements.front()->AddElementNode(StringUtil::stringW(std::string(el))).get();
	pData->m_elements.push_front(node);

	for (int i = 0; attr[i]; i += 2)
	{
		node->AddAttrib(StringUtil::stringW(std::string(attr[i])), StringUtil::stringW(std::string(attr[i + 1])));
	}
}

static void XMLCALL ExpatEnd(void* data, const char* el)
{
	CExpatData* pData = reinterpret_cast<CExpatData*>(data);
	if (!pData->m_data.empty())
	{
		pData->m_elements.front()->SetValue(StringUtil::stringW(pData->m_data));
		pData->m_data.clear();
	}
	pData->m_elements.pop_front();
}

static void XMLCALL ExpatHandleData(void* data, const char* content, int length)
{
	CExpatData* pData = reinterpret_cast<CExpatData*>(data);
	pData->m_data += std::string(content, length);
}


#elif __USE_POCO

static bool LoadXMLNode(ElementNode& node, Poco::XML::Document* pDoc, fmt::wmemory_buffer& ioErrMsg)
{
	node.clear();

	if (!pDoc)
		return false;

	Poco::XML::Node* pNode = pDoc->documentElement();
	if (!pNode)
		return false;

	node.SetName(StringUtil::stringW(pNode->nodeName()));
	ReadDoc(pDoc, pNode, node);
	return true;
}

#elif __USE_WX

static bool LoadXMLNode(ElementNode& node, wxXmlDocument& inSource, fmt::wmemory_buffer& ioErrMsg)
{
	node.clear();

	if (!inSource.GetRoot())
		return false;
	node.SetName(StringUtil::stringW(inSource.GetRoot()->GetName()));
	ReadDoc(inSource.GetRoot(), node);
	return true;
}

#else
#error No idea what XML parser you want!
#endif


bool ElementNode::LoadXML(std::istream& inStream, fmt::wmemory_buffer& ioErrMsg)
{
	if (!inStream.good())
		return false;

#if __USE_LIBXML2
	char buffer[1024];
	inStream.read(buffer, _countof(buffer));
	int res = static_cast<int>(inStream.gcount());
	if (0 >= res)
		return false;

	xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(nullptr, nullptr, buffer, res, nullptr);
	if (!ctxt)
		return false;

	for (;;)
	{
		inStream.read(buffer, _countof(buffer));
		res = static_cast<int>(inStream.gcount());
		if (0 >= res)
			break;
		xmlParseChunk(ctxt, buffer, res, 0);
	}
	xmlParseChunk(ctxt, buffer, 0, 1);

	xmlDocPtr source = nullptr;
	if (ctxt->wellFormed)
		source = ctxt->myDoc;
	xmlFreeParserCtxt(ctxt);
	ctxt = nullptr;

	bool rc = false;
	if (source)
	{
		rc = LoadXMLNode(*this, source, ioErrMsg);
		xmlFreeDoc(source);
	}
	return rc;

#elif __USE_EXPAT
	char buffer[8192];
	XML_Parser source = XML_ParserCreate(NULL);
	if (!source)
		return false;

	CExpatData data;
	data.m_root = this;
	XML_SetUserData(source, &data);
	XML_SetElementHandler(source, ExpatStart, ExpatEnd);
	XML_SetCharacterDataHandler(source, ExpatHandleData);

	bool bOk = true;
	while (bOk)
	{
		inStream.read(buffer, _countof(buffer));
		int len = static_cast<int>(inStream.gcount());
		if (0 >= len)
			break;

		if (XML_Parse(source, buffer, len, 0) == XML_STATUS_ERROR)
		{
			fmt::format_to(
				std::back_inserter(ioErrMsg),
				L"Parse error at line {}: {}",
				XML_GetCurrentLineNumber(source),
				XML_ErrorString(XML_GetErrorCode(source)));
			bOk = false;
			break;
		}
	}
	XML_ParserFree(source);
	return bOk;

#elif __USE_POCO

	try
	{
		Poco::XML::InputSource source(inStream);
		Poco::XML::DOMParser parser;
		Poco::XML::AutoPtr<Poco::XML::Document> pDoc = parser.parse(&source);
		pDoc->normalize();
		return LoadXMLNode(*this, pDoc, ioErrMsg);
	}
	catch (Poco::Exception& e)
	{
		std::wstring str = StringUtil::stringW(e.displayText());
		fmt::format_to(std::back_inserter(ioErrMsg), L"{}", str);
		return false;
	}

#elif __USE_WX
	wxLogBuffer* log = new wxLogBuffer();
	// wxLogChain will delete the log given to it.
	wxLogChain chain(log);
	chain.PassMessages(false);
	chain.DisableTimestamp();

	wxInputStdStream stream(inStream);
	wxXmlDocument source;
	if (!source.Load(stream))
	{
		fmt::format_to(std::back_inserter(ioErrMsg), L"{}", log->GetBuffer().wx_str());
		// This does not call Flush (which displays a dialog). Yea!
		chain.SetLog(nullptr);
		return false;
	}
	return LoadXMLNode(*this, source, ioErrMsg);

#else
#error No idea what XML parser you want!
#endif
}


bool ElementNode::LoadXML(char const* inData, size_t nData, fmt::wmemory_buffer& ioErrMsg)
{
	if (!inData || 0 == nData)
		return false;
#if __USE_LIBXML2
	xmlDocPtr source = xmlReadMemory(inData, static_cast<int>(nData), nullptr, nullptr, 0);
	bool rc = LoadXMLNode(*this, source, ioErrMsg);
	if (source)
		xmlFreeDoc(source);
	return rc;
#else
	std::istringstream input(std::string(inData, nData));
	return LoadXML(input, ioErrMsg);
#endif
}


bool ElementNode::LoadXML(wchar_t const* inFileName, fmt::wmemory_buffer& ioErrMsg)
{
	if (!inFileName)
		return false;
#if __USE_LIBXML2
	std::string filename(StringUtil::stringA(std::wstring(inFileName)));
	xmlDocPtr source = xmlReadFile(filename.c_str(), nullptr, 0);
	bool rc = LoadXMLNode(*this, source, ioErrMsg);
	if (source)
		xmlFreeDoc(source);
	return rc;
#else
#ifdef ARB_HAS_ISTREAM_WCHAR
	std::ifstream input(inFileName);
#else
	std::string filename(StringUtil::stringA(std::wstring(inFileName)));
	std::ifstream input(filename.c_str());
#endif
	if (!input.good())
		return false;
	return LoadXML(input, ioErrMsg);
#endif
}


bool ElementNode::SaveXML(std::ostream& outOutput) const
{
	std::string dtd;
	return SaveXML(outOutput, dtd);
}


#if __USE_LIBXML2
static int BufferWriteCallback(void* context, const char* buffer, int len)
{
	if (!context)
		return -1;
	std::ostream* output = reinterpret_cast<std::ostream*>(context);
	output->write(buffer, len);
	return len;
}
static int BufferCloseCallback(void* context)
{
	// don't close
	return 0;
}
#endif


bool ElementNode::SaveXML(std::ostream& outOutput, std::string const& inDTD) const
{
#if __USE_LIBXML2
	// On Win32, an XMLCh is a UNICODE character.
	outOutput << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	if (!inDTD.empty())
	{
#ifdef UNICODE
		std::string rootName = StringUtil::stringA(GetName());
#else
		std::string rootName = GetName();
#endif
		outOutput << "<!DOCTYPE " << rootName << " [\n";
		outOutput << inDTD;
		outOutput << "\n]>\n";
	}
	xmlOutputBufferPtr outputBuffer
		= xmlOutputBufferCreateIO(BufferWriteCallback, BufferCloseCallback, (void*)(&outOutput), nullptr);
	xmlTextWriterPtr formatter = xmlNewTextWriter(outputBuffer);
	xmlTextWriterSetIndent(formatter, 2); // Only puts out 1 space no matter what.
	xmlTextWriterSetIndentString(formatter, BAD_CAST "\t");
	XMLstring name(GetName());
	xmlTextWriterStartElement(formatter, name.c_str());
	xmlOutputBufferFlush(outputBuffer);
	CreateDoc(formatter, outputBuffer, *this);
	xmlTextWriterEndElement(formatter);
	xmlOutputBufferFlush(outputBuffer);
	xmlFreeTextWriter(formatter);
	return true;

#elif __USE_EXPAT
#pragma PRAGMA_TODO(expat writer)
	// Expat is only a reader. We have to write data ourselves.
	return false;

#elif __USE_POCO
	int optionsWriter = Poco::XML::XMLWriter::PRETTY_PRINT;
	if (inDTD.empty())
	{
		optionsWriter |= Poco::XML::XMLWriter::WRITE_XML_DECLARATION;
	}
	else
	{
		outOutput << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
#ifdef UNICODE
		std::string rootName = StringUtil::stringA(GetName());
#else
		std::string rootName = GetName();
#endif
		outOutput << "<!DOCTYPE " << rootName << " [\n";
		outOutput << inDTD;
		outOutput << "\n]>\n";
	}

	Poco::XML::DOMWriter writer;
	Poco::UTF8Encoding encoding;
	writer.setEncoding("utf-8", encoding);
	writer.setNewLine("\n");
	writer.setIndent("  ");
	writer.setOptions(optionsWriter);

	Poco::XML::AutoPtr<Poco::XML::Document> pDoc = new Poco::XML::Document();
	Poco::XML::AutoPtr<Poco::XML::Element> pRoot = pDoc->createElement(StringUtil::stringA(GetName()));
	pDoc->appendChild(pRoot);
	CreateDoc(pDoc, pRoot, *this);
	writer.writeNode(outOutput, pDoc);
	return true;

#elif __USE_WX
	wxXmlDocument doc;
	doc.SetVersion(L"1.0");
	doc.SetFileEncoding(L"utf-8");
	wxXmlNode* root = new wxXmlNode(nullptr, wxXML_ELEMENT_NODE, StringUtil::stringWX(GetName()));
	doc.SetRoot(root);
	// TODO: Insert DTD
	CreateDoc(root, *this);
	wxOutputStdStream out(outOutput);
	return doc.Save(out);

#else
#error No idea what XML parser you want!
#endif
}


bool ElementNode::SaveXML(std::wstring const& outFile) const
{
	std::string dtd;
	return SaveXML(outFile, dtd);
}


bool ElementNode::SaveXML(std::wstring const& outFile, std::string const& inDTD) const
{
	bool bOk = false;
	if (outFile.empty())
		return bOk;
#if 1
#if defined(ARB_HAS_OSTREAM_WCHAR)
	std::ofstream output(outFile.c_str(), std::ios::out | std::ios::binary);
#else
	char const* pFile = nullptr;
	std::string filename = StringUtil::stringA(outFile);
	pFile = filename.c_str();
	std::ofstream output(pFile, std::ios::out | std::ios::binary);
#endif
	output.exceptions(std::ios_base::badbit);
	if (output.is_open())
	{
		bOk = SaveXML(output, inDTD);
		output.close();
	}
#else
	wxFFileOutputStream output(outFile.c_str(), L"wb");
	if (output.IsOk())
	{
		bOk = SaveXML(output, inDTD);
		output.Close();
	}
#endif
	return bOk;
}

/////////////////////////////////////////////////////////////////////////////

namespace
{
class ElementText_concrete : public ElementText
{
public:
	ElementText_concrete()
	{
	}
	ElementText_concrete(std::wstring const& inText)
		: ElementText(inText)
	{
	}
};
} // namespace


ElementTextPtr ElementText::New()
{
	return std::make_shared<ElementText_concrete>();
}


ElementTextPtr ElementText::New(std::wstring const& inText)
{
	return std::make_shared<ElementText_concrete>(inText);
}


ElementText::ElementText()
	: m_Value()
{
}


ElementText::ElementText(std::wstring const& inText)
	: m_Value(inText)
{
}


void ElementText::Dump(int inLevel) const
{
	fmt::wmemory_buffer msg;
	fmt::format_to(std::back_inserter(msg), L"{}{}", GetIndentBuffer(inLevel), GetName());
	if (0 < m_Value.length())
	{
		fmt::format_to(std::back_inserter(msg), L": {}", m_Value);
	}
	LogMessage(msg);
}


ARBElementType ElementText::GetType() const
{
	return ARBElementType::Text;
}


std::wstring const& ElementText::GetName() const
{
	static const std::wstring name(L"#text");
	return name;
}


void ElementText::SetName(std::wstring const& /*inName*/)
{
}


std::wstring ElementText::GetValue() const
{
	return m_Value;
}


void ElementText::SetValue(std::wstring const& inValue)
{
	m_Value = inValue;
}


void ElementText::SetValue(wchar_t const* const inValue)
{
	if (inValue)
		m_Value = inValue;
	else
		m_Value.clear();
}


void ElementText::SetValue(short inValue)
{
	m_Value = fmt::format(L"{}", inValue);
}


void ElementText::SetValue(unsigned short inValue)
{
	m_Value = fmt::format(L"{}", inValue);
}


void ElementText::SetValue(long inValue)
{
	m_Value = fmt::format(L"{}", inValue);
}


void ElementText::SetValue(unsigned long inValue)
{
	m_Value = fmt::format(L"{}", inValue);
}


void ElementText::SetValue(double inValue, int inPrec)
{
	m_Value = ARBDouble::ToString(inValue, inPrec, false);
}
