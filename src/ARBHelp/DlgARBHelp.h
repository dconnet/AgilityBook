#pragma once

#include <map>
#include "resource.h"
#include "ARBTypes.h"
#include "DlgHelpPage1.h"
#include "DlgHelpPage2.h"
#include "DlgHelpPage3.h"
#include "DlgHelpPageEdit.h"

class CDlgARBHelp : public CPropertySheet
{
	DECLARE_DYNAMIC(CDlgARBHelp)
public:
	CDlgARBHelp(bool decode);

	void AddRegistryInfo(LPCTSTR inData);
	void SetARBFileStatus(CString const& inFileName, bool bInclude = true);
	typedef std::map<CString, bool> FileMap;
	FileMap const& GetARBFiles() const			{return m_IncFile;}
	void SendIt();

// Implementation
protected:
	HICON m_hIcon;
	bool m_Decode;
	CDlgHelpPage1 m_pagePage1;
	CDlgHelpPage2 m_pagePage2;
	CDlgHelpPage3 m_pagePage3;
	CDlgHelpPageEdit m_pageEdit;
	ARBString m_RegInfo;
	FileMap m_IncFile;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	DECLARE_MESSAGE_MAP()
};
