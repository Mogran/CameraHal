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

#ifndef HJIMI_CAMERAHAL_H
#define HJIMI_CAMERAHAL_H

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
	fprintf(stderr, "[%s:%d/%s] " format "\n", basename(__FILE__), __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define CAMERAHAL_ENTER() \
	fprintf(stderr, "[%s:%d] Enter %s \n", basename(__FILE__), __LINE__, __FUNCTION__)

#define CAMERAHAL_EXIT(code) \
	fprintf(stderr, "[%s:%d] Leave %s (%d) \n\n", basename(__FILE__), __LINE__, __FUNCTION__, code)
#define CAMERAHAL_EXIT_VOID() \
	fprintf(stderr, "[%s:%d] end %s\n", basename(__FILE__), __LINE__, __FUNCTION__)
#else
#define CAMERAHAL_DEBUG(format, ...)
#define CAMERAHAL_ENTER()
#define CAMERAHAL_EXIT_VOID()
#define CAMERAHAL_EXIT(code)
#endif 

#define CLEAR_VAR(var) do{memset((void*)&var, 0, sizeof(var));}while(0)


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



//#define VIDIOC_QUERYCAP		 _IOR('V',  0, struct v4l2_capability)
//#define VIDIOC_RESERVED		  _IO('V',  1)
//#define VIDIOC_ENUM_FMT         _IOWR('V',  2, struct v4l2_fmtdesc)
//#define VIDIOC_G_FMT		_IOWR('V',  4, struct v4l2_format)
//#define VIDIOC_S_FMT		_IOWR('V',  5, struct v4l2_format)
//#define VIDIOC_REQBUFS		_IOWR('V',  8, struct v4l2_requestbuffers)
//#define VIDIOC_QUERYBUF		_IOWR('V',  9, struct v4l2_buffer)
//#define VIDIOC_G_FBUF		 _IOR('V', 10, struct v4l2_framebuffer)
//#define VIDIOC_S_FBUF		 _IOW('V', 11, struct v4l2_framebuffer)
//#define VIDIOC_OVERLAY		 _IOW('V', 14, int)
//#define VIDIOC_QBUF		_IOWR('V', 15, struct v4l2_buffer)
//#define VIDIOC_EXPBUF		_IOWR('V', 16, struct v4l2_exportbuffer)
//#define VIDIOC_DQBUF		_IOWR('V', 17, struct v4l2_buffer)
//#define VIDIOC_STREAMON		 _IOW('V', 18, int)
//#define VIDIOC_STREAMOFF	 _IOW('V', 19, int)
//#define VIDIOC_G_PARM		_IOWR('V', 21, struct v4l2_streamparm)
//#define VIDIOC_S_PARM		_IOWR('V', 22, struct v4l2_streamparm)
//#define VIDIOC_G_STD		 _IOR('V', 23, v4l2_std_id)
//#define VIDIOC_S_STD		 _IOW('V', 24, v4l2_std_id)
//#define VIDIOC_ENUMSTD		_IOWR('V', 25, struct v4l2_standard)
//#define VIDIOC_ENUMINPUT	_IOWR('V', 26, struct v4l2_input)
//#define VIDIOC_G_CTRL		_IOWR('V', 27, struct v4l2_control)
//#define VIDIOC_S_CTRL		_IOWR('V', 28, struct v4l2_control)
//#define VIDIOC_G_TUNER		_IOWR('V', 29, struct v4l2_tuner)
//#define VIDIOC_S_TUNER		 _IOW('V', 30, struct v4l2_tuner)
//#define VIDIOC_G_AUDIO		 _IOR('V', 33, struct v4l2_audio)
//#define VIDIOC_S_AUDIO		 _IOW('V', 34, struct v4l2_audio)
//#define VIDIOC_QUERYCTRL	_IOWR('V', 36, struct v4l2_queryctrl)
//#define VIDIOC_QUERYMENU	_IOWR('V', 37, struct v4l2_querymenu)
//#define VIDIOC_G_INPUT		 _IOR('V', 38, int)
//#define VIDIOC_S_INPUT		_IOWR('V', 39, int)
//#define VIDIOC_G_OUTPUT		 _IOR('V', 46, int)
//#define VIDIOC_S_OUTPUT		_IOWR('V', 47, int)
//#define VIDIOC_ENUMOUTPUT	_IOWR('V', 48, struct v4l2_output)
//#define VIDIOC_G_AUDOUT		 _IOR('V', 49, struct v4l2_audioout)
//#define VIDIOC_S_AUDOUT		 _IOW('V', 50, struct v4l2_audioout)
//#define VIDIOC_G_MODULATOR	_IOWR('V', 54, struct v4l2_modulator)
//#define VIDIOC_S_MODULATOR	 _IOW('V', 55, struct v4l2_modulator)
//#define VIDIOC_G_FREQUENCY	_IOWR('V', 56, struct v4l2_frequency)
//#define VIDIOC_S_FREQUENCY	 _IOW('V', 57, struct v4l2_frequency)
//#define VIDIOC_CROPCAP		_IOWR('V', 58, struct v4l2_cropcap)
//#define VIDIOC_G_CROP		_IOWR('V', 59, struct v4l2_crop)
//#define VIDIOC_S_CROP		 _IOW('V', 60, struct v4l2_crop)
//#define VIDIOC_G_JPEGCOMP	 _IOR('V', 61, struct v4l2_jpegcompression)
//#define VIDIOC_S_JPEGCOMP	 _IOW('V', 62, struct v4l2_jpegcompression)
//#define VIDIOC_QUERYSTD      	 _IOR('V', 63, v4l2_std_id)
//#define VIDIOC_TRY_FMT      	_IOWR('V', 64, struct v4l2_format)
//#define VIDIOC_ENUMAUDIO	_IOWR('V', 65, struct v4l2_audio)
//#define VIDIOC_ENUMAUDOUT	_IOWR('V', 66, struct v4l2_audioout)
//#define VIDIOC_G_PRIORITY	 _IOR('V', 67, __u32) /* enum v4l2_priority */
//#define VIDIOC_S_PRIORITY	 _IOW('V', 68, __u32) /* enum v4l2_priority */
//#define VIDIOC_G_SLICED_VBI_CAP _IOWR('V', 69, struct v4l2_sliced_vbi_cap)
//#define VIDIOC_LOG_STATUS         _IO('V', 70)
//#define VIDIOC_G_EXT_CTRLS	_IOWR('V', 71, struct v4l2_ext_controls)
//#define VIDIOC_S_EXT_CTRLS	_IOWR('V', 72, struct v4l2_ext_controls)
//#define VIDIOC_TRY_EXT_CTRLS	_IOWR('V', 73, struct v4l2_ext_controls)
//#define VIDIOC_ENUM_FRAMESIZES	_IOWR('V', 74, struct v4l2_frmsizeenum)
//#define VIDIOC_ENUM_FRAMEINTERVALS _IOWR('V', 75, struct v4l2_frmivalenum)
//#define VIDIOC_G_ENC_INDEX       _IOR('V', 76, struct v4l2_enc_idx)
//#define VIDIOC_ENCODER_CMD      _IOWR('V', 77, struct v4l2_encoder_cmd)
//#define VIDIOC_TRY_ENCODER_CMD  _IOWR('V', 78, struct v4l2_encoder_cmd)
//
///* Experimental, meant for debugging, testing and internal use.
//   Only implemented if CONFIG_VIDEO_ADV_DEBUG is defined.
//   You must be root to use these ioctls. Never use these in applications! */
//#define	VIDIOC_DBG_S_REGISTER 	 _IOW('V', 79, struct v4l2_dbg_register)
//#define	VIDIOC_DBG_G_REGISTER 	_IOWR('V', 80, struct v4l2_dbg_register)
//
//#define VIDIOC_S_HW_FREQ_SEEK	 _IOW('V', 82, struct v4l2_hw_freq_seek)
//
//#define	VIDIOC_S_DV_TIMINGS	_IOWR('V', 87, struct v4l2_dv_timings)
//#define	VIDIOC_G_DV_TIMINGS	_IOWR('V', 88, struct v4l2_dv_timings)
//#define	VIDIOC_DQEVENT		 _IOR('V', 89, struct v4l2_event)
//#define	VIDIOC_SUBSCRIBE_EVENT	 _IOW('V', 90, struct v4l2_event_subscription)
//#define	VIDIOC_UNSUBSCRIBE_EVENT _IOW('V', 91, struct v4l2_event_subscription)
//
///* Experimental, the below two ioctls may change over the next couple of kernel
//   versions */
//#define VIDIOC_CREATE_BUFS	_IOWR('V', 92, struct v4l2_create_buffers)
//#define VIDIOC_PREPARE_BUF	_IOWR('V', 93, struct v4l2_buffer)
//
///* Experimental selection API */
//#define VIDIOC_G_SELECTION	_IOWR('V', 94, struct v4l2_selection)
//#define VIDIOC_S_SELECTION	_IOWR('V', 95, struct v4l2_selection)
//
///* Experimental, these two ioctls may change over the next couple of kernel
//   versions. */
//#define VIDIOC_DECODER_CMD	_IOWR('V', 96, struct v4l2_decoder_cmd)
//#define VIDIOC_TRY_DECODER_CMD	_IOWR('V', 97, struct v4l2_decoder_cmd)
//
///* Experimental, these three ioctls may change over the next couple of kernel
//   versions. */
//#define VIDIOC_ENUM_DV_TIMINGS  _IOWR('V', 98, struct v4l2_enum_dv_timings)
//#define VIDIOC_QUERY_DV_TIMINGS  _IOR('V', 99, struct v4l2_dv_timings)
//#define VIDIOC_DV_TIMINGS_CAP   _IOWR('V', 100, struct v4l2_dv_timings_cap)
//
///* Experimental, this ioctl may change over the next couple of kernel
//   versions. */
//#define VIDIOC_ENUM_FREQ_BANDS	_IOWR('V', 101, struct v4l2_frequency_band)
//
///* Experimental, meant for debugging, testing and internal use.
//   Never use these in applications! */
//#define VIDIOC_DBG_G_CHIP_INFO  _IOWR('V', 102, struct v4l2_dbg_chip_info)
//
///* Reminder: when adding new ioctls please add support for them to
//   drivers/media/video/v4l2-compat-ioctl32.c as well! */
//
//#define BASE_VIDIOC_PRIVATE	192		/* 192-255 are private */
//
//#endif /* __LINUX_VIDEODEV2_H */
// */

///**
//  * struct v4l2_capability - Describes V4L2 device caps returned by VIDIOC_QUERYCAP
//  *
//  * @driver:	   name of the driver module (e.g. "bttv")
//  * @card:	   name of the card (e.g. "Hauppauge WinTV")
//  * @bus_info:	   name of the bus (e.g. "PCI:" + pci_name(pci_dev) )
//  * @version:	   KERNEL_VERSION
//  * @capabilities: capabilities of the physical device as a whole
//  * @device_caps:  capabilities accessed via this particular device (node)
//  * @reserved:	   reserved fields for future extensions
//  */
//struct v4l2_capability {
//	__u8	driver[16];
//	__u8	card[32];
//	__u8	bus_info[32];
//	__u32   version;
//	__u32	capabilities;
//	__u32	device_caps;
//	__u32	reserved[3];
//};

#endif
