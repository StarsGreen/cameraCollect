// Book:      OpenGL(R) ES 3.0 Programming Guide, 2nd Edition
// Authors:   Dan Ginsburg, Budirijanto Purnomo, Dave Shreiner, Aaftab Munshi
// ISBN-10:   0-321-93388-5
// ISBN-13:   978-0-321-93388-1
// Publisher: Addison-Wesley Professional
// URLs:      http://www.opengles-book.com
//            http://my.safaribooksonline.com/book/animation-and-3d/9780133440133

#ifndef ESUTIL_H
#define ESUTIL_H

///
//  Includes
//
#include <stdlib.h>
#include <GLES3/gl3.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <EGL/eglplatform.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "config.h"
#include <CAMERA/IVAHelper.h>
#include <CAMERA/VAType.h>


#if 1
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#endif

#include <assert.h>
#include <getopt.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <semaphore.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <time.h>
#include <assert.h>
#include <pthread.h>
#include <signal.h>

#include <iostream>
#include <cstring>
///
// Types
//

typedef struct
{
   // Handle to a program object
   GLuint programObject;

   // Sampler location
   GLint samplerLoc;

   // Texture handle
   GLuint textureId;

}RenderHandler;


typedef struct ESContext ESContext;

struct ESContext
{
   /// Put platform specific data here
   void       *platformData;

   /// Window width
   GLint       width;

   /// Window height
   GLint       height;

   /// Display handle
   EGLNativeDisplayType eglNativeDisplay;

   /// Window handle
   EGLNativeWindowType  eglNativeWindow;

   /// EGL display
   EGLDisplay  eglDisplay;

   /// EGL context
   EGLContext  eglContext;

   /// EGL surface
   EGLSurface  eglSurface;

   /// EGL surface
   EGLConfig eglConfig;

   RenderHandler renderHander;
};

#define BUFFER_COUNT 4
 typedef struct CPUImageBuffer
{
   /* data */
   void* imageBuff;
   int length;
   pthread_mutex_t bufferFlag;
   pthread_cond_t readyFlag;
   int unshowdCount;


} CPUImageBuffer;
 typedef struct Camera
{
   /* data */
   CPUImageBuffer bufferInfo[BUFFER_COUNT];
   BufferInfo*  gpuBuffer[BUFFER_COUNT];
   IVAHelper* helper;
   int fd;
   int format;
   int width;
   int height;
   const char* cameraName;

} Camera;

typedef struct MediaStruct
{
        ESContext esContext;
        Camera camera;
}mediaStrcut;
#if 0
extern EGLBoolean initEGL(ESContext *  esContext);
extern EGLBoolean initShader ( ESContext * esContext);
extern void draw( ESContext * esContext );
extern EGLBoolean createNativeWindow(ESContext * esContext, const char *title);
#endif
extern void * cameraCollect(void* camera);
extern void * showVideo(void* media);

extern void writePictureToFile(const char* name,unsigned char* src,int length );



/* Maroc defination about camera*/
#define VIDEO_FORMAT V4L2_PIX_FMT_YUYV

#define DEBUG 1

/*1 stand for using memory mmap 
  0 stand for using user pointer*/
#define VIDEO_CAPTURE_METHOD 0


#endif // ESUTIL_H


#define OVERFLOW_RESET(paras,value) do{ \
 if(paras>=value)\
   paras=0;\
}while(0);

