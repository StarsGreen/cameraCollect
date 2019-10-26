#include "include.h"
using namespace std;
/////////////////////////////////////////////

//////////////////////////////////////////////
void writePictureToFile(const char* name,unsigned char* src,int length )
{
    FILE* file;
    if((file=fopen(name,"wb"))==NULL)
    {
        printf("file open error\n");
        exit;
    }
    fwrite((void*)src,length,1,file);
    fclose(file);
}
int camera_query_cap(int fd ,struct v4l2_capability* cap)
{
	int ret;
  	ret = ioctl(fd, VIDIOC_QUERYCAP, cap);
	if (ret < 0) {
		cout<<"VIDIOC_QUERYCAP failed"<<endl;
        return -1;

	}
	#if DEBUG
	cout<<"query camera success"<<endl;
	#endif
   return 0;
}

int camera_set_format(int fd ,struct v4l2_format* format)
{
	int ret;
	ret = ioctl(fd, VIDIOC_S_FMT, format);
	if (ret < 0) {
		cout<<"VIDIOC_S_FMT failed"<<endl;
        return -1;
	}
	#if DEBUG
	cout<<"set camera format success"<<endl;
	#endif
   return 0;
}
int camera_request_buffer(int fd ,struct v4l2_requestbuffers* buffer)
{
	int ret;
	ret = ioctl(fd , VIDIOC_REQBUFS, buffer);
	if(ret < 0) {
		cout<<"VIDIOC_REQBUFS failed"<<endl;
        return -1;

	}
	#if DEBUG
	cout<<"request camera buffer success"<<endl;
	#endif
   return 0;
}
int camera_query_buffer(int fd ,struct v4l2_buffer* buffer)
{
	int ret;
	ret = ioctl(fd , VIDIOC_QUERYBUF, buffer);
	if(ret < 0) {
		cout<<"VIDIOC_QUERYBUF failed"<<endl;
        return -1;

	}
	#if DEBUG
	cout<<"query camera buffer success"<<endl;
	#endif
   return 0;
}
int camera_queue_buffer(int fd ,struct v4l2_buffer* buffer)
{
	int ret;
    ret = ioctl(fd , VIDIOC_QBUF, buffer);
    if (ret < 0) {
        cout<<"VIDIOC_QBUF"<<buffer->index<<"failed"<<endl;
        return -1;
    }
	#if DEBUG
	    cout<<"queue camera buffer success"<<endl;
	#endif
   return 0;
}
int camera_dequeue_buffer(int fd ,struct v4l2_buffer* buffer)
{
	int ret;
    ret = ioctl(fd , VIDIOC_DQBUF, buffer);
    if (ret < 0) {
        cout<<"VIDIOC_DQBUF"<<buffer->index<<"failed"<<endl;
        return -1;
    }
	#if DEBUG
	    cout<<"dequeue camera buffer success"<<endl;
	#endif
   return 0;
}
int camera_stream_on(int fd ,enum v4l2_buf_type* vtype)
{
	int ret;
	ret = ioctl(fd, VIDIOC_STREAMON, vtype);
	if (ret < 0) {
		cout<<"VIDEOC_STREAMON failed"<<endl;
        return -1;
	}
	#if DEBUG
	    cout<<" camera stream on success"<<endl;
	#endif
   return 0;

}
int camera_stream_off(int fd ,enum v4l2_buf_type* vtype)
{
	int ret;
	ret = ioctl(fd, VIDIOC_STREAMOFF, vtype);
	if (ret < 0) {
		cout<<"VIDEOC_STREAMOFF failed"<<endl;
        return -1;
	}
	#if DEBUG
	    cout<<"camera stream off success"<<endl;
	#endif
   return 0;
}
int camera_open_device(const char * devicename)
{
    int fd;
	fd = open(devicename, O_RDWR, 0);
	if (fd < 0) {
		cout<<"Open "<<devicename<<"failed"<<endl;
		return -1;
	}
   	#if DEBUG
	  cout<<"open camera success"<<endl;
	#endif
   return fd ;
}

int initCamera(Camera* camera)
{
	int i=0, ret;
	#if DEBUG
	cout<<"start init camera success"<<endl;
	#endif	
	
	int fd=camera_open_device(camera->cameraName);
    camera->fd=fd;

	struct v4l2_capability cap;
    camera_query_cap(fd,&cap);

	struct v4l2_format fmt;
	memset(&fmt, 0, sizeof(fmt));
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = camera->width;
	fmt.fmt.pix.height = camera->height;
	fmt.fmt.pix.pixelformat =  camera->format;
	//fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
     camera_set_format(fd ,&fmt);

	struct v4l2_requestbuffers reqbuf;
	bzero(&reqbuf,sizeof(reqbuf));
	reqbuf.count = BUFFER_COUNT;
	reqbuf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	reqbuf.memory = V4L2_MEMORY_USERPTR;
    camera_request_buffer(fd,&reqbuf);

	struct v4l2_buffer buf;
    //start get  image buffer from gpu
	IVAHelper* helper=IVAHelper::getInstance();
    VAHelperCreateBufferInfoArg arg;
	arg.height=camera->height;
	arg.width=camera->width;
	arg.format=S3G_FORMAT_YUY2;
	arg.flag=BUFFER_FLAG_IMMED_INV_TLB;

	camera->helper=helper;

	CameraBuffer queueBuffer;

	enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    camera_stream_on(fd ,&vtype);

	for (i = 0; i < BUFFER_COUNT; i++)
	{
	  bzero(&buf,sizeof(buf));
      buf.index = i;
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_USERPTR;
	  camera_query_buffer(fd,&buf);

      helper->createBuffer(&arg);      
	  //memcpy(&camera->gpuBuffer[i],arg.buffer,sizeof(BufferInfo));
      camera->gpuBuffer[i]=arg.buffer;
      queueBuffer.allocation=camera->gpuBuffer[i]->allocation;
	  queueBuffer.bufferFd=camera->gpuBuffer[i]->bufferFd;
      // Queen buffer
	  buf.m.userptr=(unsigned long)(&queueBuffer);
      buf.length=sizeof(queueBuffer);
	  
	  camera_queue_buffer(fd,&buf);
	}
 return 0;
}
void camera_thread_cleanup(void* arg)
{

	 #if DEBUG
	  cout<<"camera thread cleanup"<<endl;
	 #endif
	 #if 0
	Camera* camera=(Camera*)arg;
	IVAHelper* helper=camera->helper;
	for(int j=0;j<BUFFER_COUNT;j++)
	{
      assert(camera->bufferInfo[j].imageBuff!=NULL);
      helper->destroyBuffer(camera->gpuBuffer[j]);
      free(camera->bufferInfo[j].imageBuff);
	  enum v4l2_buf_type vtype = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      camera_stream_off(camera->fd ,&vtype);
	}
	#endif
}
void * cameraCollect(void* cam)
{
	
	Camera* camera=(Camera*)cam;
	initCamera(camera);
#if 1	
    int ret;
	int i=0;
	int fd=camera->fd;
    struct v4l2_buffer v4l2buf;
	bzero(&v4l2buf,sizeof(v4l2buf));
	v4l2buf.type= V4L2_BUF_TYPE_VIDEO_CAPTURE;
	v4l2buf.memory= V4L2_MEMORY_USERPTR;

  	#if DEBUG
	printf("start camera collection\n");
	#endif

	IVAHelper* helper=camera->helper;
    assert(helper!=NULL);
	VAHelperLockArg lockArg;
	int length;
	void* start[BUFFER_COUNT];

	length=camera->height*camera->width*2;

   #if 1
	for(int j=0;j<BUFFER_COUNT;j++)
	{
	  start[j]=calloc(1,length);
      assert(start[j]!=NULL);
	}
   #endif
#endif	
    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED,NULL);
    pthread_cleanup_push(camera_thread_cleanup,camera);

   #if DEBUG
	printf("start camera collection thread\n");
	#endif

    while(1)
    {

	  pthread_testcancel();

#if 1
	  v4l2buf.index=i;
      camera_dequeue_buffer(fd,&v4l2buf);

      lockArg.buffer=camera->gpuBuffer[i];
	  lockArg.flag=LOCK_FLAG_READ_ONLY;

      helper->lockBuffer(&lockArg);

      assert(lockArg.pitch>0);
      
      memcpy(start[i],lockArg.data,length);
#if 1
      pthread_mutex_lock(&camera->bufferInfo[i].bufferFlag);
	  camera->bufferInfo[i].imageBuff=start[i];
      camera->bufferInfo[i].length=length;
	  cout<<"camera i :"<<i<<endl;
	  while(camera->bufferInfo[i].unshowdCount>=2);
	  camera->bufferInfo[i].unshowdCount++;
	  pthread_cond_signal(&camera->bufferInfo[i].readyFlag);
      pthread_mutex_unlock(&camera->bufferInfo[i].bufferFlag);
#endif
      helper->unlockBuffer(&lockArg);
	// Re-queen buffer
      camera_queue_buffer(fd,&v4l2buf);
      i++;
	  OVERFLOW_RESET(i,BUFFER_COUNT);
#endif
   }
   pthread_cleanup_pop(0);

   #if DEBUG
     printf("stop camera  collection thread\n");
   #endif
   return NULL;
}

