// This parses the auto-generated resource.hm in order to convert the hex
// #defines into decimal so the hhc compiler will work with the text popups.

/* This is what it looks like:
// Microsoft Visual C++ generated Help ID include file.
// Used by AgilityBook.rc
//
#define HIDC_OPTIONS_FILTER_VENUES      0x80ee04f8    // IDD_VIEW_OPTIONS_FILTER
#define HIDC_OPTIONS_FILTER_VENUES_ALL  0x80ee04f6    // IDD_VIEW_OPTIONS_FILTER
#define HIDC_OPTIONS_FILTER_VENUES_SELECTED 0x80ee04f7    // IDD_VIEW_OPTIONS_FILTER

This is what we need:
#define HIDC_OPTIONS_FILTER_VENUES      2163082488
#define HIDC_OPTIONS_FILTER_VENUES_ALL  2163082486
#define HIDC_OPTIONS_FILTER_VENUES_SELECTED 2163082487
*/

#include <iostream>
#include <tchar.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if (2 > argc)
	{
		std::cerr << "Usage: " << argv[0] << " resource.hm" << std::endl;
		return 1;
	}

	return 0;
}
