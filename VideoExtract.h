/*
 *  VideoExtract.h
 *  ImageProcessingC
 *
 *
 */


#include <opencv2/core/cvdef.h>
#include <opencv2/core/types_c.h>
#include <opencv2/core.hpp>


extern "C"{
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "inttypes.h"
#include "stdint.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "typedefin.h"
}

class CVideoExtract
{
public:
	CVideoExtract();
	virtual ~CVideoExtract();
	
    bool    Open(_TCHAR* strFilename);
    void    Close();
	void    CloseVideo();
	void    CloseVideo1();
	
	void CloseVideo_av_close_input_file_pFormatCtx();
	void CloseVideo_av_free_pFrame();
	void CloseVideo_av_free_pFrameRGB();		
	void CloseVideo_avcodec_close_pCodecCtx();
	
	bool	IsNotLowQualityVid();		
	
    long    FrameCount();
    long    GetHeight();
    long    GetWidth();
	
    bool    IsVideo();
	
    _TCHAR* GetErr();
	
    double  GetFrameRate();
    double  GetStreamLength();
	
    bool    SaveFrame(_TCHAR* szFilename, long nFrame);
   
	
	
	int    BufferWSize();
	int    BufferHSize();
	int    TotalFrame();
	float  TotalDuration();
	int    TotalNumofBytes();
	bool   AllocateFrm();
	bool   AllocateBuf(_TCHAR* strFilename);

    cv::Mat GetIplImage(long nFrame);

	bool GetStartTime (long nframe , float duration);
	
	
private:
    double  m_fFrameRate;
    double  m_fStreamLength;
    long    m_iFrames;
    long    m_iHeight;
    long    m_iWidth;
	
    void   *m_lpDet;
    void   *m_lpProv;
	int start_time;
	int input_start_time;
	int input_start_time1;
	float fTimeStep;
	
	
    _TCHAR* m_lpszErr;
	
    double  GetStreamTime(long nFrame);
  	
	/////////////////// 
	
	//AVCodecParameters *formatParams; //AVFormatParameters *formatParams;
    
  //  AVCodecParameters formatParamsDevice;//AVFormatParameters formatParamsDevice;
    AVInputFormat *iformat;
	AVFormatContext *pFormatCtx =NULL;
	AVCodecContext  *pCodecCtx;
	int videoStream;
	int iBufferWidth;
	int iBufferHeight;
	float fDuration;
	float fps;
	int totalSampleCount;
	float duration;
	AVFrame         *pFrame; 
    AVFrame         *pFrameRGB;
	uint8_t         *buffer;
	int				numBytes;
	AVCodec         *pCodec;
	unsigned int    colorChannelCount;
	AVPacket        packet;
	int             frameFinished;

    cv::Mat frame;
	unsigned char *ptr1;
	char *ptr2;
	struct SwsContext *img_convert_ctx =NULL;
};
