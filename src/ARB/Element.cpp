/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 *
 * @brief Tree structure to store XML.
 * @author David Connet
 *
 * Actual reading and writing of XML is done using Xerces (or wxWidgets)
 *
 * Revision History
 * @li 2012-04-10 DRC Based on wx-group thread, use std::string for internal use
 * @li 2012-03-16 DRC Renamed LoadXML functions, added stream version.
 * @li 2009-09-13 DRC Add support for wxWidgets 2.9, deprecate tstring.
 * @li 2009-03-12 DRC Converting all TCHAR stuff to wxWidgets
 * @li 2009-02-08 DRC Fix wxWidget xml creation
 * @li 2009-01-05 DRC Added libxml2 support
 * @li 2008-12-27 DRC Added wxWidget support (xml)
 * @li 2008-11-02 DRC Added xerces 3.0 support
 * @li 2007-09-06 DRC Added GetNthElementNode
 * @li 2007-08-15 DRC Modified to support mixed text/nodes.
 * @li 2007-08-08 DRC Moved initialization here, so all XML usage is contained.
 * @li 2007-03-37 DRC Fixed a problem releasing transcoded data.
 * @li 2005-06-09 DRC Numbers were being stored/shown in scientific notation.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Moved date parsing code to ARBDate::FromString.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added a DTD parameter to SaveXML.
 */

#include "stdafx.h"
#include "Element.h"
#include <list>
#include <map>
#include <sstream>

#include "ARBDate.h"
#include "ARBString.h"
#include "ARBStructure.h"
#include "ARBTypes.h"

#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>
#pragma message ( "Compiling with wxWidgets " wxVERSION_NUM_DOT_STRING )

#if defined(__WXMSW__)
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

bool Element::Initialize(std::wstring& outMsg)
{
	outMsg.erase();
	return true;
}


void Element::Terminate()
{
}


Element::Element()
{
}


Element::~Element()
{
}


////////////////////////////////////////////////////////////////////////////

static void ReadDoc(wxXmlNode* node, ElementNodePtr tree)
{
#if wxCHECK_VERSION(2, 9, 4)
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
#if wxCHECK_VERSION(2, 9, 4)
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
	msg << StringUtil::stringW(wxString::Format(L"%*s%s", inLevel, L" ", m_Name.c_str()));
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
	wxLogMessage(L"%s", msg.str().c_str());
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


bool ElementNode::LoadXML(
		wxInputStream& inStream,
		std::wostringstream& ioErrMsg)
{
	wxXmlDocument source;
	if (!source.Load(inStream))
		return false;
	return LoadXMLNode(m_Me.lock(), source, ioErrMsg);
}


bool ElementNode::LoadXML(
		char const* inData,
		size_t nData,
		std::wostringstream& ioErrMsg)
{
	wxXmlDocument source;
	wxMemoryInputStream input(inData, nData);
	return LoadXML(input, ioErrMsg);
}


bool ElementNode::LoadXML(
		wchar_t const* inFileName,
		std::wostringstream& ioErrMsg)
{
	wxXmlDocument source;
	wxFileInputStream stream(inFileName);
	if (!stream.IsOk())
		return false;
	return LoadXML(stream, ioErrMsg);
}


bool ElementNode::SaveXML(wxOutputStream& outOutput) const
{
	std::string dtd;
	return SaveXML(outOutput, dtd);
}


bool ElementNode::SaveXML(
		wxOutputStream& outOutput,
		std::string const& inDTD) const
{
	wxXmlDocument doc;
	doc.SetVersion(L"1.0");
	doc.SetFileEncoding(L"utf-8");
	wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, StringUtil::stringWX(GetName()));
	doc.SetRoot(root);
	// TODO: Insert DTD
	CreateDoc(root, *this);
	return doc.Save(outOutput);
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
	wxFFileOutputStream output(outFile.c_str(), L"wb");
	if (output.IsOk())
	{
		bOk = SaveXML(output, inDTD);
		output.Close();
	}
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
	msg << StringUtil::stringW(wxString::Format(L"%*s%s", inLevel, L" ", GetName().c_str()));
	if (0 < m_Value.length())
	{
		msg << L": " << m_Value;
	}
	wxLogMessage(L"%s", msg.str().c_str());
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
