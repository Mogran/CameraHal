
/**
 * @file    : camerahal.c
 * @brief   : 
 * @author  : <lumingliang@hjimi.cn>
 * @version : 1.0
 * @date    : 2018-05-10
 */

#include "camerahal.h"

/**
 * @brief   : camerahal_open 
 *
 * @param   : videonode
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_open(const char *videonode, camerahal_device_t * camera)
{
	int fd = 0;

	CAMERAHAL_ENTER();

	fd = open(videonode, O_RDWR, 0);
	if (fd < 0) {
		return CAMHAL_ENODEV;
	}

	camera->camerafd = fd;
	camera->buffers = NULL;

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_close 
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_close(camerahal_device_t * camera)
{
	int i = 0;

	CAMERAHAL_ENTER();

	close(camera->camerafd);

	if (NULL != camera->buffers) {
		for (i = 0; i < camera->req.count; i++) {
			munmap(camera->buffers[i].pstart, camera->buffers[i].length);
		}
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_get_capability 
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_get_capability(camerahal_device_t * camera)
{
	int ret = -1;

	ret = ioctl(camera->camerafd, VIDIOC_QUERYCAP, &camera->cap);
	if (ret < 0) {
		return CAMHAL_OTHER;
	}

	CAMERAHAL_DEBUG("Camera Cap info: \n"
					"   driver = %s \n"
					"   card = %s \n"
					"   bus_info = %s \n"
					"   version = %d \n"
					"   capabilities = %x \n"
					"   device_caps = %x",
					camera->cap.driver,
					camera->cap.card,
					camera->cap.bus_info,
					camera->cap.version,
					camera->cap.capabilities, camera->cap.device_caps);

	if (!(camera->cap.capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		return CAMHAL_OTHER;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_get_fmtdesc 
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_get_fmtdesc(camerahal_device_t * camera)
{
	camera->fmtdesc.index = 0;
	camera->fmtdesc.type = V4L2_CAP_VIDEO_CAPTURE;

	CAMERAHAL_ENTER();

	CAMERAHAL_DEBUG("Supported Capture FMT:");
	while (ioctl(camera->camerafd, VIDIOC_ENUM_FMT, &camera->fmtdesc) != -1) {
		CAMERAHAL_DEBUG("   %d.%s ", camera->fmtdesc.index + 1,
						camera->fmtdesc.description);
		camera->fmtdesc.index += 1;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_get_frame_format 
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_get_frame_format(camerahal_device_t * camera)
{
	int ret = -1;

	CAMERAHAL_ENTER();

	memset((void *) &camera->fmt, 0, sizeof (struct v4l2_format));

	camera->fmt.type = V4L2_CAP_VIDEO_CAPTURE;
	ret = ioctl(camera->camerafd, VIDIOC_G_FMT, &camera->fmt);
	if (ret < 0) {
		return CAMHAL_OTHER;
	}

	CAMERAHAL_DEBUG("Current frame format :\n"
					"   frame.pix.width = %d \n"
					"   frame.pix.hegiht = %d \n"
					"   frame.pix.pixelformat = %x \n"
					"   frame.pix.sizeimage = %d \n"
					"   frame.pix.colorspace = %d ",
					camera->fmt.fmt.pix.width,
					camera->fmt.fmt.pix.height,
					camera->fmt.fmt.pix.pixelformat,
					camera->fmt.fmt.pix.sizeimage,
					camera->fmt.fmt.pix.colorspace);

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_try_set_frame_format 
 *
 * @param   : camera
 * @param   : width
 * @param   : height
 * @param   : pixelformat
 *
 * @return  : 
 */
camerahal_error_t
camerahal_try_set_frame_format(camerahal_device_t * camera,
							   int width, int height, int pixelformat)
{

	int ret = -1;

	CAMERAHAL_ENTER();

	memset((void *) &camera->fmt, 0, sizeof (struct v4l2_format));

	camera->fmt.type = V4L2_CAP_VIDEO_CAPTURE;
	camera->fmt.fmt.pix.width = width;
	camera->fmt.fmt.pix.height = height;
	camera->fmt.fmt.pix.pixelformat = pixelformat;

	ret = ioctl(camera->camerafd, VIDIOC_TRY_FMT, &camera->fmt);
	if (ret < 0) {
		return CAMHAL_EINVAL;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_set_frame_format (设置帧传输的格式)
 *
 * @param   : camera
 * @param   : width (分辨率的宽度)
 * @param   : height(分辨率的高度)
 * @param   : pixelformat 像素存储格式，比如:V4L2_PIX_FMT_YUYV、V4L2_PIX_FMT_MJPEG
 *
 * @return  : 
 */
camerahal_error_t
camerahal_set_frame_format(camerahal_device_t * camera,
						   int width, int height, int pixelformat)
{
	int ret = -1;

	CAMERAHAL_ENTER();

	camera->fmt.type = V4L2_CAP_VIDEO_CAPTURE;
	camera->fmt.fmt.pix.width = width;
	camera->fmt.fmt.pix.height = height;
	camera->fmt.fmt.pix.pixelformat = pixelformat;
	ret = ioctl(camera->camerafd, VIDIOC_S_FMT, &camera->fmt);
	if (ret < 0) {
		return CAMHAL_OTHER;
	}

	CAMERAHAL_DEBUG("Set frame format :\n"
					"   frame.pix.width = %d \n"
					"   frame.pix.hegiht = %d \n"
					"   frame.pix.pixelformat = %x \n"
					"   frame.pix.sizeimage = %d \n"
					"   frame.pix.colorspace = %d ",
					camera->fmt.fmt.pix.width,
					camera->fmt.fmt.pix.height,
					camera->fmt.fmt.pix.pixelformat,
					camera->fmt.fmt.pix.sizeimage,
					camera->fmt.fmt.pix.colorspace);

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_requestbuffers (申请缓冲区存放几张照片)
 *
 * @param   : camera
 * @param   : count
 *
 * @return  : 
 */
camerahal_error_t
camerahal_requestbuffers(camerahal_device_t * camera, int count)
{
	int ret = -1, i = 0;
	struct v4l2_buffer buf;

	CAMERAHAL_ENTER();

	CLEAR_VAR(camera->req);
	camera->req.type = V4L2_CAP_VIDEO_CAPTURE;
	camera->req.count = count;
	camera->req.memory = V4L2_MEMORY_MMAP;

	ret = ioctl(camera->camerafd, VIDIOC_REQBUFS, &camera->req);
	if (ret < 0) {
		CAMERAHAL_DEBUG("request buffer failed\n");
		return CAMHAL_OTHER;
	}

	if (camera->req.count < 2) {
		CAMERAHAL_DEBUG("Insufficient memory\n");
		return CAMHAL_OTHER;
	}

	camera->buffers =
		(struct buffer *) calloc(camera->req.count, sizeof (*camera->buffers));
	if (camera->buffers == NULL)
		return CAMHAL_OTHER;

	for (i = 0; i < camera->req.count; i++) {

		CLEAR_VAR(buf);

		/* step1. QUERYBUF */
		buf.type = V4L2_CAP_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;
		ret = ioctl(camera->camerafd, VIDIOC_QUERYBUF, &buf);
		if (ret < 0) {
			CAMERAHAL_DEBUG("memory alloc failed \n");
			return CAMHAL_OTHER;
		}

		/* step2. mmap */
		camera->buffers[i].length = buf.length;
		camera->buffers[i].pstart = mmap(NULL,
										 buf.length,
										 PROT_READ | PROT_WRITE,
										 MAP_SHARED,
										 camera->camerafd, buf.m.offset);
		if (camera->buffers[i].pstart == NULL) {
			CAMERAHAL_DEBUG("memory mmap failed \n");
			return CAMHAL_OTHER;
		}

		/* step3. VIDIOC_QBUF */
		ret = ioctl(camera->camerafd, VIDIOC_QBUF, &buf);
		if (ret < 0) {
			CAMERAHAL_DEBUG("memory alloc failed \n");
			return CAMHAL_OTHER;
		}
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_stream_start 打开sensor捕捉图像
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_stream_start(camerahal_device_t * camera)
{
	int ret = -1;
	enum v4l2_buf_type type;

	CAMERAHAL_ENTER();

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(camera->camerafd, VIDIOC_STREAMON, &type);
	if (ret < 0) {
		CAMERAHAL_DEBUG("stream on failed \n");
		return CAMHAL_OTHER;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_stream_stop 关闭捕捉视频流
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_stream_stop(camerahal_device_t * camera)
{
	int ret = -1;
	enum v4l2_buf_type type;

	CAMERAHAL_ENTER();

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	ret = ioctl(camera->camerafd, VIDIOC_STREAMOFF, &type);
	if (ret < 0) {
		CAMERAHAL_DEBUG("stream close failed \n");
		return CAMHAL_OTHER;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}

/**
 * @brief   : camerahal_get_streaming 获得一张图片
 *
 * @param   : camera
 *
 * @return  : 
 */
camerahal_error_t
camerahal_get_streaming(camerahal_device_t * camera)
{
	int ret = -1;
	struct v4l2_buffer buf;
#ifdef SAVE_PICTURE
	FILE *fp = fopen("/home/lml/work/cameraHal/pic.raw", "w+");
#endif

	CAMERAHAL_ENTER();

	CLEAR_VAR(buf);
	buf.type = V4L2_CAP_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	ret = ioctl(camera->camerafd, VIDIOC_DQBUF, &buf);
	if (ret < 0) {
		CAMERAHAL_DEBUG("get stream DQBUF failed \n");
		return CAMHAL_OTHER;
	}
#ifdef SAVE_PICTURE
	if (NULL == fp) {
		CAMERAHAL_DEBUG("get stream failed \n");
		return CAMHAL_OTHER;
	}

	fwrite(camera->buffers[buf.index].pstart, 1, buf.length, fp);
	fflush(fp);
	fclose(fp);
#endif

	ret = ioctl(camera->camerafd, VIDIOC_QBUF, &buf);
	if (ret < 0) {
		CAMERAHAL_DEBUG("get stream QBUF failed \n");
		return CAMHAL_OTHER;
	}

	CAMERAHAL_EXIT(CAMHAL_SUCCESS);

	return CAMHAL_SUCCESS;
}




