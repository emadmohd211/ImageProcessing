/*
 *  CMImageProcessing.cpp
 *  ImageProcessingC
 
 *
 */

#include "CMImageProcessing.h"

/**
 * <Short Desc/>    : This will calculate the porn level based on the array of porn results.
 * <pre>
 * Function name    : CMImageProcessing::GetPornValueFromImages
 * Description      : This will calculate the porn level based on the array of porn results.
 * Return type      : INT 
 * Argument         : std::vector<INT> * pvResults -  Array containing the porn results
 * Argument         : INT iTopCount                -  The number of top values to accumulate for averaging
 * </pre>
 **/

INT GetPornValueFromImages(std::vector<INT> * pvResults, INT threshold)
{
	if (threshold < 0)
		return 0;
	
	//int framesRequired = 1;
	float average = 0;
	
	INT iResults   = 0;
	INT iMax       = static_cast<INT>(pvResults->size());
	
	if (iMax < 3 && threshold == 50)
	{
		return 0;
	}
	
	if (iMax < 6 && threshold == 70)
	{
		return 0;
	}
	
	if (iMax < 6 && threshold == 90)
	{
		return 0;
	}
	
	if (iMax > 0)
	{
		for (INT i=0; i<iMax; i++)
		{
			
			if ((*pvResults)[i] == 5) // Catched By SVM as porn make the Weight Lower /0.5/ for stream Processing only no change in Video
				average += 1;
			else average+=(*pvResults)[i];
		}
		average/=iMax;
	} // if
	
	
	if (average*100>=threshold && threshold != 10)
	{
		iResults = 100;
	}
	else if (average*100>= 90 && iMax >= 6 && threshold == 10)
	{
		iResults = 4;
	}
	
	else if (average*100>= 70 && iMax >= 6 && threshold == 10)
	{
		iResults = 3;
	}
	
	else if (average*100>= 50 && iMax >= 3 && threshold == 10)
	{
		iResults = 2;
	}
	
	else if (average*100>= 25  && threshold == 10)
	{
		iResults = 1;
	}
	
	return iResults;
}

