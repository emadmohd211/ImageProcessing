/*
 *  CMBitmapProcessing.h
 *  ImageProcessingC
 *
 *
 *
 */



/* 
 ----------
 HOW TO USE
 ----------
 
 Overview
 ========
 
 This is a class used to open and manipulate image files.
 
 
 Compilation/Linking
 ===================
 
 Other components:
 - velib
 
 Libraries:
 - strmiids.lib 
 - gdiplus.lib
 
 DLLs:
 - gdiplus.dll
 
 Run-time library:
 - Multithreaded DLL / Debug Multithreaded DLL
 
 Microsoft Foundation Classes:
 - Not using MFC
 
 Sample code
 ===========
 
 Before calling any of the functions, especially those associated
 with Gdiplus, call:
 
 CMBitmapProcessing::Init()
 
 When finished using call:
 
 CMBitmapProcessing::Uninit()
 
 NOTE: 
 Function names that are appended with Gdiplus means that it uses the Gdiplus 
 libraries.
 
 */

/*********************************************************************
 REVISION LOG ENTRY
 Revision By: Chew Chee Ming
 Revised on 03/04/2003 3:47:53 PM
 Comments:
 
 PROBLEM:
 
 Problem loading GIFs that are corrupted.
 *********************************************************************/

/*********************************************************************
 REVISION LOG ENTRY
 Revision By: Chew Chee Ming
 Revised on 31/03/2003 5:08:16 PM
 Comments:
 
 Change CalcWidthBytes to work with widthbytes that are smaller than
 1 byte. Because of the 32bit alignment, the widthbytes of these images
 are 4 bytes long
 *********************************************************************/


#if !defined(AFX_CMBITMAPPROCESSING_H__3AC1BC15_2C51_4430_88C2_990F7DE7FDA4__INCLUDED_)
#define AFX_CMBITMAPPROCESSING_H__3AC1BC15_2C51_4430_88C2_990F7DE7FDA4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/* --------
 INCLUDES
 -------- */

#include "opencv2/core/core.hpp"
#include "opencv2/core/cvdef.h"
//#include "opencv2/highgui/highgui_c.h"
#include "VideoExtract.h"
#include "typedefin.h"



/* -----------
 DECLARATION
 ----------- */

class CMBitmapProcessing  
{
public:
	
	// iProcessType for ProcessBitmap() 
	enum PROCESS_TYPE
	{
		PT_GRAYSCALE,
		PT_GRAYSCALE_NORMAL,
		PT_RED,
		PT_GREEN,
		PT_BLUE,
		PT_EDGES,
		PT_MOVEMENT,
	};
	
	enum INTERPOLATION_TYPE
	{
		IT_NEAREST,
		IT_BILINEAR,
		IT_BICUBIC,
	};
	
	enum
	{
		CM_FILENAME_LENGTH = MAX_PATH,
	};
	
	// initialisation & uninitialisation
	static bool Init();
	static bool Uninit();
	static bool initvalues(int w , int h  , int wb , int d  , int ch , int bpp, int ty);
	static bool Convert32to24bit(uchar* *phbmDest, uchar* hbmSrc);
    static bool Convert32to24bitCV(cv::Mat phbmDest,cv::Mat hbmSrc);
	static INT CalcWidthBytes(int iWidth, int iBitsPixel);
	static bool CloneBitmap(uchar* *phbmDest, uchar* hbmSrc);
	static bool CloneBitmapFull(uchar* *phbmDest, uchar* hbmSrc, int width,int height,int depth,int channels ,int &nWidth , int&nHeight ,int &nDepth ,int&nChannel);

	static bool ConvertTo32bit(uchar* *phbmDest, uchar* hbmSrc);
    static bool ConvertTo32bitCV(cv::Mat phbmDest, cv::Mat hbmSrc);
	static bool ResizeBitmap(uchar* *phbmDest, uchar* & hbmSrc, INT iTopX, INT iTopY, INT iWidth, INT iHeight, const INT iWidthNew, const INT iHeightNew , INT iBpp);
	static bool ResizeBitmapSS(uchar* *phbmDest, uchar* & hbmSrc, INT widthSrc, INT heightSrc, INT iTopX, INT iTopY, INT iWidth, INT iHeight, const INT iWidthNew, const INT iHeightNew , INT iBpp);
	static bool AddWhiteBorder(uchar* *phbmDest, uchar* &hbmSrc , int Width , int Height , int Depth , int Channels );


	// for videos
	
	static bool OpenVideo(VOID ** ppVideo, _TCHAR* lpszFilename);
	static bool GetVideoFrameCount(VOID * pVideo, long * plFrameCount);
	static bool CloseVideo(VOID ** ppVideo);

	static bool CloseVideoJpeg(VOID ** ppVideo);
	static bool CloseVideoJpeg1(VOID ** ppVideo);
	static bool CloseVideoJpegNoFrame(VOID ** ppVideo);
	
	
	static bool GetBufferWSize(VOID * pVideo, INT * plFrameCount);
	static bool GetBufferHSize(VOID * pVideo, INT * plFrameCount);
	static bool GetTotalFrame(VOID * pVideo,  INT * plFrameCount);
	static bool GetDuration(VOID * pVideo,  FLOAT * plFrameCount);
	static bool IsNotLowQualityVid(VOID * pVideo , bool *plFrameCount);
	static bool GetNumofBytes(VOID * pVideo,  INT * plFrameCount);
	static bool AllocateBuffer(VOID * pVideo , _TCHAR* lpszFilename);
    static bool GetVideoIplImage(VOID * pVideo, cv::Mat &phbm, long lFrame);
	static bool GetInitStartTime(VOID * pVideo, FLOAT Duration ,long lFrame);
	 
	
	
	// for images
	static bool       ms_bInit;
	static int       iHeight;
	static int       iWidth;
	static int       iWidthbytes;
	static int       iDepth;
	static int       iChannels;
	static int		 iBpp;
    //static int		 iType;
	
};

#endif // !defined(AFX_CMBITMAPPROCESSING_H__3AC1BC15_2C51_4430_88C2_990F7DE7FDA4__INCLUDED_)
