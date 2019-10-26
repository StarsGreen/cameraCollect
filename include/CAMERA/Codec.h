#ifndef __PLAYER_H_
#define __PLAYER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "libavcodec/avcodec.h"
#include "libavcodec/vaapi.h"
#include "libavformat/avformat.h"

#ifdef __cplusplus
}
#endif

#include "zxQueue.h"
#include "IVAHelper.h"
#include <atomic>

#define INVALID_STREAM_ID       -1
#define STREAM_EOS              0x80


#define MAX_SURFACE_NUMBER       12
#define DEMUXER_PKTS_COUNT       MAX_SURFACE_NUMBER
#define DECODER_PKTS_COUNT       MAX_SURFACE_NUMBER

#define H264_SURFACE_NUMBER      MAX_SURFACE_NUMBER
#define HEVC_SURFACE_NUMBER      MAX_SURFACE_NUMBER
#define MJPEG_SURFACE_NUMBER     3
#define DISPLAY_SURFACE_NUMBER   3

#define VIDEO_SURFACE_FLAG_NONE    0x00
#define VIDEO_SURFACE_FLAG_DECODE  0x01
#define VIDEO_SURFACE_FLAG_DISPLAY 0x10

typedef enum {
    ERROR_NONE,
    ERROR_PARAM,
    ERROR_IO,
    ERROR_MEM,
    ERROR_UNKNOWN,
    ERROR_EOS
} ERROR_TYPE;

typedef struct {
    uint32_t   demuxer_thread_enable     : 1;
    uint32_t   decoder_thread_enable     : 1;
    uint32_t   render_thread_enable      : 1;
    uint32_t   demuxer_queue_enable      : 1;
    uint32_t   decoder_queue_enable      : 1;
    uint32_t   eos                       : 1;
    uint32_t   reserved_bits             : 27;
} MARKER;

typedef enum {
    QUEUE_ELEMENT_TYPE_COMMON_USE,
    QUEUE_ELEMENT_TYPE_VIDEO_UPDATE,
    QUEUE_ELEMENT_TYPE_VIDEO_MOVE,
    QUEUE_ELEMENT_TYPE_COMMAND_BACK,
    QUEUE_ELEMENT_TYPE_FILL_COLOR,
    QUEUE_ELEMENT_TYPE_MAX,
} QUEUE_ELEMENT_TYPE;

typedef struct {
    QUEUE_ELEMENT_TYPE  type;
    void *              p;
    uint32_t            v;
    void *              queue_back;
} QUEUE_DATA;

class Codec {
public:
    class IRenderListener{
    public:
        virtual void onRenderFrame() = 0;
    };


    class DecodeBufferInfo {
    public:
        BufferInfo * surface;
        uint32_t   surfaceIndex;
        uint32_t   surfaceId;   // VASurfaceID
        uint32_t   surfaceFlag; //see VIDEO_SURFACE_FLAG_
    };

public:
    Codec(const char * filename, bool loop);
    ~Codec();
    int init();
    int deinit();
    int start();
    int stop();
    int waitStop();

    void setListener(IRenderListener * listener){
        this->listener = listener;
    }
    IRenderListener * listener;
    BufferInfo* acquireFrame(){
        return latestBuffer.load();
    }
    void releaseFrame(BufferInfo *){
        return;;
    }


    char                                file[256];
    bool                                loopPlay;

    MARKER                              marker;

    QUEUE_DATA                          demuxerpkts[DEMUXER_PKTS_COUNT];
    QUEUE_DATA                          decoderpkts[DECODER_PKTS_COUNT];
    AVPacket                            avpkts[DEMUXER_PKTS_COUNT];
    zxQueue<QUEUE_DATA *>               demuxer_to_decoder_queue;
    zxQueue<QUEUE_DATA *>               decoder_to_demuxer_queue;
    zxQueue<QUEUE_DATA *>               decoder_to_vpp_queue;
    zxQueue<QUEUE_DATA *>               vpp_to_decoder_queue;
    zxQueue<BufferInfo *>               render_queue;
    std::atomic<BufferInfo*>            latestBuffer;

    //device vaapi
    IVAHelper *                         helper;
    CodecContext *                      vadecoder;
    vaapi_context                       vacontext;

    uint32_t                            surfaceNum;
    DecodeBufferInfo *                  surfaceInfo;
    pthread_cond_t                      surfaceCond;
    pthread_mutex_t                     surfaceMutex;

    //device ffmpeg
    AVFormatContext*                    avformat_ctx;
    AVCodecContext*                     avcodec_ctx;
    AVCodec*                            avcodec;
    AVFrame*                            pic;
    uint32_t                            video_stream_index;
    float                               frame_rate;

private:
    pthread_t    demuxer_thread_id;
    pthread_t    decoder_thread_id;
    pthread_t    render_thread_id;

    int init_demuxer();
    int init_decoder();
    int prepare_to_start();
    int configure_codec_by_vaapi();

};


#endif
