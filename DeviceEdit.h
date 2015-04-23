#include "afxcmn.h"
#include "afxwin.h"
#include "..\\bin\\NetSdk.h"

#if !defined(AFX_DEVICEEDIT_H__FB9C507E_0BBB_4F90_A00F_E27F60A4DE9E__INCLUDED_)
#define AFX_DEVICEEDIT_H__FB9C507E_0BBB_4F90_A00F_E27F60A4DE9E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceEdit dialog

class CDeviceEdit : public CDialog
{
// Construction
public:
	CDeviceEdit(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeviceEdit)
	enum { IDD = IDD_CONFIG_DLG };
	CEdit	m_EditDevName;
	CEdit	m_EditPort;
	CEdit	m_EditPsw;
	CEdit	m_EditTotal;
	CEdit	m_EditUserName;
	CListCtrl	m_listDevice;
	CString	m_editIP;
	CString	m_strHostIP;
	BOOL	m_bIP;
	BOOL	m_bSerialID;
	UINT	m_nPort;
	CString	m_DevSerialID;
	CString	m_strDevName;
	CString m_strUserName;
	CString m_strPsw;
	CString m_nChannel;
	CString m_strIP;
	UINT	m_HostPort;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceEdit)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	//{{AFX_MSG(CDeviceEdit)
	afx_msg void OnDestroy();
	virtual BOOL OnInitDialog();
	afx_msg void OnOk();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	int m_nType;
	DEV_INFO m_devInfo;
	BOOL WriteXML();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	static UINT SearchDevice(LPVOID parm);
	afx_msg void OnBnClickedBtnSearch();
	SDK_CONFIG_NET_COMMON_V2 m_Device[100];
	int m_nDevNum;
	HANDLE m_hSearch;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	long m_lUpgradeHandle;
	afx_msg void OnBnClickedBtnSetting();
	void DelArpEntry(DWORD dwAddr);
	afx_msg void OnNMDblclkListUpgrade(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListUpgrade(NMHDR *pNMHDR, LRESULT *pResult);
//	CPromptDlg m_PromptDlg;
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnPaint();
private:
	std::map<int ,DEV_INFO> m_mapDev;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEEDIT_H__FB9C507E_0BBB_4F90_A00F_E27F60A4DE9E__INCLUDED_)
