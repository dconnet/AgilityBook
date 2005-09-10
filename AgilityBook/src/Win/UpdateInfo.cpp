/*
 * Copyright � 2004-2005 David Connet. All Rights Reserved.
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
 * @brief Parse and cache "version.txt" on www.agilityrecordbook.com
 * @author David Connet
 *
 * The format of the version.txt file is:
 * line 1:
 *  "ARB Version n1.n2.n3.n4"
 * line 2-n: xml (see below)
 *
 * Revision History
 * @li 2005-09-09 DRC Modified URL parsing to handle redirection.
 *                    I can now advertise the url as "www.agilityrecordbook.com"
 * @li 2004-09-28 DRC Changed how error reporting is done when loading.
 * @li 2004-08-03 DRC Created
 */

#include "stdafx.h"
#include <afxinet.h>
#include "AgilityBook.h"

#include "AgilityBookDoc.h"
#include "DlgBaseDialog.h"
#include "Element.h"
#include "HyperLink.h"
#include "Splash.h"
#include "UpdateInfo.h"
#include "VersionNum.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////

static DWORD dwHttpRequestFlags = INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT;
static const TCHAR szHeaders[] = _T("Accept: text\r\n");

/**
 * Read a file from the internet and return the data.
 */
static bool ReadHttpFile(
		CString const& inURL,
		CString& outData,
		CString& outErrMsg)
{
	CWaitCursor wait;
	outData.Empty();
	outErrMsg.Empty();
	try
	{
		// Code is based on the MSDN 'TEAR' sample.
		// Simply using OpenURL() is easier, but it doesn't handle redirection.
		DWORD dwRet;
		CString strServerName;
		CString strObject;
		INTERNET_PORT nPort;
		DWORD dwServiceType;

		if (AfxParseURL(inURL, dwServiceType, strServerName, strObject, nPort)
		&& dwServiceType == INTERNET_SERVICE_HTTP)
		{
			CInternetSession session("my version");

			CHttpConnection* pServer = session.GetHttpConnection(strServerName, nPort);
			CHttpFile* pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
				strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
			pFile->AddRequestHeaders(szHeaders);
			pFile->SendRequest();
			pFile->QueryInfoStatusCode(dwRet);
			// If we've been redirected, re-parse. In theory SendRequest()
			// should do this if NO_AUTO_REDIRECT is not specified. I haven't
			// figured out how yet...
			if (dwRet == HTTP_STATUS_MOVED
			|| dwRet == HTTP_STATUS_REDIRECT
			|| dwRet == HTTP_STATUS_REDIRECT_METHOD)
			{
				CString strNewLocation;
				pFile->QueryInfo(HTTP_QUERY_RAW_HEADERS_CRLF, strNewLocation);
				int nPlace = strNewLocation.Find(_T("Location: "));
				if (-1 == nPlace)
				{
					outErrMsg += "\nInvalid Header query: " + strNewLocation;
					strNewLocation.Empty();
				}
				else
				{
					strNewLocation = strNewLocation.Mid(nPlace + 10);
					nPlace = strNewLocation.Find('\n');
					if (nPlace > 0)
						strNewLocation = strNewLocation.Left(nPlace);
					// Put what we're accessing back into the url.
					strNewLocation += strObject;
				}
				// Now close the existing connections...
				pFile->Close();
				delete pFile;
				pFile = NULL;
				pServer->Close();
				delete pServer;
				pServer = NULL;
				// And reopen them.
				if (!strNewLocation.IsEmpty()
				&& AfxParseURL(strNewLocation, dwServiceType, strServerName, strObject, nPort))
				{
					// try again at the new location
					pServer = session.GetHttpConnection(strServerName, nPort);
					pFile = pServer->OpenRequest(CHttpConnection::HTTP_VERB_GET,
						strObject, NULL, 1, NULL, NULL, dwHttpRequestFlags);
					pFile->AddRequestHeaders(szHeaders);
					pFile->SendRequest();
					pFile->QueryInfoStatusCode(dwRet);
					if (dwRet == HTTP_STATUS_MOVED
					|| dwRet == HTTP_STATUS_REDIRECT
					|| dwRet == HTTP_STATUS_REDIRECT_METHOD)
					{
						outErrMsg += "\nURL Redirection: " + inURL;
						outErrMsg += "\nInvalid URL (2nd redirection): " + strNewLocation;
						pFile->Close();
						delete pFile;
						pFile = NULL;
					}
					else if (dwRet != HTTP_STATUS_OK)
					{
						outErrMsg += "\nInvalid URL: " + strNewLocation;
						pFile->Close();
						delete pFile;
						pFile = NULL;
					}
				}
				else
				{
					outErrMsg += "\nInvalid URL: " + strNewLocation;
				}
			}
			else if (dwRet != HTTP_STATUS_OK)
			{
				outErrMsg += "\nInvalid URL: " + inURL;
				pFile->Close();
				delete pFile;
				pFile = NULL;
			}
			if (pFile)
			{
				char buffer[1025];
				UINT nChars;
				while (0 < (nChars = pFile->Read(buffer, sizeof(buffer)-1)))
				{
					buffer[nChars] = 0;
					outData += buffer;
				}
				pFile->Close();
				delete pFile;
			}
			if (pServer)
			{
				pServer->Close();
				delete pServer;
			}
			session.Close();
		}
		else
		{
			outErrMsg += "\nInvalid URL: " + inURL;
		}
	}
	catch (CInternetException* ex)
	{
		ex->Delete();
		outData.Empty();
		outErrMsg += "\nError: Is your internet connection active?";
	}
	return (outData.GetLength() > 0);
}

/////////////////////////////////////////////////////////////////////////////

class CDownloadDialog : public CDlgBaseDialog
{
public:
	CDownloadDialog(CVersionNum inVersionNum, CWnd* pParent = NULL)
		: CDlgBaseDialog(CDownloadDialog::IDD, pParent)
		, m_VersionNum(inVersionNum)
	{
	}

private:
// Dialog Data
	enum { IDD = IDD_DOWNLOAD };
	CStatic m_ctrlText;
	CVersionNum m_VersionNum;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedYahoo();
	afx_msg void OnBnClickedSourceForge();
	DECLARE_MESSAGE_MAP()
};

void CDownloadDialog::DoDataExchange(CDataExchange* pDX)
{
	CDlgBaseDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgName)
	DDX_Control(pDX, IDC_DOWNLOAD_TEXT, m_ctrlText);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDownloadDialog, CDlgBaseDialog)
	ON_BN_CLICKED(IDC_DOWNLOAD_YAHOO, OnBnClickedYahoo)
	ON_BN_CLICKED(IDC_DOWNLOAD_SOURCEFORGE, OnBnClickedSourceForge)
END_MESSAGE_MAP()

BOOL CDownloadDialog::OnInitDialog()
{
	CDlgBaseDialog::OnInitDialog();

	CString msg;
	m_ctrlText.GetWindowText(msg);
	CString ver;
	ver.FormatMessage(msg, (LPCTSTR)m_VersionNum.GetVersionString());
	m_ctrlText.SetWindowText(ver);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDownloadDialog::OnBnClickedYahoo()
{
	CString url;
	url.LoadString(IDS_ABOUT_LINK_YAHOO_FILES);
	int nTab = url.Find('\t');
	if (0 < nTab)
		url = url.Mid(nTab+1);
	CHyperLink::GotoURL(url);
	CDlgBaseDialog::OnOK();
}

void CDownloadDialog::OnBnClickedSourceForge()
{
	CString url;
	url.LoadString(IDS_ABOUT_LINK_SOURCEFORGE);
	int nTab = url.Find('\t');
	if (0 < nTab)
		url = url.Mid(nTab+1);
	CHyperLink::GotoURL(url);
	CDlgBaseDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////

CUpdateInfo::CUpdateInfo()
	: m_VersionNum("")
	, m_VerConfig(0)
	, m_FileName()
	, m_InfoMsg()
{
}

/**
 * This will read the version.txt file and cache it.
 * In addition, it will ask to update if a newer version is found.
 * @param bVerbose Show error message.
 */
bool CUpdateInfo::ReadVersionFile(bool bVerbose)
{
	// Clear everything.
	m_VersionNum.clear();
	m_VerConfig = 0;
	m_FileName.erase();
	m_InfoMsg.erase();

	// Read the file.
	CString url;
	url.LoadString(IDS_HELP_UPDATE);
	url += "/version.txt";
	CString data, errMsg;
	if (!ReadHttpFile(url, data, errMsg))
	{
		if (bVerbose)
		{
			CSplashWnd::HideSplashScreen();
			data.LoadString(IDS_UPDATE_UNKNOWN);
			if (!errMsg.IsEmpty())
				data += errMsg;
			AfxMessageBox(data, MB_ICONEXCLAMATION);
		}
		return false;
	}

	// Now parse it into the object.
	m_VersionNum = CVersionNum(data);
	if (!m_VersionNum.Valid())
	{
		if (bVerbose)
		{
			CSplashWnd::HideSplashScreen();
			AfxMessageBox(IDS_UPDATE_UNKNOWN, MB_ICONEXCLAMATION);
		}
		return false;
	}

	// Skip the first line, that's the version.
	int n = data.Find('\n');
	if (0 < n)
		data = data.Mid(n+1);
	else
		data.Empty();
	if (!data.IsEmpty())
	{
		// The rest of the file is xml:
		// <Data>
		//   <Config ver="1" file="file">data</Config>
		// </Data>
		std::string errMsg;
		Element tree;
		if (!tree.LoadXMLBuffer((LPCSTR)data, data.GetLength(), errMsg))
		{
			if (bVerbose)
			{
				CString msg("Failed to load 'version.txt'.");
				if (0 < errMsg.length())
				{
					msg += "\n\n";
					msg += errMsg.c_str();
				}
				CSplashWnd::HideSplashScreen();
				AfxMessageBox(msg, MB_ICONEXCLAMATION);
				// Even tho we failed, we'll still report success.
				// The return code is really whether we loaded the pgm verno.
			}
		}
		else if (tree.GetName() == "Data")
		{
			int nConfig = tree.FindElement("Config");
			if (0 <= nConfig)
			{
				Element& config = tree.GetElement(nConfig);
				config.GetAttrib("ver", m_VerConfig);
				config.GetAttrib("file", m_FileName);
				m_InfoMsg = config.GetValue();
			}
		}
	}

	return true;
}

/**
 * Check the version against the web.
 */
bool CUpdateInfo::CheckProgram()
{
	bool bNeedsUpdating = false;
	ARBDate today = ARBDate::Today();
	if (IsOutOfDate())
	{
		bNeedsUpdating = true;
		CSplashWnd::HideSplashScreen();
		AfxGetApp()->WriteProfileString("Settings", "lastVerCheck", today.GetString(ARBDate::eDashYMD).c_str());
		CDownloadDialog dlg(m_VersionNum);
		dlg.DoModal();
	}
	else
		AfxGetApp()->WriteProfileString("Settings", "lastVerCheck", today.GetString(ARBDate::eDashYMD).c_str());
	return bNeedsUpdating;
}

/**
 * Check the version against the web.
 */
bool CUpdateInfo::IsOutOfDate()
{
	// If we haven't parsed the internet file yet, assume we're out-of-date.
	if (!m_VersionNum.Valid())
		return true;
	CVersionNum verThis; // This auto-loads the versioninfo.
	ASSERT(verThis.Valid());
	if (verThis < m_VersionNum)
		return true;
	else
		return false;
}

void CUpdateInfo::CheckConfig(
		CAgilityBookDoc* pDoc,
		bool bVerbose)
{
	// If the parse was successful, check for the posted config version.
	bool bUpToDate = true;
	// Cool! New config!
	if (0 < m_FileName.length() && m_VerConfig > pDoc->GetConfig().GetVersion())
	{
		bUpToDate = false;
		CString msg("The configuration has been updated. Would you like to merge the new one with your data?");
		if (0 < m_InfoMsg.length())
		{
			// If the info contains a note, append it.
			// A note will often give a brief description of things
			// the user must do. For instance, from v4->5, USDAA
			// titling pts were removed from Tournament Jumpers and
			// Snooker to allow for non-titling runs. In case the
			// user saved some that way, we need to warn them.
			msg += "\n\n";
			msg += m_InfoMsg.c_str();
		}
		CSplashWnd::HideSplashScreen();
		if (IDYES == AfxMessageBox(msg, MB_ICONQUESTION | MB_YESNO))
		{
			// Load the config.
			CString url;
			url.LoadString(IDS_HELP_UPDATE);
			url += "/";
			url += m_FileName.c_str();
			CString strConfig, errMsg;
			if (ReadHttpFile(url, strConfig, errMsg))
			{
				Element tree;
				std::string errMsg;
				if (!tree.LoadXMLBuffer((LPCSTR)strConfig, strConfig.GetLength(), errMsg))
				{
					CString msg("Failed to load '");
					msg += url;
					msg += "'.";
					if (0 < errMsg.length())
					{
						msg += "\n\n";
						msg += errMsg.c_str();
					}
					AfxMessageBox(msg, MB_ICONEXCLAMATION);
				}
				else if (tree.GetName() == "DefaultConfig")
				{
					strConfig.Empty();
					ARBVersion version = ARBAgilityRecordBook::GetCurrentDocVersion();
					tree.GetAttrib(ATTRIB_BOOK_VERSION, version);
					int nConfig = tree.FindElement(TREE_CONFIG);
					if (0 <= nConfig)
					{
						CErrorCallback err;
						ARBAgilityRecordBook book;
						if (!book.GetConfig().Load(tree.GetElement(nConfig), version, err))
						{
							if (0 < err.m_ErrMsg.length())
								AfxMessageBox(err.m_ErrMsg.c_str(), MB_ICONWARNING);
						}
						else
						{
							pDoc->ImportConfiguration(book.GetConfig());
							pDoc->SetModifiedFlag(TRUE);
						}
					}
				}
			}
		}
	}
	if (bUpToDate && bVerbose)
	{
		CSplashWnd::HideSplashScreen();
		AfxMessageBox(IDS_UPDATE_CURRENT, MB_ICONINFORMATION);
	}
}

void CUpdateInfo::AutoUpdateProgram()
{
	if (ReadVersionFile(false))
		CheckProgram();
}

void CUpdateInfo::AutoCheckConfiguration(CAgilityBookDoc* pDoc)
{
	// If we're opening a doc and we've checked the internet
	// and there is a more current version, do not continue
	// to check the config. This prevents loading a config that
	// uses a newer file version.
	if (IsOutOfDate())
		return;
	CheckConfig(pDoc, false);
}

void CUpdateInfo::UpdateConfiguration(CAgilityBookDoc* pDoc)
{
	// Only continue if we parsed the version.txt file
	// AND the version is up-to-date.
	if (!ReadVersionFile(true))
		return;
	if (CheckProgram())
		return;
	CheckConfig(pDoc, true);
}
