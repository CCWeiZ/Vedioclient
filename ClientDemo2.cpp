// ClientDemo2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "ClientDemo2Dlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2App

BEGIN_MESSAGE_MAP(CClientDemo2App, CWinApp)
	//{{AFX_MSG_MAP(CClientDemo2App)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2App construction

CClientDemo2App::CClientDemo2App()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientDemo2App object

CClientDemo2App theApp;

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2App initialization

BOOL CClientDemo2App::InitInstance()
{
	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CMultiLanguage::SetIniFileName(GET_MODULE_FILE_INFO.strPath);

	CClientDemo2Dlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

const ModuleFileInfomations&  GetModuleFileInformations();
const ModuleFileInfomations& GetModuleFileInformations()
{
	static ModuleFileInfomations __s_mi;
	static BOOL bLoad = FALSE;
	
	if(!bLoad)
	{
		// Get application's full path.
		
		::GetModuleFileName(NULL, __s_mi.strFullPath.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
		__s_mi.strFullPath.ReleaseBuffer();
		
		// Break full path into seperate components.
		_splitpath(
			__s_mi.strFullPath, 
			__s_mi.strDrive.GetBufferSetLength(_MAX_DRIVE + 1),
			__s_mi.strDir.GetBufferSetLength(_MAX_DIR + 1),
			__s_mi.strName.GetBufferSetLength(_MAX_FNAME + 1),
			__s_mi.strExt.GetBufferSetLength(_MAX_EXT + 1));
		__s_mi.strDrive.ReleaseBuffer();
		__s_mi.strDir.ReleaseBuffer();
		__s_mi.strName.ReleaseBuffer();
		__s_mi.strExt.ReleaseBuffer();
		
		
		TCHAR   sDrive[_MAX_DRIVE];   
		TCHAR   sDir[_MAX_DIR];   
		TCHAR   sFilename[_MAX_FNAME],Filename[_MAX_FNAME];   
		TCHAR   sExt[_MAX_EXT];   
		GetModuleFileName(AfxGetInstanceHandle(),   Filename,   _MAX_PATH);   
		_tsplitpath(Filename,   sDrive,   sDir,   sFilename,   sExt);   
		CString   homeDir(CString(sDrive)   +   CString(sDir));   
		int   nLen   =   homeDir.GetLength();   
		if(homeDir.GetAt(nLen-1)   !=   _T('\\'))   
			homeDir   +=   _T('\\');   
		
		__s_mi.strPath = homeDir;
		
		bLoad = TRUE;
	}
	
	return __s_mi;
}