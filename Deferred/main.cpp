#define _IRR_STATIC_LIB_
#include <irrlicht.h>
#include <iostream>
#include <cstdio>
#include "../source/Irrlicht/COpenGLBuffer.h"
#include "../source/Irrlicht/COpenGLExtensionHandler.h"
#include "../ext/FullScreenTriangle/FullScreenTriangle.h"

using namespace irr;
using namespace core;



/**
We do cool stuff here, like make an event receiver to process input
**/
class MyEventReceiver : public IEventReceiver
{
public:

	MyEventReceiver()
	{
	}

	bool OnEvent(const SEvent& event)
	{
        if (event.EventType == irr::EET_KEY_INPUT_EVENT && !event.KeyInput.PressedDown)
        {
            switch (event.KeyInput.Key)
            {
            case irr::KEY_KEY_Q: // switch wire frame mode
                exit(0);
                return true;
            default:
                break;
            }
        }

		return false;
	}

private:
};

class SimpleCallBack : public video::IShaderConstantSetCallBack
{
    int32_t mvpUniformLocation;
    video::E_SHADER_CONSTANT_TYPE mvpUniformType;
public:
    SimpleCallBack() : mvpUniformLocation(-1), mvpUniformType(video::ESCT_FLOAT_VEC3) {}

    virtual void PostLink(video::IMaterialRendererServices* services, const video::E_MATERIAL_TYPE& materialType, const core::vector<video::SConstantLocationNamePair>& constants)
    {
        //! Normally we'd iterate through the array and check our actual constant names before mapping them to locations but oh well
        mvpUniformLocation = constants[0].location;
        mvpUniformType = constants[0].type;
    }

    virtual void OnSetConstants(video::IMaterialRendererServices* services, int32_t userData)
    {
        services->setShaderConstant(services->getVideoDriver()->getTransform(video::EPTS_PROJ_VIEW_WORLD).pointer(),mvpUniformLocation,mvpUniformType,1);
    }

    virtual void OnUnsetMaterial() {}
};


class SimpleCallBack2 : public video::IShaderConstantSetCallBack
{
	int32_t mvpUniformLocation;
	video::E_SHADER_CONSTANT_TYPE mvpUniformType;
public:
	SimpleCallBack2() : mvpUniformLocation(-1), mvpUniformType(video::ESCT_FLOAT_VEC3) {}

	virtual void PostLink(video::IMaterialRendererServices* services, const video::E_MATERIAL_TYPE& materialType, const core::vector<video::SConstantLocationNamePair>& constants)
	{
		for(const video::SConstantLocationNamePair &i : constants){
			std::cout << i.name.c_str() << std::endl;
			if(i.name == "inPV"){
				mvpUniformLocation = i.location;
				mvpUniformType = i.type;
			}
		}

	}

	virtual void OnSetConstants(video::IMaterialRendererServices* services, int32_t userData)
	{

		if(mvpUniformLocation != -1)
		services->setShaderConstant(services->getVideoDriver()->getTransform(video::EPTS_PROJ_VIEW_INVERSE).pointer(), mvpUniformLocation, mvpUniformType, 1);
	}

	virtual void OnUnsetMaterial() {}
};


struct LightData_t {
	core::vector3df_SIMD position;

};


int main()
{
	printf("Please select the background:\n");
	printf(" (0 : default) Use SkyDome\n");
	printf(" (1) Use SkyBox\n");

	uint8_t c = 0;
	std::cin >> c;


	

	// create device with full flexibility over creation parameters
	// you can add more parameters if desired, check irr::SIrrlichtCreationParameters
	irr::SIrrlichtCreationParameters params;
	params.Bits = 24; //may have to set to 32bit for some platforms
	params.ZBufferBits = 24; //we'd like 32bit here
	params.DriverType = video::EDT_OPENGL; //! Only Well functioning driver, software renderer left for sake of 2D image drawing
	params.WindowSize = dimension2d<uint32_t>(1280, 720);
	params.Fullscreen = false;
	params.Vsync = true; //! If supported by target platform
	params.Doublebuffer = true;
	params.Stencilbuffer = false; //! This will not even be a choice soon
	
	IrrlichtDevice* device = createDeviceEx(params);

	if (device == 0)
		return 1; // could not create selected driver.


	video::IVideoDriver* driver = device->getVideoDriver();

    SimpleCallBack* callBack = new SimpleCallBack();

    //! First need to make a material other than default to be able to draw with custom shader
    video::SGPUMaterial material;
    material.BackfaceCulling = false; //! Triangles will be visible from both sides
    material.MaterialType = (video::E_MATERIAL_TYPE)driver->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles("../mesh.vert",
                                                        "","","", //! No Geometry or Tessellation Shaders
                                                        "../mesh.frag",
                                                        3,video::EMT_SOLID, //! 3 vertices per primitive (this is tessellation shader relevant only
                                                        callBack,
                                                        0); //! No custom user data
    callBack->drop();
	SimpleCallBack2* callBack2 = new SimpleCallBack2();

	video::SGPUMaterial quad;
	quad.BackfaceCulling = false; //! Triangles will be visible from both sides
	quad.MaterialType = (video::E_MATERIAL_TYPE)driver->getGPUProgrammingServices()->addHighLevelShaderMaterialFromFiles("../quad.vert",
		"", "", "", //! No Geometry or Tessellation Shaders
		"../quad.frag",
		3, video::EMT_SOLID, //! 3 vertices per primitive (this is tessellation shader relevant only
		callBack2,
		0); //! No custom user data

	callBack2->drop();


	scene::ISceneManager* smgr = device->getSceneManager();




	driver->setTextureCreationFlag(video::ETCF_ALWAYS_32_BIT, true);
	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, false);


	driver->setTextureCreationFlag(video::ETCF_CREATE_MIP_MAPS, true);


	//! we want to move around the scene and view it from different angles
	scene::ICameraSceneNode* camera =
		smgr->addCameraSceneNode();//smgr->addCameraSceneNodeFPS(0, 100.0f, 0.001f);
	
	camera->setPosition(core::vector3df(-4, 4, -4));
	camera->setTarget(core::vector3df(0, 0, 0));
	camera->setNearValue(0.01f);
	camera->setFarValue(10.0f);

    smgr->setActiveCamera(camera);

	//! disable mouse cursor, since camera will force it to the middle
	//! and we don't want a jittery cursor in the middle distracting us
	device->getCursorControl()->setVisible(true);
	device->getCursorControl()->setActiveIcon(gui::ECURSOR_ICON::ECI_CROSS);


	irr::core::dimension2du size = driver->getScreenSize();


	uint32_t texSize[] = {size.Width, size.Height, 1};

	video::IFrameBuffer *fbo = driver->addFrameBuffer();
	video::ITexture *albedo = driver->createGPUTexture(video::ITexture::ETT_2D, texSize, 1u, irr::asset::EF_R8G8B8A8_UNORM);
	video::ITexture *normal = driver->createGPUTexture(video::ITexture::ETT_2D, texSize, 1u, irr::asset::EF_R16G16B16_SFLOAT);
	video::ITexture *depth = driver->createGPUTexture(video::ITexture::ETT_2D, texSize, 1u, asset::EF_D32_SFLOAT);


	fbo->attach(video::EFAP_COLOR_ATTACHMENT0, albedo);
	fbo->attach(video::EFAP_COLOR_ATTACHMENT1, normal);
	fbo->attach(video::EFAP_DEPTH_ATTACHMENT, depth);
	


	quad.setTexture(0, albedo);
	quad.setTexture(1, normal);
	quad.setTexture(2, depth);

	asset::ICPUMesh *cpuCube = device->getAssetManager().getGeometryCreator()
		->createCubeMesh(core::vector3df(1, 1, 1));
	asset::ICPUMeshBuffer *cpuCubeBuffer = cpuCube->getMeshBuffer(0);

	video::IGPUMeshBuffer *cubeBuffer = driver->getGPUObjectsFromAssets(&cpuCubeBuffer, (&cpuCubeBuffer)+1).front();

	video::IGPUBuffer *lightDataBuffer = driver->createDeviceLocalGPUBufferOnDedMem(sizeof(LightData_t));


	LightData_t light;
	light.position = core::vector3df_SIMD(-5.0f, 10.0f, 0.0f);

	driver->updateBufferRangeViaStagingBuffer(lightDataBuffer,
		0, sizeof(LightData_t), &light);


	video::IGPUMeshBuffer *screenBuf = ext::FullScreenTriangle::createFullScreenTriangle(driver);
	//! Since our cursor will be enslaved, there will be no way to close the window
	//! So we listen for the "Q" key being pressed and exit the application
	MyEventReceiver receiver;
	device->setEventReceiver(&receiver);

	uint64_t lastFPSTime = 0;
	while(device->run())
	//if (device->isWindowActive())
	{
		driver->beginScene(true, true, video::SColor(255,255,255,255) );

        //! This animates (moves) the camera and sets the transforms
        //! Also draws the meshbuffer
        smgr->drawAll();

        //! Stress test for memleaks aside from demo how to create meshes that live on the GPU RAM
        {
      

			//
			
			driver->setRenderTarget(fbo);

			float clear[] = {0.0f, 0.0f, 0.0f, 0.0f};
			driver->clearColorBuffer(video::E_FBO_ATTACHMENT_POINT::EFAP_COLOR_ATTACHMENT0, clear);
			driver->clearZBuffer();
			//
			driver->setTransform(video::E4X3TS_WORLD, core::matrix4x3());
			driver->setMaterial(material);
			driver->drawMeshBuffer(cubeBuffer);

			//
			driver->setRenderTarget(0);

			video::COpenGLExtensionHandler::extGlBindBuffersBase(
				GL_UNIFORM_BUFFER, 0, 1, 
				&static_cast<video::COpenGLBuffer*>(lightDataBuffer)
				->getOpenGLName()
			);

			driver->setMaterial(quad);
			driver->drawMeshBuffer(screenBuf);

		}

		driver->endScene();

		// display frames per second in window title
		uint64_t time = device->getTimer()->getRealTime();
		if (time-lastFPSTime > 1000)
		{
			std::wostringstream str;
			str << L"GPU Mesh Demo - Irrlicht Engine [" << driver->getName() << "] FPS:" << driver->getFPS() << " PrimitvesDrawn:" << driver->getPrimitiveCountDrawn();

			device->setWindowCaption(str.str().c_str());
			lastFPSTime = time;
		}
	}

    //create a screenshot
	video::IImage* screenshot = driver->createImage(asset::EF_B8G8R8A8_UNORM,params.WindowSize);
    glReadPixels(0,0, params.WindowSize.Width,params.WindowSize.Height, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, screenshot->getData());
    {
        // images are horizontally flipped, so we have to fix that here.
        uint8_t* pixels = (uint8_t*)screenshot->getData();

        const int32_t pitch=screenshot->getPitch();
        uint8_t* p2 = pixels + (params.WindowSize.Height - 1) * pitch;
        uint8_t* tmpBuffer = new uint8_t[pitch];
        for (uint32_t i=0; i < params.WindowSize.Height; i += 2)
        {
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

	device->drop();

	return 0;
}
