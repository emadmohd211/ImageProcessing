/*
 *  VideoExtract.cpp
 *  ImageProcessingC
 *
 *
 */

#include "VideoExtract.h"

#define CVIDEOEXTRACT_OK                ""
#define CVIDEOEXTRACT_NOT_VIDEO         "Not Video"
#define CVIDEOEXTRACT_UNKNOWN_ERROR     "Unknown Error"
#define CVIDEOEXTRACT_OUTOFMEMORY       "Out Of Memory"

CVideoExtract::CVideoExtract()
{
	
    m_fFrameRate    = 0.0;
    m_fStreamLength = 0.0;
    m_iFrames       = 0;
    m_iHeight       = 0;
    m_iWidth        = 0;
	
    m_lpDet         = NULL;
	
    m_lpszErr       = CVIDEOEXTRACT_OK;
	
	// added By Homam 
///	formatParams=NULL;
    
    iformat=NULL;
  //  formatParamsDevice.channels=0;//   channel = 0;
  ///  formatParamsDevice.standard = "pal";
  
}

CVideoExtract::~CVideoExtract()
{
    	

}

/**
 * <Short Desc/>    : Open a video file.
 * <pre>
 * Function name    : CVideoExtract::Open
 * Description      : Open a video file.
 * Return type      : bool                  - True if successful.
 * Argument         : LPCTSTR strFilename   - Filename.
 * </pre>
 **/
bool CVideoExtract::Open(_TCHAR* strFilename)
{
	
	av_register_all(); 
	
	if(avformat_open_input(&pFormatCtx, strFilename , 0, NULL)!=0)//(av_open_input_file(&pFormatCtx, strFilename , NULL, 0, NULL)!=0)
	{
	
		CloseVideo_av_close_input_file_pFormatCtx();	//JH		
	//	CloseVideo1();
		return false;
	}
	else 
	{
		if(avformat_find_stream_info(pFormatCtx,NULL)<0)//(av_find_stream_info(pFormatCtx)<0) //ffmpeg update change of function name - emad
		{
			CloseVideo_av_close_input_file_pFormatCtx();	
		//	CloseVideo1();
			return false;
		}
		videoStream = av_find_default_stream_index((pFormatCtx));
		
		if(videoStream==-1)
		{		
			CloseVideo_av_close_input_file_pFormatCtx();				
		//	CloseVideo1();
			return false;
		}
		pCodecCtx=pFormatCtx->streams[videoStream]->codec; //need to check
		
		pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
		
		if(pCodec==NULL)
		{
			CloseVideo_avcodec_close_pCodecCtx();	
			CloseVideo_av_close_input_file_pFormatCtx();				
		//	CloseVideo1();
			return false;
		}
		if(avcodec_open2(pCodecCtx, pCodec,NULL)<0)
		{
			CloseVideo_avcodec_close_pCodecCtx();	
			CloseVideo_av_close_input_file_pFormatCtx();				
		//	CloseVideo1();
			return false;
		}
		iBufferWidth =  (pCodecCtx->width)/3;
		iBufferHeight = (pCodecCtx->height)/3;
		fps = av_q2d ((pFormatCtx)->streams[videoStream]->r_frame_rate);
		fDuration=((float)pFormatCtx->duration/(float)AV_TIME_BASE);    
		fDuration = (float)((unsigned int)((fDuration) * 10000)/(float)10000);
		totalSampleCount=( int)(floor((fDuration) * (fps)));
		
		return true;
	}
}



void CVideoExtract::CloseVideo()
{
	if (buffer != NULL)
		delete [] buffer;
	
	if (pFrame != NULL ) {	
		av_free(pFrame);

	}
	
	if (pFrameRGB) {
		av_free(pFrameRGB);

	}
	
	if (pCodecCtx!=NULL) {
		avcodec_close(pCodecCtx);
	
	}
	
	if (pFormatCtx !=NULL) {
	
        avformat_close_input(&pFormatCtx);

	}
	

	
    return;
}

void CVideoExtract::CloseVideo_av_close_input_file_pFormatCtx()
{	
	if (pFormatCtx !=NULL)
        avformat_close_input(&pFormatCtx);//av_close_input_file(pFormatCtx); //update for ffmpeg: emad
    return;
}
void CVideoExtract::CloseVideo_av_free_pFrame()
{	
	if (pFrame != NULL)
		av_free(pFrame);

    return;
}
void CVideoExtract::CloseVideo_av_free_pFrameRGB()
{	
	
	if (pFrameRGB)
		av_free(pFrameRGB);
	
    return;
}

void CVideoExtract::CloseVideo_avcodec_close_pCodecCtx()
{		
		if (pCodecCtx!=NULL)
			avcodec_close(pCodecCtx);
    return;
}


void CVideoExtract::CloseVideo1()
{
	if (pFrame != NULL ) {	
		av_free(pFrame);	
	}
	
	if (pFrameRGB) {
		av_free(pFrameRGB);
	}
	
	if (pCodecCtx!=NULL) {
		avcodec_close(pCodecCtx);
	}
	
	if (pFormatCtx !=NULL) {
		avformat_close_input(&pFormatCtx);//av_close_input_file(pFormatCtx);
        
	}
	

	
    return;
}

bool CVideoExtract::AllocateFrm()
{    
    pFrame= av_frame_alloc();/
	pFrameRGB =av_frame_alloc();
	if (pFrame==NULL)
		return false;   		
	else return true;
}

bool CVideoExtract::AllocateBuf(_TCHAR* strFilename)
{    
	buffer=new uint8_t[numBytes* sizeof(uint8_t)];
	avpicture_fill((AVPicture *)pFrameRGB , buffer, AV_PIX_FMT_BGR32, //need to check
				   pCodecCtx->width ,pCodecCtx->height);
    
   
    //UPDATED FFMPEG PIX_FMT_BGR32 -> AV_PIX_FMT_BGR32 ::EMAD
    switch (pCodecCtx->pix_fmt) {
		case AV_PIX_FMT_GRAY8:
		case AV_PIX_FMT_MONOWHITE:
		case AV_PIX_FMT_MONOBLACK:
		case AV_PIX_FMT_PAL8:
			colorChannelCount=1;
			break;
		case AV_PIX_FMT_YUV420P:
		case AV_PIX_FMT_RGB24:
		case AV_PIX_FMT_BGR24:
		case AV_PIX_FMT_YUV422P:
		case AV_PIX_FMT_YUV444P:
		case AV_PIX_FMT_YUV410P:
		case AV_PIX_FMT_YUV411P:
			
			colorChannelCount=3;
			break;
		default:
			colorChannelCount=3;
	}
    //UPDATE dump_format -> AV_dump_format UPDATE FFMPEG
	av_dump_format(pFormatCtx, 0, strFilename, 0);//dump_format(pFormatCtx, 0, strFilename, 0);
	
	if (colorChannelCount== -1)
		return false;
	else return true;
}

/**
 * <Short Desc/>    : Return number of frames in file.
 * <pre>
 * Function name    : CVideoExtract::FrameCount
 * Description      : Return number of frames in file.
 * Return type      : long - Total number of frames in video file.
 * </pre>
 **/
long CVideoExtract::FrameCount()
{
    if (m_lpDet)
        return m_iFrames;
    else
        return 0;
}

int CVideoExtract::BufferWSize()
{    
	return iBufferWidth;   
}

int CVideoExtract::BufferHSize()
{    
	return iBufferHeight;   
}
int CVideoExtract::TotalFrame()
{    
	return totalSampleCount;   
}

float CVideoExtract::TotalDuration()
{    
	duration = ((float)pFormatCtx->duration/(float)AV_TIME_BASE);
	//MessageBox(NULL,_T("duration"),_T("Succeded"),NULL);
	return duration;   
}

int CVideoExtract::TotalNumofBytes()
{    
	numBytes=avpicture_get_size(AV_PIX_FMT_BGR32 , pCodecCtx->width,
								pCodecCtx->height);
	return numBytes;   
}

/**
 * <Short Desc/>    : Get video image height.
 * <pre>
 * Function name    : CVideoExtract::GetHeight
 * Description      : Get video image height.
 * Return type      : long - Video image height.
 * </pre>
 **/
long CVideoExtract::GetHeight()
{
    if (m_lpDet)
        return m_iHeight;
    else
        return 0;
}

/**
 * <Short Desc/>    : Get video image width.
 * <pre>
 * Function name    : CVideoExtract::GetWidth
 * Description      : Get video image width.
 * Return type      : long - Video image width.
 * </pre>
 **/
long CVideoExtract::GetWidth()
{
    if (m_lpDet)
        return m_iWidth;
    else
        return 0;
}


/**
 * <Short Desc/>    : Check if this instance has a video object.
 * <pre>
 * Function name    : CVideoExtract::IsVideo
 * Description      : Check if this instance has a video object.
 * Return type      : bool - True if this is a video object.
 * </pre>
 **/
bool CVideoExtract::IsVideo()
{
    if (m_lpDet)
        return true;
    else
        return false;
}


/**
 * <Short Desc/>    : Get error message.
 * <pre>
 * Function name    : CVideoExtract::GetErr
 * Description      : Get error message.
 * Return type      : LPCTSTR - Error message.
 * </pre>
 **/
_TCHAR* CVideoExtract::GetErr()
{
    return m_lpszErr;
}

/**
 * <Short Desc/>    : Get frame rate.
 * <pre>
 * Function name    : CVideoExtract::GetFrameRate
 * Description      : Get frame rate.
 * Return type      : double - Frame rate.
 * </pre>
 **/
double CVideoExtract::GetFrameRate()
{
    if (m_lpDet)
        return m_fFrameRate;
    else
        return 0.0;
}


/**
 * <Short Desc/>    : Get stream length.
 * <pre>
 * Function name    : CVideoExtract::GetStreamLength
 * Description      : Get stream duration length.
 * Return type      : double - Stream duration.
 * </pre>
 **/
double CVideoExtract::GetStreamLength()
{
    if (m_lpDet)
        return m_fStreamLength;
    else
        return 0.0;
}



/**
 * <Short Desc/>    : Convert frame number to stream time.
 * <pre>
 * Function name    : CVideoExtract::GetStreamTime
 * Description      : Convert frame number to stream time.
 * Return type      : double      - Stream time.
 * Argument         : long nFrame - Frame number.
 * </pre>
 **/
double CVideoExtract::GetStreamTime(long nFrame)
{
    if (!m_lpDet)
        return -1.0;
	
    if (nFrame < 0)
        return -1.0;
	
    if (nFrame > m_iFrames)
        return -1.0;
    else
        return nFrame * m_fStreamLength / (double)m_iFrames;
}

cv::Mat CVideoExtract::GetIplImage(long nFrame)
{
	
	bool bExit = true;
	bool bExit2 = true;
	int j = 0;
    cv::Mat x;
	while(av_read_frame(pFormatCtx, &packet)>=0 && bExit)
	{
		if(packet.stream_index==videoStream)
		{
			
			avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);
			av_free_packet(&packet);
       
			
			if(frameFinished)
			{
				if(img_convert_ctx == NULL) 
				{
					int w = pCodecCtx->width;
					int h = pCodecCtx->height;
					
					
					img_convert_ctx = sws_getContext(w, h, 
													 pCodecCtx->pix_fmt, 
													 w, h, AV_PIX_FMT_RGB24, SWS_BICUBIC, 
													 NULL, NULL, NULL);
					
					if(img_convert_ctx == NULL)
					{
						return x;
					}
				}
				
				bExit2 = false;
				
				sws_scale(img_convert_ctx, pFrame->data, pFrame->linesize , 0,
						  pCodecCtx->height , pFrameRGB->data , pFrameRGB->linesize);
		
                 frame     = cv::Mat(cvSize(pCodecCtx->width, pCodecCtx->height),CV_8UC3);
				for ( int m= 0 ; m < pCodecCtx->height ; m++)
				{
					ptr1 = pFrameRGB->data[0]+ m*pFrameRGB->linesize[0];
				
                    ptr2 = (char *)frame.data + m*frame.step[0];
					for(int n = 0; n < pCodecCtx->width*3; n=n+3)
					{
						ptr2[n] =  ptr1[n+2];
						ptr2[n+2] =  ptr1[n];
						ptr2[n+1] =  ptr1[n+1];
					}
					
				}
				bExit = false;
				
			}
			j++;
		}
		
		if (bExit == false)
		{
			return frame;
		}
		if (j>20)
		{
			//return NULL;
            return x;
		}
	}
	
	if (bExit2 == true) // new added to solve the corrupted frames 30-03-2011
	{
		return x;
	}
	return x;//testemad
}


bool CVideoExtract::GetStartTime(long nFrame , float duration)
{
	
	fTimeStep = duration /10;
	input_start_time1 = fTimeStep * nFrame;
	input_start_time = input_start_time1;
	input_start_time *= AV_TIME_BASE;
	start_time       = (int64_t)input_start_time;
	
	if (start_time != (int)AV_NOPTS_VALUE) {
		
		int seek_flags = AVSEEK_FLAG_BACKWARD;
		AVStream *st=NULL;
		int ret, index;
		int64_t old_seek_pos;
		
		if (pFormatCtx->start_time != (int)AV_NOPTS_VALUE) {
			start_time += pFormatCtx->start_time;
		}
		
		if(videoStream>=0) {  
			st=pFormatCtx->streams[videoStream];
			old_seek_pos= start_time; 
			AVRational a1;
			a1.den = AV_TIME_BASE;
			a1.num = 1;
			start_time = av_rescale_q(start_time, a1, st->time_base);
		}
		index = av_index_search_timestamp(st, start_time, seek_flags);
		
		
		ret = av_seek_frame(pFormatCtx, videoStream, start_time, seek_flags);
		
		if (ret >= 0)avcodec_flush_buffers(pCodecCtx); //clearing buffer for mpg/Mpeg
		
		if (ret < 0) {
			return false;
		}
	}
	return true;
    
}

bool CVideoExtract::IsNotLowQualityVid()	//emad-added for Video Processing
{
	if (iBufferHeight*3 > 50)
		return true;
	else return false;
}
