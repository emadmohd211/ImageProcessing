/*
 *  ImageProcessing.cpp
 *  ImageProcessingC
 *
 *
 */

#include "ImageProcessing.h"
//#include "jpegdecoder.h"

#include <math.h>
#include <time.h>
#include <errno.h>//test
#include <fstream>
#include <iostream>
#include "KimPornNet.h"
#include "typedefin.h"
#include "CMImageProcessing.h"
#include "opencv2/imgproc/imgproc_c.h"
#include <opencv2/core/types_c.h>
//include <opencv2/core/core_c.h>
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui_c.h"
#include <opencv2/objdetect/objdetect.hpp>
#include "opencv2/core.hpp"
using std::ifstream;
using std::ofstream;
using std::ios;
using namespace cv;

typedef   signed char  schar;       /*  8 bits     */
typedef unsigned char  uchar;       /*  8 bits     */

//char *szfilepath = "/emad/";

char *szfilepath = "/Library/Application Support/temp/";

extern char * PornImagePath;  

//////////////////////////////////////////////////////////////////////////////////////////////
// STATIC CONST DEFINITION
//////////////////////////////////////////////////////////////////////////////////////////////
//const DOUBLE WQImageProcessing::THRESHOLD_QUICKSCAN      = 0.10;
//const INT WQImageProcessing::SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD = 15;
//const INT WQImageProcessing::JPEG_SKIN_PERCENTAGE_THRESHOLD = 6;
//const char  WQImageProcessing::cCubeMask[CUBEMASK_SIZE]  = {0x80,0x40,0x20,0x10,0x08,0x04,0x02,0x01};
const char  ImageProcessing::cCubeMask[CUBEMASK_SIZE]  = {(CHAR) 0x01,
	(CHAR) 0x02,
	(CHAR) 0x04,
	(CHAR) 0x08,
	(CHAR) 0x10,
	(CHAR) 0x20,
	(CHAR) 0x40,
	(CHAR) 0x80};
const DWORD ImageProcessing::dwColourCubeSize          = CUBESIZE;
const INT   ImageProcessing::aBitsSet[BITSET_SIZE]     =
{
	0,1,1,2,
	1,2,2,3,
	1,2,2,3,
	2,3,3,4,
	1,2,2,3,
	2,3,3,4,
	2,3,3,4,
	3,4,4,5,
	1,2,2,3,
	2,3,3,4,
	2,3,3,4,
	3,4,4,5,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	1,2,2,3,
	2,3,3,4,
	2,3,3,4,
	3,4,4,5,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	3,4,4,5,
	4,5,5,6,
	4,5,5,6,
	5,6,6,7,
	1,2,2,3,
	2,3,3,4,
	2,3,3,4,
	3,4,4,5,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	3,4,4,5,
	4,5,5,6,
	4,5,5,6,
	5,6,6,7,
	2,3,3,4,
	3,4,4,5,
	3,4,4,5,
	4,5,5,6,
	3,4,4,5,
	4,5,5,6,
	4,5,5,6,
	5,6,6,7,
	3,4,4,5,
	4,5,5,6,
	4,5,5,6,
	5,6,6,7,
	4,5,5,6,
	5,6,6,7,
	5,6,6,7,
	6,7,7,8,
	0
};

/*---------------------------------------------------------------------------
 //-  Function Name : ScanFile
 //-  Author        : Emad
 //-  Description   : To determine the porn conficence value of image path
 //-  Parameter     : lpszFile - image path
 enMedia_Type - media type either of subimage or fullimage
 enEngine_Type - engine type either of screenshield or ishield
 enScan_Type - scanning type either of normal or quick
 sysResults - a structure that contain confidence value
 //-  Return Value  : TRUE if success, and FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ScanFile_3p5(char*              lpszFile,
								EN_FSIP_MEDIA_TYPE   enMedia_Type,
                                 EN_FSIP_ENGINE_TYPE  enEngine_Type,
                                 EN_FSIP_SCAN_TYPE    enScan_Type,
                                 ImageProcessing::SYSTEM_IP_RESULTS * sysResults)
{
	//MessageBoxA(NULL,"File Scaning 2010 // WQImageProcessing::ScanFile","Point4_7",NULL);
	
	// to test the image processing functions
	
	if (IsInit() == FALSE)
	{
		return FALSE;
	}
	
    if (lpszFile==NULL) return FALSE;
	
//	EngineInitialisation(enEngine_Type);
	EngineInitialisation_V3_6(enEngine_Type);	//JH *V3.6 is backward compatible to V3.5
	WQ_IP_RESULTS pResults = {0};
	INT fastFileCheckValue = 0;
	INT fastFileCheckValueMix = 0; //JHA
	
	if(enEngine_Type == FSIP_MIXED_ENGINE_APR_2006 || enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) //JHA
	{
		// safe setting
		//	if (ShowDetailedImage == TRUE) _tcscat(ImageAnalysisMsg , _T("Starting Fast Filter For Safe Engine: "));
		fastFileCheckValueMix =   FastFileBasedDecision_V3_6(lpszFile);
		// safest Setting
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=15;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=6;
		//	if (ShowDetailedImage == TRUE) _tcscat(ImageAnalysisMsg , _T("Starting Fast Filter For Safest Engine: "));
		fastFileCheckValue=   FastFileBasedDecision_V3_6(lpszFile);
	}
	// THe FastFileBasedDecision can crash on jpg of order 7Mb so if jpg 1Mb then skipping
	else 
	{	
		
	// THe FastFileBasedDecision can crash on jpg of order 7Mb so if jpg 1Mb then skipping
		fastFileCheckValue=   FastFileBasedDecision(lpszFile); //0; //JH -retun 1 as non porn , 0 as proceed , -1 = error & exit
	}
	
	//char homam[256]={0};
	//sprintf(homam, "Fast Decision Vlaue  = %d" ,fastFileCheckValue);
	//MessageBoxA(NULL,homam,"Point 4_8",NULL);
	
	if(fastFileCheckValue==-1) 
	{
		// problem
		printf("file error");
		//MessageBox(NULL,"fastFileCheckValue==-1","",0);
		return FALSE;
	}
	
	if(fastFileCheckValue==1)
	{
		//MessageBox(NULL,"Non Porno Image","Point 5_0",0);
		
		// non porn
		
		pResults.NetPornValue=0;
		pResults.pf=0;
		pResults.kof=-2;
		pResults.kosf=-2;
		// homam fastfilter
		NumThumbs++;
		
		
	}
	
	if(fastFileCheckValue==0)
	{
		// continue processing
		
         Mat img_p = imread(lpszFile, 1); //updating to opencv 4 :: emad 18thfeb19
    //    imwrite("output_imread.jpg", mat);
        
		if (img_p.data)//SFimg)    //updating opencv api
		{
			//MessageBox(NULL,_T("image loaded"),_T("Point 5_1"),NULL);
			
			if(enMedia_Type==FSIP_MEDIA_FULL_IMAGE)
            {
				int bProcess;
				
				if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006|| enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) {
					
                    bProcess = MixedAnalysisProcess(img_p,&pResults,enEngine_Type,enScan_Type);
                } else {
				
                    bProcess = AnalysisProcess(img_p,&pResults,enEngine_Type,enScan_Type);	
                }

	
				if(bProcess)
				{
					sysResults->dConfidence_Value = static_cast<INT>(pResults.NetPornValue*100.0f);
#ifdef _JHDebug_Image		
					static unsigned int fRT[6];
					static unsigned int fpornCount;
					static unsigned int fnonPornCount;
				
					
					char pathbuffer[200];
					for(int k=0;k<200;k++)
						pathbuffer[k]='\0';
					strcpy (pathbuffer, "/Users/admin-imac2/Images/Result.log");
					char *filename = strrchr(lpszFile,'/');
					++filename;
					FILE *fi;
					fi=fopen(pathbuffer, "a");
					if (pResults.NetPornValue==1) {
						++fpornCount;						
						if (pResults.RT>=0 && pResults.RT<6) { ++fRT[pResults.RT]; }
						fprintf(fi, "\nP \t%60.60s\t%d  %d \t%d  %d \t PC:%d\t RT>1:%d\t2:%d\t3:%d\t4:%d\t5:%d\t0:%d\t",filename, pResults.RT,pResults.FT,pResults.kof,pResults.kosf,fpornCount, fRT[1],fRT[2],fRT[3],fRT[4],fRT[5],fRT[0]);
					}else {
						++fnonPornCount;
						fprintf(fi, "\nNP\t%60.60s\t%d  %d \t%d  %d \t NC:%d",filename, pResults.RT,pResults.FT,pResults.kof,pResults.kosf,fnonPornCount);
					}
					
					
					fclose(fi);
					
#endif		
				}
				else
				{		
					sysResults->dConfidence_Value = -1;
					return FALSE;
				}
				
			}
			else
			{
				//MessageBox(NULL,"Subimage process","",0);
				std::vector<struct SUB_IMAGE> pSub;
				std::vector<struct WQ_IP_RESULTS> pFeature;
				// Obtain the sub images and their porn values 
				
		//		if(CaptureWindowProcess(hbmp,pSub,pFeature,enEngine_Type,enScan_Type))	
				if(MixedCaptureWindowProcess(img_p,pSub,pFeature,enEngine_Type,enScan_Type))
				{
					// Obtain the final porn value by analysing the subimages
					std::vector <FLOAT> vRes;
					std::vector <FLOAT> vArea;
					std::vector <FLOAT> vRatio;
					for(int i=0;i<pFeature.size();i++)
					{
#ifdef _JHDebug_ScreenShot		
						printf("\nsubimage%d NPV:%.2f",i, pFeature[i].NetPornValue);
#endif
						vRes.push_back(pFeature[i].NetPornValue);
						vArea.push_back(pSub[i].imgRatioArea);
						vRatio.push_back(pSub[i].ratio);
					}
					FLOAT sub_pornvalue;
					int best = 0;
					sub_pornvalue=GetPornValueFromImage(&vRes,&vArea,&vRatio, best);
					sysResults->dConfidence_Value = static_cast<INT>(sub_pornvalue*100.0f);
					
					// Need to dump the hash value into registry
					if (best<pSub.size())
					{
				//		SetImageHash(pSub[best].hash);	//JH- remove not supported by MAC
						// "best" porn sub image is choose, hash key and path is save
						strcpy(ImagePath,pSub[best].path);
						PornImagePath = ImagePath;
						strcpy(pIHash, pSub[best].hash);
					}
				}
				else
				{
					sysResults->dConfidence_Value = -1;
					return FALSE;
				}
			}
		}
		else
		{
			//MessageBox(NULL,_T("image failed"),_T(""),NULL);
			printf("fail to open image");
			return FALSE;
		}
		//cvReleaseImage(&img_p);
	}
	return TRUE;
}

/*---------------------------------------------------------------------------
 //-  Description   : To determine the porn conficence value of image path
 //-  Parameter     : lpszFile - image path
 enMedia_Type - media type either of subimage or fullimage
 enEngine_Type - engine type either of screenshield or ishield
 enScan_Type - scanning type either of normal or quick
 sysResults - a structure that contain confidence value
 //-  Return Value  : TRUE if success, and FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ScanFile(char*              lpszFile,
								   EN_FSIP_MEDIA_TYPE   enMedia_Type,
								   EN_FSIP_ENGINE_TYPE  enEngine_Type,
								   EN_FSIP_SCAN_TYPE    enScan_Type,
								   ImageProcessing::SYSTEM_IP_RESULTS * sysResults)
{
	
	
	if (IsInit() == FALSE)
	{
		return FALSE;
	}
	
    if (lpszFile==NULL) return FALSE;
	
	//	EngineInitialisation(enEngine_Type);
	EngineInitialisation_V3_6(enEngine_Type);	
	WQ_IP_RESULTS pResults = {0};
	INT fastFileCheckValue = 0;
	INT fastFileCheckValueMix = 0; //JHA
	
	if(enEngine_Type == FSIP_MIXED_ENGINE_APR_2006 || enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) //JHA
	{
		
		fastFileCheckValueMix =   FastFileBasedDecision_V3_6(lpszFile);
		// safest Setting
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=15;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=6;
		//	if (ShowDetailedImage == TRUE) _tcscat(ImageAnalysisMsg , _T("Starting Fast Filter For Safest Engine: "));
		fastFileCheckValue=   FastFileBasedDecision_V3_6(lpszFile);
	}

	else 
	{	
		
		// THe FastFileBasedDecision can crash on jpg of order 7Mb so if jpg 1Mb then skipping
		fastFileCheckValue=   FastFileBasedDecision_V3_6(lpszFile); //0; //JH -retun 1 as non porn , 0 as proceed , -1 = error & exit
	}
	
	
	
	if(fastFileCheckValue==-1) 
	{
		// problem
		printf("file error");
		//MessageBox(NULL,"fastFileCheckValue==-1","",0);
		return FALSE;
	}
	
	if(fastFileCheckValue==1)
	{
		//MessageBox(NULL,"Non Porno Image","Point 5_0",0);
		
		// non porn
		
		pResults.NetPornValue=0;
		pResults.pf=0;
		pResults.kof=-2;
		pResults.kosf=-2;
		// homam fastfilter
		NumThumbs++;
		
	}
	
	if(fastFileCheckValue==0)
	{
		
        Mat img_p = imread(lpszFile, 1); //updating to opencv 4 :: emad 18thfeb19
        
		
		if (img_p.data)
		{
			//MessageBox(NULL,_T("image loaded"),_T("Point 5_1"),NULL);
			
			if(enMedia_Type==FSIP_MEDIA_FULL_IMAGE)
			{

				//JH choose engine version base on engine type
				if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006){
					
					if(MixedAnalysisProcess(img_p,&pResults,enEngine_Type,enScan_Type)) //updated for opencv 4 emad 18thfeb19
					{
						sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
					}
					else
					{
						sysResults->dConfidence_Value = -1;
						return FALSE;
					}
					
					if ((fastFileCheckValueMix == 1 || fastFileCheckValueMix == -1)  && pResults.RT > 1 )
					{
						
						pResults.RT = 2;
						sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
					}
					
					else sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
					
				} 
				
				//FSIP_MIXEDVID_ENGINE_APR_2006 is Specific type for Processing the video and Stream
				else if (enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006)
				{
                    if(MixedAnalysisProcess(img_p,&pResults,enEngine_Type,enScan_Type)){				{
						NumThumbs++;
						if (pResults.FT == 25 && pResults.RT ==1) // Catched By SVM as porn make sysResults->dConfidence_Value as 5 in order to be differenicated later while calculating the weight (0.5 rahter than 1) 
						{
							sysResults->dConfidence_Value = 5;
						}
						
						else sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
						
					}
					else
					{
						sysResults->dConfidence_Value = -1;
						return FALSE;
					}
					
				}
				else 
				{	
					if(AnalysisProcess(img_p,&pResults,enEngine_Type,enScan_Type))
                    {
						//sysResults->dConfidence_Value = static_cast<INT>(pResults.NetPornValue*100.0f);
						sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
					}
					else
					{
						sysResults->dConfidence_Value = -1;
						return FALSE;
					}
					
				}
				
#ifdef _JHDebug_Image		
				
				
				static unsigned int fRT[6];
				static unsigned int fpornCount;
				static unsigned int fnonPornCount;
				
				
				char pathbuffer[200];
				for(int k=0;k<200;k++)
					pathbuffer[k]='\0';
				strcpy (pathbuffer, "/Users/admin-imac2/Images/Result.log");
				char *filename = strrchr(lpszFile,'/');
				++filename;
				FILE *fi;
				fi=fopen(pathbuffer, "a");
				if (pResults.NetPornValue==1) {
					++fpornCount;						
					if (pResults.RT>=0 && pResults.RT<6) { ++fRT[pResults.RT]; }
					fprintf(fi, "\nP \t%60.60s\t%d  %d \t%d  %d \t PC:%d\t RT>1:%d\t2:%d\t3:%d\t4:%d\t5:%d\t0:%d\t",filename, pResults.RT,pResults.FT,pResults.kof,pResults.kosf,fpornCount, fRT[1],fRT[2],fRT[3],fRT[4],fRT[5],fRT[0]);
				}else {
					++fnonPornCount;
					fprintf(fi, "\nNP\t%60.60s\t%d  %d \t%d  %d \t NC:%d",filename, pResults.RT,pResults.FT,pResults.kof,pResults.kosf,fnonPornCount);
				}
				
				
				fclose(fi);
				
#endif		
				
			}
			else
			{
				//MessageBox(NULL,"Subimage process","",0);
				std::vector<struct SUB_IMAGE> pSub;
				std::vector<struct WQ_IP_RESULTS> pFeature;
				// Obtain the sub images and their porn values 
				
				//		if(CaptureWindowProcess(hbmp,pSub,pFeature,enEngine_Type,enScan_Type))	
				if(MixedCaptureWindowProcess(img_p,pSub,pFeature,enEngine_Type,enScan_Type))
				{
					// Obtain the final porn value by analysing the subimages
					std::vector <FLOAT> vRes;
					std::vector <FLOAT> vArea;
					std::vector <FLOAT> vRatio;
					for(int i=0;i<pFeature.size();i++)
					{
#ifdef _JHDebug_ScreenShot		
						printf("\nsubimage%d NPV:%.2f",i, pFeature[i].NetPornValue);
#endif
						vRes.push_back(pFeature[i].NetPornValue);
						vArea.push_back(pSub[i].imgRatioArea);
						vRatio.push_back(pSub[i].ratio);
					}
					FLOAT sub_pornvalue;
					int best = 0;
					sub_pornvalue=GetPornValueFromImage(&vRes,&vArea,&vRatio, best);
					sysResults->dConfidence_Value = static_cast<INT>(sub_pornvalue*100.0f);
					
					// Need to dump the hash value into registry
					if (best<pSub.size())
					{
						
						strcpy(ImagePath,pSub[best].path);
						PornImagePath = ImagePath;
						strcpy(pIHash, pSub[best].hash);
					}
				}
				else
				{
					sysResults->dConfidence_Value = -1;
					return FALSE;
				}
			}
		}
		else
		{
			//MessageBox(NULL,_T("image failed"),_T(""),NULL);
			printf("fail to open image");
			return FALSE;
		}

		//cvReleaseImage(&img_p);
}
	return TRUE;
}

/*---------------------------------------------------------------------------

 //-  Description   : To determine the porn conficence value of image path
 //-  Parameter     : lpszFile - image path
 enMedia_Type - media type either of subimage or fullimage
 enEngine_Type - engine type either of screenshield or ishield
 enScan_Type - scanning type either of normal or quick
 sysResults - a structure that contain confidence value
 //-  Return Value  : TRUE if success, and FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ScanImage(/*IplImage* */ Mat hbmp,
							   EN_FSIP_MEDIA_TYPE   enMedia_Type,
							   EN_FSIP_ENGINE_TYPE  enEngine_Type,
							   EN_FSIP_SCAN_TYPE    enScan_Type,
							   ImageProcessing::SYSTEM_IP_RESULTS * sysResults)
{
	if (IsInit() == FALSE)
	{
		return FALSE;
	}
	
//	EngineInitialisation(enEngine_Type);
	EngineInitialisation_V3_6(enEngine_Type);	
	if(enMedia_Type==FSIP_MEDIA_FULL_IMAGE)
	{
		WQ_IP_RESULTS pResults = {0};
		
		if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006)
		{
			if(MixedAnalysisProcess(hbmp,&pResults,enEngine_Type,enScan_Type))
			{
				sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
		}
		
		//FSIP_MIXEDVID_ENGINE_APR_2006 is Specific type for Processing the video and Stream
		else if (enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006)
		{
			//MessageBox(NULL , _T("Mixed Video") , _T("Start Dumping")  , NULL);
			if(MixedAnalysisProcess(hbmp,&pResults,enEngine_Type,enScan_Type))
			{
				NumThumbs++;
				if (pResults.FT == 25 && pResults.RT ==1) // Catched By SVM as porn make sysResults->dConfidence_Value as 5 in order to be differenicated later while calculating the weight (0.5 rahter than 1) 
				{
					sysResults->dConfidence_Value = 5;
				}
				
				else sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
				
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
		}
		else 
		{
			if(AnalysisProcess(hbmp,&pResults,enEngine_Type,enScan_Type))
			{
				sysResults->dConfidence_Value = static_cast<INT>(pResults.NetPornValue*100.0f);
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
			
		}
		

	}
	else
	{
		std::vector<struct SUB_IMAGE> pSub;
		std::vector<struct WQ_IP_RESULTS> pFeature;
		// Obtain the sub images and their porn values 
		if(CaptureWindowProcess(hbmp,pSub,pFeature,enEngine_Type,enScan_Type))
		{
			// Obtain the final porn value by analysing the subimages
			std::vector <FLOAT> vRes;
			std::vector <FLOAT> vArea;
			std::vector <FLOAT> vRatio;
			for(int i=0;i<pFeature.size();i++)
			{
				vRes.push_back(pFeature[i].NetPornValue);
				vArea.push_back(pSub[i].imgRatioArea);
				vRatio.push_back(pSub[i].ratio);
			}
			
			
			FLOAT sub_pornvalue;
			int best = 0;
			sub_pornvalue= GetPornValueFromImage(&vRes,&vArea,&vRatio, best);
			sysResults->dConfidence_Value = static_cast<INT>(sub_pornvalue*100.0f);
			// Need to dump the hash value into registry
			if (best<pSub.size())
			{
	//			SetImageHash(pSub[best].hash);
			}
			
		}
		else
		{
			sysResults->dConfidence_Value = -1;
			return FALSE;
		}
		
	}
	
	return TRUE;
}



/*---------------------------------------------------------------------------

 //-  Description   : Loading various files: T4.bin, luv.bin,fface.xml,porn.dat and fastporn.dat
 //-  Parameter     : pSecurity = This contains the information for users rights/privileges
 //-  Return Value  : -
 //-------------------------------------------------------------------------*/
ImageProcessing::ImageProcessing()
:

hColourCube(NULL), 
pCube(NULL), 
oneD_view(NULL),
m_bInit(FALSE)
{
	Init();
}



/*---------------------------------------------------------------------------

 //-  Description   : Releasing memories that are created from T4.bin, luv.bin and fface.xml
 //-  Parameter     : -
 //-  Return Value  : -
 //-------------------------------------------------------------------------*/
ImageProcessing::~ImageProcessing()
{
	Uninit();
}


/*---------------------------------------------------------------------------
  
 //-  Description   : Initialize memory buffer for T4.bin and luv.bin, as well loading 
 such as T4.bin, luv.bin, porn.dat, fastporn.dat and fface.xml
 //-  Parameter     : -
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::Init()
{
	
	//  MessageBoxA(NULL,"WQImageProcessing::Constructor Calling Init","Point1_5_0",NULL);
	
	
				m_bInit = FALSE;
	
				DWORD dwError;
				cascade = NULL;
				cascade2 = NULL;	
				cascade3 = NULL;	
	
				dwError = AllocateCube();
				if (dwError!=0) return FALSE;
				
				dwError = AllocateLUV();
				if (dwError!=0) return FALSE;
	
				int a = strlen(szfilepath); 
				int b = strlen(FS_FILENAME_SKINDAT); 
	
				int max_len = a+b;
				
				_TCHAR szFilenameSkin[MAX_PATH] = {0};
	
					
				strcpy(szFilenameSkin, szfilepath );
			
				strcat(szFilenameSkin, FS_FILENAME_SKINDAT );
		
				bool gotFile=false;
				{
					FILE *test = fopen(szFilenameSkin,"r");
                    int err = errno;
                    printf("errno : %d - \n", err);
					if (test!=NULL) 
					{
						fclose(test);
						gotFile=true;
					}
				}
				if (!gotFile) 
				{
					return -1;
					//MessageBox ( NULL , lpszFilename , "file name" , NULL);
				}
	
	
				//MessageBox(NULL,szFilenameSkin,FS_FILENAME_SKINDAT,NULL);
				
				dwError = LoadCube(szFilenameSkin);
				if (dwError!=0) return FALSE;
				
				_TCHAR szLUVFilename[MAX_PATH] = {0};
				// try the product path first
	
				strcpy(szLUVFilename, szfilepath);
	
				strcat(szLUVFilename, FS_FILENAME_LUVDAT);
	
				 gotFile=false;
				{
					FILE *test = fopen(szLUVFilename,"r");
                    int err = errno;
                    printf("errno : %d - \n", err);
					if (test!=NULL) 
					{
						fclose(test);
						gotFile=true;
					}
                   
				}
				if (!gotFile) 
				{
					return -1;
					//MessageBox ( NULL , lpszFilename , "file name" , NULL);
				}
				
				dwError = LoadLUV(szLUVFilename);
				if (dwError!=0) return FALSE;
				
				dwError = LoadPorn();
				if (dwError!=0) return FALSE;
				
				dwError = LoadCascade();
				if (dwError!=0) cascade = NULL;
				// The line immediately above was added.
	
				dwError = LoadCascade2();		
				if (dwError!=0) cascade2 = NULL;
	
				dwError = LoadCascade3();	
				if (dwError!=0) cascade3 = NULL;
				
				// MessageBox(NULL,_T("WQImageProcessing Initiated"),_T("Image Engine"),NULL);
				
				
				m_bInit = TRUE;
				
				return TRUE;
	
}



/*---------------------------------------------------------------------------
 
 //-  Description   : Release memory buffer from T4.bin, luv.bin and fface.xml
 //-  Parameter     : -
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::Uninit()
{
	//MessageBox(NULL,_T("in WQImageProcessing::Uninit()"),_T(""),NULL);
	
	ReleaseCube();
	ReleaseLUV();
	ReleaseCascade();
	
	//MessageBox(NULL,_T("WQImageProcessing::Uninit() done"),_T(""),NULL);
	
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////////////////////

//-  Description   : Return the flag to indicate if its "initialized" properly
//-  Parameter     : -
//-  Return Value  : TRUE if initialized properly, FALSE if otherwise
//-------------------------------------------------------------------------*/
bool ImageProcessing::IsInit()
{
	return m_bInit;
}

          
//-  Description   : Allocate memory buffer for T4.bin
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::AllocateCube(void)
{
	//	MessageBoxA(NULL,"WQImageProcessing::AllocateCube","Allocation",NULL);
	
	
	hColourCube=malloc(dwColourCubeSize);
	
	// allocate ptrs
	pCube=(char *)hColourCube;
	
	return 0;
}

            
//-  Description   : Allocate memory buffer for luv.bin
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::AllocateLUV(void)
{
	//	MessageBoxA(NULL,"WQImageProcessing::AllocateLUV","Allocation",NULL);
	
	
	unsigned long LUVsize = 3*64*64*64;
	hLUV=malloc(LUVsize);
	
	// allocate ptrs
	pLUV=(unsigned char *)hLUV;
	
	luvSQRT = new INT[400*400];
	luvEXP = new INT[400];
	gammaTable = new INT[400];
	
	return 0;
}

            
//-  Description   : Load the skin.bin
//-  Parameter     : pcName - the name of skin lookup file, T4.bin
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadCube(const _TCHAR *pcName)
{
	
	char cBuffer[MAX_PATH+4];
	FILE* hCubeFile;
	DWORD dwSizeLo;
	DWORD dwSizeHi;
	DWORD dwPosition;
	DWORD dwRead;
	DWORD dwLength;
	DWORD dwRed;
	DWORD dwGreen;
	size_t result;
	
	
	if(pCube==NULL)
	{
		return -1;
	}
	memset(pCube,0,dwColourCubeSize);
	
	//hCubeFile=CreateFile(pcName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_EXISTING,(FILE_ATTRIBUTE_NORMAL|FILE_FLAG_SEQUENTIAL_SCAN),NULL);
	
	hCubeFile = fopen(pcName,("r"));
	
	//DWORD dwRet = GetLastError();
	
	if(hCubeFile==NULL)
	{
		return -1;
	}
	
	// get file size
	dwSizeHi=0;
	
	fseek(hCubeFile, 0L, SEEK_END);
	dwSizeLo = ftell(hCubeFile);
	
	
	
	//dwSizeLo=GetFileSize(hCubeFile,&dwSizeHi);
	if(dwSizeLo==0xFFFFFFFF)
	{
		return -1;
	}
	
	if(dwSizeHi!=0)
	{
		return -1;
	}
	
	fseek(hCubeFile, 0L, SEEK_SET);

	
	dwLength=RGBLUELINE;
	
	for(dwPosition=0;dwPosition<dwSizeLo;)
	{
		
		dwRead=0;
		//dwError=ReadFile(hCubeFile,cBuffer,dwLength,&dwRead,NULL);
		
		result = fread(cBuffer ,1 , dwLength , hCubeFile);
		
		
		
		if(result != dwLength)
			break;	// on error or EOF end
		
   		// insert data into cube
		dwRed=(DWORD)(cBuffer[0]&0xFF);
		dwGreen=(DWORD)(cBuffer[1]&0xFF);
		
	
		memmove(&pCube[(dwRed<<REDSHIFT)|(dwGreen<<GREENSHIFT)],cBuffer+2,BLUELINE);

		
	}
	// close file
	fclose(hCubeFile);
	return 0;
}


/*---------------------------------------------------------------------------
n   : Load the fface.xml
 //-  Parameter     : void
 //-  Return Value  : 0 if success, other than 0 if fail
 //-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadCascade(void)
{
	_TCHAR lpszFilename[MAX_PATH] = {0};
	
	strcpy(lpszFilename, szfilepath);
	
	strcat(lpszFilename, FS_FILENAME_FFACEXML);
	
	bool gotFile=false;
	{
				
				FILE *test = fopen(lpszFilename,"r");
				if (test!=NULL) 
				{
					fclose(test);
					gotFile=true;
				}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	

	const CHAR * szFilenameA = lpszFilename;
	
	
      bool res = _cascade.load(szFilenameA);// updated to opencv4 emad 18feb2019
    //if (cascade==NULL) return -1;
    if (!res) return -1;
    return 0;
}


/*---------------------------------------------------------------------------

 //-  Description   : Load the fface.xml
 //-  Parameter     : void
 //-  Return Value  : 0 if success, other than 0 if fail
 //-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadCascade2(void)	
{
	_TCHAR lpszFilename[MAX_PATH] = {0};
	
	strcpy(lpszFilename, szfilepath);
	
	strcat(lpszFilename, FS_FILENAME_FFACEXML2);
	
	bool gotFile=false;
	{
		
		FILE *test = fopen(lpszFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	
	const CHAR * szFilenameA = lpszFilename;
	
	

    bool res = _cascade2.load(szFilenameA);// updated to opencv4 emad 18feb2019
    //if (cascade==NULL) return -1;
    if (!res) return -1;
    return 0;
}



/*---------------------------------------------------------------------------
 //-  Function Name : LoadCascade
 //-  Description   : Load the fface.xml
 //-  Parameter     : void
 //-  Return Value  : 0 if success, other than 0 if fail
 //-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadCascade3(void)		
{
	_TCHAR lpszFilename[MAX_PATH] = {0};
	
	strcpy(lpszFilename, szfilepath);
	
	strcat(lpszFilename, FS_FILENAME_FFACEXML3);
	
	bool gotFile=false;
	{
		
		FILE *test = fopen(lpszFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	
	const CHAR * szFilenameA = lpszFilename;
	
	
    bool res = _cascade3.load(szFilenameA);// updated to opencv4 emad 18feb2019
    //if (cascade==NULL) return -1;
    if (!res) return -1;
    return 0;
}

//-  Function Name : LoadPorn
//-  Description   : Load porn files: porn.dat, fastporn.dat
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadPorn(void)
{
	
	// try the product path first
	
	strcpy(m_szPornFilename, szfilepath);
	
	strcat(m_szPornFilename, FS_FILENAME_PORNDAT);
	
	bool gotFile=false;
	{
		FILE *test = fopen(m_szPornFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	
	strcpy(m_szFastPornFilename, szfilepath);
	
	strcat(m_szFastPornFilename, FS_FILENAME_FASTPORNDAT);
	
	gotFile=false;
	{
		FILE *test = fopen(m_szFastPornFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	
	strcpy(m_szPornSVMFilename, szfilepath);
	
	strcat(m_szPornSVMFilename, FS_FILENAME_PORNSVM);
	
	gotFile=false;
	{
		FILE *test = fopen(m_szPornSVMFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	strcpy(m_szsafestPornSVMFilename, szfilepath);
	
	strcat(m_szsafestPornSVMFilename, FS_FILENAME_SAFESTPORNSVM);
	
	gotFile=false;
	{
		FILE *test = fopen(m_szsafestPornSVMFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}

	
	strcpy(m_szsafestPornSVMFilename_old, szfilepath);
	
	strcat(m_szsafestPornSVMFilename_old, FS_FILENAME_SAFESTPORNSVM_OLD);
	
	gotFile=false;
	{
		FILE *test = fopen(m_szsafestPornSVMFilename_old,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}

	
	strcpy(m_szssafestPornSVMFilename, szfilepath);
	
	strcat(m_szssafestPornSVMFilename, FS_FILENAME_SSAFESTPORNSVM);
	
	gotFile=false;
	{
		FILE *test = fopen(m_szssafestPornSVMFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}

	strcpy(m_szsafeNonPornNNFilename, szfilepath);

	strcat(m_szsafeNonPornNNFilename, FS_FILENAME_NONPORNSAFENN);
	
//	strcat(m_szsafestNonPornSVMFilename, FS_FILENAME_SAFESTNONPORNSVM);
	
	gotFile=false;
	{
	//	FILE *test = fopen(m_szsafestNonPornSVMFilename,"r");
		FILE *test = fopen(m_szsafeNonPornNNFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	strcpy(m_szsafestNonPornSVMFilename, szfilepath);
	strcat(m_szsafestNonPornSVMFilename, FS_FILENAME_SAFESTNONPORNSVM);
	
	gotFile=false;
	{
		//	FILE *test = fopen(m_szsafestNonPornSVMFilename,"r");
		FILE *test = fopen(m_szsafestNonPornSVMFilename,"r");
		if (test!=NULL) 
		{
			fclose(test);
			gotFile=true;
		}
	}
	if (!gotFile) 
	{
		return -1;
		//MessageBox ( NULL , lpszFilename , "file name" , NULL);
	}
	
	return 0;
}



//-  Function Name : LoadLUV
             
//-  Description   : Load the luv.bin
//-  Parameter     : pcName - luv file: luv.bin
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::LoadLUV(const _TCHAR *pcName)
{

	FILE *hLUVFile;
	DWORD dwSizeLo;
	DWORD dwSizeHi;
	size_t result;
	
	
	if(pLUV==NULL) return -1;
	memset(pLUV,0,3*64*64*64);
	hLUVFile = fopen(pcName,("r"));
	if(hLUVFile==NULL)
		{
		  return -1;
		}
		   
		   // get file size
	dwSizeHi=0;
		   
	fseek(hLUVFile, 0L, SEEK_END);
	dwSizeLo = ftell(hLUVFile);
		   
		   
		   
		   //dwSizeLo=GetFileSize(hCubeFile,&dwSizeHi);
	if(dwSizeLo==0xFFFFFFFF)
	{
		return -1;
	}
		   
	if(dwSizeHi!=0)
	{
		return -1;
	}
		   
	fseek(hLUVFile, 0L, SEEK_SET);
		   
	result = fread(pLUV ,1 , 3*64*64*64 , hLUVFile);
		   
		
		   
	fclose(hLUVFile);
	
	// set up a sqrt array used as a lookup table
	INT k=0;
	for (INT i=0;i<400;i++)
	{
		INT limit = i*i;
		for (INT j=k;j<=limit;j++,k++)
		{
			luvSQRT[k]=i;
		}
	}
	
	for (int i=0;i<400;i++) luvEXP[i]=(INT) (1000.0* exp(((FLOAT)-i)));
	
	INT x0=25, x1=125;
	FLOAT gamma=0.0f;
	FLOAT phi=0.0f;
	FLOAT pi=3.142f;
	//	char* homam = new char[100];
	
	for(int i=0;i<=x0;i++)
	{
		phi=(float)(pi*i)/(2*x0);
		gamma=1+0.5*cos(phi);	
		gammaTable[i]=int(pow(i/255.0f,gamma)*255);
		
	}
	
	for(int i=x0+1;i<=x1;i++)
	{
		phi=(float)(pi/2);
		gamma=1+0.5*cos(phi);	
		gammaTable[i]=int(pow(i/255.0f,gamma)*255);
		
	}
	
	for(int i=x1+1;i<=255;i++)
	{
		phi=(pi-pi*(255-i))/(2*(255-x1));
		gamma=1+0.5*cos(phi);	
		gammaTable[i]=int(pow(i/255.0f,gamma)*255);
		
	}
	
	return 0;
}

//-  Function Name : ReleaseCascade
               
//-  Description   : Release the memory buffer from fface.xml
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::ReleaseCascade(void)
{
	
	return 0;
}

//-  Function Name : ReleaseCube
//-  Description   : Release the memory buffer from T4.bin
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::ReleaseCube(void)
{
	pCube=NULL;
	if(hColourCube!=NULL)
	{
		free(hColourCube);
		hColourCube=NULL;
	}
	return 0;
}

//-  Function Name : ReleaseLUV               
//-  Description   : Release the memory buffer from luv.bin
//-  Parameter     : void
//-  Return Value  : 0 if success, other than 0 if fail
//-------------------------------------------------------------------------*/
DWORD ImageProcessing::ReleaseLUV(void)
{
	pLUV=NULL;
	if(hLUV!=NULL)
	{
		free(hLUV);
		hLUV=NULL;
	}
	if (luvSQRT!=NULL) delete [] luvSQRT;
	if (luvEXP!=NULL) delete [] luvEXP;
	if (gammaTable!=NULL) delete [] gammaTable;
	return 0;
}
          
//-  Description   : Initialise parameters:
//-------------------------------------------------------------------------*/
VOID ImageProcessing::EngineInitialisation(EN_FSIP_ENGINE_TYPE enEngine_Type)
{
	//	MessageBoxA(NULL,"WQImageProcessing::ScanFile // Engine Initialization","Point4_7_1",NULL);
	
	if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006) //screenshield
	{
		THRESHOLD_QUICKSCAN=0.20;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=17;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=10;
		//MessageBox(NULL,"Safe Engine","",0);
	}
	if(enEngine_Type==FSIP_LAX_ENGINE_APR_2006) //screenshield
	{
		THRESHOLD_QUICKSCAN=0.20;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=17;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=10;
		//MessageBox(NULL,"Safe Engine","",0);
	}
	
	if(enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) //ishield
	{
		THRESHOLD_QUICKSCAN=0.10;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=15;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=6;
		//MessageBox(NULL,"Safest Engine","",0);
	}
}

VOID ImageProcessing::EngineInitialisation_V3_6(EN_FSIP_ENGINE_TYPE enEngine_Type)		
{
	if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006) //screenshield
	{
		THRESHOLD_QUICKSCAN=0.20;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=17;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=10;
	}
	if(enEngine_Type==FSIP_LAX_ENGINE_APR_2006 || enEngine_Type==FSIP_SLAX_ENGINE_APR_2006) //screenshield
	{
		THRESHOLD_QUICKSCAN=0.20;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=17;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=10;
	}
	
	if(enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) //ishield
	{
		THRESHOLD_QUICKSCAN=0.10;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=15;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=6;
	}
	
	if(enEngine_Type==FSIP_MIXED_ENGINE_APR_2006 || enEngine_Type==FSIP_MIXEDVID_ENGINE_APR_2006) //ishield
	{
		THRESHOLD_QUICKSCAN=0.20;
		SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD=17;
		JPEG_SKIN_PERCENTAGE_THRESHOLD=10;
	}
}

/*---------------------------------------------------------------------------
          
 //-  Description   : Crop the bottom of the image if damaged
 //-  Parameter     : pBitmapInfo - bitmap info
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/

INT ImageProcessing::QuickCropTest(BYTE* pBitmapInfo , int w , int h , int bpp)
{
	INT cropSize=0;
	if (pBitmapInfo == NULL)
	{
		return 0;
	}
	
	INT iWidth = w;
	INT iHeight = h;
	
	// Need to get new bottom in case of need to crop the image
	{
		
		BYTE * pbImage = pBitmapInfo;
		
		if (pbImage != NULL)
		{
			INT iWidthBytes = CMBitmapProcessing::CalcWidthBytes(w, bpp);    
			//delclaration and initialization of variable for gamma correction
		
			DWORD dwData=0;
			
			DWORD lastValue = RGB(pbImage[0], pbImage[1], pbImage[2]);
			
			for (int h=0; h<iHeight;h++)
			{
				INT index=0;
				dwData = RGB(pbImage[index], pbImage[index+1], pbImage[index+2]);
				if (dwData!=lastValue) {cropSize=h;break;}
				index=3*(iWidth/4);
				dwData = RGB(pbImage[index], pbImage[index+1], pbImage[index+2]);
				if (dwData!=lastValue) {cropSize=h;break;}
				index=6*(iWidth/4);
				dwData = RGB(pbImage[index], pbImage[index+1], pbImage[index+2]);
				if (dwData!=lastValue) {cropSize=h;break;}
				index=9*(iWidth/4);
				dwData = RGB(pbImage[index], pbImage[index+1], pbImage[index+2]);
				if (dwData!=lastValue) {cropSize=h;break;}
				pbImage += iWidthBytes;
			}
		}
		
		//TCHAR temps[256]={0};
		//_stprintf(temps,_T("crop_size = %d of %d"),cropSize,iHeight);
		//MessageBox(NULL,temps,_T("crop"),NULL);
	}
	return cropSize;
}



/*---------------------------------------------------------------------------
 //-  Function Name : FastFileBasedDecision
               
 //-  Description   : Fast analysis on jpeg and gif images on porngraphic level
 //-  Parameter     : fileName - file path that contain analyse image
 //-  Return Value  : -1 and -2 if file problem, 0 if undecided and 1 if nonporn
 //-------------------------------------------------------------------------*/
INT ImageProcessing::FastFileBasedDecision(const _TCHAR *fileName)
{
	//MessageBoxA(NULL,"WQImageProcessing::ScanFile // FastFileBasedDecision","Point4_7_2",NULL);
	INT w,h;
	
	FILE *readToBuffer;
    readToBuffer = fopen(fileName, "rb");
	if (readToBuffer==NULL)
	{
		printf("Failed To Open The Image File ");	//JH-added
		return -1;
	}
	
	
	
	fseek(readToBuffer, 0L, SEEK_END);     /* Position to end of file */
	int BufferLength = ftell(readToBuffer);        /* Get file length */
	
	if (BufferLength<10)
	{
		printf("Bad file Format ---- Exit");	//JH-added
		fclose(readToBuffer);
		return -1; // a bad file of some sort.
	}
	
	
	if (BufferLength>3000000)
	{
		printf("Very large file & proceed to next filter ");	//JH-added
		fclose(readToBuffer);
		//		return -1; // very large file and as such it just ain't worth checking unless it is a movie
		return 0; // very large file and as such it just ain't worth checking unless it is a movie
	}
	
	
	if (BufferLength<780)
	{
		printf("Very small file & exit as non-porn "); //JH-added
		fclose(readToBuffer);
		return 1; // very small file and as such it just ain't worth checking
	}
	
	// read file into a buffer
	rewind(readToBuffer);                  
	fread(Buffer,780,1,readToBuffer);
	fclose(readToBuffer);
	
	if (IsJPEG(Buffer,BufferLength) || IsExif(Buffer,BufferLength))
	{	
		return 0;
		
	}
	
	if (IsGIF(Buffer,BufferLength))
	{
		if (GetDimensionsGIF(Buffer,BufferLength,w,h))
		{
			if (w<=50 || h<=50) {return 1;} // nonporn
		}
		
		if (GetColourMapGIF(Buffer,BufferLength,gifColours))
		{
			int numSkinColours = GetNumberGIFSkinColours(gifColours);
			if (numSkinColours < SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD) return 1; // nonporn
			else return 0; // continue processing
		}
		else
		{
			return 0; // most likely low num of colour or animated
		}
	}
	
	
	if (IsBM(Buffer,BufferLength)) return 0; // process normally
	if (IsPNG(Buffer,BufferLength)) return 0; // process normally
	if (IsTIFF1(Buffer,BufferLength)) return 0; // process normally
	if (IsTIFF2(Buffer,BufferLength)) return 0; // process normally
	if (IsEMF(Buffer,BufferLength)) return 0; // process normally
	//MessageBox(NULL,"before Webp",_T("BufferLength<10"),NULL);
	//if (CMBitmapProcessing::ISWebp(Buffer,BufferLength)) return 0;*/
	
	//MessageBox(NULL,"4",_T("don't know what is"),NULL);
	
	return -1; // not an image
}

INT ImageProcessing::FastFileBasedDecision_V3_6(const _TCHAR *fileName)	//JH added
{
	//MessageBoxA(NULL,"WQImageProcessing::ScanFile // FastFileBasedDecision","Point4_7_2",NULL);
	INT w,h;
	
	FILE *readToBuffer;
    readToBuffer = fopen(fileName, "rb");
	if (readToBuffer==NULL)
	{
		printf("Failed To Open The Image File ");	//JH-added
		return -1;
	}
	
	
	
	fseek(readToBuffer, 0L, SEEK_END);     /* Position to end of file */
	int BufferLength = ftell(readToBuffer);        /* Get file length */
	
	if (BufferLength<10)
	{
		printf("Bad file Format ---- Exit");	//JH-added
		fclose(readToBuffer);
		return -1; // a bad file of some sort.
	}
	
	
	if (BufferLength>3000000)
	{
		printf("Very large file & proceed to next filter ");	//JH-added
		fclose(readToBuffer);
		//		return -1; // very large file and as such it just ain't worth checking unless it is a movie
		return 0; // very large file and as such it just ain't worth checking unless it is a movie
	}
	
	
	if (BufferLength<780)
	{
		printf("Very small file & exit as non-porn "); //JH-added
		fclose(readToBuffer);
		return 1; // very small file and as such it just ain't worth checking
	}
	
	// read file into a buffer
	// read file into a buffer
	unsigned char *Buffer_V3_6 = new unsigned char [780]; // FileScanIssue
	rewind(readToBuffer);                  
	fread(Buffer_V3_6,780,1,readToBuffer);
	fclose(readToBuffer);
	
	if (IsJPEG(Buffer_V3_6,BufferLength) || IsExif(Buffer_V3_6,BufferLength))
	{	

		Pjpeg_decoder_file_stream Pinput_stream = new jpeg_decoder_file_stream();
		if (!Pinput_stream->open(fileName))
		{
			Pjpeg_decoder Pd = new jpeg_decoder(Pinput_stream, true);
			if (Pd->get_error_code() == 0)
			{
				if (!Pd->begin())
				{
					uchar *Pbuf = NULL;
					
					if (Pd->get_height()<=50 || Pd->get_width()<=50) 
					{
						delete [] Buffer;
						Pinput_stream->close();
						delete Pd;
						delete Pinput_stream;
						return 1;
					} // nonporn
					
					
					
					
					int halfway = Pd->get_height()/2;
					int linesProcessed = 0;
					
					if (Pd->get_num_components() == 3)
					{
						Pbuf = (uchar *)malloc(Pd->get_width() * 3);
						if (Pbuf)
						{
							for ( ; ; )
							{
								if (linesProcessed++<halfway)
								{
									void *Pscan_line_ofs;
									uint scan_line_len;
									if (Pd->decode(&Pscan_line_ofs, &scan_line_len))
									{
										// should never reach here but might do if non-standard coding - in which case text what we have
										linesProcessed=halfway;
									}
								}
								else 
								{
									
									DWORD *ptr_pixelData = Pd->pixelData;
									int skin = GetJPEGSkinPercentage(ptr_pixelData,Pd->totalPixels);
									free(Pbuf);
									delete [] Buffer;
									Pinput_stream->close();
									delete Pd;
									delete Pinput_stream;
									
									if (skin>JPEG_SKIN_PERCENTAGE_THRESHOLD) 
									{
										if (ShowDetailedImage == TRUE) _tcscat(ImageAnalysisMsg , _T("Enough Skin Value ----> Proceed to Next Filters \n\n"));
										return 0; // continue processing
									}
									else 
									{
										if (ShowDetailedImage == TRUE) _tcscat(ImageAnalysisMsg , _T("Not Enough Skin Value ---- Exit As Non Porn \n\n"));
										return 1; // nonporn
									}
								}
							}
						}
					}
				}
				else
				{
					Pinput_stream->close();
					delete Pd;
					delete Pinput_stream;
					delete [] Buffer;
					return 0; // continue
				}
			}
			else
			{
				Pinput_stream->close();
				delete [] Buffer;
				delete Pd;
				delete Pinput_stream;
				return -1;
			}
		}*/
		return 0;	
	}
	
	if (IsGIF(Buffer_V3_6,BufferLength))
	{
		if (GetDimensionsGIF(Buffer_V3_6,BufferLength,w,h))
		{
			if (w<=50 || h<=50) {
				delete []Buffer_V3_6;
				return 1;
			} // nonporn
		}
		
		DWORD * gifColours_V3_6 = new DWORD[256];  // FileScanIssue
		
		if (GetColourMapGIF(Buffer_V3_6,BufferLength,gifColours_V3_6))
		{
			int numSkinColours = GetNumberGIFSkinColours(gifColours_V3_6);
			if (numSkinColours < SKIN_COLOURS_IN_COLOUR_MAP_THRESHOLD) 
			{	
				delete [] gifColours_V3_6;
				delete [] Buffer_V3_6;
				return 1; // nonporn
			}
			else 
			{
				delete [] gifColours_V3_6;
				delete [] Buffer_V3_6;	
				return 0; // continue processing
			}
		}
		else
		{
			delete [] gifColours_V3_6;
			delete [] Buffer_V3_6;		
			return 0; // most likely low num of colour or animated
		}
	}
	
	
	if (IsBM(Buffer,BufferLength))
	{
		delete [] Buffer_V3_6;
		return 0; // process normally
	}
	if (IsPNG(Buffer,BufferLength)) 
	{
		delete [] Buffer_V3_6;
		return 0; // process normally
	}
	if (IsTIFF1(Buffer,BufferLength)) 
	{
		delete [] Buffer_V3_6;	
		return 0; // process normally
	}
	if (IsTIFF2(Buffer,BufferLength)) 
	{
		delete [] Buffer_V3_6;		
		return 0; // process normally
	}
	if (IsEMF(Buffer,BufferLength)) 
	{
		delete [] Buffer_V3_6;	
		return 0; // process normally
	}

	
	return -1; // not an image
}

//-  Function Name : IsJPEG                
//-  Description   : Determine whethe the image type is JPEG
//-  Parameter     : Buffer - data buffer of the image
//Bufferlength - the length of the data buffer
//-  Return Value  : TRUE if success, FALSE if otherwise
//-------------------------------------------------------------------------*/
bool ImageProcessing::IsJPEG(uchar *a, INT BufferLength)
{
	
	if (BufferLength<10) return false;
	
	// JPEG
	//FF D8 FF E0 xx xx 4A 46
	//49 46 00
	if (a[0]==0xFF && a[1]==0xD8 && a[2]==0xFF && a[3]==0xE0 && a[6]==0x4A && a[7]==0x46 &&
		a[8]==0x49 && a[9]==0x46 && a[10]==0x00)
	{
		//printf("is JPEG\n");
		return TRUE;
	}
	
	// JPEG
	//FF D8 FF [DB|E2|E3|E8|FE]
	//49 46 00
	if (a[0]==0xFF && a[1]==0xD8 && a[2]==0xFF && (a[3]==0xDB || a[3]==0xE2 || a[3]==0xE3 || a[3]==0xE8 || a[3]==0xFE))
	{
		//printf("is JPEG\n");
		return TRUE;
	}
	else return FALSE;
}


bool ImageProcessing::IsExif(uchar *a, INT BufferLength)
{
	
	// EXIF
	//FF D8 FF E1 xx xx 45 78
	//69 66 00
	if (a[0]==0xFF && a[1]==0xD8 && a[2]==0xFF && a[3]==0xE1 && a[6]==0x45 && a[7]==0x78 &&
		a[8]==0x69 && a[9]==0x66 && a[10]==0x00)
	{
		//printf("is EXIF\n");
		return TRUE;
	}
	else return FALSE;
}


//-  Function Name : GetJPEGSkinPercentage
//-  Description   : Determine the skin percentage in JPEG image
//-  Parameter     : pixelData - pointer to chromatic value 
//pixelCount - number of pixels in the image
//-  Return Value  : percentage of skin pixels
//-------------------------------------------------------------------------*/
INT ImageProcessing::GetJPEGSkinPercentage(DWORD *pixelData, INT pixelCount)
{
	//MessageBoxA(NULL,"WQImageProcessing::FastFileBasedDecision // GetJPEGSkinPercentage","Point4_7_2_1",NULL);
	
	if (pixelCount==0) return 0;
	//char* homam = new char [200];
	
	INT numberSkin=0;
	for (INT i=0;i<pixelCount;i++)
	{
		if (RGBIsInCube(*(pixelData+i))) numberSkin++;
	}
	return (numberSkin*100)/pixelCount;
}


//-  Function Name : IsGIF               
//-  Description   : Determine whethe the image type is GIF
//-  Parameter     : Buffer - data buffer of the image
//Bufferlength - the length of the data buffer
//-  Return Value  : TRUE if success, FALSE if fail
//-------------------------------------------------------------------------*/
bool ImageProcessing::IsGIF(uchar *Buffer, INT BufferLength)
{
	if (BufferLength<6) return FALSE;
	// GET the GIF8 bytes at the start 
	if (*Buffer++=='G' && *Buffer++=='I' && *Buffer++=='F' && *Buffer++=='8')
	{
		if (*Buffer=='7' || *Buffer=='9')
		{
			if (*(++Buffer)=='a') return TRUE;
		}
		else return FALSE;
	}
	return FALSE;
}



/*---------------------------------------------------------------------------
 //-  Function Name : IsBM
               
 //-  Description   : Determine whethe the image type is BM
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsBM(uchar *Buffer, INT BufferLength)
{
	if (BufferLength<6) return FALSE;
	// GET the BM bytes at the start 
	if (*Buffer++=='B' && *Buffer++=='M')
	{
		return TRUE;
	}
	return FALSE;
}



/*---------------------------------------------------------------------------
 //-  Function Name : IsPNG               
 //-  Description   : Determine whethe the image type is PNG
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsPNG(uchar *Buffer, INT BufferLength)
{
	if (BufferLength<6) return FALSE;
	// GET the .PNG bytes at the start 
	Buffer++;
	
	if (*Buffer++=='P' && *Buffer++=='N' && *Buffer++=='G')
	{
		return TRUE;
	}
	return FALSE;
}


/*---------------------------------------------------------------------------
 //-  Function Name : IsTIFF1               
 //-  Description   : Determine whethe the image type is TIFF
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsTIFF1(uchar *Buffer, INT BufferLength)
{
	if (BufferLength<6) return FALSE;
	// GET the .PNG bytes at the start 
	if (*Buffer++=='I' && *Buffer++=='I')
	{
		return TRUE;
	}
	return FALSE;
}


/*---------------------------------------------------------------------------
 //-  Function Name : IsTIFF1               
 //-  Description   : Determine whethe the image type is TIFF
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsEMF(uchar *a, INT BufferLength)
{
	
	if (BufferLength<6) return FALSE;
	// GET the .EMF bytes at the start 
	if (a[41]==0x45 && a[42]==0x4D && a[43]==0x46)
	{
		//MessageBox(NULL,"EMF",_T("****************"),NULL);
		return TRUE;
	}
	return FALSE;
	
}

/*---------------------------------------------------------------------------
 //-  Function Name : IsTIFF1               
 //-  Description   : Determine whethe the image type is TIFF
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsTIFF2(uchar *Buffer, INT BufferLength)
{
	if (BufferLength<6) return FALSE;
	// GET the .PNG bytes at the start 
	if (*Buffer++=='M' && *Buffer++=='M')
	{
		return TRUE;
	}
	return FALSE;
}




/*---------------------------------------------------------------------------
 //-  Function Name : GetDimensionsGIF                
 //-  Description   : Get the dimension of GIF image
 //-  Parameter     : Buffer - data buffer of the image
 Bufferlength - the length of the data buffer
 w - width of the image
 h - height of the image
 //-  Return Value  : TRUE if success, FALSE if fail
 //-------------------------------------------------------------------------*/
bool ImageProcessing::GetDimensionsGIF(uchar *Buffer,INT BufferLength,INT &w, INT &h)
{
	if (BufferLength<10) return FALSE;
	// Skip the GIF identifier;
	Buffer+=6;
	// little indian
	w = *Buffer++;
	w = w + 256**Buffer++;
	h = *Buffer++;
	h = h + 256**Buffer++;
	return TRUE;
}


//-  Function Name : GetColourMapGIF               
//-  Description   : Obtain the colour map of GIF image
//-  Parameter     : Buffer - data buffer of the image

//-  Return Value  : TRUE if success, FALSE if fail
//-------------------------------------------------------------------------*/
bool ImageProcessing::GetColourMapGIF(uchar *Buffer,INT BufferLength,DWORD *gifColourData)
{
	if (BufferLength<800) return FALSE;
	// Skip the GIF identifier and dimensions
	Buffer+=10;
	// check the flag to see if global colour map
	if (*Buffer<128) return FALSE;
	Buffer+=3;
	
	for (INT i=0;i<256;i++)
	{
		INT red = *Buffer++;
		INT green = *Buffer++;
		INT blue = *Buffer++;
		*gifColourData++ = (2 ^ 25 + (blue * 2 ^ 16) + (green * 2 ^ 8) + red );
	}
	return TRUE;
}


//-  Function Name : GetNumberGIFSkinColours               
//-  Description   : Determine the number of skin pixel in the GIF image
//-  Parameter     : pixelData - pointer to chromatic value of GIF image
//-  Return Value  : number of skin pixels
//-------------------------------------------------------------------------*/
INT ImageProcessing::GetNumberGIFSkinColours(DWORD *pixelData)
{
	INT numberSkin=0;
	for (INT i=0;i<256;i++)
	{
		if (RGBIsInCube(*(pixelData+i))) numberSkin++;
	}
	return numberSkin;
}

/*---------------------------------------------------------------------------
 //-  Function Name : QuickSkinScan               
 //-  Description   : Obtain the skin percentage for quick skin scan process
 //-  Parameter     : pBitmapInfo - bitmap info
 pdPercentageSkin - percentage skin calculated
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
//bool WQImageProcessing::QuickSkinScan(BITMAP * pBitmapInfo, DOUBLE * pdPercentageSkin, INT iTopX, INT iTopY, INT iWidth, INT iHeight)
bool ImageProcessing::QuickSkinScan(uchar* *pBitmapInfo, DOUBLE * pdPercentageSkin, INT iTopX, INT iTopY, INT iWidth, INT iHeight, INT bpp)
{
	bool bRet = FALSE;
	INT cropSize=0;
	
	if (pBitmapInfo == NULL || pdPercentageSkin == NULL)
	{
		return bRet;
	}
	
	if ((iTopX + iWidth > iWidth) ||
		(iTopY + iHeight > iHeight))
	{
		return bRet;
	}
	
	// Define 32 x 32 poINTs to process in an image
	const INT iWidthDivision = 32;
	const INT iHeightDivision = 32;
	
	INT iCount = 0;
	INT iSkinCount = 0;
	FLOAT fWOffset = (iWidth < iWidthDivision) ? 1 : (FLOAT) iWidth / iWidthDivision;
	FLOAT fHOffset = (iHeight < iHeightDivision) ? 1 : (FLOAT) iHeight / iHeightDivision;
	FLOAT fWidth = (FLOAT) iWidth;
	FLOAT fHeight = (FLOAT) iHeight;
	
//	BYTE * pbImage = reinterpret_cast<BYTE *>(pBitmapInfo->bmBits);
	BYTE * pbImage = reinterpret_cast<BYTE *>(*pBitmapInfo); 
	if (pbImage != NULL)
	{
		INT iWidthBytes = CMBitmapProcessing::CalcWidthBytes(iWidth, bpp);
		//INT iWidthBytes = CMBitmapProcessing::CalcWidthBytes(pBitmapInfo->bmWidth, pBitmapInfo->bmBitsPixel);
		pbImage += (iWidthBytes * (iTopY + (INT) fHOffset)) + (iTopX * (bpp>>3));
		INT iPrev = (INT) fHOffset;
		
		//delclaration and initialization of variable for gamma correction
		INT bright=0;
		INT pixels=0;
		INT gammaSkinCount=0;
		for (FLOAT h=fHOffset; h<fHeight-fHOffset;)
		{
			for (FLOAT w=fWOffset; w<fWidth-fWOffset; w+= fWOffset)
			{
				INT ww = (INT) w;
				INT index=ww+ww+ww;
				DWORD dwData = RGB(pbImage[index], pbImage[index+1], pbImage[index+2]);
				
				// Gamma correction
				INT bw_value = 6*pbImage[index];
				bw_value+=6*pbImage[index+1];
				bw_value+=pbImage[index+2];
				bw_value/=13;
				pixels++;
				
				if (bw_value>175) bright++;
				DWORD dwData2 = RGB(gammaTable[pbImage[index]], gammaTable[pbImage[index+1]], gammaTable[pbImage[index+2]]);
				
				if (RGBIsInCube(dwData2)) gammaSkinCount++;
				
				if (RGBIsInCube(dwData))
				{
					if (ShowDetailedImage == TRUE)
					{
						pbImage[index] =0;
						pbImage[index+1]=0 ;
						pbImage[index+2]=255;
					}
					iSkinCount++;
				}
				iCount++;
			}
			h += fHOffset;
			INT iH = (INT) h;
			INT iHOffset = iH - iPrev;
			iPrev = iH;
			pbImage += iWidthBytes * iHOffset;
			
		}
		
		*pdPercentageSkin = (DOUBLE) iSkinCount / iCount;
			
		if (bright*100>75*pixels) 
		{
			gammaSkinCount = 90;
			*pdPercentageSkin = (DOUBLE) gammaSkinCount / iCount;
		}
		
		bRet = TRUE;
	}
	return bRet;
}

/*---------------------------------------------------------------------------
 //-  Function Name : QuickSkinScan                
 //-  Description   : Obtain the skin percentage for quick skin scan process
 //-  Parameter     : pBitmapInfo - bitmap info
 pdPercentageSkin - percentage skin calculated
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::QuickBlobScan(uchar* *pBitmapInfo, INT * info, INT iTopX, INT iTopY, INT iWidth, INT iHeight, INT bpp)
{
	bool bRet = FALSE;
	INT cropSize=0;
	
	if (pBitmapInfo == NULL)
	{
		return bRet;
	}
	
	if ((iTopX + iWidth > iWidth) ||
		(iTopY + iHeight > iHeight))
	{
		return bRet;
	}
	
	BYTE * pbImage = *pBitmapInfo;
	if (pbImage != NULL)
	{
		INT iWidthBytes = CMBitmapProcessing::CalcWidthBytes(iWidth, bpp);
		
		for (FLOAT h=0; h<iHeight; h++)
		{
			for (FLOAT w=0; w<iWidth; w++)
			{
				INT ww = (INT) w;
				INT index=ww+ww+ww;
				INT index2 = w+(h*iWidth);
				
				if (info[index2] > 0)
				{
					if (info[index2] == 1)
					{
						pbImage[index] =0;
						pbImage[index+1]=0;
						pbImage[index+2]=255;
					}
					
					else if (info[index2] == 2)
					{
						pbImage[index] =0;
						pbImage[index+1]=255;
						pbImage[index+2]=0;
					}
					
					else if (info[index2] == 3)
					{
						pbImage[index] =255;
						pbImage[index+1]=0;
						pbImage[index+2]=0;
					}
					
					else if (info[index2] == 4)
					{
						pbImage[index] =255;
						pbImage[index+1]=255;
						pbImage[index+2]=0;
					}
					else if (info[index2] == 5)
					{
						pbImage[index] =255;
						pbImage[index+1]=255;
						pbImage[index+2]=255;
					}
					else 
					{
						pbImage[index] =0;
						pbImage[index+1]=255;
						pbImage[index+2]=255;
					}
				}
			}
			pbImage += iWidthBytes;
			
		}
		
		bRet = TRUE;
	}
	
	return bRet;
}

/*---------------------------------------------------------------------------
 //-  Function Name : CheckContinueProcess              
 //-  Description   : Determine whether further process need to be continued base on percentage skin and scan type
 //-  Parameter     : pdPercentageSkin - percentage skin calculated
 enScan_Type - scanning type either of normal or quick
 pbContinue - TRUE for continue and FALSE for terminate
 
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
bool ImageProcessing::CheckContinueProcess(DOUBLE dPercentageSkin, 
                                             EN_FSIP_SCAN_TYPE enScan_Type, 
                                             bool * pbContinue)
{
	bool bRet = FALSE;
	
	if (pbContinue == NULL)
	{
		return bRet;
	}
	
	switch (enScan_Type)
	{
		case FSIP_SCAN_NORMAL:
		{
			bRet = TRUE;
			if (dPercentageSkin >= THRESHOLD_QUICKSCAN)
			{
				*pbContinue = TRUE;
			}
			else
			{
				*pbContinue = FALSE;
			}
		}
			break;
			
		case FSIP_SCAN_QUICK:
		{
			*pbContinue = FALSE;
			bRet = TRUE;
		}
			break;
			
		default:
		{
			bRet = FALSE;
		}
			break;
	}
	return bRet;
}



/*---------------------------------------------------------------------------
 //-  Function Name : NewQuickSkinScan                
 //-  Description   : Perform quick skin scan based on scanning type, either normal or quick
 //-  Parameter     : hbmp - hbitmap image
 pResults - structure that contains porn features
 enScan_Type - scanning type either of normal or quick
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/

bool ImageProcessing::NewQuickSkinScan(uchar* hbmp,
									   ImageProcessing::WQ_IP_RESULTS * pResults,
									   EN_FSIP_SCAN_TYPE enScan_Type, int Width , int Height, int bpp)
{
	//initiate a new hbitmap for conversion purpose
	//HBITMAP hbm2 = NULL; 
	uchar* hbm2 = NULL; 
	
	if (ChangeTo24Bit(hbm2,hbmp , bpp))
	{
		//obtain a new bitmap after conversion
		//BITMAP bm;
		//::GetObject(hbm2, sizeof(bm), &bm); 
		
		bool bRet;
		DOUBLE dPercentageQuickSkin;
		// Determine the percentage skin in the quick skin scan process
		//bRet = QuickSkinScan(&bm, &dPercentageQuickSkin, 0, 0, bm.bmWidth, bm.bmHeight);
		bRet = QuickSkinScan(&hbm2, &dPercentageQuickSkin, 0, 0, Width, Height, bpp);

		
		if ((ShowDetailedImage == TRUE) && (THRESHOLD_QUICKSCAN==0.10))
		{
			IplImage *test;
			//LoadHBITMAP(hbm2 , &test);
			LoadHBITMAP(hbm2 , &test, Width, Height, bpp);
			char PreWinName[56];
			sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			
			IplImage *destination = NULL;
			float fResH;
			float fResW;
			INT iHeight;
			INT iWidth  ;
			
			if (test->width > 500 || test->height >500)
			{
				
				iHeight    = test->height;
				iWidth     = test->width;
				if (iHeight > iWidth)
				{
					if (iHeight >= 500)
					{
						fResH = 500;
						fResW = iWidth * (float)(fResH / iHeight);
					}
				}
				if (iWidth > iHeight)
				{
					if (iWidth >= 500)
					{
						fResW = 500;
						fResH = iHeight * (float)(fResW / iWidth);
					}
				}
				
				destination = cvCreateImage( cvSize((int)(fResW) , (int)(fResH )),
											test->depth, test->nChannels );
				cvResize(test, destination);
			
			}
			else 
			{
		//		cvShowImage(PreWinName , test);
				INT Wloc = 20 + test->width;
				
				
				cvMoveWindow(PreWinName, Wloc , 10);
			}
			//cvWaitKey(0);
			cvReleaseImage(&test);
			cvReleaseImage(&destination);
		}
		//DeleteObject(hbm2);
		delete[] hbm2;
		hbm2 = NULL;
		if (bRet == TRUE)
		{
			bool bContinue;
			// Check whether to continue or not base on scan type and percentage skin
		//	bRet = CheckContinueProcess(dPercentageQuickSkin, enScan_Type, &bContinue);			
			bContinue = CheckContinueProcess(dPercentageQuickSkin, enScan_Type, &bContinue);	//DVM ***bug corrected
			if (bContinue == FALSE)
			{
				pResults->pf=0;
				pResults->kof=100;
				pResults->kosf=105;
				return true;
			}
		}
	}
	return false;
}



/*---------------------------------------------------------------------------
 //-  Function Name : ProportionScaling               
 //-  Description   : Proportionally scale the image
 //-  Parameter     : hbm2 - hbitmap image after scaling
 hbmp - hbitmap image before scaling
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ProportionScaling(uchar* &hbm2,uchar* hbmp, int width , int height , int depth , int channels , int bpp ,int &nWidth ,int &nHeight , int &nDepth , int &nChannel,int type, int &nType)
{
	if(width <= 300 && height <= 300) //temporarily allow scaling
	{
		if (bpp == 32)
		{
			if (CMBitmapProcessing::Convert32to24bit(&hbm2 , hbmp) == FALSE)
			{
				// failed to convert
				return FALSE;
			}	
		}
		// Make another copy of 24 bits hbitmap
		else if (bpp == 24)
		{
			// no need to convert
		//	if (CMBitmapProcessing::CloneBitmap(&hbm2, hbmp) == TRUE)
			if (CMBitmapProcessing::CloneBitmapFull(&hbm2, hbmp, width, height, depth, channels ,nWidth ,nHeight , nDepth ,nChannel) == TRUE)	//JH-optimize
			{
				// successfully cloned
			}
			else
			{
				// failed to clone
				return FALSE;
			}
		}
		// Converting lesser than 24 bits hbitmap to 24 bits hbitmap
		
		else
		{
			// failed to convert
			return FALSE;
		}
		
	}
	else
	{
		DOUBLE ratio=0.0f;
		INT scalewidth=0;
		INT scaleheight=0;
		ratio=(DOUBLE)height/(DOUBLE)width;
		if(ratio>1)
		{
			scaleheight=300;
			scalewidth=INT(300/ratio);
		}
		else
		{
			scalewidth=300;
			scaleheight=INT(300*ratio);
		}
		
	//	IplImage *img1 = cvCreateImage(cvSize(width, height),depth,channels);
        cv::Mat img1 =  cv::Mat(height,width,type );//emadtesthere
        int size_ = width*height;
	//	memcpy(img1->imageData ,hbmp, img1->imageSize);
        memcpy(img1.data ,hbmp, size_);
		//cvSaveImage("/homam/testhere.jpg", img1, 0);
		
		//IplImage* ScaledImg;
		//ScaledImg = cvCreateImage(cvSize(scalewidth,scaleheight),img1->depth,img1->nChannels);
        cv::Mat ScaledImg =  cv::Mat(scaleheight,scalewidth,img1.type() );
        
        cv::Size _sz = cv::Size(scalewidth,scaleheight);
		
		//cvResize(img1,ScaledImg, CV_INTER_NN);
        cv::resize(img1,ScaledImg,_sz,cv::INTER_NEAREST);//_sz);// ScaledImg.size());//emadtesthere
		//cvSaveImage("/homam/scaledtesthere.jpg", ScaledImg, 0);
		
		
		
        //updates for opencv4
        int ScaledImgSize =ScaledImg.total() * ScaledImg.elemSize();//ScaledImg.rows*ScaledImg.cols;
     //   ScaledImgSize =ScaledImg.rows*ScaledImg.cols;
        hbm2 = new uchar[ScaledImgSize];
        memcpy(hbm2, ScaledImg.data, ScaledImgSize);
        nWidth = ScaledImg.cols;
        nHeight = ScaledImg.rows;
        nDepth = ScaledImg.depth();
        nChannel = ScaledImg.channels();
        nType = ScaledImg.type();
        //added 
        if(nDepth == 0 || nDepth == 1){
            nDepth =8;
        }else if(nDepth == 2 ||nDepth == 3 ){
            nDepth =16;
        }
        else if(nDepth == 4 ||nDepth == 5 ){
            nDepth =32;
            
        }
        else if(nDepth == 6 ){
            nDepth =64;
        }

		
		
				
		
		if ((ScaledImg.cols = 0))//(ScaledImg->width = 0)
		{
	
			return false;
		}
		
		if ((ScaledImg.rows = 0))//(ScaledImg->height = 0)
		{
		
			return false;
		}
		
		
		
	}
	/////////////////////////////////////////////////////////////////////////////
	
	return TRUE;
}

/*---------------------------------------------------------------------------
 //-  Function Name : HomogeneousProperty              
 //-  Description   : Calculate the homogenity of the image
 //-  Parameter     : image2 - byte information of the image
 height - height of the image2
 width - width of the image2
 threshold - fraction of the difference in BLUE and GREEN components
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::HomogeneousProperty(BYTE *image2, INT height, INT width, FLOAT threshold)
{
	if(image2)
	{
	
		INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
	
		
		////////////////////////////////////////////////////////////////////////////////////
		//flexible number poINTs tested for color depend to the size of the image
		INT validno_width=0;
		INT validno_height=0;
		if (width<=150) 
			validno_width=10;
		else
			validno_width=20;
		
		if (height<=150)
			validno_height=10;
		else
			validno_height=20;
		////////////////////////////////////////////////////////////////////////////////////
		INT i,j,m,n;
		INT avrgValueB[21][21];
		INT avrgValueG[21][21];
		INT avrgValueR[21][21];
		
		////////////////////////////////////////////////////////////////////////////////////
		//Take n locations based on the validno_width and validno_height
		INT xLocation[22];
		INT yLocation[22];
		xLocation[0]=0;yLocation[0]=0;
		xLocation[validno_width+1]=width-1; yLocation[validno_height+1]=height-1;
		
		for (i=0;i<validno_width;i++)
		{
			xLocation[i+1] = (width*(i+1))/(validno_width+1);
		}
		
		for (j=0;j<validno_height;j++)
		{
			yLocation[j+1] = (height*(j+1))/(validno_height+1);
		}
		////////////////////////////////////////////////////////////////////////////////////
		
		for(i=0;i<validno_width+1;i++)
		{
			for(j=0;j<validno_height+1;j++)
			{
				//Calculate the average value
				INT totalB=0; INT totalG=0; INT totalR=0;
				INT width_window=xLocation[i+1]-xLocation[i];
				INT height_window=yLocation[j+1]-yLocation[j];
				for(m=0;m<width_window;m++)
					for(n=0;n<height_window;n++)
					{
						//Blue component
						INT pixel=*(image2+(yLocation[j]+n)*widthbytes+(xLocation[i]+m)*3);
						totalB=totalB+pixel;
						//Green component
						pixel=*(image2+(yLocation[j]+n)*widthbytes+(xLocation[i]+m)*3+1);
						totalG=totalG+pixel;
						//Red component
						pixel=*(image2+(yLocation[j]+n)*widthbytes+(xLocation[i]+m)*3+2);
						totalR=totalR+pixel;
					}
				//Obtain the average value for each colour component
				avrgValueB[i][j]=INT(totalB/((width_window+1)*(height_window+1)));
				avrgValueG[i][j]=INT(totalG/((width_window+1)*(height_window+1)));
				avrgValueR[i][j]=INT(totalR/((width_window+1)*(height_window+1)));
			}
		}
		
		//For each colour component, calculate the accumulate difference based on four neighborhood sector
		INT TotalDiff_Blue=0; INT TotalDiff_Green=0; INT TotalDiff_Red=0;
		for(i=1;i<validno_width;i++)
		{
			for(j=1;j<validno_height;j++)
			{
				//blue component using laplace operator
				INT DiffBValue=0;
				DiffBValue=abs(avrgValueB[i][j-1]+avrgValueB[i-1][j]+avrgValueB[i+1][j]+avrgValueB[i][j+1]-(4*avrgValueB[i][j]));
				TotalDiff_Blue=TotalDiff_Blue+DiffBValue;
				//green component using laplace operator
				INT DiffGValue=0;
				DiffGValue=abs(avrgValueG[i][j-1]+avrgValueG[i-1][j]+avrgValueG[i+1][j]+avrgValueG[i][j+1]-(4*avrgValueG[i][j]));
				TotalDiff_Green=TotalDiff_Green+DiffGValue;
				//red component using laplace operator
				INT DiffRValue=0;
				DiffRValue=abs(avrgValueR[i][j-1]+avrgValueR[i-1][j]+avrgValueR[i+1][j]+avrgValueR[i][j+1]-(4*avrgValueR[i][j]));
				TotalDiff_Red=TotalDiff_Red+DiffRValue;
			}
		}
		
		
		//Calculate the ratio difference (invariant to number points considered in calculation)
		INT consider_points = ((validno_width+1)*(validno_height+1))-(2*validno_width+2*validno_height);
		
		TotalDiff_Blue=TotalDiff_Blue/consider_points;
		TotalDiff_Green=TotalDiff_Green/consider_points;
		TotalDiff_Red=TotalDiff_Red/consider_points;
		
		
		//Determine whether this image should be rejected or continue for further porn processing
		INT AvrgBG=threshold*((TotalDiff_Blue+TotalDiff_Green)/2);
		
		if (TotalDiff_Red<=AvrgBG)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
		return TRUE; //if image is null
}


/*---------------------------------------------------------------------------
 //-  Function Name : FastFilterProcess              
 //-  Description   : Fast filtering base on number of colour, type of dominant colour and homogeneous property
 //-  Parameter     : hbm2 - hbitmap image
 bm - bitmap information
 enScan_Type - scanning type either of normal or quick
 filterresult - TRUE if fast filter is implemented, FALSE if otherwise
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::FastFilterProcess(uchar* hbm2,
                                          INT Width,
										  INT Height,
										  INT Depth,
										  INT Channels,	
                                          BYTE *image2,
                                          EN_FSIP_SCAN_TYPE enScan_Type,
                                          bool &filterresult,
                                          ImageProcessing::WQ_IP_RESULTS * pResults)
{
	//initialization
	INT width2 = Width;
	INT height2 = Height;
	INT widthbytes2 = (width2 * 3 % 4) ? ((((width2 * 3) >> 2) + 1) << 2) : width2 * 3;	
	const size_t ST_COLOUR_RANGE = 64;
	DWORD dwColourTable[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	DWORD x, y;
	DWORD ColourCount = 0;
	BYTE* pCurrPixel = image2;
	INT bpp = Depth * Channels;
	
	//---------------------------------------------------------------------------
	//- Determine the number of colour
	//---------------------------------------------------------------------------
	for (y=0; y<height2; y++)
	{
		for (x=0; x<width2; x++)
		{
			INT xOffset = x+x+x;
			
			DWORD dwRed = pCurrPixel[xOffset+2] >> 2;
			DWORD dwGreen = pCurrPixel[xOffset+1] >> 2;
			DWORD dwBlue = pCurrPixel[xOffset] >> 2;
			
			if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
			{
				ColourCount++;
			}
			dwColourTable[dwRed][dwGreen][dwBlue]++;
		} //end for
		pCurrPixel += widthbytes2;
	} //end for
	//---------------------------------------------------------------------------
	//- End of Determine the number of colour
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Determine the dominant colour and its relations to skin properties
	//---------------------------------------------------------------------------
	INT max1=0;
	INT max2=0;
	INT max3=0;
	DWORD maxColour1=0;
	DWORD maxColour2=0;
	DWORD maxColour3=0;
	
	INT total1=0;
	// find most popular colour...
	for (INT xi=0;xi<64;xi++)
		for (INT xj=0;xj<64;xj++)
			for (INT xk=0;xk<64;xk++)
			{
				if (max1<dwColourTable[xi][xj][xk])
				{
					max3=max2;
					max2=max1;
					max1=dwColourTable[xi][xj][xk];
					maxColour3=maxColour2;
					maxColour2=maxColour1;
					maxColour1=RGB(xk*4,xj*4,xi*4);
				}
				else if (max2<dwColourTable[xi][xj][xk])
				{
					max3=max2;
					max2=dwColourTable[xi][xj][xk];
					maxColour3=maxColour2;
					maxColour2=RGB(xk*4,xj*4,xi*4);
				}
				else if (max2<dwColourTable[xi][xj][xk])
				{
					max3=dwColourTable[xi][xj][xk];
					maxColour3=RGB(xk*4,xj*4,xi*4);
				}
				total1+=dwColourTable[xi][xj][xk];
			}
	
	bool maxColour1skin = FALSE;
	for (INT xi=0;xi<4;xi++)
		for (INT xj=0;xj<4;xj++)
			for (INT xk=0;xk<4;xk++)
				if(RGBIsInCube(maxColour1+xk+xj*256+xi*256*256)) {maxColour1skin=TRUE; xi=xj=xk=4;}
	
	bool maxColour2skin = FALSE;
	for (INT xi=0;xi<4;xi++)
		for (INT xj=0;xj<4;xj++)
			for (INT xk=0;xk<4;xk++)
				if(RGBIsInCube(maxColour2+xk+xj*256+xi*256*256)) {maxColour2skin=TRUE; xi=xj=xk=4;}
	
	bool maxColour3skin = FALSE;
	for (INT xi=0;xi<4;xi++)
		for (INT xj=0;xj<4;xj++)
			for (INT xk=0;xk<4;xk++)
				if(RGBIsInCube(maxColour3+xk+xj*256+xi*256*256)) {maxColour3skin=TRUE; xi=xj=xk=4;}
	if ((maxColour1skin && max1*5>total1) || 
		(maxColour2skin && max2*5>total1) || 
		(maxColour3skin && max3*5>total1))
	{	
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kosf=102;
		filterresult=TRUE;
		return TRUE;
	}
	//---------------------------------------------------------------------------
	//- End of Determine the dominant colour and its relations to skin properties
	//---------------------------------------------------------------------------
	
	
	//---------------------------------------------------------------------------
	//- Determine the homogenous property
	//---------------------------------------------------------------------------
	// Analyse on the number of colour
	if (ColourCount<=50) //(previously is 20) this value is chosen as a tradeoff value between gif image and subimages captured from capture window process
	{
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kosf=103;
		filterresult=TRUE;
		return TRUE;
	}
	else
	{
		if (bpp == 24)
		{
			FLOAT threshold;
			if (ColourCount <=200)
				threshold = 0.7f;
			else if(((FLOAT)height2/width2>3 && width2<100) || ((FLOAT)width2/height2>3 && height2<100))
				threshold = 0.7f;
			else
				threshold = 0.4f; //0.4
						
			//Check the homogenous property of blue,green and red components
			bool HomoFlag=TRUE;
			HomoFlag=HomogeneousProperty(image2, height2, width2, threshold);
			
			if (HomoFlag==1)
			{
				pResults->NetPornValue = 0.00f;
				pResults->pf=0;
				pResults->kosf=104;
				filterresult=TRUE;
				return TRUE;
			}
		}
	}
	/////////////////////////////////////////////////////////////////////////////
	filterresult=FALSE; //indicate that fast filter process unable to filter nonporn image
	return TRUE;
}

/*---------------------------------------------------------------------------
 //-  Function Name : rgbComputeOptimised               
 //-  Description   : Extract red,green and blue component from the image
 //-  Parameter     : image - image data in bytes
 red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 height - height of the image
 width - width of the image
 widthbytes - number of bytes in width
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::rgbComputeOptimised(BYTE *image, unsigned char *red, unsigned char *green, unsigned char *blue,INT height,INT width,INT widthbytes)
{
	BYTE *index;
	
	for (INT h=0;h<height;h++)
	{			
		index = image;
		for (INT w=0;w<width;w++)
		{		
			*blue++=*index++;
			*green++=*index++;
			*red++=*index++;
		}
		image+=widthbytes;
	}
}


/*---------------------------------------------------------------------------
 //-  Function Name : GammaCorrectionOptimised               
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::GammaCorrectionOptimised(unsigned char *red,unsigned char *green,unsigned char *blue,unsigned char *bw,INT height,INT width)
{	
	//	char homam[200];
	INT hw=height*width;
	INT h;
	INT bw_tmp;
	INT bright=0;
	unsigned char *bw_ptr=bw;
	unsigned char *red_ptr=red;
	unsigned char *green_ptr=green;
	unsigned char *blue_ptr=blue;
	for(h=0;h<hw;h++)
	{
		bw_tmp = *(blue_ptr++)*6;
		bw_tmp += *(green_ptr++)*6;
		bw_tmp += *(red_ptr++);
		*bw_ptr = bw_tmp/13;
		if (*bw_ptr++>175) bright++;
	}

	if (bright*100 < hw*75) return; // no need for gamma correction
	
	
	//---------------------------------------------------------------------------
	//- Obtain the red,green and blue base on gamma table
	//---------------------------------------------------------------------------
	bw_ptr=bw;
	red_ptr=red;
	green_ptr=green;
	blue_ptr=blue;
	for(h=0;h<hw;h++)
	{
		*blue_ptr=gammaTable[*blue_ptr];
		*green_ptr=gammaTable[*green_ptr];
		*red_ptr=gammaTable[*red_ptr];
		bw_tmp = *blue_ptr*6;
		bw_tmp += *green_ptr*6;
		bw_tmp += *red_ptr;
		
		*bw_ptr++=bw_tmp/13;
		blue_ptr++;
		green_ptr++;
		red_ptr++;
	}
	//---------------------------------------------------------------------------
	//- End of obtain the red,green and blue base on gamma table
	//---------------------------------------------------------------------------
}


/*---------------------------------------------------------------------------
 //-  Function Name : GetSkinPixelOptimised           
 //-  Description   : Determine the skin pixel base on the lookup table
 //-  Parameter     : info - skin data
 bw - grayscale component of the colour image
 red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 width - width of the image
 height - height of the image
 widthbytes - number of bytes in width
 //-  Return Value  : number of skin pixel
 //-------------------------------------------------------------------------*/
INT ImageProcessing::GetSkinPixelOptimised(INT *info,unsigned char *bw,unsigned char *red,unsigned char *green,unsigned char *blue,INT width,INT height,INT widthbytes)
{
	DWORD dwData;
	INT hw=height*width;
	INT h;
	//	INT bw_tmp;
	INT skinpixels=0;
	
	for(h=0;h<hw;h++)
	{
		dwData=RGB(*blue++,*green++,*red++);
		if(RGBIsInCube(dwData))
		{
			*info=3;
			skinpixels++;
		}
		info++;
	}
	return skinpixels;
}



// Skin shade analysis function to remove outlying regions of skin 

/*---------------------------------------------------------------------------
 //-  Function Name : SkinShadeAnalysisOptimised               
 //-  Description   : Skin shade analysis function to remove outlying regions of skin
 //-  Parameter     : info - skin data
 red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 width - width of the image
 height - height of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::SkinShadeAnalysisOptimised(INT *info, unsigned char *red, unsigned char *green, unsigned char *blue, INT width, INT height)
{
	INT simonColourSpace[100][100]; //probability map of the RG value of the skin pixels
	INT simonColourSpaceGroup[100][100]; //group index of each of RG value, iniatiate by the maximum RG value
	INT simonColourSpaceGroupScore[1000]; //distance value from the center of the image for each of the group index
	INT simonColourSpaceGroupNumber[1000]; //the number of skin pixel that having that group index
	INT simonColourSpaceGroupAverage[1000]; //average distance value from the center of the image for each of the group index
	INT total2 =0 ;  
	
	//INTialization value on the RG analysis variables
	INT *intPointer=0;
	intPointer = &simonColourSpace[0][0];
	for (INT s1=0;s1<10000;s1++) *intPointer++=0;
	
	intPointer = &simonColourSpaceGroup[0][0];
	for (INT s1=0;s1<10000;s1++) *intPointer++=-1;
	
	intPointer = &simonColourSpaceGroupScore[0];
	for (INT s1=0;s1<1000;s1++) *intPointer++=0;
	
	intPointer = &simonColourSpaceGroupNumber[0];
	for (INT s1=0;s1<1000;s1++) *intPointer++=0;
	
	intPointer = &simonColourSpaceGroupAverage[0];
	for (INT s1=0;s1<1000;s1++) *intPointer++=0;
	
	INT *infoPointer; infoPointer = info;
	unsigned char  *redPointer; redPointer= red;
	unsigned char  *greenPointer; greenPointer= green;
	unsigned char  *bluePointer; bluePointer= blue;
	INT hw = height*width;
	
	//Generate the probability map for each of the RG colour value
	//for each of the skin pixels detected from the lookup table
	INT rPos=0; INT gPos=0;
	for (INT h=0;h<hw;h++)
	{			
		if(*infoPointer==3)
		{
			total2= (*redPointer) + (*greenPointer) + (*bluePointer) + 1;
			if(total2!=0)
			{
				rPos = ((*redPointer) *100)/total2;
				gPos = ((*greenPointer) *100)/total2;
			}
			else
			{
				rPos=0;
				gPos=0;
			}
			simonColourSpace[rPos][gPos]++;
			
			
		}
		infoPointer++;
		redPointer++;
		greenPointer++;
		bluePointer++;
	}
	
	
	// get the coordinates in the RG space that have high probability value comparing the eigth connectivity pixels
	// the coordinate is keep in vector MaxColours
	INT thisValue=0;
	INT *otherValuePointer=0;
	MaxColours.erase(MaxColours.begin(),MaxColours.end());
	for (INT s1=1;s1<99;s1++)
	{
		for (INT s2=1;s2<99;s2++) 
		{
			if (simonColourSpace[s1][s2]>50)
			{
				thisValue=simonColourSpace[s1][s2];
				otherValuePointer = &simonColourSpace[s1-1][s2-1]; //top left position
				if (thisValue>*otherValuePointer++ && thisValue>*otherValuePointer++ && thisValue>*otherValuePointer)
				{
					otherValuePointer = &simonColourSpace[s1][s2-1]; //left position
					if (thisValue>*otherValuePointer++ && thisValue>*(++otherValuePointer))
					{
						otherValuePointer = &simonColourSpace[s1+1][s2-1]; //bottom left position
						if (thisValue>*otherValuePointer++ && thisValue>*otherValuePointer++ && thisValue>*otherValuePointer)
						{
							
							
							
							point thisPoint;
							thisPoint.w = s1;
							thisPoint.h = s2;
							MaxColours.push_back(thisPoint);
						}
					}
				}
			}
		}
	}
	
	MaxColourTempArray.erase(MaxColourTempArray.begin(),MaxColourTempArray.end());		
	for (INT ss=0;ss<MaxColours.size();ss++)
	{
		INT thisW = MaxColours.at(ss).w;
		INT thisH = MaxColours.at(ss).h;
		
		
		
		// Name the RG map with the respective group index whereby
		// the maximum RG value is name after the MaxColour vector index
		simonColourSpaceGroup[thisW][thisH]=ss; //position of maximum RG value 
		simonColourSpaceGroup[thisW-1][thisH]=ss; //top position in 4 connectivity
		simonColourSpaceGroup[thisW+1][thisH]=ss; //bottom position in 4 connectivity
		simonColourSpaceGroup[thisW][thisH-1]=ss; //left position in 4 connectivity	
		simonColourSpaceGroup[thisW][thisH+1]=ss; //right position in 4 connectivity
		
		// Determine whether the 4 connectivity neighbor in RG space is one of the RG value of skin pixel or not
		point thisPoint;
		if (simonColourSpace[thisW-1][thisH]>0)
		{
			thisPoint.w = thisW-1;
			thisPoint.h = thisH;
			MaxColourTempArray.push_back(thisPoint);
		}
		if (simonColourSpace[thisW+1][thisH]>0)
		{
			thisPoint.w = thisW+1;
			thisPoint.h = thisH;
			MaxColourTempArray.push_back(thisPoint);
		}
		if (simonColourSpace[thisW][thisH-1]>0)
		{
			thisPoint.w = thisW;
			thisPoint.h = thisH-1;
			MaxColourTempArray.push_back(thisPoint);
		}
		if (simonColourSpace[thisW][thisH+1]>0)
		{
			thisPoint.w = thisW;
			thisPoint.h = thisH+1;
			MaxColourTempArray.push_back(thisPoint);
		}
		
		//Continue connecting all the relevant RG values by reference to group index in the map to the maximum RG value point
		//with the condition that it must one of the RG values from the skin pixel and 
		//the RG value is lesser than the parent RG value
		while (MaxColourTempArray.size()>0)
		{
			thisW = MaxColourTempArray.at(0).w;
			thisH = MaxColourTempArray.at(0).h;
			
			/*sprintf(homam , "thisW=%d , thisH=%d , ss=%d" , thisW, thisH , simonColourSpaceGroup[thisW][thisH]);
			 MessageBox ( NULL, homam , "MaxColourTempArray" , NULL);*/
			
			MaxColourTempArray.erase(MaxColourTempArray.begin());
			
			if (simonColourSpace[thisW-1][thisH]>0 &&
				simonColourSpaceGroup[thisW-1][thisH]!=ss &&
				simonColourSpace[thisW-1][thisH]<=simonColourSpace[thisW][thisH])
			{
				simonColourSpaceGroup[thisW-1][thisH]=ss;
				point thisPoint;
				thisPoint.w = thisW-1;
				thisPoint.h = thisH;
				MaxColourTempArray.push_back(thisPoint);
			}
			
			if (simonColourSpace[thisW+1][thisH]>0 &&
				simonColourSpaceGroup[thisW+1][thisH]!=ss &&
				simonColourSpace[thisW+1][thisH]<=simonColourSpace[thisW][thisH])
			{
				simonColourSpaceGroup[thisW+1][thisH]=ss;
				point thisPoint;
				thisPoint.w = thisW+1;
				thisPoint.h = thisH;
				MaxColourTempArray.push_back(thisPoint);
			}
			
			if (simonColourSpace[thisW][thisH-1]>0 &&
				simonColourSpaceGroup[thisW][thisH-1]!=ss &&
				simonColourSpace[thisW][thisH-1]<=simonColourSpace[thisW][thisH])
			{
				simonColourSpaceGroup[thisW][thisH-1]=ss;
				point thisPoint;
				thisPoint.w = thisW;
				thisPoint.h = thisH-1;
				MaxColourTempArray.push_back(thisPoint);
			}
			
			if (simonColourSpace[thisW][thisH+1]>0 &&
				simonColourSpaceGroup[thisW][thisH+1]!=ss &&
				simonColourSpace[thisW][thisH+1]<=simonColourSpace[thisW][thisH])
			{
				simonColourSpaceGroup[thisW][thisH+1]=ss;
				point thisPoint;
				thisPoint.w = thisW;
				thisPoint.h = thisH+1;
				MaxColourTempArray.push_back(thisPoint);
			}
		}
	}
	
	infoPointer = info;
	redPointer= red;
	greenPointer= green;
	bluePointer= blue;
	INT height_2 = height/2;
	INT width_2 = width/2;
	INT total3= 0;
	
	// Determine the score and number of each RG group index
	// for each detected skin pixel from the lookup table
	for (INT h=0;h<height;h++)
	{
		for (INT w=0;w<width;w++)
		{	
			if(*infoPointer==3)
			{
				total3= (*redPointer) + (*greenPointer) + (*bluePointer) + 1;
				if(total3!=0)
				{
					rPos = ((*redPointer) *100)/total3;
					gPos = ((*greenPointer) *100)/total3;
				}
				else
				{
					rPos=0;
					gPos=0;
				}
				INT group = simonColourSpaceGroup[rPos][gPos];
				if (group>=0 && group<1000)
				{
					//determine the distance according to chessboard distance measure
					INT bandH = 10 - (abs(h-(height_2))*10)/(height_2);
					INT bandW = 10 - (abs(w-(width_2))*10)/(width_2);
					INT band = bandH;
					if (band>bandW) band=bandW; //take the smaller value (based on chessboard measure)
					
					
					simonColourSpaceGroupScore[group]+=band;
					simonColourSpaceGroupNumber[group]++;
				}
			}
			infoPointer++;
			redPointer++;
			greenPointer++;
			bluePointer++;
		}
	}
	
	
	//Determine the average score for each of the group index
	INT numberOfGroups=MaxColours.size();
	if (numberOfGroups>1000) numberOfGroups=1000; 
	
	for (INT ss=0;ss<numberOfGroups;ss++)
	{
		if (simonColourSpaceGroupNumber[ss]>0)
		{
			INT average = (100*simonColourSpaceGroupScore[ss])/simonColourSpaceGroupNumber[ss];
			simonColourSpaceGroupAverage[ss]=average;
			
		}
		else
		{
			simonColourSpaceGroupAverage[ss]=0;
		}	
	}
	
	infoPointer = info;
	redPointer= red;
	greenPointer= green;
	bluePointer= blue;
	INT group = 0;
	INT total4=0;
	
	for (INT h=0;h<hw;h++)
	{			
		if(*infoPointer==3)
		{	
			total4 = (*redPointer) + (*greenPointer) + (*bluePointer) + 1;
			if(total4>0)
			{
				rPos = ((*redPointer) *100)/total4;
				gPos = ((*greenPointer) *100)/total4;
			}
			else
			{
				rPos=0;
				gPos=0;
			}
			
			group = simonColourSpaceGroup[rPos][gPos];
			
			if (group>=0 && group<1000)
			{
				if (simonColourSpaceGroupAverage[group]<360) *infoPointer=0;
				
			}
		}
		infoPointer++;
		redPointer++;
		greenPointer++;
		bluePointer++;
	}
}


/*---------------------------------------------------------------------------
 //-  Function Name : AnalyseTextureOptimised
             
 //-  Description   : Determine skin pixel base on texture value, computed using co-occurence method
 //-  Parameter     : bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 info - skin data
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::AnalyseTextureOptimised(unsigned char *bw, INT height, INT width, INT *info)
{
	INT nn[8][8];
	INT hh,ww,h,w,val,val1,val2,val3,val4, htimes4,wtimes4;
	INT hw=height*width;
	INT height_d4=height/4-1;
	INT width_d4=width/4-1;
	
	unsigned char *bw2 = new unsigned char[hw];
	unsigned char *bw_ptr = bw2;
	//	char homam[100];
	for (h=0;h<hw;h++) 
	{
		*bw_ptr++ = *bw++ >> 5;
	}
	
	htimes4 = 0;
	for (h=0;h<height_d4;h++, htimes4+=4)
	{
		wtimes4 = 0;
		for (w=0;w<width_d4;w++, wtimes4+=4)
		{
			bool isSkin = FALSE;
			for (hh=2;hh<6;hh++)
			{
				for (ww=2;ww<6;ww++)
				{
					val = ((htimes4+hh)*width) + wtimes4 + ww;
					if (info[val]>0 ) isSkin=TRUE;
				}
			}
			
			if (isSkin)
			{
				// set the nn matrix to 0
				for (hh=0;hh<8;hh++)
				{
					for (ww=0;ww<=hh;ww++)
					{
						nn[hh][ww]=0;
					}
				}
				
				
				// fill in the nn matrix for diagonal(upper_right/lowerleft) pixels
				for (hh=1;hh<8;hh++)
				{
					for (ww=1;ww<8;ww++)
					{
						val = ((htimes4+hh)*width) + wtimes4 + ww;
						val1 = *(bw2+val);
						val2 = *(bw2+val-1);
						val3 = *(bw2+val-width);
						val4 = *((bw2+val-width)-1); 
						
						if (val1>val2) nn[val1][val2]++;
						else nn[val2][val1]++;
						if (val1>val3) nn[val1][val3]++;
						else nn[val3][val1]++;
						if (val2>val3) nn[val2][val3]++;
						else nn[val2][val1]++;
						if (val1>val4) nn[val1][val4]++;
						else nn[val4][val1]++;      
					}
				}
				
				INT interia = 0;
				for (hh=0;hh<8;hh++)
				{
					for (ww=0;ww<hh;ww++)
					{
						//sprintf(homam , "nn[%d][%d]= %d" , hh , ww , nn[hh][ww]);
						//MessageBox(NULL , homam , "NN Values" , NULL);
						interia +=nn[hh][ww]*(hh-ww)*(hh-ww);
					}
				} 
				
				if (interia>80)
				{
					// set int_# to be a reasonable value for the middle 4x4 pixels...
					for (hh=2;hh<6;hh++)
					{
						for (ww=2;ww<6;ww++)
						{
							val = ((htimes4+hh)*width) + wtimes4 + ww;
							info[val] = 0;
						}
					}
				}
			}
		}
	}
	delete [] bw2;
}




/*---------------------------------------------------------------------------
 //-  Function Name : ObtainSkinInfoOptimised
        
 //-  Description   : Determine the skin pixels and improve it by using shade and texture analysis
 //-  Parameter     : info - skin data
 bw - grayscale component of the colour image
 red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 width - width of the image
 height - height of the image
 widthbytes - number of bytes in width
 //-  Return Value  : number of skin pixel
 //-------------------------------------------------------------------------*/
INT ImageProcessing::ObtainSkinInfoOptimised(INT *info, unsigned char *bw, unsigned char *red, unsigned char *green, unsigned char *blue,INT width,INT height,INT widthbytes)
{
	INT i=0;
	INT skinpixels=0;
	
	
	//---------------------------------------------------------------------------
	//- Skin pixel determination base on lookup table
	//---------------------------------------------------------------------------
	skinpixels=GetSkinPixelOptimised(info,bw,red,green,blue,width,height,widthbytes);
	//---------------------------------------------------------------------------
	//- End of Skin pixel determination base on lookup table
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Remove skin shades on the side of the image
	//---------------------------------------------------------------------------
	SkinShadeAnalysisOptimised(info, red, green, blue, width, height);
	//---------------------------------------------------------------------------
	//- End of Remove skin shades on the side of the image
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Skin analysis base on texture information
	//---------------------------------------------------------------------------
	AnalyseTextureOptimised(bw,height,width,info);
	//---------------------------------------------------------------------------
	//- End of Skin analysis base on texture information
	//---------------------------------------------------------------------------
	
	// Calculate the number of skin
	skinpixels=0;
	INT * infoPtr = info;
	for(i=0;i<hw;i++)
	{
		if (*infoPtr++==3) skinpixels++;
	}
	
	///////////////////////////////////////////////////////////////////////////
	return skinpixels;
}


/*---------------------------------------------------------------------------
 //-  Function Name : FollowRidge               
 //-  Description   : Analyse on the edge to obtain ridge
 //-  Parameter     : edge - data array that contain edge information
 h - y coordinate of the point
 w - x coordinate of the point
 width - width of the image
 height - height of the image
 length - length of the ridge
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::FollowRidge(INT *edge, INT h, INT w, INT width, INT height, INT &length)
{
	INT val; 
	
	point thisPoint;
	std::vector <struct point> store ;
	thisPoint.w = w;
	thisPoint.h = h;
	store.push_back(thisPoint);
	
	do
	{
		w = store.at(0).w;
		h = store.at(0).h;
		store.erase(store.begin());
		
		if (w>0 && w<width-1 && h>0 && h<height-1)
		{
			val = (h-1)*width + (w - 1);
			
			if ( edge[val]>0 && edge[val]<255 ) 
			{
				edge[val]=255;
				h--;w--;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = (h-1)*width + w;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				h--;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = (h-1)*width + (w+1);
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				h--;w++;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = h*width + w-1;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				w--;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = h*width + w+1;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				w++;
				length++;			
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = (h+1)*width + w-1;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				h++;w--;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = (h+1)*width + w;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				h++;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
			
			val = (h+1)*width + w+1;
			if (edge[val]>0 && edge[val]<255) 
			{
				edge[val]=255;
				h++;w++;
				length++;
				thisPoint.w = w;
				thisPoint.h = h;
				store.push_back(thisPoint);
			}
		}
	}	while (store.size()>0) ; //END of do
}




/*---------------------------------------------------------------------------
 //-  Function Name : CannyOperator               
 //-  Description   : Determine edges in the image
 //-  Parameter     : info - skin data
 bw_current - grayscale component of the colour image
 edge_total - data array that contain edge information
 height - height of the image
 width - width of the image
 highThreshold - high treshold value for canny computation
 lowThreshold - low threshold value for canny computation
 lengthThres - threshold value to determine the lenght of skin edge
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::CannyOperator(INT *info, unsigned char *bw_current, INT *edge_total, INT height, INT width, INT highThreshold, INT lowThreshold, INT lengthThres)
{
	bool satisfied = FALSE;
	INT skinEdge = 0; 
	INT m_gauss = 3;
	INT hw = height*width;
	INT h;
	INT total9 = 0;
	
	INT * edge		 = new INT[hw];
	INT * bw		 = new INT[hw];
	INT * bw2		 = new INT[hw];
	INT * edge_dir	 = new INT[hw];
	
	memset(edge,  0 , sizeof(INT) * hw);
	memset(edge_dir, 0 , sizeof(INT) * hw);
	
	INT hMinus10     = height - 9;
	INT widthMinus10 = width - 9;
	
	do	
	{ 
		skinEdge = 0;
		for (h=hw;h--;) bw[h] = bw_current[h];
		
		/////////////// GAUSSIAN = 3 ////////////////////
		if (m_gauss==3)
		{		
			INT *bwptr;
			INT *bw2ptr = bw2;
			INT ww=0;INT hh=0;
			for (h=0;h<hw;h++) 
			{
				if (ww>9 && ww<widthMinus10 && hh>9 && hh<hMinus10) 
				{
					total9 = 0;
					bwptr = bw;
					bwptr+= h-9;
					
					total9+=*bwptr++;
					total9+=*bwptr++;
					total9+=*bwptr++ << 1;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 6;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 3;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 1;
					total9+=*bwptr++;
					total9+=*bwptr;						
					
					bwptr = bw;
					bwptr += h - 9*width;
					
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr << 1;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 6;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 1;
					bwptr+=width;
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr;						
				}		
				*bw2ptr++ = total9 >> 9;		
				ww++;
				if ( ww>=width) {ww++;hh++;}
			}
		}
		else
		{
			/////////////// GAUSSIAN = 4 ////////////////////
			satisfied = true; 
			INT *bwptr;
			INT ww=0;INT hh=0;
			INT *bw2ptr = bw2;
			for (h=0;h<hw;h++) 
			{			
		  		if (ww>9 && ww<widthMinus10 && hh>9 && hh<hMinus10) 
				{
					total9 = 0;
					bwptr = &bw[h-9];			
					
					total9+=*bwptr++;
					total9+=*bwptr++;
					total9+=*bwptr++ << 1;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 3;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 5;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 4;
					total9+=*bwptr++ << 3;
					total9+=*bwptr++ << 2;
					total9+=*bwptr++ << 1;
					total9+=*bwptr++;
					total9+=*bwptr;
					
					bwptr = bw;
					bwptr += h - 9*width;
					
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr << 1;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 3;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 5;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 4;
					bwptr+=width;
					total9+=*bwptr << 3;
					bwptr+=width;
					total9+=*bwptr << 2;
					bwptr+=width;
					total9+=*bwptr << 1;
					bwptr+=width;
					total9+=*bwptr;
					bwptr+=width;
					total9+=*bwptr;
				}
				*bw2ptr++ = total9 >> 9;					
				ww++;
				if ( ww>=width) {ww++;hh++;}
			}
		}
		
		// Real Implementation of Canny

		float PI22_5 = static_cast<float>(M_PI / 8.0);
		float PI67_5 = PI22_5 * 3.0;
		float PI112_5 = PI22_5 * 5.0;
		float PI157_5 = PI22_5 * 7.0;
		float PI90 = static_cast<float>(M_PI/2.0);
		
		INT max_edge=0;
		INT hPlus,hMinus ;
		float ORIENT;
		INT ww=0, hh=0; 
		
		for (h=0;h<hw;h++)
		{	
			if (ww>9 && ww<widthMinus10 && hh>9 && hh<hMinus10) 			
			{
				hMinus = h-width;
				hPlus  = h+width;
				
				INT sumY = (bw[hMinus+1] + bw[h+1] + bw[hPlus+1]) - 
				(bw[hMinus-1] + bw[h-1] + bw[hPlus-1]);
				
				INT sumX = (bw[hMinus+1] + bw[hMinus] + bw[hMinus-1]) - 
				(bw[hPlus+1 ] + bw[hPlus ] + bw[hPlus-1 ]);
				
				edge[h]= abs(sumX) + abs(sumY);
				
				if (max_edge<edge[h])	max_edge=edge[h];
				if (edge[h]<lowThreshold) 
					edge[h]=0;
				else
				{
					if(sumX == 0)   
					{
						if(sumY==0) 
						{
							ORIENT = 0.0;
						}
						else 
						{
							if (sumY<0)   
							{
								sumY = -sumY;
								ORIENT = PI90;
							}
							else		
							{
								ORIENT = PI90;
							}
						}
					}
					// Can't take invtan of angle in 2nd Quad 
					else 
					{
						if(sumX<0 && sumY>0)   
						{
							sumX = -sumX;
							ORIENT = M_PI - ((atan((float)(sumY)/(float)(sumX))));
						}
						// Can't take invtan of angle in 4th Quad        
						else	
						{
							if(sumX>0 && sumY<0)   
							{
								sumY = -sumY;
								ORIENT = M_PI -  ((atan((float)(sumY)/(float)(sumX))));
							}
							// else angle is in 1st or 3rd Quad 
							else 
							{
								ORIENT = (atan((float)(sumY)/(float)(sumX)));
							}
						}
					}
					
					// Find edge_dir by assigning ORIENT a value of			//
					// either 0, 45, 90 or 135 degrees, depending on which	//
					// value ORIENT is closest to							//
					
					if(ORIENT < PI22_5) edge_dir[h] = 0;
					else if(ORIENT < PI67_5) edge_dir[h]= 45;
					else if(ORIENT < PI112_5) edge_dir[h] = 90;
					else if(ORIENT < PI157_5) edge_dir[h] = 135;
					else edge_dir[h] = 0;			
				} 
			}
			if (max_edge> 0) edge[h] = (edge[h] << 8)/max_edge;
			ww++;
			if ( ww>=width) {ww=0;hh++;}
		} //END of for (h=0;h<hw;h++)
		
		
		// Obtain values of 2 adjacent pixels tagent to edge	//
		// direction and remove those not on the ridge			//
		INT leftPixel,rightPixel;
		ww=0;hh=0; 
		for (h=0;h<hw;h++)
		{
			if (ww>9 && ww<widthMinus10 && hh>9 && hh<hMinus10) 		
			{
				if (edge_dir[h] == 0)   
				{
					leftPixel = edge[h-width];
					rightPixel = edge[h+width];
				}
				else 
				{
					if (edge_dir[h] == 45)   
					{
						leftPixel = edge[h+width-1];
						rightPixel = edge[h-width+1];
					}
					else 
					{
						if (edge_dir[h] == 90)   
						{
							leftPixel = edge[h-1];
							rightPixel = edge[h+1];
						}
						else   
						{
							leftPixel = edge[h-width-1];
							rightPixel = edge[h+width+1];
						}
					}
				}
				
				if(edge[h] < abs(leftPixel) || edge[h] < abs(rightPixel)) 
				{
					edge[h] = - edge[h];
				}
			}
			ww++;
			if ( ww>=width) {ww=0;hh++;}
		} //END of for
		
		
		for (h=0;h<hw;h++)	if (edge[h]<0) edge[h]=0;
		
		// Need to follow ridge along to ensure those between	//
		// the high / low thresholds are marked if connected	//
		// to a ridge with one above the high threshold			//
		
		ww=0;hh=0; 
		for (h=0;h<hw;h++)
		{
			if (ww>9 && ww<widthMinus10 && hh>9 && hh<hMinus10) 		
			{
				if (edge[h]>=50 && edge[h]<255)
				{
					edge[h]=255;
					INT length;
					FollowRidge(edge,hh,ww,width,height,length);
				}
			}
			ww++;
			if ( ww>=width) {ww=0;hh++;}
		}
		
		
		for (h=0;h<hw;h++)
		{
			if (edge[h]<255) edge[h]=0;								
			edge_total[h] = edge[h];
		}
		
		for (h=hw;h--;)
			if (info[h]>0 && edge_total[h]==255) skinEdge++;
		
		
		if (skinEdge > 5000) 
		{
			if (m_gauss < 4)  m_gauss++;
			highThreshold += 35;
		}
		else satisfied = true; 
	} while (satisfied == false); //END of do
	
	delete[] bw;
	delete[] edge;
	delete[] bw2;
	delete[] edge_dir;
}


/*---------------------------------------------------------------------------
 //-  Function Name : CalcMoments
                
 //-  Description   : Calculate the seven Hu Moments from the edge information
 //-  Parameter     : edge_total - data array that contain edge information
 width - width of the image
 height - height of the image
 padMoments - arrays of normalized Hu Moments
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::CalcMoments(INT * edge_total, INT width, INT height, DOUBLE * padMoments) 
{
	INT h,w;
	
	DOUBLE total_skin=0.0; // Total number of pixels that are skin
	DOUBLE X=0.0; // Total X value
	DOUBLE Y=0.0; // Total Y value
	DOUBLE XX=0.0; // Total X*X value
	DOUBLE YY=0.0; // Total Y*Y value
	DOUBLE XY=0.0; // Total X*Y value
	DOUBLE XXX=0.0; // Total X*X*X value
	DOUBLE YYY=0.0; // Total Y*Y*Y value
	DOUBLE XXY=0.0; // Total X*X*Y value
	DOUBLE YYX=0.0; // Total Y*Y*X value
	
	
	for (h=0;h<height;h++)
	{
		for (w=0;w<width;w++)
		{
			if (edge_total[h*width+w] == 255) 
			{
				total_skin+=(DOUBLE)1.0; //get the number of pixels of the boundary of the blob
				X+=(DOUBLE)w;
				Y+=(DOUBLE)h;
				XX+=(DOUBLE)w*w;
				YY+=(DOUBLE)h*h;
				XY+=(DOUBLE)w*h;
				XXX+=(DOUBLE)w*w*w;
				YYY+=(DOUBLE)h*h*h;
				XXY+=(DOUBLE)w*w*h;
				YYX+=(DOUBLE)h*h*w;
			}
		}
	}
	
	DOUBLE HU_MOMENT[7]={0.0};
	
	if (total_skin>0)
	{
		DOUBLE Xmean = X / total_skin;
		DOUBLE Ymean = Y / total_skin;
		
		//translation invariant 
		DOUBLE mu00 = total_skin;
		DOUBLE mu11 = XY - mu00*Xmean*Ymean;
		DOUBLE mu20 = XX - mu00*Xmean*Xmean;
		DOUBLE mu02 = YY - mu00*Ymean*Ymean;
		DOUBLE mu21 = XXY - XX*Ymean - 2*XY*Xmean + 2*mu00*Xmean*Xmean*Ymean;
		DOUBLE mu12 = YYX - YY*Xmean - 2*XY*Ymean + 2*mu00*Ymean*Ymean*Xmean;
		DOUBLE mu30 = XXX - 3.0*XX*Xmean + 2.0*mu00*Xmean*Xmean*Xmean;
		DOUBLE mu03 = YYY - 3.0*YY*Ymean + 2.0*mu00*Ymean*Ymean*Ymean;
		//scaling invariant (this scale invariant seems not correct)
		DOUBLE nu11 = mu11 / pow(mu00,2.0);
		DOUBLE nu20 = mu20 / pow(mu00,2.0);
		DOUBLE nu02 = mu02 / pow(mu00,2.0);
		DOUBLE nu21 = mu21 / pow(mu00,2.5);
		DOUBLE nu12 = mu12 / pow(mu00,2.5);
		DOUBLE nu30 = mu30 / pow(mu00,2.5);
		DOUBLE nu03 = mu03 / pow(mu00,2.5);
		
		
		HU_MOMENT[0] = nu20 + nu02;
		
		HU_MOMENT[1] = (nu20 - nu02)*(nu20 - nu02) + 4.0*nu11*nu11;
		
		HU_MOMENT[2] = (nu30 - 3.0*nu12)*(nu30 - 3.0*nu12) + (nu03 - 3.0*nu21)*(nu03 - 3.0*nu21);
		
		HU_MOMENT[3] = (nu30 + nu12)*(nu30 + nu12) + (nu03 + nu21)*(nu03 + nu21);
		
		HU_MOMENT[4] = (nu30 - 3.0*nu12)*(nu30 + nu12)*((nu30 + nu12)*(nu30 + nu12)
														- 3.0*(nu21 + nu03)*(nu21 + nu03)) + (3.0*nu21 - nu03)*(nu21 + nu03)
		*(3.0*(nu30 + nu12)*(nu30 + nu12) - (nu21 + nu03)*(nu21 + nu03));
		
		HU_MOMENT[5] = (nu20 - nu02)*((nu30 + nu12)*(nu30 + nu12) - (nu21 + nu03)*(nu21 + nu03))
		+ 4.0*nu11*(nu30 + nu12)*(nu21 + nu03);
		
		HU_MOMENT[6] = (3.0*nu21 - nu03)*(nu30 + nu12)*((nu30 + nu12)*(nu30 + nu12)
														- 3.0*(nu21 + nu03)*(nu21 + nu03)) - (nu30 - 3.0*nu12)*(nu21 + nu03)
		*(3.0*(nu30 + nu12)*(nu30 + nu12) - (nu21 + nu30)*(nu21 + nu30));
		
		
		// normalise the stuff
		INT mom;
		for (mom=0;mom<7;mom++) if (HU_MOMENT[mom]<(DOUBLE)0.0) HU_MOMENT[mom]= - HU_MOMENT[mom];
		DOUBLE MOMENTS_TOTAL=(DOUBLE) 0.0;
		for (mom=0;mom<7;mom++) MOMENTS_TOTAL+=HU_MOMENT[mom];
		for (mom=0;mom<7;mom++) HU_MOMENT[mom]=HU_MOMENT[mom]/MOMENTS_TOTAL;
		
		for (h=0; h<WQ_MOMENTS_SIZE; h++)		padMoments[h] = HU_MOMENT[h];
	}
}


/*---------------------------------------------------------------------------
 //-  Function Name : CannyEdgeHistogram16x16Optimal             
 //-  Description   : Determine the edge histogram
 //-  Parameter     : edges - data array that contain edge information
 width - width of the image
 height - height of the image
 pResults - structure of porn features
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::CannyEdgeHistrogram16x16Optimal(INT *edges, INT width, INT height, ImageProcessing::WQ_IP_RESULTS * pResults)
{
	INT h,w;
	INT mapW[16]; 
	INT mapH[16]; 
	
	for (h=0;h<16;h++) 
	{
		mapW[h]=0;
		mapH[h]=0;
	}
	
	for (h=0;h<height;h++)
	{
		INT h_offset = ((h*16)/height);
		INT h_width = h*width;
		for (w=0;w<width;w++)
		{
			INT w_offset = (w*16)/width;
			mapH[h_offset]+=edges[h_width+w];
			mapW[w_offset]+=edges[h_width+w];
		}
	}
	
	for (h=0;h<16;h++) 
	{
		mapH[h]=((mapH[h]/255)*100)/height;
		mapW[h]=((mapW[h]/255)*100)/width;
	}
	
	for (h=0;h<16;h++) 
	{
		float value = ((float) mapH[h])/400.0;
		pResults->dEdgeMap[h]=value;
		if (value>1.0) pResults->dEdgeMap[h] = 1.0;
	}
	for (h=0;h<16;h++) 
	{
		float value = ((float) mapW[h])/400.0;
		pResults->dEdgeMap[16+h]=value;
		if (value>1.0) pResults->dEdgeMap[16+h] = 1.0;
	}
}


/*---------------------------------------------------------------------------
 //-  Function Name : AdaptSmoothOptimised                
 //-  Description   : Smooth the colour components, LUV
 //-  Parameter     : LUV - array of smoothed colour component in LUV colour model
 LUV2 - original array of colour component in LUV colour model
 weight - array of weight to represent pixel smoothness
 height - height of the image
 width - width of the image
 offset - the left and right size of smoothing mask
 interval - pixel interval for smoothing mask processing
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
INT ImageProcessing::AdaptSmoothOptimised(unsigned char *LUV, unsigned char *LUV2,INT *weight,INT height,INT width,INT offset,INT interval)
{
	INT Width_3=width*3;
	INT FilterArea=(2*offset+1)*(2*offset+1);
	INT FilterArea_m1 = FilterArea-1;
	INT *dist=new INT[FilterArea];
	INT *index_dist=new INT[FilterArea];
	INT *diffdist=new INT[FilterArea_m1];
	unsigned char **peer_array=new unsigned char *[FilterArea];
	INT *cut_value=new INT[FilterArea];
	INT *index_cut=new INT[FilterArea];
	//	char homam[200];
	
	INT diffdist_thres = 12;
	
	INT noise_interval=offset+1;
	INT noise_interval_m1 = noise_interval-1;
	INT J1,J2,J;
	INT intra_left,intra_right,mean_left,mean_right;
	INT avg=0;
	INT peer[3];
	INT countpoint=0;
	
	
	INT hlast = height-offset;
	INT hjump = interval;
	INT wjump = 3*interval;
	INT wlast = (width-offset)*3;
	INT offset_3 = offset*3;
	INT pos=0;
	INT total5 = 0;
	
	for (INT h=interval;h<hlast;h+=hjump)
	{
		for(INT w=3*interval;w<wlast;w+=wjump)
		{
			//finding the absolute distance of the neighborhood pixel and the investigated current pixel
			INT npoint=0;
			INT h_Width_3 = h*Width_3; 
			INT center=h_Width_3+w;
			INT valueC1 = LUV2[center++];
			INT valueC2 = LUV2[center++];
			INT valueC3 = LUV2[center];
			
			for(INT hh=-offset;hh<=offset;hh++)
			{
				for(INT ww=-offset_3;ww<=offset_3;ww+=3)
				{
					pos = (h+hh)*Width_3+w+ww;
					peer_array[npoint]=LUV2+pos;
					INT temp_value = LUV2[pos++]-valueC1;;
					total5= temp_value*temp_value;
					temp_value = LUV2[pos++]-valueC2;
					total5+=temp_value*temp_value;
					temp_value = LUV2[pos++]-valueC3;
					total5+=temp_value*temp_value;
					
					
					dist[npoint++]=luvSQRT[total5];
				}
			}
			
			//ascending sort the neigborhood index according to the absolute distance
			INT *index_ptr = index_dist;
			for(int i=0;i<FilterArea;i++) *index_ptr++=i;
			QsortIndexInt(0,FilterArea_m1,dist,index_dist,0);
		
			//determine the differences of the distance
			INT *dist_ptr = dist;
			INT *diffdist_ptr = diffdist;
			for(int i=0;i<FilterArea_m1;i++) *diffdist_ptr++=-*dist_ptr+*dist_ptr++;
			
			//determine the first point and the last point to compute the adaptive thresholding
			J1=0; //if no near impulsive noise detected
			diffdist_ptr=diffdist+noise_interval_m1;
			for(INT i=noise_interval_m1;i>0;i--)
			{
				if (*diffdist_ptr-->diffdist_thres) {J1=i+1; break;}
			}
			J2=FilterArea_m1; //if no far impulsive noise detected
			diffdist_ptr=diffdist+FilterArea_m1-noise_interval;
			for(int i=FilterArea_m1-noise_interval;i<FilterArea_m1;i++)
			{
				if(*diffdist_ptr++>diffdist_thres) {J2=i; break;}
			}
			J=J2-offset;
			
			//Determine the cutting point
			INT ddist=0;
			
			ddist+=abs(peer_array[index_dist[J1]][0]-peer_array[index_dist[J2]][0]);
			
			
			ddist+=abs(peer_array[index_dist[J1]][1]-peer_array[index_dist[J2]][1]);
			
			ddist+=abs(peer_array[index_dist[J1]][2]-peer_array[index_dist[J2]][2]);
			
			
			if(ddist==0)
			{
				*LUV++=peer_array[index_dist[J1]][0];
				*LUV++=peer_array[index_dist[J1]][1];
				*LUV++=peer_array[index_dist[J1]][2];
				*weight++=0;
			}
			else
			{
				for(INT i=J1;i<=J;i++)
				{
					intra_left=0;intra_right=0;
					mean_left=0;mean_right=0;
					
					for(INT j=J1;j<=i;j++) mean_left+=dist[j];
					mean_left*=100;
					mean_left/=(i-J1+1);
					for(INT j=J1;j<=i;j++) intra_left+=sqr(100*dist[j]-mean_left);
					
					for(INT j=i+1;j<=J2;j++) mean_right+=dist[j];
					mean_right*=100;
					mean_right/=(J2-i);
					for(INT j=i+1;j<=J2;j++) intra_right+=sqr(100*dist[j]-mean_right);
					
					if (mean_right==mean_left) cut_value[i-J1]=intra_left+intra_right; 
					else cut_value[i-J1]=(intra_left+intra_right)/sqr(mean_right-mean_left);
					
				}
				//ascending sort the cut_value
				index_ptr=index_cut;
				for(INT i=0;i<FilterArea;i++) *index_ptr++=i;
				QsortIndexInt(0,J-J1,cut_value,index_cut,0);
				
				
				INT distVal=dist[index_cut[0]+J1]-dist[J1];
				avg+=distVal;
				*weight++=distVal;
				
				
				for(INT i=0;i<3;i++) peer[i]=0;
				for(INT i=J1;i<=index_cut[0]+J1;i++)
				{
					peer[0]+=peer_array[index_dist[i]][0];
					peer[1]+=peer_array[index_dist[i]][1];
					peer[2]+=peer_array[index_dist[i]][2];
				}
				*LUV++=peer[0]/(index_cut[0]+1);
				*LUV++=peer[1]/(index_cut[0]+1);
				*LUV++=peer[2]/(index_cut[0]+1);
			}
			countpoint++;
		}
	}
	
	if (countpoint==0) avg=-1;
	else avg=avg/countpoint;
	
	delete[] dist;			dist=NULL;
	delete[] index_dist;	index_dist=NULL;
	delete[] index_cut;		index_cut=NULL;
	delete[] diffdist;		diffdist=NULL;
	delete[] cut_value;		cut_value=NULL;
	delete[] peer_array;	peer_array=NULL;
	return avg;
}


/*---------------------------------------------------------------------------
 //-  Function Name : QsortIndexInt             
 //-  Description   : Perform sorting
 //-  Parameter     : left - smallest index of the array
 right - largest index of the array
 A - array of elements (INT) to be sorted
 index - array of index
 order - 0 for descending and 1 for ascending
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::QsortIndexInt(INT left,INT right,INT *A,INT *index,INT order)
{
	INT	i = left; 
	INT j = right;
	
	INT middle = A[(left+right)/2];
	INT temp;
	INT	temp2;
	
	for(;i<=j;)
	{
		if(order==1)
		{
			for (;A[i] > middle && i < right;) i++;
			for (;middle > A[j] && j > left;) j--;
		}
		else
		{
			for (;A[i] < middle && i < right;) i++;
			for (;middle < A[j] && j > left;) j--;
		}
		
		if (i<=j)
		{
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;
			temp2 = index[i];
			index[i] = index[j];
			index[j] = temp2;
			i++;
			j--;
		}
	} 
	if (left<j) QsortIndexInt(left,j,A,index,order);
	if (i<right) QsortIndexInt(i,right,A,index,order);
}


/*---------------------------------------------------------------------------
 //-  Function Name : mergecbOptimised               
 //-  Description   : Combine colour cluster base on distance and area
 //-  Parameter     : LUV - data array of LUV colour component
 cb - codebook for colour cluster index
 colour map - map that contains cb index for an image
 N - number of colour cluster
 thresh - threshold value for minimum distance between two clusters
 height - height of the image
 width - width of the image
 //-  Return Value  : number of colour cluster
 //-------------------------------------------------------------------------*/
INT ImageProcessing::mergecbOptimised(unsigned char *LUV,INT **cb,unsigned char *colour_map,INT N,INT thresh,INT height,INT width)
{
	if (N==1) return 1;
	
	INT *cb_area=new INT[N];
	INT currentpixel=0;
	INT **dist_cb=new INT*[N];
	INT dist_cb_size = N;
	for(INT i=0;i<N;i++)
	{
		cb_area[i]=0;
		dist_cb[i]=new INT[N];
	}
	INT hw=height*width;
	
	// Find the area for each colour cluster
	for(INT h=0;h<hw;h++)
	{
		cb_area[colour_map[h]]++;
	}
	
	// Find the square distance between colour cluster
	INT d1;
	for(INT i=0;i<N-1;i++)
	{
		for(INT j=i+1;j<N;j++)
		{
			d1=0;
			for(INT ii=0;ii<3;ii++) d1+=(cb[i][ii]-cb[j][ii])*(cb[i][ii]-cb[j][ii]);
			dist_cb[i][j]=d1;
			dist_cb[j][i]=d1;
		}
	}
	
	// Find the pair of codebook that having the shortest distance
	INT min_dist=100000;
	INT min_i,min_j;
	for(INT i=0;i<N-1;i++)
	{
		for(INT j=i+1;j<N;j++)
		{
			if(dist_cb[i][j]<min_dist)
			{
				min_dist=dist_cb[i][j];
				min_i=i;
				min_j=j;
			}
		}
	}
	
	INT endloop=0;
	INT total_cbarea;
	while(endloop==0 && N>1)
	{
		if(min_dist>=thresh) break;
		total_cbarea=cb_area[min_i]+cb_area[min_j];
		if (total_cbarea>0)
		{
			for(INT ii=0;ii<3;ii++) 
			{
				cb[min_i][ii]=(cb_area[min_i]*cb[min_i][ii]+cb_area[min_j]*cb[min_j][ii])/total_cbarea;
			}
		}
		else
		{
			cb[min_i][0]=0;
			cb[min_i][1]=0;
			cb[min_i][2]=0;
		}
		
		cb_area[min_i]=total_cbarea;
		N--; // Reduce one codebook as two cb having distande lower than threshold
		// Update others cb area and cb
		for(INT i=min_j;i<N;i++)
		{
			cb_area[i]=cb_area[i+1];
			for(INT ii=0;ii<3;ii++) cb[i][ii]=cb[i+1][ii];
		}
		for(INT i=min_j;i<N;i++)
		{
			for(INT j=0;j<min_j;j++) dist_cb[i][j]=dist_cb[i+1][j];
		}
		for(INT j=min_j;j<N;j++)
		{
			for(INT i=0;i<min_j;i++) dist_cb[i][j]=dist_cb[i][j+1];
		}
		for(INT i=min_j;i<N;i++)
		{
			for(INT j=min_j;j<N;j++) dist_cb[i][j]=dist_cb[i+1][j+1];
		}
		
		//determine the distance of new codebook with others remaining code book
		for(INT i=0;i<N;i++)
		{
			d1=0;
			for(INT ii=0;ii<3;ii++) d1+=(cb[min_i][ii]-cb[i][ii])*(cb[min_i][ii]-cb[i][ii]);
			dist_cb[min_i][i]=d1;
			dist_cb[i][min_i]=d1;
		}
		
		//finding the next minimum distance
		min_dist=100000;
		for(INT i=0;i<N-1;i++)
		{
			for(INT j=i+1;j<N;j++)
			{
				if(dist_cb[i][j]<min_dist)
				{
					min_dist=dist_cb[i][j];
					min_i=i;
					min_j=j;
				}
			}
		}
	}
	
	for(INT i=0;i<dist_cb_size;i++)
	{
		delete [] dist_cb[i];
	}
	delete [] dist_cb; dist_cb=NULL;
	delete[] cb_area; cb_area=NULL;
	return N;
}


/*---------------------------------------------------------------------------
 //-  Function Name : UpdateColourmapOptimised                
 //-  Description   : Update cookbook
 //-  Parameter     : LUV - data array of LUV colour component
 weight - array of weight to represent pixel smoothness
 N - number of colour cluster
 cb - codebook for colour cluster index
 colour_map - map that contains cb index for an image
 height - height of the image
 width - widht of the image
 //-  Return Value  : 0 if codebook does not exist, other than 0 if exist
 //-------------------------------------------------------------------------*/
INT ImageProcessing::UpdateColourmapOptimised(unsigned char *LUV,INT *weight,INT N,INT **cb,unsigned char *colour_map,INT height,INT width)
{
	INT repetition_num=5;
	INT d1,d2;
	INT mse,last_mse,rate;
	INT *totalw=new INT[N];
	INT cb_exist=0;
	INT rand_h;
	INT hw = height*width;
	unsigned char *LUV_ptr;
	
	INT LUV1,LUV2,LUV3;
	
	for(INT rep_num=0;rep_num<repetition_num;rep_num++)
	{
		//update the colour map with the new codebook
		mse=0;
		LUV_ptr = LUV;
		for(INT h=0;h<hw;h++)
		{
			d1=0;
			LUV1 = *LUV_ptr++;
			LUV2 = *LUV_ptr++;
			LUV3 = *LUV_ptr++;
			d1+=(LUV1-cb[0][0])*(LUV1-cb[0][0]);
			d1+=(LUV2-cb[0][1])*(LUV2-cb[0][1]);
			d1+=(LUV3-cb[0][2])*(LUV3-cb[0][2]);
			colour_map[h]=0;
			for(INT ii=1;ii<N;ii++)
			{
				d2=0;
				d2+=(LUV1-cb[ii][0])*(LUV1-cb[ii][0]);
				d2+=(LUV2-cb[ii][1])*(LUV2-cb[ii][1]);
				d2+=(LUV3-cb[ii][2])*(LUV3-cb[ii][2]);
				if(d2<d1) {d1=d2; colour_map[h]=(unsigned char)ii;}
			}
			mse+=d1;
		}
		
		//initialize the codebook and the totalw
		for(INT i=0;i<N;i++)
		{
			totalw[i]=0;
			for(INT ii=0;ii<3;ii++) cb[i][ii]=0;
		}
		
		//based on the new colour map, update the codebook
		LUV_ptr = LUV;
		for(INT h=0;h<hw;h++)
		{
			cb[colour_map[h]][0]+=weight[h]*(*LUV_ptr++);
			cb[colour_map[h]][1]+=weight[h]*(*LUV_ptr++);
			cb[colour_map[h]][2]+=weight[h]*(*LUV_ptr++);
			totalw[colour_map[h]]+=weight[h];
		}
		
		for(INT i=0;i<N;i++)
		{
			if(totalw[i]>0)
			{
				for(INT ii=0;ii<3;ii++) cb[i][ii]/=totalw[i];	
			}
			else
			{
				// randomly determine the codebook which is not defined
				// rand_h=(hw*rand()-1)/RAND_MAX;
				rand_h=hw/2;
				for(INT ii=0;ii<3;ii++) cb[i][ii]=LUV[rand_h*3+ii];			
			}
		}
		if(rep_num>0)
		{
			if(last_mse>0)
				rate=(100*(last_mse-mse))/last_mse;
			else
				break;
			if(rate<5) break;
		}
		last_mse=mse;
	} //end of repetition
	
	
	//recalculate the codebook without dependent to the weight
	for(INT i=0;i<N;i++)
	{
		totalw[i]=0;
		for(INT ii=0;ii<3;ii++) cb[i][ii]=0;
	}
	
	LUV_ptr = LUV;
	for(INT h=0;h<hw;h++)
	{
		cb[colour_map[h]][0]+=*LUV_ptr++;
		cb[colour_map[h]][1]+=*LUV_ptr++;
		cb[colour_map[h]][2]+=*LUV_ptr++;
		totalw[colour_map[h]]++;
	}
	
	for(INT i=0;i<N;i++)
	{
		if(totalw[i]>0)
		{
			for(INT ii=0;ii<3;ii++) cb[i][ii]/=totalw[i];
		}
	}
	
	delete[] totalw; totalw=NULL;
	return cb_exist;
}


/*---------------------------------------------------------------------------
 //-  Function Name : ColourClusteringOptimised              
 //-  Description   : Compute the colour cluster map
 //-  Parameter     : LUV - data array of LUV colour component
 weight - array of weight to represent pixel smoothness
 N - number of colour cluster
 cb - codebook for colour cluster index
 height - height of the image
 width - width of the image
 //-  Return Value  : updated number of colour cluster
 //-------------------------------------------------------------------------*/
INT ImageProcessing::ColourClusteringOptimised(unsigned char *LUV,INT *weight,INT N,INT **cb,INT height,INT width)
{
	INT hw=height*width;
	unsigned char *colour_map=new unsigned char[hw];
	for(INT ii=0;ii<hw;ii++) colour_map[ii]=0;
	
	INT* totalw=new INT[N];
	
	int64** variance_buffer= new int64*[N];
	for(INT ii=0;ii<N;ii++) variance_buffer[ii]=new int64[3];
	
	INT* variance=new INT[N];
	INT dist[3];
	INT new_split,imax;
	INT d1,d2;
	
	unsigned char LUV1,LUV2,LUV3;
	unsigned char *LUV_ptr;
	unsigned char *colour_map_ptr;
	INT *weight_ptr;
	
	//calculate the initial centroid for first cluster for LUV
	INT currentpixel=0;
	for(INT i=0;i<3;i++) cb[0][i]=0;
	totalw[0]=0;
	
	LUV_ptr = LUV;
	weight_ptr = weight;
	
	for(INT h=0;h<hw;h++)
	{
		colour_map[h]=0;
		totalw[0]+=weight[h];
		cb[0][0]+=*LUV_ptr++*(*weight_ptr);
		cb[0][1]+=*LUV_ptr++*(*weight_ptr);
		cb[0][2]+=*LUV_ptr++*(*weight_ptr++);
	}
	for(INT i=0;i<3;i++)
	{
		if (totalw[0] != 0)
			cb[0][i]/=totalw[0];
	}
	INT currentN=1;
	
	while (currentN<N)
	{
		//initialization variables for each colour cluster
		for(INT i=0;i<currentN;i++)
		{
			for(INT ii=0;ii<3;ii++) variance_buffer[i][ii]=0;
			totalw[i]=0;
		}
		
		//calculate the variance between centroid for each cluster and the colour 
		//members in the colour cluster
		LUV_ptr = LUV;
		weight_ptr = weight;
		colour_map_ptr = colour_map;
		for(INT h=0;h<hw;h++)
		{
			dist[0]=cb[*colour_map_ptr][0]-*LUV_ptr++;
			dist[1]=cb[*colour_map_ptr][1]-*LUV_ptr++;
			dist[2]=cb[*colour_map_ptr][2]-*LUV_ptr++;
			
			variance_buffer[*colour_map_ptr][0]+=*weight_ptr*dist[0]*dist[0];
			variance_buffer[*colour_map_ptr][1]+=*weight_ptr*dist[1]*dist[1];
			variance_buffer[*colour_map_ptr][2]+=*weight_ptr*dist[2]*dist[2];
			
			totalw[*colour_map_ptr++]+=*weight_ptr++;
		}
		
		//finding the total variance for each buffer for each colour cluster
		//finding the average variance_buffer
		for(INT i=0;i<currentN;i++) 
		{
			variance[i]=0;
			for(INT ii=0;ii<3;ii++) 
			{
				variance[i]+=variance_buffer[i][ii];
				if (variance_buffer[i][ii]<0) variance_buffer[i][ii]=0; //should not be negative
				if (totalw[i]>0) variance_buffer[i][ii]=luvSQRT[variance_buffer[i][ii]/totalw[i]];
				else variance_buffer[i][ii]=0;
			}
		}
		
		INT *index_var=new INT[currentN];
		for(INT i=0;i<currentN;i++) index_var[i]=i;
		QsortIndexInt(0,currentN-1,variance,index_var,1);
		
		//determine the additional number of colour cluster to add
		new_split=currentN/2+1;
		//to determine the right amount additional cluster without exceeding predetermined number of cluster
		if((new_split+currentN)>N) new_split=N-currentN; 
		//determine the new codebook
		for(INT i=0;i<new_split;i++)
		{
			imax=index_var[i]; //the original index of codebook to split
			//update the new additional codebook and the existing codebook
			for(INT ii=0;ii<3;ii++)
			{
				cb[currentN+i][ii]=cb[imax][ii]-variance_buffer[imax][ii];
				cb[imax][ii]=cb[imax][ii]+variance_buffer[imax][ii];
			}
		}
		
		//determine the new number of colour cluster to be update
		currentN+=new_split;
		
		//based on the new codebook, update the colour map and refine the codebook
		INT cb_exist;
		cb_exist=UpdateColourmapOptimised(LUV,weight,currentN,cb,colour_map,height,width);
		
		//if similar codebook exist, remove the similar one (but seems will never come into this)
		if(cb_exist)
		{
			//			MessageBox(NULL,"imposible","",0);
			for (INT jn=0;jn<currentN-1;jn++)
			{
				for (INT kn=jn+1;kn<currentN;kn++)
				{
					d1=0;
					for(INT ii=0;ii<3;ii++) d1+=abs(cb[jn][ii]-cb[kn][ii]);
					if (d1==0)
					{
						for (INT j=kn;j<currentN-1;j++) 
						{
							for (INT ii=0;ii<3;ii++) cb[j][ii]=cb[j+1][ii];
						}
						currentN--;
					}
				}
			}
			
			LUV_ptr=LUV;
			//update the colour_map after remove some of the repeated codebook
			for(INT h=0;h<hw;h++)
			{
				d1=0;
				LUV1 = *LUV_ptr++;
				LUV2 = *LUV_ptr++;
				LUV3 = *LUV_ptr++;
				d1+=(LUV1-cb[0][0])*(LUV1-cb[0][0]);
				d1+=(LUV2-cb[0][1])*(LUV2-cb[0][1]);
				d1+=(LUV3-cb[0][2])*(LUV3-cb[0][2]);
				
				colour_map[h]=0;
				for(INT ii=1;ii<currentN;ii++)
				{
					d2=0;
					d2+=(LUV1-cb[ii][0])*(LUV1-cb[ii][0]);
					d2+=(LUV2-cb[ii][1])*(LUV2-cb[ii][1]);
					d2+=(LUV3-cb[ii][2])*(LUV3-cb[ii][2]);
					if(d2<d1) {d1=d2; colour_map[currentpixel]=(unsigned char)ii;}
				}
			}
			break;
		}
		
		//release memory
		delete[] index_var; index_var=NULL;
	}
	
	//---------------------------------------------------------------------------
	//- Combine colour cluster base on distance and area difference 
	//---------------------------------------------------------------------------
	INT thresh=255;
	currentN=mergecbOptimised(LUV,cb,colour_map,currentN,thresh,height,width);
	//---------------------------------------------------------------------------
	//- End of Combine colour cluster base on distance and area difference 
	//---------------------------------------------------------------------------
	
	delete[] colour_map; colour_map=NULL;
	delete[] totalw; totalw=NULL;
	for(INT ii=0;ii<N;ii++) {delete[] variance_buffer[ii]; variance_buffer[ii];}
	delete[] variance_buffer; variance_buffer=NULL;
	delete[] variance; variance=NULL;
	
	return currentN;
}

/*---------------------------------------------------------------------------
 //-  Function Name : RegionSegmentationOptimised               
 //-  Description   : Segment an image to skin objects
 //-  Parameter     : object_map - array of skin object referenced with identifier
 red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 height - height of the image
 width - width of the image
 //-  Return Value  : number of skin object, -1 for no segmentation takes place
 //-------------------------------------------------------------------------*/
INT ImageProcessing::RegionSegmentationOptimised(unsigned char *object_map,unsigned char *red,unsigned char *green,unsigned char *blue,INT height,INT width)
{
	INT i,h,pos,hw=height*width;
	
	//	char homam[200];
	//---------------------------------------------------------------------------
	//- Obtain the LUV from the RGB colour model
	//---------------------------------------------------------------------------
	unsigned char *LUVc = new unsigned char[hw*3];
	unsigned char *luvPtr = LUVc;
	
	unsigned char *r,*b,*g;
	r = red;
	g = green;
	b = blue;
	
	
	for(i=0;i<hw;i++)
	{
		pos = 3 * (((*r++ & 0x000000FC) << 10) | ((*g++ & 0x000000FC) << 4) | ((*b++ >> 2) & 0x0000003F));
		*luvPtr++=pLUV[pos++];
		*luvPtr++=pLUV[pos++];
		*luvPtr++=pLUV[pos++];
	}
	
	//	MessageBox ( NULL , "Image Conversion: Convert RGB to LUV" , "RegionSegmentationOptimised Step 1" , NULL);
	
	//---------------------------------------------------------------------------
	//- End of Obtain the LUV from the RGB colour model
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Perform adaptive smoothing on LUV colour components
	//---------------------------------------------------------------------------
	INT offset=2; // size of the array analsed is 5x5 = (2*offset+1 x 2*offset+1)
	INT interval=3; //interval to skip pixel for fast processing
	INT weightHeight = ((height - offset - interval - 1)/interval) + 1;
	INT weightWidth = ((width - offset - interval - 1)/interval) + 1;
	INT weightHW = weightHeight*weightWidth;
	INT *weightOpt=new INT[weightHW];
	unsigned char *tmp_LUVc = new unsigned char[weightHW*3];
	INT AvgWeightOpt = AdaptSmoothOptimised(tmp_LUVc,LUVc,weightOpt,height,width,offset,interval);
	

	//---------------------------------------------------------------------------
	//- End of Perform adaptive smoothing on LUV colour components
	//---------------------------------------------------------------------------
	
	if (AvgWeightOpt<0) // not big enough for blobs.
	{
		delete[] LUVc;			LUVc=NULL;
		delete[] tmp_LUVc;	tmp_LUVc=NULL;
		delete[] weightOpt;	weightOpt=NULL;
		return -1;
	}
	
	// Update the weight by inversing the value with exponential
	for(h=0;h<weightHW;h++)
	{
		weightOpt[h] = luvEXP[weightOpt[h]];
	}
	
	// Finding the right number of colour cluster
	INT Nopt=MAX(2*AvgWeightOpt,17);
	if (Nopt>256) Nopt=256;
	
	
	//---------------------------------------------------------------------------
	//- Compute the colour cluster map
	//---------------------------------------------------------------------------
	INT **cbOpt=new INT*[256];
	for(i=0;i<256;i++) cbOpt[i]=new INT[3];
	Nopt=ColourClusteringOptimised(tmp_LUVc,weightOpt,Nopt,cbOpt,weightHeight,weightWidth);
	//---------------------------------------------------------------------------
	//- End of Compute the colour cluster map
	//---------------------------------------------------------------------------
	
	
	//---------------------------------------------------------------------------
	//- Update colour map of the image
	//---------------------------------------------------------------------------
	unsigned char *colour_map_opt=new unsigned char[hw];
	unsigned char *colour_map_opt_ptr = colour_map_opt;
	unsigned char *LUVc_ptr = LUVc;
	
	INT mindifOpt,difOpt;
	INT LUV1,LUV2,LUV3;
	for (i=0;i<hw;i++)
	{
		mindifOpt=0;
		LUV1=*LUVc_ptr++;
		LUV2=*LUVc_ptr++;
		LUV3=*LUVc_ptr++;
		
		mindifOpt+=(LUV1-cbOpt[0][0])*(LUV1-cbOpt[0][0])+(LUV2-cbOpt[0][1])*(LUV2-cbOpt[0][1])+(LUV3-cbOpt[0][2])*(LUV3-cbOpt[0][2]);
		*colour_map_opt_ptr=0;
		for (INT j=1;j<Nopt;j++)
		{
			difOpt = 0;
			difOpt+=(LUV1-cbOpt[j][0])*(LUV1-cbOpt[j][0])+(LUV2-cbOpt[j][1])*(LUV2-cbOpt[j][1])+(LUV3-cbOpt[j][2])*(LUV3-cbOpt[j][2]);
			if (difOpt<mindifOpt) {*colour_map_opt_ptr=j; mindifOpt=difOpt; }
		}
		colour_map_opt_ptr++;
	}
	//---------------------------------------------------------------------------
	//- End of Update colour map of the image
	//---------------------------------------------------------------------------
	
	//update object map
	for (i=0;i<hw;i++)
	{
		object_map[i]=colour_map_opt[i];
	}
	
	INT object_num = Nopt;
	
	//release memory
	delete[] LUVc;			LUVc=NULL;
	delete[] tmp_LUVc;		tmp_LUVc=NULL;
	delete[] weightOpt;	weightOpt=NULL;
	delete[] colour_map_opt; colour_map_opt=NULL;
	for(i=0;i<256;i++) {delete[] cbOpt[i]; cbOpt[i]=NULL;}
	delete[] cbOpt; cbOpt=NULL;
	
	////////////////////////////////////////////////////////////////////////////////////
	return object_num;
}


/*---------------------------------------------------------------------------
 //-  Function Name : QsortIndex
                
 //-  Description   : Perform sorting
 //-  Parameter     : left - smallest index of the array
 right - largest index of the array
 A - array of elements (float) to be sorted
 index - array of index
 order - 0 for descending and 1 for ascending
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::QsortIndex(INT left,INT right,FLOAT *A,INT *index,INT order)
{
	INT	i = left; 
	INT j = right;
	
	FLOAT middle = A[(left+right)/2];
	FLOAT temp;
	INT	temp2;
	
	for(;i<=j;)
	{
		if(order==1)
		{
			for (;A[i] > middle && i < right;) i++;
			for (;middle > A[j] && j > left;) j--;
		}
		else
		{
			for (;A[i] < middle && i < right;) i++;
			for (;middle < A[j] && j > left;) j--;
		}
		
		if (i<=j)
		{
			temp = A[i];
			A[i] = A[j];
			A[j] = temp;
			temp2 = index[i];
			index[i] = index[j];
			index[j] = temp2;
			i++;
			j--;
		}
	} 
	if (left<j) QsortIndex(left,j,A,index,order);
	if (i<right) QsortIndex(i,right,A,index,order);
}


/*---------------------------------------------------------------------------
 //-  Function Name : Qsort_blob_connect              
 //-  Description   : Perform sorting on array
 //-  Parameter     : left - smallest index of the array
 right - largest index of the array
 sorted_blob_connect - array data to be sorted
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::Qsort_blob_connect(INT left,INT right, INT *sorted_blob_connect)
{
	INT	i = left; 
	INT j = right;
	
	INT middle = sorted_blob_connect[(left+right)/2];
	INT temp;
	
	for(;i<=j;)
	{
		for (;sorted_blob_connect[i] < middle && i < right;) i++;
		for (;middle < sorted_blob_connect[j] && j > left;) j--;
		
		if (i<=j)
		{
			temp = sorted_blob_connect[i];
			sorted_blob_connect[i] = sorted_blob_connect[j];
			sorted_blob_connect[j] = temp;
			i++;
			j--;
		}
	} 
	if (left<j) Qsort_blob_connect(left,j,sorted_blob_connect);
	if (i<right) Qsort_blob_connect(i,right,sorted_blob_connect);
}
/*----------------------------------
 
 
 -----------------------------------------
 //-  Function Name : segmentBlobs           
 //-  Description   : Connect the pixels with the similar properties and removing small connected pixels
 //-  Parameter     : info - skin data
 blob - array data referenced to blob identifier
 height - height of the image
 width - width of the image
 blobSizeThres - threshold to identify the allowed blob area
 blobHeight - height of the largest blob
 blobWidth - width of the largest blob
 largeBlob - index of the largest blob
 //-  Return Value  : number of blob
 //-------------------------------------------------------------------------*/
INT ImageProcessing::segmentBlobs(unsigned char *info,INT *blob, INT height,INT width, INT blobSizeThres, INT &blobHeight, INT &blobWidth, INT &largeBlob)
{
	INT h,w=0, kk,i,hw;
	INT blob_number,valh,valw,val;
	INT blob_holder_0;
	INT blob_holder_1;
	INT blob_holder_2;
	INT blob_holder_3;
	INT blob_identifier;
	INT blobNumber=0;
	hw = height*width; 
	
	INT * blob_connect = new INT[hw];
	INT * sorted_blob_connect = new INT[hw];
	bool *blob_good = NULL;
	INT * blob_size = NULL;
	
	for (i=0;i<hw;i++) {blob_connect[i] = i; blob[i]=0;}
	blob_number=1;	
	INT hh=0,ww=0;
	
	for (h=0;h<hw;h++)
	{
		if (ww>0 && ww<width-1 && hh>0 && hh<height-1)
		{
			blob[h]=0;
			if (info[h]>0)
			{
				// first element
				INT number_elements=0;
				val = blob[h-1];
				if (val) 
				{
					blob_holder_0=val;
					number_elements++;
				}
				
				// second element
				val = blob[h-width-1];
				if (val) 
				{
					switch (number_elements)
					{
						case 0: 
							blob_holder_0=val;
							number_elements++;
							break;
							
						case 1:
							if (val>blob_holder_0) 
							{
								blob_holder_1=val;
								number_elements++;
							}
							else
							{
								if (val<blob_holder_0) 
								{
									blob_holder_1=blob_holder_0;
									blob_holder_0=val;
									number_elements++;
								}
							}
							break;
							
						default:
							break;
					}
				}
				
				// third element
				val = blob[h-width];
				if (val) 
				{
					switch (number_elements)
					{
						case 0: 
							blob_holder_0=val;
							number_elements++;
							break;
							
						case 1:
							if (val>blob_holder_0) 
							{
								blob_holder_1=val;
								number_elements++;
							}
							else
							{
								if (val<blob_holder_0) 
								{
									blob_holder_1=blob_holder_0;
									blob_holder_0=val;
									number_elements++;
								}
							}
							break;
							
						case 2:
							if (val>blob_holder_1)
							{
								blob_holder_2=val;
								number_elements++;
							}
							else
							{
								if (val>blob_holder_0 && val<blob_holder_1)
								{
									blob_holder_2=blob_holder_1;
									blob_holder_1=val;
									number_elements++;
								}
								else
								{
									if (val<blob_holder_0)
									{
										blob_holder_2=blob_holder_1;
										blob_holder_1=blob_holder_0;
										blob_holder_0=val;
										number_elements++;
									}
								}
							}
							break;
							
						default:
							break;
					}
				}
				
				// fourth element
				val = blob[h-width+1];
				if (val) 
				{
					switch (number_elements)
					{
						case 0: 
							blob_holder_0=val;
							number_elements++;
							break;
							
						case 1:
							if (val>blob_holder_0) 
							{
								blob_holder_1=val;
								number_elements++;
							}
							else
							{
								if (val<blob_holder_0) 
								{
									blob_holder_1=blob_holder_0;
									blob_holder_0=val;
									number_elements++;
								}
							}
							break;
							
						case 2:
							if (val>blob_holder_1)
							{
								blob_holder_2=val;
								number_elements++;
							}
							else
							{
								if (val>blob_holder_0 && val<blob_holder_1)
								{
									blob_holder_2=blob_holder_1;
									blob_holder_1=val;
									number_elements++;
								}
								else
								{
									if (val<blob_holder_0)
									{
										blob_holder_2=blob_holder_1;
										blob_holder_1=blob_holder_0;
										blob_holder_0=val;
										number_elements++;
									}
								}
							}
							break;
							
						case 3:
							if (val>blob_holder_2)
							{
								blob_holder_3=val;
								number_elements++;
							}
							else
							{
								if (val>blob_holder_1 && val<blob_holder_2)
								{
									blob_holder_3=blob_holder_2;
									blob_holder_2=val;
									number_elements++;
								}
								else
								{
									if (val>blob_holder_0 && val<blob_holder_1)
									{
										blob_holder_3=blob_holder_2;
										blob_holder_2=blob_holder_1;
										blob_holder_1=val;
										number_elements++;
									}
									else
									{	
										if (val<blob_holder_0)
										{
											blob_holder_3=blob_holder_2;
											blob_holder_2=blob_holder_1;
											blob_holder_1=blob_holder_0;
											blob_holder_0=val;
											number_elements++;
										}
									}
								}
							}
							break;
							
						default:
							break;
					}
				}
				
				if (!number_elements)
					blob[h] = blob_number++;
				else
					blob[h]=blob_holder_0;
				
				
				switch (number_elements)
				{
					case 2:
						blob_connect[blob_holder_1] = blob_holder_0;
						break;
					case 3:
						blob_connect[blob_holder_1] = blob_holder_0;
						blob_connect[blob_holder_2] = blob_holder_0;
						break;
						
					case 4:
						blob_connect[blob_holder_1] = blob_holder_0;
						blob_connect[blob_holder_2] = blob_holder_0;
						blob_connect[blob_holder_3] = blob_holder_0;
						break;
						
					default:
						break;
				}
				
			} //end if (the pixel is non-skin)
		} //end if (inside the boundary)
		
		ww++;
		if(ww>=width) {ww=0;hh++;}
	} //end for
	
	
	//connected blobs are being related 
	for (i=0;i<blob_number;i++)
	{
		for (INT j=0;j<blob_number;j++)
		{
			val = blob_connect[i];
			if (blob_connect[val]!=val)
				blob_connect[i]=blob_connect[blob_connect[val]];
			else 
				j=blob_number;
		}
	}
	
	// create a copy since array gets messed up in sorting.
	INT sorted_blob_number = blob_number;
	for (i=blob_number;i--;) 
		sorted_blob_connect[i] = blob_connect[i];
	
	// sort the list
	Qsort_blob_connect(0,sorted_blob_number-1,sorted_blob_connect);
    
	//repeated elements are removed 
	for (i=0;i<sorted_blob_number-1;i++)
	{
		if (sorted_blob_connect[i]==sorted_blob_connect[i+1])
		{
			for (INT j=i;j<sorted_blob_number-1;j++)
			{
				sorted_blob_connect[j]=sorted_blob_connect[j+1];
			}
			sorted_blob_number--;
			i--;
		}
	}
	
	// Refine the blob base on the area
	blob_size = new INT[sorted_blob_number];
	
	for (kk=0;kk<sorted_blob_number;kk++)
	{
		blob_size[kk]=0;
	}	
	
	for (h=0;h<hw;h++)
	{
		//just check for skin pixels only
		if (info[h]>0) 
		{
			blob_identifier = blob_connect[blob[h]];
			// if the boundary contains skin pixels, then blob_size[0] may have value, and if this
			// value is small, this skin pixel may change to non-skin pixel
			for (kk=0;kk<sorted_blob_number;kk++)
			{					
				if (blob_identifier==sorted_blob_connect[kk])
				{			
					blob[h]=kk;
					info[h]=kk; 
					blob_size[kk]++; 
					break;
				}															
			}
		}
	}
	
	blob_good = new bool[sorted_blob_number];
	// Determine blob that has area larger than a threshold
	for (i=0;i<sorted_blob_number;i++)
	{
		blob_good[i]=TRUE;
		if (blob_size[i] < blobSizeThres) blob_good[i]=FALSE;		
	}
	
	// Update the index of info and blob if the blob area is small
	for (h=0;h<hw;h++)
	{
		if (!blob_good[blob[h]]) //if the blob is smaller, then change the pixel in this blob to non-skin pixel
		{
			info[h]=0;
			blob[h]=0; //change the blobs to non-skin blob
		}
	}
	
	INT largest = 0; 
	// Determine the largest blob
	for ( i=0;i<sorted_blob_number;i++)
	{
		if (blob_good[i]) blobNumber++; //counting the good blob	
		
		if (largest < blob_size[i]) 
		{
			largest = blob_size[i];
			largeBlob = i;
		}
	}
	
	if(blobNumber)
	{
		INT starth=0,minw=0,maxh=0,maxw=0;
		bool once=TRUE;
		
		for (h=0;h<hw;h++)
		{
			if (blob[h]==largeBlob)
			{
				valh = h/width;
				valw = h%width;
				if (once)
				{ 
					starth=valh;
					minw=valw; 
					once = FALSE;
				}	 
				if (valw<minw) minw=valw;
				if (valw>maxw) maxw = valw;
				if (valh>maxh) maxh = valh;
			}
		}
		blobHeight = maxh-starth;
		blobWidth = maxw-minw; 
	}
	else
		largeBlob=0;
	
	delete [] blob_size;
	blob_size = NULL; //JH-M
	delete [] sorted_blob_connect;
	sorted_blob_connect = NULL; //JH-M
	delete [] blob_connect;
	blob_connect = NULL; //JH-M
//	if (blob_good!=NULL) delete[] blob_good;
	if (blob_good!=NULL) {delete[] blob_good; blob_good	= NULL;}	//JH-M
	// relabel the blobs;
	std::vector <INT> blobIndex;
	INT blobLabel = 0;
	INT lastBlob = 0;
	INT thisBlob;
	
	for (h=0;h<hw;h++)
	{
		thisBlob = blob[h];
		if (thisBlob>0)
		{
			if (thisBlob!=lastBlob)
			{
				// see if exists
				bool exists = FALSE;
				for (INT k=0;k<blobIndex.size();k++)
				{
					if (thisBlob==blobIndex.at(k))
					{
						exists = TRUE;
						blobLabel = k+1;
						k = blobIndex.size();
					}
				}
				if (!exists)
				{
					blobIndex.push_back(thisBlob);
					blobLabel = blobIndex.size();
				}
				lastBlob=thisBlob;
			}
			blob[h]=blobLabel;
			info[h]=blobLabel; 
		}
	}
	blobIndex.clear();
	
	return blobNumber;
}





/*---------------------------------------------------------------------------
 //-  Function Name : GetSimonBlobInfoOptimised
 //-  Description   : 
 //-  Parameter     : object_map - array of skin object referenced with identifier
 object_num - number of segmented object
 height - height of the image
 width - width of the image
 bw - grayscale component of the colour image
 info - skin data
 blobs - array data referenced to blob identifier
 threshold - adaptive threhold to further segment the skin
 //-  Return Value  : number of skin blob
 //-------------------------------------------------------------------------*/
INT ImageProcessing::GetSimonBlobInfoOptimised(unsigned char *object_map, INT object_num,INT height,INT width, unsigned char *bw, INT *info, INT *blobs, FLOAT &threshold)
{
	simonBlob *object_info=new struct simonBlob[object_num+1];
	
	unsigned char * om_pos = object_map;
	unsigned char * om_pos_m1 = object_map-1;
	unsigned char * om_pos_mWidth = object_map-width;
	INT * info_pos = info;
	unsigned char * bw_pos = bw;
	unsigned char * bw_pos_m1 = bw-1;
	unsigned char * bw_pos_mWidth = bw - width;
	INT seg;
	INT neighbour;
	INT hw = height*width;
	
	//---------------------------------------------------------------------------
	//- Determine the skin object properties: area, total skin, total edge, total edge value
	//---------------------------------------------------------------------------
	for (INT i = 0; i < object_num+1; i++)
	{
		object_info[i].area=0;
		object_info[i].skinTotal=0;
		object_info[i].edgeTotal=0;
		object_info[i].edgePixels=0;
		object_info[i].neighbour.clear();
	}
	
	for(INT h=0;h<height;h++)
	{
		for(INT w=0;w<width;w++)
		{
			seg = *om_pos++;
			//determine the area of the blob
			object_info[seg].area++;
			//determine the skin count for each blob segment
			if(*info_pos++==3) object_info[seg].skinTotal++;
			// find the stuff touching by going down and back	
			if (w>0)
			{
				neighbour = *om_pos_m1;
				if (seg!=neighbour)
				{
					bool alreadyMet = FALSE;
					for (INT k=0;k<object_info[seg].neighbour.size();k++)
					{
						if (object_info[seg].neighbour.at(k)==neighbour)
						{
							alreadyMet=TRUE;
							k=object_info[seg].neighbour.size();
						}
					}
					if (!alreadyMet) 
					{
						object_info[seg].neighbour.push_back(neighbour);
						object_info[neighbour].neighbour.push_back(seg);
					}
				}
				else
				{
					object_info[seg].edgePixels++;
					object_info[seg].edgeTotal+=abs(*bw_pos-*bw_pos_m1);
				}
			}
			
			if (h>1)
			{
				neighbour = *om_pos_mWidth;
				if (seg!=neighbour)
				{
					bool alreadyMet = FALSE;
					for (INT k=0;k<object_info[seg].neighbour.size();k++)
					{
						if (object_info[seg].neighbour.at(k)==neighbour)
						{
							alreadyMet=TRUE;
							k=object_info[seg].neighbour.size();
						}
					}
					if (!alreadyMet) 
					{
						object_info[seg].neighbour.push_back(neighbour);
						object_info[neighbour].neighbour.push_back(seg);
					}
				}
				else
				{
					object_info[seg].edgePixels++;
					object_info[seg].edgeTotal+=abs(*bw_pos-*bw_pos_mWidth);
				}
			}
			om_pos_m1++;
			om_pos_mWidth++;
			bw_pos++;
			bw_pos_m1++;
			bw_pos_mWidth++;
		}
	}
	//---------------------------------------------------------------------------
	//- End to Determine the skin object properties: area, total skin, total edge, total edge value
	//---------------------------------------------------------------------------
	
	
	//---------------------------------------------------------------------------
	//- Calculate adaptive threshold
	//---------------------------------------------------------------------------
	FLOAT *percentages = new FLOAT[object_num+1];
	FLOAT *areas = new FLOAT[object_num+1];
	INT *index_percentages = new INT[object_num+1];
	INT *index_ptr = index_percentages;
	
	for(INT i=0;i<object_num;i++) 
	{
		*index_ptr++=i;
		// Calculate ratio skin over area
		if (object_info[i].area>0)
		{
			percentages[i] = ((FLOAT) object_info[i].skinTotal)/((FLOAT) object_info[i].area);
		}
		else percentages[i] = 0.0;
		// Calculate ratio skin object area over image area
		areas[i] = ((float) object_info[i].area)/((float)hw);	
	}
	
	// Perform ascending sorting on the ratio skin over area
	QsortIndex(0,object_num-1,percentages,index_percentages,0);
	
	threshold = 1.0;
	FLOAT skinArea = 0.0;
	
	for (INT i = object_num - 1; i > 0; i--)
	{
		if (percentages[i]>0.20)
		{
			if (skinArea<0.4)
			{
				if (!(skinArea>0.0 && skinArea+areas[index_percentages[i]]>0.8))
				{
					if (i<object_num-1)
					{
						if (percentages[i+1]-percentages[i]<0.2 || skinArea<0.1 || percentages[i]>0.6)
						{
							threshold = percentages[i]-0.001;
							skinArea+=areas[index_percentages[i]];
						}
						else i=0;
					}
					else 
					{
						threshold = percentages[i]-0.001;
						skinArea+=areas[index_percentages[i]];
					}
				}
			}
		}
	}
	delete [] percentages;
	delete [] areas;
	delete [] index_percentages;
	//---------------------------------------------------------------------------
	//- End of Calculate adaptive threshold
	//---------------------------------------------------------------------------
	
	// the percentage of skin pixels in segment need to be > threshold to be a skin segment
	// also need a reasonable smooth segment.
	for(INT i = 0; i < object_num+1; i++)
	{
		object_info[i].skin=FALSE; 
		if (((FLOAT)object_info[i].skinTotal)/((FLOAT)object_info[i].area) > threshold) object_info[i].skin=TRUE; 
		if (object_info[i].edgePixels>0)
		{
			if (object_info[i].edgeTotal/object_info[i].edgePixels>10) object_info[i].skin=FALSE; 
		}		
	}
	
	// Now join the skin segments together
	std::vector<INT> *skinSegments = new std::vector<INT>[object_num+2];
	INT numberSkinSegments = 0;
	for(INT i = 0; i < object_num+1; i++)
	{
		if (object_info[i].skin)
		{
			numberSkinSegments++;
			object_info[i].skin=FALSE;
			skinSegments[numberSkinSegments].push_back(i);
			for (INT k=0;k<skinSegments[numberSkinSegments].size();k++)
			{
				INT checkSeg = skinSegments[numberSkinSegments].at(k);
				for (INT l=0;l<object_info[checkSeg].neighbour.size();l++)
				{
					if (object_info[object_info[checkSeg].neighbour.at(l)].skin)
					{
						skinSegments[numberSkinSegments].push_back(object_info[checkSeg].neighbour.at(l));
						object_info[object_info[checkSeg].neighbour.at(l)].skin=FALSE;
					}
				}
			}
		}
	}
	
	// now put the new segment number back INTo the object map
	INT lastSeg=object_map[0];
	INT lastSkinSeg = 0;
	for(INT i = 1; i <=numberSkinSegments; i++)
	{
		for (INT k=0;k<skinSegments[i].size();k++)
		{
			if (skinSegments[i].at(k)==lastSeg) {lastSkinSeg=i;/*k<skinSegments[i].size();*/i=numberSkinSegments+1;}
		}
	}
	
	for(INT h=0;h<hw;h++)
	{
		seg = object_map[h];
		if (seg!=lastSeg)
		{
			lastSeg=seg;
			lastSkinSeg = 0;
			for(INT i = 1; i <=numberSkinSegments; i++)
			{
				for (INT k=0;k<skinSegments[i].size();k++)
				{
					if (skinSegments[i].at(k)==lastSeg) {lastSkinSeg=i;/*k<skinSegments[i].size();*/i=numberSkinSegments+1;}
				}
			}
		}	
		object_map[h]=lastSkinSeg;
	}
	
	
	//---------------------------------------------------------------------------
	//- Connect the similar pixels INTo blob by removing blob area below blobSizeThres
	//---------------------------------------------------------------------------
	INT blobHeight,blobWidth,largeBlob;
	INT blobSizeThres = 400;
	INT numberBlobs = segmentBlobs(object_map, blobs, height, width, blobSizeThres, blobHeight, blobWidth, largeBlob);
	//---------------------------------------------------------------------------
	//- End of Connect the similar pixels INTo blob by removing blob area below blobSizeThres
	//---------------------------------------------------------------------------
	
	
	// clear the object stuff
	for(INT i = 0; i < object_num+1; i++) 
	{
		object_info[i].neighbour.clear();
	}
	delete [] object_info;
	object_info=NULL; //JH-M 
	
	for (INT h=0;h<object_num+2;h++)
	{
		skinSegments[h].clear();
	}
	delete [] skinSegments; skinSegments = NULL;
	
	return numberBlobs;
}



/*---------------------------------------------------------------------------
 //-  Function Name : ObtainFaceInfoOptimised
 //-  Description   : Determine face to indicate portrait and shoulder portrait
 //-  Parameter     : info - skin data
 hbmp - hbitmap image
 height - height of the image
 width - width of the image
 portraitindicator - TRUE if portrait detected, FALSE if otherwise
 manyfacesindicator - TRUE if manyfaces detected, FALSE if otherwise
 shoulderportraitindicator - TRUE if shoulderportraitindicator detected, FALSE if otherwise 
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ObtainFaceInfoOptimised(INT *info,
                                                uchar* hbmp,
                                                INT height,
                                                INT width,
                                                bool &portraitindicator,
                                                bool &manyfacesindicator,
                                                bool &shoulderportraitindicator,
                                                EN_FSIP_ENGINE_TYPE enEngine_Type,
												INT Channels,
												INT Depth
												)
{
	
	if(_cascade.empty()) return 0;
	
	
	
	bool portraitflag=FALSE;
	INT h,w,hw=height*width;
	uchar* hbmWhite = NULL;
	
//	CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp , width , height , Depth , Channels);
	
	CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp , width , height , Depth , Channels);
	INT iWhiteBorderVal =(width/10);
	INT inHeight = height + iWhiteBorderVal;
	INT inWidth = width + iWhiteBorderVal;
	 
	 IplImage *img = cvCreateImage(cvSize(inWidth, inHeight),Depth,Channels);

	 memcpy(img->imageData ,hbmWhite , img->imageSize);
	
    cv::Mat _img = cv::cvarrToMat(img);
	
	if(!_cascade.empty())
	{
		CvMemStorage* storage = cvCreateMemStorage(0);
		cvClearMemStorage(storage);
		CvSeq* faces;
        vector<cv::Rect> _faces;
        INT scale = 1;
		INT bodysize,bodyskin;
		
		if(enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
            //faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2, 0);//CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,
                                       1.15, 2, 0
                                       //|CASCADE_FIND_BIGGEST_OBJECT
                                       //|CASCADE_DO_ROUGH_SEARCH
                                       //|CASCADE_DO_CANNY_PRUNING
                                       );
        else
			//faces = cvHaarDetectObjects( img, _cascade, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,1.10, 2, 0);
		
		if (_faces.size()>0) //   faces->total>0)
		{
			//////////////////////additional add in
			bool belowface=FALSE;  //new added
			INT belowfacecount=0;	//new added
			//////////////////////
			struct FaceRec sFace;
			
			INT xx,yy,ww,hh;
			INT fsizeThres=35; //for portrait checking purpose in terms of size
			INT fheightThres=40;
			INT fwidthThres=45;
           // for( vector<cv::Rect>::const_iterator r = _faces.begin(); r != _faces.end(); r++)
			for(INT i=0;i<_faces.size();i++)  //(INT i=0;i<faces->total;i++) change updates to opencv4
			{
				xx=0,yy=0,ww=0,hh=0;
			//	CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                CvRect face_rect = _faces[i]; //update to opencv4
				xx=face_rect.x*scale; yy=face_rect.y*scale;
				ww=face_rect.width*scale; hh=face_rect.height*scale;
				//Determine the x,y,height and width of the face
				sFace.x=xx;
				sFace.height=hh;
				sFace.y=height-yy-hh;
				sFace.width=ww;
				if(sFace.x<0) sFace.x=0;
				if(sFace.x>width-1) sFace.x=width-1;
				if(sFace.height<0) sFace.height=0;
				if(sFace.height>height-1) sFace.height=height-1;
				if(sFace.y<0) sFace.y=0;
				if(sFace.y>height-1) sFace.y=height-1;
				if(sFace.width<0) sFace.width=0;
				if(sFace.width>width-1) sFace.width=width-1;
				
				//determine whether it is portrait or not
				if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
				{
					//MessageBox(NULL,"Portrait based on face size and dimension","",0);
					FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
					cvReleaseMemStorage(&storage);
					cvReleaseImage(&img);
					portraitflag=TRUE;
					portraitindicator=TRUE;
					if (hbmWhite)
					{
						free(hbmWhite);
						hbmWhite = NULL;
					}
					return portraitflag;
				}
				
				//Determine the x,y,height and width of the body trunk
				if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
				{
					sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
					sFace.bheight=sFace.height/3*13;
				}
				else
				{
					sFace.by=0;
					sFace.bheight=sFace.y;
				}
				
				if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
					sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
				else
					sFace.bx=0;
				if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
					sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
				else
					sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
				
				//Determine the boob area
				if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
					sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
				else
					sFace.boobydown=sFace.by;
				
				if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
				{
					sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
					sFace.boob=TRUE;
				}
				else
				{
					sFace.boob=FALSE;
				}
				
				//Determine the private part area
				if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
				{
					sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
					sFace.ppart=TRUE;
				}
				else
				{
					sFace.ppart=FALSE;
				}
				
				if(sFace.ppart==TRUE)
				{
					if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
						sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
					else
						sFace.ppartydown=sFace.by;
				}
				
				//Determine it is a valid body structure or not
				//by calculating the amount of skin inside the body structure
				bodysize=0;bodyskin=0;
				bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
				sFace.area=bodysize;
				sFace.bodyarea=bodysize-(sFace.height*sFace.width);
				sFace.facearea=sFace.height*sFace.width;
				//face portion
				INT hEnd = sFace.y+sFace.height;
				INT wEnd = sFace.x+sFace.width;
				
				for(h=sFace.y;h<hEnd;h++) //new added remove =
				{
					INT h_width = h*width;
					INT *info_ptr = info+h_width+sFace.x;
					for(w=sFace.x;w<wEnd;w++) //new added remove =
					{
						if(*info_ptr++==3) bodyskin++;
						
					}
				}
				
				//body trunk portion
				hEnd = sFace.by+sFace.bheight;
				wEnd = sFace.bx+sFace.bwidth;
				for(h=sFace.by;h<hEnd;h++) //new added remove =
				{
					for(w=sFace.bx;w<wEnd;w++) //new added remove =
					{
						INT h_width = h*width;
						INT *info_ptr = info+h_width+sFace.bx;
						if(*info_ptr++==3) bodyskin++;
					}
				}
				if((FLOAT)bodyskin/bodysize*100>5) //previous is 10
				{
					if(sFace.bheight>=3*sFace.height/3) //check shoulder portrait
					{
						FaceAttr.push_back(sFace);
					}
					else
					{
						belowfacecount=0;
						for(INT hh=sFace.y;hh<sFace.y+sFace.height;hh++)
						{
							if(hh<height/3) belowfacecount++;
						}
						if((FLOAT)belowfacecount/sFace.height*100>50) belowface=TRUE;
					} //new added
				}
			} //for loop for number of faces
			
		} //if there exist face
		cvReleaseMemStorage(&storage);
	}
	
	cvReleaseImage(&img);
	if (hbmWhite)
	{
		free(hbmWhite);
		hbmWhite = NULL;
	}
	//	delete hbmWhite;
	//	hbmWhite = NULL;
	////////////////////////////////////////////////////////////////////////////////////
	return portraitflag;
}


bool ImageProcessing::ObtainFaceInfoOptimised_V3_6(INT *info,	//JH
											  uchar* hbmp,
											  INT height,
											  INT width,
											  bool &portraitindicator,
											  bool &manyfacesindicator,
											  bool &shoulderportraitindicator,
											  EN_FSIP_ENGINE_TYPE enEngine_Type,
											  INT Channels,
											  INT Depth
											  )
{
	if(_cascade.empty()) return 0;
	
	bool portraitflag=FALSE;
	INT h,w,hw=height*width;
	uchar* hbmWhite = NULL;
	//CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp);
	CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp , width , height , Depth , Channels);
	INT iWhiteBorderVal =(width/10);

	INT inHeight = height + iWhiteBorderVal;
	INT inWidth = width + iWhiteBorderVal;
    
    vector<cv::Rect> _faces;
	
	
	IplImage *img = cvCreateImage(cvSize(inWidth, inHeight),Depth,Channels); 
	bool bload=memcpy(img->imageData ,hbmWhite , img->imageSize);
    
    cv::Mat _img = cv::cvarrToMat(img);
	CvPoint pt1, pt2;

	//cvSaveImage("/emad/testhere2.jpg", img, 0);*/
	
	if (bload)
	{
		if(!_cascade.empty())
		{
			CvMemStorage* storage = cvCreateMemStorage(0);
			cvClearMemStorage(storage);
			CvSeq* faces;
			INT scale = 1;
			INT bodysize,bodyskin;
		//	faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2, CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,
                                      1.15, 2, 0
                                      //|CASCADE_FIND_BIGGEST_OBJECT
                                      //|CASCADE_DO_ROUGH_SEARCH
                                      //|CASCADE_DO_CANNY_PRUNING
                                      );
			if (_faces.size()>0)//(faces->total>0)
			{
				//////////////////////additional add in
				bool belowface=FALSE;  //new added
				INT belowfacecount=0;	//new added
				//////////////////////
				struct FaceRec sFace;
				
				INT xx,yy,ww,hh;
				INT fsizeThres=35; //for portrait checking purpose in terms of size
				INT fheightThres=40;
				INT fwidthThres=45;
				for(INT i=0;i<_faces.size();i++)//(INT i=0;i<faces->total;i++)
				{
					xx=0,yy=0,ww=0,hh=0;
					//CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                    CvRect face_rect = _faces[i]; //added update to opencv4
					xx=face_rect.x*scale; yy=face_rect.y*scale;
					ww=face_rect.width*scale; hh=face_rect.height*scale;
					//Determine the x,y,height and width of the face
					sFace.x=xx-(img->width/20); //New Engine to correct the location of the Corrdinates after adding the new Border
					sFace.height=hh;
					sFace.y=(height-yy-hh)- (img->width)/20; //New Engine to correct the location of the Corrdinates after adding the new Border
					sFace.width=ww;
					if(sFace.x<0) sFace.x=0;
					if(sFace.x>width-1) sFace.x=width-1;
					if(sFace.height<0) sFace.height=0;
					if(sFace.height>height-1) sFace.height=height-1;
					if(sFace.y<0) sFace.y=0;
					if(sFace.y>height-1) sFace.y=height-1;
					if(sFace.width<0) sFace.width=0;
					if(sFace.width>width-1) sFace.width=width-1;
					pt1.x = face_rect.x*scale;
					pt2.x = (face_rect.x+face_rect.width)*scale;
					pt1.y = face_rect.y*scale;
					pt2.y = (face_rect.y+face_rect.height)*scale;
				
					
					//determine whether it is portrait or not
					if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
					{
						
						if (ShowDetailedImage == TRUE)
						{
							char WinName[16]; //Showdetailedimage
							char PreWinName[16];
							sprintf (WinName , "image%d" , NumThumbs);
							sprintf (PreWinName , "Image %d - Face" , NumThumbs);
							
							pt1.x = face_rect.x*scale;
							pt2.x = (face_rect.x+face_rect.width)*scale;
							pt1.y = face_rect.y*scale;
							pt2.y = (face_rect.y+face_rect.height)*scale;
							
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
				//JHR			cvShowImage(PreWinName , img);
				//JHR			INT Wloc = 20 + (img->width-0.1*img->width)*3;
				//JHR			cvMoveWindow(PreWinName, Wloc , 10);
						}
						//MessageBox(NULL,"Portrait based on face size and dimension","",0);
						FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
						cvReleaseMemStorage(&storage);
						cvReleaseImage(&img);
						portraitflag=TRUE;
						portraitindicator=TRUE;
						if (hbmWhite)
						{
						//	DeleteObject(hbmWhite);
							delete[] hbmWhite;
							hbmWhite = NULL;
						}
						return portraitflag;
					}
					
					//Determine the x,y,height and width of the body trunk
					if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
					{
						sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
						sFace.bheight=sFace.height/3*13;
					}
					else
					{
						sFace.by=0;
						sFace.bheight=sFace.y;
					}
					
					if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
						sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
					else
						sFace.bx=0;
					if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
						sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
					else
						sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
					
					//Determine the boob area
					if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
						sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
					else
						sFace.boobydown=sFace.by;
					
					if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
					{
						sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
						sFace.boob=TRUE;
					}
					else
					{
						sFace.boob=FALSE;
					}
					
					//Determine the private part area
					if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
					{
						sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
						sFace.ppart=TRUE;
					}
					else
					{
						sFace.ppart=FALSE;
					}
					
					if(sFace.ppart==TRUE)
					{
						if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
							sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
						else
							sFace.ppartydown=sFace.by;
					}
					
					//Determine it is a valid body structure or not
					//by calculating the amount of skin inside the body structure
					bodysize=0;bodyskin=0;
					bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
					sFace.area=bodysize;
					sFace.bodyarea=bodysize-(sFace.height*sFace.width);
					sFace.facearea=sFace.height*sFace.width;
					//face portion
					INT hEnd = sFace.y+sFace.height;
					INT wEnd = sFace.x+sFace.width;
					
					for(h=sFace.y;h<hEnd;h++) //new added remove =
					{
						INT h_width = h*width;
						INT *info_ptr = info+h_width+sFace.x;
						for(w=sFace.x;w<wEnd;w++) //new added remove =
						{
							if(*info_ptr++==3) bodyskin++;
							
						}
					}
					
					//body trunk portion
					hEnd = sFace.by+sFace.bheight;
					wEnd = sFace.bx+sFace.bwidth;
					for(h=sFace.by;h<hEnd;h++) //new added remove =
					{
						for(w=sFace.bx;w<wEnd;w++) //new added remove =
						{
							INT h_width = h*width;
							INT *info_ptr = info+h_width+sFace.bx;
							if(*info_ptr++==3) bodyskin++;
						}
					}
					if((FLOAT)bodyskin/bodysize*100>5) //previous is 10
					{
						if(sFace.bheight>=3*sFace.height/3) //check shoulder portrait
						{
							FaceAttr.push_back(sFace);
						}
						else
						{
							belowfacecount=0;
							for(INT hh=sFace.y;hh<sFace.y+sFace.height;hh++)
							{
								if(hh<height/3) belowfacecount++;
							}
							if((FLOAT)belowfacecount/sFace.height*100>50) belowface=TRUE;
						} //new added
					}
				} //for loop for number of faces
				
			} //if there exist face
			cvReleaseMemStorage(&storage);
		}
	} //end of check whether the loading of HBITMAP to IplImage successful or not
	
	cvReleaseImage(&img);
	if (hbmWhite)
	{
	//	DeleteObject(hbmWhite);
		delete[] hbmWhite;
		hbmWhite = NULL;
	}
	
	return portraitflag;
}

bool ImageProcessing::ObtainFaceInfoOptimised_V3_6_MAC(INT *info,	
												   uchar* hbmp,
												   INT height,
												   INT width,
												   bool &portraitindicator,
												   bool &manyfacesindicator,
												   bool &shoulderportraitindicator,
												   EN_FSIP_ENGINE_TYPE enEngine_Type,
												   INT Channels,
												   INT Depth
												   )
{
	if(_cascade.empty()) return 0;
	
	bool portraitflag=FALSE;
	INT h,w,hw=height*width;
	uchar* hbmWhite = NULL;
    
    vector<cv::Rect> _faces;
	//CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp);
	CMBitmapProcessing::AddWhiteBorder(&hbmWhite, hbmp , width , height , Depth , Channels);
	INT iWhiteBorderVal =(width/10);
	
	INT inHeight = height + iWhiteBorderVal;
	INT inWidth = width + iWhiteBorderVal;
	
	IplImage *img = cvCreateImage(cvSize(inWidth, inHeight),Depth,Channels); 
	memcpy(img->imageData ,hbmWhite , img->imageSize);
    
    cv::Mat _img = cv::cvarrToMat(img);
	
	CvPoint pt1, pt2;
	
	
		if(!_cascade.empty())
		{
			CvMemStorage* storage = cvCreateMemStorage(0);
			cvClearMemStorage(storage);
			CvSeq* faces;
			INT scale = 1;
			INT bodysize,bodyskin;
		//	faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2,CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,
                                      1.15, 2, 0
                                      //|CASCADE_FIND_BIGGEST_OBJECT
                                      //|CASCADE_DO_ROUGH_SEARCH
                                      //|CASCADE_DO_CANNY_PRUNING
                                      );

            
			if (_faces.size()>0)//(faces->total>0)
			{
				//////////////////////additional add in
				bool belowface=FALSE;  //new added
				INT belowfacecount=0;	//new added
				//////////////////////
				struct FaceRec sFace;
				
				INT xx,yy,ww,hh;
				INT fsizeThres=35; //for portrait checking purpose in terms of size
				INT fheightThres=40;
				INT fwidthThres=45;
				for(INT i=0;i<_faces.size();i++)//for(INT i=0;i<faces->total;i++)
				{
					xx=0,yy=0,ww=0,hh=0;
					//CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                    CvRect face_rect = _faces[i];
					xx=face_rect.x*scale; yy=face_rect.y*scale;
					ww=face_rect.width*scale; hh=face_rect.height*scale;
					//Determine the x,y,height and width of the face
					sFace.x=xx-(img->width/20); //New Engine to correct the location of the Corrdinates after adding the new Border
					sFace.height=hh;
				//	sFace.y=(height-yy-hh)- (img->width)/20; //New Engine to correct the location of the Corrdinates after adding the new Border
					sFace.y= yy-(img->width)/20;
					sFace.width=ww;
					if(sFace.x<0) sFace.x=0;
					if(sFace.x>width-1) sFace.x=width-1;
					if(sFace.height<0) sFace.height=0;
					if(sFace.height>height-1) sFace.height=height-1;
					if(sFace.y<0) sFace.y=0;
					if(sFace.y>height-1) sFace.y=height-1;
					if(sFace.width<0) sFace.width=0;
					if(sFace.width>width-1) sFace.width=width-1;
					pt1.x = face_rect.x*scale;
					pt2.x = (face_rect.x+face_rect.width)*scale;
					pt1.y = face_rect.y*scale;
					pt2.y = (face_rect.y+face_rect.height)*scale;
					
					
					//determine whether it is portrait or not
					if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
					{
						
						if (ShowDetailedImage == TRUE)
						{
							char WinName[16]; //Showdetailedimage
							char PreWinName[16];
							sprintf (WinName , "image%d" , NumThumbs);
							sprintf (PreWinName , "Image %d - Face" , NumThumbs);
							
							pt1.x = face_rect.x*scale;
							pt2.x = (face_rect.x+face_rect.width)*scale;
							pt1.y = face_rect.y*scale;
							pt2.y = (face_rect.y+face_rect.height)*scale;
							
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
							//JHR			cvShowImage(PreWinName , img);
							//JHR			INT Wloc = 20 + (img->width-0.1*img->width)*3;
							//JHR			cvMoveWindow(PreWinName, Wloc , 10);
						}
						//MessageBox(NULL,"Portrait based on face size and dimension","",0);
						FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
						cvReleaseMemStorage(&storage);
						cvReleaseImage(&img);
						portraitflag=TRUE;
						portraitindicator=TRUE;
						if (hbmWhite)
						{
							//	DeleteObject(hbmWhite);
							delete[] hbmWhite;
							hbmWhite = NULL;
						}
						return portraitflag;
					}
					
					//Determine the x,y,height and width of the body trunk
					if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
					{
						sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
						sFace.bheight=sFace.height/3*13;
					}
					else
					{
						sFace.by=0;
						sFace.bheight=sFace.y;
					}
					
					if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
						sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
					else
						sFace.bx=0;
					if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
						sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
					else
						sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
					
					//Determine the boob area
					if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
						sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
					else
						sFace.boobydown=sFace.by;
					
					if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
					{
						sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
						sFace.boob=TRUE;
					}
					else
					{
						sFace.boob=FALSE;
					}
					
					//Determine the private part area
					if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
					{
						sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
						sFace.ppart=TRUE;
					}
					else
					{
						sFace.ppart=FALSE;
					}
					
					if(sFace.ppart==TRUE)
					{
						if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
							sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
						else
							sFace.ppartydown=sFace.by;
					}
					
					//Determine it is a valid body structure or not
					//by calculating the amount of skin inside the body structure
					bodysize=0;bodyskin=0;
					bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
					sFace.area=bodysize;
					sFace.bodyarea=bodysize-(sFace.height*sFace.width);
					sFace.facearea=sFace.height*sFace.width;
					//face portion
					INT hEnd = sFace.y+sFace.height;
					INT wEnd = sFace.x+sFace.width;
					
					for(h=sFace.y;h<hEnd;h++) //new added remove =
					{
						INT h_width = h*width;
						INT *info_ptr = info+h_width+sFace.x;
						for(w=sFace.x;w<wEnd;w++) //new added remove =
						{
							if(*info_ptr++==3) bodyskin++;
							
						}
					}
					
					//body trunk portion
					hEnd = sFace.by+sFace.bheight;
					wEnd = sFace.bx+sFace.bwidth;
					for(h=sFace.by;h<hEnd;h++) //new added remove =
					{
						for(w=sFace.bx;w<wEnd;w++) //new added remove =
						{
							INT h_width = h*width;
							INT *info_ptr = info+h_width+sFace.bx;
							if(*info_ptr++==3) bodyskin++;
						}
					}
					if((FLOAT)bodyskin/bodysize*100>5) //previous is 10
					{
						if(sFace.bheight>=3*sFace.height/3) //check shoulder portrait
						{
							FaceAttr.push_back(sFace);
						}
						else
						{
							belowfacecount=0;
							for(INT hh=sFace.y;hh<sFace.y+sFace.height;hh++)
							{
								if(hh<height/3) belowfacecount++;
							}
							if((FLOAT)belowfacecount/sFace.height*100>50) belowface=TRUE;
						} //new added
					}
				} //for loop for number of faces
				
			} //if there exist face
			cvReleaseMemStorage(&storage);
		}
	
	cvReleaseImage(&img);
	if (hbmWhite)
	{
		//	DeleteObject(hbmWhite);
		delete[] hbmWhite;
		hbmWhite = NULL;
	}
	
	return portraitflag;
}

/*---------------------------------------------------------------------------
 //-  Function Name : ObtainFaceInfoOptimised1

 //-  Description   : Determine face to indicate portrait and shoulder portrait
 //-  Parameter     : info - skin data
 hbmp - hbitmap image
 height - height of the image
 width - width of the image
 portraitindicator - TRUE if portrait detected, FALSE if otherwise
 manyfacesindicator - TRUE if manyfaces detected, FALSE if otherwise
 shoulderportraitindicator - TRUE if shoulderportraitindicator detected, FALSE if otherwise 
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
bool ImageProcessing::ObtainFaceInfoOptimised1(INT *info,
												 uchar* hbmp,
												 INT height,
												 INT width,
												 bool &portraitindicator,
												 bool &manyfacesindicator,
												 bool &shoulderportraitindicator,
												 EN_FSIP_ENGINE_TYPE enEngine_Type,
												 INT Channels,
												 INT Depth)
{
	
	if(_cascade.empty()) return 0;
	
	bool portraitflag=FALSE;
    vector<cv::Rect> _faces;
	INT h,w,hw=height*width;
	// Image datatype for OpenCV
	IplImage *img = cvCreateImage(cvSize(width, height),Depth,Channels); 
	memcpy(img->imageData ,hbmp , img->imageSize);
    cv::Mat _img = cv::cvarrToMat(img);
	
	
	
		if(!_cascade.empty())
		{
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* faces;
			INT scale = 1;
			INT bodysize,bodyskin;
			// Different threshold is use for Safest and Safe Engine, 1.15 and 1.10 respectively
			if(enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
				//faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2, CV_HAAR_DO_CANNY_PRUNING );
                _cascade.detectMultiScale( _img, _faces,
                                          1.15, 2, 0
                                          //|CASCADE_FIND_BIGGEST_OBJECT
                                          //|CASCADE_DO_ROUGH_SEARCH
                                          //|CASCADE_DO_CANNY_PRUNING
                                          );
                
			else
                _cascade.detectMultiScale( _img, _faces,1.10, 2, 0);
		//		faces = cvHaarDetectObjects( img, cascade, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
            			
			//if number of face is more than two, a group photo
			if (_faces.size()>2)//(faces->total>2)
			{
				FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
				cvReleaseMemStorage(&storage);
				cvReleaseImage(&img);
				portraitflag=TRUE;
				manyfacesindicator=TRUE;
				return portraitflag;
			}
			
			if (_faces.size()>0)//(faces->total>0)
			{
				//////////////////////additional add in
				bool belowface=FALSE;  //new added
				INT belowfacecount=0;	//new added
				//////////////////////
				struct FaceRec sFace;
				
				INT xx,yy,ww,hh;
				INT fsizeThres=35; //for portrait checking purpose in terms of size
				INT fheightThres=45;
				for(INT i=0;i<_faces.size();i++)//for(INT i=0;i<faces->total;i++)
				{
					xx=0,yy=0,ww=0,hh=0;
					//CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                    CvRect face_rect = _faces[i]; //added update opencv4
					xx=face_rect.x*scale; yy=face_rect.y*scale;
					ww=face_rect.width*scale; hh=face_rect.height*scale;
					//Determine the x,y,height and width of the face
					sFace.x=xx;
					sFace.height=hh;
					sFace.y=height-yy-hh;
					sFace.width=ww;
					
					if(sFace.x<0) sFace.x=0;
					if(sFace.x>width-1) sFace.x=width-1;
					if(sFace.height<0) sFace.height=0;
					if(sFace.height>height-1) sFace.height=height-1;
					if(sFace.y<0) sFace.y=0;
					if(sFace.y>height-1) sFace.y=height-1;
					if(sFace.width<0) sFace.width=0;
					if(sFace.width>width-1) sFace.width=width-1;
					
					//determine whether it is portrait or not
					if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
					{
						//MessageBox(NULL,"Portrait based on face size and dimension","",0);
						FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
						cvReleaseMemStorage(&storage);
						cvReleaseImage(&img);
						portraitflag=TRUE;
						portraitindicator=TRUE;
						return portraitflag;
					}
					
					//Determine the x,y,height and width of the body trunk
					if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
					{
						sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
						sFace.bheight=sFace.height/3*13;
					}
					else
					{
						sFace.by=0;
						sFace.bheight=sFace.y;
					}
					
					if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
						sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
					else
						sFace.bx=0;
					if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
						sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
					else
						sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
					
					//Determine the boob area
					if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
						sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
					else
						sFace.boobydown=sFace.by;
					
					if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
					{
						sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
						sFace.boob=TRUE;
					}
					else
					{
						sFace.boob=FALSE;
					}
					
					//Determine the private part area
					if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
					{
						sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
						sFace.ppart=TRUE;
					}
					else
					{
						sFace.ppart=FALSE;
					}
					
					if(sFace.ppart==TRUE)
					{
						if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
							sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
						else
							sFace.ppartydown=sFace.by;
					}
					
					//Determine it is a valid body structure or not
					//by calculating the amount of skin inside the body structure
					bodysize=0;bodyskin=0;
					bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
					sFace.area=bodysize;
					sFace.bodyarea=bodysize-(sFace.height*sFace.width);
					sFace.facearea=sFace.height*sFace.width;
					//face portion
					INT hEnd = sFace.y+sFace.height;
					INT wEnd = sFace.x+sFace.width;
					
					for(h=sFace.y;h<hEnd;h++) //new added remove =
					{
						INT h_width = h*width;
						INT *info_ptr = info+h_width+sFace.x;
						for(w=sFace.x;w<wEnd;w++) //new added remove =
						{
							if(*info_ptr++==3) bodyskin++;
						}
					}
					//body trunk portion
					hEnd = sFace.by+sFace.bheight;
					wEnd = sFace.bx+sFace.bwidth;
					for(h=sFace.by;h<hEnd;h++) //new added remove =
					{
						for(w=sFace.bx;w<wEnd;w++) //new added remove =
						{
							INT h_width = h*width;
							INT *info_ptr = info+h_width+sFace.bx;
							if(*info_ptr++==3) bodyskin++;
						}
					}
					if((FLOAT)bodyskin/bodysize*100>5) //previous is 10
					{
						if(sFace.bheight>=3*sFace.height/3) //check shoulder portrait
						{
							FaceAttr.push_back(sFace);
						}
						else
						{
							belowfacecount=0;
							for(INT hh=sFace.y;hh<sFace.y+sFace.height;hh++)
							{
								if(hh<height/3) belowfacecount++;
							}
							if((FLOAT)belowfacecount/sFace.height*100>50) belowface=TRUE;
						} //new added
					}
				} //for loop for number of faces
				
			} //if there exist face
			cvReleaseMemStorage(&storage);
		}
		
	cvReleaseImage(&img);
	////////////////////////////////////////////////////////////////////////////////////
	return portraitflag;
}

bool ImageProcessing::ObtainFaceInfoOptimised1_V3_6(INT *info,
													uchar* hbmp,
													INT height,
													INT width,
													bool &portraitindicator,
													bool &manyfacesindicator,
													bool &shoulderportraitindicator,
													EN_FSIP_ENGINE_TYPE enEngine_Type,
													INT &profileindicator,
													INT Channels,
													INT Depth,
													INT bpp)
{
	
	//if(!cascade) return 0;
	//if(!cascade2) return 0;  // New FaceModified
	//if(!cascade3) return 0;  // New FaceModified
    if(_cascade.empty()) return 0;
    if(_cascade2.empty()) return 0;
    if(_cascade3.empty()) return 0;
	
	bool portraitflag=FALSE;
	INT h,w,hw=height*width;
    
    vector<cv::Rect> _faces;
	// Image datatype for OpenCV
	CvPoint pt1, pt2;
	//V3.5	//JH
	IplImage *img = cvCreateImage(cvSize(width, height),Depth,Channels); 
	bool bload=memcpy(img->imageData ,hbmp , img->imageSize);
    cv::Mat _img = cv::cvarrToMat(img); //added updating to opencv4

	bool BCascade1 = false;
	bool BCascade2 = false;
	bool BCascade3 = false;
	
	if(_cascade.empty()) return 0;
	
	if (bload)
	{
		if(!_cascade.empty())
		{
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* faces;
			INT scale = 1;
			INT bodysize,bodyskin;
			//faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2, CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,1.15, 2, 0);
			if (_faces.size()>0)//(faces->total > 0)
			{
				BCascade1 = true;
				profileindicator = 1 ;
			}
			
			if (_faces.size() <= 0)//(faces->total <= 0)  // New FaceModified
			{
				//faces = cvHaarDetectObjects( img, cascade2, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
                _cascade2.detectMultiScale( _img, _faces,1.10, 2, 0);
				
				if (_faces.size()>0) //(faces->total > 0)
				{
					profileindicator = 2  ;
					BCascade2 = true;
				}
				
				if (_faces.size() <= 0) // (faces->total <= 0)
				{
					//faces = cvHaarDetectObjects( img, cascade3, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
                    _cascade3.detectMultiScale( _img, _faces,1.10, 2, 0);
				}
				
				if (_faces.size()>0) //(faces->total > 0)
				{
					profileindicator = 3;
					BCascade3 = true;
				}
			}
			
			if (_faces.size()>0) //(faces->total>0)
			{
				//////////////////////additional add in
				bool belowface=FALSE;  //new added
				INT belowfacecount=0;	//new added
				//////////////////////
				struct FaceRec sFace;
				
				INT xx,yy,ww,hh;
				INT fsizeThres=35; //for portrait checking purpose in terms of size
				INT fheightThres=45;
				INT fwidthThres=45;
				for(INT i=0;i<_faces.size();i++)//for(INT i=0;i<faces->total;i++)
				{
					xx=0,yy=0,ww=0,hh=0;
					//CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                    CvRect face_rect = _faces[i]; //update for opencv4
					xx=face_rect.x*scale; yy=face_rect.y*scale;
					ww=face_rect.width*scale; hh=face_rect.height*scale;
					//Determine the x,y,height and width of the face
					sFace.x=xx;
					sFace.height=hh;
					sFace.y=height-yy-hh;
					sFace.width=ww;
					
					if(sFace.x<0) sFace.x=0;
					if(sFace.x>width-1) sFace.x=width-1;
					if(sFace.height<0) sFace.height=0;
					if(sFace.height>height-1) sFace.height=height-1;
					if(sFace.y<0) sFace.y=0;
					if(sFace.y>height-1) sFace.y=height-1;
					if(sFace.width<0) sFace.width=0;
					if(sFace.width>width-1) sFace.width=width-1;
					
					pt1.x = face_rect.x*scale;
					pt2.x = (face_rect.x+face_rect.width)*scale;
					pt1.y = face_rect.y*scale;
					pt2.y = (face_rect.y+face_rect.height)*scale;
					
					if (sFace.x > ((2*width)/3) && (width > height) ) // New Engine
					{
						sFace.RightSide = TRUE;
					}
					else 
					{
						sFace.RightSide = FALSE;
					}
					
					if (sFace.x < (width /3) && (width > height)) // New Engine
					{
						sFace.LeftSide = TRUE;
					}
					else 
					{
						sFace.LeftSide = FALSE;
					}
					
					
		
					
					//determine whether it is portrait or not
					if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
					{
						if (ShowDetailedImage == TRUE)
						{
							char WinName[16]; // ShowDetailedImage
							char PreWinName[16];  // ShowDetailedImage
							pt1.x = face_rect.x*scale;
							pt2.x = (face_rect.x+face_rect.width)*scale;
							pt1.y = face_rect.y*scale;
							pt2.y = (face_rect.y+face_rect.height)*scale;
							
							sprintf (WinName , "image%d" , NumThumbs);
							sprintf (PreWinName , "Image %d - Face" , NumThumbs);
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
				
							if (sFace.RightSide == TRUE)
							{
								pt1.x = 10;
								pt2.x = sFace.x;
								pt1.y = height - (sFace.y + (sFace.height /2));
								
								if ((sFace.y - (int)(sFace.height*1.5))>0)
									pt2.y = height - ((sFace.y - (int)(sFace.height*1.5)));
								else pt2.y = 0;
								
							}
							else if (sFace.LeftSide == TRUE)
							{
								pt1.x = sFace.x + sFace.width;
								pt2.x = width -10;
								pt1.y = height - (sFace.y + (sFace.height /2));
								
								if ((sFace.y - (int)(sFace.height*1.5)) > 0)
									pt2.y = height - ((sFace.y - (int)(sFace.height*1.5)));
								else pt2.y = 0;
								
								
							}
							
							else 
							{
								pt1.x = sFace.bx;
								pt2.x =  sFace.bx+ sFace.bwidth;
								pt1.y = height - sFace.by - sFace.bheight*0.3 ;
								pt2.y = height - (sFace.by+sFace.bheight );
							}
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
							
						}
						FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
						cvReleaseMemStorage(&storage);
						cvReleaseImage(&img);
						portraitflag=TRUE;
						portraitindicator=TRUE;
						return portraitflag;
					}
					
					//Determine the x,y,height and width of the body trunk
					if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
					{
						sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
						sFace.bheight=sFace.height/3*13;
					}
					else
					{
						sFace.by=0;
						sFace.bheight=sFace.y;
					}
					
					if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
						sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
					else
						sFace.bx=0;
					if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
						sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
					else
						sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
					
					//Determine the boob area
					if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
						sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
					else
						sFace.boobydown=sFace.by;
					
					if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
					{
						sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
						sFace.boob=TRUE;
					}
					else
					{
						sFace.boob=FALSE;
					}
					
					//Determine the private part area
					if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
					{
						sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
						sFace.ppart=TRUE;
					}
					else
					{
						sFace.ppart=FALSE;
					}
					
					if(sFace.ppart==TRUE)
					{
						if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
							sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
						else
							sFace.ppartydown=sFace.by;
					}
					//Determine it is a valid body structure or not
					//by calculating the amount of skin inside the body structure
					bodysize=0;bodyskin=0;
					bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
					sFace.area=bodysize;
					sFace.bodyarea=bodysize-(sFace.height*sFace.width);
					sFace.facearea=sFace.height*sFace.width;
					FaceAttr.push_back(sFace);
				} //for loop for number of faces
				
			} //if there exist face
			cvReleaseMemStorage(&storage);
		}
	} //end of check whether the loading of HBITMAP to IplImage successful or not
	
	cvReleaseImage(&img);
	////////////////////////////////////////////////////////////////////////////////////
	return portraitflag;
}

bool ImageProcessing::ObtainFaceInfoOptimised1_V3_6_MAC(INT *info,
													uchar* hbmp,
													INT height,
													INT width,
													bool &portraitindicator,
													bool &manyfacesindicator,
													bool &shoulderportraitindicator,
													EN_FSIP_ENGINE_TYPE enEngine_Type,
													INT &profileindicator,
													INT Channels,
													INT Depth,
													INT bpp)
{
	
	if(_cascade.empty()) return 0;
	if(_cascade2.empty()) return 0;  // New FaceModified
	if(_cascade3.empty()) return 0;  // New FaceModified
	
	bool portraitflag=FALSE;
	INT h,w,hw=height*width;
    
    vector<cv::Rect> _faces;
	
	CvPoint pt1, pt2;
	//V3.5	//JH
	IplImage *img = cvCreateImage(cvSize(width, height),Depth,Channels); 
	bool bload=memcpy(img->imageData ,hbmp , img->imageSize);
    cv::Mat _img = cv::cvarrToMat(img);
	
	bool BCascade1 = false;
	bool BCascade2 = false;
	bool BCascade3 = false;
	
	if(_cascade.empty()) return 0;
	
	if (bload)
	{
		if(!_cascade.empty())
		{
			CvMemStorage* storage = cvCreateMemStorage(0);
			CvSeq* faces;
			INT scale = 1;
			INT bodysize,bodyskin;
			//faces = cvHaarDetectObjects( img, cascade, storage, 1.15, 2, CV_HAAR_DO_CANNY_PRUNING );
            _cascade.detectMultiScale( _img, _faces,1.15, 2, 0);
			if (_faces.size()>0)//(faces->total > 0)
			{
				BCascade1 = true;
				profileindicator = 1 ;
			}
			
			if (_faces.size() <= 0) //(faces->total <= 0)  // New FaceModified
			{
				//faces = cvHaarDetectObjects( img, cascade2, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
                 _cascade2.detectMultiScale( _img, _faces,1.10, 2, 0);
				
				if (_faces.size()>0) //(faces->total > 0)
				{
					profileindicator = 2  ;
					BCascade2 = true;
				}
				
				if (_faces.size()<=0)// (faces->total <= 0)
				{
					//faces = cvHaarDetectObjects( img, cascade3, storage, 1.10, 2, CV_HAAR_DO_CANNY_PRUNING );
                    _cascade3.detectMultiScale( _img, _faces,1.10, 2, 0);
				}
				
				if (_faces.size()>0) //(faces->total > 0)
				{
					profileindicator = 3;
					BCascade3 = true;
				}
			}
			
			if (_faces.size()>0) //(faces->total>0)
			{
				//////////////////////additional add in
				bool belowface=FALSE;  //new added
				INT belowfacecount=0;	//new added
				//////////////////////
				struct FaceRec sFace;
				
				INT xx,yy,ww,hh;
				INT fsizeThres=35; //for portrait checking purpose in terms of size
				INT fheightThres=45;
				INT fwidthThres=45;
				for(INT i=0;i<_faces.size();i++) //for(INT i=0;i<faces->total;i++)
				{
					xx=0,yy=0,ww=0,hh=0;
					//CvRect face_rect = *(CvRect*)cvGetSeqElem( faces, i);
                    CvRect face_rect = _faces[i];
					xx=face_rect.x*scale; yy=face_rect.y*scale;
					ww=face_rect.width*scale; hh=face_rect.height*scale;
					//Determine the x,y,height and width of the face
					sFace.x=xx;
					sFace.height=hh;
					sFace.y=height-yy-hh;
					sFace.width=ww;
					
					if(sFace.x<0) sFace.x=0;
					if(sFace.x>width-1) sFace.x=width-1;
					if(sFace.height<0) sFace.height=0;
					if(sFace.height>height-1) sFace.height=height-1;
					if(sFace.y<0) sFace.y=0;
					if(sFace.y>height-1) sFace.y=height-1;
					if(sFace.width<0) sFace.width=0;
					if(sFace.width>width-1) sFace.width=width-1;
					
					pt1.x = face_rect.x*scale;
					pt2.x = (face_rect.x+face_rect.width)*scale;
					pt1.y = face_rect.y*scale;
					pt2.y = (face_rect.y+face_rect.height)*scale;
					
					if (sFace.x > ((2*width)/3) && (width > height) ) // New Engine
					{
						sFace.RightSide = TRUE;
					}
					else 
					{
						sFace.RightSide = FALSE;
					}
					
					if (sFace.x < (width /3) && (width > height)) // New Engine
					{
						sFace.LeftSide = TRUE;
					}
					else 
					{
						sFace.LeftSide = FALSE;
					}
					
					
					//determine whether it is portrait or not
					if((FLOAT)(sFace.width*sFace.height)/hw*100>fsizeThres || (FLOAT)sFace.height/height*100>fheightThres)
					{
						if (ShowDetailedImage == TRUE)
						{
							char WinName[16]; // ShowDetailedImage
							char PreWinName[16];  // ShowDetailedImage
							pt1.x = face_rect.x*scale;
							pt2.x = (face_rect.x+face_rect.width)*scale;
							pt1.y = face_rect.y*scale;
							pt2.y = (face_rect.y+face_rect.height)*scale;
							
							sprintf (WinName , "image%d" , NumThumbs);
							sprintf (PreWinName , "Image %d - Face" , NumThumbs);
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
							
							
							if (sFace.RightSide == TRUE)
							{
								pt1.x = 10;
								pt2.x = sFace.x;
								pt1.y = height - (sFace.y + (sFace.height /2));
								
								if ((sFace.y - (int)(sFace.height*1.5))>0)
									pt2.y = height - ((sFace.y - (int)(sFace.height*1.5)));
								else pt2.y = 0;
								
							}
							else if (sFace.LeftSide == TRUE)
							{
								pt1.x = sFace.x + sFace.width;
								pt2.x = width -10;
								pt1.y = height - (sFace.y + (sFace.height /2));
								
								if ((sFace.y - (int)(sFace.height*1.5)) > 0)
									pt2.y = height - ((sFace.y - (int)(sFace.height*1.5)));
								else pt2.y = 0;
								
								
							}
							
							else 
							{
								pt1.x = sFace.bx;
								pt2.x =  sFace.bx+ sFace.bwidth;
								pt1.y = height - sFace.by - sFace.bheight*0.3 ;
								pt2.y = height - (sFace.by+sFace.bheight );
							}
							cvRectangle( img, pt1, pt2, CV_RGB(255,0,0), 3, 8, 0 );
							
						}
						FaceAttr.erase(FaceAttr.begin(),FaceAttr.end());
						cvReleaseMemStorage(&storage);
						cvReleaseImage(&img);
						portraitflag=TRUE;
						portraitindicator=TRUE;
						return portraitflag;
					}
					
					//Determine the x,y,height and width of the body trunk
					if(sFace.y-(sFace.height/3*13)+1>0) //new added plus 1
					{
						sFace.by=sFace.y-(sFace.height/3*13)+1; //new added plus 1
						sFace.bheight=sFace.height/3*13;
					}
					else
					{
						sFace.by=0;
						sFace.bheight=sFace.y;
					}
					
					if(sFace.x-(2*sFace.width/3)+1>0) //new added plus 1
						sFace.bx=sFace.x-(2*sFace.width/3)+1; //new added plus 1
					else
						sFace.bx=0;
					if(sFace.x+(5*sFace.width/3)-1<width) //new added minus 1
						sFace.bwidth=sFace.x+(5*sFace.width/3)-sFace.bx;
					else
						sFace.bwidth=width-1-sFace.bx+1; //new added plus 1
					
					//Determine the boob area
					if(sFace.y-(sFace.height/3*7)+1>sFace.by) //new added plus 1
						sFace.boobydown=sFace.y-(sFace.height/3*6)+1;  //new added plus 1
					else
						sFace.boobydown=sFace.by;
					
					if(sFace.y-(sFace.height/3*3)+1>sFace.by) //new added plus 1
					{
						sFace.boobytop=sFace.y-(sFace.height/3*2)+1; //new added plus 1
						sFace.boob=TRUE;
					}
					else
					{
						sFace.boob=FALSE;
					}
					
					//Determine the private part area
					if(sFace.y-(sFace.height/3*8)+1>sFace.by) //new added plus 1
					{
						sFace.ppartytop=sFace.y-(sFace.height/3*8)+1; //new added plus 1
						sFace.ppart=TRUE;
					}
					else
					{
						sFace.ppart=FALSE;
					}
					
					if(sFace.ppart==TRUE)
					{
						if(sFace.y-(sFace.height/3*12)+1>sFace.by) //new added plus 1
							sFace.ppartydown=sFace.y-(sFace.height/3*12)+1; //new added plus 1
						else
							sFace.ppartydown=sFace.by;
					}
					//Determine it is a valid body structure or not
					//by calculating the amount of skin inside the body structure
					bodysize=0;bodyskin=0;
					bodysize=sFace.height*sFace.width + sFace.bheight*sFace.bwidth;
					sFace.area=bodysize;
					sFace.bodyarea=bodysize-(sFace.height*sFace.width);
					sFace.facearea=sFace.height*sFace.width;
					FaceAttr.push_back(sFace);
				} //for loop for number of faces
				
			} //if there exist face
			cvReleaseMemStorage(&storage);
		}
	} //end of check whether the loading of HBITMAP to IplImage successful or not
	
	cvReleaseImage(&img);
	////////////////////////////////////////////////////////////////////////////////////
	return portraitflag;
}

/*---------------------------------------------------------------------------
 //-  Function Name : LoadHBITMAP         
 //-  Description   : Convert HBITMAP image datatype to OpenCV image datatype
 //-  Parameter     : hBmp - hbitmap image
 img - OpenCv image datatype
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
//bool ImageProcessing::LoadHBITMAP(BYTE* hBmp,IplImage **img) 
bool ImageProcessing::LoadHBITMAP(BYTE* hBmp,IplImage **img, INT nWidth ,INT nHeight ,INT bpp)
{
	// check hbmp
	if(hBmp==NULL)
		return FALSE;
	
	
	if (24!= bpp)
		return FALSE;
	
	// Create IplImage.
	CvSize size;

	size.width  = nWidth;
	size.height = nHeight;
	*img = cvCreateImage(size, IPL_DEPTH_8U, 3);
	
	if (*img == NULL)
		return FALSE;
	
	// Copy Data over.
	unsigned char *lpSrc, *lpDst;
	INT iPixelBytes = (*img)->widthStep;
	

	
	//JH-Added for MAC openCV image format 
	lpSrc   = (unsigned char*)hBmp ;
	lpDst   = (unsigned char*) (*img)->imageData;
	for (INT i = 0; i < size.height; i++)
	{
		memcpy((void*)lpDst, (void*)lpSrc, iPixelBytes);
		lpSrc += iPixelBytes;
		lpDst += iPixelBytes;
	}
	
	return TRUE;
}

//bool ImageProcessing::LoadHBITMAP(BYTE* hBmp,IplImage **img) 
bool ImageProcessing::LoadHBITMAPCV(cv::Mat hBmp,cv::Mat &img)//(IplImage *hBmp,IplImage **img)
{
	// check hbmp
	if(hBmp.empty())//(hBmp==NULL)
		return FALSE;
	
	
	
	int bpp= hBmp.channels()* hBmp.depth(); //hBmp-> nChannels* hBmp->depth;
	
	//if (24 != ds.dsBm.bmBitsPixel)
	if (24!= bpp)
		return FALSE;
	
	// Create IplImage.
	CvSize size;
	
	size.width  = hBmp.cols;//hBmp->width;
	size.height = hBmp.rows;//hBmp->height;
	
     img = cv::Mat(size,CV_8UC3); //update opencv4 emad
	
	if (img.empty())
		return FALSE;
	
	// Copy Data over.
	unsigned char *lpSrc, *lpDst;
    int iPixelBytes = img.step[0];//(*img)->widthStep;//opencvupdate4
	
	
	
	lpSrc   = (unsigned char*)hBmp.data ;
    lpDst   = (unsigned char*) img.data; 
	for (int i = 0; i < size.height; i++)
	{
		memcpy((void*)lpDst, (void*)lpSrc, iPixelBytes);
		lpSrc += iPixelBytes;
		lpDst += iPixelBytes;
	}
	
	return TRUE;
}


bool ImageProcessing::LoadHBITMAP_Reverse(BYTE* hBmp,cv::Mat &img, INT nWidth ,INT nHeight ,INT bpp)
{
	// check hbmp
	if(hBmp==NULL)
		return FALSE;
	
	
	if (24!= bpp)
		return FALSE;
	
    cv::Size size;
	

	size.width  = nWidth;
	size.height = nHeight;
	
    img = cv::Mat(size,CV_8UC3);
	
	if (img.empty())//(*img == NULL)
		return FALSE;
	
	// Copy Data over.
	unsigned char *lpSrc, *lpDst;
//	INT iPixelBytes = (*img)->widthStep;
    INT iPixelBytes = img.step[0];
	

	 lpSrc   = (unsigned char*)hBmp + ((nHeight-1) * iPixelBytes);
    lpDst   = (unsigned char*) img.data;
	 for (INT i = 0; i < size.height; i++)
	 {
	 memcpy((void*)lpDst, (void*)lpSrc, iPixelBytes);
	 lpSrc -= iPixelBytes;
	 lpDst += iPixelBytes;
	 }

	
	return TRUE;
}

/*---------------------------------------------------------------------------
 //-  Function Name : AnalyseHumanStructureOptimised
 Description   : Analyse the human structure below the face to determine the naked area and topology
 //-  Parameter     : blob_segment2 - information data in the blob
 object_map2 - array data referenced to blob identifier
 newlabel - number of blob
 height - height of the image
 width - width of the image
 pResults - structure contains porn features
 info - skin data
 faceskin - number of skin pixels in the face area
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::AnalyseHumanStructureOptimised(ImageProcessing::simonBlob *blob_segment2,INT *object_map2,INT newlabel,INT height,INT width,ImageProcessing::WQ_IP_RESULTS * pResults, INT* info, INT &faceSkin)

{
	INT i,j,h,w;
	INT finalpornvalue=0;
	INT nostruct=0;
	INT hw=height*width;
	bool shoulderflag=FALSE;
	bool halfbodyflag=FALSE;
	bool decisionflag=TRUE;
	bool harddecision=FALSE;
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	// Determine the body structure based on face and body trunk topology
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	INT body_num=DetermineBodyOptimised(blob_segment2,object_map2,newlabel,height,width, faceSkin);
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	// Determine the human skin blob based on the body structure and skin information
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	INT face_num=FaceAttr.size();
	INT *totalskinblob=new INT[face_num+1];
	INT *totalskinstructure=new INT[face_num+1];
	INT *totalbodystructure=new INT[face_num+1];
	INT *pornbody_value=new INT[face_num+1];
	bool *validbody=new bool[face_num+1];
	INT skinblobthres=20; //from 20
	INT i_m1;
	DetermineSkinBodyOptimised(totalskinblob,totalskinstructure,totalbodystructure,blob_segment2,object_map2,info,newlabel,height,width,skinblobthres);
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	// Determine the pornographic level
	//////////////////////////////////////////////////////////////////////////////////////////////////////   
	INT no_validbody=0;
	INT bodystruct_diff=0;
	INT ratiobodystruct;
	INT ratiobodyblob;
	INT boob_area=0;
	INT privatepart_area=0;
	INT centerboob_area=0;
	INT centerprivatepart_area=0;
	INT ratioexposed=0;
	//half body analysis
	INT leftbody=0,topleftbody=0;
	INT rightbody=0,toprightbody=0;
	
	//a list of threshold 
	//////////////////////////////////////
	INT nonpornthres=10;
	INT pornthres=55;
	INT earlyhardthres=30;
	//////////////////////////////////////
	
	for(i=1;i<face_num+1;i++)
	{
		i_m1 = i-1;
		if(FaceAttr[i_m1].bheight>=4*FaceAttr[i_m1].height/3) //check shoulder portrait
		{
			//determine the size of the face
			INT faceratio=((FLOAT )FaceAttr[i_m1].facearea/(height*width))*100;
			
			//decide on the new porn threshold if face ratio above a certain threshold
			if(faceratio>=10) pornthres=65;
			
			bodystruct_diff=(FLOAT )(abs(totalbodystructure[i]-FaceAttr[i_m1].bodyarea))/FaceAttr[i_m1].bodyarea*100;
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			// FLEXIBLE HUMAN STRUCTURE
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			if(bodystruct_diff<50)
			{
				
				//Segmented body structure analysis
				ratiobodystruct=(FLOAT )totalskinstructure[i]/totalbodystructure[i]*100;
				ratiobodyblob=(FLOAT )totalskinblob[i]/totalbodystructure[i]*100;
				/////////////////////////////////////////////////////////////////////////////
				//Extreme non-porn
				/////////////////////////////////////////////////////////////////////////////
				if(ratiobodystruct<=5 || ratiobodyblob<=10)
				{
					pornbody_value[i]=ratiobodyblob;
				}	
				/////////////////////////////////////////////////////////////////////////////
				
				/////////////////////////////////////////////////////////////////////////////
				//Performing slanting body and exposed body analysis
				/////////////////////////////////////////////////////////////////////////////
				if(face_num<=2 && ratiobodyblob>=earlyhardthres && ratiobodyblob<pornthres && FaceAttr[i-1].bheight>=6*FaceAttr[i-1].height/3)
				{
					/////////////////////////////////////////////////
					//Obtain information for Half body analysis
					/////////////////////////////////////////////////
					halfbodyflag=TRUE;
					leftbody=0,topleftbody=0;
					rightbody=0,toprightbody=0;
					INT centerbody=0,topcenterbody=0;
					INT topbody=0,toptopbody=0;
					INT halfshoulderarea=(FaceAttr[i_m1].bwidth/2)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT centershoulderarea=(FaceAttr[i_m1].width)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT topshoulderarea=(FaceAttr[i_m1].bwidth)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT midpoINTbody=FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth/2;
					INT leftpoINT=FaceAttr[i_m1].x;
					INT rightpoINT=FaceAttr[i_m1].x+FaceAttr[i_m1].width-1;
					INT h_end_loop = FaceAttr[i_m1].by+FaceAttr[i_m1].bheight;
					INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth;
					for(h=FaceAttr[i_m1].by;h<h_end_loop;h++)
					{
						INT h_width = h*width;
						for(w=FaceAttr[i_m1].bx;w<w_end_loop;w++)
						{
							INT h_width_w = h_width+w;
							if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()>0)
							{
								//the segmented blob does not share by more than one human structure (complete blob)
								if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()==1 && blob_segment2[object_map2[h_width_w]].humanskin_display[0]==i)
								{
									//calculate the left or right body
									if(w<midpoINTbody)
									{
										leftbody++;
										if(h>FaceAttr[i_m1].boobytop) topleftbody++;
									}
									else
									{
										rightbody++;
										if(h>FaceAttr[i_m1].boobytop) toprightbody++;
									}
									//calculate the center body
									if(w>=leftpoINT && w<=rightpoINT)
									{
										centerbody++;
										if(h>FaceAttr[i_m1].boobytop) topcenterbody++;
									}
									//calculate the top body
									if(FaceAttr[i_m1].ppart==TRUE)
									{
										if(h>=FaceAttr[i_m1].ppartytop && h<FaceAttr[i_m1].y)
										{
											topbody++;
											if(h>FaceAttr[i_m1].boobytop) toptopbody++;					
										}
									}
								}
								else
									//the segmented blob is shared by more than one human structure (partial blob)
								{
									for(nostruct=0;nostruct<blob_segment2[object_map2[h_width_w]].humanskin_display.size();nostruct++)
									{
										if(blob_segment2[object_map2[h_width_w]].humanskin_display[nostruct]==-i)
										{
											//calculate left or right body
											if(w<midpoINTbody)
											{
												leftbody++;
												if(h>FaceAttr[i_m1].boobytop) topleftbody++;
											}
											else
											{
												rightbody++;
												if(h>FaceAttr[i_m1].boobytop) toprightbody++;
											}
											//calculate the center body
											if(w>=leftpoINT && w<=rightpoINT)
											{
												centerbody++;
												if(h>FaceAttr[i_m1].boobytop) topcenterbody++;
											}
											//calculate the top body
											if(FaceAttr[i_m1].ppart==TRUE)
											{
												if(h>=FaceAttr[i_m1].ppartytop && h<FaceAttr[i_m1].y)
												{
													topbody++;
													if(h>FaceAttr[i_m1].boobytop) toptopbody++;					
												}
											}
										}
									}
								}
							}
						}
					}
					
					/////////////////////////////////////////////////
					//Information before half body decision","",0);
					/////////////////////////////////////////////////
					if(leftbody>rightbody)
					{
						pResults->NetPornValue=(FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2);
					}
					else
					{
						pResults->NetPornValue=(FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2);
					}
					/////////////////////////////////////////////////
					
					
					/////////////////////////////////////////////////
					// Performing analysis based on the information from half body analysis
					/////////////////////////////////////////////////
					//determine it is extreme porn based on vertical half body analysis
					if(leftbody>rightbody)
					{
						if((FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2)*100>pornthres && (FLOAT )topleftbody/halfshoulderarea*100>25) //30
						{
							pResults->NetPornValue=(FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2);
							pResults->pf=1;
							pResults->kosf=402;
							pResults->FT =1171;
							decisionflag=FALSE;
							break;
						}
					}
					else
					{
						if((FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2)*100>pornthres && (FLOAT )toprightbody/halfshoulderarea*100>25) //30
						{
							pResults->NetPornValue=(FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2);
							pResults->pf=1;
							pResults->kosf=403;
							pResults->FT =1172;
							decisionflag=FALSE;
							break;
						}
					}
					//center body
					if((FLOAT )centerbody/(FaceAttr[i_m1].width*FaceAttr[i_m1].bheight)*100>pornthres && (FLOAT )topcenterbody/centershoulderarea*100>25) //30
					{
						pResults->NetPornValue=(FLOAT )centerbody/(FaceAttr[i_m1].width*FaceAttr[i_m1].bheight);
						pResults->pf=1;
						pResults->kosf=404;
						pResults->FT =1173;
						decisionflag=FALSE;
						break;
					}
					//top body
					if((FLOAT )topbody/(FaceAttr[i_m1].bwidth*(FaceAttr[i_m1].y-FaceAttr[i_m1].ppartytop+1))*100>pornthres && (FLOAT )topcenterbody/centershoulderarea*100>25) //30
					{
						pResults->NetPornValue=(FLOAT )topbody/(FaceAttr[i_m1].bwidth*(FaceAttr[i_m1].y-FaceAttr[i_m1].ppartytop+1));
						pResults->pf=1;
						pResults->kosf=405;
						pResults->FT =1174;
						decisionflag=FALSE;
					}
					/////////////////////////////////////////////////////////////////////////////
					// End of half body analysis
					/////////////////////////////////////////////////////////////////////////////
					
					/////////////////////////////////////////////////////////////////////////////
					//Exposed area analysis - analyse on the exposed of boob and private part
					/////////////////////////////////////////////////////////////////////////////
					//starting of boob validation
					boob_area=0;
					centerboob_area=0;
					INT midboob=0;
					midboob=FaceAttr[i_m1].boobydown+((FaceAttr[i_m1].boobytop-FaceAttr[i_m1].boobydown)/(FLOAT )2);
					h_end_loop = FaceAttr[i_m1].boobytop;
					w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth;
					if(FaceAttr[i_m1].boob==TRUE)
					{
						for(h=FaceAttr[i_m1].boobydown;h<=h_end_loop;h++) 
						{
							INT h_width = h*width;
							for(w=FaceAttr[i_m1].bx;w<w_end_loop;w++) 
							{
								INT h_width_w = h_width+w;
								if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()>0)
								{
									//complete blob
									if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()==1 && blob_segment2[object_map2[h_width_w]].humanskin_display[0]==i)
									{
										boob_area++;
										//update center boob area
										if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width && h<=midboob)
										{
											centerboob_area++;
										}
									}
									//partial blob
									else 
									{
										for(nostruct=0;nostruct<blob_segment2[object_map2[h_width_w]].humanskin_display.size();nostruct++)
										{
											if(blob_segment2[object_map2[h_width_w]].humanskin_display[nostruct]==-i)
											{
												boob_area++;
												//update center boob area
												if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width)
												{
													centerboob_area++;
												}
											}
										}
									}
								}
							}
						}
					} //end of boob validation
					
					//starting of private part validation
					privatepart_area=0;
					centerprivatepart_area=0;
					if(FaceAttr[i_m1].ppart==TRUE)
					{
						INT h_end_loop = FaceAttr[i_m1].ppartytop;
						INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth-1;
						for(h=FaceAttr[i_m1].ppartydown;h<=h_end_loop;h++)
						{
							INT h_width = h*width;
							for(w=FaceAttr[i_m1].bx;w<=w_end_loop;w++)
							{
								INT h_width_w = h_width+w;
								if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()>0)
								{
									//complete blob
									if(blob_segment2[object_map2[h_width_w]].humanskin_display.size()==1 && blob_segment2[object_map2[h_width_w]].humanskin_display[0]==i)
									{
										privatepart_area++;
										//update center boob area
										//update center private part area
										if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width)
										{
											centerprivatepart_area++;
										}
									}
									else //partial blob
									{
										for(nostruct=0;nostruct<blob_segment2[object_map2[h_width_w]].humanskin_display.size();nostruct++)
										{
											if(blob_segment2[object_map2[h_width_w]].humanskin_display[nostruct]==-i)
											{
												privatepart_area++;
												//update center private part area
												if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width)
												{
													centerprivatepart_area++;
												}
											}
										}
									}
								}
							}
						}
					} //end of private part validation
					
					//consider the boob area is valid or not
					if((FLOAT )centerboob_area/(FaceAttr[i_m1].width*(midboob-FaceAttr[i_m1].boobydown+1))*100<pornthres) boob_area=0; //35
					//consider the private part area is valid or not
					if((FLOAT )centerprivatepart_area/(FaceAttr[i_m1].width*(FaceAttr[i_m1].ppartytop-FaceAttr[i_m1].ppartydown+1))*100<45) privatepart_area=0; //50
					
					ratioexposed=(FLOAT )(boob_area+privatepart_area)/totalskinblob[i]*100;
					if((FLOAT )centerboob_area/(FaceAttr[i_m1].width*(midboob-FaceAttr[i_m1].boobydown+1))*100<25) //previous from 30
					{
						//nonporn coz of closed exposed area
						pResults->NetPornValue=(FLOAT )centerboob_area/(FaceAttr[i_m1].width*(midboob-FaceAttr[i_m1].boobydown+1));
						pResults->pf=0;
						pResults->FT =1181;
						pResults->kosf=406;
						decisionflag=FALSE;
						break;
					}
					if(ratioexposed>pornthres) //change from 50 to 60 to 70
					{
						//Porn coz of exposed private areas
						pResults->NetPornValue=(FLOAT )ratioexposed/100;
						pResults->pf=1;
						pResults->kosf=407;
						pResults->FT =1175;
						decisionflag=FALSE;
						break;
					}
					// End of exposed area analysis
					/////////////////////////////////////////////////////////////////////////////
				}
				/////////////////////////////////////////////////////////////////////////////
				//End of slanting body and exposed body analysis
				/////////////////////////////////////////////////////////////////////////////
				
				/////////////////////////////////////////////////////////////////////////////
				//Extreme porn
				/////////////////////////////////////////////////////////////////////////////
				if(ratiobodyblob>=pornthres)
				{
					if(face_num<=2 || (face_num<=3 && (FLOAT )FaceAttr[i_m1].facearea/(height*width)*100>5))
					{
						//Extreme Porn
						pResults->NetPornValue=(FLOAT )ratiobodyblob/100;
						pResults->pf=1;
						pResults->kosf=408;
						pResults->FT =1176;
						decisionflag=FALSE;
						harddecision=TRUE;
						break;
					}
					else
					{
						pornbody_value[i]=ratiobodyblob;
					}
				}
				else
				{
					pornbody_value[i]=ratiobodyblob;
				}
				/////////////////////////////////////////////////////////////////////////////
			}
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			// END OF FLEXIBLE HUMAN STRUCTURE
			//////////////////////////////////////////////////////////////////////////////////////////////////////
			else
				//////////////////////////////////////////////////////////////////////////////////////////////////////
				// SQUARE HUMAN STRUCTURE - the segmented body structure not the same with the approximate body structure
				//////////////////////////////////////////////////////////////////////////////////////////////////////
			{
				pornthres=pornthres-5;
				earlyhardthres=earlyhardthres-5;
				INT skINTable=0;
				INT h_end_loop = FaceAttr[i_m1].by+FaceAttr[i_m1].bheight;
				INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth;
				for(h=FaceAttr[i_m1].by;h<h_end_loop;h++)
				{
					for(w=FaceAttr[i_m1].bx;w<w_end_loop;w++)
					{
						if(info[h*width+w]==3) skINTable++;
					}
				}
				ratiobodyblob=(FLOAT )skINTable/FaceAttr[i_m1].bodyarea*100;
				/////////////////////////////////////////////////////////////////////////////
				//Extreme non-porn
				/////////////////////////////////////////////////////////////////////////////
				if(ratiobodyblob<=10)
				{
					if(face_num==1)
					{
						//Extreme Not Porn
						pResults->NetPornValue=(FLOAT )ratiobodyblob/100;
						pResults->pf=0;
						pResults->kosf=451;
						decisionflag=FALSE;
						pResults->FT =1182;
						harddecision=TRUE;
						break;
					}
					else
					{
						pornbody_value[i]=ratiobodyblob;
					}
				}
				/////////////////////////////////////////////////////////////////////////////
				
				/////////////////////////////////////////////////////////////////////////////
				//Performing slanting body and exposed body analysis
				/////////////////////////////////////////////////////////////////////////////
				if(face_num<=2 && ratiobodyblob>=earlyhardthres && ratiobodyblob<pornthres && FaceAttr[i_m1].bheight>=6*FaceAttr[i_m1].height/3)
				{
					
					halfbodyflag=TRUE;
					////////////////////////////////////////////////////////////////
					//Half Body Analysis - Find slanting porn body 
					////////////////////////////////////////////////////////////////
					leftbody=0,topleftbody=0;
					rightbody=0,toprightbody=0;
					INT centerbody=0,topcenterbody=0;
					INT topbody=0,toptopbody=0;
					INT halfshoulderarea=(FaceAttr[i_m1].bwidth/2)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT centershoulderarea=(FaceAttr[i_m1].width)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT topshoulderarea=(FaceAttr[i_m1].bwidth)*(FaceAttr[i_m1].y-FaceAttr[i_m1].boobytop+1);
					INT midpoINTbody=FaceAttr[i_m1].bx+(FaceAttr[i_m1].bwidth/2)-1;
					INT leftpoINT=FaceAttr[i_m1].x;
					INT rightpoINT=FaceAttr[i_m1].x+FaceAttr[i_m1].width-1;
					INT h_end_loop = FaceAttr[i_m1].by+FaceAttr[i_m1].bheight;
					INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth;
					
					for(h=FaceAttr[i_m1].by;h<h_end_loop;h++)
					{
						for(w=FaceAttr[i_m1].bx;w<w_end_loop;w++)
						{
							if(info[h*width+w]==3)
							{
								if(w<midpoINTbody)
								{
									leftbody++;
									if(h>FaceAttr[i_m1].boobytop) topleftbody++;
								}
								else
								{
									rightbody++;
									if(h>FaceAttr[i_m1].boobytop) toprightbody++;
								}
								//calculate the center body
								if(w>=leftpoINT && w<=rightpoINT)
								{
									centerbody++;
									if(h>FaceAttr[i_m1].boobytop) topcenterbody++;
								}
								//calculate the top body
								if(FaceAttr[i_m1].ppart==TRUE)
								{
									if(h>=FaceAttr[i_m1].ppartytop && h<FaceAttr[i_m1].y)
									{
										topbody++;
										if(h>FaceAttr[i_m1].boobytop) toptopbody++;					
									}
								}
							}
						}
					}
					//Information before half body decision
					if(leftbody>rightbody)
					{
						pResults->NetPornValue=(FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2);
					}
					else
					{
						pResults->NetPornValue=(FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2);
					}
					
					
					//determine it is extreme porn based on vertical half body analysis
					if(leftbody>rightbody)
					{
						if((FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2)*100>pornthres && (FLOAT )topleftbody/halfshoulderarea*100>25-5) //30
						{
							//Exposed on the left body","Half body analysis"
							pResults->NetPornValue=(FLOAT )leftbody/(FaceAttr[i_m1].bodyarea/2);
							pResults->pf=1;
							pResults->kosf=452;
							pResults->FT =1171;
							decisionflag=FALSE;
							break;
						}
					}
					else
					{
						if((FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2)*100>pornthres && (FLOAT )toprightbody/halfshoulderarea*100>25-5) //30
						{
							//Exposed on the right body","Half body analysis"
							pResults->NetPornValue=(FLOAT )rightbody/(FaceAttr[i_m1].bodyarea/2);
							pResults->pf=1;
							pResults->kosf=453;
							pResults->FT =1172;
							decisionflag=FALSE;
							break;
						}
					}
					
					INT centerarea=(FLOAT )centerbody/(FaceAttr[i_m1].width*FaceAttr[i_m1].bheight)*100;
					//center body
					if((FLOAT )centerbody/(FaceAttr[i_m1].width*FaceAttr[i_m1].bheight)*100>pornthres && (FLOAT )topcenterbody/centershoulderarea*100>25-5) //30
					{
						//Center body exposed","Half body analysis"
						pResults->NetPornValue=(FLOAT )centerbody/(FaceAttr[i_m1].width*FaceAttr[i_m1].bheight);
						pResults->pf=1;
						pResults->kosf=454;
						pResults->FT =1173;
						decisionflag=FALSE;
						break;
					}
					//top body
					if((FLOAT )topbody/(FaceAttr[i_m1].bwidth*(FaceAttr[i_m1].y-FaceAttr[i_m1].ppartytop+1))*100>pornthres && (FLOAT )topcenterbody/centershoulderarea*100>25-5) //30
					{
						//"Top body exposed","Half body analysis"
						pResults->NetPornValue=(FLOAT )topbody/(FaceAttr[i_m1].bwidth*(FaceAttr[i_m1].y-FaceAttr[i_m1].ppartytop+1));
						pResults->pf=1;
						pResults->kosf=455;
						pResults->FT =1174;
						decisionflag=FALSE;
						break;
					}
					// End of half body analysis
					////////////////////////////////////////////////////////////////
					
					////////////////////////////////////////////////////////////////
					//Exposed area analysis
					////////////////////////////////////////////////////////////////
					boob_area=0;
					centerboob_area=0;
					INT midboob=0;
					midboob=FaceAttr[i_m1].boobydown+((FaceAttr[i_m1].boobytop-FaceAttr[i_m1].boobydown+1)/(FLOAT )2);
					if(FaceAttr[i_m1].boob==TRUE)
					{
						INT h_end_loop = FaceAttr[i_m1].boobytop;
						INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth;
						for(h=FaceAttr[i_m1].boobydown;h<=h_end_loop;h++) 
						{
							for(w=FaceAttr[i_m1].bx;w<w_end_loop;w++) 
							{
								if(info[h*width+w]==3)
								{
									boob_area++;
									//update center boob area
									if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width && h<=midboob)
									{
										centerboob_area++;
									}
								}
							}
						}
					} //end of boob validation
					
					privatepart_area=0;
					centerprivatepart_area=0;
					if(FaceAttr[i_m1].ppart==TRUE)
					{
						INT h_end_loop = FaceAttr[i_m1].ppartytop;
						INT w_end_loop = FaceAttr[i_m1].bx+FaceAttr[i_m1].bwidth-1;
						for(h=FaceAttr[i_m1].ppartydown;h<=h_end_loop;h++)
						{
							for(w=FaceAttr[i_m1].bx;w<=w_end_loop;w++)
							{
								if(info[h*width+w]==3)
								{
									privatepart_area++;
									//update center private part area
									if(w>=FaceAttr[i_m1].x && w<FaceAttr[i_m1].x+FaceAttr[i_m1].width)
									{
										centerprivatepart_area++;
									}
								}
							}
						}
					} //end of private part validation
					
					//consider the boob area is valid or not
					if((FLOAT )centerboob_area/(FaceAttr[i_m1].width*(midboob-FaceAttr[i_m1].boobydown+1))*100<pornthres) boob_area=0; //35
					//consider the private part area is valid or not
					if((FLOAT )centerprivatepart_area/(FaceAttr[i_m1].width*(FaceAttr[i_m1].ppartytop-FaceAttr[i_m1].ppartydown+1))*100<45-5) privatepart_area=0; //50
					
					ratioexposed=(FLOAT )(boob_area+privatepart_area)/totalskinblob[i]*100;
					
					if(ratioexposed>pornthres) //change from 50 to 60 to 70
					{
						//"Porn coz of exposed private areas"
						pResults->NetPornValue=(FLOAT )ratioexposed/100;
						pResults->pf=1;
						pResults->kosf=457;
						pResults->FT =1175;
						decisionflag=FALSE;
						break;
					}
					//End of exposed area analysis
					////////////////////////////////////////////////////////////////
				}
				/////////////////////////////////////////////////////////////////////////////
				if(ratiobodyblob>=pornthres)
				{
					if(face_num<=2 || (face_num<=3 && (FLOAT )FaceAttr[i_m1].facearea/(height*width)*100>5))
					{
						//"Extreme Porn"
						pResults->NetPornValue=(FLOAT )ratiobodyblob/100;
						pResults->pf=1;
						pResults->kosf=458;
						pResults->FT =1176;
						decisionflag=FALSE;
						harddecision=TRUE;
						break;
					}
					else
					{
						pornbody_value[i]=ratiobodyblob;
					}
				}
				else
				{
					pornbody_value[i]=ratiobodyblob;
				}
			} //end of non-segmented blob analysis
		}
		else
			//if it is a shoulder portrait
		{
			shoulderflag=TRUE;
			pornbody_value[i]=0;
		}
	} //end of face iteration
	
	
	//further analysis on the body structure
	if(decisionflag)
	{
		
		//MessageBox(NULL,_T("decisionflag"),_T("AnalyseHumanBody"),NULL);
		INT totalfacearea=0;
		INT totalpornvalue=0;
		//"Getting average porn value"
		if(face_num==1)
		{
			;
		}
		else
		{
			//determine the total face area;
			totalfacearea=0;
			for(j=1;j<face_num+1;j++)
			{
				totalfacearea+=FaceAttr[j-1].facearea;
			}
			//calculate the final porn value
			totalpornvalue=0;
			for(j=1;j<face_num+1;j++)
			{
				totalpornvalue+=(((FLOAT )FaceAttr[j-1].facearea/totalfacearea)*pornbody_value[j]);
			}
			
			
			
			
			pResults->NetPornValue=(FLOAT )totalpornvalue/100;
			if(totalpornvalue>50)  //from 55
			{
				pResults->NetPornValue=1.0f;
				pResults->pf=1;
				pResults->FT =1177;
				pResults->kosf=492;
			}
			else
			{
				pResults->NetPornValue=0.0f;
				pResults->pf=0;
				pResults->FT =1182;
				pResults->kosf=492;
			}
		}
	}
	
	//Reanalyse predetermined non-porn image
	{
		INT furtherthres=35;
		INT extremethres=80;
		
		bool continueflag=TRUE;
		//Flip Condition
		if((FLOAT )FaceAttr[0].facearea/hw*100<10 && (FLOAT )FaceAttr[0].facearea/hw*100>1 && (FLOAT )height/2<FaceAttr[0].y+(FaceAttr[0].height/2)-1 && continueflag==TRUE)
		{
			//"Very slanted human body analysis"
			//in the middle
			if((FLOAT )width/3<FaceAttr[0].x+(FaceAttr[0].width/2)-1 && (FLOAT )width/3*2>=FaceAttr[0].x+(FaceAttr[0].width/2)-1)
			{
				if(FaceAttr[0].ppart==TRUE)
				{
					INT leftskin=0;
					INT rightskin=0;
					INT leftpt=0;
					INT rightpt=0;
					if(halfbodyflag==TRUE)
					{
						//"Gone thru half body analysis"
						if(leftbody>rightbody)
						{
							leftpt=0;
							//rightpt=width;
							if(FaceAttr[0].x-FaceAttr[0].bwidth+1>0) leftpt=FaceAttr[0].x-FaceAttr[0].bwidth+1;
							rightpt=FaceAttr[0].x;
							for(h=FaceAttr[0].by;h<FaceAttr[0].by+((FLOAT )FaceAttr[0].bheight/2);h++)
							{
								for(w=leftpt;w<=rightpt;w++)
								{
									if(info[h*width+w]==3) leftskin++;
								}
							}
							INT leftskinarea=(FLOAT )leftskin/((rightpt-leftpt+1)*(FaceAttr[0].bheight/2))*100;
							
							if((FLOAT )leftskin/((rightpt-leftpt+1)*(FaceAttr[0].bheight/2))*100>=furtherthres && (FLOAT )(rightpt-leftpt+1)/FaceAttr[0].bwidth*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=493;	
								pResults->FT =1178;
								continueflag=FALSE;
							}
						}
						else
						{
							leftpt=FaceAttr[0].x+FaceAttr[0].width-1;
							rightpt=width;
							if(leftpt+FaceAttr[0].bwidth-1<width) rightpt=leftpt+FaceAttr[0].bwidth-1;
							INT h_end_loop = FaceAttr[0].by+((FLOAT )FaceAttr[0].bheight/2);
							for(h=FaceAttr[0].by;h<h_end_loop;h++)
							{
								for(w=leftpt;w<=rightpt;w++)
								{
									if(info[h*width+w]==3) rightskin++;
								}
							}
							
							INT rightskinarea=(FLOAT )rightskin/((rightpt-leftpt+1)*(FaceAttr[0].bheight/2))*100;
							
							if((FLOAT )rightskin/((rightpt-leftpt+1)*(FaceAttr[0].bheight/2))*100>=furtherthres && (FLOAT )(rightpt-leftpt+1)/FaceAttr[0].bwidth*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=493;
								pResults->FT =1179;
								continueflag=FALSE;
							}
						}
					}
					else
					{
						//no previous analysis on the half body analysis
						//left body
						INT leftptL=0;
						//INT rightptL=width;
						if(FaceAttr[0].x-FaceAttr[0].bwidth+1>0) leftptL=FaceAttr[0].x-FaceAttr[0].bwidth+1;
						//if(leftptL+FaceAttr[0].bwidth-1<width) rightptL=leftptL+FaceAttr[0].bwidth-1;
						INT rightptL=FaceAttr[0].x;
						INT h_end_loop = FaceAttr[0].by+((FLOAT )FaceAttr[0].bheight/2);
						for(h=FaceAttr[0].by;h<h_end_loop;h++)
						{
							for(w=leftptL;w<=rightptL;w++)
							{
								if(info[h*width+w]==3) leftskin++;
							}
						}
						//right body
						INT leftptR=FaceAttr[0].x+FaceAttr[0].width-1;
						INT rightptR=width;
						if(leftptR+FaceAttr[0].bwidth-1<width) rightptR=leftptR+FaceAttr[0].bwidth-1;
						
						h_end_loop = FaceAttr[0].by+((FLOAT )FaceAttr[0].bheight/2);
						for(h=FaceAttr[0].by;h<h_end_loop;h++)
						{
							for(w=leftptR;w<=rightptR;w++)
							{
								if(info[h*width+w]==3) rightskin++;
							}
						}
						if(leftskin>rightskin && (FLOAT )(rightptL-leftptL+1)/FaceAttr[0].bwidth*100>=extremethres)
						{
							INT leftskinarea=(FLOAT )leftskin/((rightptL-leftptL+1)*(FaceAttr[0].bheight/2))*100;
							
							
							if((FLOAT )leftskin/((rightptL-leftptL+1)*(FaceAttr[0].bheight/2))*100>=furtherthres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=493;
								pResults->FT =11710;
								continueflag=FALSE;
							}
						}
						else
						{
							if((FLOAT )(rightptR-leftptR+1)/FaceAttr[0].bwidth*100>=extremethres)
							{
								INT rightskinarea=(FLOAT )rightskin/((rightptR-leftptR+1)*(FaceAttr[0].bheight/2))*100;
								
								if((FLOAT )rightskin/((rightptR-leftptR+1)*(FaceAttr[0].bheight/2))*100>=furtherthres)
								{
									pResults->NetPornValue=1.0f;
									pResults->pf=1;
									pResults->kosf=493;
									pResults->FT =11711;
									continueflag=FALSE;
								}
							}
						}
					}
				}
			}
			else
				//at the side of the image
			{
				if(FaceAttr[0].ppart==TRUE)
				{
					//Lying body analysis
					INT leftskin=0;
					INT rightskin=0;
					INT leftpt=0;
					INT rightpt=0;
					if(halfbodyflag==TRUE)
					{
						INT leftface=FaceAttr[0].x;
						INT onethirdwidth=(FLOAT )width/3;
						if(FaceAttr[0].x>(FLOAT )width/3)
						{
							leftpt=0;
							if(FaceAttr[0].x-(2*FaceAttr[0].width)+1>0) leftpt=FaceAttr[0].x-(2*FaceAttr[0].width)+1;
							rightpt=FaceAttr[0].x;
							INT h_end_loop = FaceAttr[0].boobytop;
							for(h=FaceAttr[0].ppartytop;h<h_end_loop;h++)
							{
								for(w=leftpt;w<=rightpt;w++)
								{
									if(info[h*width+w]==3) leftskin++;
								}
							}
							if((FLOAT )leftskin/((rightpt-leftpt+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100>=furtherthres && (FLOAT )(rightpt-leftpt+1)/(FaceAttr[0].width*2)*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=494;
								pResults->FT =11712;
								continueflag=FALSE;
							}
						}
						else
						{
							leftpt=FaceAttr[0].x+FaceAttr[0].width-1;
							rightpt=width;
							if(leftpt+(FaceAttr[0].width*2)<width) rightpt=leftpt+(FaceAttr[0].width*2);
							INT h_end_loop = FaceAttr[0].boobytop;
							for(h=FaceAttr[0].ppartytop;h<h_end_loop;h++)
							{
								for(w=leftpt;w<=rightpt;w++)
								{
									if(info[h*width+w]==3) rightskin++;
								}
							}
							INT rightbodyarea=(FLOAT )rightskin/((rightpt-leftpt+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100;
							if((FLOAT )rightskin/((rightpt-leftpt+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100>=furtherthres && (FLOAT )(rightpt-leftpt+1)/(FaceAttr[0].width*2)*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=494;
								pResults->FT =11713;
								continueflag=FALSE;
							}
						}
					}
					else
					{
						//no previous analysis on the half body analysis
						if(FaceAttr[0].x>(FLOAT )width/3)
						{
							//left body
							INT leftptL=0;
							//INT rightptL=width;
							if(FaceAttr[0].x-(2*FaceAttr[0].width)+1>0) leftptL=FaceAttr[0].x-(2*FaceAttr[0].width)+1;
							//if(leftptL+(FaceAttr[0].width*2)-1<width) rightptL=leftptL+(FaceAttr[0].width*2)-1;
							INT rightptL=FaceAttr[0].x;
							
							INT h_end_loop = FaceAttr[0].boobytop;
							for(h=FaceAttr[0].ppartytop;h<h_end_loop;h++)
							{
								for(w=leftptL;w<=rightptL;w++)
								{
									if(info[h*width+w]==3) leftskin++;
								}
							}
							INT leftbodyarea=(FLOAT )leftskin/((rightptL-leftptL+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100;
							if((FLOAT )leftskin/((rightptL-leftptL+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100>=furtherthres && (FLOAT )(rightptL-leftptL+1)/(FaceAttr[0].width*2)*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=494;
								pResults->FT =11714;
								continueflag=FALSE;
							}
						}
						else
						{
							//right body
							INT leftptR=FaceAttr[0].x+FaceAttr[0].width-1;
							INT rightptR=width;
							if(leftptR+(FaceAttr[0].width*2)<width) rightptR=leftptR+(FaceAttr[0].width*2);
							INT h_end_loop = FaceAttr[0].boobytop;
							for(h=FaceAttr[0].ppartytop;h<h_end_loop;h++)
							{
								for(w=leftptR;w<=rightptR;w++)
								{
									if(info[h*width+w]==3) rightskin++;
								}
							}
							INT rightbodyarea=(FLOAT )rightskin/((rightptR-leftptR+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100;
							if((FLOAT )rightskin/((rightptR-leftptR+1)*(FaceAttr[0].boobytop-FaceAttr[0].ppartytop+1))*100>=furtherthres && (FLOAT )(rightptR-leftptR+1)/(FaceAttr[0].width*2)*100>=extremethres)
							{
								pResults->NetPornValue=1.0f;
								pResults->pf=1;
								pResults->kosf=494;
								pResults->FT =11715;
								continueflag=FALSE;
							}
							
						}
					}
				} //end of checking if boob and private part exist
			} //end of lying body analysis
		} //end of condition three analysis
	}
	
	
	//release memory
	delete[] totalskinblob;				totalskinblob=NULL;
	delete[] totalskinstructure;		totalskinstructure=NULL;
	delete[] totalbodystructure;		totalbodystructure=NULL;
	delete[] pornbody_value;			pornbody_value=NULL;
	delete[] validbody;					validbody=NULL;
}


/*---------------------------------------------------------------------------
 //-  Function Name : QSortVector
               
 //-  Description   : Sorting the members of array structure
 //-  Parameter     : left - smallest index of an array
 right - largest index of an array
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::QSortVector(INT left,INT right)
{
	INT	i = left; 
	INT j = right;
	
	INT middle = FaceAttr[(left+right)/2].width;
	INT temp;
	bool temp2;
	
	for(;i<=j;)
	{
		for (;FaceAttr[i].width > middle && i < right;) i++;
		for (;middle > FaceAttr[j].width && j > left;) j--;
		
		if (i<=j)
		{
			//face
			temp = FaceAttr[i].width;
			FaceAttr[i].width = FaceAttr[j].width;
			FaceAttr[j].width = temp;
			
			temp = FaceAttr[i].height;
			FaceAttr[i].height = FaceAttr[j].height;
			FaceAttr[j].height = temp;
			
			temp = FaceAttr[i].x;
			FaceAttr[i].x = FaceAttr[j].x;
			FaceAttr[j].x = temp;
			
			temp = FaceAttr[i].y;
			FaceAttr[i].y = FaceAttr[j].y;
			FaceAttr[j].y = temp;
			
			//body
			temp = FaceAttr[i].bwidth;
			FaceAttr[i].bwidth = FaceAttr[j].bwidth;
			FaceAttr[j].bwidth = temp;
			
			temp = FaceAttr[i].bheight;
			FaceAttr[i].bheight = FaceAttr[j].bheight;
			FaceAttr[j].bheight = temp;
			
			temp = FaceAttr[i].bx;
			FaceAttr[i].bx = FaceAttr[j].bx;
			FaceAttr[j].bx = temp;
			
			temp = FaceAttr[i].by;
			FaceAttr[i].by = FaceAttr[j].by;
			FaceAttr[j].by = temp;
			
			temp = FaceAttr[i].area;
			FaceAttr[i].area = FaceAttr[j].area;
			FaceAttr[j].area = temp;
			
			temp = FaceAttr[i].bodyarea;
			FaceAttr[i].bodyarea = FaceAttr[j].bodyarea;
			FaceAttr[j].bodyarea = temp;
			
			temp = FaceAttr[i].facearea;
			FaceAttr[i].facearea = FaceAttr[j].facearea;
			FaceAttr[j].facearea = temp;
			
			//boob area
			temp = FaceAttr[i].boobydown;
			FaceAttr[i].boobydown = FaceAttr[j].boobydown;
			FaceAttr[j].boobydown = temp;
			
			temp = FaceAttr[i].boobytop;
			FaceAttr[i].boobytop = FaceAttr[j].boobytop;
			FaceAttr[j].boobytop = temp;
			
			temp2 = FaceAttr[i].boob;
			FaceAttr[i].boob = FaceAttr[j].boob;
			FaceAttr[j].boob = temp2;
			
			//private part area
			temp = FaceAttr[i].ppartydown;
			FaceAttr[i].ppartydown = FaceAttr[j].ppartydown;
			FaceAttr[j].ppartydown = temp;
			
			temp = FaceAttr[i].ppartytop;
			FaceAttr[i].ppartytop = FaceAttr[j].ppartytop;
			FaceAttr[j].ppartytop = temp;
			
			temp2 = FaceAttr[i].ppart;
			FaceAttr[i].ppart=FaceAttr[j].ppart;
			FaceAttr[j].ppart = temp2;
			
			i++;
			j--;
		}
	} 
	if (left<j) QSortVector(left,j);
	if (i<right) QSortVector(i,right);
};


/*---------------------------------------------------------------------------
 //-  Function Name : DetermineSkinBodyOptimised
                
 //-  Description   : Determine the total pixels and skin in the body structure
 //-  Parameter     : totalskinblob - total skin pixels in the valid skin blob
 totalskinstructure - total skin pixels in body structure with face
 totalbodystructure - total pixels in the geometric body structure
 blob_segment2 - information data in the blob
 object_map2 - array data referenced to blob identifier
 info - skin data
 newlabel - number of blob
 height - height of the image
 width - width of the image
 skinblobthres - threshold to determine the minimum skin level in the human structure
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::DetermineSkinBodyOptimised(INT *totalskinblob,INT *totalskinstructure,INT *totalbodystructure,ImageProcessing::simonBlob *blob_segment2,INT *object_map2,INT *info,INT newlabel,INT height,INT width,INT skinblobthres)
{
	INT i,/*j,*/h,w,hw=height*width;
	INT nostruct,faceref;
	INT partialskincount=0,elementcount=0,faceskin=0,faceblobskin=0;
	INT leftbody=0,rightbody=0,midpoINTbody=0;
	
	for(i=0;i<FaceAttr.size()+1;i++) 
	{
		totalskinblob[i]=0;
		totalskinstructure[i]=0;
		totalbodystructure[i]=0;
	}
	
	for(i=1;i<newlabel+1;i++)
	{
		if(blob_segment2[i].humanstruct_display.size()>0)
		{
			//full human blob
			if(blob_segment2[i].humanstruct_display[0]>0)
			{
				faceref=blob_segment2[i].humanstruct_display[0]-1;
				//Determine skin area in the human structure
				if(blob_segment2[i].inface==false)
				{
					//determine the body structure area
					totalbodystructure[blob_segment2[i].humanstruct_display[0]]+=blob_segment2[i].area;
					totalskinstructure[blob_segment2[i].humanstruct_display[0]]+=blob_segment2[i].skinTotal;
					if((float)blob_segment2[i].skinTotal/blob_segment2[i].area*100>=skinblobthres)
					{
						blob_segment2[i].humanskin_display.push_back(blob_segment2[i].humanstruct_display[0]);
						totalskinblob[blob_segment2[i].humanstruct_display[0]]+=blob_segment2[i].area;
					}
				}
				else
				{
					faceskin=0;
					faceblobskin=0;
					
					INT hStart = FaceAttr[faceref].y*width;
					INT *info_ptr=info+hStart;
					INT *blob_ptr=object_map2+hStart;
					for (h=hStart;h<hw;h++)
					{
						if (*blob_ptr==i)
						{
							faceblobskin++;
							if (*info_ptr>0) faceskin++;
						}
						info_ptr++;
						blob_ptr++;
					}
					
					totalskinstructure[blob_segment2[i].humanstruct_display[0]]+=(blob_segment2[i].skinTotal-faceskin);
					//determine the body structure area
					totalbodystructure[blob_segment2[i].humanstruct_display[0]]+=(blob_segment2[i].area-faceblobskin);
					
					if((float)blob_segment2[i].skinTotal/blob_segment2[i].area*100>=skinblobthres)
					{
						blob_segment2[i].humanskin_display.push_back(blob_segment2[i].humanstruct_display[0]);
						totalskinblob[blob_segment2[i].humanstruct_display[0]]+=(blob_segment2[i].area-faceblobskin);
					}
				}
			}
			else
				//partial human blob
			{
				//calculate the partial skin count
				for(nostruct=0;nostruct<blob_segment2[i].humanstruct_display.size();nostruct++)
				{
					partialskincount=0;
					elementcount=0;
					//body trunk
					INT hEnd = FaceAttr[nostruct].by+FaceAttr[nostruct].bheight;
					INT wStart = FaceAttr[nostruct].bx;
					INT wEnd = FaceAttr[nostruct].bx+FaceAttr[nostruct].bwidth;
					
					for(h=FaceAttr[nostruct].by;h<hEnd;h++) //new added remove =
					{
						INT *info_ptr=info+h*width+wStart;
						INT *blob_ptr=object_map2+h*width+wStart;
						
						for(w=wStart;w<wEnd;w++) //new added remove =
						{
							if(*blob_ptr++==i)
							{
								elementcount++;
								if(*info_ptr==3) partialskincount++;
							}
							info_ptr++;
						}
					}
					//calculate total skin in the body structure
					totalskinstructure[blob_segment2[i].humanstruct_display[nostruct]*-1]+=partialskincount;
					//determine the body structure area
					totalbodystructure[blob_segment2[i].humanstruct_display[nostruct]*-1]+=elementcount;
					
					//determine whether that particular partial blob of the body structure is skin partial blob or not
					if((float)partialskincount/elementcount*100>=skinblobthres)
					{
						blob_segment2[i].humanskin_display.push_back(blob_segment2[i].humanstruct_display[nostruct]);
						totalskinblob[blob_segment2[i].humanstruct_display[nostruct]*-1]+=elementcount;					
					}
				}
			}
		}
	}
}




/*---------------------------------------------------------------------------
 //-  Function Name : DetermineBodyOptimised
               
 //-  Description   : Determine the valid body structure and number of face skin
 //-  Parameter     : blob_segment2 - information data in the blob
 object_map2 - array data referenced to blob identifier
 newlabel - number of blob
 height - height of the image
 width - width of the image
 faceskin - number of skin pixels in the face area
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/
INT ImageProcessing::DetermineBodyOptimised(ImageProcessing::simonBlob *blob_segment2, INT *object_map2 ,INT newlabel,INT height,INT width,INT &faceSkin)
{
	INT i,j,k,h,w,hw=height*width;
	INT *bodyblob_area=new INT[newlabel+1]; 
	INT *facegroup_area=new INT[hw]; 
//	ZeroMemory(facegroup_area, sizeof(INT) * hw);
	memset(facegroup_area,0,sizeof(INT) * hw);
		   
	//---------------------------------------------------------------------------
	//- Label the face array with respective identifier
	//---------------------------------------------------------------------------
	INT faceSize = FaceAttr.size();
	for(i=0;i<faceSize;i++)
	{
		INT hEnd = FaceAttr[i].y+FaceAttr[i].height;
		INT wStart = FaceAttr[i].x;
		INT wEnd = FaceAttr[i].x+FaceAttr[i].width;
		for(h=FaceAttr[i].y;h<hEnd;h++)
		{
			INT *face_ptr = facegroup_area+h*width+wStart;
			for(w=wStart;w<wEnd;w++) 
				*face_ptr++ = faceSize;
		}
	}
	//---------------------------------------------------------------------------
	//- End to Label the face array with respective identifier
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Rearrange the data members related to face area, boob area and private part area
	//---------------------------------------------------------------------------
	QSortVector(0,FaceAttr.size()-1);
	//---------------------------------------------------------------------------
	//- End to Rearrange the data members related to face area, boob area and private part area
	//---------------------------------------------------------------------------
	
	// reset the counter for numer of blob pixels in the face
	faceSkin = 0;
	//////////////////////////////////////////////////////
	for(i=0;i<faceSize;i++)
	{
		for(j=0;j<newlabel+1;j++) bodyblob_area[j]=0;
		//Determine blob that belong to individual body structure
		//face portion
		INT hEnd = FaceAttr[i].y+FaceAttr[i].height;
		INT wStart = FaceAttr[i].x;
		INT wEnd = FaceAttr[i].x+FaceAttr[i].width;
		
		for(h=FaceAttr[i].y;h<hEnd;h++)
		{
			INT * o_ptr = object_map2+h*width+wStart;
			for(w=wStart;w<wEnd;w++)
			{
				if (*o_ptr>0)
				{
					if(blob_segment2[*o_ptr].humanstruct_display.size()==0)
					{
						bodyblob_area[*o_ptr]++;
						blob_segment2[*o_ptr].inface=TRUE;
					}
					faceSkin++;
				}
				o_ptr++;
			}
		}
		
		//body trunk portion
		hEnd = FaceAttr[i].by+FaceAttr[i].bheight;
		wStart = FaceAttr[i].bx;
		wEnd = FaceAttr[i].bx+FaceAttr[i].bwidth;
		
		for(h=FaceAttr[i].by;h<hEnd;h++)
		{
			INT *o_ptr = object_map2+h*width+wStart;
			INT *face_ptr = facegroup_area+h*width+wStart;
			for(w=wStart;w<wEnd;w++)
			{
				if (*o_ptr>0)
				{
					if(blob_segment2[*o_ptr].humanstruct_display.size()==0 && *face_ptr==0)
					{
						bodyblob_area[*o_ptr]++;
					}
				}
				o_ptr++;
				face_ptr++;
			}
		}
		
		for(k=1;k<newlabel+1;k++)
		{
			if(bodyblob_area[k]>0)
			{
				if((float)bodyblob_area[k]/blob_segment2[k].area*100>70)
				{
					blob_segment2[k].humanstruct_display.push_back(i+1);
				}
				else
				{
					if((float)bodyblob_area[k]/blob_segment2[k].area*100>40 && (float)bodyblob_area[k]/blob_segment2[k].area*100<=70)
					{
						if((float)bodyblob_area[k]/FaceAttr[i].area*100>10)
							blob_segment2[k].humanstruct_display.push_back((i+1)*-1);
					}
				}
			}
		}
	}
	
	////////////////////////////////////////////////////////////////////////////////////
	delete[] bodyblob_area;		bodyblob_area=NULL;
	delete[] facegroup_area;	facegroup_area=NULL;
	
	return FaceAttr.size();
}




/*---------------------------------------------------------------------------
 //-  Function Name :	inantColour
               
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
bool ImageProcessing::NoisyfacesColour(unsigned char *red,unsigned char *green,unsigned char *blue,INT height,INT width)
{	
	char homam[200];
	INT h;
	INT w;
	INT face_num=FaceAttr.size();
	
	
	unsigned char *red_ptr=red;
	unsigned char *green_ptr=green;
	unsigned char *blue_ptr=blue;
	
	const size_t ST_COLOUR_RANGE = 64;
	DWORD dwColourTable[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	INT width2 = 0;
	INT height2 = 0;
	
	DWORD ColourCount = 0;
	
	
	DWORD dwData;
	
	int totalfaceskin = 0;
	bool bRet = true;
	bool finalresult[100]; // after Debbuging emad
	
	
	for (int i= 0 ; i < face_num ; i++)
	{
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
				}
			}
		}
		
		width2 = FaceAttr[i].width;
		height2 = FaceAttr[i].height;
		finalresult[i]  = false;
		
		sprintf (homam , "%d Face Width , %d Face Height , %d Face x , %d Face y " , width2 , height2 , FaceAttr[i].x , FaceAttr[i].y);
		
		INT hEnd = FaceAttr[i].y+height2;
		INT wEnd = FaceAttr[i].x+width2;
		
		
		for(h=FaceAttr[i].y;h<hEnd;h++) //new added remove =
		{
			INT h_width = h*width;
			red_ptr   = red +  h_width+FaceAttr[i].x;
			green_ptr = green +h_width+FaceAttr[i].x;
			blue_ptr  = blue +h_width+FaceAttr[i].x;
			
			for(w=FaceAttr[i].x;w<wEnd;w++) //new added remove =
			{
				
				DWORD dwRed   = *(red_ptr++)>>2;
				DWORD dwGreen = *(green_ptr++)>>2;
				DWORD dwBlue  = *(blue_ptr++)>>2;
				
				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				
				if(RGBIsInCube(dwData))
				{
					totalfaceskin++;
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				
			}
			
		}
		
				
		
		if ((ColourCount < 600) &&(width>=200 || height >=200) &&(width2>30 || height2 >30)) // added in the new release 
			//***if((FaceAttr[i].x+ width2) > (width - width*0.01))
		{
			finalresult[i] = true;
		}
		bRet = bRet && finalresult[i];
	}
	
	return bRet;
	
}

/*---------------------------------------------------------------------------
 //-  Function Name :	inantColour
                
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/

bool ImageProcessing::FilterNoisyFaces(unsigned char *red,unsigned char *green,unsigned char *blue,INT height,INT width , BYTE* hbmp, INT nChannel , INT nDepth)
{	
	INT hw=height*width;
	INT h;
	INT w;
	INT face_num=FaceAttr.size();
	INT NoisyFaceInd[5];
	INT IndVal = -1;
	for (int ind=0 ; ind < 5 ; ind++)
		NoisyFaceInd[ind] = -1;
	unsigned char *red_ptr=red;
	unsigned char *green_ptr=green;
	unsigned char *blue_ptr=blue;
	unsigned char *red_ptr1=red;
	unsigned char *green_ptr1=green;
	unsigned char *blue_ptr1=blue;
	const size_t ST_COLOUR_RANGE = 64;
	DWORD dwColourTable[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	DWORD dwColourTable1[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	INT width2 = 0;
	INT height2 = 0;
	INT widthbytes2 = 0;	
	DWORD ColourCount = 0;
	DWORD ColourCount1 = 0;
	float NonSkinOverSkin = 0;
	FLOAT NoisyFace = FALSE;
	DWORD dwData;
	int totalfaceskin = 0;
	bool bRet = true;
	bool NoisyDetected = false; 
	
	

	
	for (int i= 0 ; i < face_num ; i++)
	{
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
					dwColourTable1[i1][j1][k1] =0;
					
				}
			}
		}
		
		ColourCount = 0;
		ColourCount1 = 0;
		NonSkinOverSkin = 0;
		NoisyFace = FALSE;
		width2 = FaceAttr[i].width;
		height2 = FaceAttr[i].height;
		INT hEnd = FaceAttr[i].y+height2 -  0.2 *  FaceAttr[i].height;
		INT wEnd = FaceAttr[i].x+width2 - 0.2 *  FaceAttr[i].width;
#ifdef _JHDebug_Image		
//		FILE *fdf;//JHDF
//		fdf=fopen("/Users/admin-imac2/Images/PornCategory/face_porn/Mac_debug_5_681696.txt", "a");
//		fprintf(fdf, "\n\thEnd=%d \twEnd=%d\t width2=%d\t height2=%d", hEnd,wEnd,width2,height2 );
//		fclose(fdf);
#endif
		for(h=FaceAttr[i].y + 0.2 *  FaceAttr[i].height ;h<hEnd;h++) //new added remove =
		{
			INT h_width = h*width;
			red_ptr   = red +  h_width+FaceAttr[i].x;
			green_ptr = green +h_width+FaceAttr[i].x;
			blue_ptr  = blue +h_width+FaceAttr[i].x;
			
			for(w=FaceAttr[i].x + 0.2 *  FaceAttr[i].width ;w<wEnd;w++) //new added remove =
			{
				
				DWORD dwRed   = *(red_ptr++)>>2;
				DWORD dwGreen = *(green_ptr++)>>2;
				DWORD dwBlue  = *(blue_ptr++)>>2;

				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				
				if(RGBIsInCube(dwData))
				{
					totalfaceskin++;
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				else 
				{
					if (dwColourTable1[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount1++;
					}
					dwColourTable1[dwRed][dwGreen][dwBlue]++;
				}
				
			}
			
		}
		
		if (ColourCount > 0)
			NonSkinOverSkin = (float) (ColourCount1*100 /ColourCount);
#ifdef _JHDebug_Image		
//		fprintf(fdf,	"\nif ((ColourCount < 10 || ColourCount1 < 10 || (NonSkinOverSkin > 200 && ColourCount < 100) || (NonSkinOverSkin > 400))  &&(width>=200 || height >=200) &&(width2>30 || height2 >30))" );
//		fprintf(fdf, "\nColourCount=%d \tColourCount1=%d \tNonSkinOverSkin=%.2f \t totalfaceskin=%d\n", ColourCount, ColourCount1,NonSkinOverSkin, totalfaceskin);
//		fclose(fdf);//JHDF
#endif		
		// NOISY Face Detected --- Not Enough Skiny tones or too Much non Skiny Tones 
		if ((ColourCount < 10 || ColourCount1 < 10 || (NonSkinOverSkin > 200 && ColourCount < 100) || (NonSkinOverSkin > 400))  &&(width>=200 || height >=200) &&(width2>30 || height2 >30)) // added in the new release 
		{
			
			if (i < 5) 
			{
				NoisyFace = TRUE;
				NoisyDetected = true;
				IndVal++;
				NoisyFaceInd[IndVal] = i;
			}
			
		}
		
		// Checking For Intersected Faces 
		int x11 = 0;
		int x12 = 0;
		int x21 = 0;
		int x22 = 0;
		int y11 = 0;
		int y12 = 0;
		int y21 = 0;
		int y22 = 0;
		int intersectedFaceVal = -1;
		
		
		if (face_num > 1 && NoisyFace == FALSE)
		{
			for (int j= i+1 ; j < face_num ; j++)
			{
				
				x11 = FaceAttr[i].x;
				x12 = FaceAttr[i].x + FaceAttr[i].width;
				x21 = FaceAttr[j].x;
				x22 = FaceAttr[j].x + FaceAttr[j].width;
				y11 = FaceAttr[i].y;
				y12 = FaceAttr[i].y + FaceAttr[i].height;
				y21 = FaceAttr[j].y;
				y22 = FaceAttr[j].y + FaceAttr[j].height;
				
				if (x21 > x11)
				{
					if ( (x21 +10 > x11 && x21 +10 < x12 && y22 > y11 && y22< y12 ))	
					{
						
						if (i < 5) 
						{
							NoisyDetected = true;
							IndVal++;
							NoisyFaceInd[IndVal] = j;
						}
						
					}
					
				}
				else 
				{
					if ( (x11 +10 > x21 && x11 +10 < x22 && y12 > y21 && y12< y22 ))	
					{
						intersectedFaceVal = i;
						
						if (i < 5) 
						{
							NoisyDetected = true;
							IndVal++;
							NoisyFaceInd[IndVal] = i;
						}
						
						
					}
				}
				
			}
		}
	}
	
	
	INT DeletedItem = 0;	
	
	for (int ind = 4 ; ind >= 0 ; ind--)
	{
		
		std::vector<FaceRec>::iterator iter = FaceAttr.begin() ; 

		if (NoisyFaceInd[ind] > -1  && (FaceAttr.size() > 1))
		{
			for (int i = 0 ; i < NoisyFaceInd[ind]- DeletedItem ; i++ )
			{
				++iter;
			}
			FaceAttr.erase(iter);
			DeletedItem++;
		}
		else if (NoisyFaceInd[ind] > -1  && (FaceAttr.size() == 1))
			FaceAttr.clear();
	}
	
	// Dump The Final Detected Faces After Filtering 
	IplImage* img ; 
//	bool bload = LoadHBITMAP(hbmp, &img);
	int bpp = (nDepth & 255) * nChannel ;
	bool bload = LoadHBITMAP(hbmp, &img, width, height, bpp);   

	if (bload)
	{
		CvPoint pt1, pt2;
		for (int i= 0 ; i < FaceAttr.size() ; i++)
		{
			
			if (ShowDetailedImage == TRUE)
			{
				char WinName[16]; // ShowDetailedImage
				char PreWinName[16];  // ShowDetailedImage
				pt1.x = FaceAttr[i].x;
				pt2.x = (FaceAttr[i].x+FaceAttr[i].width);
				pt1.y =img->height - FaceAttr[i].y;
				pt2.y = img->height - (FaceAttr[i].y+FaceAttr[i].height);
#ifdef _JHDebug_showImage						
				sprintf (WinName , "image%d" , NumThumbs);
				sprintf (PreWinName , "Image %d - Face" , NumThumbs);
				cvRectangle( img, pt1, pt2, CV_RGB(255*i,0,0), 3, 8, 0 );				
				cvShowImage(PreWinName , img); //JHR
#endif				
			
				
				if ( FaceAttr[i].RightSide == TRUE)
				{
					pt1.x = 10;
					pt2.x =  FaceAttr[i].x;
					pt1.y = height - ( FaceAttr[i].y + ( FaceAttr[i].height /2));
					
					if (( FaceAttr[i].y - (int)( FaceAttr[i].height*1.5))>0)
						pt2.y = height - (( FaceAttr[i].y - (int)( FaceAttr[i].height*1.5)));
					else pt2.y = 0;
					
				}
				else if ( FaceAttr[i].LeftSide == TRUE)
				{
					pt1.x =  FaceAttr[i].x +  FaceAttr[i].width;
					pt2.x = width -10;
					pt1.y = height - ( FaceAttr[i].y + ( FaceAttr[i].height /2));
					
					if (( FaceAttr[i].y - (int)( FaceAttr[i].height*1.5)) > 0)
						pt2.y = height - (( FaceAttr[i].y - (int)( FaceAttr[i].height*1.5)));
					else pt2.y = 0;
					
					
				}
				
				else 
				{
					pt1.x =  FaceAttr[i].bx;
					pt2.x =   FaceAttr[i].bx+  FaceAttr[i].bwidth;
					pt1.y = height -  FaceAttr[i].by -  FaceAttr[i].bheight*0.3 ;
					pt2.y = height - ( FaceAttr[i].by+ FaceAttr[i].bheight );
				}
				cvRectangle( img, pt1, pt2, CV_RGB(255*i,0,0), 3, 8, 0 );
				
			}
			
		}
		cvReleaseImage(&img);
	}
	
	return true;
	
}

/*---------------------------------------------------------------------------
 //-  Function Name :	inantColour
              
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
bool ImageProcessing::AnalyseDominantColourDump(unsigned char *red,unsigned char *green,unsigned char *blue,INT height,INT width ,/* HBITMAP hbmp ,*/ bool &NonPorn , float &fBodyskin , float &fFaceSkin)
{	
	INT hw=height*width;
	INT h;
	INT w;
	INT face_num=FaceAttr.size();
	if (face_num > 3) face_num = 3;
	unsigned char *red_ptr=red;
	unsigned char *green_ptr=green;
	unsigned char *blue_ptr=blue;
	unsigned char *red_ptr1=red;
	unsigned char *green_ptr1=green;
	unsigned char *blue_ptr1=blue;
	const size_t ST_COLOUR_RANGE = 64;
	DWORD dwColourTable[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	INT width2 = 0;
	INT height2 = 0;
	INT widthbytes2 = 0;	
	DWORD ColourCount = 0;
	INT  max1[3];
	INT  max2[3];
	INT  max3[3];
	INT  max1b[3];
	INT max2b[3];
	INT max3b[3];
	int maxColour1[3];
	int maxColour2[3];
	int maxColour3[3];
	int maxColour1b[3];
	int maxColour2b[3];
	int maxColour3b[3];
	bool finalresult[100];
	bool finalresultN[100];
	DWORD dwData;
	INT R1 = 0;
	int g1=0 , b1=0 , r2=0 , b2=0 , g2=0 , r3=0 , b3=0 , g3=0 ,  r4=0 , g4=0 , b4=0 , r5=0 , b5=0 , g5=0 , r6=0 , b6=0 , g6 = 0;
	int Diff1[3];
	int Diff2[3];
	int Diff3[3];
	int totDiff1 = 0;
	int totDiff2 = 0;
	int  totDiff3 = 0;
	int totalfaceskin = 0;
	int totalbodyskin = 0;
	int totalskindetected = 0;
	int totalskindetected1 = 0;
	int totalskindetected2 = 0;
	bool bRet = true;
	bool bRetN = true;
	int sRLeft = 0;
	int sBLeft = 0;
	int sGLeft = 0;
	int sRLeft1 = 0;
	int sBLeft1 = 0;
	int sGLeft1 = 0;
	int sRLeft2 = 0;
	int sBLeft2 = 0;
	int sGLeft2 = 0;
	//
	for (int h1 = 0 ; h1 < 3 ; h1++)
	{
		max1[h1] = 0;
		max2[h1] = 0;
		max3[h1] = 0;
		max1b[h1]= 0;
		max2b[h1]= 0;
		max3b[h1]= 0;
		maxColour1[h1] = 0;
		maxColour2[h1] = 0;
		maxColour3[h1] = 0;
		maxColour1b[h1]= 0;
		maxColour2b[h1]= 0;
		maxColour3b[h1]= 0;
		Diff1[h1]= 0;
		Diff2[h1]= 0;
		Diff3[h1]= 0;
		finalresult[h1] = false;
		finalresultN[h1] = false;
	}
	INT total6=0;
	
	for (int i= 0 ; i < face_num ; i++)
	{
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
				}
			}
		}
		
		width2 = FaceAttr[i].width;
		height2 = FaceAttr[i].height;
		INT hEnd = FaceAttr[i].y+height2 -  0.1 *  FaceAttr[i].height;
		INT wEnd = FaceAttr[i].x+width2 - 0.1 *  FaceAttr[i].width;
		for(h=FaceAttr[i].y + 0.1 *  FaceAttr[i].height ;h<hEnd;h++) //new added remove =
		{
			INT h_width = h*width;
			red_ptr   = red +  h_width+FaceAttr[i].x;
			green_ptr = green +h_width+FaceAttr[i].x;
			blue_ptr  = blue +h_width+FaceAttr[i].x;
			
			for(w=FaceAttr[i].x +  0.1 *  FaceAttr[i].width;w<wEnd;w++) //new added remove =
			{
				
				DWORD dwRed   = *(red_ptr++)>>2;
				DWORD dwGreen = *(green_ptr++)>>2;
				DWORD dwBlue  = *(blue_ptr++)>>2;
				
				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				
				if(RGBIsInCube(dwData))
				{
					totalfaceskin++;
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				
			}
			
		}
		// find most popular colour...
		for (INT xi=0;xi<64;xi++)
			for (INT xj=0;xj<64;xj++)	
				for (INT xk=0;xk<64;xk++)
				{
					if (max1[i]<dwColourTable[xi][xj][xk])
					{
						r3 = r2 ;
						b3 = b2 ;
						g3 = g2;
						max3[i]=max2[i];
						r2 = R1;
						b2 = b1;
						g2 = g1;
						max2[i]=max1[i];
						R1 = xi ;
						g1 = xj ;
						b1 = xk;
						max1[i]=dwColourTable[xi][xj][xk];
						maxColour3[i]=maxColour2[i];
						maxColour2[i]=maxColour1[i];
						maxColour1[i]=RGB(xk*4,xj*4,xi*4);
						
					}
					else if (max2[i]<dwColourTable[xi][xj][xk])
					{
						max3[i]=max2[i];
						r3 = r2 ;
						b3 = b2 ;
						g3 = g2;
						max2[i]=dwColourTable[xi][xj][xk];
						r2 = xi ;
						g2 = xj ;
						b2 = xk;
						maxColour3[i]=maxColour2[i];
						maxColour2[i]=RGB(xk*4,xj*4,xi*4);
						
						
					}
					else if (max2[i]<dwColourTable[xi][xj][xk])
					{
						max3[i]=dwColourTable[xi][xj][xk];
						r3 = xi ;
						g3 = xj ;
						b3 = xk;
						maxColour3[i]=RGB(xk*4,xj*4,xi*4);
						
					}
					total6+=dwColourTable[xi][xj][xk];
				}
		
		int sh = 0;
		int shw =0;
		int hw = 0;
		
		if (FaceAttr[i].RightSide == TRUE)
		{
			//MessageBox (NULL , _T("Right") , _T("******") , NULL);
			hEnd = FaceAttr[i].y + FaceAttr[i].height/2 ;
			wEnd = FaceAttr[i].x;
			sh = 0;
			
			if ((FaceAttr[i].y - (int)(FaceAttr[i].height*1.5)) > 0)
				sh = (FaceAttr[i].y - (int)(FaceAttr[i].height*1.5));
			else sh = 0;
			
			shw = 10;
			hw = (FaceAttr[i].x-20) * (FaceAttr[i].height*1.5 - FaceAttr[i].height*1.5*0.3) ;
		}
		
		if (FaceAttr[i].LeftSide == TRUE)
		{
			hEnd = FaceAttr[i].y + FaceAttr[i].height/2;
			wEnd = width - 10;
			sh = 0;
			if ((FaceAttr[i].y - (int)(FaceAttr[i].height*1.5)) > 0)
				sh = (FaceAttr[i].y - (int)(FaceAttr[i].height*1.5));
			else sh = 0;
			shw = (FaceAttr[i].x + FaceAttr[i].width);
			hw = (width - 20 - (FaceAttr[i].x + FaceAttr[i].width))* (FaceAttr[i].height*1.5 - FaceAttr[i].height*1.5*0.3) ;
		}
		
		else 
		{
			hEnd = FaceAttr[i].by+FaceAttr[i].bheight - FaceAttr[i].bheight*0.3;
			wEnd = FaceAttr[i].bx+FaceAttr[i].bwidth;
			sh = FaceAttr[i].by;
			shw = FaceAttr[i].bx;
			hw = FaceAttr[i].bwidth * (FaceAttr[i].bheight - FaceAttr[i].bheight*0.3) ;
		}
		
		dwData=0;
		totalbodyskin = 0;
		totalskindetected =0;
		ColourCount =0;
		
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
				}
			}
		}
		
		for(h=sh;h<hEnd;h++)
		{
			INT h_width = h*width;
			red_ptr1   = red   +  h_width+FaceAttr[i].bx;
			green_ptr1 = green +  h_width+FaceAttr[i].bx;
			blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
			
			for(w=shw;w<wEnd;w++)
			{
				DWORD dwRed   = *(red_ptr1++)>>2;
				DWORD dwGreen = *(green_ptr1++)>>2;
				DWORD dwBlue  = *(blue_ptr1++)>>2;
				
				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				if(RGBIsInCube(dwData))
				{
					totalbodyskin++;
					
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				
				sRLeft = R1 -5;
				if (sRLeft < 0)  sRLeft = 0;
				sBLeft = b1 -5;
				if (sBLeft < 0)  sBLeft = 0;
				sGLeft = g1 -5;
				if (sGLeft < 0)  sGLeft = 0;
				
				
				if ((dwRed >= sRLeft)&& (dwRed <= R1 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b1 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g1 +5))
				{
					totalskindetected++;
				}
			}
		}
		
		float fres2 =0.0;
		float fres3 =0.0;
		float fres1 = (float)totalskindetected /(float)totalbodyskin;
		fres2 = (float)totalbodyskin / (float) hw;
		fres3 = (float)totalskindetected / (float) hw;
		
		if (fres2 <= 0.65 || fres3 <=0.1) // try the second detected face skin tone 
		{
			totalskindetected =0;
			totalskindetected1 = 0;
			totalskindetected2 = 0;
			for(h=sh;h<hEnd;h++)
			{
				INT h_width = h*width;
				red_ptr1   = red   +  h_width+FaceAttr[i].bx;
				green_ptr1 = green +  h_width+FaceAttr[i].bx;
				blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
				
				for(w=shw;w<wEnd;w++)
				{
					DWORD dwRed   = *(red_ptr1++)>>2;
					DWORD dwGreen = *(green_ptr1++)>>2;
					DWORD dwBlue  = *(blue_ptr1++)>>2;
					
					sRLeft = r2 -5;
					if (sRLeft < 0)  sRLeft = 0;
					sBLeft = b2 -5;
					if (sBLeft < 0)  sBLeft = 0;
					sGLeft = g2 -5;
					if (sGLeft < 0)  sGLeft = 0;
					
					
					if ((dwRed >= sRLeft)&& (dwRed <= r2 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b2 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g2 +5))
					{
						totalskindetected++;
					}
				}
			}
			
			
			fres1 = (float)totalskindetected /(float)totalbodyskin;
			float frestemp2 =0.0;
			float frestemp3 = 0.0;
			
			frestemp2 = (float)totalbodyskin / (float) hw;
			frestemp3 = (float)totalskindetected / (float) hw;
			if (frestemp2 > fres2)
				fres2 = frestemp2;
			if (frestemp3 > fres3)
				fres3 = frestemp3 ;
			
		}
		
		
		if (fres2 <= 0.65 || fres3 <=0.1) // try the third detected face skin tone 
		{
			
			totalskindetected =0.0;
			
			
			for(h=sh;h<hEnd;h++)
			{
				INT h_width = h*width;
				red_ptr1   = red   +  h_width+FaceAttr[i].bx;
				green_ptr1 = green +  h_width+FaceAttr[i].bx;
				blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
				
				for(w=shw;w<wEnd;w++)
				{
					DWORD dwRed   = *(red_ptr1++)>>2;
					DWORD dwGreen = *(green_ptr1++)>>2;
					DWORD dwBlue  = *(blue_ptr1++)>>2;
					
					sRLeft = r3 -5;
					if (sRLeft < 0)  sRLeft = 0;
					sBLeft = b3 -5;
					if (sBLeft < 0)  sBLeft = 0;
					sGLeft = g3 -5;
					if (sGLeft < 0)  sGLeft = 0;
					
					
					if ((dwRed >= sRLeft)&& (dwRed <= r3 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b3 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g3 +5))
					{
						totalskindetected++;
					}
					
					
				}
			}
			
			fres1 = (float)totalskindetected /(float)totalbodyskin;
			float frestemp2 =0.0;
			float frestemp3 = 0.0;
			
			frestemp2 = (float)totalbodyskin / (float) hw;
			frestemp3 = (float)totalskindetected / (float) hw;
			
			if (frestemp2 > fres2)
				fres2 = frestemp2;
			if (frestemp3 > fres3)
				fres3 = frestemp3 ;
			
		}
		
		
		fBodyskin += fres2;
		fFaceSkin += fres3;
		
		if ((fres2 > 0.6 && fres3 > 0.1) || (totalbodyskin>12000 && fres3 > 0.1) )// && (totalbodyskin> 500 && totalskindetected > 500 ))
			finalresult[i] = true;
		
		
		if ((fres2 < 0.3 && fres3 < 0.04))// || (totalbodyskin <&& fres3 > 0.1) )// && (totalbodyskin> 500 && totalskindetected > 500 ))
			finalresultN[i] = true;
		
		
	}
	
	for (int i= 0 ; i <face_num ; i++ )
	{
		bRet = bRet && finalresult[i]; // all the faces have to pass the condition
	}
	
	for (int i= 0 ; i <face_num ; i++ )
	{
		bRetN = bRetN && finalresultN[i]; // all the faces have to pass the condition
	}
	
	NonPorn = bRetN;
	
	fBodyskin = (float) fBodyskin/face_num;
	fFaceSkin = (float) fFaceSkin/face_num;
	
	return bRet;
	
}


/*---------------------------------------------------------------------------
 //-  Function Name : AnalyseDominantColour
               
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
bool ImageProcessing::Noisyfaces(INT height,INT width)
{	
	//	char homam[200];
	INT face_num=FaceAttr.size();
	bool bRet = true;
	bool finalresult[100];
	
	INT width2 = 0;
	INT height2 = 0;
	
	
	for (int i= 0 ; i < face_num ; i++)
	{
		finalresult[i]  = false;
		width2 = FaceAttr[i].width;
		height2 = FaceAttr[i].height;
		
		if (( width2 <= width*0.08 ) || ((FaceAttr[i].y < height *0.22) && ( width2 <= width*0.12 )) || ((FaceAttr[i].x < width *0.05)&& ( width2 <= width*0.12 )) ||  (((FaceAttr[i].y+ height2) > (height - height*0.02)) && ( width2 <= width*0.12 )) || (((FaceAttr[i].x+ width2) > (width - width*0.01))&& ( width2 <= width*0.12 )))
		{
			finalresult[i] = true;
		}
		bRet = bRet && finalresult[i];
		
	}
	
	return bRet;
	
}




/*---------------------------------------------------------------------------
 //-  Function Name :	inantColour
                
 //-  Description   : Apply gamma correction on red, green and blue components of the image
 //-  Parameter     : red - data in red component of the image
 green - data in green component of the image
 blue - data in blue component of the image
 bw - grayscale component of the colour image
 height - height of the image
 width - width of the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
bool ImageProcessing::AnalyseDominantColour(unsigned char *red,unsigned char *green,unsigned char *blue,INT height,INT width)
{	
	//char homam[200];
	INT hw=height*width;
	INT h;
	INT w;
	INT face_num=FaceAttr.size();
	
	
	unsigned char *red_ptr=red;
	unsigned char *green_ptr=green;
	unsigned char *blue_ptr=blue;
	unsigned char *red_ptr1=red;
	unsigned char *green_ptr1=green;
	unsigned char *blue_ptr1=blue;
	const size_t ST_COLOUR_RANGE = 64;
	DWORD dwColourTable[ST_COLOUR_RANGE][ST_COLOUR_RANGE][ST_COLOUR_RANGE] = {0};
	INT width2 = 0;
	INT height2 = 0;
	INT widthbytes2 = 0;	
	DWORD ColourCount = 0;
	
	INT  max1[3];//= new int[face_num];
	INT  max2[3];//= new int[face_num];
	INT  max3[3];//=new int[face_num];
	INT  max1b[3];
	INT max2b[3];
	INT max3b[3];
	int maxColour1[3];
	int maxColour2[3];
	int maxColour3[3];
	int maxColour1b[3];
	int maxColour2b[3];
	int maxColour3b[3];
	bool finalresult[100];
	DWORD dwData;
	INT R1 = 0;
	int g1=0 , b1=0 , r2=0 , b2=0 , g2=0 , r3=0 , b3=0 , g3=0 ,  r4=0 , g4=0 , b4=0 , r5=0 , b5=0 , g5=0 , r6=0 , b6=0 , g6 = 0;
	int Diff1[3];
	int Diff2[3];
	int Diff3[3];
	int totalfaceskin = 0;
	int totalbodyskin = 0;
	int totalskindetected = 0;
	int totalskindetected1 = 0;
	int totalskindetected2 = 0;
	bool bRet = true;
	int sRLeft = 0;
	int sBLeft = 0;
	int sGLeft = 0;
	//
	for (int h1 = 0 ; h1 < 3 ; h1++)
	{
		max1[h1] = 0;
		max2[h1] = 0;
		max3[h1] = 0;
	    max1b[h1]= 0;
		max2b[h1]= 0;
		max3b[h1]= 0;
		maxColour1[h1] = 0;
		maxColour2[h1] = 0;
		maxColour3[h1] = 0;
		maxColour1b[h1]= 0;
		maxColour2b[h1]= 0;
		maxColour3b[h1]= 0;
		Diff1[h1]= 0;
		Diff2[h1]= 0;
		Diff3[h1]= 0;
		finalresult[h1] = false;
	}
	INT total6=0;
	
	//	sprintf (emad , "Image W = %d , Image H = %d" , width , height);
	//	MessageBox (NULL , emad , "*************" , NULL);
	
	//	sprintf (emad , "%d face Number" , face_num);
	//	MessageBox (NULL , emad , "*************" , NULL);
	
	
	for (int i= 0 ; i < face_num ; i++)
	{
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
				}
			}
		}
		
		width2 = FaceAttr[i].width;
		height2 = FaceAttr[i].height;
		
		//		sprintf (emad , "%d Face Width , %d Face Height , %d Face x , %d Face y " , width2 , height2 , FaceAttr[i].x , FaceAttr[i].y);
		//		MessageBox (NULL , emad , "*************" , NULL);
		
		INT hEnd = FaceAttr[i].y+height2;
		INT wEnd = FaceAttr[i].x+width2;
		
		//		sprintf (emad , "hstart=%d , hEnd=%d  , wstart= %d wEnd= %d" ,FaceAttr[i].y, hEnd , FaceAttr[i].x ,wEnd);
		//		MessageBox (NULL , emad , "*************" , NULL);
		
		
		for(h=FaceAttr[i].y;h<hEnd;h++) //new added remove =
		{
			INT h_width = h*width;
			red_ptr   = red +  h_width+FaceAttr[i].x;
			green_ptr = green +h_width+FaceAttr[i].x;
			blue_ptr  = blue +h_width+FaceAttr[i].x;
			
			for(w=FaceAttr[i].x;w<wEnd;w++) //new added remove =
			{
				
				DWORD dwRed   = *(red_ptr++)>>2;
				DWORD dwGreen = *(green_ptr++)>>2;
				DWORD dwBlue  = *(blue_ptr++)>>2;
				
				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				
				if(RGBIsInCube(dwData))
				{
					totalfaceskin++;
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				
			}
			
			
		}
		
		
		// find most popular colour...
		for (INT xi=0;xi<64;xi++)
			for (INT xj=0;xj<64;xj++)	
				for (INT xk=0;xk<64;xk++)
				{
					if (max1[i]<dwColourTable[xi][xj][xk])
					{
						r3 = r2 ;
						b3 = b2 ;
						g3 = g2;
						max3[i]=max2[i];
						r2 = R1;
						b2 = b1;
						g2 = g1;
						max2[i]=max1[i];
						R1 = xi ;
						g1 = xj ;
						b1 = xk;
						max1[i]=dwColourTable[xi][xj][xk];
						maxColour3[i]=maxColour2[i];
						maxColour2[i]=maxColour1[i];
						maxColour1[i]=RGB(xk*4,xj*4,xi*4);
						
					}
					else if (max2[i]<dwColourTable[xi][xj][xk])
					{
						max3[i]=max2[i];
						r3 = r2 ;
						b3 = b2 ;
						g3 = g2;
						max2[i]=dwColourTable[xi][xj][xk];
						r2 = xi ;
						g2 = xj ;
						b2 = xk;
						maxColour3[i]=maxColour2[i];
						maxColour2[i]=RGB(xk*4,xj*4,xi*4);
						
						
					}
					else if (max2[i]<dwColourTable[xi][xj][xk])
					{
						max3[i]=dwColourTable[xi][xj][xk];
						r3 = xi ;
						g3 = xj ;
						b3 = xk;
						maxColour3[i]=RGB(xk*4,xj*4,xi*4);
						
					}
					total6+=dwColourTable[xi][xj][xk];
				}
		
			
		hEnd = FaceAttr[i].by+FaceAttr[i].bheight - FaceAttr[i].bheight*0.3;
		wEnd = FaceAttr[i].bx+FaceAttr[i].bwidth;
		
		
		dwData=0;
		totalbodyskin = 0;
		totalskindetected =0;
		ColourCount =0;
		
		for (int i1 = 0 ; i1 < 64 ; i1++)
		{
			for (int j1 = 0 ; j1 < 64 ; j1++)
			{
				for (int k1 = 0 ; k1 < 64 ; k1++)
				{
					dwColourTable[i1][j1][k1] =0;
				}
			}
		}
		
		//INT hStart = FaceAttr[i].by;
		
		
		
		for(h=FaceAttr[i].by;h<hEnd;h++)
		{
			INT h_width = h*width;
			red_ptr1   = red   +  h_width+FaceAttr[i].bx;
			green_ptr1 = green +  h_width+FaceAttr[i].bx;
			blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
			
			for(w=FaceAttr[i].bx;w<wEnd;w++)
			{
				DWORD dwRed   = *(red_ptr1++)>>2;
				DWORD dwGreen = *(green_ptr1++)>>2;
				DWORD dwBlue  = *(blue_ptr1++)>>2;
				
				
				
				dwData=RGB(dwBlue*4,dwGreen*4,dwRed*4);
				if(RGBIsInCube(dwData))
				{
					totalbodyskin++;
					
					if (dwColourTable[dwRed][dwGreen][dwBlue] == 0)
					{
						ColourCount++;
					}
					dwColourTable[dwRed][dwGreen][dwBlue]++;
				}
				
				
				sRLeft = R1 -5;
				if (sRLeft < 0)  sRLeft = 0;
				sBLeft = b1 -5;
				if (sBLeft < 0)  sBLeft = 0;
				sGLeft = g1 -5;
				if (sGLeft < 0)  sGLeft = 0;
				
								
				if ((dwRed >= sRLeft)&& (dwRed <= R1 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b1 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g1 +5))
				{
					totalskindetected++;
				}
				
							
			}
		}
		
		int hw = FaceAttr[i].bwidth * (FaceAttr[i].bheight - FaceAttr[i].bheight*0.3) ;
				
		
		float fres2 =0.0;
		float fres3 =0.0;
		float fres1 = (float)totalskindetected /(float)totalbodyskin;
		fres2 = (float)totalbodyskin / (float) hw;
		fres3 = (float)totalskindetected / (float) hw;
		
		
		
		
		if (fres2 <= 0.65 || fres3 <=0.1) // try the second detected face skin tone 
		{
			
			totalskindetected =0;
			totalskindetected1 = 0;
			totalskindetected2 = 0;
			
			
			for(h=FaceAttr[i].by;h<hEnd;h++)
			{
				INT h_width = h*width;
				red_ptr1   = red   +  h_width+FaceAttr[i].bx;
				green_ptr1 = green +  h_width+FaceAttr[i].bx;
				blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
				
				for(w=FaceAttr[i].bx;w<wEnd;w++)
				{
					DWORD dwRed   = *(red_ptr1++)>>2;
					DWORD dwGreen = *(green_ptr1++)>>2;
					DWORD dwBlue  = *(blue_ptr1++)>>2;
					
					sRLeft = r2 -5;
					if (sRLeft < 0)  sRLeft = 0;
					sBLeft = b2 -5;
					if (sBLeft < 0)  sBLeft = 0;
					sGLeft = g2 -5;
					if (sGLeft < 0)  sGLeft = 0;
					
					if ((dwRed >= sRLeft)&& (dwRed <= r2 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b2 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g2 +5))
					{
						totalskindetected++;
					}
					
					
					
					
					
				}
			}
			
					
			fres1 = (float)totalskindetected /(float)totalbodyskin;
			float frestemp2 =0.0;
			float frestemp3 = 0.0;
			
			frestemp2 = (float)totalbodyskin / (float) hw;
			frestemp3 = (float)totalskindetected / (float) hw;
		
			if (frestemp2 > fres2)
				fres2 = frestemp2;
			if (frestemp3 > fres3)
				fres3 = frestemp3 ;
			
		}
		
		
		if (fres2 <= 0.65 || fres3 <=0.1) // try the third detected face skin tone 
		{
			
			totalskindetected =0.0;
			
			
			for(h=FaceAttr[i].by;h<hEnd;h++)
			{
				INT h_width = h*width;
				red_ptr1   = red   +  h_width+FaceAttr[i].bx;
				green_ptr1 = green +  h_width+FaceAttr[i].bx;
				blue_ptr1  = blue  +  h_width+FaceAttr[i].bx;
				
				for(w=FaceAttr[i].bx;w<wEnd;w++)
				{
					DWORD dwRed   = *(red_ptr1++)>>2;
					DWORD dwGreen = *(green_ptr1++)>>2;
					DWORD dwBlue  = *(blue_ptr1++)>>2;
					
					sRLeft = r3 -5;
					if (sRLeft < 0)  sRLeft = 0;
					sBLeft = b3 -5;
					if (sBLeft < 0)  sBLeft = 0;
					sGLeft = g3 -5;
					if (sGLeft < 0)  sGLeft = 0;
					
					
					
					if ((dwRed >= sRLeft)&& (dwRed <= r3 +5)&& (dwBlue >= sBLeft)&& (dwBlue <= b3 +5) && (dwGreen >= sGLeft)&& (dwGreen <= g3 +5))
					{
						totalskindetected++;
					}
					
									
					
					
				}
			}
			
						
			fres1 = (float)totalskindetected /(float)totalbodyskin;
			float frestemp2 =0.0;
			float frestemp3 = 0.0;
			
			frestemp2 = (float)totalbodyskin / (float) hw;
			frestemp3 = (float)totalskindetected / (float) hw;
			
			if (frestemp2 > fres2)
				fres2 = frestemp2;
			if (frestemp3 > fres3)
				fres3 = frestemp3 ;
			
		}
		
		
		if ((fres2 > 0.65 && fres3 > 0.1) || (totalbodyskin>12000 && fres3 > 0.1) )// && (totalbodyskin> 500 && totalskindetected > 500 ))
			finalresult[i] = true;
		else break;
		
		
				
	}
	

	for (int i= 0 ; i <face_num ; i++ )
	{
		bRet = bRet && finalresult[i]; // all the faces have to pass the condition
	}
	
	
	return bRet;
	
}

/*---------------------------------------------------------------------------
 //-  Function Name : DiamondSkinOptimal
               
 //-  Description   : Determine the total skin pixel and normalized value in the center diamond shape area in the image
 //-  Parameter     : info - skin data
 width - width of the image
 height - height of the image
 diamondSkin - normalized skin pixel percentage in the center diamond shape area in the image
 skinInDiamond - total skin pixels in the center diamond shape area in the image
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::DiamondSkinOptimal(INT *info, INT width, INT height, FLOAT &diamondSkin, INT &skinInDiamond)
{
	INT total7=0;
	INT totalSkin=0;
	INT width2 = width/2;
	INT hstart = height/4; // Height/3
	INT hend = (3*height)/4; // 2*height/3
	
	for (INT h=hstart;h<hend;h++)
	{
		INT smallest = h - hstart;
		if (smallest>hend-h) smallest = hend-h;
		INT wstart = width2-((smallest*width)/height);
		INT wend = width2+((smallest*width)/height);
		total7+=(wend-wstart);
		INT Pos=h*width+wstart;
		for (INT w=wstart;w<wend;w++)
		{	
			if (info[Pos++]>0) totalSkin++;
		}
	}
	diamondSkin = (float) ((float)totalSkin)/((float)total7);
	skinInDiamond = totalSkin;
}


/*---------------------------------------------------------------------------
 //-  Function Name : IsSkin
               
 //-  Description   : Determine the investigated area is skin area or not
 //-  Parameter     : info - skin data
 iWidth - width of the image
 h - y coordinate of the center point of the investigated area
 w - x coordinate of the center point of the investigated area
 //-  Return Value  : TRUE if success, FALSE if otherwise
 //-------------------------------------------------------------------------*/
bool ImageProcessing::IsSkin(INT *info, INT iWidth, INT h, INT w)
{
	INT total10=0;
	//at the middle of the image position
	info+=((h*6-3)*iWidth)+w*6-3;
	for (INT i=-3;i<=3;i++)
	{
		for (INT j=-3;j<=3;j++)
		{
			if (*info>0) total10++;
			info++;
		}
		info+=iWidth-7;
	}
	
	if (total10>18) return TRUE;
	else return FALSE;
}


/*---------------------------------------------------------------------------
 //-  Function Name : CurvaturePoint            
 //-  Description   : Compute the number of curvature points from the skin blobs
 //-  Parameter     : bw - grayscale component of the colour image
 info - skin data
 height - height of the image
 width - width of the image
 direction - array contains the direction of the angles
 directionIn - number of brightness points flowing in
 directionOut - number of brightness points flowing out
 //-  Return Value  : NULL
 //-------------------------------------------------------------------------*/
VOID ImageProcessing::CurvaturePoint(unsigned char *bw, INT *info, INT height, INT width, INT *direction, INT *directionIn, INT *directionOut)
{
	// Apply a gaussian smoothing to greyscale image	
	INT hw = height*width;
	INT h,w;
	DOUBLE total8 = 0.0;
	
	INT * bw2 = new INT[hw];
	
	for (h=0;h<hw;h++) bw2[h]=bw[h];
	
	INT *bwptr;
	INT *bw2ptr = bw2;
	
	INT valh = height-9;
	INT valw = width-9;
	
	// Gaussina Filter Based on Gaussian Value 4
	w=0;
	INT hh=0;
	
	for (h=0;h<hw;h++) 
	{			
		if (w>9 && w<valw && hh>9 && hh<valh)
		{
			total8 = 0;
			bwptr = &bw2[h-9];
			
			total8+=*bwptr++ ;
			total8+=*bwptr++ << 1;
			total8+=*bwptr++ << 1;
			total8+=*bwptr++ << 2;
			total8+=*bwptr++ << 3;
			total8+=*bwptr++ << 4;
			total8+=*bwptr++ << 4;
			total8+=*bwptr++ << 5;
			total8+=*bwptr++ << 5;
			total8+=*bwptr++ << 5;
			total8+=*bwptr++ << 5;
			total8+=*bwptr++ << 5;
			total8+=*bwptr++ << 4;
			total8+=*bwptr++ << 4;
			total8+=*bwptr++ << 3;
			total8+=*bwptr++ << 2;
			total8+=*bwptr++ << 1;
			total8+=*bwptr++ << 1;
			total8+=*bwptr   ;
			
			bwptr = bw2;
			bwptr += h-9*width;
			
			total8+=*bwptr ;
			bwptr+=width;
			total8+=*bwptr << 1;
			bwptr+=width;
			total8+=*bwptr << 1;
			bwptr+=width;
			total8+=*bwptr << 2;
			bwptr+=width;
			total8+=*bwptr << 3;
			bwptr+=width;
			total8+=*bwptr << 4;
			bwptr+=width;
			total8+=*bwptr << 4;
			bwptr+=width;
			total8+=*bwptr << 5;
			bwptr+=width;
			total8+=*bwptr << 5;
			bwptr+=width;
			total8+=*bwptr << 5;
			bwptr+=width;
			total8+=*bwptr << 5;
			bwptr+=width;
			total8+=*bwptr << 5;
			bwptr+=width;
			total8+=*bwptr << 4;
			bwptr+=width;
			total8+=*bwptr << 4;
			bwptr+=width;
			total8+=*bwptr << 3;
			bwptr+=width;
			total8+=*bwptr << 2;
			bwptr+=width;
			total8+=*bwptr << 1;
			bwptr+=width;
			total8+=*bwptr << 1;
			bwptr+=width;
			total8+=*bwptr ;	
		}
		*bw2ptr++ = (INT)total8 >> 9;					
		w++;
		if (w>=width) { w=0;hh++;}
	}
	
	INT ww;
	for (h=0; h<height/6;h++)
	{
		for (w=0;w<width/6;w++)
		{
			INT valh = h*6;
			INT valw = w*6;
			
			INT top = 0;
			for (ww=0;ww<6;ww++) top+=bw2[((valh+5)*width)+valw +ww];
			for (ww=1;ww<5;ww++) top+=bw2[((valh+4)*width)+valw +ww];
			for (ww=2;ww<4;ww++) top+=bw2[((valh+3)*width)+valw +ww];
            
			INT bottom = 0;
			for (ww=2;ww<4;ww++) bottom+=bw2[((valh+2)*width)+valw+ww];
			for (ww=1;ww<5;ww++) bottom+=bw2[((valh+1)*width)+valw+ww];
			for (ww=0;ww<6;ww++) bottom+=bw2[((valh+0)*width)+valw+ww];
			
			INT left = 0;
			for (hh=0;hh<6;hh++) left+=bw2[((valh+hh)*width)+valw+0];
			for (hh=1;hh<5;hh++) left+=bw2[((valh+hh)*width)+valw+1];
			for (hh=2;hh<4;hh++) left+=bw2[((valh+hh)*width)+valw+2];
			
			INT right = 0;
			for (hh=2;hh<4;hh++) right+=bw2[((valh+hh)*width)+valw+3];
			for (hh=1;hh<5;hh++) right+=bw2[((valh+hh)*width)+valw+4];
			for (hh=0;hh<6;hh++) right+=bw2[((valh+hh)*width)+valw+5];
			
			INT topleft = 0;
			for (hh=2;hh<6;hh++) topleft+=bw2[((valh+hh)*width)+valw+0];
			for (hh=3;hh<6;hh++) topleft+=bw2[((valh+hh)*width)+valw+1];
			for (hh=4;hh<6;hh++) topleft+=bw2[((valh+hh)*width)+valw+2];
			for (hh=5;hh<6;hh++) topleft+=bw2[((valh+hh)*width)+valw+3];
			
			INT bottomleft = 0;
			for (hh=0;hh<4;hh++) bottomleft+=bw2[((valh+hh)*width)+valw+0];
			for (hh=0;hh<3;hh++) bottomleft+=bw2[((valh+hh)*width)+valw+1];
			for (hh=0;hh<2;hh++) bottomleft+=bw2[((valh+hh)*width)+valw+2];
			for (hh=0;hh<1;hh++) bottomleft+=bw2[((valh+hh)*width)+valw+3];
			
			INT topright = 0;
			for (hh=2;hh<6;hh++) topright+=bw2[((valh+hh)*width)+valw+5];
			for (hh=3;hh<6;hh++) topright+=bw2[((valh+hh)*width)+valw+4];
			for (hh=4;hh<6;hh++) topright+=bw2[((valh+hh)*width)+valw+3];
			for (hh=5;hh<6;hh++) topright+=bw2[((valh+hh)*width)+valw+2];
			
			INT bottomright = 0;
			for (hh=0;hh<4;hh++) bottomright+=bw2[((valh+hh)*width)+valw+5];
			for (hh=0;hh<3;hh++) bottomright+=bw2[((valh+hh)*width)+valw+4];
			for (hh=0;hh<2;hh++) bottomright+=bw2[((valh+hh)*width)+valw+3];
			for (hh=0;hh<1;hh++) bottomright+=bw2[((valh+hh)*width)+valw+2];
			
			bool t_b = FALSE;
			INT t_b_v = bottom - top;
			bool l_r = FALSE;
			INT l_r_v = right - left;
			bool bl_tr = FALSE;
			INT bl_tr_v = topright - bottomleft;
			bool tl_br = FALSE;
			INT tl_br_v = bottomright - topleft;
			
			if (top>bottom) {t_b = TRUE; t_b_v = top - bottom;}
			if (left>right) {l_r = TRUE; l_r_v = left - right;}
			if (bottomleft > topright) {bl_tr = TRUE; bl_tr_v = bottomleft - topright;}
			if (topleft > bottomright) {tl_br = TRUE; tl_br_v = topleft - bottomright;}
			
			if (t_b_v >= l_r_v && t_b_v >= bl_tr_v && t_b_v >= tl_br_v)
			{
				direction[h*width+w] = 0;
				if (!t_b) direction[h*width+w]= 4;
			}
			
			if (l_r_v >= t_b_v && l_r_v >= bl_tr_v && l_r_v >= tl_br_v)
			{
				direction[h*width+w] = 6;
				if (!l_r) direction[h*width+w]= 2;
			}
			
			if (bl_tr_v >= l_r_v && bl_tr_v >= t_b_v && bl_tr_v >= tl_br_v)
			{
				direction[h*width+w] = 5;
				if (!bl_tr) direction[h*width+w]= 1;
			}
			
			if (tl_br_v >= l_r_v && tl_br_v >= bl_tr_v && tl_br_v >= t_b_v)
			{
				direction[h*width+w] = 7;
				if (!tl_br) direction[h*width+w]= 3;
			}
		}
	}
	
    // for each of the detected pixels by the window size
	for (h=2;h<(height/6)-1;h++)
	{
		for (w=2;w<(width/6)-1;w++)
		{
			directionIn[h*width+w]=0;
			directionOut[h*width+w]=0;
			if (IsSkin(info,width,h,w))
			{
				if (direction[((h-1)*width)+w-1]==1 && IsSkin(info,width,h-1,w-1)) directionIn[h*width+w]++;
				if (direction[((h-1)*width)+w  ]==0 && IsSkin(info,width,h-1,w  )) directionIn[h*width+w]++;
				if (direction[((h-1)*width)+w+1]==7 && IsSkin(info,width,h-1,w+1)) directionIn[h*width+w]++;
				if (direction[((h)*width)+w-1]==2 && IsSkin(info,width,h  ,w-1))   directionIn[h*width+w]++;
				if (direction[((h)*width)+w+1]==6 && IsSkin(info,width,h  ,w+1))   directionIn[h*width+w]++;
				if (direction[((h+1)*width)+w-1]==3 && IsSkin(info,width,h+1,w-1)) directionIn[h*width+w]++;
				if (direction[((h+1)*width)+w  ]==4 && IsSkin(info,width,h+1,w  )) directionIn[h*width+w]++;
				if (direction[((h+1)*width)+w+1]==5 && IsSkin(info,width,h+1,w+1)) directionIn[h*width+w]++;
				
				if (direction[((h-1)*width)+w-1]==5 && IsSkin(info,width,h-1,w-1)) directionOut[h*width+w]++;
				if (direction[((h-1)*width)+w  ]==4 && IsSkin(info,width,h-1,w  )) directionOut[h*width+w]++;
				if (direction[((h-1)*width)+w+1]==3 && IsSkin(info,width,h-1,w+1)) directionOut[h*width+w]++;
				if (direction[((h)*width)+w-1]==6 && IsSkin(info,width,h  ,w-1))   directionOut[h*width+w]++;
				if (direction[((h)*width)+w+1]==2 && IsSkin(info,width,h  ,w+1))   directionOut[h*width+w]++;
				if (direction[((h+1)*width)+w-1]==7 && IsSkin(info,width,h+1,w-1)) directionOut[h*width+w]++;
				if (direction[((h+1)*width)+w  ]==0 && IsSkin(info,width,h+1,w  )) directionOut[h*width+w]++;
				if (direction[((h+1)*width)+w+1]==1 && IsSkin(info,width,h+1,w+1)) directionOut[h*width+w]++;
			}
		}
	}
	delete [] bw2; 
}



/*---------------------------------------------------------------------------
 //-  Function Name : MixedAnalysisProcess
                 
 //-  Description   : Process a hbitmap image and obtain the confidence value of pornographic
 //-  Parameter     : hbmp - hbitmap image
 pResults - structure contains all the porn features
 enEngine_Type - engine type either of screenshield or ishield
 enScan_Type - scanning type either of normal or quick
 //-  Return Value  : 
 //-------------------------------------------------------------------------*/

bool ImageProcessing::MixedAnalysisProcess( Mat MAimg,								//emad -added
											 ImageProcessing::WQ_IP_RESULTS * pResults,
											 EN_FSIP_ENGINE_TYPE enEngine_Type,
											 EN_FSIP_SCAN_TYPE enScan_Type)
{
	// Initialize the return value flags 
	bool Safe_Non = FALSE;
	bool Safest_Porn = FALSE;
	bool Safest_Non = FALSE;
	bool Portrait_Non = FALSE;
	bool Portrait_Non1 = FALSE;
	bool Body_Non = FALSE;
	bool Human_Porn = FALSE;
	bool Smooth_Non = FALSE;
	
	// Initialize the structure of porn result
	if (pResults != NULL)
	{
		memset(pResults, 0, sizeof(WQ_IP_RESULTS)); 
	}
	
	// Initialize flag
	pResults->pf=-1;
	pResults->kof=-1;
	pResults->kosf=-1;
	pResults->RT=-1;
	pResults->FT=-1;
	pResults->cf=FALSE;
	

	int height, width , channels  , depth , imagesize, widthstep , bpp,type  ;
    int image_depth =0;
    uchar* data = (uchar*) MAimg.data;//   
	
	
	height = MAimg.rows;
	width = MAimg.cols ;
    channels = MAimg.channels();//  
    depth = MAimg.depth();
    imagesize =(int)MAimg.rows*MAimg.cols;
    type = MAimg.type();
    widthstep = MAimg.step[0];
	//retriving depth to calculate bits per pixel.
    if(depth == 0 || depth == 1){
        image_depth =8;
    }else if(depth == 2 ||depth == 3 ){
        image_depth =16;
    }
    else if(depth == 4 ||depth == 5 ){
        image_depth =32;
        
    }
    else if(depth == 6 ){
        image_depth =64;
    }
    depth = image_depth; //added emad 2019 march 15
   
    bpp = ( image_depth & 255) * MAimg.channels();
	
	CMBitmapProcessing::initvalues(width , height  , widthstep ,depth  , channels , bpp, type);
	
	
	//---------------------------------------------------------------------------
	//- Fast Filter(1)-Check the size of the image
	//---------------------------------------------------------------------------
//	if (bm.bmWidth < 50 && bm.bmHeight < 50 )
	if (width < 50 && height < 50 )
	{
		if (ShowDetailedImage == TRUE)
		{
		}
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=101;
		pResults->RT=0;
		pResults->FT=1;
#ifdef _JHDebug_showlog		
		printf("\n f1 if (width < 50 && height < 50 ) "); //JHP
		printf("\n NetPornValue=%.2f pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif		
		return TRUE;
	}
	//---------------------------------------------------------------------------
	//- End of Fast Filter(1) 
	//---------------------------------------------------------------------------
			
	uchar* hbmCrop = NULL; 			
	uchar* hbmpCropped = NULL; 	
	

	if (ChangeTo24Bit(hbmCrop,data , bpp))
	{
		//obtain a new bitmap after conversion

			
	int cropSize = QuickCropTest(hbmCrop , width , height , bpp);
		
	if (cropSize==0)
		CMBitmapProcessing::CloneBitmap(&hbmpCropped, hbmCrop);
	else

		CMBitmapProcessing::ResizeBitmap(&hbmpCropped, hbmCrop,0,0,width,height-cropSize,width,height-cropSize,bpp);	
		CMBitmapProcessing::initvalues (width,height-cropSize, widthstep, depth  , channels , bpp,type);	//JH added
		height=height-cropSize; //JHDV1
	}
	

	if (hbmCrop!=NULL) delete[] hbmCrop;
	hbmCrop=NULL;
	
	//---------------------------------------------------------------------------
	//- Fast Filter (2)-Perform quick skin scan 
	//---------------------------------------------------------------------------
	//THRESHOLD_QUICKSCAN=0.10; // SAFEST Setting
	THRESHOLD_QUICKSCAN=0.095; // SAFEST Setting - JH to correlate with Win GDI+ vs MAC difference
	if (ShowDetailedImage == TRUE)
	{
	//	_tcscat(ImageAnalysisMsg , _T("Starting New Quick Scan Filter For Safest Engine: "));
		
	}
	

	if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type , width , height , bpp)) 	
	{
		if (ShowDetailedImage == TRUE)
		{
		
			
		}
		pResults->NetPornValue = 0.0f;
		pResults->RT=0;
		pResults->FT=2;
	
		if (hbmpCropped!=NULL) delete[] hbmpCropped;
		hbmpCropped=NULL;
#ifdef _JHDebug_showlog		
		printf("\n f2 if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type , width , height , bpp)) "); //JHP
		printf("\n NetPornValue=%.2f pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif		
		return TRUE;
	}
	else 
	{
		THRESHOLD_QUICKSCAN=0.20; // SAFE Setting
	//	if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type)) 
		if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type , width , height , bpp)) 
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By Filter Two (New Quick Scan) - Safe Engine \n\n"));
			}
			pResults->NetPornValue = 0.0f;
			Safe_Non = TRUE;
		}
	}
	//---------------------------------------------------------------------------
	//- End of Fast Filter (2)
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Perform Scaling to a predetermine fix ratio size
	//---------------------------------------------------------------------------
//	HBITMAP hbm2 = NULL; //initiate a new hbitmap for scaling purpose
	uchar* hbm2 = NULL; //initiate a new hbitmap for scaling purpose
	bool scalesuccess=true;
//	scalesuccess=ProportionScaling(hbm2,hbmpCropped);
	INT nWidth , nHeight, nDepth, nChannel,nType;	//JH ealier declared prior this
	scalesuccess=ProportionScaling(hbm2, hbmpCropped,  width ,  height ,  depth , channels , bpp , nWidth , nHeight , nDepth , nChannel,type,nType);
	
	CMBitmapProcessing::initvalues (nWidth,nHeight, widthstep, nDepth  , nChannel , bpp, type);	//JHDV1 added

	if(scalesuccess==false) 
	{
	
		delete[] hbm2;
		hbm2 = NULL;
	
		if (hbmpCropped!=NULL) delete[] hbmpCropped;
		hbmpCropped=NULL;
		

		return false;
	}
	
	if (hbmpCropped!=NULL) delete[] hbmpCropped;
	hbmpCropped=NULL;
	//---------------------------------------------------------------------------
	//- End of Perform Scaling
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------

	if (nWidth < 20 || nHeight < 20 )
	{
		if (ShowDetailedImage == TRUE)
		{
		//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter Three: Size Of the Image after Scaling is lower than 20 || 20) - All Engines \n\n"));
		//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			char PreWinName[56];
			sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			cvDestroyWindow(PreWinName);
		//	_tcscpy(ImageAnalysisMsg , _T(""));
			
		}
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=121;
		pResults->RT=0;
		pResults->FT=3;
#ifdef _JHDebug_showlog
		printf("\n f4 (nWidth < 20 || nHeight < 20 )"); //JHP
		printf("\n NetPornValue=%.2f pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif
		return TRUE;
	}
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------
	
	
    cv::Mat imgskin ;

     imgskin = cv::Mat(nHeight,nWidth,CV_8UC3,hbm2).clone();
  
    BYTE * image = (BYTE * ) imgskin.data;
	BYTE * image2 = (BYTE * ) imgskin.data; 

	//---------------------------------------------------------------------------
	//- Fast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------
	bool filtersuccess=true;
	bool filterresult=false;
	if (ShowDetailedImage == TRUE)
	{
	//	_tcscat(ImageAnalysisMsg , _T("Starting Filter 4 to Check type of colours, homogeneity , number of colours: "));
	}

	filtersuccess=FastFilterProcess(hbm2,nWidth , nHeight , nDepth , nChannel,image2,enScan_Type,filterresult,pResults);

	if(filtersuccess==false) 
	{

		delete[] hbm2;
		hbm2 = NULL;
#ifdef _JHDebug_showlog
		printf("\n f4 FastFilterProcess (filtersuccess==false)"); //JHP
		printf("\n NetPornValue=%.2f pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif	
		return false;
	}
	else
	{
		if(filterresult)
		{
			pResults->kof=100;
			if(pResults->NetPornValue>0.5) 
			{
				if (ShowDetailedImage == TRUE)
				{
				//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By Filter Four - All Engines \n\n"));
				//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
					char PreWinName[56];
					sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
					cvDestroyWindow(PreWinName);
				//	_tcscpy(ImageAnalysisMsg , _T(""));
				}
				pResults->pf=1;
				pResults->RT = 2;
				pResults->FT =4;
			}
			else
			{
				if (ShowDetailedImage == TRUE)
				{
				
					
				}
				pResults->pf=0;
				pResults->RT = 0;
				pResults->FT =4;
			}

			delete[] hbm2; 
			hbm2 = NULL;
#ifdef _JHDebug_showlog
			printf("\n f4 FastFilterProcess if(filterresult)"); //JHP
			printf("\n NetPornValue=%.2f pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif	
			return true;
		}
	}
	
	if (ShowDetailedImage == TRUE)
	{
	//	_tcscat(ImageAnalysisMsg , _T("Passed Filter Four - Proceed to Next Filters \n\n"));
	}
	
	//---------------------------------------------------------------------------
	//- End ofFast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------
	
	
	//---------------------------------------------------------------------------
	//- Perform Detail Filter for image that pass thru Fast Filter
	//---------------------------------------------------------------------------
	if(image)
	{
		//---------------------------------------------------------------------------
		//- Initialization variables
		//---------------------------------------------------------------------------
		INT h,i,w;
		INT width=nWidth;
		INT height=nHeight;
		INT hw=width*height;
		INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
	
		
        unsigned char *red=new unsigned char[hw];
        unsigned char *green= new unsigned char[hw];
        unsigned char *blue= new unsigned char[hw];
        unsigned char *bw=  new unsigned char[hw];
		unsigned char *object_map = NULL;
		struct simonBlob *blob_info = NULL;
		INT *info=new INT[hw];
		INT *blob=new INT[hw];
		bool portraitflag=FALSE;
		bool dominantcolour = FALSE ;
		bool Noisyface = FALSE ;
		bool NoisyFaceColour = FALSE;
		bool portraitindicator=FALSE;
		INT profileindicator = 0;
		bool manyfacesindicator=FALSE;
		bool shoulderportraitindicator=FALSE;
		INT object_num=0;
		INT *blob_ptr;
		I
		INT numberBlobs;
		INT boxBottom,boxTop,boxLeft,boxRight,totalBlobSkin;
	
		FLOAT threshold=0.0;
		INT numberFaces=0;
		INT innerEdges=0;
		INT outerEdges=0;
		INT innerEdgesSum=0;
		INT outerEdgesSum=0;
		INT numberWeakInnerEdges_0=0;
		INT numberWeakInnerEdges_1=0;
		INT numberWeakInnerEdges_2=0;
		INT numberWeakInnerEdges_5=0;
		INT numberWeakOuterEdges_1=0;
		INT numberWeakOuterEdges_2=0;
		INT totalStrongEdges2=0;
		FLOAT diamondSkin;
		INT skinInDiamond;
		INT faceSkin=0;
		//		INT totalSkinArea;
		INT totalBlobArea;
		//		INT number_limb_objects;
		
		INT blob_horizontal80;
		INT blob_horizontal90;
		
		//     Added For New Engine 
		FLOAT BlobAreaRatio = 0.0;
		FLOAT BlobSkinRation = 0.0;
		
		//variables for image display
		INT index=0;
	//	INT val=0;
		
		//variables for curvature computation
		INT *direction=new INT[hw];
		INT *directionIn=new INT[hw];
		INT *directionOut=new INT[hw];
		
		//variables for edge computation
		INT *edge_total = new INT[hw];
		INT highThreshold=75;
		INT lowThreshold=45;
		INT lengthThres=10;	
		INT total_points=0;
		
		FLOAT f_numberBlobs,f_numberFaces,f_threshold,f_diamondSkin;
		FLOAT f_boxTop,f_boxBottom,f_boxHeight,edges1,edges2,edges3;
		FLOAT skin1,skin2,skin3,skin4,edgeScore,moment,curves , fBodyskin , fFaceSkin;
		FLOAT c1 , c2 , c3 , c4 , c5 , c6 , c7 , c8 ,c9 , c10;
		//---------------------------------------------------------------------------
		//- End of Initialization variables
		//---------------------------------------------------------------------------
		
		// SVM Classification Variables 
		
        cv::Mat m_porn;
		
		float porn_ret = -1.0;
		float sporn_ret = -1.0;
      
        Ptr<cv::ml::SVM> pornsvm = ml::SVM::create(); //updating to opencv4
        Ptr<cv::ml::SVM> spornsvm = ml::SVM::create();
		fBodyskin = 0;
		fFaceSkin = 0;
		
		CvMat m_nonporn;
	
        Ptr<cv::ml::SVM> nonsvm = ml::SVM::create();
        
		//---------------------------------------------------------------------------
		//- Initialization variables for neural network load
		//---------------------------------------------------------------------------
		FLOAT fPornFeature[55];

		//---------------------------------------------------------------------------
		//- End of Initialization variables for neural network load
		//---------------------------------------------------------------------------
		

		memset(info, 0 ,  sizeof(INT) * hw);
		memset(blob, 0 , sizeof(INT) * hw);
		memset(bw, 0, sizeof(char) * hw);
		memset(direction, 0 , sizeof(char) * hw);
		memset(directionIn, 0, sizeof(char) * hw);
		memset(directionOut,  0, sizeof(char) * hw);
		memset(edge_total, 0 , sizeof(INT) * hw);
		
		//---------------------------------------------------------------------------
		//- Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		rgbComputeOptimised(image,red,green,blue,height,width,widthbytes);
		//---------------------------------------------------------------------------
		//- End of Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Gamma correction
		//---------------------------------------------------------------------------
		GammaCorrectionOptimised(red,green,blue,bw,height,width);		
		//---------------------------------------------------------------------------
		//- End of Gamma correction
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		INT skinpixels=0;
	//	INT ratioskin=0;
		skinpixels=ObtainSkinInfoOptimised(info,bw,red,green,blue,width,height,widthbytes);
#ifdef _JHDebug_Image2 	//JHDV		
		//JHDF2
		FILE *fi;
		fi=fopen("/MAC_info_skin.txt", "w");
		int *pinfo;
		pinfo= info;
		//	int fputc( int c, FILE *fi );
		for (long i=0; i<height*width ;i++)
		{	if( i%width==0)
		{fprintf(fi, "\n");	}
			if (*(pinfo+i)) {
				fprintf(fi, "%d", *(pinfo+i));
			}
			else{
				fprintf(fi, "-", *(pinfo+i)); 		
			}
		} 
		//JHDF2-end 
#endif		
		
		//---------------------------------------------------------------------------
		//- End of Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006 || enEngine_Type==FSIP_LAX_ENGINE_APR_2006)
        {
			//---------------------------------------------------------------------------
			//- Determine the edges of the image
			//---------------------------------------------------------------------------
			CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
			//---------------------------------------------------------------------------
			//- End of Determine the edges of the image
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			CalcMoments(edge_total, width, height, pResults->daMoments);
			//---------------------------------------------------------------------------
			//- End of Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Calculate the edge histogram 
			//---------------------------------------------------------------------------
			CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
			//---------------------------------------------------------------------------
			//- End of Calculate the edge histogram 
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			float percentage_skin = ((float) skinpixels)/((float)hw);
			fPornFeature[0]=percentage_skin;
			
			moment = ((float)pResults->daMoments[0]);
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[1]=moment;
			moment = ((float)pResults->daMoments[1])/0.67;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[2]=moment;
			moment = ((float)pResults->daMoments[2])/0.7;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[3]=moment;
			moment = ((float)pResults->daMoments[3])/0.33;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[4]=moment;
			moment = ((float)pResults->daMoments[4])/0.81;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[5]=moment;
			moment = ((float)pResults->daMoments[5])/0.32;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[6]=moment;
			moment = ((float)pResults->daMoments[6])/0.82;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[7]=moment;
			for(i=0;i<32;i++)
			{
				fPornFeature[i+8]=pResults->dEdgeMap[i];
			}
			//---------------------------------------------------------------------------
			//- End of Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Determine the porn value from the fast porn classifier
			//---------------------------------------------------------------------------
		//----------------------------------------------------------------
			// ----------------------- Train SVM -----------------------------
	
			
         
            Ptr<cv::ml::ANN_MLP> NN = cv::ml::ANN_MLP::create();
			
			// For Unicode Compiling 
			char safeNonPornNNFilename[MAX_PATH] = {0};
			
			for (int i=0 ; i < MAX_PATH ; i++)
			{
				safeNonPornNNFilename[i] = NULL; 
			}
			
		
			for (int i=0 ; i < strlen(m_szsafeNonPornNNFilename) ; i++)
			{
				safeNonPornNNFilename[i] = m_szsafeNonPornNNFilename[i]; 
			}
			
		
            NN = Algorithm::load<ml::ANN_MLP>(safeNonPornNNFilename);
            
			
             m_porn = cv::Mat(1, 40, CV_32FC1, fPornFeature);
            
	
            cv::Mat classificationResult;
            classificationResult.create(1, 2, CV_32FC1);
            NN->predict (m_porn , classificationResult);
            
			int pred = 0;
			float max_value = classificationResult.data[0];

			for (int k=1;k<2;k++)
			{
                if (max_value<classificationResult.data[k]); 
				{
					pred=k;
					max_value=classificationResult.data[k];
				}
			}
			
			
			if (pred == 0)
			{
				if (ShowDetailedImage == TRUE)
				{
				//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 5: Neural Network) - Safe Engine \n\n"));
				}
				Safe_Non = TRUE;
			}
		
			//---------------------------------------------------------------------------
			//- End of Determine the porn value from the fast porn classifier
			//---------------------------------------------------------------------------
		}
		
		//---------------------------------------------------------------------------
		//- Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------		
		if (skinpixels==0 ) 
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 5: no skin pixels after removing dodgy texture ) - All Engines \n\n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
				
			}
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=100;
			pResults->kosf=106;
			pResults->RT=0;
			pResults->FT=5;
		//	printf(" \nf5 (skinpixels==0) "); //JHP
		//	printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);				
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Segment the image base on skin properties
		//---------------------------------------------------------------------------
		object_map = new unsigned char[hw];
        memset(object_map, 0, sizeof(char) * hw);
/*        for(i=0;i<hw;i++){
            *object_map++=0;
        }*/
		object_num=RegionSegmentationOptimised(object_map,red,green,blue,height,width);
		//---------------------------------------------------------------------------
		//- End of Segment the image base on skin properties
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		if (object_num<0 ) 
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 6: No Colour cluster after segmentation) - All Engines \n\n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
				char PreWinName[56];
				sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
				cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}
			//MessageBox ( NULL , _T("Filter 6") , _T("***********") , NULL);
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=207;
			pResults->RT=0;
			pResults->FT=6;
#ifdef _JHDebug_showlog
			printf("\n f6 (object_num<0) "); //JHP
			printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		numberBlobs = GetSimonBlobInfoOptimised(object_map, object_num, height, width, bw, info, blob,threshold);
		//---------------------------------------------------------------------------
		//- End of Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- End of Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		if (numberBlobs==0) 
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 7: no blob after blob analysis) - All Engines \n\n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
				char PreWinName[56];
				sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
				cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}
			//MessageBox ( NULL , _T("Filter 7") , _T("***********") , NULL);
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=208;
			pResults->RT=0;
			pResults->FT=7;
#ifdef _JHDebug_showlog
			printf("\n f7 (numberBlobs==0) "); //JHP
			printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif
			goto EndRemove;
		}
		
		bpp = (nDepth & 255) * nChannel ;
			
		CMBitmapProcessing::initvalues(width , height  , widthstep ,depth  , channels , bpp,type);//JH -addded to resolve non GDI+ bmp issue
		
		if (ShowDetailedImage == TRUE)
			NewBlobScan(hbm2,blob, nWidth , nHeight , nDepth , nChannel, widthstep);
		//	NewBlobScan(hbm2,blob);
		
		
		
		if (ShowDetailedImage == TRUE)
		{
		//	_stprintf(emad, _TEXT("\n Passed Filter 7 - Number of Detected Blobs = %d ---> Proceed To Next Filters \n\n") , numberBlobs);
		//	_tcscat(ImageAnalysisMsg , emad);
		//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		blob_info = new struct simonBlob[numberBlobs+1];
		for (i = 0; i < numberBlobs+1; i++)
		{
			blob_info[i].area=0;
			blob_info[i].skinTotal=0;
			blob_info[i].inface=FALSE;
		}
		
		boxBottom=height; //largest coordinate y for MER of skin
		boxTop=0; //smallest coordinate y for MER of skin
		boxLeft=width; //smallest coordinate x for MER of skin
		boxRight=0; //largest coordinate x for MER of skin
		blob_ptr=blob; //segmented result
		info_ptr=info; //original skin info before segmentation
		blob_horizontal80=0;
		blob_horizontal90=0;
		totalStrongEdges2=0;
		
		for (h=0;h<height;h++) 
		{
			INT blob_horizontal=0;
			for (w=0;w<width;w++) 
			{
				INT pos=h*width+w;
				if (w>0 && h>0) //horizontal edges
				{ 
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-1));
					if (edge>100) totalStrongEdges2++;
				}
				if (h>0) //vertical edges
				{
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-width));
					if (edge>80) totalStrongEdges2++;
				}
				
				if (*blob_ptr>0)
				{
					blob_horizontal++;
					blob_info[*blob_ptr].area++;
					if (*info_ptr>0) blob_info[*blob_ptr].skinTotal++;
					if (boxBottom>h) boxBottom=h;
					if (boxTop<h) boxTop=h;
					if (boxLeft>w) boxLeft=w;
					if (boxRight<w) boxRight=w;
				
					unsigned char *bw_ptr = bw+pos;
					if (w>0)
					{
						if (*(blob_ptr-1)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
					if (h>0)
					{
						if (*(blob_ptr-width)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-width));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge =abs (*bw_ptr - *(bw_ptr-width));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
				}
				blob_ptr++;
				info_ptr++;
			}
			//determine the total number of horizontal line that is over 80% and 90% of image width
			if (blob_horizontal*100>80*width) blob_horizontal80++;
			if (blob_horizontal*100>90*width) blob_horizontal90++;
		}
		
		if (innerEdges != 0 && outerEdges != 0 )
			
		{
			c1 = (100*outerEdges)/innerEdges; // >25
			c2 = (100*outerEdgesSum)/outerEdges; //>7000
			c3 = (100*outerEdgesSum)/outerEdges ; //<300
			c4 = (10000*totalStrongEdges2)/hw ; //< 10
			c5 = (1000*numberWeakInnerEdges_2)/innerEdges ; //>800 
			c6 = (1000*numberWeakInnerEdges_5)/innerEdges; //>600
			c7 = (1000*numberWeakInnerEdges_0)/innerEdges; //; <600 
			c8 = (1000*numberWeakInnerEdges_1)/innerEdges ;//<300
			c9 = (1000*numberWeakOuterEdges_1)/outerEdges ; // ;<600 
			c10 = (1000*numberWeakOuterEdges_2)/outerEdges ; //<400
			
		}
		else 
		{
			c1 = 0; // >25
			c2 = 0; //>7000
			c3 = 0 ; //<300
			c4 = 0 ; //< 10
			c5 = 0 ; //>800 
			c6 = 0; //>600
			c7 = 0; //; <600 
			c8 = 0 ;//<300
			c9 = 0 ; // ;<600 
			c10 =0 ; //<400
			
		}
		
		// Determine the total blob area and skin area in the image
		totalBlobSkin=0;
		totalBlobArea=0;
		
		// Added for new emgine 
		BlobAreaRatio = 0.0;
		BlobSkinRation = 0.0;
		
		for (i = 1; i <= numberBlobs; i++)
		{
			totalBlobSkin+=blob_info[i].skinTotal;
			totalBlobArea+=blob_info[i].area;
		}
		//---------------------------------------------------------------------------
		//- End of Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
		// Added for new Engine 
		// Calculate the percentage of Skin over the Blob Area 
		BlobSkinRation = ((100*totalBlobSkin)/((boxTop-boxBottom)*(boxRight-boxLeft)));
		// Calculate the percentage of Blob over the whole Image 
		BlobAreaRatio = (100*(boxTop-boxBottom)*(boxRight-boxLeft))/hw;
		//---------------------------------------------------------------------------
		//- End of Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
		
		// Obtain the ratio position of skin MER as compared to image dimension
		boxBottom = (100*boxBottom)/height;
		boxTop = (100*boxTop)/height;
		boxLeft = (100*boxLeft)/width;
		boxRight = (100*boxRight)/width;
		//---------------------------------------------------------------------------
		//- Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------


		portraitflag=ObtainFaceInfoOptimised_V3_6(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, nChannel  , nDepth);

		FILE *fi;
		fi=fopen("/MAC_info.txt", "w");
		int *pinfo;
		pinfo= info;
		//	int fputc( int c, FILE *fi );
		for (long i=0; i<h*w ;i++)
		{	if( i%w==0)
		{fprintf(fi, "\n");	}
			if (*(pinfo+i)) {
			fprintf(fi, "%d", *(pinfo+i));
			}
			else{
			fprintf(fi, "-", *(pinfo+i)); 		
			}
		}
		fclose(fi);
		//JHDF2-end */
		//---------------------------------------------------------------------------
		//- End of Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------
		// Determine number of faces
		numberFaces = FaceAttr.size();
		
		//---------------------------------------------------------------------------
		//- Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
		//---------------------------------------------------------------------------
		if(portraitflag==TRUE )
		{
		
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 11 - Portrait Face has Been Detected) - safest Engine \n\n"));
			}
			Portrait_Non1 = TRUE;
			//pResults->FT=11;
		}
		
		//--------------------------------------------------------------------------------
		// Checking the possibility to detect the face if we removed the added White Border to the image 
		//------------------------------------------------------------------------------
		else
		{
	
			FaceAttr.clear();
			portraitflag=ObtainFaceInfoOptimised1_V3_6(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, profileindicator, nChannel , nDepth, bpp);
			if(portraitflag==TRUE )
			{
				
				if (ShowDetailedImage == TRUE)
				{
				//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 11 - Portrait Face has Been Detected) - safest Engine \n\n"));
				}
				Portrait_Non = TRUE;
				//pResults->FT=111;
				
			}
			
			
			numberFaces = FaceAttr.size();
			
			if (ShowDetailedImage == TRUE)
			{
			//	_stprintf(emad, _TEXT("Number of Detected Faces = %d ---> Proceed To Next Filters \n\n") , numberFaces);
			//	_tcscat(ImageAnalysisMsg , emad);
			}
			
			
			//---------------------------------------------------------------------------
			//- End of Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Analyse the human strucutre below the face to determine the nude area and topology
			//---------------------------------------------------------------------------
			if(numberFaces>0)
			{
				hbm2= (BYTE * ) hbm2; 
		
				bool DoFiltering = FilterNoisyFaces(red , green , blue , height , width , hbm2, nChannel , nDepth);			
				if (ShowDetailedImage == TRUE)
				{
			//		_stprintf(homam, _TEXT("Number of Detected Faces after Noisy Face Removal = %d ---> Proceed To Next Filters \n\n") , FaceAttr.size());
			//		_tcscat(ImageAnalysisMsg , homam);
				}
				
				if (FaceAttr.size()> 3)
				{
					if (ShowDetailedImage == TRUE)
					{
			
					}
					pResults->pf=0;
					pResults->NetPornValue=0.0f;
					pResults->kof=200;
					pResults->kosf=253;
					pResults->RT = 0;
					pResults->FT =112;
					// end
#ifdef _JHDebug_showlog
				//	printf("\n f11-12 (FaceAttr.size()> 3) "); //JHP
				//	printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
					goto EndRemove;
				}
				
				if (FaceAttr.size()> 0)
				{
					// Super Lax Detection 
					//-----------------------------------------------------------------
					if (profileindicator == 1)
					{
						bool NoisyFaceColour = TRUE;
						NoisyFaceColour = NoisyfacesColour(red , green , blue , height , width);
						if (NoisyFaceColour == FALSE)
						{
							bool Noisyface = TRUE;
							Noisyface = Noisyfaces(height , width);
							
							if (Noisyface == FALSE)
							{
								bool dominantcolourSL = FALSE;
								dominantcolourSL = AnalyseDominantColour(red , green , blue , height , width);
								if ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) && (Safe_Non == FALSE))
								{
									
									if (ShowDetailedImage == TRUE)
									{
									//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 11 - Matched Skins Between Face and Body Are Enough) - Super Lax \n"));
									//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
									//	char PreWinName[56];
									//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
									//	cvDestroyWindow(PreWinName);
									//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
									//	cvDestroyWindow(PreWinName);
									//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
									//	cvDestroyWindow(PreWinName);
									//	_tcscpy(ImageAnalysisMsg , _T(""));
										
									}
									pResults->NetPornValue=1.0f;
									pResults->pf=1;
									pResults->kof=200;
									pResults->kosf=254;
									pResults->RT = 5;
									pResults->FT =115;
#ifdef _JHDebug_showlog
									printf("\n f11-12 ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) && (Safe_Non == FALSE))");	//JHP
									printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
									goto EndRemove;
								}
								
								if ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400))
								{
									if (ShowDetailedImage == TRUE)
									{
									
									}
									pResults->NetPornValue=1.0f;
									pResults->pf=1;
									pResults->kof=200;
									pResults->kosf=254;
									pResults->RT = 2;
									pResults->FT =115;
#ifdef _JHDebug_showlog
									printf("\n f11-12 ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400)) "); //JHP
									printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
									goto EndRemove;
								}
							}
						}
					}
					// End Of Super Lax Detection Part 
					//-----------------------------------------------------------------
					bool NonPorn = FALSE;
				//	dominantcolour = AnalyseDominantColourDump(red , green , blue , height , width , hbm2 , NonPorn ,  fBodyskin , fFaceSkin);		
					dominantcolour = AnalyseDominantColourDump(red , green , blue , height , width , NonPorn ,  fBodyskin , fFaceSkin);	//JH- hbm2 redundent, not use, so remove
					if (NonPorn == TRUE )
					{
					
						if (ShowDetailedImage == TRUE)
						{
						//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 11 - Matched Skins Between Face and Body Are not Enough) - Safest Engine \n\n"));
						}
						Body_Non = TRUE;
						pResults->FT=114;
					}
					if (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400 && (Safe_Non == FALSE))
					{
						if (ShowDetailedImage == TRUE)
						{
						
							
						}
						pResults->NetPornValue=1.0f;
						pResults->pf=1;
						pResults->kof=200;
						pResults->kosf=254;
						pResults->RT = 4;
						pResults->FT =115;
#ifdef _JHDebug_showlog
						printf("\n f11-12 (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400 && (Safe_Non == FALSE)) "); //JHP	
						printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
						goto EndRemove;
						
					}
					
					if (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400)
					{
						if (ShowDetailedImage == TRUE)
						{
					
							
						}
						pResults->NetPornValue=1.0f;
						pResults->pf=1;
						pResults->kof=200;
						pResults->kosf=254;
						pResults->RT = 2;
						pResults->FT =115;
#ifdef _JHDebug_showlog
						printf("\n f11-12 (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) "); //JHP	
						printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
						goto EndRemove;
						
					}
					
					
					pResults->pf = -1;
					
					AnalyseHumanStructureOptimised(blob_info,blob,numberBlobs,height,width,pResults, info, faceSkin);
					if (pResults->pf== 0)
					{
						if (ShowDetailedImage == TRUE)
						{
						//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 11 - AnalyseHumanStructureOptimised) - All Engines \n\n"));
						//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
						//	char PreWinName[56];
						//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
						//	cvDestroyWindow(PreWinName);
						//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
						//	cvDestroyWindow(PreWinName);
						//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
						//	cvDestroyWindow(PreWinName);
						//	_tcscpy(ImageAnalysisMsg , _T(""));
							
						}
						
						pResults->kof=400;
						pResults->NetPornValue=0.0f;
						pResults->pf=0;
						pResults->kosf=254;
						pResults->RT = 0;
						pResults->FT = 116;
#ifdef _JHDebug_showlog
						printf("\n f11-12 (pResults->pf== 0) "); //JHP	
						printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif
						goto EndRemove;
					}
					
					if (pResults->pf== 1)
					{
						if (ShowDetailedImage == TRUE)
						{
						//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 11 - AnalyseHumanStructureOptimised) - Super Safest Engine \n\n"));
						}
						Human_Porn = TRUE;
						pResults->FT=116;
					}
					
				}
				
			}
		}
		
		//---------------------------------------------------------------------------
		//- Determine the skin Ratio over the detected blob
		//---------------------------------------------------------------------------
		if (BlobSkinRation < 3 )
		{
			if (ShowDetailedImage == TRUE)
			{
		
				
			}
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=209;
			pResults->RT=0;
			pResults->FT=9;
#ifdef _JHDebug_showlog
			printf("\n f9 (BlobSkinRation < 3 ) "); //JHP	
			printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
			goto EndRemove;
		}
		
		
		if (innerEdges == 0 || outerEdges == 0 || totalBlobArea == 0) // To Avoid Crashing Later if The image has problem While processing 
		{
			
			pResults->NetPornValue=0;
			pResults->pf=0.0f;
			pResults->kof=200;
			pResults->kosf=258;
			pResults->RT = -1;
			pResults->FT =-1;
#ifdef _JHDebug_showlog
			printf("\n f9/-1 (innerEdges == 0 || outerEdges == 0 || totalBlobArea == 0) "); //JHP	
			printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
			goto EndRemove;
			
		}
		
		
		
		//---------------------------------------------------------------------------
		//- End of Analyse the human strucutre below the face to determine the nude area and topology
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		DiamondSkinOptimal(blob,width,height,diamondSkin,skinInDiamond);
		//---------------------------------------------------------------------------
		//- End of Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		if (diamondSkin>0.90  ) 
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 12 - Amount of skin in the center diamond is Enough) - Safest Engine \n"));
			}
			Safest_Porn = TRUE;
			pResults->FT=12;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		if ((boxTop - boxBottom<40 ) && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 13 - Hieght of MER skin is less than threshold value) - All Engines  \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=13;
			
		}
		
		if ((boxTop - boxBottom<40 ) )
		{
			Safest_Non = TRUE;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------
		//if(myfile.is_open()) myfile << "Filter 14";
		//			MessageBox ( NULL , "Apply Filter 14" , "Filter" , NULL);
		if ((((100*outerEdges)/innerEdges)>25 )  && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 14 - Inner skin blob is smoother as compared to boundary of skin blob) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=14;
			
		}
		
		if ((((100*outerEdges)/innerEdges)>25 ) )
		{
			Safest_Non = TRUE;
		}
		
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------
		if (((100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 ) && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE)) // was 150
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 15 - images seems to be not that smooth or too smooth) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=15;
			
		}
		
		if (((100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 )) // was 150
		{
			Safest_Non = TRUE;	
		}
		
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if (((100*totalBlobSkin)/totalBlobArea)>97  && (Safest_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE && Safest_Porn == FALSE )
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 17 - ratio of skin area to the blob area is too high ) - Safest Engine \n"));
			}
			Safest_Porn = TRUE;
			pResults->FT=17;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/hw > 40 && (Safest_Non == FALSE) )
		{
			if (ShowDetailedImage == TRUE && Safest_Porn == FALSE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 18 - ratio of skin in the blob as compared to the image area is too high ) - Safest Engine \n"));
			}
			Safest_Porn = TRUE;
			pResults->FT=18;
		}
		
		
		
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/totalBlobArea > 90 && (Safest_Non == FALSE) )
		{
			if (ShowDetailedImage == TRUE && Safest_Porn == FALSE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 19 - ratio of skin area to the blob is too high ) - Safest Engine \n"));
			}
			Safest_Porn = TRUE;
			pResults->FT=19;
		}
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		
		//- Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
		if (enEngine_Type != FSIP_MIXEDVID_ENGINE_APR_2006) 
		{
			if (((10000*totalStrongEdges2)/hw < 10)  && (Safest_Porn == FALSE) && (Safest_Non == FALSE)&& (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE))
			{
				if (ShowDetailedImage == TRUE)
				{
				//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 20 - very smooth image) - All Engines \n\n"));
				}
				Smooth_Non = TRUE;
				pResults->FT=20;
				
			}
			
			if (((10000*totalStrongEdges2)/hw < 10) )
			{
				Smooth_Non = TRUE;
			}
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		if ((((100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80))  && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE) && (Smooth_Non == FALSE) )
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 21 - Shape of the blob isn't similar to human shape) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=21;
			
		}
		
		if ((((100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80)) )
		{
			Safest_Non = TRUE;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		if (((((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600))  && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE) && (Smooth_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 22 - Too many weak internal edges) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=22;
			
		}
		
		if (((((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600)))
		{
			Safest_Non = TRUE;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		if (((((1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300)) && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE) && (Smooth_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 23 - Too few weak internal edges) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=23;
			
		}
		
		
		if (((((1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300)))
		{
			Safest_Non = TRUE;	
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		if (((((1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400)) && (Safest_Porn == FALSE) && (Safest_Non == FALSE) && (Body_Non == FALSE) && (Portrait_Non == FALSE) && (Portrait_Non1 == FALSE) && (Smooth_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 24 - Too few weak external edges) - All Engines \n\n"));
			}
			Safest_Non = TRUE;
			pResults->FT=24;
			
		}
		
		if (((((1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400)))
		{
			Safest_Non = TRUE;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		
		// Should be portrait due to the porn Filters 
		if ((Portrait_Non1 == TRUE)  && (Safest_Porn == TRUE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By Super Safest \n\n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}		
			//MessageBox ( NULL , "Apply Filter 25_1" , "Filter" , NULL);
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=0;
#ifdef _JHDebug_showlog			
			printf("\n f24 ((Portrait_Non1 == TRUE)  && (Safest_Porn == TRUE)) "); //JHP	
			printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif
            goto EndRemove;
		}
		
		else if (Portrait_Non1 == TRUE)
		{
			Safest_Non = TRUE;
		}
		
		//------------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the number of brightness points
		//---------------------------------------------------------------------------
		CurvaturePoint (bw,blob, height, width, direction, directionIn, directionOut);
		for (h=0;h<height;h++)
			for (w=0;w<width;w++)
			{
				index = w+w+w;
				INT val = ((h/6)*width) + w/6;
				if (directionIn[val]>4) if (h%6==0 && w%6==0) total_points++;
				if (directionOut[val]>4) if (h%6==0 && w%6==0) total_points++;
			}
		pResults->numberOfCurvature = total_points;
		//---------------------------------------------------------------------------
		//- End of Determine the number of brightness points
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the edges of the image
		//---------------------------------------------------------------------------
		CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
		//---------------------------------------------------------------------------
		//- End of Determine the edges of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		CalcMoments(edge_total, width, height, pResults->daMoments);
		//---------------------------------------------------------------------------
		//- End of Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate the edge histogram 
		//---------------------------------------------------------------------------
		CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
		//---------------------------------------------------------------------------
		//- End of Calculate the edge histogram 
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Compute 55 feature for porn classifier
		//---------------------------------------------------------------------------
		//Feature (1)
		f_numberBlobs = ((float)(numberBlobs-1))/5.0; // was / 14.0
		if (f_numberBlobs<0.0) f_numberBlobs=0.0;
		if (f_numberBlobs>1.0) f_numberBlobs=1.0;
		fPornFeature[0]=f_numberBlobs;
		//Feature (2) Body Skincolor Matching With Faces 
		f_numberFaces = ((float)fBodyskin);
		if (f_numberFaces<0.0) f_numberFaces=0.0;
		if (f_numberFaces>1.0) f_numberFaces=1.0;
		fPornFeature[1]=f_numberFaces;
		//Feature (3)
		f_threshold = (threshold-0.1)/0.7;
		if (f_threshold<0.0) f_threshold=0.0;
		if (f_threshold>1.0) f_threshold=1.0;
		fPornFeature[2]=f_threshold;
		//Feature (4)
		f_diamondSkin = (diamondSkin)/0.9;
		if (f_diamondSkin<0.0) f_diamondSkin=0.0;
		if (f_diamondSkin>1.0) f_diamondSkin=1.0;
		fPornFeature[3]=f_diamondSkin;
		//Feature (5)
		f_boxTop = ((float) (boxTop-60))/40.0;
		if (f_boxTop<0.0) f_boxTop=0.0;
		if (f_boxTop>1.0) f_boxTop=1.0;
		fPornFeature[4]=f_boxTop;
		//Feature (6)
		f_boxBottom = ((float) (boxBottom))/45.0;
		if (f_boxBottom<0.0) f_boxBottom=0.0;
		if (f_boxBottom>1.0) f_boxBottom=1.0;
		fPornFeature[5]=f_boxBottom;
		//Feature (7)
		f_boxHeight = ((float) (boxTop-boxBottom-40))/60.0;
		if (f_boxHeight<0.0) f_boxHeight=0.0;
		if (f_boxHeight>1.0) f_boxHeight=1.0;
		fPornFeature[6]=f_boxHeight;
		//Feature (8)
		edges1 = ((float)(((100*outerEdges)/innerEdges)-1))/21.0;
		if (edges1<0.0) edges1=0.0;
		if (edges1>1.0) edges1=1.0;
		fPornFeature[7]=edges1;
		//Feature (9)
		edges2 = ((float)(((100*outerEdgesSum)/outerEdges)-304))/6500.0;
		if (edges2<0.0) edges2=0.0;
		if (edges2>1.0) edges2=1.0;
		fPornFeature[8]=edges2;
		//Feature (10)
		edges3 = ((float)(((100*innerEdgesSum)/innerEdges)-128))/1200.0;
		if (edges3<0.0) edges3=0.0;
		if (edges3>1.0) edges3=1.0;
		fPornFeature[9]=edges3;
		//Feature (11)
		skin1 = ((float)(((100*totalBlobSkin)/hw)-5))/35.0;
		if (skin1<0.0) skin1=0.0;
		if (skin1>1.0) skin1=1.0;
		fPornFeature[10]=skin1;
		//Feature (12)
		skin2 = ((float)(((100*totalBlobSkin)/totalBlobArea)-21))/69.0;
		if (skin2<0.0) skin2=0.0;
		if (skin2>1.0) skin2=1.0;
		fPornFeature[11]=skin2;
		//Feature (13)
		skin3 = (float)(fFaceSkin)/20.0;
		if (skin3<0.0) skin3=0.0;
		if (skin3>1.0) skin3=1.0;
		fPornFeature[12]=skin3;
		//Feature (14)
		skin4 = ((float)(((100*skinInDiamond)/totalBlobArea)))/71.0;
		if (skin4<0.0) skin4=0.0;
		if (skin4>1.0) skin4=1.0;
		fPornFeature[13]=skin4;
		//Feature (15)
		edgeScore = ((float)(((10000*totalStrongEdges2)/hw)))/2500.0;
		if (edgeScore<0.0) edgeScore=0.0;
		if (edgeScore>1.0) edgeScore=1.0;
		fPornFeature[14]=edgeScore;
		//Feature (16 - 22) - 7 Hu Moments
		moment = ((float)pResults->daMoments[0]);
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[15]=moment;
		moment = ((float)pResults->daMoments[1])/0.67;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[16]=moment;
		moment = ((float)pResults->daMoments[2])/0.7;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[17]=moment;
		moment = ((float)pResults->daMoments[3])/0.33;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[18]=moment;
		moment = ((float)pResults->daMoments[4])/0.81;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[19]=moment;
		moment = ((float)pResults->daMoments[5])/0.32;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[20]=moment;
		moment = ((float)pResults->daMoments[6])/0.82;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[21]=moment;
		//Feature (23) - curvature points
		curves = ((float)pResults->numberOfCurvature)/5.0;
		if (curves<0.0) curves=0.0;
		if (curves>1.0) curves=1.0;
		fPornFeature[22]=curves;
		//Feature (24-55) - edge histogram
		for(i=23;i<55;i++)
		{
			fPornFeature[i]=pResults->dEdgeMap[i-23];
		}
		
	
		char safestPornSVMFilename[MAX_PATH];
		for (int i=0 ; i < MAX_PATH ; i++)
		{
			safestPornSVMFilename[i] = NULL; 
		}
		
		for (int i=0 ; i < strlen(m_szsafestPornSVMFilename) ; i++)		
		{
			safestPornSVMFilename[i] = m_szsafestPornSVMFilename[i];		
		}
		
	
        pornsvm = Algorithm::load<ml::SVM>(safestPornSVMFilename);
		////////////////////////////////////////////
	
         m_porn  = cv::Mat(1, 55, CV_32FC1, fPornFeature);
  
        porn_ret = pornsvm->predict(m_porn); // update for opencv4
		
		
		// Super Safest Engine
		// Discussing the porn Filters Before the (Portrait and Body) Non Porn Filters
		//-----------------------------------------------------------------------------
		if (((Body_Non == TRUE) || (Portrait_Non == TRUE))  && ((Safest_Porn == TRUE) || (porn_ret ==1)))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By Super Safest \n\n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}		
			//MessageBox ( NULL , "Apply Filter 25_1" , "Filter" , NULL);
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=1;
		//	pResults->FT=25;	//JH -added - should be Filter 14
#ifdef _JHDebug_showlog			
			printf("\n f11-12 (((Body_Non == TRUE) || (Portrait_Non == TRUE))  && ((Safest_Porn == TRUE) || (porn_ret ==1))) Win=1011"); //JHP	
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif
			goto EndRemove;
		}
		
		else if ((Body_Non == TRUE) || (Portrait_Non == TRUE))
		{
			Safest_Non = TRUE;
		}
		
		
		if ( (Smooth_Non == TRUE)  && ((Safest_Porn == FALSE) && (porn_ret ==1)))
		{
			if (ShowDetailedImage == TRUE)
			{
			
				
			}		
			//MessageBox ( NULL , "Apply Filter 25_1" , "Filter" , NULL);
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=1;
		//	pResults->FT=25;	//JH -added Should be Filter 20
#ifdef _JHDebug_showlog			
			printf("\n f25 ( (Smooth_Non == TRUE)  && ((Safest_Porn == FALSE) && (porn_ret ==1))) "); //JHP	
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif
			goto EndRemove;
		}
		
		else if ((Smooth_Non == TRUE))
		{
			Safest_Non = TRUE;
		}
		
		
		
		// Super Safest Loading 
		//----------------------------------------------------------3-------------
		//spornsvm = CvSVM ();  //changes for opencv4
		// For Unicode Compiling 
	
		
		char ssafestPornSVMFilename[MAX_PATH];

		for (int i=0 ; i < MAX_PATH ; i++)
		{
			ssafestPornSVMFilename[i] = NULL; 
		}
		
		for (int i=0 ; i < strlen(m_szssafestPornSVMFilename) ; i++)
		{
			ssafestPornSVMFilename[i] = m_szssafestPornSVMFilename[i]; 
		}

        spornsvm = Algorithm::load<ml::SVM>(ssafestPornSVMFilename);

		
        sporn_ret =spornsvm->predict(m_porn); //opencv4 update

		
		//if (sporn_ret == 1)
		//	MessageBox (NULL, _T("Porn - super safest") , _T("*******") , NULL);
		
		//----------------------------------------------------------------------
		
		if (porn_ret ==1 && (Safe_Non == TRUE) && (Safest_Porn == FALSE) && (Safest_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 25 - Support Vector Machine) - Safest Engine \n"));
				
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}		
			//MessageBox ( NULL , "Apply Filter 25_1" , "Filter" , NULL);
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=2;
			pResults->FT=25;
#ifdef _JHDebug_showlog
			printf("\n f25 (porn_ret ==1 && (Safe_Non == TRUE) && (Safest_Porn == FALSE) && (Safest_Non == FALSE)) "); //JHP
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);				
#endif
			goto EndRemove;
		}
		else if ((porn_ret ==1) && (Safe_Non == FALSE) && (Safest_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 25 - Support Vector Machine) - Safe Engine \n"));
				
				
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
				
			}
			//MessageBox ( NULL , "Apply Filter 25_2" , "Filter" , NULL);
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=3;
			pResults->FT=25;
#ifdef _JHDebug_showlog
			printf("\n f25 ((porn_ret ==1) && (Safe_Non == FALSE) && (Safest_Non == FALSE)) "); //JHP	
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);				
#endif
				goto EndRemove;
		}
		
		else if ((Safest_Porn == TRUE))
		{
			
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 25 - Support Vector Machine) - Safest Engine \n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
			}
			
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=2;
		//	pResults->FT=25;	//JH -added	//Filter 17/18/19
#ifdef _JHDebug_showlog
			printf("\n f25 ((Safest_Porn == TRUE)) "); //JHP
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif		
			goto EndRemove;
		}
		
		else if ((sporn_ret == 1) && (porn_ret != 1) && (Safest_Non == FALSE))
		{
			if (ShowDetailedImage == TRUE)
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Porn By (Filter 25 - Support Vector Machine) - Super Safest Engine \n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
			}
			
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=1;
			pResults->FT=25;	//JH -added
#ifdef _JHDebug_showlog
			printf("\n f25 ((sporn_ret == 1) && (porn_ret != 1) && (Safest_Non == FALSE)) "); //JHP
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);				
#endif
			goto EndRemove;
		}
		
		else if (Human_Porn == TRUE)
		{
			if (ShowDetailedImage == TRUE)
			{
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
			}
			
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT=1;
		//	pResults->FT=25;	//JH -added Should be Filter 11/12
#ifdef _JHDebug_showlog
			printf("\n f25 (Human_Porn == TRUE) "); //JHP
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);				
#endif
			goto EndRemove;
		}
		
		else
		{
			if (ShowDetailedImage == TRUE && (Safest_Non == FALSE))
			{
			//	_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 25 - Support Vector Machine) - All Engines \n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
			}
			
			else if (ShowDetailedImage == TRUE )
			{
				//_tcscat(ImageAnalysisMsg , _T("****** Detected As Non Porn By (Filter 25 - Support Vector Machine) - All Engines \n"));
			//	MessageBox (NULL , ImageAnalysisMsg , _T("Image Analysis Report") , NULL);
			//	char PreWinName[56];
			//	sprintf (PreWinName , "Image %d - Quick Skin" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Blob" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	sprintf (PreWinName , "Image %d - Face" , NumThumbs);
			//	cvDestroyWindow(PreWinName);
			//	_tcscpy(ImageAnalysisMsg , _T(""));
			}
			
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=600;
			pResults->kosf=660;
			pResults->RT=0;
			pResults->FT=25;
#ifdef _JHDebug_showlog			
			printf("\n f25 (ShowDetailedImage == TRUE && (Safest_Non == FALSE)) "); //JHP
			printf("\n safestporn=%.2f  NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", porn_ret, pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);			
#endif
			goto EndRemove;
		}
		
		//pornsvm.clear();
        pornsvm->clear();
		//spornsvm.clear();
        spornsvm->clear();
		//nonsvm.clear();
        nonsvm->clear();
		nonporn_ret = -1.0;
		porn_ret = -1.0;
		sporn_ret = -1.0;
		
		//---------------------------------------------------------------------------
		//- End of Determine the porn value from the classifier
		//---------------------------------------------------------------------------
		
	EndRemove:
        
		//Release memory
		delete[] red;			red=NULL;
		delete[] green;			green=NULL;
		delete[] blue;			blue=NULL;
		delete[] bw;			bw=NULL;
		delete[] info;			info=NULL;
		delete[] blob;			blob=NULL;
		if (object_map!=NULL) delete[] object_map;	object_map=NULL;
		if (blob_info!=NULL) delete[] blob_info;		blob_info=NULL;
		FaceAttr.clear();
		delete[] edge_total;	edge_total=NULL;
		delete[] direction;		direction=NULL;
		delete[] directionIn;	directionIn=NULL;
		delete[] directionOut;	directionOut=NULL;
        
	}
	//---------------------------------------------------------------------------
	//- End of Perform Detail Filter for image that pass thru Fast Filter
	//---------------------------------------------------------------------------
	
	delete[] hbm2;
	hbm2 = NULL;
	
	return TRUE;
}


/*---------------------------------------------------------------------------
//-  Function Name : AnalysisProcess              
//-  Description   : Process a hbitmap image and obtain the confidence value of pornographic
//-  Parameter     : hbmp - hbitmap image
//pResults - structure contains all the porn features
//enEngine_Type - engine type either of screenshield or ishield
//enScan_Type - scanning type either of normal or quick
//-  Return Value  : 
//-------------------------------------------------------------------------*/

bool ImageProcessing::Old_AnalysisProcess(/*IplImage * */ cv::Mat img,
                                        ImageProcessing::WQ_IP_RESULTS * pResults,
                                        EN_FSIP_ENGINE_TYPE enEngine_Type,
                                        EN_FSIP_SCAN_TYPE enScan_Type
										)
{
		
	// Initialize the structure of porn result
	if (pResults != NULL)
	{
		memset(pResults, 0, sizeof(WQ_IP_RESULTS)); 
	}
	
	// Initialize flag
	pResults->pf=-1;
	pResults->kof=-1;
	pResults->kosf=-1;
	pResults->cf=FALSE;
	
	
	int height, width , channels  , depth , imagesize, widthstep , bpp, type  ;
    uchar* data = (uchar*) img.data;
	
	
	height = img.rows;
    width = img.cols;
    channels = img.channels();
    depth = img.depth();
    imagesize =(int)(size_t)img.size.p;
    widthstep = img.step[0];
    type = img.type();
	
    bpp = (img.depth() & 255) * img.channels() ;
	
	CMBitmapProcessing::initvalues(width , height  , widthstep ,depth  , channels , bpp,type);
	
	
	
	//- Fast Filter(1)-Check the size of the image
	//---------------------------------------------------------------------------
	if (width < 50 && height < 50 )
	{
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=101;
		return TRUE;
	}
	
	uchar* hbmCrop = NULL; 			
	uchar* hbmpCropped = NULL; 	
	
	
	//---------------------------------------------------------------------------
	//- End of Fast Filter(1) 
	//---------------------------------------------------------------------------
	
	
	if (ChangeTo24Bit(hbmCrop,data , bpp))
	{
		
		
		int cropSize = QuickCropTest(hbmCrop , width , height , bpp);
		
		if (cropSize==0)
			CMBitmapProcessing::CloneBitmap(&hbmpCropped, hbmCrop);
		else
			CMBitmapProcessing::ResizeBitmap(&hbmpCropped, hbmCrop,0,0,width,height-cropSize,width,height-cropSize,bpp);
			CMBitmapProcessing::initvalues (width,height-cropSize, widthstep, depth  , channels , bpp,type);	//JH added
			height=height-cropSize; //JHDV1
	}
	
	if (hbmCrop!=NULL) free(hbmCrop); 
	hbmCrop=NULL;
	
		
	
	//---------------------------------------------------------------------------
	//- Fast Filter (2)-Perform quick skin scan 
	//---------------------------------------------------------------------------
	if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type , width , height , bpp)) 
	{
		//MessageBox ( NULL , "Inside Filter 2" , "Filters" , NULL);
#ifdef _JHDebug_showlog
		printf("\nNewQuickSkinScan rejected: pResults->NetPornValue = 0.0f;");
#endif		
		pResults->NetPornValue = 0.0f;
		if (hbmpCropped!=NULL) free(hbmpCropped); 
		hbmpCropped=NULL; 
		return TRUE;
	}
	//---------------------------------------------------------------------------
	//- End of Fast Filter (2)
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Perform Scaling to a predetermine fix ratio size
	//---------------------------------------------------------------------------
	
	uchar* hbm2 = NULL; //initiate a new hbitmap for scaling purpose
	bool scalesuccess=true;
	INT nWidth , nHeight, nDepth, nChannel,nType;
	scalesuccess=ProportionScaling(hbm2,hbmpCropped,  width ,  height ,  depth , channels , bpp , nWidth , nHeight , nDepth , nChannel,type,nType);
	if(scalesuccess==false) 
	{
		free(hbm2);
		hbm2 = NULL;
		if (hbmpCropped!=NULL) free(hbmpCropped);
		hbmpCropped=NULL;
		return false;
	}
	
	//---------------------------------------------------------------------------
	//- End of Perform Scaling
	//---------------------------------------------------------------------------
	
	
	if (hbmpCropped!=NULL) free(hbmpCropped);
	hbmpCropped=NULL;
	
	
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------
	if (nWidth < 20 || nHeight < 20 )
	{
		free(hbm2);
		hbm2 = NULL;
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=121;
		return true;
	}
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------
	
	
    cv::Mat imgskin;
	
    LoadHBITMAP_Reverse(hbm2, imgskin, nWidth, nHeight, bpp);
    BYTE * image = (BYTE * ) imgskin.data;
    BYTE * image2 = (BYTE * ) imgskin.data;
	
	//---------------------------------------------------------------------------
	//- Fast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------
	bool filtersuccess=true;
	bool filterresult=false;
	filtersuccess=FastFilterProcess(hbm2,nWidth , nHeight , nDepth , nChannel,image2,enScan_Type,filterresult,pResults);
	
	if(filtersuccess==false) 
	{
		free(hbm2);
		hbm2 = NULL;
		return false;
	}
	else
	{
		if(filterresult)
		{
			pResults->kof=100;
			if(pResults->NetPornValue>0.5) 
				pResults->pf=1;
			else
				pResults->pf=0;
			free(hbm2);
			hbm2 = NULL;
			return true;
		}
	}
	
	
	//---------------------------------------------------------------------------
	//- End ofFast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------

	//---------------------------------------------------------------------------
	//- Perform Detail Filter for image that pass thru Fast Filter
	//---------------------------------------------------------------------------
	
	if(image)
	{
		INT h,i,w;
		INT width=nWidth;
		INT height=nHeight;
		INT hw=width*height;
		INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
		
		unsigned char *red=new unsigned char[hw];
		unsigned char *green=new unsigned char[hw];
		unsigned char *blue=new unsigned char[hw];
		unsigned char *bw=new unsigned char[hw];
		unsigned char *object_map = NULL;
		struct simonBlob *blob_info = NULL;
		INT *info=new INT[hw];
		INT *blob=new INT[hw];
		bool portraitflag=FALSE;
		bool dominantcolour = FALSE ;
		bool Noisyface = FALSE ;
		bool NoisyFaceColour = FALSE;
		bool portraitindicator=FALSE;
		bool manyfacesindicator=FALSE;
		bool shoulderportraitindicator=FALSE;
		INT object_num=0;
		INT *blob_ptr;
		INT *info_ptr;
		//		unsigned char *edge_ptr;
		INT numberBlobs;
		INT boxBottom,boxTop,boxLeft,boxRight,totalBlobSkin;
		//    INT boxArea;
		FLOAT threshold=0.0;
		INT numberFaces=0;
		INT innerEdges=0;
		INT outerEdges=0;
		INT innerEdgesSum=0;
		INT outerEdgesSum=0;
		INT numberWeakInnerEdges_0=0;
		INT numberWeakInnerEdges_1=0;
		INT numberWeakInnerEdges_2=0;
		INT numberWeakInnerEdges_5=0;
		INT numberWeakOuterEdges_1=0;
		INT numberWeakOuterEdges_2=0;
		INT totalStrongEdges2=0;
		FLOAT diamondSkin;
		INT skinInDiamond;
		INT faceSkin=0;
		//		INT totalSkinArea;
		INT totalBlobArea;
		//		INT number_limb_objects;
		
		INT blob_horizontal80;
		INT blob_horizontal90;
		
		//variables for image display
		INT index=0;
			
		//variables for curvature computation
		INT *direction=new INT[hw];
		INT *directionIn=new INT[hw];
		INT *directionOut=new INT[hw];
		
		//variables for edge computation
		INT *edge_total = new INT[hw];
		INT highThreshold=75;
		INT lowThreshold=45;
		INT lengthThres=10;	
		INT total_points=0;
		
		FLOAT f_numberBlobs,f_numberFaces,f_threshold,f_diamondSkin;
		FLOAT f_boxTop,f_boxBottom,f_boxHeight,edges1,edges2,edges3;
		FLOAT skin1,skin2,skin3,skin4,edgeScore,moment,curves;
		FLOAT c1 , c2 , c3 , c4 , c5 , c6 , c7 , c8 ,c9 , c10;
		//---------------------------------------------------------------------------
		//- End of Initialization variables
		//---------------------------------------------------------------------------
		
		// SVM Classification Variables 
	
        cv::Mat m_porn;
		float porn_ret = -1.0;
		
        Ptr<cv::ml::SVM> pornsvm ;
	
        cv::Mat m_nonporn;
		float nonporn_ret = -1.0;
	
        Ptr<cv::ml::SVM> nonsvm ;
		
		//---------------------------------------------------------------------------
		//- Initialization variables for neural network load
		//---------------------------------------------------------------------------
		FLOAT fPornFeature[55];
		KimPornNet kimnet;
		KimPornResult kimResults;
		//---------------------------------------------------------------------------
		//- End of Initialization variables for neural network load
		//---------------------------------------------------------------------------
		
		// Initialization poINTer to zero
		memset(info, 0 ,  sizeof(INT) * hw);
		memset(blob, 0 , sizeof(INT) * hw);
		memset(bw, 0, sizeof(char) * hw);
		memset(direction, 0 , sizeof(char) * hw);
		memset(directionIn, 0, sizeof(char) * hw);
		memset(directionOut,  0, sizeof(char) * hw);
		memset(edge_total, 0 , sizeof(INT) * hw);
		
		//---------------------------------------------------------------------------
		//- Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		rgbComputeOptimised(image,red,green,blue,height,width,widthbytes);
		//---------------------------------------------------------------------------
		//- End of Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		
		//- Gamma correction
		//---------------------------------------------------------------------------
		GammaCorrectionOptimised(red,green,blue,bw,height,width);		
		//---------------------------------------------------------------------------
		//- End of Gamma correction
		//---------------------------------------------------------------------------
		
	
		//- Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		INT skinpixels=0;
		skinpixels=ObtainSkinInfoOptimised(info,bw,red,green,blue,width,height,widthbytes);
		
		//---------------------------------------------------------------------------
		//- End of Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		
		if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006 || enEngine_Type==FSIP_LAX_ENGINE_APR_2006)
		{
			//---------------------------------------------------------------------------
			//- Determine the edges of the image
			//---------------------------------------------------------------------------
			CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
			//---------------------------------------------------------------------------
			//- End of Determine the edges of the image
			//---------------------------------------------------------------------------
			
			
			//---------------------------------------------------------------------------
			//- Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			CalcMoments(edge_total, width, height, pResults->daMoments);
			//---------------------------------------------------------------------------
			//- End of Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Calculate the edge histogram 
			//---------------------------------------------------------------------------
			CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
			//---------------------------------------------------------------------------
			//- End of Calculate the edge histogram 
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			float percentage_skin = ((float) skinpixels)/((float)hw);
			fPornFeature[0]=percentage_skin;
			
			moment = ((float)pResults->daMoments[0]);
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[1]=moment;
			moment = ((float)pResults->daMoments[1])/0.67;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[2]=moment;
			moment = ((float)pResults->daMoments[2])/0.7;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[3]=moment;
			moment = ((float)pResults->daMoments[3])/0.33;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[4]=moment;
			moment = ((float)pResults->daMoments[4])/0.81;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[5]=moment;
			moment = ((float)pResults->daMoments[5])/0.32;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[6]=moment;
			moment = ((float)pResults->daMoments[6])/0.82;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[7]=moment;
			for(i=0;i<32;i++)
			{
				fPornFeature[i+8]=pResults->dEdgeMap[i];
			}
			//---------------------------------------------------------------------------
			//- End of Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Determine the porn value from the fast porn classifier
			//---------------------------------------------------------------------------
			kimnet.NNValueProcess(m_szFastPornFilename, fPornFeature, &kimResults,40);
			pResults->NetPornValue=kimResults.fNNOutput;
#ifdef _JHDebug_showlog
			printf("\n f4-f5 kimnet.NNValueProcess = %f ", pResults->NetPornValue); //JH	
#endif						
			if (pResults->NetPornValue<0.53698) 
			{
#ifdef _JHDebug_showlog
				printf("\n f4-f5 kimnet.NNValueProcess (pResults->NetPornValue<0.53698) = %f ", pResults->NetPornValue); //JH	
#endif				
				pResults->NetPornValue=0.0f;
				pResults->pf=0;
				pResults->kof=600;
				pResults->kosf=601;
				goto EndRemove;
			}
			
			
		}
		
		//---------------------------------------------------------------------------
		//- Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------
		
		
		if (skinpixels==0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f5 skinpixels==0 "); //JH	
#endif							
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=100;
			pResults->kosf=106;
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Segment the image base on skin properties
		//---------------------------------------------------------------------------
		
		object_map = new unsigned char[hw];
		for(i=0;i<hw;i++) object_map[i]=0;
		object_num=RegionSegmentationOptimised(object_map,red,green,blue,height,width);
		//---------------------------------------------------------------------------
		//- End of Segment the image base on skin properties
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		//if(myfile.is_open()) myfile << "Filter 6\n";
		//		MessageBox ( NULL , "Apply Filter 6" , "Filter" , NULL);
		if (object_num<0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f6 object_num<0 "); //JH	
#endif						
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=207;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		numberBlobs = GetSimonBlobInfoOptimised(object_map, object_num, height, width, bw, info, blob,threshold);
		//---------------------------------------------------------------------------
		//- End of Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- End of Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		if (numberBlobs==0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f7 numberBlobs==0  PV=0"); //JH	
#endif		
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=208;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		blob_info = new struct simonBlob[numberBlobs+1];
		for (i = 0; i < numberBlobs+1; i++)
		{
			blob_info[i].area=0;
			blob_info[i].skinTotal=0;
			blob_info[i].inface=FALSE;
		}
		
		
		boxBottom=height; //largest coordinate y for MER of skin
		boxTop=0; //smallest coordinate y for MER of skin
		boxLeft=width; //smallest coordinate x for MER of skin
		boxRight=0; //largest coordinate x for MER of skin
		blob_ptr=blob; //segmented result
		info_ptr=info; //original skin info before segmentation
		blob_horizontal80=0;
		blob_horizontal90=0;
		totalStrongEdges2=0;
		
		for (h=0;h<height;h++) 
		{
			INT blob_horizontal=0;
			for (w=0;w<width;w++) 
			{
				INT pos=h*width+w;
				if (w>0 && h>0) //horizontal edges
				{ 
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-1));
					if (edge>100) totalStrongEdges2++;
				}
				if (h>0) //vertical edges
				{
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-width));
					if (edge>80) totalStrongEdges2++;
				}
				
				if (*blob_ptr>0)
				{
					blob_horizontal++;
					blob_info[*blob_ptr].area++;
					if (*info_ptr>0) blob_info[*blob_ptr].skinTotal++;
					if (boxBottom>h) boxBottom=h;
					if (boxTop<h) boxTop=h;
					if (boxLeft>w) boxLeft=w;
					if (boxRight<w) boxRight=w;
					
					// sum the inner and outer edges as a measure
					// inner means difference between pixels in the skin blob
					// outer means difference between pixels at the boundary of skin blob
					unsigned char *bw_ptr = bw+pos;
					if (w>0)
					{
						if (*(blob_ptr-1)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
					if (h>0)
					{
						if (*(blob_ptr-width)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-width));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge =abs (*bw_ptr - *(bw_ptr-width));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
				}
				blob_ptr++;
				info_ptr++;
			}
			//determine the total number of horizontal line that is over 80% and 90% of image width
			if (blob_horizontal*100>80*width) blob_horizontal80++;
			if (blob_horizontal*100>90*width) blob_horizontal90++;
		}
		
		
		if (innerEdges != 0 && outerEdges != 0 )
			
		{
			c1 = (100*outerEdges)/innerEdges; // >25
			c2 = (100*outerEdgesSum)/outerEdges; //>7000
			c3 = (100*outerEdgesSum)/outerEdges ; //<300
			c4 = (10000*totalStrongEdges2)/hw ; //< 10
			c5 = (1000*numberWeakInnerEdges_2)/innerEdges ; //>800 
			c6 = (1000*numberWeakInnerEdges_5)/innerEdges; //>600
			c7 = (1000*numberWeakInnerEdges_0)/innerEdges; //; <600 
			c8 = (1000*numberWeakInnerEdges_1)/innerEdges ;//<300
			c9 = (1000*numberWeakOuterEdges_1)/outerEdges ; // ;<600 
			c10 = (1000*numberWeakOuterEdges_2)/outerEdges ; //<400
			
		}
		else 
		{
			c1 = 0; // >25
			c2 = 0; //>7000
			c3 = 0 ; //<300
			c4 = 0 ; //< 10
			c5 = 0 ; //>800 
			c6 = 0; //>600
			c7 = 0; //; <600 
			c8 = 0 ;//<300
			c9 = 0 ; // ;<600 
			c10 =0 ; //<400
			
		}
		
		totalBlobSkin=0;
		totalBlobArea=0;
		for (i = 1; i <= numberBlobs; i++)
		{
			totalBlobSkin+=blob_info[i].skinTotal;
			totalBlobArea+=blob_info[i].area;
		}
		//---------------------------------------------------------------------------
		//- End of Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
		//if(myfile.is_open()) myfile << "Filter 8\n";
		//			MessageBox ( NULL , "Apply Filter 8" , "Filter" , NULL);
		/*if (abs((100*totalBlobSkin)/((boxTop-boxBottom)*(boxRight-boxLeft)))>90)
		 {
		 pResults->NetPornValue=0.0f;
		 pResults->pf=0;
		 pResults->kof=200;
		 pResults->kosf=241;
		 goto EndRemove;
		 }*/
		//---------------------------------------------------------------------------
		//- End of Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
		
		
		// Obtain the ratio position of skin MER as compared to image dimension
		boxBottom = (100*boxBottom)/height;
		boxTop = (100*boxTop)/height;
		boxLeft = (100*boxLeft)/width;
		boxRight = (100*boxRight)/width;	
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (9) - Skin MER is at the side of the image
		//---------------------------------------------------------------------------
		//if(myfile.is_open()) myfile << "Filter 9\n";
		//		MessageBox ( NULL , "Apply Filter 9" , "Filter" , NULL);
		/*if ((boxBottom>45 && boxBottom!=100) || (boxTop<55 && boxTop!=0) || 
		 (boxLeft>70 && boxLeft!=100) || (boxRight<30 && boxRight!=0)) 
		 {
		 pResults->NetPornValue=0.0f;
		 pResults->pf=0;
		 pResults->kof=200;
		 pResults->kosf=209;
		 goto EndRemove;
		 }*/
		//---------------------------------------------------------------------------
		//- End of Fast Filter (9) - Skin MER is at the side of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (10) - Compactness skin over image area, less than 5%
		//---------------------------------------------------------------------------
		/*if (100*totalBlobSkin<5*hw) // was 5
		 {
		 pResults->NetPornValue=0.0f;
		 pResults->pf=0;
		 pResults->kof=200;
		 pResults->kosf=211;
		 goto EndRemove;
		 }*/
		//---------------------------------------------------------------------------
		//- End of Fast Filter (10) - Compactness skin over image area, less than 5%
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------
		
		portraitflag=ObtainFaceInfoOptimised(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, nChannel  , nDepth);
		//---------------------------------------------------------------------------
		//- End of Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------
		// Determine number of faces
		
		numberFaces = FaceAttr.size();
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
		//---------------------------------------------------------------------------
		if(portraitflag==TRUE)
		{
#ifdef _JHDebug_showlog
			printf("\n f11 portraitflag==TRUE PV=0"); //JH	
#endif	
			pResults->pf=0;
			pResults->NetPornValue=0.0f;
			pResults->kof=200;
			if (portraitindicator) pResults->kosf=203;
			if (shoulderportraitindicator) pResults->kosf=206;
			if (manyfacesindicator) pResults->kosf=204;
			// end
			goto EndRemove;
		}
		
		//--------------------------------------------------------------------------------
		// Checking the possibility to detect the face if we removed the added White Border to the image 
		//------------------------------------------------------------------------------
		
		if (numberFaces == 0)
		{
			portraitflag=ObtainFaceInfoOptimised1(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, nChannel , nDepth);
			if(portraitflag==TRUE)
			{
#ifdef _JHDebug_showlog
				printf("\n f11 portraitflag==TRUE, numberFaces == 0 PV=0"); //JH	
#endif	
				pResults->pf=0;
				pResults->NetPornValue=0.0f;
				pResults->kof=200;
				if (portraitindicator) pResults->kosf=203;
				if (shoulderportraitindicator) pResults->kosf=206;
				if (manyfacesindicator) pResults->kosf=204;
				// end
				goto EndRemove;
			}
			
			numberFaces = FaceAttr.size();
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
		//---------------------------------------------------------------------------
		
		if(numberFaces>0)
		{
			NoisyFaceColour = NoisyfacesColour(red , green , blue , height , width);
			
			if (NoisyFaceColour == true && enEngine_Type==FSIP_SAFE_ENGINE_APR_2006)
			{
#ifdef _JHDebug_showlog
				printf("\n f11 numberFaces>0 &NoisyFaceColour == true PV=0"); //JH	
#endif					
				pResults->pf=0;
				pResults->NetPornValue=0.0f;
				pResults->kof=200;
				pResults->kosf=268;
				// end
				goto EndRemove;
				
			}
			
			if ( NoisyFaceColour == false)
			{
				
				Noisyface = Noisyfaces(height , width);
				
				if (Noisyface == true)
				{
#ifdef _JHDebug_showlog
					printf("\n f11 NoisyFaceColour == false & Noisyface == true PV=0"); //JH	
#endif							
					pResults->pf=0;
					pResults->NetPornValue=0.0f;
					pResults->kof=200;
					pResults->kosf=269;
					// end
					goto EndRemove;
					
				}
				dominantcolour = AnalyseDominantColour(red , green , blue , height , width);
				
				if (dominantcolour == false)
				{
#ifdef _JHDebug_showlog
					printf("\n f11 NoisyFaceColour == false & dominantcolour == false PV=0"); //JH	
#endif							
					pResults->pf=0;
					pResults->NetPornValue=0.0f;
					pResults->kof=200;
					pResults->kosf=253;
					// end
					goto EndRemove;
				}
				if (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400)
				{
#ifdef _JHDebug_showlog
					printf("\n f11 NoisyFaceColour == false & dominantcolour == true PV=1"); //JH	
#endif					
					pResults->NetPornValue=1.0f;
					pResults->pf=1;
					pResults->kof=200;
					pResults->kosf=254;
					goto EndRemove;
					
				}
				else 
				{
#ifdef _JHDebug_showlog
					printf("\n f11 NoisyFaceColour == false & !dominantcolour == true & c PV=0"); //JH	
#endif	
					pResults->NetPornValue=0;
					pResults->pf=0.0f;
					pResults->kof=200;
					pResults->kosf=257;
					goto EndRemove;
				}
				
			}
		}
		
		if(enEngine_Type==FSIP_LAX_ENGINE_APR_2006)
		{
			
			if(numberFaces<=0)
			{
#ifdef _JHDebug_showlog
				printf("\n f11 numberFaces<=0 PV=0"); //JH	
#endif				
				pResults->NetPornValue=0;
				pResults->pf=0.0f;
				pResults->kof=200;
				pResults->kosf=258;
				goto EndRemove;
			}
		}
		
		
		//---------------------------------------------------------------------------
		//- End of Analyse the human strucutre below the face to determine the nude area and topology
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		DiamondSkinOptimal(blob,width,height,diamondSkin,skinInDiamond);
		//---------------------------------------------------------------------------
		//- End of Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		if (diamondSkin>0.90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) 
		{
#ifdef _JHDebug_showlog
			printf("\n f12 diamondSkin>0.90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif	
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=212;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		if (boxTop - boxBottom<40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f13 boxTop - boxBottom<40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=220;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------

		if (((100*outerEdges)/innerEdges)>25 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) 
		{
#ifdef _JHDebug_showlog
			printf("\n f14 100*outerEdges)/innerEdges)>25 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=221;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------

		if ((100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) // was 150
		{
#ifdef _JHDebug_showlog
			printf("\n f15 100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=225;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (16) - ratio of face skin as compared to the area of blob
		//---------------------------------------------------------------------------

		if (((100*faceSkin)/totalBlobArea)>20 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f16 (100*faceSkin)/totalBlobArea)>20 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=222;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (16) - ratio of face skin as compared to the area of blob
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if (((100*totalBlobSkin)/totalBlobArea)>97 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f17 (100*totalBlobSkin)/totalBlobArea)>97 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif	
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=224;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/hw > 40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f18 (100*totalBlobSkin)/hw > 40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif				
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=228;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/totalBlobArea > 90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f19 (100*totalBlobSkin)/totalBlobArea > 90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif		
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=229;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		
		//- Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
		if (((10000*totalStrongEdges2)/hw < 10) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f20 (10000*totalStrongEdges2)/hw < 10) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif		
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=230;
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
	
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		if (((100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f21 (100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=231;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		if ((((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f22 ((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=232;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		if ((((1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f23 (1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=233;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		if ((((1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n ((f24 1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=234;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the number of brightness points
		//---------------------------------------------------------------------------
		CurvaturePoint (bw,blob, height, width, direction, directionIn, directionOut);
		for (h=0;h<height;h++)
			for (w=0;w<width;w++)
			{
				index = w+w+w;
				INT val = ((h/6)*width) + w/6;
				if (directionIn[val]>4) if (h%6==0 && w%6==0) total_points++;
				if (directionOut[val]>4) if (h%6==0 && w%6==0) total_points++;
			}
		pResults->numberOfCurvature = total_points;
		//---------------------------------------------------------------------------
		//- End of Determine the number of brightness points
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the edges of the image
		//---------------------------------------------------------------------------
		CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
		//---------------------------------------------------------------------------
		//- End of Determine the edges of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		CalcMoments(edge_total, width, height, pResults->daMoments);
		//---------------------------------------------------------------------------
		//- End of Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate the edge histogram 
		//---------------------------------------------------------------------------
		CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
		//---------------------------------------------------------------------------
		//- End of Calculate the edge histogram 
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Compute 55 feature for porn classifier
		//---------------------------------------------------------------------------
		//Feature (1)
		f_numberBlobs = ((float)(numberBlobs-1))/5.0; // was / 14.0
		if (f_numberBlobs<0.0) f_numberBlobs=0.0;
		if (f_numberBlobs>1.0) f_numberBlobs=1.0;
		fPornFeature[0]=f_numberBlobs;
		//Feature (2)
		f_numberFaces = ((float)numberFaces);
		if (f_numberFaces<0.0) f_numberFaces=0.0;
		if (f_numberFaces>1.0) f_numberFaces=1.0;
		fPornFeature[1]=f_numberFaces;
		//Feature (3)
		f_threshold = (threshold-0.1)/0.7;
		if (f_threshold<0.0) f_threshold=0.0;
		if (f_threshold>1.0) f_threshold=1.0;
		fPornFeature[2]=f_threshold;
		//Feature (4)
		f_diamondSkin = (diamondSkin)/0.9;
		if (f_diamondSkin<0.0) f_diamondSkin=0.0;
		if (f_diamondSkin>1.0) f_diamondSkin=1.0;
		fPornFeature[3]=f_diamondSkin;
		//Feature (5)
		f_boxTop = ((float) (boxTop-60))/40.0;
		if (f_boxTop<0.0) f_boxTop=0.0;
		if (f_boxTop>1.0) f_boxTop=1.0;
		fPornFeature[4]=f_boxTop;
		//Feature (6)
		f_boxBottom = ((float) (boxBottom))/45.0;
		if (f_boxBottom<0.0) f_boxBottom=0.0;
		if (f_boxBottom>1.0) f_boxBottom=1.0;
		fPornFeature[5]=f_boxBottom;
		//Feature (7)
		f_boxHeight = ((float) (boxTop-boxBottom-40))/60.0;
		if (f_boxHeight<0.0) f_boxHeight=0.0;
		if (f_boxHeight>1.0) f_boxHeight=1.0;
		fPornFeature[6]=f_boxHeight;
		//Feature (8)
		edges1 = ((float)(((100*outerEdges)/innerEdges)-1))/21.0;
		if (edges1<0.0) edges1=0.0;
		if (edges1>1.0) edges1=1.0;
		fPornFeature[7]=edges1;
		//Feature (9)
		edges2 = ((float)(((100*outerEdgesSum)/outerEdges)-304))/6500.0;
		if (edges2<0.0) edges2=0.0;
		if (edges2>1.0) edges2=1.0;
		fPornFeature[8]=edges2;
		//Feature (10)
		edges3 = ((float)(((100*innerEdgesSum)/innerEdges)-128))/1200.0;
		if (edges3<0.0) edges3=0.0;
		if (edges3>1.0) edges3=1.0;
		fPornFeature[9]=edges3;
		//Feature (11)
		skin1 = ((float)(((100*totalBlobSkin)/hw)-5))/35.0;
		if (skin1<0.0) skin1=0.0;
		if (skin1>1.0) skin1=1.0;
		fPornFeature[10]=skin1;
		//Feature (12)
		skin2 = ((float)(((100*totalBlobSkin)/totalBlobArea)-21))/69.0;
		if (skin2<0.0) skin2=0.0;
		if (skin2>1.0) skin2=1.0;
		fPornFeature[11]=skin2;
		//Feature (13)
		skin3 = ((float)(((100*faceSkin)/totalBlobArea)))/20.0;
		if (skin3<0.0) skin3=0.0;
		if (skin3>1.0) skin3=1.0;
		fPornFeature[12]=skin3;
		//Feature (14)
		skin4 = ((float)(((100*skinInDiamond)/totalBlobArea)))/71.0;
		if (skin4<0.0) skin4=0.0;
		if (skin4>1.0) skin4=1.0;
		fPornFeature[13]=skin4;
		//Feature (15)
		edgeScore = ((float)(((10000*totalStrongEdges2)/hw)))/2500.0;
		if (edgeScore<0.0) edgeScore=0.0;
		if (edgeScore>1.0) edgeScore=1.0;
		fPornFeature[14]=edgeScore;
		//Feature (16 - 22) - 7 Hu Moments
		moment = ((float)pResults->daMoments[0]);
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[15]=moment;
		moment = ((float)pResults->daMoments[1])/0.67;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[16]=moment;
		moment = ((float)pResults->daMoments[2])/0.7;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[17]=moment;
		moment = ((float)pResults->daMoments[3])/0.33;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[18]=moment;
		moment = ((float)pResults->daMoments[4])/0.81;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[19]=moment;
		moment = ((float)pResults->daMoments[5])/0.32;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[20]=moment;
		moment = ((float)pResults->daMoments[6])/0.82;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[21]=moment;
		//Feature (23) - curvature points
		curves = ((float)pResults->numberOfCurvature)/5.0;
		if (curves<0.0) curves=0.0;
		if (curves>1.0) curves=1.0;
		fPornFeature[22]=curves;
		//Feature (24-55) - edge histogram
		for(i=23;i<55;i++)
		{
			fPornFeature[i]=pResults->dEdgeMap[i-23];
		}
		
		//---------------------------------------------------------------------------
		//- End of Compute 55 feature for porn classifier
		//---------------------------------------------------------------------------
			
		
		//---------------------------------------------------------------------------
		//- SVM porn classifier
		//---------------------------------------------------------------------------
		
        pornsvm->load (m_szsafestPornSVMFilename_old); //changes to update opencv4

        m_porn = cv::Mat(1, 55, CV_32FC1, fPornFeature);
	//
         porn_ret = pornsvm->predict (m_porn);
		
		if (porn_ret ==1)
		{

			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			goto EndRemove;
		}
		
		// Pron Vector Machine	
		
        nonsvm->load (m_szsafestNonPornSVMFilename); //update for opencv4
	
        m_nonporn= cv::Mat(1, 55, CV_32FC1, NULL);
		cvSetData (&m_nonporn, fPornFeature, sizeof (float) * 55);
        nonporn_ret = nonsvm->predict (m_nonporn);
		
		if (nonporn_ret == 1)
		{ 
#ifdef _JHDebug_showlog
			printf("\n ((f25 SVM porn classifier :: nonporn_ret == 1 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=600;
			pResults->kosf=660;
			goto EndRemove;
		}
		
		
        pornsvm->clear();
        nonsvm->clear();
		nonporn_ret = -1.0;
		porn_ret = -1.0;
		
		//---------------------------------------------------------------------------
		//- End of Determine the porn value from the classifier
		//---------------------------------------------------------------------------
		
	EndRemove:
		//Release memory
		delete[] red;			red=NULL;
		delete[] green;			green=NULL;
		delete[] blue;			blue=NULL;
		delete[] bw;			bw=NULL;
		delete[] info;			info=NULL;
		delete[] blob;			blob=NULL;
		if (object_map!=NULL) delete[] object_map;	object_map=NULL;
		if (blob_info!=NULL) delete[] blob_info;		blob_info=NULL;
		FaceAttr.clear();
		delete[] edge_total;	edge_total=NULL;
		delete[] direction;		direction=NULL;
		delete[] directionIn;	directionIn=NULL;
		delete[] directionOut;	directionOut=NULL;
		
		
		
	}
	
	
	
	free(hbm2);
	hbm2 = NULL;

	return TRUE;
}



bool ImageProcessing::AnalysisProcess(/*IplImage *img*/ Mat img,
									  ImageProcessing::WQ_IP_RESULTS * pResults,
									  EN_FSIP_ENGINE_TYPE enEngine_Type,
									  EN_FSIP_SCAN_TYPE enScan_Type
									  )
{
	
	// Initialize the structure of porn result
	if (pResults != NULL)
	{
		memset(pResults, 0, sizeof(WQ_IP_RESULTS)); 
	}
	
	// Initialize flag
	pResults->pf=-1;
	pResults->kof=-1;
	pResults->kosf=-1;
	pResults->RT=-1;
	pResults->FT=-1;	
	pResults->cf=FALSE;
	
	
	int height, width , channels  , depth , imagesize, widthstep , bpp ,type ;
    uchar* data = (uchar*) img.data;   //img->imageData;  update to opencv4
	
	
    height = img.rows;
    width = img.cols;
    channels = img.channels();  
    depth = img.depth();    
    imagesize = (int)(size_t)img.size.p;   
    widthstep = img.step;
    type = img.type();
	
    bpp = (img.depth() & 255) * img.channels();  

	CMBitmapProcessing::initvalues(width , height  , widthstep ,depth  , channels , bpp,type);
	
	
	
	//- Fast Filter(1)-Check the size of the image
	//---------------------------------------------------------------------------
	if (width < 50 && height < 50 )
	{
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=101;
		pResults->RT=0;
		pResults->FT=1;
		return TRUE;
	}
	
	uchar* hbmCrop = NULL; 			
	uchar* hbmpCropped = NULL; 	
	
	
	//---------------------------------------------------------------------------
	//- End of Fast Filter(1) 
	//---------------------------------------------------------------------------
	
	
	if (ChangeTo24Bit(hbmCrop,data , bpp))
	{
		
		

		
		
		int cropSize = QuickCropTest(hbmCrop , width , height , bpp);
		
		if (cropSize==0)
			CMBitmapProcessing::CloneBitmap(&hbmpCropped, hbmCrop);
		else
			CMBitmapProcessing::ResizeBitmap(&hbmpCropped, hbmCrop,0,0,width,height-cropSize,width,height-cropSize,bpp);
		CMBitmapProcessing::initvalues (width,height-cropSize, widthstep, depth  , channels , bpp,type);	//JH added
		height=height-cropSize; //JHDV1
	}
	
	if (hbmCrop!=NULL) free(hbmCrop); 
	hbmCrop=NULL;
	
	
	
	//---------------------------------------------------------------------------
	//- Fast Filter (2)-Perform quick skin scan 
	//---------------------------------------------------------------------------
	if (NewQuickSkinScan(hbmpCropped,pResults,enScan_Type , width , height , bpp)) 
	{
		//MessageBox ( NULL , "Inside Filter 2" , "Filters" , NULL);
#ifdef _JHDebug_showlog
		printf("\nNewQuickSkinScan rejected: pResults->NetPornValue = 0.0f;");
#endif		
		pResults->NetPornValue = 0.0f;
		pResults->RT=0;
		pResults->FT=2;
		if (hbmpCropped!=NULL) free(hbmpCropped); 
		hbmpCropped=NULL; 
		return TRUE;
	}
	//---------------------------------------------------------------------------
	//- End of Fast Filter (2)
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Perform Scaling to a predetermine fix ratio size
	//---------------------------------------------------------------------------
	
	uchar* hbm2 = NULL; //initiate a new hbitmap for scaling purpose
	bool scalesuccess=true;
	INT nWidth , nHeight, nDepth, nChannel,nType;
	scalesuccess=ProportionScaling(hbm2,hbmpCropped,  width ,  height ,  depth , channels , bpp , nWidth , nHeight , nDepth , nChannel,type,nType);
	if(scalesuccess==false) 
	{
		free(hbm2);
		hbm2 = NULL;
		if (hbmpCropped!=NULL) free(hbmpCropped);
		hbmpCropped=NULL;
		return false;
	}
	
	//---------------------------------------------------------------------------
	//- End of Perform Scaling
	//---------------------------------------------------------------------------
	
	
	
	
	if (hbmpCropped!=NULL) free(hbmpCropped);
	hbmpCropped=NULL;
	
	
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------
	if (nWidth < 20 || nHeight < 20 )
	{
		free(hbm2);
		hbm2 = NULL;
		pResults->NetPornValue = 0.0f;
		pResults->pf=0;
		pResults->kof=100;
		pResults->kosf=121;
		pResults->RT=0;
		pResults->FT=3;		
		return true;
	}
	//---------------------------------------------------------------------------
	//- Fast Filter(3)-Check the size of the image
	//---------------------------------------------------------------------------
	
	
    cv::Mat imgskin;

    LoadHBITMAP_Reverse(hbm2, imgskin, nWidth, nHeight, bpp);
    BYTE * image = (BYTE * ) imgskin.data;
	BYTE * image2 = (BYTE * ) imgskin.data;
	
	//---------------------------------------------------------------------------
	//- Fast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------
	bool filtersuccess=true;
	bool filterresult=false;
	filtersuccess=FastFilterProcess(hbm2,nWidth , nHeight , nDepth , nChannel,image2,enScan_Type,filterresult,pResults);
	
	if(filtersuccess==false) 
	{
		free(hbm2);
		hbm2 = NULL;
		return false;
	}
	else
	{
		if(filterresult)
		{
			pResults->kof=100;
			if(pResults->NetPornValue>0.5) {
				pResults->pf=1;
				pResults->RT = 1;
				pResults->FT =4;
			}
			else{
				pResults->pf=0;
				pResults->RT = 0;
				pResults->FT =4;
			}
			free(hbm2);
			hbm2 = NULL;
			return true;
		}
	}
	
	
	//---------------------------------------------------------------------------
	//- End ofFast Filter (4) - type of colour, homogeneous, number of colour
	//---------------------------------------------------------------------------
	
	//---------------------------------------------------------------------------
	//- Perform Detail Filter for image that pass thru Fast Filter
	//---------------------------------------------------------------------------
	
	if(image)
	{
		INT h,i,w;
		INT width=nWidth;
		INT height=nHeight;
		INT hw=width*height;
		INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
		
		unsigned char *red=new unsigned char[hw];
		unsigned char *green=new unsigned char[hw];
		unsigned char *blue=new unsigned char[hw];
		unsigned char *bw=new unsigned char[hw];
		unsigned char *object_map = NULL;
		struct simonBlob *blob_info = NULL;
		INT *info=new INT[hw];
		INT *blob=new INT[hw];
		bool portraitflag=FALSE;
		bool dominantcolour = FALSE ;
		bool portraitindicator=FALSE;
		INT profileindicator = 0;	//JH 
		
		bool manyfacesindicator=FALSE;
		bool shoulderportraitindicator=FALSE;
		
		INT object_num=0;
		INT *blob_ptr;
		INT *info_ptr;
		//		unsigned char *edge_ptr;
		INT numberBlobs;
		INT boxBottom,boxTop,boxLeft,boxRight,totalBlobSkin;
		//    INT boxArea;
		FLOAT threshold=0.0;
		INT numberFaces=0;
		INT innerEdges=0;
		INT outerEdges=0;
		INT innerEdgesSum=0;
		INT outerEdgesSum=0;
		INT numberWeakInnerEdges_0=0;
		INT numberWeakInnerEdges_1=0;
		INT numberWeakInnerEdges_2=0;
		INT numberWeakInnerEdges_5=0;
		INT numberWeakOuterEdges_1=0;
		INT numberWeakOuterEdges_2=0;
		INT totalStrongEdges2=0;
		FLOAT diamondSkin;
		INT skinInDiamond;
		INT faceSkin=0;
		//		INT totalSkinArea;
		INT totalBlobArea;
		//		INT number_limb_objects;
		
		INT blob_horizontal80;
		INT blob_horizontal90;
		
		//     Added For New Engine 
		FLOAT BlobAreaRatio = 0.0;		//JH
		FLOAT BlobSkinRation = 0.0;	//JH
		
		//variables for image display
		INT index=0;
		
		//variables for curvature computation
		INT *direction=new INT[hw];
		INT *directionIn=new INT[hw];
		INT *directionOut=new INT[hw];
		
		//variables for edge computation
		INT *edge_total = new INT[hw];
		INT highThreshold=75;
		INT lowThreshold=45;
		INT lengthThres=10;	
		INT total_points=0;
		
		FLOAT f_numberBlobs,f_numberFaces,f_threshold,f_diamondSkin;
		FLOAT f_boxTop,f_boxBottom,f_boxHeight,edges1,edges2,edges3;
		FLOAT skin1,skin2,skin3,skin4,edgeScore,moment,curves;
		FLOAT fBodyskin , fFaceSkin;
		FLOAT c1 , c2 , c3 , c4 , c5 , c6 , c7 , c8 ,c9 , c10;
		//---------------------------------------------------------------------------
		//- End of Initialization variables
		//---------------------------------------------------------------------------
		
		// SVM Classification Variables 
		//CvMat m_porn; //update opencv4cvmat
        cv::Mat m_porn;
		float porn_ret = -1.0;
	//	CvSVM pornsvm;
        Ptr<ml::SVM> pornsvm = ml::SVM::create();
		fBodyskin = 0; //JH
		fFaceSkin = 0;//JH
		
		//CvMat m_nonporn;
        cv::Mat m_nonporn;
		float nonporn_ret = -1.0;
	//	CvSVM nonsvm;
        Ptr<ml::SVM> nonsvm = ml::SVM::create();
		//---------------------------------------------------------------------------
		//- Initialization variables for neural network load
		//---------------------------------------------------------------------------
		FLOAT fPornFeature[55];
		KimPornNet kimnet;
		KimPornResult kimResults;
		//---------------------------------------------------------------------------
		//- End of Initialization variables for neural network load
		//---------------------------------------------------------------------------
		
		// Initialization poINTer to zero
		memset(info, 0 ,  sizeof(INT) * hw);
		memset(blob, 0 , sizeof(INT) * hw);
		memset(bw, 0, sizeof(char) * hw);
		memset(direction, 0 , sizeof(char) * hw);
		memset(directionIn, 0, sizeof(char) * hw);
		memset(directionOut,  0, sizeof(char) * hw);
		memset(edge_total, 0 , sizeof(INT) * hw);
		
		//---------------------------------------------------------------------------
		//- Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		rgbComputeOptimised(image,red,green,blue,height,width,widthbytes);
		//---------------------------------------------------------------------------
		//- End of Determine the red,green and blue component of the image
		//---------------------------------------------------------------------------
		
		//- Gamma correction
		//---------------------------------------------------------------------------
		GammaCorrectionOptimised(red,green,blue,bw,height,width);		
		//---------------------------------------------------------------------------
		//- End of Gamma correction
		//---------------------------------------------------------------------------
		
		
		//- Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		INT skinpixels=0;
		INT ratioskin=0;	//JH
		skinpixels=ObtainSkinInfoOptimised(info,bw,red,green,blue,width,height,widthbytes);
		
		//---------------------------------------------------------------------------
		//- End of Determine skin pixel base on lookup table, skin shade and texture analysis
		//---------------------------------------------------------------------------
		
		//	if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006 || enEngine_Type==FSIP_LAX_ENGINE_APR_2006)
		if(enEngine_Type==FSIP_SAFE_ENGINE_APR_2006 || enEngine_Type==FSIP_LAX_ENGINE_APR_2006 || enEngine_Type==FSIP_SLAX_ENGINE_APR_2006)
		{
			//---------------------------------------------------------------------------
			//- Determine the edges of the image
			//---------------------------------------------------------------------------
			CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
			//---------------------------------------------------------------------------
			//- End of Determine the edges of the image
			//---------------------------------------------------------------------------
			
			
			//---------------------------------------------------------------------------
			//- Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			CalcMoments(edge_total, width, height, pResults->daMoments);
			//---------------------------------------------------------------------------
			//- End of Calculate Hu moments from the edge data
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Calculate the edge histogram 
			//---------------------------------------------------------------------------
			CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
			//---------------------------------------------------------------------------
			//- End of Calculate the edge histogram 
			//---------------------------------------------------------------------------
			
			
#ifdef _SVMTRAIN
			{
				
				// Not normalized features 
				//---------------------------------------------------------------------------
				//- Compute 40 feature for porn classifier
				//---------------------------------------------------------------------------
				float percentage_skin = ((float) skinpixels)/((float)hw);
				fPornFeature[0]=percentage_skin;
				
				moment = ((float)pResults->daMoments[0]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[1]=moment;
				moment = ((float)pResults->daMoments[1]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[2]=moment;
				moment = ((float)pResults->daMoments[2]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[3]=moment;
				moment = ((float)pResults->daMoments[3]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[4]=moment;
				moment = ((float)pResults->daMoments[4]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[5]=moment;
				moment = ((float)pResults->daMoments[5]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[6]=moment;
				moment = ((float)pResults->daMoments[6]);
				if (moment<0.0) moment=0.0;
				if (moment>1.0) moment=1.0;
				fPornFeature[7]=moment;
				for(i=0;i<32;i++)
				{
					fPornFeature[i+8]=pResults->dEdgeMap[i];
				}
				//MessageBox ( NULL , "Inside Safest Engine" , "Dumping File" , NULL);
				FILE *out;
				LPTSTR lpszFilename = _T("C:\\train\\SVM\\features_safe_NonNorm.txt");
				out = _tfopen(lpszFilename,_T("a"));
				if (out!=NULL)
				{ 
					for(i=0;i<40;i++)
					{
						_ftprintf(out,_T("%f "),fPornFeature[i]);
						
					}
					
					_ftprintf(out,_T("\n"));
					fclose(out);
				}
				if (out!=NULL)
				{
					fclose(out);
				}
			}
#endif		
			
			
			//---------------------------------------------------------------------------
			//- Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			float percentage_skin = ((float) skinpixels)/((float)hw);
			fPornFeature[0]=percentage_skin;
			
			moment = ((float)pResults->daMoments[0]);
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[1]=moment;
			moment = ((float)pResults->daMoments[1])/0.67;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[2]=moment;
			moment = ((float)pResults->daMoments[2])/0.7;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[3]=moment;
			moment = ((float)pResults->daMoments[3])/0.33;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[4]=moment;
			moment = ((float)pResults->daMoments[4])/0.81;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[5]=moment;
			moment = ((float)pResults->daMoments[5])/0.32;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[6]=moment;
			moment = ((float)pResults->daMoments[6])/0.82;
			if (moment<0.0) moment=0.0;
			if (moment>1.0) moment=1.0;
			fPornFeature[7]=moment;
			for(i=0;i<32;i++)
			{
				fPornFeature[i+8]=pResults->dEdgeMap[i];
			}
			//---------------------------------------------------------------------------
			//- End of Compute 40 features for fast processing in Safe Engine
			//---------------------------------------------------------------------------
			
#ifdef _SVMTRAIN
			{
				//MessageBox ( NULL , "Inside Safe Engine" , "Dumping File" , NULL);
				FILE *out;
				LPTSTR lpszFilename = _T("C:\\train\\SVM\\features_safe.txt");
				out = _tfopen(lpszFilename,_T("a"));
				if (out!=NULL)
				{ 
					for(i=0;i<40;i++)
					{
						_ftprintf(out,_T("%f "),fPornFeature[i]);
						
					}
					
					_ftprintf(out,_T("\n"));
					fclose(out);
				}
				if (out!=NULL)
				{
					fclose(out);
				}
			}
#endif		
			
			// New SVM Classifier Developed By Homam 
			
			//CvANN_MLP NN;  //opencv4 update
            Ptr<cv::ml::ANN_MLP> NN = cv::ml::ANN_MLP::create();
            
			
			// For Unicode Compiling 
			char safeNonPornNNFilename[MAX_PATH] = {0};
			
			for (int i=0 ; i < MAX_PATH ; i++)
			{
				safeNonPornNNFilename[i] = NULL; 
			}
			
			//	for (int i=0 ; i < _tcslen(m_szsafeNonPornNNFilename) ; i++)
			//	{
			//		safeNonPornNNFilename[i] = m_szsafeNonPornNNFilename[i]; 
			//	}
			for (int i=0 ; i < strlen(m_szsafeNonPornNNFilename) ; i++)
			{
				safeNonPornNNFilename[i] = m_szsafeNonPornNNFilename[i]; 
			}
			
			//NN.load (safeNonPornNNFilename); //opencv4 update
            Algorithm::load<ml::ANN_MLP>(safeNonPornNNFilename);

		//	cvInitMatHeader (&m_porn, 1, 40, CV_32FC1, NULL);  //opencv4 updated
            m_porn =  cv::Mat(1, 40, CV_32FC1, fPornFeature);
            
		//	cvSetData (&m_porn, fPornFeature, sizeof (float) * 40);
		//	CvMat *classificationResult = cvCreateMat(1, 2, CV_32FC1);
            
            cv::Mat classificationResult;
            classificationResult.create(1, 2, CV_32FC1);
            
			//NN.predict (&m_porn , classificationResult); //updated opencv4
            
            NN->predict (m_porn , classificationResult);
			int pred = 0;
			float max_value = classificationResult.data[0];//classificationResult->data.fl[0];
			
			for (int k=1;k<2;k++) 
			{
				if (max_value<classificationResult.data[k]) //classificationResult->data.fl[k])
				{
					pred=k;
					max_value=classificationResult.data[k];//classificationResult->data.fl[k];
				}
			}
			
			if (pred == 0) 
			{
				pResults->NetPornValue=0.0f;
				pResults->pf=0;
				pResults->kof=600;
				pResults->kosf=601;
				pResults->RT = 0;
				pResults->FT =51;
				goto EndRemove;
			}
			
			//---------------------------------------------------------------------------
			//- End of Determine the porn value from the fast porn classifier
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Determine the porn value from the fast porn classifier
			//---------------------------------------------------------------------------
			//JH --- *** Not applicable in Windows AnalysisProcess 
			kimnet.NNValueProcess(m_szFastPornFilename, fPornFeature, &kimResults,40);
			pResults->NetPornValue=kimResults.fNNOutput;
#ifdef _JHDebug_showlog
			printf("\n f4-f5 kimnet.NNValueProcess = %f ", pResults->NetPornValue); //JH	
#endif						
			if (pResults->NetPornValue<0.53698) 
			{
#ifdef _JHDebug_showlog
				printf("\n f4-f5 kimnet.NNValueProcess (pResults->NetPornValue<0.53698) = %f ", pResults->NetPornValue); //JH	
#endif				
				pResults->NetPornValue=0.0f;
				pResults->pf=0;
				pResults->kof=600;
				pResults->kosf=601;
				goto EndRemove;
			}
			//JH --- *** Not applicable in Windows AnalysisProcess 
			
		}
		
		//---------------------------------------------------------------------------
		//- Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------
		
		
		if (skinpixels==0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f5 skinpixels==0 "); //JH	
#endif							
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=100;
			pResults->kosf=106;
			pResults->RT = 0;	//JH
			pResults->FT =5;	//JH		
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (5) - If no skin pixels after removing dodgy texture
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Segment the image base on skin properties
		//---------------------------------------------------------------------------
		
		object_map = new unsigned char[hw];
		for(i=0;i<hw;i++) object_map[i]=0;
		object_num=RegionSegmentationOptimised(object_map,red,green,blue,height,width);
		//---------------------------------------------------------------------------
		//- End of Segment the image base on skin properties
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		//if(myfile.is_open()) myfile << "Filter 6\n";
		//		MessageBox ( NULL , "Apply Filter 6" , "Filter" , NULL);
		if (object_num<0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f6 object_num<0 "); //JH	
#endif						
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=207;
			pResults->RT=0;	//JH
			pResults->FT=6;	//JH		
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (6) - If no colour cluster after segmentation
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		numberBlobs = GetSimonBlobInfoOptimised(object_map, object_num, height, width, bw, info, blob,threshold);
		//---------------------------------------------------------------------------
		//- End of Determine the number of blob by blob analysis on area, skinarea, total edge and value
		//- End of Determine the adaptive skin threshold value
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		if (numberBlobs==0) 
		{
#ifdef _JHDebug_showlog
			printf("\n f7 numberBlobs==0  PV=0"); //JH	
#endif		
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=208;
			pResults->RT=0;	//JH
			pResults->FT=7;	//JH		
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (7) - If no blob after blob analysis
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		blob_info = new struct simonBlob[numberBlobs+1];
		for (i = 0; i < numberBlobs+1; i++)
		{
			blob_info[i].area=0;
			blob_info[i].skinTotal=0;
			blob_info[i].inface=FALSE;
		}
		
		
		boxBottom=height; //largest coordinate y for MER of skin
		boxTop=0; //smallest coordinate y for MER of skin
		boxLeft=width; //smallest coordinate x for MER of skin
		boxRight=0; //largest coordinate x for MER of skin
		blob_ptr=blob; //segmented result
		info_ptr=info; //original skin info before segmentation
		blob_horizontal80=0;
		blob_horizontal90=0;
		totalStrongEdges2=0;
		
		for (h=0;h<height;h++) 
		{
			INT blob_horizontal=0;
			for (w=0;w<width;w++) 
			{
				INT pos=h*width+w;
				if (w>0 && h>0) //horizontal edges
				{ 
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-1));
					if (edge>100) totalStrongEdges2++;
				}
				if (h>0) //vertical edges
				{
					unsigned char *bw_ptr = bw+pos;
					INT edge = abs(*bw_ptr - *(bw_ptr-width));
					if (edge>80) totalStrongEdges2++;
				}
				
				if (*blob_ptr>0)
				{
					blob_horizontal++;
					blob_info[*blob_ptr].area++;
					if (*info_ptr>0) blob_info[*blob_ptr].skinTotal++;
					if (boxBottom>h) boxBottom=h;
					if (boxTop<h) boxTop=h;
					if (boxLeft>w) boxLeft=w;
					if (boxRight<w) boxRight=w;
					
					// sum the inner and outer edges as a measure
					// inner means difference between pixels in the skin blob
					// outer means difference between pixels at the boundary of skin blob
					unsigned char *bw_ptr = bw+pos;
					if (w>0)
					{
						if (*(blob_ptr-1)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-1));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
					if (h>0)
					{
						if (*(blob_ptr-width)>0) 
						{
							INT edge = abs(*bw_ptr - *(bw_ptr-width));
							innerEdgesSum+=edge;
							if (edge>0) numberWeakInnerEdges_0++;
							if (edge>1) numberWeakInnerEdges_1++;
							if (edge>2) numberWeakInnerEdges_2++;
							if (edge>5) numberWeakInnerEdges_5++;
							innerEdges++;
						}
						else
						{
							INT edge =abs (*bw_ptr - *(bw_ptr-width));
							outerEdgesSum+=edge;
							if (edge>1) numberWeakOuterEdges_1++;
							if (edge>2) numberWeakOuterEdges_2++;
							outerEdges++;
						}
					}
				}
				blob_ptr++;
				info_ptr++;
			}
			//determine the total number of horizontal line that is over 80% and 90% of image width
			if (blob_horizontal*100>80*width) blob_horizontal80++;
			if (blob_horizontal*100>90*width) blob_horizontal90++;
		}
		
		
		if (innerEdges != 0 && outerEdges != 0 )
			
		{
			c1 = (100*outerEdges)/innerEdges; // >25
			c2 = (100*outerEdgesSum)/outerEdges; //>7000
			c3 = (100*outerEdgesSum)/outerEdges ; //<300
			c4 = (10000*totalStrongEdges2)/hw ; //< 10
			c5 = (1000*numberWeakInnerEdges_2)/innerEdges ; //>800 
			c6 = (1000*numberWeakInnerEdges_5)/innerEdges; //>600
			c7 = (1000*numberWeakInnerEdges_0)/innerEdges; //; <600 
			c8 = (1000*numberWeakInnerEdges_1)/innerEdges ;//<300
			c9 = (1000*numberWeakOuterEdges_1)/outerEdges ; // ;<600 
			c10 = (1000*numberWeakOuterEdges_2)/outerEdges ; //<400
			
		}
		else 
		{
			c1 = 0; // >25
			c2 = 0; //>7000
			c3 = 0 ; //<300
			c4 = 0 ; //< 10
			c5 = 0 ; //>800 
			c6 = 0; //>600
			c7 = 0; //; <600 
			c8 = 0 ;//<300
			c9 = 0 ; // ;<600 
			c10 =0 ; //<400
			
		}
		
		// Determine the total blob area and skin area in the image
		totalBlobSkin=0;
		totalBlobArea=0;
		
		// Added for new emgine 
		BlobAreaRatio = 0.0;	//JH
		BlobSkinRation = 0.0;	//JH
		
		for (i = 1; i <= numberBlobs; i++)
		{
			totalBlobSkin+=blob_info[i].skinTotal;
			totalBlobArea+=blob_info[i].area;
		}
		//---------------------------------------------------------------------------
		//- End of Extraction of features from blob to be use in Fast Filter
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
			
		BlobSkinRation = ((100*totalBlobSkin)/((boxTop-boxBottom)*(boxRight-boxLeft)));	//JH
		// Calculate the percentage of Blob over the whole Image 
		BlobAreaRatio = (100*(boxTop-boxBottom)*(boxRight-boxLeft))/hw;	//JH
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (8) - Rectangle skin object, high possible non body part
		//---------------------------------------------------------------------------
		
		
		// Obtain the ratio position of skin MER as compared to image dimension
		boxBottom = (100*boxBottom)/height;
		boxTop = (100*boxTop)/height;
		boxLeft = (100*boxLeft)/width;
		boxRight = (100*boxRight)/width;	
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (9) - Skin MER is at the side of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (10) - Compactness skin over image area, less than 5%
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------
		
		portraitflag=ObtainFaceInfoOptimised(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, nChannel  , nDepth);
		//---------------------------------------------------------------------------
		//- End of Determine portrait or shoulder portrait image
		//---------------------------------------------------------------------------
		// Determine number of faces
		
		numberFaces = FaceAttr.size();
		
#ifdef _JHDebug_showlog
		printf("\n f11 portraitflag=%d numberFaces=%d", portraitflag,numberFaces); //JH	
#endif	
		
		//---------------------------------------------------------------------------
		//- Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
		//---------------------------------------------------------------------------
		if(portraitflag==TRUE)
		{
#ifdef _JHDebug_showlog
			printf("\n f11 portraitflag==TRUE PV=0"); //JH	
#endif	
			pResults->pf=0;
			pResults->NetPornValue=0.0f;
			pResults->kof=200;
			pResults->RT=0;	//JH
			pResults->FT=11;	//JH
			if (portraitindicator) pResults->kosf=203;
			if (shoulderportraitindicator) pResults->kosf=206;
			if (manyfacesindicator) pResults->kosf=204;
			// end
			goto EndRemove;
		}
		/// Added By Homam 
		//--------------------------------------------------------------------------------
		// Checking the possibility to detect the face if we removed the added White Border to the image 
		//------------------------------------------------------------------------------
		else
		{
			
			FaceAttr.clear();
				portraitflag=ObtainFaceInfoOptimised1_V3_6(info,hbm2,height,width,portraitindicator,manyfacesindicator,shoulderportraitindicator,enEngine_Type, profileindicator, nChannel , nDepth, bpp);
			if(portraitflag==TRUE )
			{
				//log image as being non-porn due to face stuff
				pResults->pf=0;
				pResults->NetPornValue=0.0f;
				pResults->kof=200;
				pResults->RT=0;
				pResults->FT=111;
				if (portraitindicator) pResults->kosf=203;
				if (shoulderportraitindicator) pResults->kosf=206;
				if (manyfacesindicator) pResults->kosf=204;
				// end
				goto EndRemove;
				
			}
			
			
			numberFaces = FaceAttr.size();
			
#ifdef _JHDebug_showlog
			printf("\n f11 portraitflag=%d numberFaces=%d", portraitflag,numberFaces); //JH	
#endif	
			
			//---------------------------------------------------------------------------
			//- End of Fast Filter (11) - Portrait, Shoulder Portrait and ManyFaces filter
			//---------------------------------------------------------------------------
			
			//---------------------------------------------------------------------------
			//- Analyse the human strucutre below the face to determine the nude area and topology
			//---------------------------------------------------------------------------
			if(numberFaces>0)
			{
				hbm2= (BYTE * ) hbm2; 
				//	bool DoFiltering = FilterNoisyFaces(red , green , blue , height , width , hbm2);
				bool DoFiltering = FilterNoisyFaces(red , green , blue , height , width , hbm2, nChannel , nDepth);			
				
				if (FaceAttr.size()> 3)
				{
					pResults->pf=0;
					pResults->NetPornValue=0.0f;
					pResults->kof=200;
					pResults->kosf=253;
					pResults->RT = 0;
					pResults->FT =112;
					// end
#ifdef _JHDebug_showlog
					printf("\n f11-12 (FaceAttr.size()> 3) "); //JHP
					printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
					goto EndRemove;
				}
				// Super Lax Detection 
				//-----------------------------------------------------------------
				if (FaceAttr.size()> 0)
				{
					
					if (profileindicator == 1)
					{
						bool NoisyFaceColour = TRUE;
						NoisyFaceColour = NoisyfacesColour(red , green , blue , height , width);
#ifdef _JHDebug_showlog
						printf("\n f11 numberFaces>0 &NoisyFaceColour = %d PV=0 height=%d, width=%d", NoisyFaceColour,height , width); //JH	
#endif	
						if (NoisyFaceColour == FALSE)
						{
							bool Noisyface = TRUE;
							Noisyface = Noisyfaces(height , width);
							
							if (Noisyface == FALSE)
							{
								bool dominantcolourSL = FALSE;
								dominantcolourSL = AnalyseDominantColour(red , green , blue , height , width);
								//	if ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) && (Safe_Non == FALSE))
								if ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400))
								{
									
									pResults->NetPornValue=1.0f;
									pResults->pf=1;
									pResults->kof=200;
									pResults->kosf=254;
									pResults->RT = 1;
									pResults->FT =115;
#ifdef _JHDebug_showlog
									printf("\n f11-12 ((dominantcolourSL == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) && (Safe_Non == FALSE))");	//JHP
									printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
									goto EndRemove;
								}
							}	
						}	
					}
					
					if(enEngine_Type==FSIP_SLAX_ENGINE_APR_2006)
					{
						pResults->NetPornValue=0;
						pResults->pf=0.0f;
						pResults->kof=200;
						pResults->kosf=258;
						pResults->RT = 0;
						pResults->FT =1171;
						goto EndRemove;
						//}
					}
					
					// End Of Super Lax Detection Part 
					//-----------------------------------------------------------------
					bool NonPorn = FALSE;
					//	dominantcolour = AnalyseDominantColourDump(red , green , blue , height , width , hbm2 , NonPorn ,  fBodyskin , fFaceSkin);		
					dominantcolour = AnalyseDominantColourDump(red , green , blue , height , width , NonPorn ,  fBodyskin , fFaceSkin);	//JH- hbm2 redundent, not use, so remove
					
					if (NonPorn == TRUE )
					{
						pResults->pf=0;
						pResults->NetPornValue=0.0f;
						pResults->kof=200;
						pResults->kosf=253;
						pResults->RT = 0;
						pResults->FT =114;
						// end
						goto EndRemove;						
					}
					//	if (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400 && (Safe_Non == FALSE))
					if (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400)
					{
						
						pResults->NetPornValue=1.0f;
						pResults->pf=1;
						pResults->kof=200;
						pResults->kosf=254;
						pResults->RT = 1;
						pResults->FT =115;
#ifdef _JHDebug_showlog
						printf("\n f11-12 (dominantcolour == true && c1 < 25 && c2 < 7000 && c3>300 && c4> 10 && c5 < 800 && c6 < 600 && c9 > 700 && c10 > 400) "); //JHP	
						printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);
#endif
						goto EndRemove;
						
					}		
					
					AnalyseHumanStructureOptimised(blob_info,blob,numberBlobs,height,width,pResults, info, faceSkin);
					if (pResults->pf== 0)
					{
						
						pResults->kof=400;
						pResults->NetPornValue=0.0f;
						pResults->pf=0;
						pResults->kosf=254;
						pResults->RT = 0;
						pResults->FT = 116;
#ifdef _JHDebug_showlog
						printf("\n f11-12 (pResults->pf== 0) "); //JHP	
						printf("\n NetPornValue=%.2f  pf=%d  kof=%d  kosf=%d  RT=%d  FT=%d \n", pResults->NetPornValue, pResults->pf, pResults->kof, pResults->kosf, pResults->RT, pResults->FT);	
#endif
						goto EndRemove;
					}					
					
				}
			}
		}
		
		if(enEngine_Type==FSIP_LAX_ENGINE_APR_2006 || enEngine_Type==FSIP_SLAX_ENGINE_APR_2006)
		{
			pResults->NetPornValue=0;
			pResults->pf=0.0f;
			pResults->kof=200;
			pResults->kosf=258;
			pResults->RT = 0;
			pResults->FT =117;
			goto EndRemove;
			//}
		}
		
		
		if (innerEdges == 0 || outerEdges == 0 || totalBlobArea == 0) // To Avoid Crashing Later if The image has problem While processing 
		{
			
			pResults->NetPornValue=0;
			pResults->pf=0.0f;
			pResults->kof=200;
			pResults->kosf=258;
			pResults->RT = -1;
			pResults->FT =-1;
			goto EndRemove;
			
		}
		
		//---------------------------------------------------------------------------
		//- End of Analyse the human strucutre below the face to determine the nude area and topology
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the skin Ratio over the detected blob
		//---------------------------------------------------------------------------
		if (BlobSkinRation < 3 )
		{
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=209;
			pResults->RT=0;
			pResults->FT=9;
			goto EndRemove;
		}
		
		
		//---------------------------------------------------------------------------
		//- Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		DiamondSkinOptimal(blob,width,height,diamondSkin,skinInDiamond);
		//---------------------------------------------------------------------------
		//- End of Determine the total skin pixel and normalized value in the center diamond shape area in the image
		//---------------------------------------------------------------------------
		
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		if (diamondSkin>0.90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) 
		{
#ifdef _JHDebug_showlog
			printf("\n f12 diamondSkin>0.90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif	
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=212;
			pResults->RT = 1;
			pResults->FT =12;		
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (12) - Analyse the amount of skin in the center diamond
		//---------------------------------------------------------------------------
		
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		//	if (boxTop - boxBottom<40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if (boxTop - boxBottom<40 )	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f13 boxTop - boxBottom<40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=220;
			pResults->RT = 0;
			pResults->FT =13;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (13) - Minimum height of the MER skin
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------
		
		//	if (((100*outerEdges)/innerEdges)>25 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) 
		if (((100*outerEdges)/innerEdges)>25) 		//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f14 100*outerEdges)/innerEdges)>25 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=221;
			pResults->RT = 0;
			pResults->FT =14;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (14) - Inner skin blob is smoother as compared to boundary of skin blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------
		
		//	if ((100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006) // was 150
		if ((100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 ) // was 150	
		{
#ifdef _JHDebug_showlog
			printf("\n f15 100*outerEdgesSum)/outerEdges>7000 || (100*outerEdgesSum)/outerEdges<300 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=225;
			pResults->RT = 0;
			pResults->FT =15;
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (15) - Non porn images seems to be not that smooth or too smooth
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (16) - ratio of face skin as compared to the area of blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (16) - ratio of face skin as compared to the area of blob
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if (((100*totalBlobSkin)/totalBlobArea)>97 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f17 (100*totalBlobSkin)/totalBlobArea)>97 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif	
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=224;
			pResults->RT = 1;
			pResults->FT =17;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (17) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/hw > 40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f18 (100*totalBlobSkin)/hw > 40 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif				
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=228;			
			pResults->RT = 1;
			pResults->FT =18;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (18) - ratio of skin in the blob as compared to the image area
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		if ((100*totalBlobSkin)/totalBlobArea > 90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		{
#ifdef _JHDebug_showlog
			printf("\n f19 (100*totalBlobSkin)/totalBlobArea > 90 && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=1"); //JH	
#endif		
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=229;
			pResults->RT = 1;
			pResults->FT =19;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- Fast Filter (19) - ratio of skin area in the blob
		//---------------------------------------------------------------------------
		
		
		//- Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
		//	if (((10000*totalStrongEdges2)/hw < 10) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if (((10000*totalStrongEdges2)/hw < 10))	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f20 (10000*totalStrongEdges2)/hw < 10) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif		
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=230;
			pResults->RT = 0;
			pResults->FT =20;
			goto EndRemove;
		}
		
		//---------------------------------------------------------------------------
		//- End of Fast Filter (20) - for very smooth image
		//---------------------------------------------------------------------------
		
		
		
		//---------------------------------------------------------------------------
		//- Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		//	if (((100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if (((100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80))	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f21 (100*blob_horizontal90)/height>30 && (100*blob_horizontal80)/height<80) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=231;
			pResults->RT = 0;
			pResults->FT =21;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (21) - Shape of the blob as similar to human shape
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		//	if ((((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if ((((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600))	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f22 ((1000*numberWeakInnerEdges_2)/innerEdges)>800 || ((1000*numberWeakInnerEdges_5)/innerEdges)>600) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=232;
			pResults->RT = 0;
			pResults->FT =22;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (22) - Too many weak internal edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		//	if ((((1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if ((((1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300))	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n f23 (1000*numberWeakInnerEdges_0)/innerEdges)<600 || ((1000*numberWeakInnerEdges_1)/innerEdges)<300) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=233;
			pResults->RT = 0;
			pResults->FT =23;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (23) - Too few weak internal edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		//		if ((((1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006)
		if ((((1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400))	//JH - new engine V3.6	
		{
#ifdef _JHDebug_showlog
			printf("\n ((f24 1000*numberWeakOuterEdges_1)/outerEdges)<600 || ((1000*numberWeakOuterEdges_2)/outerEdges)<400) && enEngine_Type==FSIP_SAFEST_ENGINE_APR_2006 PV=0"); //JH	
#endif	
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=200;
			pResults->kosf=234;
			pResults->RT = 0;
			pResults->FT =24;
			goto EndRemove;
		}
		//---------------------------------------------------------------------------
		//- End of Fast Filter (24) - Too few weak external edges
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the number of brightness points
		//---------------------------------------------------------------------------
		CurvaturePoint (bw,blob, height, width, direction, directionIn, directionOut);
		for (h=0;h<height;h++)
			for (w=0;w<width;w++)
			{
				index = w+w+w;
				INT val = ((h/6)*width) + w/6;
				if (directionIn[val]>4) if (h%6==0 && w%6==0) total_points++;
				if (directionOut[val]>4) if (h%6==0 && w%6==0) total_points++;
			}
		pResults->numberOfCurvature = total_points;
		//---------------------------------------------------------------------------
		//- End of Determine the number of brightness points
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Determine the edges of the image
		//---------------------------------------------------------------------------
		CannyOperator(blob,bw,edge_total,height,width,highThreshold,lowThreshold,lengthThres);
		//---------------------------------------------------------------------------
		//- End of Determine the edges of the image
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		CalcMoments(edge_total, width, height, pResults->daMoments);
		//---------------------------------------------------------------------------
		//- End of Calculate Hu moments from the edge data
		//---------------------------------------------------------------------------
		
		//---------------------------------------------------------------------------
		//- Calculate the edge histogram 
		//---------------------------------------------------------------------------
		CannyEdgeHistrogram16x16Optimal(edge_total, width, height,pResults);
		//---------------------------------------------------------------------------
		//- End of Calculate the edge histogram 
		//---------------------------------------------------------------------------
		
		
		
#ifdef _SVMTRAIN
		{
			
			// Not normalized features 
			//---------------------------------------------------------------------------
			//- Compute 55 feature for porn classifier
			//---------------------------------------------------------------------------
			//Feature (1)
			f_numberBlobs = (float)numberBlobs; // was / 14.0
			fPornFeature[0]=f_numberBlobs;
			//Feature (2)
			/*f_numberFaces = ((float)numberFaces);
			 fPornFeature[1]=f_numberFaces;*/
			f_numberFaces = ((float)fBodyskin);
			fPornFeature[1]=f_numberFaces;
			//Feature (3)
			f_threshold = threshold;
			fPornFeature[2]=f_threshold;
			//Feature (4)
			f_diamondSkin = (diamondSkin);
			fPornFeature[3]=f_diamondSkin;
			//Feature (5)
			f_boxTop = (float) boxTop;
			fPornFeature[4]=f_boxTop;
			//Feature (6)
			f_boxBottom = ((float) (boxBottom));
			fPornFeature[5]=f_boxBottom;
			//Feature (7)
			f_boxHeight = (float) (boxTop-boxBottom);
			fPornFeature[6]=f_boxHeight;
			//Feature (8)
			edges1 = ((float)((100*outerEdges)/innerEdges));
			fPornFeature[7]=edges1;
			//Feature (9)
			edges2 = ((float)((100*outerEdgesSum)/outerEdges));
			
			fPornFeature[8]=edges2;
			//Feature (10)
			edges3 = ((float)((100*innerEdgesSum)/innerEdges));
			fPornFeature[9]=edges3;
			//Feature (11)
			skin1 = ((float)((100*totalBlobSkin)/hw));
			fPornFeature[10]=skin1;
			//Feature (12)
			skin2 = ((float)(((100*totalBlobSkin)/totalBlobArea)));
			fPornFeature[11]=skin2;
			//Feature (13)
			/*skin3 = ((float)(((100*faceSkin)/totalBlobArea)));
			 fPornFeature[12]=skin3;*/
			skin3 = fFaceSkin;
			fPornFeature[12]=skin3;
			//Feature (14)
			skin4 = ((float)(((100*skinInDiamond)/totalBlobArea)));
			fPornFeature[13]=skin4;
			//Feature (15)
			edgeScore = ((float)(((10000*totalStrongEdges2)/hw)));
			fPornFeature[14]=edgeScore;
			//Feature (16 - 22) - 7 Hu Moments
			moment = ((float)pResults->daMoments[0]);
			fPornFeature[15]=moment;
			moment = ((float)pResults->daMoments[1]);
			fPornFeature[16]=moment;
			moment = ((float)pResults->daMoments[2]);
			fPornFeature[17]=moment;
			moment = ((float)pResults->daMoments[3]);
			fPornFeature[18]=moment;
			moment = ((float)pResults->daMoments[4]);
			fPornFeature[19]=moment;
			moment = ((float)pResults->daMoments[5]);
			fPornFeature[20]=moment;
			moment = ((float)pResults->daMoments[6]);
			fPornFeature[21]=moment;
			//Feature (23) - curvature points
			curves = ((float)pResults->numberOfCurvature);
			fPornFeature[22]=curves;
			//Feature (24-55) - edge histogram
			for(i=23;i<55;i++)
			{
				fPornFeature[i]=pResults->dEdgeMap[i-23];
			}
			//MessageBox ( NULL , "Inside Safest Engine" , "Dumping File" , NULL);
			FILE *out;
			LPTSTR lpszFilename = _T("C:\\train\\SVM\\features_safest_NonNorm.txt");
			out = _tfopen(lpszFilename,_T("a"));
			if (out!=NULL)
			{ 
				for(i=0;i<55;i++)
				{
					_ftprintf(out,_T("%f "),fPornFeature[i]);
					
				}
				
				_ftprintf(out,_T("\n"));
				fclose(out);
			}
			if (out!=NULL)
			{
				fclose(out);
			}
		}
#endif	
		
		
		
		//---------------------------------------------------------------------------
		//- Compute 55 feature for porn classifier
		//---------------------------------------------------------------------------
		//Feature (1)
		f_numberBlobs = ((float)(numberBlobs-1))/5.0; // was / 14.0
		if (f_numberBlobs<0.0) f_numberBlobs=0.0;
		if (f_numberBlobs>1.0) f_numberBlobs=1.0;
		fPornFeature[0]=f_numberBlobs;
		//Feature (2) Body Skincolor Matching With Faces 
		f_numberFaces = ((float)fBodyskin);
		if (f_numberFaces<0.0) f_numberFaces=0.0;
		if (f_numberFaces>1.0) f_numberFaces=1.0;
		fPornFeature[1]=f_numberFaces;
		//Feature (3)
		f_threshold = (threshold-0.1)/0.7;
		if (f_threshold<0.0) f_threshold=0.0;
		if (f_threshold>1.0) f_threshold=1.0;
		fPornFeature[2]=f_threshold;
		//Feature (4)
		f_diamondSkin = (diamondSkin)/0.9;
		if (f_diamondSkin<0.0) f_diamondSkin=0.0;
		if (f_diamondSkin>1.0) f_diamondSkin=1.0;
		fPornFeature[3]=f_diamondSkin;
		//Feature (5)
		f_boxTop = ((float) (boxTop-60))/40.0;
		if (f_boxTop<0.0) f_boxTop=0.0;
		if (f_boxTop>1.0) f_boxTop=1.0;
		fPornFeature[4]=f_boxTop;
		//Feature (6)
		f_boxBottom = ((float) (boxBottom))/45.0;
		if (f_boxBottom<0.0) f_boxBottom=0.0;
		if (f_boxBottom>1.0) f_boxBottom=1.0;
		fPornFeature[5]=f_boxBottom;
		//Feature (7)
		f_boxHeight = ((float) (boxTop-boxBottom-40))/60.0;
		if (f_boxHeight<0.0) f_boxHeight=0.0;
		if (f_boxHeight>1.0) f_boxHeight=1.0;
		fPornFeature[6]=f_boxHeight;
		//Feature (8)
		edges1 = ((float)(((100*outerEdges)/innerEdges)-1))/21.0;
		if (edges1<0.0) edges1=0.0;
		if (edges1>1.0) edges1=1.0;
		fPornFeature[7]=edges1;
		//Feature (9)
		edges2 = ((float)(((100*outerEdgesSum)/outerEdges)-304))/6500.0;
		if (edges2<0.0) edges2=0.0;
		if (edges2>1.0) edges2=1.0;
		fPornFeature[8]=edges2;
		//Feature (10)
		edges3 = ((float)(((100*innerEdgesSum)/innerEdges)-128))/1200.0;
		if (edges3<0.0) edges3=0.0;
		if (edges3>1.0) edges3=1.0;
		fPornFeature[9]=edges3;
		//Feature (11)
		skin1 = ((float)(((100*totalBlobSkin)/hw)-5))/35.0;
		if (skin1<0.0) skin1=0.0;
		if (skin1>1.0) skin1=1.0;
		fPornFeature[10]=skin1;
		//Feature (12)
		skin2 = ((float)(((100*totalBlobSkin)/totalBlobArea)-21))/69.0;
		if (skin2<0.0) skin2=0.0;
		if (skin2>1.0) skin2=1.0;
		fPornFeature[11]=skin2;
		//Feature (13)
		skin3 = (float)(fFaceSkin)/20.0;
		if (skin3<0.0) skin3=0.0;
		if (skin3>1.0) skin3=1.0;
		fPornFeature[12]=skin3;
		//Feature (14)
		skin4 = ((float)(((100*skinInDiamond)/totalBlobArea)))/71.0;
		if (skin4<0.0) skin4=0.0;
		if (skin4>1.0) skin4=1.0;
		fPornFeature[13]=skin4;
		//Feature (15)
		edgeScore = ((float)(((10000*totalStrongEdges2)/hw)))/2500.0;
		if (edgeScore<0.0) edgeScore=0.0;
		if (edgeScore>1.0) edgeScore=1.0;
		fPornFeature[14]=edgeScore;
		//Feature (16 - 22) - 7 Hu Moments
		moment = ((float)pResults->daMoments[0]);
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[15]=moment;
		moment = ((float)pResults->daMoments[1])/0.67;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[16]=moment;
		moment = ((float)pResults->daMoments[2])/0.7;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[17]=moment;
		moment = ((float)pResults->daMoments[3])/0.33;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[18]=moment;
		moment = ((float)pResults->daMoments[4])/0.81;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[19]=moment;
		moment = ((float)pResults->daMoments[5])/0.32;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[20]=moment;
		moment = ((float)pResults->daMoments[6])/0.82;
		if (moment<0.0) moment=0.0;
		if (moment>1.0) moment=1.0;
		fPornFeature[21]=moment;
		//Feature (23) - curvature points
		curves = ((float)pResults->numberOfCurvature)/5.0;
		if (curves<0.0) curves=0.0;
		if (curves>1.0) curves=1.0;
		fPornFeature[22]=curves;
		//Feature (24-55) - edge histogram
		for(i=23;i<55;i++)
		{
			fPornFeature[i]=pResults->dEdgeMap[i-23];
		}
		
		
		
		//-------------------------------------------------------------
		//MessageBox ( NULL , "Inside Safest Engine" , "Before Dumping File" , NULL);
		
#ifdef _SVMTRAIN
		{
			//MessageBox ( NULL , "Inside Safest Engine" , "Dumping File" , NULL);
			FILE *out;
			LPTSTR lpszFilename = _T("C:\\train\\SVM\\features_safest.txt");
			out = _tfopen(lpszFilename,_T("a"));
			if (out!=NULL)
			{ 
				for(i=0;i<55;i++)
				{
					_ftprintf(out,_T("%f "),fPornFeature[i]);
					
				}
				
				_ftprintf(out,_T("\n"));
				fclose(out);
			}
			if (out!=NULL)
			{
				fclose(out);
			}
		}
#endif
		
		//---------------------------------------------------------------------------
		//- End of Compute 55 feature for porn classifier
		//---------------------------------------------------------------------------
		
		
		//---------------------------------------------------------------------------
		//- SVM porn classifier
		//---------------------------------------------------------------------------
		
		
		// For Unicode Compiling 
		char safestPornSVMFilename[MAX_PATH];
		
		for (int i=0 ; i < MAX_PATH ; i++)
		{
			safestPornSVMFilename[i] = NULL; 
		}
		
		for (int i=0 ; i < strlen(m_szsafestPornSVMFilename) ; i++)		////JHDVS-V3.6
		{
			safestPornSVMFilename[i] = m_szsafestPornSVMFilename[i]; 
		}
		
		pornsvm->load (safestPornSVMFilename); //update opencv4
		////////////////////////////////////////////
		
		//pornsvm.load (m_szsafestPornSVMFilename);
		//cvInitMatHeader (&m_porn, 1, 55, CV_32FC1, NULL);
        m_porn = cv::Mat(1, 55, CV_32FC1, fPornFeature);
		//cvSetData (&m_porn, fPornFeature, sizeof (float) * 55);
        
		porn_ret = pornsvm->predict (m_porn); //opencv4 update
		
		if (porn_ret ==1)
		{
#ifdef _JHDebug_showlog
			printf("\n ((f25 SVM porn classifier :: porn_ret ==1"); //JH	
#endif	
			pResults->NetPornValue=1.0f;
			pResults->pf=1;
			pResults->kof=200;
			pResults->kosf=661;
			pResults->RT = 1;
			pResults->FT =25;
			goto EndRemove;
		}		// Pron Vector Machine	
		else
		{ 
#ifdef _JHDebug_showlog
			printf("\n ((f25 SVM porn classifier :: nonporn_ret == 1 PV=0"); //JH	
#endif				
			pResults->NetPornValue=0.0f;
			pResults->pf=0;
			pResults->kof=600;
			pResults->kosf=660;
			pResults->RT = 0;
			pResults->FT =25;
			goto EndRemove;
		}
		
	
        pornsvm->clear();
        nonsvm->clear();
		nonporn_ret = -1.0;
		porn_ret = -1.0;
		
		
		
		//---------------------------------------------------------------------------
		//- End of Determine the porn value from the classifier
		//---------------------------------------------------------------------------
		
	EndRemove:
		//Release memory
		delete[] red;			red=NULL;
		delete[] green;			green=NULL;
		delete[] blue;			blue=NULL;
		delete[] bw;			bw=NULL;
		delete[] info;			info=NULL;
		delete[] blob;			blob=NULL;
		if (object_map!=NULL) delete[] object_map;	object_map=NULL;
		if (blob_info!=NULL) delete[] blob_info;		blob_info=NULL;
		FaceAttr.clear();
		delete[] edge_total;	edge_total=NULL;
		delete[] direction;		direction=NULL;
		delete[] directionIn;	directionIn=NULL;
		delete[] directionOut;	directionOut=NULL;
		
	}
	
	//cvReleaseImage(&imgskin);
	
	free(hbm2);
	hbm2 = NULL;
	
	return TRUE;
}



bool ImageProcessing::NewBlobScan(uchar* data, INT* info, INT nWidth ,INT nHeight ,INT nDepth ,INT nChannel, INT widthstep)
{
	//initiate a new hbitmap for conversion purpose

	uchar* hbm2 = NULL; 			
	
	int bpp;
	bpp = (nDepth & 255) * nChannel ;
	

	if (ChangeTo24Bit(hbm2, data , bpp))
	{
		
		bool bRet;
		DOUBLE dPercentageQuickSkin;
	
		bRet = QuickBlobScan(&hbm2, info, 0, 0, nWidth, nHeight, bpp);	
	
		IplImage *test;

		LoadHBITMAP(hbm2 , &test, nWidth, nHeight, bpp); 

		cvReleaseImage(&test);
		
		delete[] hbm2;
		hbm2 = NULL;
		return true;
	}
	return false;
}



//-  Function Name : Changeto24Bit
        
//-  Description   : Convert any hbitmap image to 24 bit hbitmap image
//-  Parameter     : hbm2 - 24 bits hbitmap image
//hbmp - any bits hbitmap image
//-  Return Value  : TRUE if sucess
//-------------------------------------------------------------------------*/
bool ImageProcessing::ChangeTo24Bit(uchar* &hbm2,uchar* hbmp , int bpp)
{
	// Obtain bitmap information
	if (bpp == 32)
	{
		if (CMBitmapProcessing::Convert32to24bit(&hbm2 , hbmp) == FALSE)
		{
			// failed to convert
			return FALSE;
		}	
	}
	// Make another copy of 24 bits hbitmap
	else if (bpp == 24)
	{
		// no need to convert
		if (CMBitmapProcessing::CloneBitmap(&hbm2, hbmp) == TRUE)
		{
			// successfully cloned
		}
		else
		{
			// failed to clone
			return FALSE;
		}
	}
	// Converting lesser than 24 bits hbitmap to 24 bits hbitmap
	else if (bpp < 24 )
	{
		
		return FALSE;
		turn FALSE;
		}*/
	}
	else
	{
		// failed to convert
		return FALSE;
	}
	
	return TRUE;
	
	
}



FLOAT ImageProcessing::GetPornValueFromImage(std::vector<FLOAT>*vRes,std::vector<FLOAT> *vArea,std::vector<FLOAT> *vRatio, int &best)
{
	FLOAT iResults = 0;
	
	INT total_subimage=0;
	FLOAT FPornValue=0.0f;
	FLOAT TotalArea=0.0f;
	INT nofeature;
	if(vRes->size()==1)
	{
		
		if ((*vRes)[0]>0.25f && (*vRatio)[0]<=3) //previously is 0.0f
			FPornValue=(*vRes)[0];
		else
			FPornValue=0;
		best = 0;
	}
	
	if(vRes->size()==2)
	{
		for(nofeature=0;nofeature<vRes->size();nofeature++)
		{
			if ((*vRes)[nofeature]>0.25f && (*vRatio)[nofeature]<=3) //previously is 0.0f
			{
				total_subimage++;
				TotalArea+=(*vArea)[nofeature];
				FPornValue+=(*vArea)[nofeature]*(*vRes)[nofeature];
			}
		}
		if(TotalArea>0) FPornValue/=TotalArea;
		
		best = 0;
		if ((*vRes)[1]>(*vRes)[0]) best=1;
		
	}
	
	if(vRes->size()==3)
	{
		for(nofeature=0;nofeature<vRes->size();nofeature++)
		{
			if ((*vRes)[nofeature]>0.25f && (*vRatio)[nofeature]<=3) //previously is 0.0f
			{
				total_subimage++;
				TotalArea+=(*vArea)[nofeature];
				FPornValue+=(*vArea)[nofeature]*(*vRes)[nofeature];
			}
		}
		if(TotalArea>0) FPornValue/=TotalArea;
		
		best = 0;
		if ((*vRes)[1]>=(*vRes)[0] && (*vRes)[1]>=(*vRes)[2]) best=1;
		if ((*vRes)[2]>=(*vRes)[0] && (*vRes)[2]>=(*vRes)[1]) best=2;
	}
	
	//DECISION MAKING PROCESS
	if (total_subimage==0)   iResults=FPornValue;
	if (total_subimage==1)
	{
		if (TotalArea<=0.06) //previously is 0.05f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	
	if (total_subimage==2)
	{
		if (TotalArea<=0.12) //previously is 0.1f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	
	if (total_subimage==3)
	{
		if (TotalArea<=0.18) //previously is 0.15f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	return iResults;
}

//added to compare mats  27feb2019
bool matIsEqual(const cv::Mat mat1, const cv::Mat mat2){
    // treat two empty mat as identical as well
    if (mat1.empty() && mat2.empty()) {
        return true;
    }
    // if dimensionality of two mat is not identical, these two mat is not identical
    if (mat1.cols != mat2.cols || mat1.rows != mat2.rows || mat1.dims != mat2.dims) {
        return false;
    }
    cv::Mat diff;
    cv::compare(mat1, mat2, diff, cv::CMP_NE);
    int nz = cv::countNonZero(diff);
    return nz==0;
}

bool ImageProcessing::CaptureWindowProcess(/*IplImage* */ Mat hbmp,
											 std::vector<struct SUB_IMAGE> &pSub,
											 std::vector<WQ_IP_RESULTS> &pFeature,
											 EN_FSIP_ENGINE_TYPE enEngine_Type,
											 EN_FSIP_SCAN_TYPE enScan_Type )
{
	
	if(pSub.size()>0)	pSub.clear();
	if(pFeature.size()>0) pFeature.clear();

    Mat hbm0;

    INT BitsPixel = (hbmp.depth()*hbmp.channels());
	
		
	if ( BitsPixel  < 24 )
	{
		if (CMBitmapProcessing::ConvertTo32bitCV(hbm0, hbmp) == false)
		{
			return false;
		}
	}
	

    Mat hbm1;
	
	if ( BitsPixel  < 32 )		
	{
		if (CMBitmapProcessing::Convert32to24bitCV(hbm1, hbm0) == false)
		{
            
			if ( matIsEqual(hbm0, hbmp))
			{
			
				hbm0 = NULL;
			}
			return false; 
		}
	}
	
	if ( matIsEqual(hbm0, hbmp))
	{
		
		hbm0 = NULL;
	}
	
	
	INT height=hbm1->height;*/
    //CHANGES FOR UPDATING OPENCV4
    BYTE *image=(BYTE*) hbm1.data;
    INT width=hbm1.cols;
    INT height=hbm1.rows;
    
    
    
	INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
	
	if (width < 150 || height < 150)  
	{
		struct SUB_IMAGE subResult;
		subResult.imgX=0;
		subResult.imgY=0;
		subResult.imgWidth=width;
		subResult.imgHeight=height;
		subResult.imgRatioArea=1.0f;
		subResult.ratio=(FLOAT)width/height;
		pSub.push_back(subResult);
		
		WQ_IP_RESULTS pResults = {0};
		pResults.NetPornValue = 0.0f;
		pFeature.push_back(pResults);
		
		if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
		{
			
			hbm1 = NULL;
		}
		return true;
	}
	
	if(image)
	{
		INT sub_image=0;
		
		sub_image=WindowSegment(image, width, height, pSub);
		
		if(sub_image)
		{
			//Process each sub image
			for(INT cnt=0;cnt<pSub.size();cnt++)
			{
				INT x = pSub[cnt].imgX;
				INT y = pSub[cnt].imgY;
				INT width2 = pSub[cnt].imgWidth;
				INT height2 = pSub[cnt].imgHeight;
				
				INT topY = (height-y-height2);      
				if (topY<0) topY = - topY;
				
				uchar * hbmSub = NULL;
			
                uchar * imagehbm1  = (uchar*) hbm1.data;
                int bpp=  (hbm1.depth()*hbm1.channels());
                
				INT scalewidth2=0;
				INT scaleheight2=0;
				
				if(width2 <= 300 && height2 <= 300)
				{
			
					if (CMBitmapProcessing::ResizeBitmapSS(&hbmSub, imagehbm1, hbm1.cols, hbm1.rows,x,topY,width2,height2,width2,height2, bpp) == FALSE) //JH optimise
					{
						if  ( matIsEqual(hbm0, hbmp))
						{
							
							hbm1 = NULL;
						}
						return false;
					}
					scalewidth2=width2;
					scaleheight2=height2;
				}
				else
				{
					DOUBLE ratio2=0.0f;
					ratio2=(DOUBLE)height2/(DOUBLE)width2;
					
					// Made Changes 5th Feb 
					if(ratio2>1)
					{
						scaleheight2=300;
						scalewidth2=INT(300/ratio2);
					}
					else
					{
						scalewidth2=300;
						scaleheight2=INT(300*ratio2);
					}
			
					if (CMBitmapProcessing::ResizeBitmapSS(&hbmSub, imagehbm1, hbm1.cols, hbm1.rows, x,topY,width2,height2,scalewidth2,scaleheight2, bpp) == FALSE)
					{
						if ( matIsEqual(hbm0, hbmp))//
						{
							
							hbm1 = NULL;
						}
						return false;
					}
					width2=scalewidth2;
					height2=scaleheight2;
				}
				
				WQ_IP_RESULTS pResults = {0};
				
				
				BYTE *subImage=(BYTE*) hbmSub;
				int hbmSubWidthBytes=(width2 * 3 % 4) ? ((((width2 * 3) >> 2) + 1) << 2) : width2 * 3;
				int  length =  height2*hbmSubWidthBytes;

				bool bProcess = FALSE;
			
                cv::Mat cvhbmSub =	cv::Mat(scalewidth2,scaleheight2,hbm1.depth(), hbm1.channels() );
                cvhbmSub.data= (uchar*)hbmSub;

                
                
				
				if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006|| enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) {
					bProcess = MixedAnalysisProcess(cvhbmSub,&pResults,enEngine_Type,enScan_Type);
				} else {
					
					bProcess = MixedAnalysisProcess(cvhbmSub,&pResults,enEngine_Type,enScan_Type); // - new engine
				}
				
				if (bProcess == TRUE)
				{
		
					{
						pobjLogger->Add(s_szAnalysisResultFormat, 
										_TEXT("CAPTUREWINDOWPROCESS"), 
										pResults.FT,
										pResults.RT);
					}
					
#ifdef _OUTPUT_INTEGRATION_VALUES
					
					{
						_TCHAR buffTemp[256] = {0};
						GetAnalysisResultText(buffTemp, 256, _TEXT("CAPTUREWINDOWPROCESS"), &pResults);
						OutputDebugString(buffTemp);
						
						
					//	 MessageBox(NULL,buffTemp,"",0);
						 
					}
					
#endif
					*/
					pFeature.push_back(pResults);
					
				}
				else
				{
					pSub.erase(pSub.begin()+cnt);
				}
				
				free(hbmSub);
				hbmSub = NULL;
				
				/////////////////////////////////////////////////////////////////////////////
			} //end of each subimage processing
			
		} //end of if(subimage)
		else
		{
			// if no subimage found
			WQ_IP_RESULTS pResults = {0};
			
			struct SUB_IMAGE subResult;
			
            //change opencv4
            BYTE *subImage=(BYTE*) hbm1.data;
		
            int hbm1WidthBytes=(hbm1.cols * 3 % 4) ? ((((hbm1.rows * 3) >> 2) + 1) << 2) : hbm1.cols * 3;
			
            int  length =  hbm1.rows*hbm1WidthBytes;
			MD5SS md5(subImage,length); // call a limited version of the file hash that only looks at max the first 1Mb
			md5.hex_digest(subResult.hash);
			
			//JH select engine base on engine type
			bool bProcess = FALSE;
			if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006|| enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) {
				bProcess = MixedAnalysisProcess(hbm1,&pResults,enEngine_Type,enScan_Type);	
			
				bProcess = MixedAnalysisProcess(hbm1,&pResults,enEngine_Type,enScan_Type);	// - new engine
			}
			
			if (bProcess == TRUE)
			{
				subResult.imgX=0;
				subResult.imgY=0;
				subResult.imgWidth=width;
				subResult.imgHeight=height;
				subResult.imgRatioArea=1.0f;
				subResult.ratio=(FLOAT)width/height;
				pSub.push_back(subResult);
				pFeature.push_back(pResults);
			}
			else
			{
				if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
				{
					//cvReleaseImage(&hbm1);
				//	DeleteObject(hbm1);
					hbm1 = NULL;
				}
				return FALSE;
			}
		}
	} //end of if(image)
	else
	{
		if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
		{
			//cvReleaseImage(&hbm1);
		//	DeleteObject(hbm1);
			hbm1 = NULL;
		}
		return FALSE;
	}
	
	// if the subimage found successfully
	if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
	{
		//cvReleaseImage(&hbm1);
	//	DeleteObject(hbm1);
		hbm1 = NULL;
	}
	

	return TRUE;
} 



INT ImageProcessing::WindowSegment(BYTE *image,INT width,INT height,std::vector<struct SUB_IMAGE> &pSub)
{
	if(image)
	{
		INT hw=width*height;
		INT widthbytes=(width * 3 % 4) ? ((((width * 3) >> 2) + 1) << 2) : width * 3;
		
		//////////////////////////////////////////////////////////////////////
		//              Take 100 points              //
		//////////////////////////////////////////////////////////////////////
		INT h,w;
		INT i,j,k;
		
		INT xLocation[10];
		INT yLocation[10];
		
		for (i=0;i<10;i++)
		{
			xLocation[i] = (width*(i+1))/11;
			yLocation[i] = (height*(i+1))/11;
		}
		
		// clear the vector
		std::vector<windowItem> windowVector; // holds details of images that have been checked in this document
		windowVector.clear();
		
		// push windows with valid starting points onto vector
		for (i=0;i<10;i++)
		{
			for (j=0;j<10;j++)
			{
				INT total11=0;
				INT pixel = *(image+yLocation[j]*widthbytes + xLocation[i]*3);
				if (pixel == *(image+((yLocation[j]+2)*widthbytes) + (xLocation[i]*3))) total11++;
				if (pixel == *(image+((yLocation[j]-2)*widthbytes) + (xLocation[i]*3))) total11++;
				if (pixel == *(image+(yLocation[j]*widthbytes) + ((xLocation[i]-2)*3))) total11++;
				if (pixel == *(image+(yLocation[j]*widthbytes) + ((xLocation[i]+2)*3))) total11++;
				if (total11<2) //not surrounded by colour pixel or only surrounded by one colour pixel
				{
					windowItem thisWindow;
					thisWindow.xPos = xLocation[i];
					thisWindow.yPos = yLocation[j];
					windowVector.push_back(thisWindow);
				}
			}
		}
		
		// find the edges of the windows
		for (i=0;i<windowVector.size();i++) //maximum 100 possible windows
		{
			INT xPos = windowVector.at(i).xPos;
			INT yPos = windowVector.at(i).yPos;
			INT RightEdge=-1;
			INT LeftEdge=-1;
			INT TopEdge=-1;
			INT BottomEdge=-1;
			
			for (j=0;j<i;j++)
			{
				INT oldXPos = windowVector.at(j).xPos;
				INT oldYPos = windowVector.at(j).yPos;
				INT oldTopEdge = windowVector.at(j).TopEdge;
				INT oldRightEdge = windowVector.at(j).RightEdge;
				
				if (oldXPos==xPos && oldTopEdge>yPos) TopEdge=oldTopEdge;
				if (oldYPos==yPos && oldRightEdge>xPos) RightEdge=oldRightEdge;
			}
			
			// RIGHT EDGE
			if (RightEdge==-1)
			{
				w = xPos;
				h = yPos;
				INT width_Minus_3 = width-3;
				INT h_Times_widthbytes = h*widthbytes;
				for (;w<width_Minus_3;w++)
				{
					// row pixels from the valid point until the last three pixel at the same row
					INT pixel = *(image+h_Times_widthbytes + (w*3)); 
					if (pixel==*(image+h_Times_widthbytes + ((w+1)*3)) && pixel==*(image+h_Times_widthbytes + ((w+2)*3))) 
					{
						INT pixel2 = *(image+h_Times_widthbytes + (w*3) + 1);
						if (pixel2==*(image+h_Times_widthbytes + ((w+1)*3) + 1) && pixel2==*(image+h_Times_widthbytes + ((w+2)*3) + 1)) 
						{
							// check to see if edge
							// edge is defined as the stuff above and below the point also being most common colour, whilst one pixel to the left is not
							bool isEdge = TRUE;
							INT edge_strength1 = *(image+h*widthbytes + ((w-1)*3)) - pixel;
							INT edge_strength2 = *(image+h*widthbytes + ((w-1)*3) + 1) - pixel2;
							if (!(edge_strength1>10 || edge_strength1<-10 || edge_strength2>10 || edge_strength2<-10)) isEdge=FALSE; 
							if (isEdge)
							{
								for (k=-5;k<=5;k++)
								{
									if (*(image+(h+k)*widthbytes + w*3)!=pixel || *(image+(h+k)*widthbytes + (w-1)*3)==pixel) isEdge=FALSE;
									if (*(image+(h+k)*widthbytes + w*3 + 1)!=pixel2 || *(image+(h+k)*widthbytes + (w-1)*3 + 1)==pixel2) isEdge=FALSE;
									if (!isEdge) k=5;
								}
							}
							if (isEdge)
							{
								RightEdge=w;
								w=width+10;
							}
						}
					}
				}
				if (w<width+10) RightEdge=width-1; //if non of the good RightEdge found
			}
			
			
			// LEFT EDGE
			w = xPos;
			h = yPos;
			INT h_Times_widthbytes = h*widthbytes;
			for (;w>=3;w--)
			{
				INT pixel = *(image+h_Times_widthbytes + w*3);
				if (pixel==*(image+h_Times_widthbytes + (w-1)*3) && pixel==*(image+h_Times_widthbytes + (w-2)*3)) 
				{
					INT pixel2 = *(image+h*widthbytes + w*3 + 1);
					if (pixel2==*(image+h_Times_widthbytes + (w-1)*3 + 1) && pixel2==*(image+h_Times_widthbytes + (w-2)*3 + 1)) 
					{
						// check to see if edge
						// edge is defined as the stuff above and below the point also being most common colour, whilst one pixel to the left is not
						bool isEdge = TRUE;
						INT edge_strength1 = *(image+h*widthbytes + (w+1)*3) - pixel;
						INT edge_strength2 = *(image+h*widthbytes + (w+1)*3 + 1) - pixel2;
						if (!(edge_strength1>10 || edge_strength1<-10 || edge_strength2>10 || edge_strength2<-10)) isEdge=FALSE; 
						if (isEdge)
						{
							for (k=-5;k<=5;k++)
							{
								if (*(image+(h+k)*widthbytes + w*3)!=pixel || *(image+(h+k)*widthbytes + (w+1)*3)==pixel) isEdge=FALSE;
								if (*(image+(h+k)*widthbytes + w*3 + 1)!=pixel2 || *(image+(h+k)*widthbytes + (w+1)*3 + 1)==pixel2) isEdge=FALSE;
								if (!isEdge) k=5;
							}
						}
						if (isEdge)
						{
							LeftEdge=w;
							w=-10;
						}
					}
				}
			}
			if (w>-10) LeftEdge=0;
			
			
			// TOP EDGE
			if (TopEdge==-1)
			{
				w = xPos;
				h = yPos;
				INT height_Minus_3 = height-3;
				INT w3 = w*3;
				for (;h<height_Minus_3;h++)
				{
					INT pixel = *(image+h*widthbytes + w3);
					if (pixel==*(image+(h+1)*widthbytes + w3) && pixel==*(image+(h+2)*widthbytes + w3)) 
					{
						INT pixel2 = *(image+h*widthbytes + w3 + 1);
						if (pixel2==*(image+(h+1)*widthbytes + w3 + 1) && pixel2==*(image+(h+2)*widthbytes + w3 + 1)) 
						{
							// check to see if edge
							// edge is defined as the stuff above and below the point also being most common colour, whilst one pixel to the left is not
							bool isEdge = TRUE;
							INT edge_strength1 = *(image+(h-1)*widthbytes + w3) - pixel;
							INT edge_strength2 = *(image+(h-1)*widthbytes + w3 + 1) - pixel2;
							if (!(edge_strength1>10 || edge_strength1<-10 || edge_strength2>10 || edge_strength2<-10)) isEdge=FALSE; 
							if (isEdge)
							{
								for (k=-5;k<=5;k++)
								{
									if (*(image+h*widthbytes + (w+k)*3)!=pixel || *(image+(h-1)*widthbytes + (w+k)*3)==pixel) isEdge=FALSE;
									if (*(image+h*widthbytes + (w+k)*3 + 1)!=pixel2 || *(image+(h-1)*widthbytes + (w+k)*3 + 1)==pixel2) isEdge=FALSE;
									if (!isEdge) k=5;
								}
							}
							if (isEdge)
							{
								TopEdge=h;
								//for (k=-5;k<5;k++) edge[h][w+k]=1;
								h=height+10;
							}
						}
					}
				}
				if (h<height+10) TopEdge=height-1;
			}
			
			// BOTTOM EDGE
			w = xPos;
			h = yPos;
			INT height_Minus_3 = height-3;
			INT w3 = w*3;
			for (;h>=3;h--)
			{
				INT pixel = *(image+h*widthbytes + w3);
				if (pixel==*(image+(h-1)*widthbytes + w3) && pixel==*(image+(h-2)*widthbytes + w3)) 
				{
					INT pixel2 = *(image+h*widthbytes + w3 + 1);
					if (pixel2==*(image+(h-1)*widthbytes + w3 + 1) && pixel2==*(image+(h-2)*widthbytes + w3 + 1)) 
					{
						// check to see if edge
						// edge is defined as the stuff above and below the point also being most common colour, whilst one pixel to the left is not
						bool isEdge = TRUE;
						INT edge_strength1 = *(image+(h+1)*widthbytes + w3) - pixel;
						INT edge_strength2 = *(image+(h+1)*widthbytes + w3 + 1) - pixel2;
						if (!(edge_strength1>10 || edge_strength1<-10 || edge_strength2>10 || edge_strength2<-10)) isEdge=FALSE; 
						if (isEdge)
						{
							for (k=-5;k<=5;k++)
							{
								if (*(image+h*widthbytes + (w+k)*3)!=pixel || *(image+(h+1)*widthbytes + (w+k)*3)==pixel) isEdge=FALSE;
								if (*(image+h*widthbytes + (w+k)*3 + 1)!=pixel2 || *(image+(h+1)*widthbytes + (w+k)*3 + 1)==pixel2) isEdge=FALSE;
								if (!isEdge) k=5;
							}
						}
						if (isEdge)
						{
							BottomEdge=h;
							//for (k=-5;k<5;k++) edge[h][w+k]=1;
							h=-10;
						}
					}
				}
			}
			if (h>-10) BottomEdge=0;
			
			// Get rid of windows with unlikely dimensions (4:1 or greater)
			if (((TopEdge-BottomEdge)/(RightEdge-LeftEdge)>3) || ((RightEdge-LeftEdge)/(TopEdge-BottomEdge)>3)) 
			{
				windowVector.erase(windowVector.begin()+i);
				i--;
			}
			else
			{
				windowVector.at(i).RightEdge=RightEdge;
				windowVector.at(i).LeftEdge=LeftEdge;
				windowVector.at(i).TopEdge=TopEdge;
				windowVector.at(i).BottomEdge=BottomEdge;
				windowVector.at(i).NumberWindows=1;
			}
		}
		
		// Now do a window count and collapse repeats
		for (i=0;i<windowVector.size();i++)
		{
			INT thisTopEdge = windowVector.at(i).TopEdge;
			INT thisBottomEdge = windowVector.at(i).BottomEdge;
			INT thisLeftEdge = windowVector.at(i).LeftEdge;
			INT thisRightEdge = windowVector.at(i).RightEdge;
			
			for (j=0;j<i;j++)
			{
				if (thisTopEdge == windowVector.at(j).TopEdge && thisBottomEdge == windowVector.at(j).BottomEdge && thisLeftEdge == windowVector.at(j).LeftEdge && thisRightEdge == windowVector.at(j).RightEdge)
				{
					windowVector.erase(windowVector.begin()+i);
					i--;
					windowVector.at(j).NumberWindows++;
					j=i;
				}
			}
		}
		
		// collapse the least popular of overlapping windows
		for (i=0;i<windowVector.size();i++)
		{
			INT thisTopEdge = windowVector.at(i).TopEdge;
			INT thisBottomEdge = windowVector.at(i).BottomEdge;
			INT thisLeftEdge = windowVector.at(i).LeftEdge;
			INT thisRightEdge = windowVector.at(i).RightEdge;
			
			for (j=0;j<i;j++)
			{
				INT otherTopEdge = windowVector.at(j).TopEdge;
				INT otherBottomEdge = windowVector.at(j).BottomEdge;
				INT otherLeftEdge = windowVector.at(j).LeftEdge;
				INT otherRightEdge = windowVector.at(j).RightEdge;
				if (
					
					(
					 ((thisTopEdge>=otherTopEdge && otherTopEdge>=thisBottomEdge) ||
					  (thisTopEdge>=otherBottomEdge && otherBottomEdge>=thisBottomEdge)) &&
					 
					 ((thisRightEdge>=otherRightEdge && otherRightEdge>=thisLeftEdge) ||
					  (thisRightEdge>=otherLeftEdge && otherLeftEdge>=thisLeftEdge))
					 ) 
					
					||
					
					((otherTopEdge>=thisTopEdge && thisTopEdge>=otherBottomEdge) ||
					 (otherTopEdge>=thisBottomEdge && thisBottomEdge>=otherBottomEdge)) &&
					
					((otherRightEdge>=thisRightEdge && thisRightEdge>=otherLeftEdge) ||
					 (otherRightEdge>=thisLeftEdge && thisLeftEdge>=otherLeftEdge))
					) 
					
				{
					bool eraseI = FALSE;
					if (windowVector.at(i).NumberWindows<windowVector.at(j).NumberWindows) eraseI=TRUE;
					if (windowVector.at(i).NumberWindows==windowVector.at(j).NumberWindows)
					{
						if ((thisTopEdge-thisBottomEdge)*(thisRightEdge-thisLeftEdge)>(otherTopEdge-otherBottomEdge)*(otherRightEdge-otherLeftEdge)) eraseI=TRUE;
					}
					
					if (eraseI)
					{
						windowVector.erase(windowVector.begin()+i);
						i--;
						j=i;
					}
					else
					{
						windowVector.erase(windowVector.begin()+j);
						i--;
						j--;
					}
				}
			}
		}
		
		INT topWindow1=0;
		INT topWindow1i=-1;
		INT topWindow2=0;
		INT topWindow2i=-1;
		INT topWindow3=0;
		INT topWindow3i=-1;
		
		for (i=0;i<windowVector.size();i++)
		{
			INT numberWindows = windowVector.at(i).NumberWindows;
			if (numberWindows>topWindow1)
			{
				topWindow3=topWindow2;
				topWindow3i=topWindow2i;
				topWindow2=topWindow1;
				topWindow2i=topWindow1i;
				topWindow1=numberWindows;
				topWindow1i=i;
			}
			else if (numberWindows>topWindow2)
			{
				topWindow3=topWindow2;
				topWindow3i=topWindow2i;
				topWindow2=numberWindows;
				topWindow2i=i;
			}
			else if (numberWindows>topWindow3)
			{
				topWindow3=numberWindows;
				topWindow3i=i;
			}
		}
		
		//Convert the subimage information to relevant structure
		struct SUB_IMAGE subResult;
		if (topWindow1>0)
		{
			subResult.imgX=windowVector.at(topWindow1i).LeftEdge;
			subResult.imgY=windowVector.at(topWindow1i).BottomEdge;
			subResult.imgWidth=windowVector.at(topWindow1i).RightEdge - windowVector.at(topWindow1i).LeftEdge;
			subResult.imgHeight=windowVector.at(topWindow1i).TopEdge - windowVector.at(topWindow1i).BottomEdge;
			subResult.imgRatioArea=(FLOAT)(subResult.imgWidth*subResult.imgHeight)/(height*width);
			if(subResult.imgWidth>subResult.imgHeight)
				subResult.ratio=(FLOAT)subResult.imgWidth/subResult.imgHeight;
			else
				subResult.ratio=(FLOAT)subResult.imgHeight/subResult.imgWidth;
			
			pSub.push_back(subResult);
		}
		
		if (topWindow2>0)
		{
			subResult.imgX=windowVector.at(topWindow2i).LeftEdge;
			subResult.imgY=windowVector.at(topWindow2i).BottomEdge;
			subResult.imgWidth=windowVector.at(topWindow2i).RightEdge - windowVector.at(topWindow2i).LeftEdge;
			subResult.imgHeight=windowVector.at(topWindow2i).TopEdge - windowVector.at(topWindow2i).BottomEdge;
			subResult.imgRatioArea=(FLOAT)(subResult.imgWidth*subResult.imgHeight)/(height*width);
			
			if(subResult.imgWidth>subResult.imgHeight)
				subResult.ratio=(FLOAT)subResult.imgWidth/subResult.imgHeight;
			else
				subResult.ratio=(FLOAT)subResult.imgHeight/subResult.imgWidth;
			
			pSub.push_back(subResult);
		}
		
		if (topWindow3>0)
		{
			subResult.imgX=windowVector.at(topWindow3i).LeftEdge;
			subResult.imgY=windowVector.at(topWindow3i).BottomEdge;
			subResult.imgWidth=windowVector.at(topWindow3i).RightEdge - windowVector.at(topWindow3i).LeftEdge;
			subResult.imgHeight=windowVector.at(topWindow3i).TopEdge - windowVector.at(topWindow3i).BottomEdge;
			subResult.imgRatioArea=(FLOAT)(subResult.imgWidth*subResult.imgHeight)/(height*width);
			
			if(subResult.imgWidth>subResult.imgHeight)
				subResult.ratio=(FLOAT)subResult.imgWidth/subResult.imgHeight;
			else
				subResult.ratio=(FLOAT)subResult.imgHeight/subResult.imgWidth;
			
			pSub.push_back(subResult);
		}
		
		if (pSub.size()>0)
		{
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		return FALSE;
	}
}


/*---------------------------------------------------------------------------
 //-  Function Name : ScanHbitmapHash
          
 //-  Description   : To determine the porn confidence value of image in hbitmap format
 //-  Parameter     : hbmp - hbitmap image
 enMedia_Type - media type either of subimage or fullimage
 enEngine_Type - engine type either of screenshield or ishield
 enScan_Type - scanning type either of normal or quick
 sysResults - a structure that contain confidence value
 //-  Return Value  : TRUE if success, and FALSE if otherwise
 //-------------------------------------------------------------------------*/

bool ImageProcessing::ScanHbitmapHash(/*IplImage*  hbmp*/ Mat hbmp,
										EN_FSIP_MEDIA_TYPE   enMedia_Type,
										EN_FSIP_ENGINE_TYPE  enEngine_Type,
										EN_FSIP_SCAN_TYPE    enScan_Type,
										ImageProcessing::SYSTEM_IP_RESULTS * sysResults,
										//						CMLogger *           pobjLogger,
										char *				 Hash)
{
	if (IsInit() == FALSE)
	{
		return FALSE;
	}
	
//	EngineInitialisation(enEngine_Type);
	EngineInitialisation_V3_6(enEngine_Type);	//JH *V3.6 is backward compatible to V3.5
	if(enMedia_Type==FSIP_MEDIA_FULL_IMAGE)
	{
		WQ_IP_RESULTS pResults = {0};
		
		if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006)
		{
			if(MixedAnalysisProcess(hbmp,&pResults,enEngine_Type,enScan_Type))
			{
				sysResults->dConfidence_Value = static_cast<INT>(pResults.RT);
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
		}
		else 
		{
			if(AnalysisProcess(hbmp,&pResults,enEngine_Type,enScan_Type))
			{
				sysResults->dConfidence_Value = static_cast<INT>(pResults.NetPornValue*100.0f);
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
		}
		
		
		
#ifdef _OUTPUT_INTEGRATION_VALUES
			_TCHAR buffTemp[256] = {0};
			GetAnalysisResultText(buffTemp, 256, _TEXT("SCANHBITMAP"), &pResults);
			OutputDebugString(buffTemp);
#endif
		
	}
	else
	{
		std::vector<struct SUB_IMAGE> pSub;
		std::vector<struct WQ_IP_RESULTS> pFeature;
		// Obtain the sub images and their porn values 
		if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006)
		{
			
			if(MixedCaptureWindowProcess(hbmp,pSub,pFeature,enEngine_Type,enScan_Type))
			{
				std::vector <FLOAT> vRes;
				std::vector <FLOAT> vArea;
				std::vector <FLOAT> vRatio;
				
				for(int i=0;i<pFeature.size();i++)
				{
					vRes.push_back(pFeature[i].RT);
					vArea.push_back(pSub[i].imgRatioArea);
					vRatio.push_back(pSub[i].ratio);
				}
				
				FLOAT sub_pornvalue;
				int best = 0;
				sub_pornvalue=MixedGetPornValueFromImage(&vRes,&vArea,&vRatio, best);
				sysResults->dConfidence_Value = static_cast<INT>(sub_pornvalue);
				
				// Need to dump the hash value into registry
				if (best<pSub.size())
				{
					memcpy(Hash , pSub[best].hash , 33); //SetImageHash(pSub[best].hash);
				}
				
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
		}
		else 
		{
			if(CaptureWindowProcess(hbmp,pSub,pFeature,enEngine_Type,enScan_Type))
			{
				// Obtain the final porn value by analysing the subimages
				std::vector <FLOAT> vRes;
				std::vector <FLOAT> vArea;
				std::vector <FLOAT> vRatio;
				for(int i=0;i<pFeature.size();i++)
				{
					vRes.push_back(pFeature[i].NetPornValue);
					vArea.push_back(pSub[i].imgRatioArea);
					vRatio.push_back(pSub[i].ratio);
				}
				
				FLOAT sub_pornvalue;
				int best = 0;
				sub_pornvalue=GetPornValueFromImage(&vRes,&vArea,&vRatio, best);
				sysResults->dConfidence_Value = static_cast<INT>(sub_pornvalue*100.0f);
			}
			else
			{
				sysResults->dConfidence_Value = -1;
				return FALSE;
			}
			
		}
		
	}
	
	return TRUE;
}

bool ImageProcessing::MixedCaptureWindowProcess(/*IplImage* */ cv::Mat hbmp,
												  std::vector<struct SUB_IMAGE> &pSub,
												  std::vector<WQ_IP_RESULTS> &pFeature,
												  EN_FSIP_ENGINE_TYPE enEngine_Type,
												  EN_FSIP_SCAN_TYPE enScan_Type)
{
	
	if(pSub.size()>0)	pSub.clear();
	if(pFeature.size()>0) pFeature.clear();
	

     cv::Mat hbm0 = cv::Mat(hbmp.cols,hbmp.rows,hbmp.type());
    
	uchar* hbm0_img;
	INT BitsPixel = (hbmp.depth()*hbmp.channels());
                      
                        if (cnt == 0) rectangle( AreasImage, pt1, pt2, CV_RGB(255,0,0), 12, CV_AA, 0 );
                        else if (cnt == 1) rectangle( AreasImage, pt1, pt2, CV_RGB(0,255,0), 12, CV_AA, 0 );
                        else if (cnt == 2) rectangle( AreasImage, pt1, pt2, CV_RGB(0,0,255), 12, CV_AA, 0 );
                        else if (cnt == 3) rectangle( AreasImage, pt1, pt2, CV_RGB(255,255,0), 12, CV_AA, 0 );
                        else if (cnt == 4) rectangle( AreasImage, pt1, pt2, CV_RGB(0,255,255), 12, CV_AA, 0 );
                        else if (cnt == 5) rectangle( AreasImage, pt1, pt2, CV_RGB(255,0,255), 12, CV_AA, 0 );
                        else  rectangle( AreasImage, pt1, pt2, CV_RGB(255,255,255), 12, CV_AA, 0 );
						
						
					}
#ifdef _JHDebug_showImage					
					cvShowImage("Full Screen Image" , AreasImage);
#endif					
					if (AreasImage.cols > 600 && AreasImage.rows >800)//
					{
						
						iHeight    = AreasImage.rows;
						iWidth     = AreasImage.cols;//AreasImage->width;
						if (iHeight > iWidth)
						{
							if (iHeight >= 800)
							{
								fResH = 800;
								fResW = iWidth * (float)(fResH / iHeight);
							}
						}
						if (iWidth > iHeight)
						{
							if (iWidth >= 600)
							{
								fResW = 600;
								fResH = iHeight * (float)(fResW / iWidth);
							}
						}
					
                        destination = cv::Mat((int)(fResW) , (int)(fResH ),AreasImage.type());
                   
                        cv::resize(AreasImage, destination,destination.size());
#ifdef _JHDebug_showImage
					
                        cv::imshow("Full Screen Image" , destination);
#endif
					}
					else 
					{
#ifdef _JHDebug_showImage							
					
                        cv::imshow("Full Screen Image" , AreasImage);
#endif						
					}
					
                    cv::moveWindow("Full Screen Image", 20, 20);
#ifdef _JHDebug_showlog
				//	TCHAR temps[256]={0};
					printf("======Starting Analysing Full Screen Mode (Number of Sub Images %d ) ====== \n" ,pSub.size());
					printf("Starting Process");
#endif					
					
                    cv::destroyWindow("Full Screen Image");
			
                cv::destroyWindow("Full Screen Image");
			}
			//Process each sub image
			for(INT cnt=0;cnt<pSub.size();cnt++)
			{

#ifdef _JHDebug_showlog	
		//		TCHAR temps[256]={0};
				printf("\n-------------------------- Starting Analysis Of SubImage %d -------------------------- \n\n" ,cnt+1);

			//	if (ShowDetailedImage == TRUE) // _tcscat(ImageAnalysisMsg , temps);
#endif				
				int x = pSub[cnt].imgX;
				INT y = pSub[cnt].imgY;
				INT width2 = pSub[cnt].imgWidth;
				INT height2 = pSub[cnt].imgHeight;
				
				INT topY = (height-y-height2);      
				if (topY<0) topY = - topY;
				//uchar * imghbm1  = (uchar*) hbm1->imageData;//opencv4 update
                uchar * imghbm1  = (uchar*) hbm1.data;
			//	int bpp=  (hbm1->depth*hbm1->nChannels);
                int bpp=  (hbm1.depth()*hbm1.channels());
				uchar * hbmSub = NULL;
#ifdef _JHDebug_ScreenShot_SaveImage				
	//			cvSaveImage("/Library/Application Support/e-safe/Reports/M/ScreenShotorg.jpg", hbm1, 0);
#endif				
				
			//	HBITMAP hbmSub = NULL;
				if(width2 <= 300 && height2 <= 300)
				{
				
					if (CMBitmapProcessing::ResizeBitmapSS(&hbmSub,imghbm1,x, topY, hbm1.cols, hbm1.rows, width2,height2,width2,height2, bpp) == FALSE)
					//		if (CMBitmapProcessing::ResizeBitmapGdiplus(&hbmSub,hbm1,x,topY,width2,height2,width2,height2) == FALSE)
					{
						if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp) //updated api for opencv 4 emad
						{
						//	cvReleaseImage(&hbm1);
					//		DeleteObject(hbm1);
							hbm1 = NULL;
						}
						return FALSE;
					}
				}
				else
				{
					DOUBLE ratio2=0.0f;
					INT scalewidth2=0;
					INT scaleheight2=0;
					ratio2=(DOUBLE)height2/(DOUBLE)width2;
					
					// Made Changes 5th Feb 
					if(ratio2>1)
					{
						scaleheight2=300;
						scalewidth2=INT(300/ratio2);
					}
					else
					{
						scalewidth2=300;
						scaleheight2=INT(300*ratio2);
					}
				       
					if (CMBitmapProcessing::ResizeBitmapSS(&hbmSub, imghbm1, hbm1.cols, hbm1.rows, x, topY, width2, height2, scalewidth2, scaleheight2, bpp) == FALSE)		//JH - optimise
	
					{
						if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
						{
							//cvReleaseImage(&hbm1);
						//	DeleteObject(hbm1);
							hbm1 = NULL;
						}
						return FALSE;
					}
					width2=scalewidth2;
					height2=scaleheight2;
				}
				
				WQ_IP_RESULTS pResults = {0};
				
			
				BYTE *subImage=(BYTE*) hbmSub;
				int subImageWidthBytes=(width2 * 3 % 4) ? ((((width2 * 3) >> 2) + 1) << 2) : width2 * 3;
		//		int wb_subImage= CMBitmapProcessing::CalcWidthBytes(width2, bpp);	
				int  length = height2*subImageWidthBytes;
				MD5SS md5(subImage,length); // call a limited version of the file hash that only looks at max the first 1Mb
				md5.hex_digest(pSub[cnt].hash);
				
				bool bProcess = FALSE;

			
                
                cv::Mat cvhbmSub =	cv::Mat(width2,height2,hbm1.type());	
                cvhbmSub.data= (uchar*)hbmSub;
                
#ifdef _JHDebug_ScreenShot_SaveImage	
				char * path= new char[100];
				sprintf(path, "/Library/Application Support/e-safe/Reports/M/ScreenShotSub_%d.jpg", cnt) ;
				cvSaveImage(path, cvhbmSub, 0);
				delete[] path;
#endif				
				if (ShowDetailedImage == TRUE)
				{
					// Show the Sub Imag
					
					char WinName[16];
					sprintf (WinName , "Image %d" , cnt+1);
					
				
					uchar* hbmpCropped = NULL;	
					uchar* hbmCrop = NULL;
					IplImage *img = NULL;
					
					int cropSize=0;
					
					if (ChangeTo24Bit(hbmCrop,hbmSub,bpp))
				
					{
					
						cropSize = QuickCropTest(hbmCrop , width2 , height2 , bpp);
						
						if (cropSize==0) 
							CMBitmapProcessing::CloneBitmap(&hbmpCropped, hbmCrop);
						else CMBitmapProcessing::ResizeBitmap(&hbmpCropped, hbmCrop,0,0,width2,height2-cropSize,width2,height2-cropSize,bpp);
						
					}
					
					if (hbmCrop!=NULL) 
					free(hbmCrop);
				
					hbmCrop=NULL;
					bool bload = LoadHBITMAP(hbmpCropped, &img,width2,height2-cropSize,bpp);
					if (bload )
					{
#ifdef _JHDebug_showImage						
						cvNamedWindow(WinName, CV_WINDOW_AUTOSIZE);
						cvShowImage(WinName , img);
						cvMoveWindow(WinName, 10 , 10);
#endif							
					}
					cvReleaseImage(&img);
				}
				
		
				if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006|| enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) {
					bProcess = MixedAnalysisProcess(cvhbmSub,&pResults,enEngine_Type,enScan_Type);	
				} else {
		
					bProcess = MixedAnalysisProcess(cvhbmSub,&pResults,enEngine_Type,enScan_Type);	
				}
				
				
				if (pResults.pf) {
					
					char * subImagePath= new char[100];
					strcpy(subImagePath, szfilepath);
					char * filename = new char[12];;
					sprintf(filename, "sub_%d.jpg",cnt);
					strcat(subImagePath, filename);
                    
			//		cvSaveImage(subImagePath, cvhbmSub, 0);
                    bool save = imwrite(subImagePath, cvhbmSub);
                    
					strcpy( pSub[cnt].path, subImagePath);
					delete[] filename;
					delete[] subImagePath;
					
				} else {
					strcpy( pSub[cnt].path, "");
				}

				
#ifdef _DUMPSCREENIMAGE
				
				INT DUMP_IMAGES = GetRegValue(_T("DUMP_IMAGES"));
				if(DUMP_IMAGES == 2 || DUMP_IMAGES == 3)
				{
					NumThumbs++;
					TCHAR commonPath1[MAX_PATH] = {0};
					_stprintf(commonPath1,_T("%s\\SubImage%d_%d.jpg"), debugBlockedOnScreenPath  , NumThumbs , pResults.RT);
					HBITMAP hbmpCropped = NULL; 
					HBITMAP hbmCrop = NULL; 
					IplImage *img = NULL;
					
					if (ChangeTo24Bit(hbmCrop,hbmSub))
					{
						//obtain a new bitmap after conversion
						BITMAP bm;
						::GetObject(hbmCrop, sizeof(bm), &bm); 
						
						int cropSize = QuickCropTest(&bm);
						
						if (cropSize==0) 
							CMBitmapProcessing::CloneBitmap(&hbmpCropped, hbmCrop);
						else CMBitmapProcessing::ResizeBitmap(&hbmpCropped, hbmCrop,0,0,bm.bmWidth,bm.bmHeight-cropSize,bm.bmWidth,bm.bmHeight-cropSize);
						
					}
					
					if (hbmCrop!=NULL) DeleteObject(hbmCrop);
					hbmCrop=NULL;
					
					bool bload = LoadHBITMAP(hbmpCropped, &img);
					
					char path[MAX_PATH] = {0};
					
					for (int i=0 ; i < MAX_PATH ; i++)
					{
						path[i] = NULL; 
					}
					
					for (int i=0 ; i < _tcslen(commonPath1) ; i++)
					{
						path[i] = commonPath1[i]; 
					}
					
					if (bload && pResults.RT >0 )
					{
						cvSaveImage (path , img);
					}
					cvReleaseImage(&img);
					if (hbmpCropped!=NULL) DeleteObject(hbmpCropped);
					hbmpCropped=NULL;
				}
				
#endif
				
				if (bProcess == TRUE)
				{
		
					
#ifdef _OUTPUT_INTEGRATION_VALUES
					
					{
						_TCHAR buffTemp[256] = {0};
						GetAnalysisResultText(buffTemp, 256, _TEXT("CAPTUREWINDOWPROCESS"), &pResults);
						OutputDebugString(buffTemp);
						
						/*
						 MessageBox(NULL,buffTemp,"",0);
						 */
					}
					
#endif
					
					pFeature.push_back(pResults);
					
				}
				else
				{
					pSub.erase(pSub.begin()+cnt);
				}
				
			//	cvReleaseImage(&cvhbmSub);
				free(hbmSub);
			//	DeleteObject(hbmSub);
				hbmSub = NULL;
				
				/////////////////////////////////////////////////////////////////////////////
			} //end of each subimage processing
		} //end of if(subimage)
		else
		{
	#ifdef _JHDebug_showlog			
			if (ShowDetailedImage == TRUE)  //
			printf("Failed To Detect any SubImage Out of The Whole Image \n\n");
	#endif
			// if no subimage found
			WQ_IP_RESULTS pResults = {0};
			
			struct SUB_IMAGE subResult;
			
			
            
            int hbm1WidthBytes=(hbm1.cols* 3 % 4) ? ((((hbm1.cols* 3) >> 2) + 1) << 2) : hbm1.cols * 3;
            BYTE *subImage=(BYTE*) hbm1.data;
            
			//int  length = hbm1->height*hbm1WidthBytes;
            int  length = hbm1.rows*hbm1WidthBytes;
			MD5SS md5(subImage,length); // call a limited version of the file hash that only looks at max the first 1Mb
			md5.hex_digest(subResult.hash);
#ifdef _JHDebug_showlog				
			if (ShowDetailedImage == TRUE)  printf("Starting To analysis The Whole Image The Whole Image \n\n");
#endif				
								
			bool bProcess = false;
		
			if (enEngine_Type == FSIP_MIXED_ENGINE_APR_2006|| enEngine_Type == FSIP_MIXEDVID_ENGINE_APR_2006) {
				bProcess = MixedAnalysisProcess(hbm1,&pResults,enEngine_Type,enScan_Type);
			} else {
			
				bProcess = MixedAnalysisProcess(hbm1,&pResults,enEngine_Type,enScan_Type);
			}
			
			if (bProcess == TRUE)
			{
				subResult.imgX=0;
				subResult.imgY=0;
				subResult.imgWidth=width;
				subResult.imgHeight=height;
				subResult.imgRatioArea=1.0f;
				subResult.ratio=(FLOAT)width/height;
				
				//JH if it is porn image save the subimage and path
				if (pResults.pf) {
					
					char * subImagePath= new char[100];
					strcpy(subImagePath, szfilepath);
					char * filename = new char[12];
					sprintf(filename, "image.jpg");
					strcat(subImagePath, filename);
                    
					
                    imwrite(subImagePath, hbm1);
					strcpy( subResult.path, subImagePath);
					delete[] filename;
					delete[] subImagePath;
					
				} else {
					strcpy( subResult.path, "");
				}
				
				pSub.push_back(subResult);
				pFeature.push_back(pResults);
				
			}
			else
			{
				if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
				{
					//cvReleaseImage(&hbm1);
				//	DeleteObject(hbm1);
					hbm1 = NULL;
				}
				return FALSE;
			}
		}	//end of else (subimage)
	} //end of if(image)
	else
	{
		if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
		{
		//	cvReleaseImage(&hbm1);
		//	DeleteObject(hbm1);
			hbm1 = NULL;
		}
		return FALSE;
	}
	
	// if the subimage found successfully
	if ( matIsEqual(hbm0, hbmp))//(hbm1 != hbmp)
	{
		//cvReleaseImage(&hbm1);
	//	DeleteObject(hbm1);
		hbm1 = NULL;
	}
	
	
	return TRUE;
}

FLOAT ImageProcessing::MixedGetPornValueFromImage(std::vector<FLOAT>*vRes, std::vector<FLOAT> *vArea, std::vector<FLOAT> *vRatio, int &best)		//JH-added
{
	FLOAT iResults = 0;
	
	INT total_subimage=0;
	FLOAT FPornValue=0.0f;
	FLOAT TotalArea=0.0f;
	INT nofeature;
	if(vRes->size()==1)
	{
		
		if ((*vRes)[0]>0 && (*vRatio)[0]<=5) //previously is 0.0f
			FPornValue=(*vRes)[0];
		else
			FPornValue=0;
		best = 0;
	}
	
	if(vRes->size()==2)
	{
		for(nofeature=0;nofeature<vRes->size();nofeature++)
		{
			if ((*vRes)[nofeature]>0 && (*vRatio)[nofeature]<=5) //previously is 0.0f
			{
				total_subimage++;
				TotalArea+=(*vArea)[nofeature];
			}
		}
		
		if ((*vRes)[1]>=(*vRes)[0]) FPornValue= (*vRes)[1];
		else FPornValue= (*vRes)[0];
		
	}
	
	if(vRes->size()==3)
	{
		for(nofeature=0;nofeature<vRes->size();nofeature++)
		{
			if ((*vRes)[nofeature]>0.25f && (*vRatio)[nofeature]<=5) //previously is 0.0f
			{
				total_subimage++;
				TotalArea+=(*vArea)[nofeature];
			}
		}
		
		best = 0;
		if ((*vRes)[1]>=(*vRes)[0] && (*vRes)[1]>=(*vRes)[2]) FPornValue= (*vRes)[1];
		else if ((*vRes)[2]>=(*vRes)[0] && (*vRes)[2]>=(*vRes)[1]) FPornValue= (*vRes)[2];
		else FPornValue= (*vRes)[0];
	}
	
	//DECISION MAKING PROCESS
	if (total_subimage==0)   iResults=FPornValue;
	if (total_subimage==1)
	{
		if (TotalArea<=0.06) //previously is 0.05f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	
	if (total_subimage==2)
	{
		if (TotalArea<=0.12) //previously is 0.1f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	
	if (total_subimage==3)
	{
		if (TotalArea<=0.18) //previously is 0.15f
			iResults=0.0f;
		else
			iResults=FPornValue;
	}
	return iResults;
}

