// ClientDemo2Dlg.h : header file
//

#if !defined(AFX_CLIENTDEMO2DLG_H__52FB4B39_2C5D_4347_9F2C_51572258F99F__INCLUDED_)
#define AFX_CLIENTDEMO2DLG_H__52FB4B39_2C5D_4347_9F2C_51572258F99F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CClientDemo2Dlg dialog

#include "DeviceEditDlg.h"
#include "VideoWnd.h"
#include "NetAlarmDlg.h"
#include "vlc.h"

class CClientDemo2Dlg : public CDialog
{
// Construction
public:
	CClientDemo2Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CClientDemo2Dlg)
	enum { IDD = IDD_CLIENTDEMO2_DIALOG };
	CButton	m_checkOsd;
	CComboBox	m_cmbWnd;
	CString	m_VideoPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CClientDemo2Dlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CClientDemo2Dlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnCapture();
	afx_msg void OnBtnDevice();
	afx_msg void OnBtnNetAlarm();
	afx_msg void OnBtnSave();
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSelchangeCombowndnum();
	afx_msg void OnBtnPlayback();
	afx_msg void OnButtonAssure();
	afx_msg void OnButtonStop();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
		

public:
	int m_nTotalWnd;
	int m_nCurIndex;
	CDeviceEditDlg      m_devDLg;
	CVideoWnd			m_videoWnd[MAXWNDNUM];
	CNetAlarmDlg		m_NetAlarmDLg;
	DEV_INFO			m_devInfo;
	void	ArrayWindow(WORD iNumber);
private:
	BOOL	InitSDK();
	BOOL	ExitSDK();
	
	void	DrawActivePage(BOOL bActive);
	void	SetColor(int nIndex);
public:
	void ArrayWindow1(WORD iNumber);
	bool	DealwithAlarm(long lDevcID, char* pBuf, DWORD dwLen);
	BOOL	SetDevChnColor(DWORD nBright, DWORD nContrast, DWORD nSaturation, DWORD nHue);
	void	SetActiveWnd(int nIndex);
	int		Connect(DEV_INFO *pDev, int nChannel, int nWndIndex = -1);
	void	SetColor(DWORD nBright, DWORD nContrast, DWORD nSaturation, DWORD nHue);
	void	PtzControl(DWORD dwBtn, bool dwStop);
	void	KeyBoardMsg(DWORD dwValue, DWORD dwState);
	void	NetAlarmMsg(DWORD dwValue, DWORD dwState);
	Devc_Map *GetDeviceMap();
	void	SetDevInfo(DEV_INFO *pDev);
	void	ReConnect(LONG lLoginID, char *pchDVRIP, LONG nDVRPort);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLIENTDEMO2DLG_H__52FB4B39_2C5D_4347_9F2C_51572258F99F__INCLUDED_)
