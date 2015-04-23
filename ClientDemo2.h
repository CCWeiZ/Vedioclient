// ClientDemo2.h : main header file for the CLIENTDEMO2 application
//

#if !defined(AFX_CLIENTDEMO2_H__28020418_A0C2_4907_9019_96D0CF3413AE__INCLUDED_)
#define AFX_CLIENTDEMO2_H__28020418_A0C2_4907_9019_96D0CF3413AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2App:
// See ClientDemo2.cpp for the implementation of this class
//
const ModuleFileInfomations&    GetModuleFileInformations();

#define GET_MODULE_FILE_INFO    (GetModuleFileInformations())


class CClientDemo2App : public CWinApp
{
public:
	CClientDemo2App();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDemo2App)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CClientDemo2App)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDEMO2_H__28020418_A0C2_4907_9019_96D0CF3413AE__INCLUDED_)
