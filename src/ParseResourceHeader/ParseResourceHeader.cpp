/*
 * Copyright © 2004-2007 David Connet. All Rights Reserved.
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
 * @brief Translate resource.h/resource.hm for use in help.
 * @author David Connet
 *
 * This parses the auto-generated resource.hm in order to convert the hex
 * defines into decimal so the hhc compiler will work with the text popups.
 * It also translates the IDD_ defines from resource.h into contextid.h
 * as id+HID_BASE_RESOURCE.
 *
 * From afxpriv.h:
 *  HID_BASE_COMMAND:  ID and IDM
 *  HID_BASE_RESOURCE: IDR and IDD
 *  HID_BASE_PROMPT:   IDP
 *  HID_BASE_NCAREAS
 *  HID_BASE_CONTROL:  IDC
 *  HID_BASE_DISPATCH: IDispatch help codes
 *
 * Revision History
 * @li 2004-10-14 DRC Verify HIDC_ defines have the correct value. When values
 *                    are modified in resource.h, the .hm define isn't
 *                    necessarily updated.
 * @li 2004-06-05 DRC Changed usage to create contextid.h also.
 * @li 2004-05-20 DRC Created
 */

#if 0
>>>>>This is what resource.hm looks like:
// Microsoft Visual C++ generated Help ID include file.
// Used by AgilityBook.rc
//
#define HIDC_ABOUT_UPDATE               0x80c803ee    // IDD_ABOUTBOX

>>>>>This is what we need:
#define HIDC_ABOUT_UPDATE 2160591854
#endif

#include <atlstr.h>
#include <iostream>
#include <fstream>
#include <locale>
#include <map>
#include <set>
#include <string>
#include <tchar.h>

// From afxpriv.h, this eliminated the dependency on mfc.
// Help ID bases
#define HID_BASE_COMMAND    0x00010000UL        // ID and IDM
#define HID_BASE_RESOURCE   0x00020000UL        // IDR and IDD
#define HID_BASE_PROMPT     0x00030000UL        // IDP
#define HID_BASE_NCAREAS    0x00040000UL
#define HID_BASE_CONTROL    0x00050000UL        // IDC
#define HID_BASE_DISPATCH   0x00060000UL        // IDispatch help codes

// Include these simply for re-compile and to cause the post-build event.
#include "../Win/resource.h"
#include "../Win/resource.hm"

using namespace std;

static string get_line(istream& file)
{
	char buffer[1000]; // We just "know" this is big enough. In fact, 80 would be good.

	file.getline(buffer, sizeof(buffer) - 1);
	int len = static_cast<int>(strlen(buffer));
	while (0 < len && !file.eof() && '\\' == buffer[len-1])
	{
		file.getline(&buffer[len-1], sizeof(buffer) - len - 1);
		len = static_cast<int>(strlen(buffer));
	}
	if (0 < len)
		return buffer;
	else
		return "";
}

static bool parse_define(string line, string& outDefine, unsigned long& outValue, std::string* pOutIDD = NULL)
{
	outDefine.clear();
	outValue = 0;
	bool bParsed = false;
	if (0 < line.length())
	{
		string::size_type pos = line.find("//");
		if (0 <= pos)
		{
			if (pOutIDD)
			{
				CStringA str(line.substr(pos + 2).c_str());
				str.TrimLeft();
				str.TrimRight();
				*pOutIDD = (LPCSTR)str;
			}
			line = line.substr(0, pos);
		}
		if (0 < line.length())
		{
			if (0 == line.find("#define"))
			{
				locale loc;
				bParsed = true;
				pos = line.length() - 1;
				while (pos >= 0 && use_facet< ctype<char> >(loc).is(ctype_base::space, line[pos]))
					--pos;
				if (pos != line.length() - 1)
					line = line.substr(0, pos + 1);
				pos = line.find_first_of(" \t");
				while (use_facet< ctype<char> >(loc).is(ctype_base::space, line[pos]))
					++pos;
				line = line.substr(pos); // skip define and whitespace
				pos = line.find_first_of(" \t");
				outDefine = line.substr(0, pos); // macro
				while (use_facet< ctype<char> >(loc).is(ctype_base::space, line[pos]))
					++pos;
				line = line.substr(pos); // skip macro and whitespace
				if (0 == line.find("0x"))
					outValue = strtoul(line.c_str(), NULL, 16);
				else
					outValue = atol(line.c_str());
			}
		}
	}
	return bParsed;
}

int _tmain(int argc, char* argv[])
{
	if (6 != argc)
	{
		cerr << "Usage: " << argv[0] << " resource.h resource.hm Help/AgilityBook.txt output/AgilityBook.h output/contextid.h" << endl;
		return 1;
	}
	string resourceH(argv[argc-5]);
	string resourceHM(argv[argc-4]);
	string agilitybookTXT(argv[argc-3]);
	string agilitybookH(argv[argc-2]);
	string contextidH(argv[argc-1]);

	// Maps to store the ids of IDD/IDR/IDC controls from resource.h for
	// lookup when parsing resource.hm
	typedef std::map<std::string, unsigned long> ResourceMap;
	ResourceMap resIDDs;
	ResourceMap resIDCs;

	// First, we'll parse the IDD/IDR defines into the context.h file.
	ifstream resH(resourceH.c_str());
	if (!resH.is_open())
	{
		cerr << "Can't read " << resourceH << endl;
		return 1;
	}
	ofstream contextH(contextidH.c_str());
	if (!contextH.is_open())
	{
		cerr << "Can't write " << contextidH << endl;
		return 1;
	}

	contextH << "// Autogenerated from " << resourceH << endl;
	while (!resH.eof())
	{
		string line = get_line(resH);
		string outDefine;
		unsigned long outValue;
		if (parse_define(line, outDefine, outValue))
		{
			string defPrefix = outDefine.substr(0, 4);
			if (defPrefix == "IDD_" || defPrefix == "IDR_")
			{
				contextH << "#define H" << outDefine << " " << HID_BASE_RESOURCE + outValue << endl;
				resIDDs[outDefine] = outValue;
			}
			else if (defPrefix == "IDC_")
			{
				string key = "H" + outDefine;
				resIDCs[key] = outValue;
			}
		}
	}
	resH.close();
	contextH.close();

	// Now translate the resource.hm file into a help-compatible format.
	ifstream resHM(resourceHM.c_str());
	if (!resHM.is_open())
	{
		cerr << "Can't read " << resourceHM << endl;
		return 1;
	}
	ofstream arbH(agilitybookH.c_str());
	if (!arbH.is_open())
	{
		cerr << "Can't write " << agilitybookH << endl;
		return 1;
	}

	set<string> existingHelpIds;
	arbH << "// Autogenerated from " << resourceHM  << endl;
	while (!resHM.eof())
	{
		string line = get_line(resHM);
		string outDefine;
		unsigned long outValue;
		std::string strIDD;
		if (parse_define(line, outDefine, outValue, &strIDD))
		{
			existingHelpIds.insert(outDefine);
			arbH << "#define " << outDefine << " " << outValue << endl;
			ResourceMap::const_iterator iter = resIDCs.find(outDefine);
			if (resIDCs.end() == iter)
			{
				cerr << "Error: Cannot find " << outDefine << " base in resource.h" << endl;
				continue;
			}
			unsigned long idd = 0;
			if (0 < strIDD.length())
			{
				ResourceMap::const_iterator iterIDD = resIDDs.find(strIDD);
				if (resIDDs.end() == iterIDD)
				{
					cerr << "Error: Cannot find " << strIDD << " in resource.h" << endl;
					continue;
				}
				idd = (*iterIDD).second;
			}
			// See article Q149343: How to implement context-sensitive help for dialog controls
			unsigned long id = iter->second;
			id |= (idd << 16);
			id |= 0x80000000;
			if (outValue != id)
			{
				cerr << "Warning: " << outDefine << " value (0x" << hex << outValue << ") is not correct (0x" << hex << id << ")" << endl;
			}
		}
	}
	resHM.close();
	arbH.close();

	// Now verify that all the help ids in the HM file have context
	// help strings in the txt file.
	ifstream arbTXT(agilitybookTXT.c_str());
	if (!arbTXT.is_open())
	{
		cerr << "Can't read " << agilitybookTXT << endl;
		return 1;
	}
	while (!arbTXT.eof())
	{
		string line = get_line(arbTXT);
		string::size_type pos = line.find(".topic");
		if (0 == pos)
		{
			CStringA str(line.substr(6).c_str());
			str.TrimRight();
			str.TrimLeft();
			if (!str.IsEmpty())
			{
				string s((LPCSTR)str);
				set<string>::iterator i = existingHelpIds.find(s);
				if (existingHelpIds.end() == i)
					cerr << "Warning: " << s << " is defined in " << agilitybookTXT << " but has no definition in " << resourceHM << endl;
				else
					existingHelpIds.erase(i);
			}
		}
	}
	arbTXT.close();
	for (set<string>::iterator i = existingHelpIds.begin(); i != existingHelpIds.end(); ++i)
	{
		cerr << "Warning: " << *i << " is defined in " << resourceHM << " but has no help topic in " << agilitybookTXT << endl;
	}

	return 0;
}
