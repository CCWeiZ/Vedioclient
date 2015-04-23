// IPEdit.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "IPEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CIPEdit dialog


CIPEdit::CIPEdit(CWnd* pParent /*=NULL*/)
	: CDialog(CIPEdit::IDD, pParent)
{
	//{{AFX_DATA_INIT(CIPEdit)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CIPEdit::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPEdit)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CIPEdit, CDialog)
	//{{AFX_MSG_MAP(CIPEdit)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIPEdit message handlers
