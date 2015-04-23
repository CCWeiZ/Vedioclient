#if !defined(AFX_NETALARMDLG_H__0EC3C93C_A27E_4E65_914A_2CCFD302D1B1__INCLUDED_)
#define AFX_NETALARMDLG_H__0EC3C93C_A27E_4E65_914A_2CCFD302D1B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NetAlarmDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CNetAlarmDlg dialog

class CNetAlarmDlg : public CDialog
{
// Construction
public:
	CNetAlarmDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNetAlarmDlg)
	enum { IDD = IDD_DLG_NETALARM };
	CComboBox	m_cmbChannel;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNetAlarmDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNetAlarmDlg)
	afx_msg void OnBtnStart();
	afx_msg void OnBtnStop();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	DEV_INFO *m_pDevInfo;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NETALARMDLG_H__0EC3C93C_A27E_4E65_914A_2CCFD302D1B1__INCLUDED_)
