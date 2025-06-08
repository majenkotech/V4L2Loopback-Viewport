#include "register_types.h"

#include "V4L2Loopback_Viewport.h"

#include <gdextension_interface.h>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/godot.hpp>

using namespace godot;

void initialize_v4l2(ModuleInitializationLevel p_level) {
	printf("INIT CALLED\n");
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }

	GDREGISTER_RUNTIME_CLASS(V4L2LoopbackViewport);
}

void uninitialize_v4l2(ModuleInitializationLevel p_level) {
    if (p_level != MODULE_INITIALIZATION_LEVEL_SCENE) {
        return;
    }
}

extern "C" {
	// Initialization.
	GDExtensionBool GDE_EXPORT v4l2_init(GDExtensionInterfaceGetProcAddress p_get_proc_address, const GDExtensionClassLibraryPtr p_library, GDExtensionInitialization *r_initialization) {
		printf("STARTING\n\n");
	    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address, p_library, r_initialization);
	
	    init_obj.register_initializer(initialize_v4l2);
	    init_obj.register_terminator(uninitialize_v4l2);
	    init_obj.set_minimum_library_initialization_level(MODULE_INITIALIZATION_LEVEL_SCENE);
	
	    int r = init_obj.init();
		printf("%d\n", r);
		return r;
		
	}
}
