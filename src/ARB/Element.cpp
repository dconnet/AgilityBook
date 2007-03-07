/*
 * Copyright © 2002-2007 David Connet. All Rights Reserved.
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
 * @li 2007-03-37 DRC Fixed a problem releasing transcoded data.
 * @li 2005-06-09 DRC Numbers were being stored/shown in scientific notation.
 * @li 2004-06-16 DRC Changed ARBDate::GetString to put leadingzero into format.
 * @li 2004-01-04 DRC Moved date parsing code to ARBDate::FromString.
 * @li 2003-11-26 DRC Changed version number to a complex value.
 * @li 2003-10-22 DRC Added a DTD parameter to SaveXML.
 */

#include "stdafx.h"
#include "Element.h"
#include <iostream>
#include <list>
#include <map>

#include "ARBAgilityRecordBook.h"
#include "ARBDate.h"
#include "ARBTypes.h"

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
#include <xercesc/util/XMLException.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUniDefs.hpp>
#if _MSC_VER >= 1300
#pragma warning ( pop )
#endif
XERCES_CPP_NAMESPACE_USE

#if _MSC_VER < 1300
using namespace std;
#endif

// Write the dump and errors to cerr
#define ERRORS_TO_CERR
#if defined(_WINDOWS) && !defined(_CONSOLE)
// On windows, we'll write the dump to TRACE and errors to AfxMessageBox.
#undef ERRORS_TO_CERR
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////

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

#ifndef UNICODE

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
typedef std::basic_string<XMLCh> XMLstring;
typedef std::wstring StringDOM;

#endif

/////////////////////////////////////////////////////////////////////////////

/**
 * @brief SAX callback class
 *
 * @attention We assume no element may contain both text and elements.
 * Because of the XML structure we have defined, this is not an issue.
 */
class SAXImportHandlers : public DefaultHandler
{
public:
	SAXImportHandlers(Element& outTree, StringDOM& outMsg)
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
	void startElement(
			XMLCh const* const uri,
			XMLCh const* const localname,
			XMLCh const* const qname,
			Attributes const& attrs);
	void endElement(
			XMLCh const* const uri,
			XMLCh const* const localname,
			XMLCh const* const qname);
	void characters(
			XMLCh const* const chars,
			unsigned int const length);
	void resetDocument();

	// -----------------------------------------------------------------------
	//  Handlers for the SAX ErrorHandler interface
	// -----------------------------------------------------------------------
	void warning(SAXParseException const& exception);
	void error(SAXParseException const& exception);
	void fatalError(SAXParseException const& exception);

private:
	Element& m_Tree; ///< Parsed XML data
	StringDOM& m_Msg; ///< Any messages generated in ErrorHandler interface
	int m_Warnings;
	int m_Errors;
	int m_FatalErrors;
	std::list<Element*> m_Parents; ///< Keeps track of where we are.
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
				ARBString const& name,
				XMLCh const* const value)
			: m_Name(name)
			, m_Value(value)
		{}
		ARBString m_Name;
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
		XMLCh const* const /*qname*/,
		Attributes const& attrs)
{
	// If the name is non-zero, we're caching an element. This startElement is
	// starting inside the current element. Since we don't allow mixing data
	// and elements, we don't need to worry about m_CurrentData.
	if (0 < m_CurrentName.length())
	{
		// Since we're caching the element, we need to flush it so we can
		// start processing this incoming element.
		Element* pParent = NULL;
		if (0 == m_Parents.size())
		{
			pParent = &m_Tree;
			m_Parents.push_front(pParent);
			pParent->SetName(m_CurrentName);
		}
		else
		{
			pParent = m_Parents.front();
			ASSERT(pParent);
			Element& element = pParent->AddElement(m_CurrentName);
			pParent = &element;
			m_Parents.push_front(pParent);
		}
		for (std::list<CurrentAttrib>::iterator iter = m_CurrentAttribs.begin(); m_CurrentAttribs.end() != iter; ++iter)
		{
			pParent->AddAttrib((*iter).m_Name, (*iter).m_Value);
		}
		m_CurrentAttribs.clear();
		// The parent element is now flushed, we can safely work on this element.
	}
	// Cache the data...
	m_CurrentData.erase();
#ifdef UNICODE
	m_CurrentName = localname;
#else
	m_CurrentName = m_ElementNames.TranscodeElement(localname);
#endif
	unsigned int nAttribs = attrs.getLength();
	for (unsigned int i = 0; i < nAttribs; ++i)
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
	// Insert the element.
	if (0 < m_CurrentName.length())
	{
		Element* pParent = NULL;
		if (0 < m_Parents.size())
			pParent = m_Parents.front();
		Element* pElement = NULL;
		// It's the root element
		if (!pParent)
		{
			pElement = &m_Tree;
			pElement->SetName(m_CurrentName);
		}
		else
		{
			Element& element = pParent->AddElement(m_CurrentName);
			pElement = &element;
		}
		for (std::list<CurrentAttrib>::iterator iter = m_CurrentAttribs.begin(); m_CurrentAttribs.end() != iter; ++iter)
		{
			pElement->AddAttrib((*iter).m_Name, (*iter).m_Value);
		}
		if (0 < m_CurrentData.length())
		{
			StringDOM data(m_CurrentData.c_str());
			pElement->SetValue(data);
		}
	}
	// If the property has already been inserted, nothing to do.
	else
		m_Parents.pop_front();
	m_CurrentName.erase();
	m_CurrentData.erase();
	m_CurrentAttribs.clear();
}

void SAXImportHandlers::characters(
		XMLCh const* const chars,
		unsigned int const length)
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
	ARBostringstream tmp;
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
	ARBostringstream tmp;
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
	ARBostringstream tmp;
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
	void writeChars(
		XMLByte const* const toWrite,
		unsigned int const count,
		XMLFormatter* const /*formatter*/)
	{
		m_out.write(reinterpret_cast<char const* const>(toWrite), count);
	}
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

static void Indent(
		std::ostream& target,
		int indentLevel)
{
	if (0 < indentLevel)
	{
		for (int i = 0; i < indentLevel; ++i)
			target << '\t';
	}
}

std::ostream& operator<<(
		std::ostream& target,
		Element const& toWrite)
{
	Indent(target, gIndentLevel);
	XMLstring nodeName(toWrite.GetName().c_str());
	// The name has to be representable without any escapes
	*gFormatter << XMLFormatter::NoEscapes << chOpenAngle << nodeName;

	int i;
	// Output any attributes on this element
	for (i = 0; i < toWrite.GetAttribCount(); ++i)
	{
		ARBString name, value;
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
		target << std::endl;
		// Output children.
		// No escapes are legal here
		++gIndentLevel;
		for (i = 0; i < count; ++i)
			target << (toWrite.GetElement(i));
		--gIndentLevel;
		// Write end-tag.
		Indent(target, gIndentLevel);
		*gFormatter << XMLFormatter::NoEscapes << gEndElement << nodeName << chCloseAngle;
	}
	// If there are no children, see if there is any data to write.
	else
	{
		XMLstring value(toWrite.GetValue().c_str());
		if (0 < value.length())
		{
			// Close start-tag.
			*gFormatter << XMLFormatter::NoEscapes << chCloseAngle;
			// Output content.
			gFormatter->formatBuf(value.c_str(), static_cast<int>(value.length()), XMLFormatter::CharEscapes);
			// Write end-tag.
			*gFormatter << XMLFormatter::NoEscapes << gEndElement << nodeName << chCloseAngle;
		}
		else
			*gFormatter << XMLFormatter::NoEscapes << chForwardSlash << chCloseAngle;
	}
	target << std::endl;
	return target;
}

/////////////////////////////////////////////////////////////////////////////

Element::Element()
{
}

Element::Element(ARBString const& inName)
	: m_Name(inName)
	, m_Value(_T(""))
{
}

Element::Element(Element const& rhs)
	: m_Name(rhs.m_Name)
	, m_Value(rhs.m_Value)
{
	m_Attribs = rhs.m_Attribs;
	m_Elements = rhs.m_Elements;
}

Element& Element::operator=(Element const& rhs)
{
	if (this != &rhs)
	{
		m_Name = rhs.m_Name;
		m_Value = rhs.m_Value;
		m_Attribs = rhs.m_Attribs;
		m_Elements = rhs.m_Elements;
	}
	return *this;
}

Element::~Element()
{
}

void Element::Dump(int inLevel) const
{
	int i;
	ARBostringstream msg;
	msg.width(inLevel);
	msg << _T(" ") << m_Name;
	for (i = 0; i < GetAttribCount(); ++i)
	{
		ARBString name, value;
		GetNthAttrib(i, name, value);
		msg << _T(" ")
			<< name
			<< _T("=\"")
			<< value
			<< _T("\"");
	}
	if (0 < m_Value.length())
	{
		msg << _T(": ")
			<< m_Value;
	}
#ifdef ERRORS_TO_CERR
	cerr << msg.str() << endl;
#else
	TRACE(_T("%s\n"), msg.str().c_str());
#endif
	for (i = 0; i < GetElementCount(); ++i)
	{
		GetElement(i).Dump(inLevel+1);
	}
}

void Element::clear()
{
	m_Name.erase();
	m_Value.erase();
	m_Attribs.clear();
	m_Elements.clear();
}

ARBString const& Element::GetName() const
{
	return m_Name;
}

void Element::SetName(ARBString const& inName)
{
	m_Name = inName;
}

ARBString const& Element::GetValue() const
{
	return m_Value;
}

void Element::SetValue(ARBString const& inValue)
{
	ASSERT(0 == m_Elements.size());
	m_Value = inValue;
}

void Element::SetValue(TCHAR const* const inValue)
{
	ASSERT(0 == m_Elements.size());
	if (inValue)
		m_Value = inValue;
	else
		m_Value.erase();
}

void Element::SetValue(short inValue)
{
	ASSERT(0 == m_Elements.size());
	ARBostringstream str;
	str << inValue;
	m_Value = str.str();
}

void Element::SetValue(long inValue)
{
	ASSERT(0 == m_Elements.size());
	ARBostringstream str;
	str << inValue;
	m_Value = str.str();
}

void Element::SetValue(
		double inValue,
		int inPrec)
{
	ASSERT(0 == m_Elements.size());
	m_Value = ARBDouble::str(inValue, inPrec);
}

int Element::GetAttribCount() const
{
	return static_cast<int>(m_Attribs.size());
}

Element::AttribLookup Element::GetNthAttrib(
		int inIndex,
		ARBString& outName,
		ARBString& outValue) const
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

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		ARBString& outValue) const
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

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		ARBVersion& outValue) const
{
	ARBString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		unsigned short major = 0;
		unsigned short minor = 0;
		ARBString::size_type pos = value.find('.');
		if (ARBString::npos != pos)
		{
			major = static_cast<unsigned short>(_tstol(value.c_str()));
			value = value.substr(pos+1);
			minor = static_cast<unsigned short>(_tstol(value.c_str()));
		}
		else
		{
			major = static_cast<unsigned short>(_tstol(value.c_str()));
		}
		outValue = ARBVersion(major, minor);
	}
	return rc;
}

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		ARBDate& outValue) const
{
	ARBString value;
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

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		bool& outValue) const
{
	ARBString value;
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

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		short& outValue) const
{
	ARBString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = static_cast<short>(_tstol(value.c_str()));
		else
			rc = eInvalidValue;
	}
	return rc;
}

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		long& outValue) const
{
	ARBString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = _tstol(value.c_str());
		else
			rc = eInvalidValue;
	}
	return rc;
}

Element::AttribLookup Element::GetAttrib(
		ARBString const& inName,
		double& outValue) const
{
	ARBString value;
	AttribLookup rc = GetAttrib(inName, value);
	if (eFound == rc)
	{
		if (0 < value.length())
			outValue = _tcstod(value.c_str(), NULL);
		else
			rc = eInvalidValue;
	}
	return rc;
}

bool Element::AddAttrib(
		ARBString const& inName,
		ARBString const& inValue)
{
	m_Attribs[inName] = inValue;
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		TCHAR const* const inValue)
{
	if (inValue)
		m_Attribs[inName] = inValue;
	else
		m_Attribs[inName] = _T("");
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		ARBVersion const& inValue)
{
	return AddAttrib(inName, inValue.str());
}

bool Element::AddAttrib(
		ARBString const& inName,
		ARBDate const& inValue)
{
	if (inValue.IsValid())
		AddAttrib(inName, inValue.GetString(ARBDate::eDashYMD));
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		bool inValue)
{
	if (inValue)
		m_Attribs[inName] = _T("y");
	else
		m_Attribs[inName] = _T("n");
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		short inValue)
{
	ARBostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		long inValue)
{
	ARBostringstream str;
	str << inValue;
	m_Attribs[inName] = str.str();
	return true;
}

bool Element::AddAttrib(
		ARBString const& inName,
		double inValue,
		int inPrec)
{
	m_Attribs[inName] = ARBDouble::str(inValue, inPrec);
	return true;
}

bool Element::RemoveAttrib(ARBString const& inName)
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

void Element::RemoveAllAttribs()
{
	m_Attribs.clear();
}

int Element::GetElementCount() const
{
	return static_cast<int>(m_Elements.size());
}

Element const& Element::GetElement(int inIndex) const
{
	return m_Elements[inIndex];
}

Element& Element::GetElement(int inIndex)
{
	return m_Elements[inIndex];
}

Element& Element::AddElement(
		ARBString const& inName,
		int inAt)
{
	ASSERT(0 == m_Value.length());
	size_t index;
	std::vector<Element>::iterator iter = m_Elements.begin();
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
	m_Elements.insert(iter, Element(inName));
	return m_Elements[index];
}

bool Element::RemoveElement(int inIndex)
{
	bool bOk = false;
	if (0 <= inIndex && inIndex < static_cast<int>(m_Elements.size()))
	{
		std::vector<Element>::iterator iter = m_Elements.begin();
		iter += inIndex;
		m_Elements.erase(iter);
		bOk = true;
	}
	return bOk;
}

void Element::RemoveAllElements()
{
	m_Elements.clear();
}

int Element::FindElement(
		ARBString const& inName,
		int inStartFrom) const
{
	if (0 > inStartFrom)
		inStartFrom = 0;
	for (; inStartFrom < static_cast<int>(m_Elements.size()); ++inStartFrom)
	{
		if (m_Elements[inStartFrom].GetName() == inName)
			return inStartFrom;
	}
	return -1;
}

//private
bool Element::LoadXML(
		XERCES_CPP_NAMESPACE_QUALIFIER InputSource const& inSource,
		ARBString& ioErrMsg)
{
	clear();
	bool bOk = false;

	SAX2XMLReader* parser = XMLReaderFactory::createXMLReader();
	// On Win32, an XMLCh is a UNICODE character.
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://xml.org/sax/features/namespaces"), false);
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://xml.org/sax/features/validation"), true);
	parser->setFeature(reinterpret_cast<XMLCh*>(L"http://apache.org/xml/features/validation/dynamic"), true);
	StringDOM eMsg;
	SAXImportHandlers handler(*this, eMsg);
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
	return bOk;
}

bool Element::LoadXMLBuffer(
		char const* inData,
		unsigned int nData,
		ARBString& ioErrMsg)
{
	MemBufInputSource source(reinterpret_cast<XMLByte const*>(inData), nData, "buffer");
	return LoadXML(source, ioErrMsg);
}

bool Element::LoadXMLFile(
		ARBString const& inFileName,
		ARBString& ioErrMsg)
{
	XMLstring fileName(inFileName.c_str());
	LocalFileInputSource source(fileName.c_str());
	return LoadXML(source, ioErrMsg);
}

bool Element::SaveXML(
		std::ostream& outOutput,
		ARBString const* inDTD) const
{
	// On Win32, an XMLCh is a UNICODE character.
	XMLCh* encodingName = reinterpret_cast<XMLCh*>(L"UTF-8");
	CXMLFormatTarget formatTarget(outOutput);
	outOutput << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
	if (inDTD && 0 < inDTD->length())
	{
		outOutput << "<!DOCTYPE " << TREE_BOOK << " [\n";
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
}
