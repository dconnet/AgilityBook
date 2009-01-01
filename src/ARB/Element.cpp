/*
 * Copyright © 2002-2009 David Connet. All Rights Reserved.
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
 * @brief Tree structure to store XML.
 * @author David Connet
 *
 * Actual reading and writing of XML is done using Xerces.
 *
 * Revision History
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
#include <fstream>
#include <iostream>
#include <list>
#include <map>

#include "ARBDate.h"
#include "ARBStructure.h"
#include "ARBTypes.h"

#ifdef WXWIDGETS
#include <wx/mstream.h>
#include <wx/stream.h>
#include <wx/xml/xml.h>

#else // WXWIDGETS
#if _MSC_VER >= 1300
#pragma warning ( push )
// Since we treat warnings as errors, turn off some xerces warnings.
// warning C4244: 'return' : conversion from '__w64 int' to 'unsigned long', possible loss of data
// warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#pragma warning ( disable : 4127 ) // for xerces2.2, 2.7 doesn't require it
#pragma warning ( disable : 4244 4267 )
#endif
#include <xercesc/dom/DOMException.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax/InputSource.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax2/Attributes.hpp>
#include <xercesc/sax2/DefaultHandler.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesVersion.hpp>
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>

// Note, see XercesVersion.hpp for how to use the version macros.
// Currently, we've used versions 2.2 and 2.7. There were no source code
// changes needed between the two, hence we haven't needed to do any
// funny stuff! Include 2.8 in that list...
#if _XERCES_VERSION < 20200
#error Minimum version of Xerces is 2.2.
#elif _XERCES_VERSION > 20800
#if _XERCES_VERSION != 30000
#pragma message ( "Warning: Untested version of Xerces" )
#endif
#endif
#pragma message ( "Compiling with Xerces " XERCES_FULLVERSIONDOT )

#if _MSC_VER >= 1300
#pragma warning ( pop )
#endif
XERCES_CPP_NAMESPACE_USE
#endif // WXWIDGETS

#if _MSC_VER < 1300
using namespace std;
#endif

#if defined(_MFC_VER) && defined(_DEBUG)
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////

bool Element::Initialize(tstring& outMsg)
{
	outMsg.erase();
#ifndef WXWIDGETS
	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (XMLException const& toCatch)
	{
		outMsg = _T("Error during Xerces Initialization.\n\n");
#ifdef UNICODE
		outMsg += toCatch.getMessage();
#else
		char* pStr = XMLString::transcode(toCatch.getMessage());
		outMsg += pStr;
		XMLString::release(&pStr);
#endif
		return false;
	}
#endif // WXWIDGETS
	return true;
}


void Element::Terminate()
{
#ifndef WXWIDGETS
	XMLPlatformUtils::Terminate();
#endif // WXWIDGETS
}


Element::Element()
{
}


Element::~Element()
{
}


////////////////////////////////////////////////////////////////////////////

#ifdef WXWIDGETS

static void ReadDoc(wxXmlNode* node, ElementNodePtr tree)
{
	wxXmlProperty* attribs = node->GetProperties();
	while (attribs)
	{
		tree->AddAttrib(attribs->GetName().c_str(), attribs->GetValue().c_str());
		attribs = attribs->GetNext();
	}
	wxString content = node->GetNodeContent();
	if (!content.empty())
		tree->SetValue(content.c_str());
	wxXmlNode* child = node->GetChildren();
	while (child)
	{
		if (wxXML_ELEMENT_NODE == child->GetType())
		{
			ElementNodePtr subtree = tree->AddElementNode(child->GetName().c_str());
			ReadDoc(child, subtree);
		}
		child = child->GetNext();
	}
}


class CWrapSTLStream : public wxOutputStream
{
public:
	CWrapSTLStream(std::ostream& output) : m_output(output) {}
    
    virtual wxOutputStream& Write(const void *buffer, size_t size)
	{
		m_output.write((const char*)buffer, size);
		return *this;
	}

private:
	std::ostream& m_output;
};


static void CreateDoc(wxXmlNode* node, ElementNode const& toWrite)
{
	int i;
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		tstring name, value;
		toWrite.GetNthAttrib(i, name, value);
		node->AddProperty(name.c_str(), value.c_str());
	}
	int count = toWrite.GetElementCount();
	for (i = 0; i < count; ++i)
	{
		ElementPtr element = toWrite.GetElement(i);
		switch (element->GetType())
		{
		case Element::Element_Node:
			{
				wxXmlNode* child = new wxXmlNode(node, wxXML_ELEMENT_NODE, element->GetName().c_str());
				node->AddChild(child);
				CreateDoc(child, *(dynamic_cast<ElementNode*>(element.get())));
			}
			break;
		case Element::Element_Text:
			node->SetContent(element->GetValue().c_str());
			break;
		}
	}
}

#else // WXWIDGETS

// [Copied from xerces DOMPrint.cpp sample code, with my comments added]
// ---------------------------------------------------------------------------
// Local const data
//
// Note: This is the 'safe' way to do these strings. If your compiler supports
// L"" style strings, and portability is not a concern, you can use
// those types constants directly.
// ---------------------------------------------------------------------------
///< Data: </
static XMLCh const gEndElement[] =
{
	chOpenAngle, chForwardSlash, chNull
};
///< Data: ?>
static XMLCh const gEndPI[] =
{
	chQuestion, chCloseAngle, chNull
};
///< Data: <?
static XMLCh const gStartPI[] =
{
	chOpenAngle, chQuestion, chNull
};
///< Data: <?xml version="
static XMLCh const gXMLDecl1[] =
{
	chOpenAngle, chQuestion, chLatin_x, chLatin_m, chLatin_l,
	chSpace, chLatin_v, chLatin_e, chLatin_r, chLatin_s, chLatin_i,
	chLatin_o, chLatin_n, chEqual, chDoubleQuote, chNull
};
///< Data: " encoding="
static XMLCh const gXMLDecl2[] =
{
	chDoubleQuote, chSpace, chLatin_e, chLatin_n, chLatin_c,
	chLatin_o, chLatin_d, chLatin_i, chLatin_n, chLatin_g, chEqual,
	chDoubleQuote, chNull
};
///< Data: " standalone="
static XMLCh const gXMLDecl3[] =
{
	chDoubleQuote, chSpace, chLatin_s, chLatin_t, chLatin_a,
	chLatin_n, chLatin_d, chLatin_a, chLatin_l, chLatin_o,
	chLatin_n, chLatin_e, chEqual, chDoubleQuote, chNull
};
///< Data: "?>\n
static XMLCh const gXMLDecl4[] =
{
	chDoubleQuote, chQuestion, chCloseAngle, chLF, chNull
};
///< Data: <![CDATA[
static XMLCh const gStartCDATA[] =
{
	chOpenAngle, chBang, chOpenSquare, chLatin_C, chLatin_D,
	chLatin_A, chLatin_T, chLatin_A, chOpenSquare, chNull
};
///< Data: ]]>
static XMLCh const gEndCDATA[] =
{
	chCloseSquare, chCloseSquare, chCloseAngle, chNull
};
///< Data: (xml comment begin - not shown as doxygen is having problems with it)
static XMLCh const gStartComment[] =
{
	chOpenAngle, chBang, chDash, chDash, chNull
};
///< Data: -->
static XMLCh const gEndComment[] =
{
	chDash, chDash, chCloseAngle, chNull
};
///< Data: <!DOCTYPE
static XMLCh const gStartDoctype[] =
{
	chOpenAngle, chBang, chLatin_D, chLatin_O, chLatin_C, chLatin_T,
	chLatin_Y, chLatin_P, chLatin_E, chSpace, chNull
};
///< Data: PUBLIC "
static XMLCh const gPublic[] =
{
	chLatin_P, chLatin_U, chLatin_B, chLatin_L, chLatin_I,
	chLatin_C, chSpace, chDoubleQuote, chNull
};
///< Data: SYSTEM "
static XMLCh const gSystem[] =
{
	chLatin_S, chLatin_Y, chLatin_S, chLatin_T, chLatin_E,
	chLatin_M, chSpace, chDoubleQuote, chNull
};
///< Data: <!ENTITY
static XMLCh const gStartEntity[] =
{
	chOpenAngle, chBang, chLatin_E, chLatin_N, chLatin_T, chLatin_I,
	chLatin_T, chLatin_Y, chSpace, chNull
};
///< Data: NDATA "
static XMLCh const gNotation[] =
{
	chLatin_N, chLatin_D, chLatin_A, chLatin_T, chLatin_A,
	chSpace, chDoubleQuote, chNull
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Provides an easy interface to translate from narrow characters (char)
 * to wide characters (XMLCh).
 */
class XMLstring : public std::basic_string<XMLCh>
{
public:
	/// Default constructor
	XMLstring()
		: std::basic_string<XMLCh>()
	{
	}
	/// Copy constructor
	XMLstring(XMLstring const& str)
		: std::basic_string<XMLCh>(str)
	{
	}
	/// XMLCh string constructor
	XMLstring(XMLCh const* str)
		: std::basic_string<XMLCh>(str)
	{
	}
	/// wstring->XMLCh translation constructor
	XMLstring(std::wstring const& str)
		: std::basic_string<XMLCh>(str)
	{
	}
	/// string->XMLCh translation constuctor
	XMLstring(std::string const& str)
	{
		if (0 < str.length())
		{
			XMLCh* data = XMLString::transcode(str.c_str());
			operator=(data);
			XMLString::release(&data);
		}
	}
	/// char->XMLCh translation constuctor
	XMLstring(char const* str)
	{
		if (str && *str)
		{
			XMLCh* data = XMLString::transcode(str);
			operator=(data);
			XMLString::release(&data);
		}
	}
	// From here down, we handle:
	//  XMLstring
	//  XMLCh*
	//  std::string
	//  char*
	XMLstring& operator=(XMLstring const& str)
	{
		if (this != &str)
#if _MSC_VER < 1300
			basic_string<XMLCh>::operator=(str);
#else
			std::basic_string<XMLCh>::operator=(str);
#endif
		return *this;
	}
	XMLstring& operator=(XMLCh const* str)
	{
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator=(str);
#else
		std::basic_string<XMLCh>::operator=(str);
#endif
		return *this;
	}
	XMLstring& operator=(std::wstring const& str)
	{
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator=(str);
#else
		std::basic_string<XMLCh>::operator=(str);
#endif
		return *this;
	}
	XMLstring& operator=(std::string const& str)
	{
		XMLstring data(str);
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator=(data);
#else
		std::basic_string<XMLCh>::operator=(data);
#endif
		return *this;
	}
	XMLstring& operator=(char const* str)
	{
		XMLstring data(str);
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator=(data);
#else
		std::basic_string<XMLCh>::operator=(data);
#endif
		return *this;
	}
	XMLstring& operator+=(XMLstring const& str)
	{
		if (this != &str)
#if _MSC_VER < 1300
			basic_string<XMLCh>::operator+=(str);
#else
			std::basic_string<XMLCh>::operator+=(str);
#endif
		return *this;
	}
	XMLstring& operator+=(XMLCh const* str)
	{
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator+=(str);
#else
		std::basic_string<XMLCh>::operator+=(str);
#endif
		return *this;
	}
	XMLstring& operator+=(std::wstring const& str)
	{
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator+=(str);
#else
		std::basic_string<XMLCh>::operator+=(str);
#endif
		return *this;
	}
	XMLstring& operator+=(std::string const& str)
	{
		XMLstring data(str);
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator+=(data);
#else
		std::basic_string<XMLCh>::operator+=(data);
#endif
		return *this;
	}
	XMLstring& operator+=(char const* str)
	{
		XMLstring data(str);
#if _MSC_VER < 1300
		basic_string<XMLCh>::operator+=(data);
#else
		std::basic_string<XMLCh>::operator+=(data);
#endif
		return *this;
	}
};


#ifndef UNICODE

/**
 * Provides an easy interface to translate from wide characters (XMLCh)
 * to narrow characters (char).
 */
class StringDOM : public std::string
{
public:
	/// Default constructor
	StringDOM()
#if _MSC_VER < 1300
		: string()
#else
		: std::string()
#endif
	{
	}
	/// Copy constructor
	StringDOM(StringDOM const& str)
#if _MSC_VER < 1300
		: string(str)
#else
		: std::string(str)
#endif
	{
	}
	/// Copy constructor
	StringDOM(std::string const& str)
#if _MSC_VER < 1300
		: string(str)
#else
		: std::string(str)
#endif
	{
	}
	/// char string constructor
	StringDOM(char const* str)
#if _MSC_VER < 1300
		: string(str)
#else
		: std::string(str)
#endif
	{
	}
	/// XMLCh->string translation constuctor
	StringDOM(XMLCh const* str)
	{
		if (str && XMLString::stringLen(str))
		{
			char* pStr = XMLString::transcode(str);
#if _MSC_VER < 1300
			string::operator=(pStr);
#else
			std::string::operator=(pStr);
#endif
			XMLString::release(&pStr);
		}
	}
	StringDOM& operator=(StringDOM const& str)
	{
		if (this != &str)
#if _MSC_VER < 1300
			string::operator=(str);
#else
			std::string::operator=(str);
#endif
		return *this;
	}
	StringDOM& operator=(std::string const& str)
	{
#if _MSC_VER < 1300
		string::operator=(str);
#else
		std::string::operator=(str);
#endif
		return *this;
	}
	StringDOM& operator=(char const* str)
	{
#if _MSC_VER < 1300
		string::operator=(str);
#else
		std::string::operator=(str);
#endif
		return *this;
	}
	StringDOM& operator=(XMLCh const* str)
	{
		StringDOM data(str);
#if _MSC_VER < 1300
		string::operator=(data);
#else
		std::string::operator=(data);
#endif
		return *this;
	}
	StringDOM& operator+=(StringDOM const& str)
	{
#if _MSC_VER < 1300
		string::operator+=(str);
#else
		std::string::operator+=(str);
#endif
		return *this;
	}
	StringDOM& operator+=(std::string const& str)
	{
#if _MSC_VER < 1300
		string::operator+=(str);
#else
		std::string::operator+=(str);
#endif
		return *this;
	}
	StringDOM& operator+=(char const* str)
	{
#if _MSC_VER < 1300
		string::operator+=(str);
#else
		std::string::operator+=(str);
#endif
		return *this;
	}
	StringDOM& operator+=(XMLCh const* str)
	{
		if (str && XMLString::stringLen(str))
		{
			char* pStr = XMLString::transcode(str);
#if _MSC_VER < 1300
			string::operator+=(pStr);
#else
			std::string::operator+=(pStr);
#endif
			XMLString::release(&pStr);
		}
		return *this;
	}
};


/**
 * @brief Transcoding optimizer
 *
 * This provides an optimization to reduce the number of transcodings we do
 * by caching all the element names.
 *
 * @todo Determine if there are a large number of elements if the performance
 *       gain we get from not transcoding may be lost due to lookup time.
 */
class StringTranscode : public std::map<XMLstring, char*>
{
public:
	StringTranscode()
	{
	}

	/**
	 * Clean up the cached data.
	 */
	~StringTranscode()
	{
		for (iterator iter = begin(); iter != end(); ++iter)
		{
			XMLString::release(&(*iter).second);
		}
	}

	/**
	 * Transcode an element name.
	 * @param xmlStr The incoming name from SAX to transcode.
	 * @return The transcoded string.
	 */
	char const* TranscodeElement(XMLCh const* const xmlStr);
};


char const* StringTranscode::TranscodeElement(XMLCh const* const xmlStr)
{
	XMLstring str(xmlStr);
	StringTranscode::iterator iter = find(str);
	if (iter == end())
	{
		iter = insert(std::pair<XMLstring, char*>(str, XMLString::transcode(xmlStr))).first;
	}
	return (*iter).second;
}


#else
typedef std::wstring StringDOM;

#endif

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief SAX callback class
 */
class SAXImportHandlers : public DefaultHandler
{
public:
	SAXImportHandlers(ElementNodePtr outTree, StringDOM& outMsg)
		: m_Tree(outTree)
		, m_Msg(outMsg)
		, m_Warnings(0)
		, m_Errors(0)
		, m_FatalErrors(0)
	{
	}
	~SAXImportHandlers()
	{
	}

	int GetWarnings() const		{return m_Warnings;}
	int GetErrors() const		{return m_Errors;}
	int GetFatalErrors() const	{return m_FatalErrors;}

	// -----------------------------------------------------------------------
	//  Handlers for the SAX DocumentHandler interface
	// -----------------------------------------------------------------------
	virtual void startElement(
			XMLCh const* const uri,
			XMLCh const* const localname,
			XMLCh const* const qname,
			Attributes const& attrs);
	virtual void endElement(
			XMLCh const* const uri,
			XMLCh const* const localname,
			XMLCh const* const qname);
	virtual void characters(
			XMLCh const* const chars,
#if _XERCES_VERSION < 30000
			unsigned int const length
#else
			const XMLSize_t length
#endif
			);
	void resetDocument();

	// -----------------------------------------------------------------------
	//  Handlers for the SAX ErrorHandler interface
	// -----------------------------------------------------------------------
	void warning(SAXParseException const& exception);
	void error(SAXParseException const& exception);
	void fatalError(SAXParseException const& exception);

private:
	ElementNodePtr m_Tree; ///< Parsed XML data
	StringDOM& m_Msg; ///< Any messages generated in ErrorHandler interface
	int m_Warnings;
	int m_Errors;
	int m_FatalErrors;
	std::list<ElementNodePtr> m_Parents; ///< Keeps track of where we are.
	StringDOM m_CurrentName; ///< Name of the element currently being parsed.
	XMLstring m_CurrentData; ///< Data of the element currently being parsed.
	class CurrentAttrib
	{
	public:
		CurrentAttrib()
			: m_Name()
			, m_Value()
		{
		}
		CurrentAttrib(
				tstring const& name,
				XMLCh const* const value)
			: m_Name(name)
			, m_Value(value)
		{}
		tstring m_Name;
		StringDOM m_Value;
	};
	std::list<CurrentAttrib> m_CurrentAttribs; ///< Attributes of the element currently being parsed.
#ifndef UNICODE
	StringTranscode m_ElementNames; ///< Reduce the number of transcodings on element names.
#endif
};


void SAXImportHandlers::startElement(
		XMLCh const* const /*uri*/,
		XMLCh const* const localname,
		XMLCh const* const qname,
		Attributes const& attrs)
{
	// If the name is non-zero, we're caching an element. This startElement is
	// starting inside the current element.
	if (0 < m_CurrentName.length())
	{
		// Since we're caching the element, we need to flush it so we can
		// start processing this incoming element.
		ElementNodePtr pParent;
		if (0 == m_Parents.size())
		{
			pParent = m_Tree;
			m_Parents.push_front(pParent);
			pParent->SetName(m_CurrentName);
		}
		else
		{
			pParent = m_Parents.front();
			assert(pParent);
			pParent = pParent->AddElementNode(m_CurrentName);
			m_Parents.push_front(pParent);
		}
		for (std::list<CurrentAttrib>::iterator iter = m_CurrentAttribs.begin(); m_CurrentAttribs.end() != iter; ++iter)
		{
			pParent->AddAttrib((*iter).m_Name, (*iter).m_Value);
		}
		m_CurrentAttribs.clear();
		// The parent element is now flushed, we can safely work on this element.
	}
	if (0 < m_CurrentData.length())
	{
		assert(0 < m_Parents.size());
		// Get the most current node.
		ElementNodePtr pParent = m_Parents.front();
		StringDOM data(m_CurrentData.c_str());
		m_CurrentData.erase();
		pParent->AddElementText(data);
	}
	// Cache the data...
#if _XERCES_VERSION < 30000
	qname; // unused param suppression
	XMLCh const* const elementName = localname;
#else
	localname; // unused param suppression
	XMLCh const* const elementName = qname;
#endif
#ifdef UNICODE
	m_CurrentName = elementName;
#else
	m_CurrentName = m_ElementNames.TranscodeElement(elementName);
#endif
#if _XERCES_VERSION < 30000
	unsigned int nAttribs = attrs.getLength();
	unsigned int i;
#else
	XMLSize_t nAttribs = attrs.getLength();
	XMLSize_t i;
#endif
	for (i = 0; i < nAttribs; ++i)
	{
#ifdef UNICODE
		m_CurrentAttribs.push_back(CurrentAttrib(attrs.getLocalName(i), attrs.getValue(i)));
#else
		m_CurrentAttribs.push_back(CurrentAttrib(m_ElementNames.TranscodeElement(attrs.getLocalName(i)), attrs.getValue(i)));
#endif
	}
}


void SAXImportHandlers::endElement(
		XMLCh const* const /*uri*/,
		XMLCh const* const /*localname*/,
		XMLCh const* const /*qname*/)
{
	ElementNodePtr pDataElement;
	// Insert the element.
	if (0 < m_CurrentName.length())
	{
		ElementNodePtr pParent;
		if (0 < m_Parents.size())
			pParent = m_Parents.front();
		ElementNodePtr pElement;
		// It's the root element
		if (!pParent)
		{
			pElement = m_Tree;
			pElement->SetName(m_CurrentName);
		}
		else
		{
			pElement = pParent->AddElementNode(m_CurrentName);
		}
		for (std::list<CurrentAttrib>::iterator iter = m_CurrentAttribs.begin(); m_CurrentAttribs.end() != iter; ++iter)
		{
			pElement->AddAttrib((*iter).m_Name, (*iter).m_Value);
		}
		if (0 < m_CurrentData.length())
			pDataElement = pElement;
	}
	// If the property has already been inserted, nothing to do.
	else
	{
		pDataElement = m_Parents.front();
		m_Parents.pop_front();
	}
	if (pDataElement && 0 < m_CurrentData.length())
	{
		StringDOM data(m_CurrentData.c_str());
		m_CurrentData.erase();
		pDataElement->AddElementText(data);
	}
	m_CurrentName.erase();
	m_CurrentAttribs.clear();
}


void SAXImportHandlers::characters(
		XMLCh const* const chars,
#if _XERCES_VERSION < 30000
		unsigned int const length
#else
		const XMLSize_t length
#endif
		)
{
	m_CurrentData.append(chars, length);
}


void SAXImportHandlers::resetDocument()
{
	m_Warnings = 0;
	m_Errors = 0;
	m_FatalErrors = 0;
}


void SAXImportHandlers::warning(SAXParseException const& toCatch)
{
	++m_Warnings;
	otstringstream tmp;
	tmp << _T("Warning in file '")
		<< StringDOM(toCatch.getSystemId())
		<< _T("', line ")
		<< toCatch.getLineNumber()
		<< _T(", column ")
		<< toCatch.getColumnNumber()
		<< _T(".  Message: ")
		<< StringDOM(toCatch.getMessage())
		<< std::endl;
	m_Msg += tmp.str();
}


void SAXImportHandlers::error(SAXParseException const& toCatch)
{
	++m_Errors;
	otstringstream tmp;
	tmp << _T("Error in file '")
		<< StringDOM(toCatch.getSystemId())
		<< _T("', line ")
		<< toCatch.getLineNumber()
		<< _T(", column ")
		<< toCatch.getColumnNumber()
		<< _T(".  Message: ")
		<< StringDOM(toCatch.getMessage())
		<< std::endl;
	m_Msg += tmp.str();
}


void SAXImportHandlers::fatalError(SAXParseException const& toCatch)
{
	++m_FatalErrors;
	otstringstream tmp;
	tmp << _T("Fatal Error in file '")
		<< StringDOM(toCatch.getSystemId())
		<< _T("', line ")
		<< toCatch.getLineNumber()
		<< _T(", column ")
		<< toCatch.getColumnNumber()
		<< _T(".  Message: ")
		<< StringDOM(toCatch.getMessage())
		<< std::endl;
	m_Msg += tmp.str();
}

/////////////////////////////////////////////////////////////////////////////

/// Current indent level when outputting data.
static int gIndentLevel = 0;
/// Formatting object so XML is generated properly.
static XMLFormatter* gFormatter = NULL;


/**
 * XMLFormatTarget object to write to an ostream.
 */
class CXMLFormatTarget : public XMLFormatTarget
{
public:
	CXMLFormatTarget(std::ostream& out)
		: m_out(out)
	{
	}
	~CXMLFormatTarget()
	{
	}
#if _XERCES_VERSION < 30000
	void writeChars(
		XMLByte const* const toWrite,
		unsigned int const count,
		XMLFormatter* const /*formatter*/)
	{
		m_out.write(reinterpret_cast<char const* const>(toWrite), count);
	}
#else
	virtual void writeChars(
		const XMLByte* const toWrite,
		const XMLSize_t count,
		XMLFormatter* const /*formatter*/)
	{
		m_out.write(reinterpret_cast<char const* const>(toWrite), static_cast<std::streamsize>(count));
	}
#endif
private:
	std::ostream& m_out; ///< Output stream.
	CXMLFormatTarget(CXMLFormatTarget const& other); ///< Unimplemented method.
};


XMLFormatter& operator<<(
		XMLFormatter& strm,
		XMLstring const& s)
{
	size_t lent = s.length();
	if (0 < lent)
	{
		// Only write out valid chars.
		// #x9 | #xA | #xD | [#x20-#xD7FF] | [#xE000-#xFFFD] | [#x10000-#x10FFFF]
		for (size_t i = 0; i < lent; ++i)
		{
			if (!(0x9 == s[i] || 0xA == s[i] || 0xD == s[i]
			|| (s[i] >= 0x20 && s[i] <= 0xD7FF)
			|| (s[i] >= 0xE000 && s[i] <= 0xFFFD)
			|| (s[i] >= 0x10000 && s[i] <= 0x10FFFF)))
				strm << chSpace;
			else
				strm << s[i];
		}
	}
	return strm;
}


static std::list<bool> s_ProcessingText;


static void Indent(
		std::ostream& target,
		int indentLevel)
{
	if (0 == s_ProcessingText.size() || !s_ProcessingText.front())
	{
		if (0 < indentLevel)
		{
			for (int i = 0; i < indentLevel; ++i)
				target << '\t';
		}
	}
}


std::ostream& operator<<(
		std::ostream& target,
		ElementText const& toWrite)
{
	XMLstring value(toWrite.GetValue().c_str());
	if (0 < value.length())
	{
		// Output content.
		gFormatter->formatBuf(value.c_str(), static_cast<int>(value.length()), XMLFormatter::CharEscapes);
	}
	return target;
}


std::ostream& operator<<(
		std::ostream& target,
		ElementNode const& toWrite)
{
	Indent(target, gIndentLevel);
	XMLstring nodeName(toWrite.GetName().c_str());
	// The name has to be representable without any escapes
	*gFormatter << XMLFormatter::NoEscapes << chOpenAngle << nodeName;

	int i;
	// Output any attributes on this element
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		tstring name, value;
		toWrite.GetNthAttrib(i, name, value);
		XMLstring attribName(name);
		XMLstring attribValue(value);
		// Again the name has to be completely representable. But the
		// attribute can have refs and requires the attribute style
		// escaping.
		*gFormatter << XMLFormatter::NoEscapes
			<< chSpace << attribName
			<< chEqual << chDoubleQuote
			<< XMLFormatter::AttrEscapes
			<< attribValue
			<< XMLFormatter::NoEscapes
			<< chDoubleQuote;
	}

	// Take care of children.
	int count = toWrite.GetElementCount();
	if (0 < count)
	{
		// Close start-tag.
		*gFormatter << XMLFormatter::NoEscapes << chCloseAngle;
		bool bHasText = toWrite.HasTextNodes();
		s_ProcessingText.push_front(bHasText);
		if (!bHasText)
			target << std::endl;
		// Output children.
		// No escapes are legal here
		++gIndentLevel;
		for (i = 0; i < count; ++i)
		{
			ElementPtr element = toWrite.GetElement(i);
			switch (element->GetType())
			{
			case Element::Element_Node:
				target << *(dynamic_cast<ElementNode*>(element.get()));
				break;
			case Element::Element_Text:
				target << *(dynamic_cast<ElementText*>(element.get()));
				break;
			}
		}
		--gIndentLevel;
		// Write end-tag.
		Indent(target, gIndentLevel);
		s_ProcessingText.pop_front();
		*gFormatter << XMLFormatter::NoEscapes << gEndElement << nodeName << chCloseAngle;
	}
	// If there are no children, just close.
	else
	{
		*gFormatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
	}
	if (0 == s_ProcessingText.size() || !s_ProcessingText.front())
		target << std::endl;
	return target;
}

#endif // WXWIDGETS

/////////////////////////////////////////////////////////////////////////////

ElementNodePtr ElementNode::New()
{
	ElementNodePtr pNode(new ElementNode());
	pNode->m_Me = pNode;
	return pNode;
}


ElementNodePtr ElementNode::New(tstring const& inText)
{
	ElementNodePtr pNode(new ElementNode(inText));
	pNode->m_Me = pNode;
	return pNode;
}


ElementNode::ElementNode()
{
}


ElementNode::ElementNode(tstring const& inName)
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
	otstringstream msg;
	msg.width(inLevel);
	msg << _T(" ") << m_Name;
	for (i = 0; i < GetAttribCount(); ++i)
	{
		tstring name, value;
		GetNthAttrib(i, name, value);
		msg << _T(" ")
			<< name
			<< _T("=\"")
			<< value
			<< _T("\"");
	}
	DumpErrorMessage(msg.str());
	for (i = 0; i < GetElementCount(); ++i)
	{
		GetElement(i)->Dump(inLevel+1);
	}
}


Element::ElementType ElementNode::GetType() const
{
	return Element::Element_Node;
}


tstring const& ElementNode::GetName() const
{
	return m_Name;
}


void ElementNode::SetName(tstring const& inName)
{
	m_Name = inName;
}


tstring ElementNode::GetValue() const
{
	tstring value;
	for (int i = 0; i < GetElementCount(); ++i)
	{
		if (Element::Element_Text == GetElement(i)->GetType())
			value += GetElement(i)->GetValue();
	}
	return value;
}


void ElementNode::SetValue(tstring const& inValue)
{
	RemoveAllTextNodes();
	ElementTextPtr pText = ElementText::New();
	pText->SetValue(inValue);
	m_Elements.push_back(pText);
}


void ElementNode::SetValue(TCHAR const* const inValue)
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
		tstring& outName,
		tstring& outValue) const
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
		tstring const& inName,
		tstring& outValue) const
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
		tstring const& inName,
		ARBVersion& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		unsigned short major = 0;
		unsigned short minor = 0;
		tstring::size_type pos = value.find('.');
		if (tstring::npos != pos)
		{
			major = static_cast<unsigned short>(tstringUtil::atol(value));
			value = value.substr(pos+1);
			minor = static_cast<unsigned short>(tstringUtil::atol(value));
		}
		else
		{
			major = static_cast<unsigned short>(tstringUtil::atol(value));
		}
		outValue = ARBVersion(major, minor);
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		tstring const& inName,
		ARBDate& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		ARBDate date = ARBDate::FromString(value, ARBDate::eDefault);
		if (date.IsValid())
			outValue = date;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		tstring const& inName,
		bool& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (value == _T("y"))
			outValue = true;
		else if (value == _T("n"))
			outValue = false;
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		tstring const& inName,
		short& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = static_cast<short>(tstringUtil::atol(value));
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		tstring const& inName,
		long& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = tstringUtil::atol(value);
		else
			rc = eInvalidValue;
	}
	return rc;
}


ElementNode::AttribLookup ElementNode::GetAttrib(
		tstring const& inName,
		double& outValue) const
{
	tstring value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = tstringUtil::strtod(value);
		else
			rc = eInvalidValue;
	}
	return rc;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		tstring const& inValue)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = inValue;
	return true;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		TCHAR const* const inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = inValue;
	else
		m_Attribs[inName] = _T("");
	return true;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		ARBVersion const& inValue)
{
	return AddAttrib(inName, inValue.str());
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		ARBDate const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.GetString(ARBDate::eDashYMD));
	return false;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		bool inValue)
{
	if (inName.empty())
		return false;
	if (inValue)
		m_Attribs[inName] = _T("y");
	else
		m_Attribs[inName] = _T("n");
	return true;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		short inValue)
{
	if (inName.empty())
		return false;
	otstringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		long inValue)
{
	if (inName.empty())
		return false;
	otstringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}


bool ElementNode::AddAttrib(
		tstring const& inName,
		double inValue,
		int inPrec)
{
	if (inName.empty())
		return false;
	m_Attribs[inName] = ARBDouble::str(inValue, inPrec);
	return true;
}


bool ElementNode::RemoveAttrib(tstring const& inName)
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
	return tr1::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
}


ElementNodePtr ElementNode::GetElementNode(int inIndex)
{
	return tr1::dynamic_pointer_cast<ElementNode, Element>(m_Elements[inIndex]);
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
		tstring const& inName,
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
		tstring const& inText,
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
		tstring const& inName,
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
		tstring const& inName,
		tstring const* inValue) const
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


static bool LoadXML(
		ElementNodePtr node,
#ifdef WXWIDGETS
		wxXmlDocument& inSource,
#else // WXWIDGETS
		XERCES_CPP_NAMESPACE_QUALIFIER InputSource const& inSource,
#endif // WXWIDGETS
		tstring& ioErrMsg)
{
	node->clear();
	bool bOk = false;

#ifdef WXWIDGETS
	if (!inSource.GetRoot())
		return bOk;
	node->SetName(inSource.GetRoot()->GetName().c_str());
	ReadDoc(inSource.GetRoot(), node);
	bOk = true;

#else // WXWIDGETS
	SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
	// On Win32, an XMLCh is a UNICODE character.
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://xml.org/sax/features/namespaces"), false);
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://xml.org/sax/features/validation"), true);
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://apache.org/xml/features/validation/dynamic"), true);
	StringDOM eMsg;
	SAXImportHandlers handler(node, eMsg);
	parser->setContentHandler(&handler);
	parser->setErrorHandler(&handler);

	try
	{
		parser->parse(inSource);
		bOk = true;
	}
	catch (DOMException const& e)
	{
		eMsg += _T("An error occured during parsing.  Exception message: ");
		eMsg += e.msg;
	}
	catch (XMLException const& e)
	{
		eMsg += _T("An error occured during parsing.  Exception message: ");
		eMsg += e.getMessage();
	}
	catch (SAXException const& e)
	{
		eMsg += _T("An error occured during parsing.  Exception message: ");
		eMsg += e.getMessage();
	}
	if (bOk && handler.GetWarnings() > 0 || handler.GetErrors() > 0 || handler.GetFatalErrors() > 0)
	{
		if (handler.GetErrors() > 0 || handler.GetFatalErrors() > 0)
			bOk = false;
	}
	if (!bOk)
	{
		ioErrMsg += eMsg;
	}
	delete parser;
#endif // WXWIDGETS
	return bOk;
}


bool ElementNode::LoadXMLBuffer(
		char const* inData,
		size_t nData,
		tstring& ioErrMsg)
{
#ifdef WXWIDGETS
	wxXmlDocument source;
	wxMemoryInputStream input(inData, nData);
	if (!source.Load(input))
		return false;
#else // WXWIDGETS
	MemBufInputSource source(reinterpret_cast<XMLByte const*>(inData), nData, "buffer");
#endif // WXWIDGETS
	return LoadXML(m_Me.lock(), source, ioErrMsg);
}


bool ElementNode::LoadXMLFile(
		TCHAR const* inFileName,
		tstring& ioErrMsg)
{
#ifdef WXWIDGETS
	wxXmlDocument source;
	if (!source.Load(inFileName))
		return false;
#else // WXWIDGETS
	XMLstring fileName(inFileName);
	LocalFileInputSource source(fileName.c_str());
#endif // WXWIDGETS
	return LoadXML(m_Me.lock(), source, ioErrMsg);
}


#ifndef _WIN32
#define _tcscmp		strcmp
#endif

bool ElementNode::SaveXML(
		std::ostream& outOutput,
		std::string const* inDTD) const
{
#ifdef WXWIDGETS
	wxXmlDocument doc;
	doc.SetVersion(wxT("1.0"));
	doc.SetFileEncoding(wxT("utf-8"));
	wxXmlNode* root = new wxXmlNode(NULL, wxXML_ELEMENT_NODE, GetName().c_str());
	doc.SetRoot(root);
	CreateDoc(root, *this);
	CWrapSTLStream output(outOutput);
	return doc.Save(output, 1);

#else // WXWIDGETS
	// On Win32, an XMLCh is a UNICODE character.
	XMLCh* encodingName = reinterpret_cast<XMLCh*>(L"utf-8");
	CXMLFormatTarget formatTarget(outOutput);
	outOutput << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	if (inDTD && 0 < inDTD->length())
	{
		const char* treeBook = "AgilityBook";
#ifdef _DEBUG
		assert(0 == _tcscmp(TREE_BOOK, _T("AgilityBook")));
#endif
		outOutput << "<!DOCTYPE " << treeBook << " [\n";
		outOutput << *inDTD;
		outOutput << "\n]>\n";
	}
	gIndentLevel = 0;
	gFormatter = new XMLFormatter(encodingName, &formatTarget,
		XMLFormatter::NoEscapes, XMLFormatter::UnRep_CharRef);
	outOutput << *this;
	delete gFormatter;
	gFormatter = NULL;
	return true;
#endif // WXWIDGETS
}


bool ElementNode::SaveXML(
		TCHAR const* outFile,
		std::string const* inDTD) const
{
	bool bOk = false;
	if (!outFile)
		return bOk;
	char const* pFile = NULL;
#ifdef UNICODE
	std::string filename = tstringUtil::Convert(std::wstring(outFile));
	pFile = filename.c_str();
#else
	pFile = outFile;
#endif
	std::ofstream output(pFile, std::ios::out | std::ios::binary);
	output.exceptions(std::ios_base::badbit);
	if (output.is_open())
	{
		bOk = SaveXML(output, inDTD);
		output.close();
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


ElementTextPtr ElementText::New(tstring const& inText)
{
	ElementTextPtr pText(new ElementText(inText));
	pText->m_Me = pText;
	return pText;
}


ElementText::ElementText()
	: m_Value()
{
}


ElementText::ElementText(tstring const& inText)
	: m_Value(inText)
{
}


void ElementText::Dump(int inLevel) const
{
	otstringstream msg;
	msg.width(inLevel);
	msg << _T(" ") << GetName();
	if (0 < m_Value.length())
	{
		msg << _T(": ")
			<< m_Value;
	}
	DumpErrorMessage(msg.str());
}


Element::ElementType ElementText::GetType() const
{
	return Element::Element_Text;
}


tstring const& ElementText::GetName() const
{
	static const tstring name(_T("#text"));
	return name;
}


void ElementText::SetName(tstring const& /*inName*/)
{
}


tstring ElementText::GetValue() const
{
	return m_Value;
}


void ElementText::SetValue(tstring const& inValue)
{
	m_Value = inValue;
}


void ElementText::SetValue(TCHAR const* const inValue)
{
	if (inValue)
		m_Value = inValue;
	else
		m_Value.erase();
}


void ElementText::SetValue(short inValue)
{
	otstringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(long inValue)
{
	otstringstream str;
	str << inValue;
	m_Value = str.str();
}


void ElementText::SetValue(
		double inValue,
		int inPrec)
{
	m_Value = ARBDouble::str(inValue, inPrec);
}
