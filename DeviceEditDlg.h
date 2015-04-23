#if !defined(AFX_DEVICEEDITDLG_H__08741863_F075_4BAD_84C4_6A096763708A__INCLUDED_)
#define AFX_DEVICEEDITDLG_H__08741863_F075_4BAD_84C4_6A096763708A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DeviceEditDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDeviceEditDlg dialog

typedef	std::map<long , DEV_INFO*> Devc_Map;   //id index number is database index ID

class CDeviceEditDlg : public CDialog
{
// Construction
public:
	CDeviceEditDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDeviceEditDlg)
	enum { IDD = IDD_DEVICE_DLG };
	CTreeCtrl	m_deviceTree;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDeviceEditDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDeviceEditDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnRclickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkTree1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnClickTree1(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	afx_msg void AddDevice();
	afx_msg void DeleteDevice();
	afx_msg void Connect();
	afx_msg void OnTalk();
	afx_msg void OnDeviceCapture();
	DECLARE_MESSAGE_MAP()


private:
	Devc_Map m_devMap;
	Devc_Map m_devReconnetMap;
	DEV_INFO m_talkDevice;
	long m_lTalkHandle;
	BYTE* m_pTalkDecodeBuf;

public:
	Devc_Map *GetDeviceMap()
	{
		return &m_devMap;
	}

	BOOL StartTalkPlay(int nPort);
	BOOL StartTalk(DEV_INFO *pDevice);
	BOOL StopTalk(DEV_INFO *pDevice);
	BOOL StopTalkPlay(int nPort);
	BOOL StopTalkPlay(LPBYTE *pBuf, DWORD nBufLen);
	BOOL SendTalkDate(LPBYTE pDataBuffer, DWORD dwDataLength);
	BOOL InputTalkData(BYTE *pBuf, DWORD nBufLen);
	DEV_INFO ReadXML();
	long DevLogin(DEV_INFO* pdev);
	void ReConnect(LONG lLoginID, char *pchDVRIP,  LONG nDVRPort);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DEVICEEDITDLG_H__08741863_F075_4BAD_84C4_6A096763708A__INCLUDED_)
