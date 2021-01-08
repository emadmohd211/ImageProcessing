/*
 *  imageprocessing.1.0.0.h
 *  imageprocessing.1.0.0
 *
 *  Created by admin-Imac2 on 4/29/13.
 *  Copyright 2013 __MyCompanyName__. All rights reserved.
 *
 */
#include <Cocoa/Cocoa.h>

#ifndef imageprocessing_1_0_0_
#define imageprocessing_1_0_0_
#define EXPORT __attribute__((visibility("default")))

/* The classes below are exported */
//#pragma GCC visibility push(default)
/*
class imageprocessing_1_0_0
{
	public:
		void HelloWorld(const char *);
};
*/

//#ifdef __cplusplus
//extern "C"
//#endif
//int scanImage (char*);

#ifdef __cplusplus
extern "C"
#endif
int scanImage(char *action, char *pathIn, char *mediaType, int thumbsize, char *imageEngine, char *videoEngine, char *pathOut ,int threshold, int frames, int imageType, CFArrayRef currentAppInfo);

typedef int (*scanImage_t) ( char*, char*, char*, int, char*, char*,char *, int, int, int, CFArrayRef);

//typedef int (*scanImage_t) (char*);

#ifdef __cplusplus
extern "C"
#endif
int sum(int a, int b);
typedef int (*sum_t)(int,int);
			  
//#pragma GCC visibility pop
#endif
