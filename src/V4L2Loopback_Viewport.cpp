#include "V4L2Loopback_Viewport.h"

#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/viewport_texture.hpp>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

using namespace godot;

V4L2LoopbackViewport::V4L2LoopbackViewport() {
	_buffer = NULL;
	frame_size = 0;
	dev_fd = -1;
}

V4L2LoopbackViewport::~V4L2LoopbackViewport() {
	if (_buffer) {
		bzero(_buffer, frame_size);
		if (dev_fd > 0) {
			write(dev_fd, _buffer, frame_size);
		}
		::free(_buffer);
	}
}

void V4L2LoopbackViewport::_set_device(String dev) {
	_device = dev;
}

String V4L2LoopbackViewport::_get_device() {
	return _device;
}

void V4L2LoopbackViewport::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_device", "device"), &V4L2LoopbackViewport::_set_device);
	ClassDB::bind_method(D_METHOD("get_device"), &V4L2LoopbackViewport::_get_device);
	ClassDB::add_property("V4L2LoopbackViewport", 
		PropertyInfo(
			Variant::STRING,
			"device"
		),
		"set_device",
		"get_device"
	);
}

bool V4L2LoopbackViewport::init_v4l2_device() {
	dev_fd = open(_device.ascii(), O_RDWR);

	if (!dev_fd) {
		printf("Unable to open video device: %s\n", strerror(errno));
		return false;
	}


	struct v4l2_capability vid_caps;
	struct v4l2_format vid_format;


	int ret_code = 0;
	ret_code = ioctl(dev_fd, VIDIOC_QUERYCAP, &vid_caps);

	if (ret_code < 0) {
		printf("Error querying caps: %s\n", strerror(errno));
		return false;
	}

	memset(&vid_format, 0, sizeof(vid_format));

	Vector2i vp_size = get_size();

	num_pix = vp_size.x * vp_size.y;
	bytes_per_pixel = 3;
	if (has_transparent_background()) {
		bytes_per_pixel = 4;
	}

	frame_size = num_pix * bytes_per_pixel;

	vid_format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	vid_format.fmt.pix.width = vp_size.x;
	vid_format.fmt.pix.height = vp_size.y;
    vid_format.fmt.pix.sizeimage = frame_size;
	if (bytes_per_pixel == 4) {
		vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_ABGR32;
	} else {
		vid_format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	}
	vid_format.fmt.pix.field = V4L2_FIELD_NONE;
    vid_format.fmt.pix.bytesperline = vp_size.x * bytes_per_pixel;
	vid_format.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

	ret_code = ioctl(dev_fd, VIDIOC_S_FMT, &vid_format);

	if (ret_code < 0) {
		printf("Error setting format: %s\n", strerror(errno));
		return false;
	}

	_buffer = (uint8_t *)malloc(frame_size);

	return true;
}

void V4L2LoopbackViewport::_ready() {
	init_v4l2_device();
}

void V4L2LoopbackViewport::_process(float _delta) {
	Ref<ViewportTexture> texture = get_texture();
	Ref<Image> image = texture->get_image();

	const uint8_t *data = image->get_data().ptr();

	if (bytes_per_pixel == 4) {
		for (int i = 0; i < frame_size; i+=4) {
			_buffer[i + 0] = data[i + 2];
			_buffer[i + 1] = data[i + 1];
			_buffer[i + 2] = data[i + 0];
			_buffer[i + 3] = data[i + 3];
		}
	} else {
		for (int i = 0; i < frame_size; i+=3) {
			_buffer[i + 0] = data[i + 2];
			_buffer[i + 1] = data[i + 1];
			_buffer[i + 2] = data[i + 0];
		}
	}

	write(dev_fd, _buffer, frame_size);
}
