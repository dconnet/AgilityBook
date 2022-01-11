/*
 * Copyright (c) David Connet. All Rights Reserved.
 *
 * License: See License.txt
 */

/**
 * @file
 * @brief Defines the class behaviors for the application.
 * @author David Connet
 *
 * Revision History
 * 2013-10-30 Fixed a problem where msi was spawned hidden.
 * 2009-09-13 Add support for wxWidgets 2.9, deprecate tstring.
 * 2009-03-01 Ported to wxWidgets.
 * 2007-02-08 Created
 */

#include "stdafx.h"
#include "CalSites.h"

#include "ARBConfigCalSite.h"
#include "CalendarSites.h"

#include "../Win/ConfigHandler.h"
#include "../Win/SetupAppARB.h"
#include "LibARBWin/ImageHelperBase.h"
#include "LibARBWin/SetupApp.h"
#include <wx/cmdline.h>
#include <wx/config.h>

#ifdef __WXMSW__
#include <wx/msw/msvcrt.h>
#endif


wxIMPLEMENT_APP(CCalSitesApp);


CCalSitesApp::CCalSitesApp()
	: CBaseApp(ARB_CONFIG_ENTRY, wxEmptyString, ARBLanguageCatalog::Embedded)
	, m_Localization()
{
	m_BaseInfoName = ARB_CONFIG_INFO;
}


bool CCalSitesApp::InitLanguage()
{
	IARBLocalization::Init(&m_Localization);
	return CBaseApp::InitLanguage();
}


bool CCalSitesApp::OnInit()
{
	if (!CBaseApp::OnInit())
		return false;

	/*
	static const wxCmdLineEntryDesc cmdLineDesc[] = {
		{wxCMD_LINE_SWITCH, "g", "generate", "Ignore all other options and display a dialog allowing MD5 generation"},
		{wxCMD_LINE_OPTION, "f", "file", "Downloaded file"},
		{wxCMD_LINE_NONE},
	};
	wxCmdLineParser cmdline(cmdLineDesc, argc, argv);
	if (0 != cmdline.Parse(true))
	{
		BaseAppCleanup(true);
		return false;
	}
	*/
	/*
	 * old DTD entries
<!--
Define a calendar source site.
 name: Name displayed in plugin dialog
 search: Url to request data from with special formatting
	  "!L!": Add selected LocCodes (code1+code2...)
	  "!V!": Add selected venues (v1+v2...)
 help: Url to page describing what you get (for instance if data is limited)
	   (Note, not currently used. The idea is this would eventually be used
	   at a site where they provide limited data, but if you pay, you get more.
	   This would be a link to a page describing the differences and trying to
	   convince the user to subscribe. I haven't figured out where to put this
	   in the UI yet)
Note: The requested data will be returned using with the venue code. We'll
run a 'fixup' routine before actually importing the Calendar data.
This means that the URL query can be specified with no locations or venues.
Depending on the server, this could mean 'return all'. When processing returned
data, no assumptions are made based on the query we started with.
-->
<!ELEMENT CalSite (Desc?, LocCode*, VenueCode*) >
	<!ATTLIST CalSite
		name CDATA #REQUIRED
		search CDATA #REQUIRED
		help CDATA #IMPLIED
		>
<!--
Description of source, displayed in plugin dialog.
-->
<!ELEMENT Desc (#PCDATA) >
<!--
List of locations we can select from.
 code: Transmitted to url
 name: Displayed name
-->
<!ELEMENT LocCode EMPTY >
	<!ATTLIST LocCode
		code CDATA #REQUIRED
		name CDATA #REQUIRED
		>
<!--
List of venues we can select from.
 code: Transmitted to url
 venue: mapping to our names, if not set, code is same as ours
-->
<!ELEMENT VenueCode EMPTY >
	<!ATTLIST VenueCode
		code CDATA #REQUIRED
		venue CDATA #IMPLIED
		>

	<CalSite help="none" name="AgilityEvents" search="http://beta.agilityrecordbook.com/cal.php">
	  <Desc>Test data for AgilityEvents.net</Desc>
	  <LocCode code="AK" name="Alaska"/>
	  <LocCode code="AL" name="Alabama"/>
	  <LocCode code="AR" name="Arkansas"/>
	  <LocCode code="AZ" name="Arizona"/>
	  <LocCode code="CA" name="California"/>
	  <LocCode code="CO" name="Colorado"/>
	  <LocCode code="CT" name="Connecticut"/>
	  <LocCode code="DE" name="Delaware"/>
	  <LocCode code="FL" name="Florida"/>
	  <LocCode code="GA" name="Georgia"/>
	  <LocCode code="HI" name="Hawaii"/>
	  <LocCode code="IA" name="Iowa"/>
	  <LocCode code="ID" name="Idaho"/>
	  <LocCode code="IL" name="Illinois"/>
	  <LocCode code="IN" name="Indiana"/>
	  <LocCode code="KS" name="Kansas"/>
	  <LocCode code="KY" name="Kentucky"/>
	  <LocCode code="LA" name="Louisianna"/>
	  <LocCode code="MA" name="Massachussetts"/>
	  <LocCode code="MD" name="Maryland"/>
	  <LocCode code="ME" name="Maine"/>
	  <LocCode code="MI" name="Michigan"/>
	  <LocCode code="MN" name="Minnesota"/>
	  <LocCode code="MO" name="Missouri"/>
	  <LocCode code="MS" name="Mississippi"/>
	  <LocCode code="MT" name="Montana"/>
	  <LocCode code="NC" name="North Carolina"/>
	  <LocCode code="ND" name="North Dakota"/>
	  <LocCode code="NE" name="Nebraska"/>
	  <LocCode code="NH" name="New Hampshire"/>
	  <LocCode code="NJ" name="New Jersey"/>
	  <LocCode code="NM" name="New Mexico"/>
	  <LocCode code="NV" name="Nevada"/>
	  <LocCode code="NY" name="New York"/>
	  <LocCode code="OH" name="Ohio"/>
	  <LocCode code="OK" name="Oklahoma"/>
	  <LocCode code="OR" name="Oregon"/>
	  <LocCode code="PA" name="Pennsylvania"/>
	  <LocCode code="RI" name="Rhode Island"/>
	  <LocCode code="SC" name="South Carolina"/>
	  <LocCode code="SD" name="South Dakota"/>
	  <LocCode code="TN" name="Tennessee"/>
	  <LocCode code="TX" name="Texas"/>
	  <LocCode code="UT" name="Utah"/>
	  <LocCode code="VA" name="Virginia"/>
	  <LocCode code="VT" name="Vermont"/>
	  <LocCode code="WA" name="Washington"/>
	  <LocCode code="WI" name="Wisconsin"/>
	  <LocCode code="WV" name="West Virginia"/>
	  <LocCode code="WY" name="Wyoming"/>
	  <VenueCode code="AAC" venue="Agility Association of Canada"/>
	  <VenueCode code="AKC" venue="American Kennel Club"/>
	  <VenueCode code="ASCA" venue="Australian Shepherd Club of America"/>
	  <VenueCode code="CKC" venue="Canadian Kennel Club"/>
	  <VenueCode code="CPE" venue="Canine Performance Events"/>
	  <VenueCode code="DOCNA" venue="Dogs On Course in North America"/>
	  <VenueCode code="FCI" venue="Fédération Cynologique Internationale"/>
	  <VenueCode code="NADAC" venue="North American Dog Agility Council"/>
	  <VenueCode code="SCC" venue="Société Centrale Canine"/>
	  <VenueCode code="TDAA" venue="Teacup Dogs Agility Association"/>
	  <VenueCode code="UKC" venue="United Kennel Club"/>
	  <VenueCode code="USDAA" venue="United States Dog Agility Association"/>
	</CalSite>
	*/

	// TODO: loading and saving of entries
	ARBConfigCalSiteList sites;

	CCalendarSites calSites;
	calSites.FindEntries(sites);

	BaseAppCleanup(true);
	return false;
}


bool CCalSitesApp::GetResImageName(
	wxArtID const& id,
	wxArtClient const& client,
	std::wstring& outName,
	bool& outSvg,
	bool& outCall) const
{
	outSvg = false;
	outCall = false;

	bool found = true;
	if (id == ImageMgrApp)
	{
		// No SVG for the icon yet
		//#if wxCHECK_VERSION(3, 1, 6)
		// outName = L"AgilityBook";
		// outSvg = true;
		//#else
		if (client == wxART_MESSAGE_BOX)
			outName = L"AgilityBook32";
		else
			outName = L"AgilityBook16";
		//#endif
	}

	else if (id == ImageMgrChecked)
		outName = L"checked";
	else if (id == ImageMgrUnChecked)
		outName = L"unchecked";

	else
		found = false;
	return found;
}

/////////////////////////////////////////////////////////////////////////////

CErrorCallback::CErrorCallback()
	: ARBErrorCallback(m_ErrMsg)
{
}


bool CErrorCallback::OnError(wchar_t const* const pMsg)
{
	return wxYES == wxMessageBox(pMsg, wxMessageBoxCaptionStr, wxYES_NO | wxNO_DEFAULT | wxCENTRE | wxICON_EXCLAMATION);
}
