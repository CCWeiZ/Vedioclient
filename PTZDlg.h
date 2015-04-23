#if !defined(AFX_PTZDLG_H__EA8AB737_FD23_42BB_BC3A_BCDFFFA1D9B9__INCLUDED_)
#define AFX_PTZDLG_H__EA8AB737_FD23_42BB_BC3A_BCDFFFA1D9B9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PTZDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPTZDlg dialog

class CPTZDlg : public CDialog
{
// Construction
public:
	CPTZDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPTZDlg)
	enum { IDD = IDD_PTZ_DLG };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPTZDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPTZDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PTZDLG_H__EA8AB737_FD23_42BB_BC3A_BCDFFFA1D9B9__INCLUDED_)
