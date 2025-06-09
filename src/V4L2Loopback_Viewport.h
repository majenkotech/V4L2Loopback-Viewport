#ifndef _V4L2LOOPBACK_VIEWPORT_H
#define _V4L2LOOPBACK_VIEWPORT_H

#include <godot_cpp/classes/sub_viewport.hpp>
#include <godot_cpp/variant/string.hpp>

#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>

namespace godot {
	class V4L2LoopbackViewport : public SubViewport {
		GDCLASS(V4L2LoopbackViewport, SubViewport)

		private:
			String _device;

			bool init_v4l2_device();


			int dev_fd;
			int frame_size;
			int num_pix;
			int bytes_per_pixel;

			uint8_t *_buffer;

		protected:
			static void _bind_methods();

		public:
			V4L2LoopbackViewport();
			~V4L2LoopbackViewport();

			void _set_device(String device);
			String _get_device();

			void _ready();

			void _process(float _delta);
			
			

	};
}

#endif
