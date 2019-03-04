#define _IRR_STATIC_LIB_
#include <irrlicht.h>
#include "../source/Irrlicht/COpenGLExtensionHandler.h"


const char *vS = R"(
#version 430 core
layout(location = 0) in vec4 pos;
layout(location = 1) in vec2 texcoords;

out vec2 texCoords;        

void main(){
    gl_Position = pos;
    texCoords = texcoords;
}
)";

const char *fS = R"(
#version 430 core

layout(binding = 0) uniform sampler2D tex0;
layout(location = 0) out vec4 color;

in vec2 texCoords;


void main(){
    color = texture(tex0, texCoords);
}
)";



using namespace irr;

class TestReceiver : public IEventReceiver{
public:
    TestReceiver() {}

    bool OnEvent(const SEvent &e) {
        switch (e.KeyInput.Key) {
        case KEY_KEY_Q:
            exit(0);
            return true;
        }
        return false;
    }
};



#include "irr/irrunpack.h"
struct VertexLayout{
    float pos[3];
    uint8_t texcoord[2];
    uint8_t useless[2]; //float size plz
} PACK_STRUCT;
#include "irr/irrpack.h"


video::IGPUMeshBuffer *createTestQuad(video::IVideoDriver *driver) {
    VertexLayout vertexData[4];
    vertexData[0] = { 0.25f, 0.25f,  0.0f, 255, 255 };
    vertexData[1] = {  0.25f, -0.25f,  0.0f, 255, 0 };
    vertexData[2] = {  -0.25f, -0.25f, 0.0f, 0, 0 };
    vertexData[3] = { -0.25f, 0.25f, 0.0f, 0, 255 };

    uint16_t indexData[] = { 0,1,2, 0,2,3 };

    auto upBuf = driver->getDefaultUpStreamingBuffer();
    const void *dataToPlace[] = { vertexData, indexData };
    uint32_t offsets[2] = { video::StreamingTransientDataBufferMT<>::invalid_address, 
                            video::StreamingTransientDataBufferMT<>::invalid_address };
    uint32_t alignments[2] = { sizeof(decltype(vertexData[0u])), 
                              sizeof(decltype(indexData[0u]))};
    uint32_t sizes[2] = { sizeof(vertexData), sizeof(indexData) };

    upBuf->multi_place(2u, (const void* const*)dataToPlace, (uint32_t*)offsets, (uint32_t*)sizes, (uint32_t*)alignments);
    if (upBuf->needsManualFlushOrInvalidate())
    {
        auto upStreamMem = upBuf->getBuffer()->getBoundMemory();
        driver->flushMappedMemoryRanges({ video::IDriverMemoryAllocation::MappedMemoryRange(upStreamMem,offsets[0],sizes[0]),video::IDriverMemoryAllocation::MappedMemoryRange(upStreamMem,offsets[1],sizes[1]) });
    }


    auto buf = upBuf->getBuffer();
    video::IGPUMeshDataFormatDesc* desc = driver->createGPUMeshDataFormatDesc();
    desc->setVertexAttrBuffer(buf, asset::EVAI_ATTR0, asset::EF_R32G32B32_SFLOAT, sizeof(VertexLayout), offsetof(VertexLayout, pos[0]) + offsets[0]);
    desc->setVertexAttrBuffer(buf, asset::EVAI_ATTR1, asset::EF_R8G8_UNORM, sizeof(VertexLayout), offsetof(VertexLayout, texcoord[0]) + offsets[0]);
    desc->setIndexBuffer(buf);

    video::IGPUMeshBuffer *mb = new video::IGPUMeshBuffer();
    
    mb->setMeshDataAndFormat(desc);
    mb->setIndexBufferOffset(offsets[1]);
    mb->setIndexType(asset::EIT_16BIT);
    mb->setIndexCount(6);
    desc->drop();

    return mb;
}

int main(){
	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.Bits = 24; //may have to set to 32bit for some platforms
	params.ZBufferBits = 24; //we'd like 32bit here
	params.DriverType = video::EDT_OPENGL; //! Only Well functioning driver, software renderer left for sake of 2D image drawing
	params.WindowSize = core::dimension2d<uint32_t>(1280, 720);
	params.Fullscreen = false;
	params.Vsync = true; //! If supported by target platform
	params.Doublebuffer = true;
	params.Stencilbuffer = false; //! This will not even be a choice soon
	params.AuxGLContexts = 16;
	IrrlichtDevice* device = createDeviceEx(params);



	if (device == 0)
		return 1; // could not create selected driver.
    
    TestReceiver eventMgr;
    device->setEventReceiver(&eventMgr);
   

	video::IVideoDriver* driver = device->getVideoDriver();



    video::SGPUMaterial material;
    material.MaterialType = (video::E_MATERIAL_TYPE)driver->getGPUProgrammingServices()->addHighLevelShaderMaterial(
        vS, nullptr, nullptr, nullptr, fS, 3, video::EMT_SOLID, 0, 0);
    material.BackfaceCulling = false;


    video::IGPUMeshBuffer *mb = createTestQuad(driver);

    asset::IAssetLoader::SAssetLoadParams lparams;
    asset::ICPUTexture *cpuTex = static_cast<asset::ICPUTexture*>(device->getAssetManager().getAsset("wall.jpg", lparams));
    

    video::ITexture *tex = driver->getGPUObjectsFromAssets(&cpuTex, (&cpuTex) + 1).front();
    

    material.setTexture(0, tex);
    

	scene::ISceneManager* smgr = device->getSceneManager();

	uint64_t lastFPSTime = 0;

	while(device->run())
	if (device->isWindowActive()){
		driver->beginScene(true, true, video::SColor(0,0,0,0) ); //this gets 11k FPS

        driver->setTransform(video::E4X3TS_WORLD, core::matrix4x3());
        driver->setMaterial(material);
        driver->drawMeshBuffer(mb);

		driver->endScene();

		// display frames per second in window title
		uint64_t time = device->getTimer()->getRealTime();
		if (time-lastFPSTime > 1000){
		    std::wostringstream str(L"Hello World - Irrlicht Engine [");
		    str.seekp(0,std::ios_base::end);
            str << driver->getName() << "] FPS:" << driver->getFPS() << " PrimitivesDrawn: " << driver->getPrimitiveCountDrawn();
            
			device->setWindowCaption(str.str());
			lastFPSTime = time;
		}
	}

    //create a screenshot
	video::IImage* screenshot = driver->createImage(asset::EF_B8G8R8A8_UNORM,params.WindowSize);
    glReadPixels(0,0, params.WindowSize.Width,params.WindowSize.Height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, screenshot->getData());{
        // images are horizontally flipped, so we have to fix that here.
        uint8_t* pixels = (uint8_t*)screenshot->getData();

        const int32_t pitch=screenshot->getPitch();
        uint8_t* p2 = pixels + (params.WindowSize.Height - 1) * pitch;
        uint8_t* tmpBuffer = new uint8_t[pitch];
        for (uint32_t i=0; i < params.WindowSize.Height; i += 2){
            memcpy(tmpBuffer, pixels, pitch);
            memcpy(pixels, p2, pitch);
            memcpy(p2, tmpBuffer, pitch);
            pixels += pitch;
            p2 -= pitch;
        }
        delete [] tmpBuffer;
    }
    asset::CImageData* img = new asset::CImageData(screenshot);
    asset::IAssetWriter::SAssetWriteParams wparams(img);
    device->getAssetManager().writeAsset("screenshot.png", wparams);
    img->drop();
	screenshot->drop();

    mb->drop();

	device->drop();

	return 0;
}
