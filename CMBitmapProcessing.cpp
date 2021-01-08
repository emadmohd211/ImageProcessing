// CMBitmapProcessing.cpp: implementation of the CMBitmapProcessing class.
//
//////////////////////////////////////////////////////////////////////

/* --------
 INCLUDES
 -------- */


#include "CMBitmapProcessing.h"
#include "typedefin.h"
#include "opencv2/imgproc/imgproc_c.h"
#include "mat.hpp"
#include <opencv2/ml.hpp>


bool       CMBitmapProcessing::ms_bInit         = FALSE;
int        CMBitmapProcessing::iWidth			= -1;
int        CMBitmapProcessing::iHeight			= -1;
int        CMBitmapProcessing::iWidthbytes		= -1;
int        CMBitmapProcessing::iDepth			= -1;
int        CMBitmapProcessing::iChannels		= -1;
int		   CMBitmapProcessing::iBpp				= -1;


static int iType;


/**
 * <Short Desc/>    : Uninitialize the library.
 * <pre>
 * Function name    : CMBitmapProcessing::Uninit
 * Description      : Uninitialize the library. Executed after all calls to CMBitmapProcessing functions.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * </pre>
 **/
bool CMBitmapProcessing::Uninit()
{
	
	//MessageBox(NULL,_T("in CMBitmapProcessing::Uninit()"),_T(""),NULL);
	
	if (ms_bInit == TRUE)
	{
		
		
		ms_bInit = FALSE;
		return TRUE;
	}
	
	//MessageBox(NULL,_T("CMBitmapProcessing::Uninit() done"),_T(""),NULL);
	
	return FALSE;
}


/**
 * <Short Desc/>    : Initialize the library.
 * <pre>
 * Function name    : CMBitmapProcessing::Init
 * Description      : Initialize the library. Executed before any calls to CMBitmapProcessing functions.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * </pre>
 **/
bool CMBitmapProcessing::Init()
{
    return 0;
}

bool CMBitmapProcessing::initvalues(int width , int height , int widthbytes , int depth  , int channels , int bpp,int type)
{
	
	iWidth = width;
	iHeight = height;
	iWidthbytes = widthbytes;
	iDepth = depth;
	iChannels = channels;
	iBpp = bpp;
    iType = type;
	if (iWidth == -1 || iHeight == -1 || iWidthbytes == -1 || iDepth == -1  || iChannels == -1 || iBpp ==-1)
		return false;
	
	else return true;
}


/**
 * <Short Desc/>    : Convert a 32bit hbitmap to 24bit hbitmap.
 * <pre>
 * Function name    : CMBitmapProcessing::Convert32to24bit
 * Description      : Convert a 32bit hbitmap to 24bit hbitmap.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * Argument         : HBITMAP * phbmDest  - Pointer to the hbitmap to store the converted hbitmap
 * Argument         : HBITMAP & hbmSrc    - The source hbitmap
 * </pre>
 **/
bool CMBitmapProcessing::Convert32to24bit(uchar* *phbmDest, uchar* hbmSrc)
{
	if (phbmDest == NULL)
	{
		return FALSE;
	}
	
	if (hbmSrc == NULL)
	{
		return FALSE;
	}
	
	//if (iBpp != 32)
	//{
	//	return FALSE;
	//}
	
	// extract info from source bitmap
	uchar *image = hbmSrc;
	
	INT iWidthBytes = CalcWidthBytes(iWidth, 32);
	
	INT iBitsPixelDest = 24;
	INT iWidthBytesDest = CalcWidthBytes(iWidth, iBitsPixelDest);
	
	*phbmDest = new uchar (iBitsPixelDest*iWidth * iHeight);
	
	uchar *dest = *phbmDest;

	// convert 32-bit to 24-bit
	INT h;
	INT w;
	
	INT index;
	INT indexDest;
	for (h=0; h<iHeight; h++)
	{
		for (w=0; w<iWidth; w++)
		{
			index = w+w+w+w;
			indexDest = w+w+w;
			
			dest[indexDest] = image[index];		// blue
			dest[indexDest+1] = image[index+1];	// green
			dest[indexDest+2] = image[index+2];	// red
		}
		
		// go to the next scanline
		image += iWidthBytes;
		dest += iWidthBytesDest;
	}
	
	return TRUE;
}
	


bool CMBitmapProcessing::Convert32to24bitCV(cv::Mat phbmDest,cv::Mat hbmSrc)//IplImage *phbmDest, IplImage* hbmSrc) //updating opencv4
{
	if (phbmDest.empty())// == NULL)
	{
		return FALSE;
	}
	
	if (hbmSrc.empty())// == NULL)
	{
		return FALSE;
	}

	 uchar* image= new uchar (hbmSrc.cols * hbmSrc.rows); //(hbmSrc->imageSize);
	INT wsrc = hbmSrc.cols; //hbmSrc-> width;
	INT hsrc = hbmSrc.rows; //hbmSrc -> height;
	
	// create 24 bits CV image
	//phbmDest = cvCreateImage(cvSize(hbmSrc-> width, hbmSrc -> height),IPL_DEPTH_8U,3);
    phbmDest = cv::Mat(wsrc, hsrc,CV_8UC3); //update opencv4
    
    
    uchar *dest = new uchar (phbmDest.cols * phbmDest.rows);//phbmDest->imageSize);
	
	// convert 32-bit to 24-bit
	INT h;
	INT w;
	
	INT index;
	INT indexDest;
	
	for (h=0; h<hsrc; h++)
	{
		for (w=0; w<wsrc; w++)
		{
			index = w+w+w+w;
			indexDest = w+w+w;
			
			dest[indexDest] = image[index];		// blue
			dest[indexDest+1] = image[index+1];	// green
			dest[indexDest+2] = image[index+2];	// red
		}
		
		// go to the next scanline
		image += hbmSrc.step[0];//hbmSrc->widthStep;
		dest += hbmSrc.step[0];//hbmSrc->widthStep ;
	}
	
	// copy data into 24 bits CV image
//	memcpy(phbmDest->imageData ,dest, phbmDest->imageSize); //emadcheck
    memcpy(phbmDest.data ,dest, phbmDest.elemSize());//(phbmDest.rows*phbmDest.cols));
    
    
	
	return TRUE;
}



INT CMBitmapProcessing::CalcWidthBytes(int iWidth, int iBitsPixel)
{
	// calculate width bytes - the width in windows bitmaps are aligned in 32-bit words
	int iWidthBits = iWidth * iBitsPixel;
	iWidthBits = (iWidthBits % 32) ? (((iWidthBits >> 5) + 1) << 5) : iWidthBits;
	
	int iWidthBytes = iWidthBits >> 3;
	
	return iWidthBytes;
}



/**
 * <Short Desc/>    : Duplicate a hbitmap.
 * <pre>
 * Function name    : CMBitmapProcessing::CloneBitmap
 * Description      : Duplicate a hbitmap. Works with 1bit, 4bit, 8bit, 16bit, 24bit, 32bit.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * Argument         : HBITMAP * phbmDest  - Pointer to the hbitmap that will store the duplicated hbitmap
 * Argument         : HBITMAP & hbmSrc    - Pointer to the source hbitmap
 * </pre>
 **/
bool CMBitmapProcessing::CloneBitmap(uchar* *phbmDest, uchar* hbmSrc) 
{
	
	// do memory copy to duplicate bitmap
	
	uchar* pbSrc = hbmSrc;
	INT iWidthBytes = CalcWidthBytes(iWidth, iBpp);
	
	uchar* pbDest = new uchar[iWidthBytes * iHeight];
	
	int i=0;
	
	if (i==1){
	 IplImage *test;
	 //Create IplImage.
	 	 CvSize size;
	 	 size.width  = iWidth;
	 	 size.height = iHeight;
	 	 test = cvCreateImage(size, IPL_DEPTH_8U, 3);
	 	 test->imageData = (char*)hbmSrc ;
#ifdef _JHDebug_showImage				
	 char PreWinName[16];
	 sprintf (PreWinName , "Image %d - Blob" , 999);
	 cvShowImage(PreWinName , test);	
	cvSaveImage("/homam/testcccc.jpg", test, 0);
#endif		
		cvReleaseImage(&test);
	}
	i++;
	
	memcpy(pbDest, pbSrc, iWidthBytes * iHeight);
	
	*phbmDest = pbDest;
	
	return TRUE;
	
	// get bitmap info of source bitmap
	
}


bool CMBitmapProcessing::CloneBitmapFull(uchar* *phbmDest, uchar* hbmSrc, int width,int height,int depth,int channels ,int &nWidth , int&nHeight ,int &nDepth ,int&nChannel)
{
	// get bitmap info of source bitmap
	//BITMAP bmSrc;
	uchar* pbSrc = hbmSrc;
	
	
	nWidth = width;
	nHeight = height;
	nDepth = depth;
	nChannel = channels;

	INT iWidthBytes = CalcWidthBytes(nWidth, ((nDepth & 255) * nChannel));
	uchar* pbDest = new uchar[iWidthBytes * nHeight];
	//	uchar* pbDest = (uchar *)alloca(iWidthBytes * iHeight* sizeof(uchar));
	memcpy(pbDest, pbSrc, iWidthBytes * nHeight);
	//	memmove(pbDest, pbSrc, iWidthBytes * iHeight); // added by JH
	
	*phbmDest = pbDest;
	
	return TRUE;
	

	
	return TRUE;
}

bool CMBitmapProcessing::AddWhiteBorder(uchar* *phbmDest, uchar* &hbmSrc , int Width , int Height , int Depth , int Channels)
{
	if (phbmDest == NULL)
	{
		return FALSE;
	}
	
	if (hbmSrc == NULL)
	{
		return FALSE;
	}
			
	// ensure that source bitmap is 32bit
	INT iBitsPixel = Channels * Depth;
	
	// extract info from source bitmap
	INT iWidth = Width;
	INT iWidthBytes = CalcWidthBytes(iWidth, iBitsPixel);
	INT iHeight = Height;
	
	INT iWhiteBorderVal =(iWidth/10);
	INT iWhiteStep = (iWhiteBorderVal/2);
	INT inHeight = Height + iWhiteBorderVal;
	INT inWidth = Width + iWhiteBorderVal;
	INT iBitsPixelDest = 24;
	INT iWidthBytesDest = CalcWidthBytes(inWidth, iBitsPixelDest);
	
	
	BYTE *image = (BYTE*) (hbmSrc);

	
	
	*phbmDest  = new uchar[iWidthBytesDest *inHeight];
	

	
		
	BYTE *dest = (BYTE *) (*phbmDest);
	
	
	
	
	// convert 32-bit to 24-bit
	INT h;
	INT w;
	
	INT index;
	INT indexDest;
	for (h=0; h<inHeight; h++)
	{
		for (w=0; w<inWidth; w++)
		{
			
			indexDest = w+w+w;
			index = (indexDest-iWhiteStep*3);
			
			if (w < iWhiteStep || w > iWidth +iWhiteStep || h < iWhiteStep || h >  iHeight +iWhiteStep)
			{
				//sprintf(emad , "h = %d , w= %d ,  index = %d, indexDest = %d " , h , w, index , indexDest );
				//MessageBox (NULL , emad , "Insid White" , NULL);
				
				dest[indexDest]	  = 255;		// blue
				dest[indexDest+1] = 255;	   // green
				dest[indexDest+2] = 255;	   // red	
				
			}
			else 
			{
				
				//sprintf(emad , "h = %d , w= %d ,  index = %d, indexDest = %d " , h , w, index , indexDest );
				//MessageBox (NULL , emad , "outside Black" , NULL);
				
				dest[indexDest] =image[index];		// blue
				dest[indexDest+1] =image[index+1];	// green
				dest[indexDest+2] =	image[index+2];	// blue
			}
		}
		
		// go to the next scanline
		if (h > iWhiteStep && h < iHeight +iWhiteStep-1) image += iWidthBytes;
		dest += iWidthBytesDest;
	}
	


	
	return TRUE;
}


bool CMBitmapProcessing::ConvertTo32bit(uchar* *phbmDest, uchar* hbmSrc)
{
	if (phbmDest == NULL)
	{
		return FALSE;
	}
	
	if (hbmSrc == NULL)
	{
		return FALSE;
	}
	
	IplImage *ImageIn = cvCreateImage(cvSize(iWidth, iHeight),IPL_DEPTH_8U,1); 
	IplImage *ImageOut = cvCreateImage(cvSize(iWidth, iHeight),IPL_DEPTH_32F,1); 
	
	memcpy(ImageIn->imageData ,hbmSrc, ImageIn->imageSize);
	
	cvConvertScale(ImageIn,ImageOut,1.0,0); 

	
	
	INT iWidthBytes = CalcWidthBytes(ImageOut->width , ImageOut->nChannels * 8);
	
	uchar* pbDest = new uchar[iWidthBytes * iHeight];
	
	memcpy(pbDest, ImageOut->imageData, iWidthBytes * iHeight);
	
	*phbmDest = pbDest;
	

	return TRUE;
}

bool CMBitmapProcessing::ConvertTo32bitCV( cv::Mat phbmDest, cv::Mat hbmSrc)// IplImage *phbmDest, IplImage* hbmSrc)	//JH
{
	if (phbmDest.empty())// == NULL)
	{
		return FALSE;
	}
	
	if (hbmSrc.empty())// == NULL)
	{
		return FALSE;
	}
	
    phbmDest = cv::Mat(iWidth, iHeight,CV_32FC1);

    
	//cvConvertScale(hbmSrc,phbmDest,1.0,0);
    hbmSrc.convertTo(phbmDest,CV_32F,1.0,0);	
	return TRUE;
}


bool CMBitmapProcessing::ResizeBitmap(uchar* *phbmDest, uchar* & hbmSrc,  INT iTopX, INT iTopY, INT iWidth, INT iHeight, const INT iWidthNew, const INT iHeightNew , INT bpp)
{
	
	if (phbmDest == NULL)
	{
		return false;
	}
	
	if (hbmSrc == NULL)
	{
		return false;
	}
	
		
	// only works for 24 bit
	if (bpp != 24)
	{
		return false;
	}
	
	if ((iTopX < 0) || (iTopX >= iWidth))
//	if ((iTopX < 0) || (iTopX >= widthSrc))		
	{
		return false;
	}
	
	if ((iTopY < 0) || (iTopY >= iHeight))
//	if ((iTopY < 0) || (iTopY >= heightSrc))	
	{
		return false;
	}
	
	if (iWidth <= 0 || iHeight <= 0)
	{
		return FALSE;
	}
	
	int iBottomX = iTopX + iWidth;
	
	if (iBottomX > iWidth)
	{
		iWidth -= iBottomX - iWidth;
	//	widthSrc -= iBottomX - widthSrc;
	}
	
	int iBottomY = iTopY + iHeight;
	if (iBottomY > iHeight)
	{
		iHeight -= iBottomY - iHeight;
	//	iHeight -= iBottomY - heightSrc;
	}
	
	if (iWidthNew <= 0 || iHeightNew <= 0)
	{
		return false;
	}
	
	*phbmDest = new uchar[iWidthNew*iHeightNew*bpp];
	
	if (phbmDest == NULL)
	{
		return FALSE;
	}
	
	// calculate the scale
	FLOAT fScaleX = static_cast<FLOAT>(iWidth) / iWidthNew;
	FLOAT fScaleY = static_cast<FLOAT>(iHeight) / iHeightNew;
	
	//	if (iScaleX < 1)
	//	{
	//		iScaleX = 1.0f;
	//	}
	
	//	if (iScaleY < 1)
	//	{
	//		iScaleY = 1.0f;
	//	}
	
	INT iXMax = static_cast<INT>(fScaleX);
	INT iYMax = static_cast<INT>(fScaleY);
	
	if (iXMax < 0)
	{
		iXMax = 0;
	}
	
	if (iYMax < 0)
	{
		iYMax = 0;
	}
	
	if (fScaleX < 1.0)
	{
		iXMax = 1;
	}
	
	if (fScaleY < 1.0)
	{
		iYMax = 1;
	}
	
	INT iDest;
	INT iSrc;
	INT x;
	INT y;
	INT x2;
	INT y2;
	
	INT iWidthBytesSrc = CalcWidthBytes(iWidth, bpp);
	INT iWidthBytesDest = CalcWidthBytes(iWidthNew, bpp);
	
	
	BYTE *pImgSrc = hbmSrc;
	BYTE *pImgDest = *phbmDest;
	
	// translate pointer to point at the last scanline
	// which is the first scanline when bitmap is displayed
	pImgDest += (iHeightNew - 1) * iWidthBytesDest;
	
	// keep colours
	INT iRed;
	INT iGreen;
	INT iBlue;
	
	// index for source pixels
	INT iSrcX;
	INT iSrcY;
	
	// to keep scale to access pixels
	INT iSclX;
	INT iSclY;
	
	// outer 2 for loops to access pixels for destination bitmap (image to be resized)
	for (y=0; y<iHeightNew; y++)
	{
		for (x=0; x<iWidthNew; x++)
		{
			iRed = 0;
			iGreen = 0;
			iBlue = 0;
			
			iSclX = static_cast<INT>(fScaleX * x);
			iSclY = static_cast<INT>(fScaleY * y);
			
			// inner 2 for loops to access pixels and access data for source bitmap
			
			// gather data for resizing
			for (y2=0; y2<iYMax; y2++)
			{
				for (x2=0; x2<iXMax; x2++)
				{
					iSrcX = iTopX + x2 + iSclX;
					iSrcX += iSrcX + iSrcX;
					
					iSrcY = (iHeight - (iTopY + y2 + iSclY) - 1) * iWidthBytesSrc;
					
					iSrc = iSrcX + iSrcY;
					
					iRed += pImgSrc[iSrc+2];
					iGreen += pImgSrc[iSrc+1];
					iBlue += pImgSrc[iSrc];
				}
			}
			
			iDest = x+x+x;
			
			iRed /= (iYMax * iXMax);
			iGreen /= (iYMax * iXMax);
			iBlue /= (iYMax * iXMax);
			
			pImgDest[iDest+2] = iRed; // red;
			pImgDest[iDest+1] = iGreen; // green;
			pImgDest[iDest] = iBlue; // blue;
		}
		pImgDest -= iWidthBytesDest;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::ResizeBitmapSS(uchar* *phbmDest, uchar* & hbmSrc, INT widthSrc, INT heightSrc, INT iTopX, INT iTopY, INT iWidth, INT iHeight, const INT iWidthNew, const INT iHeightNew , INT bpp)
{
	
	if (phbmDest == NULL)
	{
		return false;
	}
	
	if (hbmSrc == NULL)
	{
		return false;
	}
	
	
	// only works for 24 bit
	if (bpp != 24)
	{
		return false;
	}
	
	//	if ((iTopX < 0) || (iTopX >= iWidth))
	if ((iTopX < 0) || (iTopX >= widthSrc))		
	{
		return false;
	}
	
	//	if ((iTopY < 0) || (iTopY >= iHeight))
	if ((iTopY < 0) || (iTopY >= heightSrc))	
	{
		return false;
	}
	
	if (iWidth <= 0 || iHeight <= 0)
	{
		return false;
	}
	
	int iBottomX = iTopX + iWidth;
	
//	if (iBottomX > iWidth)
	if (iBottomX > widthSrc)		
	{
		//	iWidth -= iBottomX - iWidth;
		widthSrc -= iBottomX - widthSrc;
	}
	
	int iBottomY = iTopY + iHeight;
//	if (iBottomY > iHeight)
	if (iBottomY > heightSrc)		
	{
		//	iHeight -= iBottomY - iHeight;
		iHeight -= iBottomY - heightSrc;
	}
	
	if (iWidthNew <= 0 || iHeightNew <= 0)
	{
		return false;
	}
	
	*phbmDest = new uchar[iWidthNew*iHeightNew*bpp];
	
	if (phbmDest == NULL)
	{
		return false;
	}
	
	// calculate the scale
	FLOAT fScaleX = static_cast<FLOAT>(iWidth) / iWidthNew;
	FLOAT fScaleY = static_cast<FLOAT>(iHeight) / iHeightNew;
	
	//	if (iScaleX < 1)
	//	{
	//		iScaleX = 1.0f;
	//	}
	
	//	if (iScaleY < 1)
	//	{
	//		iScaleY = 1.0f;
	//	}
	
	INT iXMax = static_cast<INT>(fScaleX);
	INT iYMax = static_cast<INT>(fScaleY);
	
	if (iXMax < 0)
	{
		iXMax = 0;
	}
	
	if (iYMax < 0)
	{
		iYMax = 0;
	}
	
	if (fScaleX < 1.0)
	{
		iXMax = 1;
	}
	
	if (fScaleY < 1.0)
	{
		iYMax = 1;
	}
	
	INT iDest;
	INT iSrc;
	INT x;
	INT y;
	INT x2;
	INT y2;
	
//	INT iWidthBytesSrc = CalcWidthBytes(iWidth, bpp);
	INT iWidthBytesDest = CalcWidthBytes(iWidthNew, bpp);
	INT iWidthBytesSrc = CalcWidthBytes(widthSrc, bpp);
	
	
	BYTE *pImgSrc = hbmSrc;
	BYTE *pImgDest = *phbmDest;
	
	// translate pointer to point at the last scanline
	// which is the first scanline when bitmap is displayed
	pImgDest += (iHeightNew - 1) * iWidthBytesDest;
	
	// keep colours
	INT iRed;
	INT iGreen;
	INT iBlue;
	
	// index for source pixels
	INT iSrcX;
	INT iSrcY;
	
	// to keep scale to access pixels
	INT iSclX;
	INT iSclY;
	
	// outer 2 for loops to access pixels for destination bitmap (image to be resized)
	for (y=0; y<iHeightNew; y++)
	{
		for (x=0; x<iWidthNew; x++)
		{
			iRed = 0;
			iGreen = 0;
			iBlue = 0;
			
			iSclX = static_cast<INT>(fScaleX * x);
			iSclY = static_cast<INT>(fScaleY * y);
			
			// inner 2 for loops to access pixels and access data for source bitmap
			
			// gather data for resizing
			for (y2=0; y2<iYMax; y2++)
			{
				for (x2=0; x2<iXMax; x2++)
				{
					iSrcX = iTopX + x2 + iSclX;
					iSrcX += iSrcX + iSrcX;
					
				//	iSrcY = (iHeight - (iTopY + y2 + iSclY) - 1) * iWidthBytesSrc;
					iSrcY = (heightSrc - (iTopY + y2 + iSclY) - 1) * iWidthBytesSrc;
					
					iSrc = iSrcX + iSrcY;
					
					iRed += pImgSrc[iSrc+2];
					iGreen += pImgSrc[iSrc+1];
					iBlue += pImgSrc[iSrc];
				}
			}
			
			iDest = x+x+x;
			
			iRed /= (iYMax * iXMax);
			iGreen /= (iYMax * iXMax);
			iBlue /= (iYMax * iXMax);
			
			pImgDest[iDest+2] = iRed; // red;
			pImgDest[iDest+1] = iGreen; // green;
			pImgDest[iDest] = iBlue; // blue;
		}
		pImgDest -= iWidthBytesDest;
	}
	
	return true;
}



/**
 * <Short Desc/>    : Open a video file.
 * <pre>
 * Function name    : CMBitmapProcessing::OpenVideo
 * Description      : Open a video file.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * Argument         : VOID ** ppVideo        -  Pointer to a pointer to a video object
 * Argument         : LPCTSTR lpszFilename   -  Pointer to the filename string
 * </pre>
 **/
bool CMBitmapProcessing::OpenVideo(VOID ** ppVideo, _TCHAR* lpszFilename)
{
	 
#ifdef _DEBUG 
	 assert(ppVideo != NULL);
	 assert(lpszFilename != NULL);
#endif
	
	if (ppVideo == NULL) return false;
	if (lpszFilename == NULL) return false;
	
	CVideoExtract * & pVE = reinterpret_cast<CVideoExtract * &>(*ppVideo);
	pVE = NULL;
	try
	{
		pVE = new CVideoExtract();
	
		if (pVE == NULL)
		{
			return false;
		}
		
		if (pVE->Open(lpszFilename) == false)
		{
			//MessageBox(NULL,_T("OPen 2 ok"),_T("***********"),NULL);
			if (pVE != NULL)
			{
			//	delete pVE;
			//	free(pVE);	//JH - solve malloc error:  free at else routine
				pVE = NULL;
			}
			return false;
		}
		else
		{
			return true;
		}
	}
	catch (...)
	{
		if (pVE != NULL)
		{
		//	delete pVE;
		//	free(pVE);	//JH - solve malloc error:  free at else routine			
			pVE = NULL;
		}
		
		return false;
	}
}

/**
 * <Short Desc/>    : Get the frame count of the video.
 * <pre>
 * Function name    : CMBitmapProcessing::GetVideoFrameCount
 * Description      : Get the frame count of the video.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * Argument         : VOID * pVideo       -  Pointer to the video object
 * Argument         : LONG * plFrameCount -  Pointer to the variable to hold the frame count
 * </pre>
 **/
bool CMBitmapProcessing::GetVideoFrameCount(VOID * pVideo, long * plFrameCount)
{

#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->FrameCount();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}
///// homam 


bool CMBitmapProcessing::GetBufferWSize(VOID * pVideo, INT * plFrameCount)
{
#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->BufferWSize();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}


bool CMBitmapProcessing::GetBufferHSize(VOID * pVideo, INT * plFrameCount)
{
#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->BufferHSize();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::GetTotalFrame (VOID * pVideo, INT * plFrameCount)
{
	
#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
		
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->TotalFrame();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::GetDuration (VOID * pVideo, FLOAT * plFrameCount)
{
	
#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
	
	//MessageBox(NULL,_T("OPen D 1"),_T("***********"),NULL);
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->TotalDuration();
		
	    // MessageBox(NULL,_T("OPen D 2"),_T("***********"),NULL);
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}


bool CMBitmapProcessing::AllocateFrame(VOID * pVideo)
{
#ifdef _DEBUG 
	assert(pVideo != NULL);
#endif
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		pVE->AllocateFrm();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}


bool CMBitmapProcessing::GetNumofBytes (VOID * pVideo, INT * plFrameCount)
{

#ifdef _DEBUG 
	assert(pVideo != NULL);
	assert(plFrameCount != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->TotalNumofBytes();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::AllocateBuffer(VOID * pVideo , _TCHAR* lpszFilename)
{
	
#ifdef _DEBUG 
	assert(ppVideo != NULL);
	assert(lpszFilename != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		pVE->AllocateBuf(lpszFilename);
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::GetVideoIplImage(VOID * pVideo, /*IplImage* *phbm*/ cv::Mat &phbm, long lFrame)
{
#ifdef _DEBUG 
	assert(ppVideo != NULL);
	assert(*phbm != NULL);
#endif
	
	if (pVideo == NULL) return false;
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		//*phbm = pVE->GetIplImage(lFrame);
        phbm = pVE->GetIplImage(lFrame);
		if (phbm.empty())// (*phbm == NULL)
		{
		//	free(pVE);	//JH - cant free, all the pVideo content will gone..
		//	pVE = NULL;
			return false;
		}
		//free(pVE);
		//pVE = NULL;
	}
	catch (...)
	{
		return false;
	}
	
	return true;
}


bool CMBitmapProcessing::GetInitStartTime(VOID * pVideo, FLOAT duration, long lFrame)
{
#ifdef _DEBUG 
	assert(ppVideo != NULL);
#endif
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		bool result = pVE->GetStartTime(lFrame , duration); 
		return result;
	}
	catch (...)
	{
		return FALSE;
	}
	return TRUE;
}


bool CMBitmapProcessing::IsNotLowQualityVid(VOID * pVideo , bool * plFrameCount)		//JH-added for Video Processing
{
#ifdef _DEBUG 	
	 assert(pVideo != NULL);
#endif
	
	if (pVideo == NULL) return FALSE;
	
	try
	{
		CVideoExtract * pVE = reinterpret_cast<CVideoExtract *>(pVideo);
		*plFrameCount = pVE->IsNotLowQualityVid();
	}
	catch (...)
	{
		return FALSE;
	}
	
	return *plFrameCount;
}


/**
 * <Short Desc/>    : Close the video file.
 * <pre>
 * Function name    : CMBitmapProcessing::CloseVideo
 * Description      : Close the video file.
 * Return type      : bool - TRUE   - Success
 *                           FALSE  - Error
 * Argument         : VOID ** ppVideo  -  Pointer to the pointer to the video object
 * </pre>
 **/

bool CMBitmapProcessing::CloseVideoJpeg(VOID ** ppVideo)
{
//	__DEBUG_EXECUTE assert(ppVideo != NULL);
	//MessageBox (NULL, " Close Video JPEG" , "Final result" , NULL);

	try
	{
		CVideoExtract * & pVE = reinterpret_cast<CVideoExtract * &>(*ppVideo);
		pVE->CloseVideo();
#ifdef _JHDEBUG_FileScan		//JH-DFS 
		printf("\nCloseVideo");
#endif		
		if ( &pVE != NULL)
		{
		//	free(pVE);	//JH - solve malloc error
			delete pVE;
#ifdef _JHDEBUG_FileScan		//JH-DFS 
			printf("\ndelete pVE");
#endif					
			pVE = NULL;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::CloseVideoJpeg1(VOID ** ppVideo)
{
//	__DEBUG_EXECUTE assert(ppVideo != NULL);
	//MessageBox (NULL, " Close Video JPEG1" , "Final result" , NULL);
	
	try
	{
		CVideoExtract * & pVE = reinterpret_cast<CVideoExtract * &>(*ppVideo);
		pVE->CloseVideo1();
		
		if (pVE != NULL)
		{
		//	free(pVE);	//JH - solve malloc error
			delete pVE;
			pVE = NULL;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}

bool CMBitmapProcessing::CloseVideoJpegNoFrame(VOID ** ppVideo)
{
	//	__DEBUG_EXECUTE assert(ppVideo != NULL);
	//MessageBox (NULL, " Close Video JPEG1" , "Final result" , NULL);
	
	try
	{
		CVideoExtract * & pVE = reinterpret_cast<CVideoExtract * &>(*ppVideo);
		pVE->CVideoExtract::CloseVideo_avcodec_close_pCodecCtx();	
		pVE->CloseVideo_av_close_input_file_pFormatCtx();
		
		if (pVE != NULL)
		{
			//	free(pVE);	//JH - solve malloc error
			delete pVE;
			pVE = NULL;
		}
	}
	catch (...)
	{
		return FALSE;
	}
	
	return TRUE;
}
