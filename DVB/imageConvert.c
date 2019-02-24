
#if 0
#include <libavutil/imgutils.h>
#include <libavutil/parseutils.h>
#include <libswscale/swscale.h>


void convert(char *frame, int w, int h)
{
  enum AVPixelFormat src_pix_fmt = AV_PIX_FMT_YUV420P;
  enum AVPixelFormat dst_pix_fmt = AV_PIX_FMT_RGB24;
  const char *dst_size = NULL;
  uint8_t *dst_data[4];
  int dst_linesize[4];
  
   struct SwsContext *sws_ctx;
   sws_ctx = sws_getContext(w,h, src_pix_fmt,w,h, dst_pix_fmt,SWS_BILINEAR, NULL, NULL, NULL);		
   av_image_alloc(dst_data, dst_linesize,w,h, dst_pix_fmt, 1);	
   sws_scale(sws_ctx, (const uint8_t * const*)frame,w*4, 0, h, dst_data,w*4);
	
	
	
  sws_freeContext(sws_ctx);
  return;
}
#endif