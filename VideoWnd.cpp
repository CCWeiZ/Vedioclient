// VideoWnd.cpp : implementation file
//

#include "stdafx.h"
#include "ClientDemo2.h"
#include "VideoWnd.h"
#include "ClientDemo2Dlg.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd dialog
 int CVideoWnd::counter = 0;

CVideoWnd::CVideoWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CVideoWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVideoWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	m_nIndex = -1;
	m_iPlayhandle = -1;
	m_nPlaydecHandle = -1;
	m_bRecord = FALSE;

	for ( int i = 0; i < 100; i ++)
	{
		m_strInfoFrame[i] = "";
	}
	memset(&m_oldMouseRect,0,sizeof(RECT));
	memset(&m_downPoint,0,sizeof(CPoint));
	m_bTalkIPC = FALSE;
	

	m_vlcMedia = NULL;
	m_vlcMplay = NULL;
	m_vlcInst = NULL;

}


void CVideoWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVideoWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CVideoWnd, CDialog)
	//{{AFX_MSG_MAP(CVideoWnd)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_DISCONNECT, OnDisconnct)
	//ON_COMMAND(ID_Talk_IPC, OnTalkIPC)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVideoWnd message handlers

void CVideoWnd::drawOSD(LONG nPort,HDC hDc)
{
	if ( m_strInfoFrame[nPort] != "" )
	{
		HFONT oldfont;
		oldfont = (HFONT)::GetCurrentObject( hDc, OBJ_FONT );
		

		//�ı�������ɫ
		::SetTextColor( hDc, RGB(255,0,0) );

		//��������
		LOGFONT lf;
		::GetObject( oldfont, sizeof(lf), &lf );
		strcpy( lf.lfFaceName, "Arial" );
		lf.lfWeight = FW_BOLD;
		HFONT out_ft;		//�������������
		out_ft = ::CreateFontIndirect( &lf );

		::SelectObject( hDc, out_ft );

		TextOut( hDc, 10, 10, "testtesttest", strlen("testtesttest") );

		::SelectObject( hDc, oldfont );
	}
}

void CVideoWnd::GetColor(long *nBright, long *nContrast, long *nSaturation, long *nHue)
{
	if(m_iPlayhandle <= 0 )
	{
		return;
	}
	long nRegionNum = 0;
	H264_PLAY_GetColor(m_nIndex, nRegionNum,nBright, nContrast, nSaturation, nHue);
}

long CVideoWnd::GetLoginHandle()
{
	return m_lLogin;
}

int __stdcall RealDataCallBack_V2(long lRealHandle,const PACKET_INFO_EX *pFrame, unsigned int dwUser)//���һ��һ�������
{
	CVideoWnd *pDataChnl = (CVideoWnd*)dwUser;
	bool ret=H264_PLAY_InputData( pDataChnl->m_nPlaydecHandle , (unsigned char*)pFrame->pPacketBuffer, pFrame->dwPacketSize);

# if 0 //��׼H264��Ƶ����
 	STDH264_PACKET_INFO stdH264Info;
 	memset(&stdH264Info,0,sizeof(STDH264_PACKET_INFO)); 	

	if(pFrame->nPacketType == VIDEO_I_FRAME)
	{
		stdH264Info.pPacketBuffer=pFrame->pPacketBuffer+16;
		stdH264Info.dwPacketSize=pFrame->dwPacketSize-16;
		stdH264Info.dwFrameRate=pFrame->dwFrameRate;
		H264_PLAY_InputStdH264Data(pDataChnl->m_nPlaydecHandle,&stdH264Info);
	}
	else if(pFrame->nPacketType == VIDEO_P_FRAME)
	{
		stdH264Info.pPacketBuffer=pFrame->pPacketBuffer+8;
		stdH264Info.dwPacketSize=pFrame->dwPacketSize-8;
		stdH264Info.dwFrameRate=pFrame->dwFrameRate;
		H264_PLAY_InputStdH264Data(pDataChnl->m_nPlaydecHandle,&stdH264Info);
	}
#endif

#if 0
	if (pFrame->nPacketType == AUDIO_PACKET)
	{
		FILE *fp = fopen("stream/AudioTotal.pcm", "ab+");
		if (fp)
		{
			fwrite(pFrame->pPacketBuffer, pFrame->dwPacketSize, 1, fp);
			fclose(fp);
		}
		char filename[256];
		static int index = 0;

		sprintf(filename, "stream/Audio%02d.idx", index++);
		fp = fopen(filename, "ab+");
		if (fp)
		{
			fwrite(pFrame->pPacketBuffer, pFrame->dwPacketSize, 1, fp);
			fclose(fp);
		}
	}
	else
	{
		char filename[256];
		static int index = 0;
		static int iFrame = 0;

		FILE *fp = NULL;
		if (pFrame->nPacketType == VIDEO_I_FRAME)
		{
			sprintf(filename, "stream/stream_%02d.idx", index++);
			fp = fopen(filename, "ab+");
			if (fp)
			{
				fwrite(pFrame->pPacketBuffer, pFrame->dwPacketSize, 1, fp);
				fclose(fp);
			}
			iFrame = 1;
		}
		
		if (iFrame == 1)
		{
			fp = fopen("stream/StreamTotal.h264", "ab+");
			if (fp)
			{
				fwrite(pFrame->pPacketBuffer, pFrame->dwPacketSize, 1, fp);
				fclose(fp);
			}
		}
	}
#endif
	return 1;
}

void CVideoWnd::OnDestroy() 
{
	CDialog::OnDestroy();
	
	// TODO: Add your message handler code here
	if(m_iPlayhandle != -1)
	{		
		//H264_DVR_DelRealDataCallBack(m_iPlayhandle, RealDataCallBack, (long)this);
		H264_DVR_DelRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, (long)this);
		if(!H264_DVR_StopRealPlay(m_iPlayhandle))
		{
			TRACE("H264_DVR_StopRealPlay fail m_iPlayhandle = %d", m_iPlayhandle);
		}
	}

	m_bRecord = FALSE;
}

void CVideoWnd::OnDisconnct()
{
	//H264_DVR_DelRealDataCallBack(m_iPlayhandle, RealDataCallBack, (long)this);
	if (m_iPlayhandle > 0)
	{
		H264_DVR_DelRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, (long)this);
		H264_DVR_StopRealPlay(m_iPlayhandle);
		m_iPlayhandle = -1;
	}
	

	//close decoder
	if (m_nPlaydecHandle >= 0)
	{
		H264_PLAY_CloseStream(m_nPlaydecHandle);
		H264_PLAY_Stop(m_nPlaydecHandle);
		m_nPlaydecHandle = -1;
		m_lLogin = -1;
		Invalidate();
	}	
}

void CVideoWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
	if ( pParent  )
	{
		pParent->SetActiveWnd(m_nIndex);
	}
#ifdef DISPLAYREGION
	m_downPoint=point;
#endif
	
	CDialog::OnLButtonDown(nFlags, point);
}



void CVideoWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	#ifdef DISPLAYREGION

	long nLeft,nTop,nRight,nBottom;
	nLeft=m_downPoint.x;
	nTop=m_downPoint.y;
	nRight=point.x;
	nBottom=point.y;
	long nChannel=m_iChannel;
//*******************************
	//ѡ��Χ̫С
	if ( (abs(nBottom - nTop) < 15 || abs(nRight - nLeft) < 15) )
	{
		return;
	}
	
	
	RECT mouseRect; //��������ľ��ο�
	// ��ȡ��ǰ�Ӵ�λ��.
	//..................	
	
	RECT videoWndRect;
	GetClientRect(&videoWndRect);
	
	int width  = videoWndRect.right - videoWndRect.left;
	int height = videoWndRect.bottom - videoWndRect.top; 
	
	//ѡȡ����
	mouseRect.left = nLeft;
	mouseRect.top = nTop;
	mouseRect.right = nRight;
	mouseRect.bottom = nBottom;

		long nPicWidth = 0;
		long nPicHeight = 0;
		
		if ( mouseRect.left > mouseRect.right )
		{
			mouseRect.left = nRight;
			mouseRect.right = nLeft;
		}
		if ( mouseRect.top > mouseRect.bottom )
		{
			mouseRect.top = nBottom;
			mouseRect.bottom = nTop;
		}
		
		H264_PLAY_GetPictureSize(m_nIndex, &nPicWidth, &nPicHeight);
		
		int oldWidth = m_oldMouseRect.right -m_oldMouseRect.left;
		int oldHeight = m_oldMouseRect.bottom - m_oldMouseRect.top;
		if( oldHeight != 0 && oldWidth != 0 )
		{
			int newWidth = mouseRect.right - mouseRect.left;
			int newHeight = mouseRect.bottom - mouseRect.top;
			
			mouseRect.left = m_oldMouseRect.left + mouseRect.left * oldWidth / width;
			mouseRect.right = mouseRect.left + oldWidth * newWidth / width;
			mouseRect.top = m_oldMouseRect.top + mouseRect.top * oldHeight / height;
			mouseRect.bottom = mouseRect.top + oldHeight * newHeight / height;
		}
		else
		{
			
			mouseRect.left = mouseRect.left * nPicWidth / width;
			mouseRect.right = mouseRect.right * nPicWidth / width;
			
			mouseRect.top = mouseRect.top * nPicHeight / height;
			mouseRect.bottom = mouseRect.bottom * nPicHeight / height;
		}
		
		
		memcpy( &m_oldMouseRect, &mouseRect, sizeof(mouseRect) );
		
		TRACE("left = %d top = %d right = %d bottpm = %d \n", mouseRect.left, mouseRect.top, mouseRect.right, mouseRect.bottom);
		bool ret=H264_PLAY_SetDisplayRegion(m_nIndex,0,&mouseRect,NULL,TRUE);

	#endif
	CDialog::OnLButtonUp(nFlags, point);
}

void CVideoWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	// TODO: Add your message handler code here
	
	// Do not call CDialog::OnPaint() for painting messages
}

void CVideoWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
#ifdef DISPLAYREGION
		H264_PLAY_SetDisplayRegion(m_nIndex,0,NULL,NULL,false);
#endif
	
	if ( m_iPlayhandle > 0 )
	{
		CMenu Menu;
		Menu.CreatePopupMenu();
		Menu.AppendMenu(MF_STRING , ID_DISCONNECT, _CS("Desktop.CloseWindow"));
		
		CPoint pt;
		::GetCursorPos(&pt);
		Menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, this);
	}

	CDialog::OnRButtonDown(nFlags, point);
}

BOOL CVideoWnd::SaveRecord()
{
	//��ȡ��ǰ���ڲ��ž��
	if ( m_iPlayhandle <= 0 )
	{
		return FALSE;
	}
	
	CString  cFilename;
	
	CTime time = CTime::GetCurrentTime();
	cFilename.Format("%s\\record\\%4d%02d%02d_%02d%02d%02d.h264", 
		"c:", 
		time.GetYear(), 
		time.GetMonth(), 
		time.GetDay(), 
		time.GetHour(), 
		time.GetMinute(), 
		time.GetSecond());
	

	if ( m_bRecord )
	{
		if ( H264_PLAY_StopDataRecord(m_nPlaydecHandle) )
		{
			m_bRecord = FALSE;
			MessageBox("stop record OK.");
		}
	}
	else
	{
		int nTemp = 0;
		for(;;)
		{
			int nIndex = cFilename.Find('\\',nTemp);
			if (nIndex == -1) 
			{
				break;
			}
			CString str = cFilename.Left(nIndex);
			nTemp = nIndex+1;
			_mkdir(str);
		}

		
		if ( H264_PLAY_StartDataRecord(m_nPlaydecHandle, cFilename.GetBuffer(0), MEDIA_FILE_NONE))
		{
			m_bRecord = TRUE;
			MessageBox("start record OK.");
		}
		else
		{
			MessageBox("start record fail.");
		}
	}

	return TRUE;
}

void CVideoWnd::SetColor(long nBright, long nContrast, long nSaturation, long nHue)
{
	H264_PLAY_SetColor(m_nIndex, 0, nBright, nContrast, nSaturation, nHue );
}

long CVideoWnd::SetDevChnColor(SDK_CONFIG_VIDEOCOLOR* pVideoColor)
{
	 long ret=H264_DVR_SetDevConfig(m_lLogin,E_SDK_VIDEOCOLOR,m_iChannel,(char*)pVideoColor,sizeof(SDK_CONFIG_VIDEOCOLOR),3000);
	 return ret;
}

void __stdcall CVideoWnd::videoInfoFramCallback(LONG nPort, LONG nType, LPCSTR pBuf,LONG nSize, LONG nUser)
{
 	CVideoWnd *pThis = (CVideoWnd*)nUser;
 	//�յ���Ϣ֡, 0x03 ����GPRS��Ϣ
 	if ( nType == 0x03 )
 	{
 		pThis->m_strInfoFrame[nPort] = pBuf;
 	}
}

void __stdcall pProc(LONG nPort,LPCSTR pBuf,LONG nSize,LONG nWidth,LONG nHeight, LONG nStamp,LONG nType,LONG nUser)
{

}

void CALLBACK drawOSDCall(LONG nPort,HDC hDc,LONG nUser)
{
	CVideoWnd *pThis = (CVideoWnd*)nUser;
	pThis->drawOSD(nPort,hDc);
}


int CVideoWnd::ConnectRealPlay(DEV_INFO *pDev, int nChannel, bool bOsd)
{
	if(m_iPlayhandle != -1)
	{		
		//H264_DVR_DelRealDataCallBack(m_iPlayhandle, RealDataCallBack, (long)this);
		//?����ص�����,�ú�����Ҫ��H264_DVR_StopRealPlayǰ����
		H264_DVR_DelRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, (long)this);
		if(!H264_DVR_StopRealPlay(m_iPlayhandle))
		{
			TRACE("H264_DVR_StopRealPlay fail m_iPlayhandle = %d", m_iPlayhandle);
		}
	}

	if ( m_nPlaydecHandle == -1 )
	{
		//open decoder
		BYTE byFileHeadBuf;
		if (H264_PLAY_OpenStream(m_nIndex, &byFileHeadBuf, 1, SOURCE_BUF_MIN*50))
		{	
			//������Ϣ֡�ص�
			H264_PLAY_SetInfoFrameCallBack(m_nIndex, videoInfoFramCallback,(DWORD)this);

			//����osd��Ϣ
			if ( bOsd )
			{
				OSD_INFO_TXT osd;
				osd.bkColor = RGB(0,255,0);
				osd.color = RGB(255,0,0);
				osd.pos_x = 10;
				osd.pos_y = 10;
				osd.isTransparent = 0;
				strcpy(osd.text, "test osd info"); 
				H264_PLAY_SetOsdTex(m_nIndex, &osd);


				OSD_INFO_TXT osd2;
				osd2.bkColor = RGB(255,0,0);
				osd2.color = RGB(0,255,0);
				osd2.pos_x = 10;
				osd2.pos_y = 40;
				osd2.isTransparent = 0;
				osd2.isBold = 1;
				strcpy(osd2.text, "test222 osd info"); 
				H264_PLAY_SetOsdTex(m_nIndex, &osd2);


				//����osd���ӻص�
				H264_PLAY_RigisterDrawFun(m_nIndex, drawOSDCall, (DWORD)this);
			}
			H264_PLAY_SetStreamOpenMode(m_nIndex, STREAME_REALTIME);	
			if ( H264_PLAY_Play(m_nIndex, this->m_hWnd) )
			{
					H264_PLAY_SetDisplayCallBack(m_nIndex, pProc, (LONG)this);
				m_nPlaydecHandle = m_nIndex;
			}
		}	
	}
	H264_DVR_CLIENTINFO playstru;

	playstru.nChannel = nChannel;
	playstru.nStream = 0;
	playstru.nMode = 3;
	m_iPlayhandle = H264_DVR_RealPlay(pDev->lLoginID, &playstru);	
	if(m_iPlayhandle <= 0 )
	{
		DWORD dwErr = H264_DVR_GetLastError();
		CString sTemp("");
		sTemp.Format("access %s channel%d fail, dwErr = %d",pDev->szDevName,nChannel, dwErr);
		MessageBox(sTemp);
	}
	else
	{
		//set callback to decode receiving data
		//H264_DVR_SetRealDataCallBack(m_iPlayhandle, RealDataCallBack, (long)this);
		H264_DVR_MakeKeyFrame(pDev->lLoginID, nChannel, 0);
		H264_DVR_SetRealDataCallBack_V2(m_iPlayhandle, RealDataCallBack_V2, (long)this);
	}
	m_lLogin = pDev->lLoginID;
	m_iChannel = nChannel;
	return m_iPlayhandle;
}


void CVideoWnd::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	CClientDemo2Dlg *pParent = (CClientDemo2Dlg *)GetParent();
	CVideoWnd::counter++;
	if(counter % 2 != 0)
	{
		pParent->ArrayWindow1(pParent->m_nCurIndex);
	}
	else
	{
		pParent->ArrayWindow(4);
	}
	CDialog::OnLButtonDblClk(nFlags, point);
}

BOOL CVideoWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	char *vlc_argv[100];
	int vlc_argc = 0;
	
	m_vlcInst = libvlc_new (vlc_argc, vlc_argv);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CVideoWnd::URLplay(CString VideoPath)
{
	if (m_vlcMplay != NULL)
	{
		libvlc_media_player_release(m_vlcMplay);
	}
	m_vlcMedia = libvlc_media_new_location(m_vlcInst,VideoPath.GetBuffer(0));
	m_vlcMplay = libvlc_media_player_new_from_media(m_vlcMedia);
	HWND hwnd = this->GetSafeHwnd();
	libvlc_media_player_set_hwnd (m_vlcMplay, hwnd);
	libvlc_media_player_play(m_vlcMplay);
}

void CVideoWnd::URLstop()
{
	if (m_vlcMplay == NULL)
	 {
		return;
	 }
	 libvlc_media_player_stop(m_vlcMplay);
	 m_vlcMplay = NULL;
	 if (m_vlcMedia != NULL)
	 {
		libvlc_media_release(m_vlcMedia);
	 }
	 if (m_vlcMplay != NULL)
	 {
		libvlc_media_player_release(m_vlcMplay);
	 }
	 OnPaint();
}
