
#include "include.h"
using namespace std;
pthread_t camera_thread,video_thread;

mediaStrcut media;

int create_thread()
  {
      pthread_create(&camera_thread,NULL,cameraCollect,&media.camera);
      pthread_create(&video_thread,NULL,showVideo,&media);
      return 0;
  }
int cancel_thread()
{
  sleep(1);
	if(pthread_cancel(camera_thread)!=0)
		printf("cancel camera thread failed\n");
	else{
		if(pthread_join(camera_thread,NULL)==0)
		printf("cancel camera thread successfully\n");
		}
	if(pthread_cancel(video_thread)!=0)
		printf("cancel video get thread failed\n");
	else{
		if(pthread_join(video_thread,NULL)==0)
		printf("cancel video thread successfully\n");
		}
	return 0;
}
void signal_proceed(int signo)
{
  if(signo==SIGINT){
    cout<<"recieve SIGINT sinal"<<endl;
    cancel_thread();
  }
  exit(1);
}
int main(int argc, char ** argv)
{
      unsigned char* pixes=NULL;
      memset ( &media, 0, sizeof( mediaStrcut ) );

      media.esContext.height=720;
      media.esContext.width=1280;
        #if DEBUG
      cout<<"hello camera"<<endl;
        #endif
      media.camera.cameraName="/dev/video12";
      media.camera.format=V4L2_PIX_FMT_YUYV;

      for(int i=0;i<BUFFER_COUNT;i++)
      {
        pthread_mutex_init(&media.camera.bufferInfo[i].bufferFlag,NULL);
        pthread_cond_init(&media.camera.bufferInfo[i].readyFlag,NULL);

      }

      #if 1

      media.camera.height=720;
      media.camera.width=1280;

	    if(signal(SIGINT,signal_proceed)==SIG_ERR)
		         perror("signal error");

      create_thread();
      //cameraCollect(&camera);
    #endif
    while (1);
    return EGL_TRUE;
}