// PTZDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "PTZDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPTZDlg dialog


CPTZDlg::CPTZDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPTZDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPTZDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CPTZDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPTZDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPTZDlg, CDialog)
	//{{AFX_MSG_MAP(CPTZDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTZDlg message handlers
