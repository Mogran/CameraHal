#include "camerahal.h"

camerahal_device_t camera;

static const char device[] = { "/dev/video0" };

int
main(int argc, char *argv[])
{
	camerahal_error_t err;

	printf("CameraHal Example \n");

	if (argc < 0) {
		printf("[Usage] : ./example [Options]\n "
			   "   [Options] may be /dev/videoX\n"
			   "                    X may 0,1,2,3,...\n");
		return -1;
	}
	//device open
	err = camerahal_open(device, &camera);
	if (err != CAMHAL_SUCCESS)
		printf("open  video failed\n");
	else
		printf("open  camera video success \n");

	//get device cap
	err = camerahal_get_capability(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, get cap failed\n");
	}
	//get video fmtdesc
	err = camerahal_get_fmtdesc(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, get cap failed\n");
	}
	//get current frame format info
	err = camerahal_get_frame_format(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, get current frame format failed\n");
	}
	//set frame 640x480
	err = camerahal_set_frame_format(&camera, 640, 480, V4L2_PIX_FMT_MJPEG);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, get current frame format failed\n");
	}
	//get current frame format info
	err = camerahal_get_frame_format(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, get current frame format failed\n");
	}
	//requeset buffer
	err = camerahal_requestbuffers(&camera, 4);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, requeset buffers failed\n");
	}
	//stream begin to caputre
	err = camerahal_stream_start(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, stream open failed\n");
	}
	//get picture frame one by one 
	int count = 5;
	while (count--) {
		err = camerahal_get_streaming(&camera);
		if (err != CAMHAL_SUCCESS) {
			printf("camera, get one picture failed\n");
		}
		sleep(1);
	}
	//sensor stop caputre
	err = camerahal_stream_stop(&camera);
	if (err != CAMHAL_SUCCESS) {
		printf("camera, stream open failed\n");
	}
	//close device
	err = camerahal_close(&camera);
	if (err != CAMHAL_SUCCESS)
		printf("close video failed\n");
	else
		printf("close camera video success \n");

	return 0;
}
