#if !defined(AFX_IPEDIT_H__EDB7F79C_D6F3_450E_B682_19B5ACCCE8CF__INCLUDED_)
#define AFX_IPEDIT_H__EDB7F79C_D6F3_450E_B682_19B5ACCCE8CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// IPEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CIPEdit dialog

class CIPEdit : public CDialog
{
// Construction
public:
	CIPEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CIPEdit)
	enum { IDD = IDD_DIALOG_IPEdit };
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CIPEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CIPEdit)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IPEDIT_H__EDB7F79C_D6F3_450E_B682_19B5ACCCE8CF__INCLUDED_)
