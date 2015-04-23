// Localpalyback.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "Localpalyback.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLocalpalyback dialog


CLocalpalyback::CLocalpalyback(CWnd* pParent /*=NULL*/)
	: CDialog(CLocalpalyback::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLocalpalyback)
	//}}AFX_DATA_INIT
}


void CLocalpalyback::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLocalpalyback)
	DDX_Control(pDX, IDC_SLIDERFILE, m_ctrlSlider);
	DDX_Control(pDX, IDC_STATIC_PLAY, m_playwindow);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLocalpalyback, CDialog)
	//{{AFX_MSG_MAP(CLocalpalyback)
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BUTTON_PLAY, OnButtonPlay)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, OnButtonPause)
	ON_BN_CLICKED(IDC_BUTTON_FAST, OnButtonFast)
	ON_BN_CLICKED(IDC_BUTTON_SLOW, OnButtonSlow)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLocalpalyback message handlers

void __stdcall CLocalpalyback::InfoFramCallback(LONG nPort, LONG nType, LPCSTR pBuf,LONG nSize, LONG nUser)
{
	CLocalpalyback *pThis = (CLocalpalyback*)nUser;
	//收到信息帧, 0x03 代表GPRS信息
	if ( nType == 0x03 )
	{
		pThis->m_strInfoFrame[nPort] = pBuf;
	}
}

void CLocalpalyback::OnButtonOpen() 
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(TRUE);
	if(dlg.DoModal()==IDOK)
	{
		m_FilePathName=dlg.GetPathName();
		m_PlayerStatus=STATUS_STOP;
		m_bplaythread=false;
		H264_PLAY_CloseFile(m_nPort);
		H264_PLAY_CloseStream(m_nPort);
		H264_PLAY_Stop(m_nPort);
		m_FastSpeed=0;
		GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast"));
		m_SlowSpeed=0;
		GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow"));
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(_CS("PlayBack.Pause"));
		OnButtonPlay();
	}
}

void __stdcall CLocalpalyback::SDKPlayFileEndCallback(LONG nPort,LONG nUser)
{
	CLocalpalyback * p = (CLocalpalyback*)nUser;
	if ( NULL == p )
	{
		return;
	}
	
	p->PlayEndCallback(nPort);
}

void CALLBACK PlaybackdrawOSDCall(LONG nPort,HDC hDc,LONG nUser)
{
	CLocalpalyback *pThis = (CLocalpalyback*)nUser;
	pThis->drawOSD(nPort,hDc);
}

void CALLBACK FileRefDone(LONG nReserved,LONG nUser)
{
}

void CLocalpalyback::drawOSD(LONG nPort,HDC hDc)
{
	if ( m_strInfoFrame[nPort] != "" )
	{
		HFONT oldfont;
		oldfont = (HFONT)::GetCurrentObject( hDc, OBJ_FONT );
		
		
		//改变字体颜色
		::SetTextColor( hDc, RGB(255,0,0) );
		
		//更改字体
		LOGFONT lf;
		::GetObject( oldfont, sizeof(lf), &lf );
		strcpy( lf.lfFaceName, "Arial" );
		lf.lfWeight = FW_BOLD;
		HFONT out_ft;		//创建的字体对象
		out_ft = ::CreateFontIndirect( &lf );
		
		::SelectObject( hDc, out_ft );
		
		TextOut( hDc, 10, 10, m_strInfoFrame[nPort], m_strInfoFrame[nPort].GetLength() );
		
		::SelectObject( hDc, oldfont );
	}
}

void CLocalpalyback::PlayEndCallback(int nPort)
{
	CRect rect;
	KillTimer(1);
	m_ctrlSlider.SetPos(0);
	GetClientRect(&rect);
	InvalidateRect(rect);
}


void CLocalpalyback::OnButtonPlay() 
{
	// TODO: Add your control notification handler code here
		if(m_PlayerStatus==STATUS_STOP)
	{
		m_bplaythread=true;

		H264_PLAY_SetFileRefCallBack(m_nPort, FileRefDone, (DWORD)this);
		
		if(H264_PLAY_OpenFile(m_nPort, m_FilePathName))
		{
			H264_PLAY_SetInfoFrameCallBack(m_nPort, InfoFramCallback,(DWORD)this);

			m_TotalTime = H264_PLAY_GetFileTime( m_nPort); // Total play time
			
			H264_PLAY_SetFileEndCallBack(m_nPort,SDKPlayFileEndCallback,(DWORD)this);
			
			H264_PLAY_RigisterDrawFun(m_nPort, PlaybackdrawOSDCall, (DWORD)this);
			if(H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd)==false)
			{
				MessageBox(_CS("Error.PlayFailed"));
				return;
			}

			GetDlgItem(IDC_SLIDERFILE)->EnableWindow(TRUE);
		}
		else
		{
			int nErr=H264_PLAY_GetLastError(m_nPort);
			if(nErr == H264_PLAY_CHECK_FILE_ERROR)
			{
				MessageBox(_CS("Error.CheckFileError"));
			}
			else
			{
				MessageBox(_CS("Error.OpenFileFail"));
			}
			return;
		}

		m_PlayerStatus=STATUS_PLAY;
	}
	else if(m_PlayerStatus==STATUS_FAST||m_PlayerStatus==STATUS_SLOW)
	{
		H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd);
		m_PlayerStatus=STATUS_PLAY;
		m_FastSpeed=0;
		GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast"));
		m_SlowSpeed=0;
		GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow"));
	}
	else if(m_PlayerStatus==STATUS_PAUSE)
	{
		H264_PLAY_Pause(m_nPort,FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(_CS("PlayBack.Pause"));
		m_PlayerStatus=STATUS_PLAY;
	}
	
	SetTimer(1,1000,NULL);	
}

void CLocalpalyback::OnButtonStop() 
{
	// TODO: Add your control notification handler code here
	m_bplaythread=false;
	m_PlayerStatus=STATUS_STOP;
	H264_PLAY_CloseFile(m_nPort);
	H264_PLAY_CloseStream(m_nPort);
	H264_PLAY_Stop(m_nPort);
	m_FastSpeed=0;
	GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast"));
	m_SlowSpeed=0;
	GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow"));
	GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(_CS("PlayBack.Pause"));

	if ( m_hPlayThread )
	{
		CloseHandle(m_hPlayThread); //close the Thread
	}

	KillTimer(1);
	GetDlgItem(IDC_SLIDERFILE)->EnableWindow(FALSE);
	Invalidate(TRUE);
}

void CLocalpalyback::OnButtonPause() 
{
	// TODO: Add your control notification handler code here
	if(m_PlayerStatus==STATUS_PLAY||m_PlayerStatus==STATUS_SLOW||m_PlayerStatus==STATUS_FAST)
	{
		H264_PLAY_Pause(m_nPort,TRUE);
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(_CS("PlayBack.Recover"));
		m_PlayerStatus=STATUS_PAUSE;
		GetDlgItem(IDC_SLIDERFILE)->EnableWindow(FALSE);
	}
	else if(m_PlayerStatus==STATUS_PAUSE)
	{
		H264_PLAY_Pause(m_nPort,FALSE);
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(_CS("PlayBack.Pause"));
		m_PlayerStatus=STATUS_PLAY;
		GetDlgItem(IDC_SLIDERFILE)->EnableWindow(TRUE);
	}
}

void CLocalpalyback::OnButtonFast() 
{
	// TODO: Add your control notification handler code here
	if(m_PlayerStatus==STATUS_PLAY||m_PlayerStatus==STATUS_FAST||m_PlayerStatus==STATUS_SLOW)
	{
		if(m_PlayerStatus==STATUS_SLOW)
		{
			H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd);
			m_SlowSpeed=0;
			GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow"));
		}
		if(m_FastSpeed<4)
		{
			H264_PLAY_Fast(m_nPort);
			m_FastSpeed++;
			CString temp;
			temp.Format("%d",m_FastSpeed);
			GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast")+_T(temp));
			m_PlayerStatus=STATUS_FAST;
		}
		else
		{
			m_FastSpeed=0;
			H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd);
			H264_PLAY_Fast(m_nPort);
			m_FastSpeed++;
			CString temp;
			temp.Format("%d",m_FastSpeed);
			GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast")+_T(temp));
			m_PlayerStatus=STATUS_FAST;
		}
		GetDlgItem(IDC_SLIDERFILE)->EnableWindow(TRUE);
	}
}

void CLocalpalyback::OnButtonSlow() 
{
	// TODO: Add your control notification handler code here
	if(m_PlayerStatus==STATUS_PLAY||m_PlayerStatus==STATUS_SLOW||m_PlayerStatus==STATUS_FAST)
	{
		if(m_PlayerStatus==STATUS_FAST)
		{
			H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd);
			m_FastSpeed=0;
			GetDlgItem(IDC_BUTTON_FAST)->SetWindowText(_CS("PlayBack.Fast"));
		}
		if(m_SlowSpeed<4)
		{
			H264_PLAY_Slow(m_nPort);
			m_PlayerStatus=STATUS_SLOW;
			m_SlowSpeed++;
			CString temp;
			temp.Format("%d",m_SlowSpeed);
			GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow")+_T(temp));
		}
		else
		{
			m_SlowSpeed=0;
			H264_PLAY_Play(m_nPort,m_playwindow.m_hWnd);
			H264_PLAY_Slow(m_nPort);
			m_PlayerStatus=STATUS_SLOW;
			m_SlowSpeed++;
			CString temp;
			temp.Format("%d",m_SlowSpeed);
			GetDlgItem(IDC_BUTTON_SLOW)->SetWindowText(_CS("PlayBack.Slow")+_T(temp));
			
		}
		GetDlgItem(IDC_SLIDERFILE)->EnableWindow(TRUE);
	}
}

void CLocalpalyback::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	float pos = H264_PLAY_GetPlayPos( m_nPort );
	//m_ctrlSlider.SetPos( pos * 100);

	int nTime = H264_PLAY_GetPlayedTimeEx( m_nPort );
	CString strTmp;
	strTmp.Format("time = %d", nTime);
	OutputDebugString(strTmp);

	SetSliderPos(pos);
	CDialog::OnTimer(nIDEvent);
	CDialog::OnTimer(nIDEvent);
}

void CLocalpalyback::SetSliderPos(float nplaypos)
{
	if ( m_PreTime > 0 )
	{
		DWORD dwTime = GetTickCount();
		if ( (dwTime - m_PreTime) >= 2000 )
		{
			m_ctrlSlider.SetPos( nplaypos * 1000 );
			
			m_PreTime = 0;
		}
	}
	else
	{
		m_ctrlSlider.SetPos( nplaypos * 1000 );
	}
}


unsigned int CLocalpalyback::StartPlay(LPVOID parm)
{
	CLocalpalyback *pThis = ( CLocalpalyback* ) parm;
	H264_PLAY_SetFileRefCallBack(pThis->m_nPort, FileRefDone, (DWORD)parm);

	if(H264_PLAY_OpenFile(pThis->m_nPort,pThis->m_FilePathName))
	{
		pThis->m_TotalTime = H264_PLAY_GetFileTime( pThis->m_nPort); // Total play time
		
		H264_PLAY_SetFileEndCallBack(pThis->m_nPort,SDKPlayFileEndCallback,(DWORD)parm);

		if(H264_PLAY_Play(pThis->m_nPort,pThis->m_playwindow.m_hWnd)==false)
		{
			pThis->MessageBox(_CS("Error.PlayFailed"));
			return 0;
		}
	}
	else
	{
		int nErr=H264_PLAY_GetLastError(pThis->m_nPort);
		if(nErr == H264_PLAY_CHECK_FILE_ERROR)
		{
			pThis->MessageBox(_CS("Error.CheckFileError"));
		}
		else
		{
			pThis->MessageBox(_CS("Error.OpenFileFail"));
		}
		return 0;
	}

	return 1;
}

void CLocalpalyback::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	m_bplaythread=false;
	m_PlayerStatus=STATUS_STOP;
	H264_PLAY_CloseFile(m_nPort);
	H264_PLAY_CloseStream(m_nPort);
	
	KillTimer(1);
	CloseHandle(m_hPlayThread);
	CDialog::OnClose();
}

BOOL CLocalpalyback::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	m_nPort = 90;
	m_bplaythread = false;
	m_PlayerStatus =STATUS_READY;
	m_FastSpeed = 0;
	m_SlowSpeed = 0;
	m_nFileLen = 0;

	m_ctrlSlider.SetRange(0,1000);
	m_SliderPos = 0;
	m_ctrlSlider.SetPos(0);

	m_PreTime = 0;
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
