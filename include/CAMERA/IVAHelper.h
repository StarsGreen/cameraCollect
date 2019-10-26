#ifndef LIBCAMERA_VAHELPER_H
#define LIBCAMERA_VAHELPER_H

#include "VAType.h"

//------------------------------------
// BufferInfo
class BufferInfo {
public:
    int             width;
    int             height;
    S3GFormat       format;
    unsigned int    allocation;
    int             bufferFd;
};
class CameraBuffer
{
    public:
    unsigned int    allocation;
    int             bufferFd;
    int reserved0;
    int reserved1;
};
class VAHelperCreateBufferInfoArg {
public:
    VAHelperCreateBufferInfoArg(){
        width = 0;
        height = 0;
        format = S3G_FORMAT_YUY2;
        flag = BUFFER_FLAG_NONE;
        buffer = NULL;
    }
    //in
    int             width;
    int             height;
    S3GFormat       format;
    BufferFlag      flag;
    //out
    BufferInfo *    buffer;     // DO NOT CHANGE
};
//BufferInfo
//------------------------------------

//------------------------------------
// Lock/Unlock
class VAHelperLockArg {
public:
    VAHelperLockArg(){
        buffer = NULL;
        flag = LOCK_FLAG_READ_ONLY;
        data = NULL;
        pitch = 0;
    }
    VAHelperLockArg(BufferInfo * buf, LockFlag fg){
        buffer = buf;
        flag = fg;
        data = NULL;
        pitch = 0;
    }
    //in
    BufferInfo*     buffer;
    LockFlag        flag;
    //out
    void *          data;
    unsigned int    pitch;      //if equal <=0, mean invalid
};
// Lock/Unlock
//-----------------------------------

//----------------------------------
// vppblt
class VAHelperVppbltArg {
public:
    VAHelperVppbltArg(){
        srcBuffer = NULL;
        dstBuffer = NULL;
        srcRect = {0, 0, 0, 0};
        dstRect = {0, 0, 0, 0};
        backGround = 0;
        rotateDegree = ROTATE_DEGREE_IDENTITY;
    }
    //in
    BufferInfo *    srcBuffer;
    BufferInfo *    dstBuffer;
    Rect            srcRect;
    Rect            dstRect;
    int             backGround;
    RotateDegree    rotateDegree;
};
// vppblt
//----------------------------------

//------------------------------
//JPEG Decoder
class JpegDecoder {
public:
    int             width;
    int             height;
};

class VAHelperCreateJpegDecoderArg {
public:
    VAHelperCreateJpegDecoderArg(){
        width = 0;
        height = 0;
        decoder = NULL;
    }
    VAHelperCreateJpegDecoderArg(int w, int h){
        width = w;
        height = h;
        decoder = NULL;
    }
    //in
    int             width;
    int             height;
    //out
    JpegDecoder *   decoder;    //DO NOT CHANGE
};

class VAHelperJpegDecodeArg {
public:
    VAHelperJpegDecodeArg(){
        compressBuffer = NULL;
        bufferLength = 0;
        outputBuffer = NULL;
    }
    VAHelperJpegDecodeArg(const unsigned char * buf, int len){
        compressBuffer = buf;
        bufferLength = len;
        outputBuffer = NULL;
    }
    //in
    const unsigned char*    compressBuffer;
    int                     bufferLength;
    //out
    BufferInfo *            outputBuffer;
};
//JPEG Decoder
//-------------------------------------

//-------------------------------------
// Codec Context

class CodecContext {
public:
    int                 width;
    int                 height;
    CodecFormat         format;
};

class VAHelperCreateCodecContextArg {
public:
    VAHelperCreateCodecContextArg(){
        width = 0;
        height = 0;
        format = CODEC_FORMAT_ENCODE_H264;
        bufferNum = 0;
        context = NULL;
    }
    //in
    int                     width;
    int                     height;
    CodecFormat             format;
    unsigned int            bufferNum;  // if 0, decide by IVAHelper
    //out
    CodecContext *          context;    // DO NOT CHANGE
};

class VAHelperEncodeArg {
public:
    VAHelperEncodeArg(){
        encbuf = NULL;
    }
    VAHelperEncodeArg(BufferInfo * buf){
        encbuf = buf;
    }
    //in
    BufferInfo *                encbuf;
};
// Codec Context
//-------------------------------------

class IVAHelper
{
public:
    class IEncoderListener{
    public:
        virtual ~IEncoderListener(){};
        /**
         * @brief monitoring encoder output, when frames encoding success, it will be invoked,
         * to notify listener encoding bitstream avaible
         * @param buf bitstream buf
         * @param size bitstream size
         */
        virtual void onEncodeBitstreamAvaible(unsigned char * buf, unsigned int size) = 0;
    };
    static IVAHelper * getInstance();
    virtual ~IVAHelper(){};

    /**
     * @brief create gpu buffer, see @VAHelperCreateBufferInfoArg
     * @param arg buffer arg
     * @return return 0 if success
     */
    virtual int createBuffer(VAHelperCreateBufferInfoArg *arg) = 0;
    /**
     * @brief destroy gpu buffer
     * @param arg gpu buffer BufferInfo
     * @return return 0 if success
     */
    virtual int destroyBuffer(BufferInfo *arg) = 0;

    /**
     * @brief lockBuffer/unlockBuffer using for access gpu buffer data
     * @param arg lock arg, see @VAHelperLockArg
     * @return return 0 if success
     */
    virtual int lockBuffer(VAHelperLockArg *arg) = 0;
    virtual int unlockBuffer(VAHelperLockArg *arg) = 0;

    /**
     * @brief video post process, for buffer scale/cut/rotate/format convert and so on
     * @param arg vppblt from src buffer to dst buffer args.
     * @return return 0 if success
     */
    virtual int vppBlt(VAHelperVppbltArg *arg) = 0;

    /**
     * @brief create jpeg decoder
     * @param arg see @VAHelperCreateJpegDecoderArg
     * @return return 0 if success
     */
    virtual int createJpegDecoder(VAHelperCreateJpegDecoderArg *arg) = 0;
    /**
     * @brief destroy jpeg decoder
     * @param decoder jpeg decoder handle
     * @return return 0 if success
     */
    virtual int destroyJpegDecoder(JpegDecoder *decoder) = 0;
    /**
     * @brief excute decode jpeg image, with output arg->outputBuffer
     * @param decoder the jpeg decoder handle
     * @param arg decode args, with input coding bitstream, if decode success, with output a gpu buffer arg->outputBuffer
     * @return return 0 if success
     */
    virtual int decodeJpeg(JpegDecoder *decoder, VAHelperJpegDecodeArg *arg) = 0;

    /**
     * @brief create codec context
     * @param arg see @VAHelperCreateCodecContextArg
     * @return return 0 if success
     */
    virtual int createVACodecContext(VAHelperCreateCodecContextArg *arg) = 0;
    /**
     * @brief destroy codec context
     * @param context the Codec context
     * @return return 0 if success
     */
    virtual int destroyVACodecContext(CodecContext *context) = 0;

    /**
     * @brief set an IEncoderListener to CodecContext, monitoring output bitstream avaible when encode frames.
     * @param context the CodecContext want to monitoring
     * @param listener the listener will be callback
     * @return return 0 if success
     */
    virtual int setEncoderListener(CodecContext *context, IEncoderListener * listener) = 0;

    /**
     * @brief encode the frames. if context has a IEncoderListener, it will be invoked if encoding success
     * @param context the encode context, make sure the CodecFormat is right
     * @param encbuf see @VAHelperEncodeArg, the buffer to be encode
     * @return return 0 if success
     */
    virtual int encode(CodecContext *context, VAHelperEncodeArg *encbuf) = 0;
};
#endif // LIBCAMERA_VAHELPER_H
