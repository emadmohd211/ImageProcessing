/*
 *  imageprocessing.1.0.0.cp
 *  imageprocessing.1.0.0
 *
 *  Created by admin-Imac2 on 4/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include "testDll.h"
#include <iostream>
#include "imageprocessing.1.0.0.h"
#include "imageprocessing.1.0.0Priv.h"



/*
void imageprocessing_1_0_0::HelloWorld(const char * s)
{
	 imageprocessing_1_0_0Priv *theObj = new imageprocessing_1_0_0Priv;
	 theObj->HelloWorldPriv(s);
	 delete theObj;
};

void imageprocessing_1_0_0Priv::HelloWorldPriv(const char * s) 
{
	std::cout << s << std::endl;
};*/

/*
EXPORT                        // Symbol to export
int scanImage (char* szFilePath)
{
	char *action = "file";
	char *pathIn = szFilePath;
	char *type = "videos";	
	int thumbsize = 0;
	char *engine = "safe";
	char *pathOut = "/homam";
	int threshold = 50;
	int frames = 10;
	int rtResult ;
	rtResult=-99;
	int err=  ImageAnalysis( action, pathIn,type,  thumbsize,  engine,  pathOut,  threshold ,  frames, &rtResult);
	
	if (err ==0 )
		return rtResult;
	else
	{
		switch (err) {
			case 1:
				printf("action invalid") ;
				break;
			case 3:
				printf("type invalid") ;
				break;
			case 5:
				printf("engine invalid") ;
				break;
			case 6:
				printf("path invalid") ;
				break;
			default:
				printf("unknown error") ;	
				break;
		}
		return -1;
	}
}*/
extern int PiResult;

EXPORT                        // Symbol to export
int scanImage (char *action, char *pathIn, char *mediaType, int thumbsize, char *imageEngine, char *videoEngine, char *pathOut ,int threshold, int frames, int imageType, CFArrayRef currentAppInfo)
{
    
//	int rtResult=-99;
	int err=0;
	if (currentAppInfo) {
	//	err=  ScreenAnalysis( action, pathIn, mediaType,  thumbsize,  imageEngine,  videoEngine, pathOut,  threshold ,  frames, imageType, currentAppInfo);
		err=  ScreenAnalysisNewJH( action, pathIn, mediaType,  thumbsize,  imageEngine,  videoEngine, pathOut,  threshold ,  frames, imageType, currentAppInfo);		
	} else {
		err=  ImageAnalysis( action, pathIn, mediaType,  thumbsize,  imageEngine,  videoEngine, pathOut,  threshold ,  frames, imageType);
	}
	
//	int ImageAnalysis(char* actionString1, char* pathIn, char* typeString1, int thumbsize, char* engineString1, char* engineStringVideo1, char* pathOut, int threshold , int frames,, bool imageType)
	if (err ==0 )
		return PiResult;
	else
	{
		switch (err) {
			case 1:
				printf("action invalid") ;
				break;
			case 3:
				printf("type invalid") ;
				break;
			case 5:
				printf("engine invalid") ;
				break;
			case 6:
				printf("path invalid") ;
				break;
			default:
				printf("unknown error") ;	
				break;
		}
		return -1;
	}
}

EXPORT      
int sum( int a , int b){
	return (a+b);
}
