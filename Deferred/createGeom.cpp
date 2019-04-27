#include "createGeom.h"

namespace kosu{

#include "irr/irrunpack.h"
	struct QuadLayout {
		float v[2];
		uint8_t t[2];
		uint8_t useless[2]; 
	} PACK_STRUCT;
#include "irr/irrpack.h"

#include "irr/irrpack.h"
	struct DefaultLayout {
		float v[3]; 
		float n[3];
		uint8_t c[3]; 
		uint8_t useless[1]; 
	} PACK_STRUCT;
#include "irr/irrunpack.h"

	video::IGPUMeshBuffer* screenQuad(video::IVideoDriver* driver){
		QuadLayout vertexData[4];
		vertexData[0] = { -1.0f, 1.0f,  0, 255 };
		vertexData[1] = { -1.0f, -1.0f, 0,0 };
		vertexData[2] = { 1.0f, 1.0f,  255, 255 };
		vertexData[3] = { 1.0f, -1.0f, 255,0 };
		
		video::IGPUBuffer *buf = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(vertexData));
		driver->updateBufferRangeViaStagingBuffer(buf, 0, sizeof(vertexData), vertexData);

		video::IGPUMeshDataFormatDesc *desc = driver->createGPUMeshDataFormatDesc();
		video::IGPUMeshBuffer *mb = new video::IGPUMeshBuffer;
		mb->setMeshDataAndFormat(desc);
		desc->drop();

		desc->setVertexAttrBuffer(buf, asset::EVAI_ATTR0, asset::EF_R32G32_SFLOAT, sizeof(QuadLayout), 0);
		desc->setVertexAttrBuffer(buf, asset::EVAI_ATTR1, asset::EF_R8G8B8_UNORM, sizeof(QuadLayout), 8);
		

		mb->setIndexType(asset::EIT_UNKNOWN);
		mb->setPrimitiveType(asset::E_PRIMITIVE_TYPE::EPT_TRIANGLE_STRIP);
		mb->setIndexCount(4);
		
		buf->drop();
		return mb;
	}

	video::IGPUMeshBuffer* cube(video::IVideoDriver* driver){
	
		DefaultLayout vertexData[24];
		int i = 0;
		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, 0.5  }, { 0,0,-1 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, 0.5, 0.5  }, { 0,0,-1 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, -0.5, 0.5  }, { 0,0,-1 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, 0.5  }, { 0,0,-1 }, { 0, 255, 0 }};

		vertexData[i++] = DefaultLayout{ { 0.5, 0.5, 0.5   }, { 1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { 0.5, 0.5, -0.5  }, { 1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { 0.5, -0.5, -0.5  }, { 1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { 0.5, -0.5, 0.5   }, { 1,0,0 }, { 0, 255, 0 }};

		vertexData[i++] = DefaultLayout{ { 0.5, 0.5, -0.5 }, { 0,0,1 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, -0.5 }, { 0,0,1 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, -0.5 }, { 0,0,1 }, { 0, 255,0 }};
		vertexData[i++] = DefaultLayout{ { 0.5, -0.5, -0.5 }, { 0,0,1 }, { 0, 255, 0 }};

		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, -0.5 }, { -1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, 0.5  }, { -1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, 0.5  }, { -1,0,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, -0.5 }, { -1,0,0 }, { 0, 255, 0 }};

		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, -0.5	}, { 0,1,0 }, { 0,255,0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, 0.5, -0.5	}, { 0,1,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, 0.5, 0.5	}, { 0,1,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, 0.5, 0.5	}, { 0,1,0 }, { 0, 255, 0 }};

		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, 0.5	}, { 0,-1,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, -0.5, 0.5  }, { 0,-1,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ {  0.5, -0.5, -0.5  }, { 0,-1,0 }, { 0, 255, 0 }};
		vertexData[i++] = DefaultLayout{ { -0.5, -0.5, -0.5  }, { 0,-1,0 }, { 0, 255, 0 }};

		uint16_t indexData[] = {
		2,1,0,		3,2,0,
		6,5,4,		7,6,4,
		10,9,8,		11,10,8, 
		14,13,12,	15,14,12,
		18,17,16,	19,18,16,
		22,21,20,   23,22,20
		};


		video::IGPUBuffer* vb = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(vertexData));
		video::IGPUBuffer* ib = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(indexData));


		video::IGPUMeshDataFormatDesc* desc = driver->createGPUMeshDataFormatDesc();
		video::IGPUMeshBuffer* mb = new video::IGPUMeshBuffer;
		mb->setMeshDataAndFormat(desc);
		desc->drop();

		driver->updateBufferRangeViaStagingBuffer(vb, 0, sizeof(vertexData), vertexData);
		driver->updateBufferRangeViaStagingBuffer(ib, 0, sizeof(indexData), indexData);


		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR0, asset::EF_R32G32B32_SFLOAT, sizeof(DefaultLayout), 0);
		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR1, asset::EF_R32G32B32_SFLOAT, sizeof(DefaultLayout), 12);
		desc->setVertexAttrBuffer(vb, asset::EVAI_ATTR2, asset::EF_R8G8B8_UNORM, sizeof(DefaultLayout), 24);
		desc->setIndexBuffer(ib);

		mb->setIndexType(asset::EIT_16BIT);
		mb->setIndexCount(6 * 6);
		
		vb->drop();
		ib->drop();

		return mb;
	}

}