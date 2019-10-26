//
// Simple_Texture2D.c
//
//    This is a simple example that draws a quad with a 2D
//    texture image. The purpose of this example is to demonstrate
//    the basics of 2D texturing
//
#include "include.h"
using namespace std;



///
// Create a simple 2x2 texture image with four different colors
//
GLuint CreateTexture2D(ESContext * esContext,GLubyte* pixels,int width,int height)
{
   // Texture object handle
   GLuint textureId;

   // 2x2 Image, 3 bytes per pixel (R, G, B)
   /*
   GLubyte pixels[4 * 3] =
   {
      255,   0,   0, // Red
        0, 255,   0, // Green
        0,   0, 255, // Blue
      255, 255,   0  // Yellow
   };
   */
   // Use tightly packed data
   glPixelStorei ( GL_UNPACK_ALIGNMENT, 1 );

   // Generate a texture object
   glGenTextures ( 1, &textureId );

   // Bind the texture object
   glBindTexture ( GL_TEXTURE_2D, textureId );

   // Load the texture
   glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE,pixels);

   // Set the filtering mode
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
   glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

   esContext->renderHander.textureId=textureId;

   return textureId;

}

GLuint LoadShader ( GLenum type, const char *shaderSrc )
{
   GLuint shader;
   GLint compiled;

   // Create the shader object
   shader = glCreateShader(type);
   if ( glGetError() !=GL_NO_ERROR )
   {
      printf("glCreateShader error\n");
      return 0;
   }

   // Load the shader source
   glShaderSource ( shader, 1, &shaderSrc, NULL );

   // Compile the shader
   glCompileShader ( shader );

   // Check the compile status
   glGetShaderiv ( shader, GL_COMPILE_STATUS, &compiled );

   if ( !compiled )
   {
      GLint infoLen = 0;

      glGetShaderiv ( shader, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog = (char *)malloc(sizeof ( char ) * infoLen );

         glGetShaderInfoLog ( shader, infoLen, NULL, infoLog );
         printf("Error compiling shader\n");

         free ( infoLog );
      }
      glDeleteShader ( shader );
      return 0;
   }
   return shader;
}
EGLBoolean initShader ( ESContext * esContext )
{

   char vShaderStr[] =
      "#version 300 es                            \n"
      "layout(location = 0) in vec4 a_position;   \n"
      "layout(location = 1) in vec2 a_texCoord;   \n"
      "out vec2 v_texCoord;                       \n"
      "void main()                                \n"
      "{                                          \n"
      "   gl_Position = a_position;               \n"
      "   v_texCoord = a_texCoord;                \n"
      "}                                          \n";

   char fShaderStr[] =
      "#version 300 es                                     \n"
      "precision mediump float;                            \n"
      "in vec2 v_texCoord;                                 \n"
      "layout(location = 0) out vec4 outColor;             \n"
      "uniform sampler2D s_texture;                        \n"
      "void main()                                         \n"
      "{                                                   \n"
      "  outColor = texture( s_texture, v_texCoord );      \n"
      "}                                                   \n";
   GLuint vertexShader;
   GLuint fragmentShader;
   GLuint programObject;
   GLint linked;

   // Load the vertex/fragment shaders
   vertexShader = LoadShader(GL_VERTEX_SHADER, vShaderStr );
      if ( vertexShader == 0 )
   {
      printf("vertexShader create error\n");
      return EGL_FALSE;
   }
   fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fShaderStr );
   if ( fragmentShader == 0 )
   {
      printf("fragmentShader create error\n");
      return EGL_FALSE;
   }
   // Create the program object
   programObject = glCreateProgram ();

   if ( programObject == 0 )
   {
      printf("create program object error\n");
      return EGL_FALSE;
   }

   glAttachShader ( programObject, vertexShader );
   glAttachShader ( programObject, fragmentShader );

   // Link the program
   glLinkProgram ( programObject );

   // Check the link status
   glGetProgramiv ( programObject, GL_LINK_STATUS, &linked );

   if ( !linked )
   {
      GLint infoLen = 0;

      glGetProgramiv ( programObject, GL_INFO_LOG_LENGTH, &infoLen );

      if ( infoLen > 1 )
      {
         char *infoLog =( char *) malloc ( sizeof ( char ) * infoLen );

         glGetProgramInfoLog ( programObject, infoLen, NULL, infoLog );
         printf("linking program\n");
         free ( infoLog );
      }
      glDeleteProgram ( programObject );
      return EGL_FALSE;
   }
   // Store the program object
  esContext->renderHander.programObject = programObject;


     // Get the sampler location
   esContext->renderHander.samplerLoc = glGetUniformLocation (esContext->renderHander.programObject, "s_texture" );

   glClearColor ( 1.0f, 1.0f, 1.0f, 0.0f );
   return EGL_TRUE;
}

EGLBoolean initEGL(ESContext * esContext)
{
        EGLDisplay display;
        EGLConfig  config;
        EGLContext  context;
        EGLSurface  surface;
        NativeWindowType  native_window=0;
        NativeDisplayType  native_display=0;

        EGLint contextAttribs[] = { 
           EGL_CONTEXT_CLIENT_VERSION, 3,
           EGL_NONE };
      
        EGLint const attribute_list[] = {
           EGL_RED_SIZE, 1,
           EGL_GREEN_SIZE, 1,
           EGL_BLUE_SIZE, 1,
           EGL_NONE
};
        EGLint num_config;

#if PLATFORM
         /* create a native window*/
        /* create a native window using x11 window*/
        if(!createNativeWindow(esContext,"test window"))
        {
           printf("createNativeWindow error");
           return EGL_FALSE;
         }
        /* get an EGL display connection */

        native_display=esContext->eglNativeDisplay;
        native_window=esContext->eglNativeWindow;
#endif
        display = eglGetDisplay(native_display);
        if(eglGetError()!=EGL_SUCCESS)
        {
            printf("eglGetDisplay error");
            return EGL_FALSE;
        }
        /* initialize the EGL display connection */
        eglInitialize(display, NULL, NULL);
        /* get an appropriate EGL frame buffer configuration */
        eglChooseConfig(display, attribute_list, &config, 1, &num_config);
        /* create an EGL rendering context */
        context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
        if(eglGetError()!=EGL_SUCCESS)
        {
           printf("eglCreateContext error");
           return EGL_FALSE;
        }
        /* create an EGL window surface */     
        surface = eglCreateWindowSurface(display, config, native_window, NULL);
        if(eglGetError()!=EGL_SUCCESS)
        {
            printf("eglCreateWindowSurface error");
            return EGL_FALSE;
        }
        /* connect the context to the surface */
        eglMakeCurrent(display, surface, surface, context);
        /* clear the color buffer */
        esContext->eglDisplay=display;
        esContext->eglContext=context;
        esContext->eglConfig=config;
        esContext->eglSurface=surface;

        eglQuerySurface(display,surface,EGL_WIDTH,&esContext->width);
        eglQuerySurface(display,surface,EGL_HEIGHT,&esContext->height);

/*
        glClearColor(1.0, 1.0, 1.0, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
        glFlush();
        eglSwapBuffers(display, surface);
        */
}

void draw( ESContext * esContext)
{
   GLfloat vVertices[] = { -0.8f,  0.8f, 0.0f,  // Position 0
                            0.0f,  0.0f,        // TexCoord 0 
                           -0.8f, -0.8f, 0.0f,  // Position 1
                            0.0f,  1.0f,        // TexCoord 1
                            0.8f, -0.8f, 0.0f,  // Position 2
                            1.0f,  1.0f,        // TexCoord 2
                            0.8f,  0.8f, 0.0f,  // Position 3
                            1.0f,  0.0f         // TexCoord 3
                         };
   GLushort indices[] = { 0, 1, 2, 0, 2, 3 };
   // Set the viewport
   glViewport ( 0, 0, esContext->width, esContext->height );

   // Clear the color buffer
   glClear ( GL_COLOR_BUFFER_BIT );

   // Use the program object
   glUseProgram ( esContext->renderHander.programObject );

   // Load the vertex data
   glVertexAttribPointer ( 0, 3, GL_FLOAT, GL_FALSE, 0, vVertices );
   glEnableVertexAttribArray ( 0 );

   // Load the vertex position
   glVertexAttribPointer ( 0, 3, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), vVertices );
   // Load the texture coordinate
   glVertexAttribPointer ( 1, 2, GL_FLOAT,
                           GL_FALSE, 5 * sizeof ( GLfloat ), &vVertices[3] );

   glEnableVertexAttribArray ( 0 );
   glEnableVertexAttribArray ( 1 );

   // Bind the texture
   glActiveTexture ( GL_TEXTURE0 );
   glBindTexture ( GL_TEXTURE_2D, esContext->renderHander.textureId );

   // Set the sampler texture unit to 0
   glUniform1i (esContext->renderHander.samplerLoc, 0 );
   
   glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
}
void video_thread_cleanup(void* arg)
{
	 #if DEBUG
	  cout<<"video thread cleanup"<<endl;
	 #endif
}

void * showVideo(void* media)
{
      MediaStruct* mediaStruct=(MediaStruct* )media;
      ESContext* esContext=&mediaStruct->esContext;
      Camera* camera=&mediaStruct->camera;
      unsigned char* pixes=NULL;
      cv::Mat imgYUV,imgRGB;
      if(!initEGL(esContext))
        {
            cout<<"initEGL error"<<endl;
            return EGL_FALSE;
        }
      if(!initShader(esContext))
        {
            cout<<"initShader error"<<endl;
            return EGL_FALSE;
        }
      imgYUV.create(camera->height,camera->width,CV_8UC2);
      imgRGB.create(camera->height,camera->width,CV_8UC3);
      int i=0;
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
      pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
      pthread_cleanup_push(video_thread_cleanup,media);

      #if DEBUG
	    cout<<"start showVideo thread"<<endl;
	   #endif
      while(1)
      {
      pthread_testcancel();

#if 1

	  #if 0
    //write data tio file
     char strName[10];
	  sprintf(strName,"pic_%d.yuv",i);
	  writePictureToFile(strName,(unsigned char*)camera->bufferInfo[i].imageBuff,camera->bufferInfo[i].length);
     #endif

      pthread_mutex_lock(&camera->bufferInfo[i].bufferFlag);
      pthread_cond_wait(&camera->bufferInfo[i].readyFlag,&camera->bufferInfo[i].bufferFlag);
      cout<<"video i :"<<i<<endl;
      while(camera->bufferInfo[i].unshowdCount<=0);
	   camera->bufferInfo[i].unshowdCount--;
      assert(camera->bufferInfo[i].imageBuff!=NULL);
      memcpy(imgYUV.data,camera->bufferInfo[i].imageBuff,camera->bufferInfo[i].length);

      pthread_mutex_unlock(&camera->bufferInfo[i].bufferFlag);
      #if 1
      cv::cvtColor(imgYUV,imgRGB,CV_YUV2RGB_YUYV);
      pixes=imgRGB.data;
      CreateTexture2D(esContext,(GLubyte*)pixes,camera->width,camera->height);
      draw(esContext);
      eglSwapBuffers(esContext->eglDisplay,esContext->eglSurface);
      #endif
      i++;
      OVERFLOW_RESET(i,BUFFER_COUNT);
#endif
      }
      pthread_cleanup_pop(0);
      #if DEBUG
	    cout<<"stop showVideo thread"<<endl;
	   #endif
      return NULL;
}