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
#include <fstream>
#include <list>
#include <map>
#include <sstream>

#include "ARBCommon/ARBDate.h"
#include "ARBCommon/ARBTypes.h"
#include "ARBCommon/StringUtil.h"

#if !defined(USE_LIBXML2)
#if defined(__WXWINDOWS__)
#define USE_LIBXML2		0
#else
#error No idea what XML parser you want!
#endif
#endif

#if USE_LIBXML2
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
#pragma message ( "Compiling with libxml2 " LIBXML_DOTTED_VERSION )
#endif

#else // USE_LIBXML2
#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>
#pragma message ( "Compiling with wxWidgets " wxVERSION_NUM_DOT_STRING )
#endif // USE_LIBXML2

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

#ifndef _WIN32
#include <iostream>
static void OutputDebugString(wchar_t const* msg)
{
	if (msg)
		std::wcout << msg;
}
#endif

////////////////////////////////////////////////////////////////////////////

#if defined(__WXWINDOWS__)

class wxInputStdStream : public wxInputStream
{
public:
	wxInputStdStream(std::istream& stream) :
		m_stream(stream)
	{
	}

	virtual size_t OnSysRead(void* buffer, size_t size)
	{
		size_t count = 0;
		if (m_stream.good())
		{
			m_stream.read((char*)buffer, size);
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
	wxOutputStdStream(std::ostream& stream) :
		m_stream(stream)
	{
	}

	virtual size_t OnSysWrite(const void* buffer, size_t size)
	{
		size_t count = 0;
		if (m_stream.good())
		{
			m_stream.write((char*)buffer, size);
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
	outMsg.erase();
#if USE_LIBXML2
	xmlInitParser();
	xmlSubstituteEntitiesDefault(1);
#endif
	return true;
}


void Element::Terminate()
{
#if USE_LIBXML2
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

#if USE_LIBXML2

class XMLstring : public std::basic_string<xmlChar>
{
public:
	XMLstring() {}
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
	StringDOM() {}
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


static void ReadDoc(xmlNode* inNode, ElementNodePtr tree)
{
	if (!inNode)
		return;
	for (xmlAttr* attrib = inNode->properties; attrib; attrib = attrib->next)
	{
		tree->AddAttrib(StringDOM(attrib->name), StringDOM(attrib->children->content));
	}
	for (xmlNode* child = inNode->children; child; child = child->next)
	{
		if (child->type == XML_ELEMENT_NODE)
		{
			if (tree->HasTextNodes())
				tree->RemoveAllElements();
			ElementNodePtr subtree = tree->AddElementNode(StringDOM(child->name));
			ReadDoc(child, subtree);
		}
		else if (child->type == XML_TEXT_NODE)
		{
			if (tree->HasTextNodes() || 0 == tree->GetElementCount())
				tree->SetValue(StringDOM(child->content));
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
		case Element::Element_Node:
			{
				XMLstring name(element->GetName());
				xmlTextWriterStartElement(formatter, name.c_str());
				xmlOutputBufferFlush(outputBuffer);
				CreateDoc(formatter, outputBuffer, *(dynamic_cast<ElementNode*>(element.get())));
				xmlTextWriterEndElement(formatter);
				xmlOutputBufferFlush(outputBuffer);
			}
			break;
		case Element::Element_Text:
			{
				XMLstring value(element->GetValue());
				xmlTextWriterWriteString(formatter, value.c_str());
				xmlOutputBufferFlush(outputBuffer);
			}
			break;
		}
	}
}

#else // USE_LIBXML2

static void ReadDoc(wxXmlNode* node, ElementNodePtr tree)
{
#if wxCHECK_VERSION(3, 0, 0)
	wxXmlAttribute* attribs = node->GetAttributes();
#else
	wxXmlProperty* attribs = node->GetProperties();
#endif
	while (attribs)
	{
		tree->AddAttrib(StringUtil::stringW(attribs->GetName()), StringUtil::stringW(attribs->GetValue()));
		attribs = attribs->GetNext();
	}
	std::wstring content = StringUtil::stringW(node->GetNodeContent());
	if (!content.empty())
		tree->SetValue(content);
	wxXmlNode* child = node->GetChildren();
	while (child)
	{
		if (wxXML_ELEMENT_NODE == child->GetType())
		{
			ElementNodePtr subtree = tree->AddElementNode(StringUtil::stringW(child->GetName()));
			ReadDoc(child, subtree);
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
#if wxCHECK_VERSION(3, 0, 0)
		node->AddAttribute(name, value);
#else
		node->AddProperty(StringUtil::stringWX(name), StringUtil::stringWX(value));
#endif
	}
	int count = toWrite.GetElementCount();
	wxXmlNode* lastChild = NULL;
	for (i = 0; i < count; ++i)
	{
		ElementPtr element = toWrite.GetElement(i);
		switch (element->GetType())
		{
		case Element::Element_Node:
			{
				wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, StringUtil::stringWX(element->GetName()));
				if (lastChild)
					lastChild->SetNext(child);
				else
					node->AddChild(child);
				lastChild = child;
				CreateDoc(child, *(dynamic_cast<ElementNode*>(element.get())));
			}
			break;
		case Element::Element_Text:
			{
				wxXmlNode* child = new wxXmlNode(NULL, wxXML_TEXT_NODE, StringUtil::stringWX(element->GetName()), StringUtil::stringWX(element->GetValue()));
				node->AddChild(child);
			}
			break;
		}
	}
}

#endif // USE_LIBXML2

/////////////////////////////////////////////////////////////////////////////

static std::wstring GetIndentBuffer(int indent)
{
	std::wstring str;
	if (0 < indent)
	{
		wchar_t* buffer = new wchar_t[indent + 1];
		swprintf(buffer, indent + 1, L"%*s", indent, L" ");
		str = buffer;
		delete [] buffer;
	}
	return str;
}

static void LogMessage(	std::wostringstream& msg)
{
#if defined(__WXWINDOWS__)
	wxLogMessage(L"%s", msg.str().c_str());
#else
	msg << L"\n";
	OutputDebugString(msg.str().c_str());
#endif
}

/////////////////////////////////////////////////////////////////////////////

ElementNodePtr ElementNode::New()
{
	ElementNodePtr pNode(new ElementNode());
	pNode->m_Me = pNode;
	return pNode;
}


ElementNodePtr ElementNode::New(std::wstring const& inText)
{
	ElementNodePtr pNode(new ElementNode(inText));
	pNode->m_Me = pNode;
	return pNode;
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
	for (std::vector<ElementPtr>::iterator i = m_Elements.begin();
		i != m_Elements.end();
		)
	{
		if (Element::Element_Text == (*i)->GetType())
			i = m_Elements.erase(i);
		else
			++i;
	}
}


void ElementNode::Dump(int inLevel) const
{
	int i;
	std::wostringstream msg;
	msg << GetIndentBuffer(inLevel) << m_Name;
	for (i = 0; i < GetAttribCount(); ++i)
	{
		std::wstring name, value;
		GetNthAttrib(i, name, value);
		msg << L" "
			<< name
			<< L"=\""
			<< value
			<< L"\"";
	}
	LogMessage(msg);
	for (i = 0; i < GetElementCount(); ++i)
	{
		GetElement(i)->Dump(inLevel+1);
	}
}


Element::ElementType ElementNode::GetType() const
{
	return Element::Element_Node;
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
		if (Element::Element_Text == GetElement(i)->GetType())
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
	m_Name.erase();
	m_Attribs.clear();
	m_Elements.clear();
}


int ElementNode::GetAttribCount() const
{
	return static_cast<int>(m_Attribs.size());
}


ElementNode::AttribLookup ElementNode::GetNthAttrib(
		int inIndex,
		std::wstring& outName,
		std::wstring& outValue) const
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
		return eFound;
	}
	else
		return eNotFound;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		std::wstring& outValue) const
{
	MyAttributes::const_iterator iter = m_Attribs.find(inName);
	if (iter != m_Attribs.end())
	{
		outValue = (*iter).second;
		return eFound;
	}
	else
		return eNotFound;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		ARBVersion& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
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
		outValue = ARBVersion(major, minor);
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		ARBDate& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		ARBDate date = ARBDate::FromString(value, ARBDate::eDashYMD);
		if (date.IsValid())
			outValue = date;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		bool& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (value == L"y")
			outValue = true;
		else if (value == L"n")
			outValue = false;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		short& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = static_cast<short>(StringUtil::ToCLong(value));
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		unsigned short& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = static_cast<unsigned short>(StringUtil::ToCULong(value));
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		long& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCLong(value, outValue))
				rc = eInvalidValue;
		}
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		unsigned long& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCULong(value, outValue))
				rc = eInvalidValue;
		}
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		std::wstring const& inName,
		double& outValue) const
{
	std::wstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
		{
			if (!StringUtil::ToCDouble(value, outValue))
				rc = eInvalidValue;
		}
		else
			rc = eInvalidValue;
	}
	return rc;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		std::wstring const& inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = inValue;
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		wchar_t const* const inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = inValue;
	else
		m_Attribs[inName] = std::wstring();
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		ARBVersion const& inValue)
{
	return AddAttrib(inName, inValue.str());
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		ARBDate const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.GetString(ARBDate::eDashYMD));
	return false;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		bool inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = L"y";
	else
		m_Attribs[inName] = L"n";
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		short inValue)
{
	if (inName.empty())
		return false;
	std::wostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		unsigned short inValue)
{
	if (inName.empty())
		return false;
	std::wostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		long inValue)
{
	if (inName.empty())
		return false;
	std::wostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		unsigned long inValue)
{
	if (inName.empty())
		return false;
	std::wostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		std::wstring const& inName,
		double inValue,
		int inPrec)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = ARBDouble::ToString(inValue, inPrec, ARBDouble::eNone);
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


int ElementNode::GetNodeCount(ElementType type) const
{
	int nCount = 0;
	for (std::vector<ElementPtr>::const_iterator iter = m_Elements.begin();
		iter != m_Elements.end();
		++iter)
	{
		if (type == (*iter)->GetType())
			++nCount;
	}
	return nCount;
}


bool ElementNode::HasTextNodes() const
{
	for (std::vector<ElementPtr>::const_iterator iter = m_Elements.begin();
		iter != m_Elements.end();
		++iter)
	{
		if (Element::Element_Text == (*iter)->GetType())
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
	return std::tr1::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
}


ElementNodePtr ElementNode::GetElementNode(int inIndex)
{
	return std::tr1::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
}


ElementNodePtr ElementNode::GetNthElementNode(int inIndex) const
{
	return const_cast<ElementNode*>(this)->GetNthElementNode(inIndex);
}


ElementNodePtr ElementNode::GetNthElementNode(int inIndex)
{
	int index = -1;
	int nElements = static_cast<int>(m_Elements.size());
	for (int iElement = 0; iElement < nElements; ++iElement)
	{
		if (Element::Element_Node == m_Elements[iElement]->GetType())
		{
			++index;
			if (index == inIndex)
				return GetElementNode(iElement);
		}
	}
	return ElementNodePtr();
}


ElementNodePtr ElementNode::AddElementNode(
		std::wstring const& inName,
		int inAt)
{
	size_t index;
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


ElementTextPtr ElementNode::AddElementText(
		std::wstring const& inText,
		int inAt)
{
	assert(0 == m_Value.length());
	size_t index;
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


int ElementNode::FindElement(
		std::wstring const& inName,
		int inStartFrom) const
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
		ElementNodePtr& outParentNode,
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
		if (element->GetName() == inName &&
		(!inValue || (inValue && element->GetValue() == *inValue)))
		{
			outParentNode = m_Me.lock();
			outElementIndex = i;
			return true;
		}
		else if (element->FindElementDeep(outParentNode, outElementIndex, inName, inValue))
			return true;
	}
	return false;
}


#if USE_LIBXML2
static bool LoadXMLNode(
		ElementNodePtr node,
		xmlDocPtr inSource,
		std::wostringstream& ioErrMsg)
{
	node->clear();

	xmlNode* root = xmlDocGetRootElement(inSource);
	if (!root)
		return false;
	node->SetName(StringDOM(root->name));
	ReadDoc(root, node);
	return true;
}

#else

static bool LoadXMLNode(
		ElementNodePtr node,
		wxXmlDocument& inSource,
		std::wostringstream& ioErrMsg)
{
	node->clear();

	if (!inSource.GetRoot())
		return false;
	node->SetName(StringUtil::stringW(inSource.GetRoot()->GetName()));
	ReadDoc(inSource.GetRoot(), node);
	return true;
}
#endif


bool ElementNode::LoadXML(
		std::istream& inStream,
		std::wostringstream& ioErrMsg)
{
#if USE_LIBXML2
	if (!inStream.good())
		return false;

	char buffer[1024];
	inStream.read(buffer, ARRAYSIZE(buffer));
	int res = static_cast<int>(inStream.gcount());
	if (0 >= res)
		return false;

	xmlParserCtxtPtr ctxt = xmlCreatePushParserCtxt(NULL, NULL, buffer, res, NULL);
	if (!ctxt)
		return false;

	for (;;)
	{
		inStream.read(buffer, ARRAYSIZE(buffer));
		res = static_cast<int>(inStream.gcount());
		if (0 >= res)
			break;
		xmlParseChunk(ctxt, buffer, res, 0);
	}
	xmlParseChunk(ctxt, buffer, 0, 1);

	xmlDocPtr source = NULL;
	if (ctxt->wellFormed)
		source = ctxt->myDoc;
	xmlFreeParserCtxt(ctxt);
	ctxt = NULL;

	bool rc = false;
	if (source)
	{
		rc = LoadXMLNode(m_Me.lock(), source, ioErrMsg);
		xmlFreeDoc(source);
	}
	return rc;

#else
	wxLogBuffer* log = new wxLogBuffer();
	// wxLogChain will delete the log given to it.
	wxLogChain chain(log);
	chain.PassMessages(false);
#if wxCHECK_VERSION(3, 0, 0)
	chain.DisableTimestamp();
#endif

	wxInputStdStream stream(inStream);
	wxXmlDocument source;
	if (!source.Load(stream))
	{
		ioErrMsg << StringUtil::stringW(log->GetBuffer());
		// This does not call Flush (which displays a dialog). Yea!
		chain.SetLog(NULL);
		return false;
	}
	return LoadXMLNode(m_Me.lock(), source, ioErrMsg);
#endif
}


bool ElementNode::LoadXML(
		char const* inData,
		size_t nData,
		std::wostringstream& ioErrMsg)
{
	if (!inData || 0 == nData)
		return false;
#if USE_LIBXML2
	xmlDocPtr source = xmlReadMemory(inData, static_cast<int>(nData), NULL, NULL, 0);
	bool rc = LoadXMLNode(m_Me.lock(), source, ioErrMsg);
	if (source)
		xmlFreeDoc(source);
	return rc;
#else
	std::istringstream input(std::string(inData, nData));
	return LoadXML(input, ioErrMsg);
#endif
}


bool ElementNode::LoadXML(
		wchar_t const* inFileName,
		std::wostringstream& ioErrMsg)
{
	if (!inFileName)
		return false;
#if USE_LIBXML2
	std::string filename = StringUtil::stringA(inFileName);
	xmlDocPtr source = xmlReadFile(filename.c_str(), NULL, 0);
	bool rc = LoadXMLNode(m_Me.lock(), source, ioErrMsg);
	if (source)
		xmlFreeDoc(source);
	return rc;
#else
#ifdef ARB_HAS_ISTREAM_WCHAR
	std::ifstream input(inFileName);
#else
	std::string filename(StringUtil::stringA(inFileName));
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


#if USE_LIBXML2
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


bool ElementNode::SaveXML(
		std::ostream& outOutput,
		std::string const& inDTD) const
{
#if USE_LIBXML2
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
	xmlOutputBufferPtr outputBuffer = xmlOutputBufferCreateIO(BufferWriteCallback, BufferCloseCallback, (void*)(&outOutput), NULL);
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
#else
	wxXmlDocument doc;
	doc.SetVersion(L"1.0");
	doc.SetFileEncoding(L"utf-8");
	wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, StringUtil::stringWX(GetName()));
	doc.SetRoot(root);
	// TODO: Insert DTD
	CreateDoc(root, *this);
	wxOutputStdStream out(outOutput);
	return doc.Save(out);
#endif
}


bool ElementNode::SaveXML(std::wstring const& outFile) const
{
	std::string dtd;
	return SaveXML(outFile, dtd);
}


bool ElementNode::SaveXML(
		std::wstring const& outFile,
		std::string const& inDTD) const
{
	bool bOk = false;
	if (outFile.empty())
		return bOk;
#if 1
#if defined(ARB_HAS_OSTREAM_WCHAR)
	std::ofstream output(outFile.c_str(), std::ios::out | std::ios::binary);
#else
	char const* pFile = NULL;
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

ElementTextPtr ElementText::New()
{
	ElementTextPtr pText(new ElementText());
	pText->m_Me = pText;
	return pText;
}


ElementTextPtr ElementText::New(std::wstring const& inText)
{
	ElementTextPtr pText(new ElementText(inText));
	pText->m_Me = pText;
	return pText;
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
	std::wostringstream msg;
	msg << GetIndentBuffer(inLevel) << GetName();
	if (0 < m_Value.length())
	{
		msg << L": " << m_Value;
	}
	LogMessage(msg);
}


Element::ElementType ElementText::GetType() const
{
	return Element::Element_Text;
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
		m_Value.erase();
}


void ElementText::SetValue(short inValue)
{
	std::wostringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(unsigned short inValue)
{
	std::wostringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(long inValue)
{
	std::wostringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(unsigned long inValue)
{
	std::wostringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(
		double inValue,
		int inPrec)
{
	m_Value = ARBDouble::ToString(inValue, inPrec, ARBDouble::eNone);
}
