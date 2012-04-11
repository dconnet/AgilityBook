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

#include "ARBDate.h"
#include "ARBStructure.h"
#include "ARBTypes.h"

#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/wfstream.h>
#include <wx/xml/xml.h>
#pragma message ( "Compiling with wxWidgets " wxVERSION_NUM_DOT_STRING )

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif

////////////////////////////////////////////////////////////////////////////

bool Element::Initialize(wxString& outMsg)
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
#if wxCHECK_VERSION(2, 9, 3)
	wxXmlAttribute* attribs = node->GetAttributes();
#else
	wxXmlProperty* attribs = node->GetProperties();
#endif
	while (attribs)
	{
		tree->AddAttrib(attribs->GetName(), attribs->GetValue());
		attribs = attribs->GetNext();
	}
	wxString content = node->GetNodeContent();
	if (!content.empty())
		tree->SetValue(content);
	wxXmlNode* child = node->GetChildren();
	while (child)
	{
		if (wxXML_ELEMENT_NODE == child->GetType())
		{
			ElementNodePtr subtree = tree->AddElementNode(child->GetName());
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
		wxString name, value;
		toWrite.GetNthAttrib(i, name, value);
#if wxCHECK_VERSION(2, 9, 3)
		node->AddAttribute(name, value);
#else
		node->AddProperty(name, value);
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
				wxXmlNode* child = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, element->GetName());
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
				wxXmlNode* child = new wxXmlNode(NULL, wxXML_TEXT_NODE, element->GetName(), element->GetValue());
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


ElementNodePtr ElementNode::New(wxString const& inText)
{
	ElementNodePtr pNode(new ElementNode(inText));
	pNode->m_Me = pNode;
	return pNode;
}


ElementNode::ElementNode()
{
}


ElementNode::ElementNode(wxString const& inName)
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
	wxString msg = wxString::Format(wxT("%*s%s"), inLevel, wxT(" "), m_Name.c_str());
	for (i = 0; i < GetAttribCount(); ++i)
	{
		wxString name, value;
		GetNthAttrib(i, name, value);
		msg << wxT(" ")
			<< name
			<< wxT("=\"")
			<< value
			<< wxT("\"");
	}
	wxLogMessage(wxT("%s"), msg.c_str());
	for (i = 0; i < GetElementCount(); ++i)
	{
		GetElement(i)->Dump(inLevel+1);
	}
}


Element::ElementType ElementNode::GetType() const
{
	return Element::Element_Node;
}


wxString const& ElementNode::GetName() const
{
	return m_Name;
}


void ElementNode::SetName(wxString const& inName)
{
	m_Name = inName;
}


wxString ElementNode::GetValue() const
{
	wxString value;
	for (int i = 0; i < GetElementCount(); ++i)
	{
		if (Element::Element_Text == GetElement(i)->GetType())
			value += GetElement(i)->GetValue();
	}
	return value;
}


void ElementNode::SetValue(wxString const& inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(wxChar const* const inValue)
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


void ElementNode::SetValue(long inValue)
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
		wxString& outName,
		wxString& outValue) const
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
		wxString const& inName,
		wxString& outValue) const
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
		wxString const& inName,
		ARBVersion& outValue) const
{
	wxString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		unsigned short major = 0;
		unsigned short minor = 0;
		wxString::size_type pos = value.find('.');
		if (wxString::npos != pos)
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
		wxString const& inName,
		ARBDate& outValue) const
{
	wxString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		ARBDate date = ARBDate::FromString(value.wx_str(), ARBDate::eDashYMD);
		if (date.IsValid())
			outValue = date;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		wxString const& inName,
		bool& outValue) const
{
	wxString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (value == wxT("y"))
			outValue = true;
		else if (value == wxT("n"))
			outValue = false;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		wxString const& inName,
		short& outValue) const
{
	wxString value;
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
		wxString const& inName,
		long& outValue) const
{
	wxString value;
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
		wxString const& inName,
		double& outValue) const
{
	wxString value;
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
		wxString const& inName,
		wxString const& inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = inValue;
	return true;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		wxChar const* const inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = inValue;
	else
		m_Attribs[inName] = wxT("");
	return true;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		ARBVersion const& inValue)
{
	return AddAttrib(inName, inValue.str());
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		ARBDate const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.GetString(ARBDate::eDashYMD));
	return false;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		bool inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = wxT("y");
	else
		m_Attribs[inName] = wxT("n");
	return true;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		short inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] << inValue;
	return true;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		long inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] << inValue;
	return true;
}


bool ElementNode::AddAttrib(
		wxString const& inName,
		double inValue,
		int inPrec)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = ARBDouble::ToString(inValue, inPrec, ARBDouble::eNone);
	return true;
}


bool ElementNode::RemoveAttrib(wxString const& inName)
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
		wxString const& inName,
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
		wxString const& inText,
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
		wxString const& inName,
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
		wxString const& inName,
		wxString const* inValue) const
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
		wxString& ioErrMsg)
{
	node->clear();

	if (!inSource.GetRoot())
		return false;
	node->SetName(inSource.GetRoot()->GetName());
	ReadDoc(inSource.GetRoot(), node);
	return true;
}


bool ElementNode::LoadXML(
		wxInputStream& inStream,
		wxString& ioErrMsg)
{
	wxXmlDocument source;
	if (!source.Load(inStream))
		return false;
	return LoadXMLNode(m_Me.lock(), source, ioErrMsg);
}


bool ElementNode::LoadXML(
		char const* inData,
		size_t nData,
		wxString& ioErrMsg)
{
	wxXmlDocument source;
	wxMemoryInputStream input(inData, nData);
	return LoadXML(input, ioErrMsg);
}


bool ElementNode::LoadXML(
		wxChar const* inFileName,
		wxString& ioErrMsg)
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
	doc.SetVersion(wxT("1.0"));
	doc.SetFileEncoding(wxT("utf-8"));
	wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, GetName());
	doc.SetRoot(root);
	// TODO: Insert DTD
	CreateDoc(root, *this);
	return doc.Save(outOutput);
}


bool ElementNode::SaveXML(wxString const& outFile) const
{
	std::string dtd;
	return SaveXML(outFile, dtd);
}


bool ElementNode::SaveXML(
		wxString const& outFile,
		std::string const& inDTD) const
{
	bool bOk = false;
	if (outFile.empty())
		return bOk;
	wxFFileOutputStream output(outFile, wxT("wb"));
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


ElementTextPtr ElementText::New(wxString const& inText)
{
	ElementTextPtr pText(new ElementText(inText));
	pText->m_Me = pText;
	return pText;
}


ElementText::ElementText()
	: m_Value()
{
}


ElementText::ElementText(wxString const& inText)
	: m_Value(inText)
{
}


void ElementText::Dump(int inLevel) const
{
	wxString msg = wxString::Format(wxT("%*s%s"), inLevel, wxT(" "), GetName().c_str());
	if (0 < m_Value.length())
	{
		msg << wxT(": ") << m_Value;
	}
	wxLogMessage(wxT("%s"), msg.c_str());
}


Element::ElementType ElementText::GetType() const
{
	return Element::Element_Text;
}


wxString const& ElementText::GetName() const
{
	static const wxString name(wxT("#text"));
	return name;
}


void ElementText::SetName(wxString const& /*inName*/)
{
}


wxString ElementText::GetValue() const
{
	return m_Value;
}


void ElementText::SetValue(wxString const& inValue)
{
	m_Value = inValue;
}


void ElementText::SetValue(wxChar const* const inValue)
{
	if (inValue)
		m_Value = inValue;
	else
		m_Value.erase();
}


void ElementText::SetValue(short inValue)
{
	m_Value << inValue;
}


void ElementText::SetValue(long inValue)
{
	m_Value << inValue;
}


void ElementText::SetValue(
		double inValue,
		int inPrec)
{
	m_Value = ARBDouble::ToString(inValue, inPrec, ARBDouble::eNone);
}
