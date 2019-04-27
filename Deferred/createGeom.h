#pragma once
#define _IRR_STATIC_LIB_
#include <irrlicht.h>

namespace kosu{
	using namespace irr;
	video::IGPUMeshBuffer* screenQuad(video::IVideoDriver* driver);
	video::IGPUMeshBuffer* cube(video::IVideoDriver* driver);
};