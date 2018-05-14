/** 
* @file      CameraHal.h 
* @brief     CameraHal.h
* @details   header files for rk3288 UVC Camera device
* @author    hjimi 
* @date      2018.3.20
* @version   1.0 
* @par Copyright (c): www.hjimi.com 
* @par History:          
*   1.0: lumingliang, 2018.3.14, first version\n 
*/ 

#ifndef __HJIMI_CAMERAHAL_H
#define __HJIMI_CAMERAHAL_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <linux/videodev2.h>

#define CAMERAHAL_DEBUGGING

#ifdef CAMERAHAL_DEBUGGING 
#include <libgen.h>
#define CAMERAHAL_DEBUG(format, ...) \
	fprintf(stderr, \
			"[%s:%d/%s] " format "\n", \
			basename(__FILE__), __LINE__,\
			__FUNCTION__, ##__VA_ARGS__)

#define CAMERAHAL_ENTER() \
	fprintf(stderr, \
			"[%s:%d] Enter %s \n", \
			basename(__FILE__), __LINE__, __FUNCTION__)

#define CAMERAHAL_EXIT(code) \
	fprintf(stderr, \
			"[%s:%d] Leave %s (%d) \n\n",\
			basename(__FILE__), __LINE__, __FUNCTION__, code)

#define CAMERAHAL_EXIT_VOID() \
	fprintf(stderr, \
			"[%s:%d] end %s\n",\
			basename(__FILE__), __LINE__, __FUNCTION__)

#else
#define CAMERAHAL_DEBUG(format, ...)
#define CAMERAHAL_ENTER()
#define CAMERAHAL_EXIT_VOID()
#define CAMERAHAL_EXIT(code)
#endif 

#define CLEAR_VAR(var) \
	do{memset((void*)&var, 0, sizeof(var));}while(0)


typedef enum camerahal_error{
	CAMHAL_SUCCESS = 0,
	/* Operation not permitted */
	CAMHAL_EPERM = -1,
	/* No such file or directory */
	CAMHAL_ENOENT = -2,
	/* I/O error */
	CAMHAL_EIO = -5,
	/* Bad file number */
	CAMHAL_EBADF = -9,
	/* No such device */
	CAMHAL_ENODEV = -19,
	/* Invalid argument */
	CAMHAL_EINVAL = -22, 
	/* unKnown error, maybe ioctl failed*/
	CAMHAL_OTHER = -23,

}camerahal_error_t;

struct buffer {
	void *pstart;
	size_t length;
};

typedef struct camerahal_device{
	int camerafd;
	struct v4l2_capability cap;
	struct v4l2_fmtdesc fmtdesc;	
	struct v4l2_format fmt;
	struct v4l2_crop crop;
	struct v4l2_requestbuffers req;
	struct buffer *buffers;
}camerahal_device_t;

typedef void (*camerahal_frame_callback_t)(camerahal_device_t *camera, void *user_ptr);

camerahal_error_t camerahal_open(const char *videonode, 
		camerahal_device_t *camera);

camerahal_error_t camerahal_close(camerahal_device_t *camera);

camerahal_error_t camerahal_get_capability(camerahal_device_t *camera);

camerahal_error_t camerahal_get_fmtdesc(camerahal_device_t *camera);

camerahal_error_t camerahal_get_frame_format(camerahal_device_t *camera);

/*
 *@brief    : 尝试设置，看看设备是否支持该分辨率格式 
 *@param[0] : camera object
 *@param[1] : width 分辨率的宽度
 *@param[2] : hegiht 分辨率的高度
 *@param[3] : pixelformat 像素存储格式，比如：V4L2_PIX_FMT_YUYV、V4L2_PIX_FMT_MJPEG
 * */
camerahal_error_t camerahal_try_set_frame_format(camerahal_device_t *camera,
		int width, int height, int pixelformat);

/*
 *@brief    : 设置帧传输的格式 
 *@param[0] : camera object
 *@param[1] : width 分辨率的宽度
 *@param[2] : hegiht 分辨率的高度
 *@param[3] : pixelformat 像素存储格式，比如：V4L2_PIX_FMT_YUYV、V4L2_PIX_FMT_MJPEG
 * */
camerahal_error_t camerahal_set_frame_format(camerahal_device_t *camera, 
		int width, int height, int pixelformat);

/*
 *@brief    : 设置帧传输的格式 
 *@param[0] : camera object
 *@param[1] : 申请缓冲区存放几张照片，最大不超过5张，常规设置成4
 *@param[2] :
 *@param[3] :
 * */
camerahal_error_t camerahal_requestbuffers(camerahal_device_t *camera,
		int count);


/*
 *@brief    : 开始采集数据流
 *@param[0] : camera object
 *@param[1] :
 *@param[2] :
 *@param[3] :
 * */
camerahal_error_t camerahal_stream_start(camerahal_device_t *camera);


/*
 *@brief    : 关闭捕捉视频流
 *@param[0] : camera object
 *@param[1] :
 *@param[2] :
 *@param[3] :
 * */
camerahal_error_t camerahal_stream_stop(camerahal_device_t *camera);

/*
 *@brief    : 获取一帧图像
 *@param[0] : camera object
 *@param[1] :
 *@param[2] :
 *@param[3] :
 * */
camerahal_error_t camerahal_get_streaming(camerahal_device_t *camera);

#endif
