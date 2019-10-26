#ifndef LIBCAMERA_VATYPE_H
#define LIBCAMERA_VATYPE_H

#include <stdio.h>

/**
 * @brief S3GFormat indicate BufferInfo Format
 */
typedef enum S3GFormat{
    S3G_FORMAT_RGB24          = 7,
    S3G_FORMAT_YUY2           = 12,
    S3G_FORMAT_NV12_LINEAR    = 13,
    S3G_FORMAT_NV12_TILED     = 14,
    S3G_FORMAT_NV21_LINEAR    = 15,
} S3GFormat;

/**
 * @brief BufferFlag indicate BufferInfo used for.
 * default use BUFFER_FLAG_NONE this flag,
 * BUFFER_FLAG_IMMED_INV_TLB only used for vip.
 *
 */
typedef enum BufferFlag{
    BUFFER_FLAG_NONE            = 0x00000000,
    BUFFER_FLAG_IMMED_INV_TLB   = 0x00000001,       /* when create buffer for vip use, set this flag*/
} BufferFlag;

/**
 * @brief LockFlag use for access gpu buffer @BufferInfo,
 * LOCK_FLAG_READ_ONLY use to read_only access,
 * LOCK_FLAG_READ_WRITE use to read_write access, it will slower than LOCK_FLAG_READ_ONLY
 */
typedef enum LockFlag{
    LOCK_FLAG_READ_ONLY      = 0x00,
    LOCK_FLAG_READ_WRITE     = 0x01,
} LockFlag;

typedef struct Rect{
    int             x;
    int             y;
    int             width;
    int             height;
} Rect;

typedef enum RotateDegree{
    ROTATE_DEGREE_IDENTITY  = 0,
    ROTATE_DEGREE_0         = 1,
    ROTATE_DEGREE_90        = 2,
    ROTATE_DEGREE_180       = 3,
    ROTATE_DEGREE_270       = 4,
} RotateDegree;

typedef enum CodecFormat{
    CODEC_FORMAT_ENCODE_H264=0x1,
    CODEC_FORMAT_ENCODE_HEVC,

    CODEC_FORMAT_DECODE_H264=0x10,
    CODEC_FORMAT_DECODE_HEVC,
    CODEC_FORMAT_DECODE_MJPEG,
} CodecFormat;



#endif // LIBCAMERA_VATYPE_H
