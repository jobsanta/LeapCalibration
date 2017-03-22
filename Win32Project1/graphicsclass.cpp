////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


using namespace SimpleMath;

GraphicsClass::GraphicsClass()
{
	m_Direct3D = 0;
	m_Camera = 0;
	m_boxes = 0;
	m_spheres = 0;
	m_3dvision = 0;
	m_physx = 0;
	m_Light = 0;
	m_LightShader = 0;
	m_ColorShader = 0;
	m_TextureShader = 0;
	m_Terrain = 0;
	m_Text = 0;
	m_Leap = 0;
	m_Bitmap = 0;
	m_RenderHand = true;
	m_Shape = 0;
	globalFactor = 1;
	m_Kinect = 0;
	score = -1;
	m_RenderTexture = 0;
	m_DepthShader = 0;
	m_ground = 0;
	m_groundShader = 0;

	m_BlackWhiteRenderTexture = 0;
	m_ShadowShader = 0;
	m_DownSampleTexure = 0;
	m_SmallWindow = 0;
	m_HorizontalBlurTexture = 0;
	m_HorizontalBlurShader = 0;
	m_VerticalBlurTexture = 0;
	m_VerticalBlurShader = 0;
	m_UpSampleTexure = 0;
	m_FullScreenWindow = 0;

	handlength = 150;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{

	bool result;
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;
	int downSampleWidth, downSampleHeight;

	// Create the Direct3D object.
	m_Direct3D = new D3DClass;
	if (!m_Direct3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_Direct3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Direct3D", L"Error", MB_OK);
		return false;
	}

	//Create the 3Dvision object
	m_3dvision = new Vision3DClass;
	if (!m_3dvision)
	{
		return false;
	}

	result = m_3dvision->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize 3D Vision", L"Error", MB_OK);
		return false;
	}

	m_physx = new PhysxClass;
	if (!m_physx)
	{
		return false;
	}
	result = m_physx->Initialize();
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize Physx ", L"Error", MB_OK);
		return false;
	}

	m_Leap = new LeapClass;
	if (!m_Leap)
	{
		return false;
	}
	m_Leap->connect();
	m_Leap->InitPhysx(m_physx->getPhysicsSDK(), m_physx->getScene());

	m_Kinect = new KinectClass;
	if (!m_Kinect)
	{
		return false;
	}
	m_Kinect->Initialize();

	m_Tracker = new TrackerClass();
	if (!m_Tracker)
	{
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	m_Camera->RenderBaseViewMatrix();



	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	m_Light->SetPosition(0.0f, 0.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f,1.0f,1.0f,1.0f);
	m_Light->SetAmbientColor(0.15,0.15,0.15f, 1.0f);
	m_Light->SetLookAt(0.0f, 1.0f, 0.0f);
	m_Light->GenerateProjectionMatrix(SCREEN_DEPTH, SCREEN_NEAR);

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if (!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the depth shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the render to texture object.
	m_BlackWhiteRenderTexture = new RenderTextureClass;
	if (!m_BlackWhiteRenderTexture)
	{
		return false;
	}

	// Initialize the black and white render to texture object.
	result = m_BlackWhiteRenderTexture->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the black and white render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if (!m_ShadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the shadow shader object.", L"Error", MB_OK);
		return false;
	}

	// Set the size to sample down to.
	downSampleWidth = SHADOWMAP_WIDTH / 2;
	downSampleHeight = SHADOWMAP_HEIGHT / 2;

	// Create the down sample render to texture object.
	m_DownSampleTexure = new RenderTextureClass;
	if (!m_DownSampleTexure)
	{
		return false;
	}

	// Initialize the down sample render to texture object.
	result = m_DownSampleTexure->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, 100.0f, 1.0f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the down sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the small ortho window object.
	m_SmallWindow = new OrthoWindowClass;
	if (!m_SmallWindow)
	{
		return false;
	}

	// Initialize the small ortho window object.
	result = m_SmallWindow->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the small ortho window object.", L"Error", MB_OK);
		return false;
	}
	// Create the horizontal blur render to texture object.
	m_HorizontalBlurTexture = new RenderTextureClass;
	if (!m_HorizontalBlurTexture)
	{
		return false;
	}

	// Initialize the horizontal blur render to texture object.
	result = m_HorizontalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the horizontal blur shader object.
	m_HorizontalBlurShader = new HorizontalBlurShaderClass;
	if (!m_HorizontalBlurShader)
	{
		return false;
	}

	// Initialize the horizontal blur shader object.
	result = m_HorizontalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the horizontal blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur render to texture object.
	m_VerticalBlurTexture = new RenderTextureClass;
	if (!m_VerticalBlurTexture)
	{
		return false;
	}

	// Initialize the vertical blur render to texture object.
	result = m_VerticalBlurTexture->Initialize(m_Direct3D->GetDevice(), downSampleWidth, downSampleHeight, SCREEN_DEPTH, 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the vertical blur shader object.
	m_VerticalBlurShader = new VerticalBlurShaderClass;
	if (!m_VerticalBlurShader)
	{
		return false;
	}

	// Initialize the vertical blur shader object.
	result = m_VerticalBlurShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the vertical blur shader object.", L"Error", MB_OK);
		return false;
	}

	// Create the up sample render to texture object.
	m_UpSampleTexure = new RenderTextureClass;
	if (!m_UpSampleTexure)
	{
		return false;
	}

	// Initialize the up sample render to texture object.
	result = m_UpSampleTexure->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, 0.1f);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the up sample render to texture object.", L"Error", MB_OK);
		return false;
	}

	// Create the full screen ortho window object.
	m_FullScreenWindow = new OrthoWindowClass;
	if (!m_FullScreenWindow)
	{
		return false;
	}

	// Initialize the full screen ortho window object.
	result = m_FullScreenWindow->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the full screen ortho window object.", L"Error", MB_OK);
		return false;
	}


	// Create the model object.
	m_boxes = new ModelClass;
	if (!m_boxes)
	{
		return false;
	}

	// Initialize the model object.
	result = m_boxes->Initialize(m_Direct3D->GetDevice(),"../Win32Project1/data/cube.txt", L"../Win32Project1/data/WoodCrate01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the boxes model object.", L"Error", MB_OK);
		return false;
	}
	m_Target_Texture = new TextureClass;
	if (!m_Target_Texture)
	{
		return false;
	}

	// Initialize the model object.
	result = m_Target_Texture->Initialize(m_Direct3D->GetDevice(), L"../Win32Project1/data/CrossWood.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}

	m_spheres = new ModelClass;
	if (!m_spheres)
	{
		return false;
	}

	// Initialize the model object.
	result = m_spheres->Initialize(m_Direct3D->GetDevice(), "../Win32Project1/data/sphere.txt", L"../Win32Project1/data/WoodCrate01.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the sphere model object.", L"Error", MB_OK);
		return false;
	}

	m_ground = new ModelClass;
	if (!m_ground)
	{
		return false;
	}

	// Initialize the ground model object.
	result = m_ground->Initialize(m_Direct3D->GetDevice(), "../Win32Project1/data/plane01.txt", L"../Win32Project1/data/seafloor.dds");
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground model object.", L"Error", MB_OK);
		return false;
	}


	m_Shape = new ShapeClass;
	if (!m_Shape)
	{
		return false;
	}
	result = m_Shape->Initialize(m_Direct3D->GetDevice());
	if(!result)
	{
		MessageBox(hwnd, L"Could not initialize the shape object.", L"Error", MB_OK);
		return false;
	}
	// Create the color shader object.
	m_ColorShader = new ColorShaderClass;
	if (!m_ColorShader)
	{
		return false;
	}

	// Initialize the color shader object.
	result = m_ColorShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the color shader object.", L"Error", MB_OK);
		return false;
	}

	m_LightShader = new LightShaderClass;
	if (!m_LightShader)
	{
		return false;
	}
	result = m_LightShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the Light shader object.", L"Error", MB_OK);
		return false;
	}

	m_groundShader = new GroundShaderClass;
	if (!m_groundShader)
	{
		return false;
	}
	result = m_groundShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the ground shader object.", L"Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader)
	{
		return false;
	}
	result = m_TextureShader->Initialize(m_Direct3D->GetDevice(), hwnd);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the texture shader object.", L"Error", MB_OK);
		return false;
	}
	m_Terrain = new TerrainClass;
	if (!m_Terrain)
	{
		return false;
	}
	result = m_Terrain->Initialize(m_Direct3D->GetDevice());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the terrain object.", L"Error", MB_OK);
		return false;
	}

	m_Text = new TextClass;
	if(!m_Text)
	{
		return false;
	}
	XMMATRIX baseView;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render(XMFLOAT3(0.0f,0.0f,0.0f));
	m_Camera->GetViewMatrix(baseView);
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, m_screenWidth, m_screenHeight, baseView);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.5f, -8.0f);


	//m_cylinder = GeometricPrimitive::CreateCylinder(m_Direct3D->GetDeviceContext(),1.0f,1.0f,32Ui64,false);
	//m_palm = GeometricPrimitive::CreateBox(m_Direct3D->GetDeviceContext(), XMFLOAT3(1.0f, 1.0f, 1.0f),false);
	spriteBatch = std::make_unique<SpriteBatch>(m_Direct3D->GetDeviceContext());
	m_font = std::make_unique<SpriteFont>(m_Direct3D->GetDevice(), L"../Win32Project1/data/myfile.spritefont");
	m_font_bold = std::make_unique<SpriteFont>(m_Direct3D->GetDevice(), L"../Win32Project1/data/myfileb.spritefont");

	//std::unique_ptr<GeometricPrimitive> shape;
	//shape = GeometricPrimitive::CreateTeapot(m_Direct3D->GetDeviceContext());
	//DGSLEffectFactory fx(m_Direct3D->GetDevice());
	//// Can also use EffectFactory, but will ignore pixel shader material settings

	//auto teapot = Model::CreateFromCMO(m_Direct3D->GetDevice(), L"teapot.cmo", fx);

	m_states = std::make_unique<CommonStates>(m_Direct3D->GetDevice());

	m_fxFactory = std::make_unique<EffectFactory>(m_Direct3D->GetDevice());

	m_model = Model::CreateFromSDKMESH(m_Direct3D->GetDevice(), L"teapot.sdkmesh", *m_fxFactory);

	m_world = SimpleMath::Matrix::Identity;

	m_view = SimpleMath::Matrix::CreateLookAt(Vector3(2.f, 2.f, 2.f),
		Vector3::Zero, Vector3::UnitY);
	m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
		float(1920) / float(1080), 0.1f, 10.f);

	globalZ = 0;
	globalFactor = 1;
	globalFOV = 21.239;

	ReadFile();
	StopCalibrate(false);

	
	return true;
}

void GraphicsClass::StopCalibrate(bool writeMode)
{
	for (int i = 0; i < spheres.size(); i++)
		m_physx->removeActor(spheres[i]);

	for (int i = 0; i < cylinders.size(); i++)
		m_physx->removeActor(cylinders[i]);

	for (int i = 0; i<targets.size(); i++)
		m_physx->removeActor(targets[i]);

	for (int i = 0; i<boxes.size(); i++)
		m_physx->removeActor(boxes[i]);

	for (int i = 0; i<obstacles.size(); i++)
		m_physx->removeActor(obstacles[i]);

	spheres.clear();
	boxes.clear();
	targets.clear();
	obstacles.clear();
	cylinders.clear();
	m_physx->clearTargetStatus();



	//for (int i = 0; i < 1; i++)
	//{
	//	PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0, i+2, -2), PxQuat::createIdentity());
	//	boxes.push_back(box);
	//}
	//for (int i = 0; i < 1; i++)
	//{
	//	PxRigidActor* sphere = m_physx->createSphere(SPHERE_RAD, PxVec3(i - 1.5f, 1, -1), PxQuat::createIdentity());
	//	spheres.push_back(sphere);
	//}
	//for (int i = 0; i < 1; i++)
	//{
	//	PxRigidActor* cylinder = m_physx->createCapsule(SPHERE_RAD, SPHERE_RAD, PxVec3(i - 1.0f, 1, -1), PxQuat::createIdentity());
	//	cylinders.push_back(cylinder);
	//}



	//PxRigidActor* folder = m_physx->createBox(BROAD_SIZE, PxVec3(0.0f, 1.0f, -1.5f), PxQuat::createIdentity());
	//folders.push_back(folder);

	if (fingertipDetected > 100)
	{
		m_Tracker->estimateAffineTransform();
		fingertipDetected = 0;
	}

	m_physx->moveBackWall(0);
	lastGameMode = gameMode;
	gameMode = 0;
	calibrateMode = false;
	targetHit = false;
	handColor = HANDCOLOR;
	WriteFile();
}

void GraphicsClass::StartGameMode(int mode)
{
	if (gameMode == 0)
	{
		gameMode = mode;
		calibrateMode = false;
		for (int i = 0; i < boxes.size(); i++)
		{
			m_physx->removeActor(boxes[i]);

		}
		for (int i = 0; i < spheres.size(); i++)
		{
			m_physx->removeActor(spheres[i]);
		}

		for (int i = 0; i < folders.size(); i++)
		{
			m_physx->removeActor(folders[i]);
		}

		boxes.clear();
		spheres.clear();
		folders.clear();
		score = 0;
		count = 0;

		if (gameMode == 1)
		{
			countDownTimer = 5;
			countFrame = 0;
			m_physx->moveBackWall(2.0f);
			m_physx->moveFloor(0.0f);
		}
		else if (gameMode == 2)
		{
			countDownTimer = 5000;
			m_physx->moveFrontWall(-4);
			m_physx->clearTargetStatus();
		}
		else if (gameMode == 3)
		{
			countDownTimer = 5000;
			m_physx->moveFrontWall(-4);
			for (int i = 0; i < 6; i++)
			{
				PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0, i + 2, -2), PxQuat::createIdentity());
				boxes.push_back(box);
			}
		}

		start = std::chrono::steady_clock::now();
		smallStart = std::chrono::steady_clock::now();
	}
}

void GraphicsClass::StartCalibrate()
{
	if (calibrateMode == false)
	{
		calibrateMode = true;
		for (int i = 0; i < boxes.size(); i++)
		{
			m_physx->removeActor(boxes[i]);

		}
		for (int i = 0; i < spheres.size(); i++)
		{
			m_physx->removeActor(spheres[i]);
		}
		for (int i = 0; i < cylinders.size(); i++)
		{
			m_physx->removeActor(cylinders[i]);
		}

		for (int i = 0; i < folders.size(); i++)
		{
			m_physx->removeActor(folders[i]);
		}

		boxes.clear();
		spheres.clear();
		folders.clear();
		cylinders.clear();
		frameCalibrated = 1;
		fingertipDetected = 0;
	}
}

void GraphicsClass::ChangeFOV(float value)
{
	if (calibrateMode)
	{
		globalFOV += value;
		m_Direct3D->ChangeFOV(globalFOV, m_screenWidth, m_screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
	}
}

void GraphicsClass::ChangeZvalue(float value)
{
	if (calibrateMode)
	{
		globalZ += value;

		//m_physx->moveBackWall(globalZ);
	}
}

void GraphicsClass::ChangeSize(float value)
{
	if (calibrateMode)
	{
		std::vector<handActor> handactor = m_Leap->getHandActor();
		if (handactor.size() == 1)
		{
			PxVec3 wristPos = m_Leap->getWristPosition(handactor[0].id);
			float hs = m_Tracker->handMeasurement(wristPos);
			if (hs > 50)
			{
				handlength = hs;
				m_Leap->clearHand();

				spriteBatch->Begin();

				std::wstring s = L"hand size: " + std::to_wstring(handlength);
				m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 300), Colors::Red);

				spriteBatch->End();

				m_Direct3D->TurnOffAlphaBlending();
				m_Direct3D->TurnZBufferOn();
				
			}
		}
	}
}

void GraphicsClass::RenderActor(int mode)
{
	for (int i = 0; i < boxes.size(); i++)
	{
		RenderTextureBox(mode,boxes[i],m_boxes->GetTexture(),BOX_SIZE.x, BOX_SIZE.y, BOX_SIZE.z);
	}
	for (int i = 0; i < spheres.size(); i++)
	{
		RenderSphere(mode, spheres[i], SPHERE_RAD);
	}

	for (int i = 0; i < spheres.size(); i++)
	{
		RenderSphere(mode, spheres[i], SPHERE_RAD);
	}

	for (int i = 0; i < cylinders.size(); i++)
	{
		RenderCylinder(mode, cylinders[i],XMFLOAT4(0,0.5,1.0,1.0), SPHERE_RAD*4, SPHERE_RAD*2);
	}



	for (int i = 0; i < folders.size(); i++)
	{
		RenderTextureBox(mode, folders[i], m_Target_Texture->GetTexture(), BROAD_SIZE.x, BROAD_SIZE.y, BROAD_SIZE.z);
	}

	for (int i = 0; i < targets.size(); i++)
	{
		RenderTextureBox(mode, targets[i], m_Target_Texture->GetTexture(), TARGET_SIZE.x, TARGET_SIZE.y, TARGET_SIZE.z );
	}

	for (int i = 0; i < obstacles.size(); i+=2)
	{
		RenderColorBox(mode, obstacles[i], targetColor, BARRIER_SIZE_X.x, BARRIER_SIZE_X.y, BARRIER_SIZE_X.z);
		RenderColorBox(mode, obstacles[i+1], targetColor, BARRIER_SIZE_Z.x, BARRIER_SIZE_Z.y, BARRIER_SIZE_Z.z);
	}
}

void GraphicsClass::RenderHand(int mode)
{
	mHandlist = m_Leap->getHandActor();

	if(mHandlist.size() != 0.0 && !FULL_SCREEN )
	m_Tracker->handVisualize(mHandlist);
	

	
	
	for (int i = 0; i< mHandlist.size(); i++)
	{
		
		for (int j = 0; j < 5; j++)
		{
			//for (int k = 0; k < 5; k++)
			//{
			//	Leap::Vector px = Leap::Vector( mHandlist[i].leapJointPosition[j * 4 + k].x, mHandlist[i].leapJointPosition[j * 4 + k].y, mHandlist[i].leapJointPosition[j * 4 + k].z);
			//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
			//}

			//RenderColorSphere(mode, mHandlist[i].finger_tip_achor[j], mHandlist[i].fingerWidth[j][3]/2, handColor);


			if (j == 0)
			{
				for (int k = 2; k < 4; k++)
					RenderCylinder(mode, mHandlist[i].finger_actor[j][k],handColor, mHandlist[i].halfHeight[j][k] * 2.0, mHandlist[i].fingerWidth[j][k]);

				RenderPalm(mode, mHandlist[i].finger_actor[j][1], handColor, mHandlist[i].halfHeight[j][1] * 2.0, mHandlist[i].fingerWidth[j][1], mHandlist[i].fingerWidth[j][1]/2);
			}
			else
			{
				for (int k = 1; k < 4; k++)
					RenderCylinder(mode, mHandlist[i].finger_actor[j][k], handColor, mHandlist[i].halfHeight[j][k] * 2.0, mHandlist[i].fingerWidth[j][k]);

				RenderPalm(mode, mHandlist[i].finger_actor[j][0], handColor, mHandlist[i].halfHeight[j][0] * 2.0, mHandlist[i].fingerWidth[j][0], mHandlist[i].fingerWidth[j][0]/2);

			}
		}


		//PxVec3 dimension = mHandlist[i].palmDimension;
		//RenderPalm(mHandlist[i].palm, dimension.x, dimension.y, dimension.z);
	}
}

void GraphicsClass::RenderTextureBox(int mode,PxRigidActor* box,
	ID3D11ShaderResourceView* texture, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width, height, depth);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_boxes->Render(m_Direct3D->GetDevice());

		if (mode ==0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_boxes->GetIndexCount(),0,0, mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{

			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_boxes->GetIndexCount(), 0,
				0, mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_boxes->GetIndexCount(), mat, viewMatrix, projectionMatrix, m_boxes->GetTexture(),
				m_UpSampleTexure->GetShaderResourceView(),m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Camera->GetPosition(),globalZ);
		}

		// Render the model using the color shader.

	}

	delete[] shapes;
}

void GraphicsClass::RenderColorBox(int mode,PxRigidActor* box,
	XMFLOAT4 color, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);



	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width*2, height*2, depth*2);
		mat = s*mat;

		m_Shape->Render(m_Direct3D->GetDevice());

		if (mode ==0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());
		}
		else
		{
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor()
				, color, m_Camera->GetPosition(),globalZ);
		}



	}

	delete[] shapes;
}

void GraphicsClass::RenderPalm(int mode,PxRigidActor* box, XMFLOAT4 color, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width, height, depth);
		XMMATRIX r = XMMatrixRotationX(90 * XM_PI / 180);
		mat = s*r*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.

		m_Shape->Render(m_Direct3D->GetDevice());
	
		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor()
				, color, m_Camera->GetPosition(),globalZ);
		}
	}

	delete[] shapes;
}

void GraphicsClass::RenderSphere(int mode,PxRigidActor* sphere, float radius)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	PxU32 nShapes = sphere->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	sphere->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);



	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *sphere);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(radius, radius, radius);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_spheres->Render(m_Direct3D->GetDevice());

		// Render the model using the color shader.
		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(),0,
				0, mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), 0,
				0, mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{
			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(),
				 mat, viewMatrix, projectionMatrix,  m_spheres->GetTexture(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(),
				m_Light->GetDiffuseColor() , m_Camera->GetPosition(),globalZ);
		}
	}

	delete[] shapes;
}

void GraphicsClass::RenderDebugSphere(int mode, PxVec3 pos, float radius, XMFLOAT4 color)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);



		XMMATRIX mat = XMMatrixTranslation(pos.x, pos.y, pos.z);
		XMMATRIX s = XMMatrixScaling(radius, radius, radius);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_spheres->Render(m_Direct3D->GetDevice());

		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), 0,
				0, mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), 0,
				0, mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{

			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(),
				mat, viewMatrix, projectionMatrix, m_spheres->GetTexture(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(),
				m_Light->GetDiffuseColor(), m_Camera->GetPosition(), globalZ);

		}
	


}

void GraphicsClass::RenderColorSphere(int mode, PxRigidActor* sphere, float radius, XMFLOAT4 color)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;

	PxU32 nShapes = sphere->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];
	sphere->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *sphere);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(radius, radius, radius);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_spheres->Render(m_Direct3D->GetDevice());

		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), 0,
				0, mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), 0,
				0, mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(),0,0,
				mat, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(),
				color, m_Camera->GetPosition(), globalZ);

		}
	}



}

void GraphicsClass::CreateBox()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0].palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.25), pose, PxQuat::createIdentity());
		boxes.push_back(box);
	}

}

void GraphicsClass::CreateSphere()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0].palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* sphere = m_physx->createSphere(0.25, pose, PxQuat::createIdentity());
		spheres.push_back(sphere);
	}
}

void GraphicsClass::CreateCylinder()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0].palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* sphere = m_physx->createCapsule(SPHERE_RAD,SPHERE_RAD, pose, PxQuat::createIdentity());
		cylinders.push_back(sphere);
	}
}

void GraphicsClass::RenderCylinder(int mode,PxRigidActor* cylinder, XMFLOAT4 color, float height, float radius)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	PxU32 nShapes = cylinder->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	cylinder->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *cylinder);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX r = XMMatrixRotationZ(90 * XM_PI / 180);
		XMMATRIX s = XMMatrixScaling(radius, height, radius);
		mat = s*r*mat;

		m_Shape->Render(m_Direct3D->GetDevice());

		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

		}
		else if (mode == 1)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions());

		}
		else
		{
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset(), mat, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor()
				, color, m_Camera->GetPosition(),globalZ);
		}

	}

	delete[] shapes;
}

void GraphicsClass::RenderTerrian(int mode =0)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	XMMATRIX trans2 = XMMatrixTranslation(0, m_physx->FLOOR_LEVEL - 0.26, -0.0f);
	XMMATRIX s = XMMatrixScaling(5, 0.5, 8);
	XMMATRIX trans = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -4.0f);
	XMMATRIX back = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -0.5);
	XMMATRIX rotate = XMMatrixRotationX(-XM_PI / 2);
	XMMATRIX slant = XMMatrixRotationX(187*XM_PI / 180);
	//XMMATRIX mat = rotat
	if (mode == 0 || mode == 2 || mode ==3)
	{

		m_Shape->Render(m_Direct3D->GetDevice());

		m_groundShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(),m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(), s*trans2, 
			viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_UpSampleTexure->GetShaderResourceView(),
			m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());


		m_Terrain->Render(m_Direct3D->GetDeviceContext());
	

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, rotate*back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
			m_Camera->GetPosition(), globalZ);

		m_Terrain->Render(m_Direct3D->GetDeviceContext());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
			m_Camera->GetPosition(),globalZ);

		m_Terrain->Render(m_Direct3D->GetDeviceContext());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, trans, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
			m_Camera->GetPosition(),globalZ);





	}
	else if (mode == 1)
	{
		trans = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -4.0f );

		m_Terrain->Render(m_Direct3D->GetDeviceContext());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, rotate*back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
			m_Camera->GetPosition(), globalZ);

		back = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -0.5 );

		m_Terrain->Render(m_Direct3D->GetDeviceContext());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f) , XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			m_Camera->GetPosition(), globalZ);

		m_Terrain->Render(m_Direct3D->GetDeviceContext());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, trans, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			m_Camera->GetPosition(), globalZ);

	}
	
}

XMMATRIX GraphicsClass::PxtoXMMatrix(PxTransform input)
{
	PxMat33 quat = PxMat33(input.q);
	XMFLOAT4X4 start;
	start._11 = quat.column0[0];
	start._12 = quat.column0[1];
	start._13 = quat.column0[2];
	start._14 = 0;


	start._21 = quat.column1[0];
	start._22 = quat.column1[1];
	start._23 = quat.column1[2];
	start._24 = 0;

	start._31 = quat.column2[0];
	start._32 = quat.column2[1];
	start._33 = quat.column2[2];
	start._34 = 0;

	start._41 = input.p.x;
	start._42 = input.p.y;
	start._43 = input.p.z;
	start._44 = 1;
	return XMLoadFloat4x4(&start);
}

void GraphicsClass::Shutdown()
{

	m_states.reset();
	m_fxFactory.reset();
	m_model.reset();
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
	}
	if (m_Text)
	{
		m_Text->Shutdown();
		delete m_Text;
		m_Text = 0;
	}

	if (m_groundShader)
	{
		m_groundShader->Shutdown();
		delete m_groundShader;
		m_groundShader;
	}

	// Release the full screen ortho window object.
	if (m_FullScreenWindow)
	{
		m_FullScreenWindow->Shutdown();
		delete m_FullScreenWindow;
		m_FullScreenWindow = 0;
	}

	// Release the up sample render to texture object.
	if (m_UpSampleTexure)
	{
		m_UpSampleTexure->Shutdown();
		delete m_UpSampleTexure;
		m_UpSampleTexure = 0;
	}

	// Release the vertical blur shader object.
	if (m_VerticalBlurShader)
	{
		m_VerticalBlurShader->Shutdown();
		delete m_VerticalBlurShader;
		m_VerticalBlurShader = 0;
	}

	// Release the vertical blur render to texture object.
	if (m_VerticalBlurTexture)
	{
		m_VerticalBlurTexture->Shutdown();
		delete m_VerticalBlurTexture;
		m_VerticalBlurTexture = 0;
	}

	// Release the horizontal blur shader object.
	if (m_HorizontalBlurShader)
	{
		m_HorizontalBlurShader->Shutdown();
		delete m_HorizontalBlurShader;
		m_HorizontalBlurShader = 0;
	}

	// Release the horizontal blur render to texture object.
	if (m_HorizontalBlurTexture)
	{
		m_HorizontalBlurTexture->Shutdown();
		delete m_HorizontalBlurTexture;
		m_HorizontalBlurTexture = 0;
	}

	// Release the texture shader object.
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	// Release the small ortho window object.
	if (m_SmallWindow)
	{
		m_SmallWindow->Shutdown();
		delete m_SmallWindow;
		m_SmallWindow = 0;
	}

	// Release the down sample render to texture object.
	if (m_DownSampleTexure)
	{
		m_DownSampleTexure->Shutdown();
		delete m_DownSampleTexure;
		m_DownSampleTexure = 0;
	}

	// Release the shadow shader object.
	if (m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// Release the black and white render to texture.
	if (m_BlackWhiteRenderTexture)
	{
		m_BlackWhiteRenderTexture->Shutdown();
		delete m_BlackWhiteRenderTexture;
		m_BlackWhiteRenderTexture = 0;
	}


	m_font.reset();

	if (m_Tracker)
	{
		delete m_Tracker;
		m_Tracker = 0;
	}

	if (m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}
	if (m_TextureShader)
	{
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// Release the render to texture object.
	if (m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}


	if (m_physx)
	{
		m_physx->Shutdown();
		delete m_physx;
		m_physx = 0;
	}
	if (m_Leap)
	{
		delete m_Leap;
		m_Leap = 0;
	}

	// Release the model object.
	if (m_boxes)
	{
		m_boxes->Shutdown();
		delete m_boxes;
		m_boxes = 0;
	}

	if (m_ground)
	{
		m_ground->Shutdown();
		delete m_ground;
		m_ground = 0;
	}

	if (m_Target_Texture)
	{
		m_Target_Texture->Shutdown();
		delete m_Target_Texture;
		m_Target_Texture = 0;
	}

	if (m_spheres)
	{
		m_spheres->Shutdown();
		delete m_spheres;
		m_spheres = 0;
	}

	// Release the camera object.
	if (m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	//Release the 3D vision object
	if (m_3dvision)
	{
		m_3dvision->Shutdown();
		delete m_3dvision;
		m_3dvision = 0;
	}

	// Release the Direct3D object.
	if (m_Direct3D)
	{
		m_Direct3D->Shutdown();
		delete m_Direct3D;
		m_Direct3D = 0;
	}

	if (m_Kinect)
	{
		m_Kinect->Shutdown();
		delete m_Kinect;
		m_Kinect = 0;
	}

	return;
}

bool GraphicsClass::Frame()
{

	bool result;

	// Update the position of the light.
	m_Light->SetPosition(0, 5.0f, -5.0f);
	// Update the position of the light.
	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

bool GraphicsClass::Render()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, othoMatrix;
	XMFLOAT4X4 proj;
	XMFLOAT4X4 view;
	bool result;
	HRESULT hr;
	Point3f* head_pos = NULL;
	bool Head_Error = false;

	

	
	hr = m_Kinect->Process();
	if (SUCCEEDED(hr))
	{
		hr = m_Tracker->Process(m_Kinect->getDepthWidth(), m_Kinect->getDepthHeight(), m_Kinect->getColorWidth(), m_Kinect->getColorHeight(),
			m_Kinect->getMinDistance(), m_Kinect->getMaxDistance(), m_Kinect->getDepthBuffer(), m_Kinect->getColorBuffer(), m_Kinect->getCoordinateMapper());

		if (SUCCEEDED(hr))
		{
			head_pos = m_Tracker->goggleDetection();
			if (calibrateMode)
			{
				mHandlist = m_Leap->getHandActor();

			}
		}
	}
	if (head_pos != NULL)
	{
		if (*head_pos == Point3f(0, 0, 0))
		{
			Head_Error = true;
		}
		else
		{
			m_Camera->SetPosition(head_pos->x, head_pos->y, head_pos->z);
			if (!CORRECT_PERPECTIVE)
			{
				//m_Light->SetPosition(head_pos->x, head_pos->y, head_pos->z);
				globalFOV = m_Direct3D->CalculateFOV(head_pos->x, head_pos->y, head_pos->z);
				m_Direct3D->ChangeFOV(globalFOV, m_screenWidth, m_screenHeight, SCREEN_NEAR, SCREEN_DEPTH);
			}
			else
			{
				XMFLOAT3 eyepos(head_pos->x, head_pos->y, head_pos->z);
				XMFLOAT3 left = XMFLOAT3(LEFT.x - head_pos->x, LEFT.y - head_pos->y, LEFT.z - head_pos->z);
				XMFLOAT3 right = XMFLOAT3(RIGHT.x - head_pos->x, RIGHT.y - head_pos->y, RIGHT.z - head_pos->z);
				XMFLOAT3 top = XMFLOAT3(TOP.x - head_pos->x, TOP.y - head_pos->y, TOP.z - head_pos->z);
				m_Direct3D->ChangeHeadPosition(XMFLOAT3(0, 0, 0), left, right, top, SCREEN_NEAR, SCREEN_DEPTH);
			}

			if (gameMode == 0|| gameMode ==1)
			m_physx->moveFrontWall(head_pos->z);
		}
	}

	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}

	 //Next render the shadowed scene in black and white.
	result = RenderBlackAndWhiteShadows();
	if (!result)
	{
		return false;
	}

	// Then down sample the black and white scene texture.
	result = DownSampleTexture();
	if (!result)
	{
		return false;
	}

	// Perform a horizontal blur on the down sampled texture.
	result = RenderHorizontalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// Now perform a vertical blur on the texture.
	result = RenderVerticalBlurToTexture();
	if (!result)
	{
		return false;
	}

	// Finally up sample the final blurred render to texture that can now be used in the soft shadow shader.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}



	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.5,0.5,0.5, 1.0f);
	
	float cameraDistance = m_Camera->GetPosition().x*m_Camera->GetPosition().x + (m_Camera->GetPosition().y-1.7)*(m_Camera->GetPosition().y-1.7) +
		(m_Camera->GetPosition().z)*(m_Camera->GetPosition().z);
	cameraDistance = sqrt(cameraDistance);

	m_3dvision->Render(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), cameraDistance);

	// Generate the view matrix based on the camera's position.
	if(CORRECT_PERPECTIVE)
	m_Camera->Render(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, -m_Camera->GetPosition().z));
	else
	m_Camera->Render(XMFLOAT3(0,1.5,0));

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(othoMatrix);

	XMStoreFloat4x4(&proj, projectionMatrix);
	XMStoreFloat4x4(&view, viewMatrix);


	m_Leap->processFrame(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 0, view, proj , handlength );
	m_physx->setHandActor(m_Leap->getHandActor());

	map<int, PxRigidDynamic*> contacts = m_physx->getActiveContact();
	if (contacts.size() > 0)
	{
		map<int, PxVec3> forces = m_Leap->computeForce(contacts);
		m_physx->applyForce(forces);
		if(gameMode ==1)
		targetHit = true;
	}


	m_physx->Render();
	
	if (gameMode)
	{
		GameFrame();
		if (targetHit)
		{
			if (gameMode == 1)
			{
				handColor = HANDCOLORHIT;
			}
			else if(gameMode ==2)
			{
				targetColor = TARGETCOLORHIT;
			}
		}
		else
		{
			if (gameMode == 1)
			{
				handColor = HANDCOLOR;
			}
			else if (gameMode == 2)
			{
				targetColor = TARGETCOLOR;
			}
		}
	}
	RenderTerrian(gameMode);




	if(m_RenderHand)
		RenderHand(2);

	RenderActor(2);


	RenderText(Head_Error,head_pos);

//	m_model->Draw(m_Direct3D->GetDeviceContext(), *m_states, m_world, m_view, m_proj);



	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}

void GraphicsClass::GameFrame()
{
	if (gameMode == 1)
	{
		end = std::chrono::steady_clock::now();
		long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (mils>1000)
		{
			start = end;
			countDownTimer--;
		}
		if (countDownTimer >= 0)
		{
			return;
		}



		for (int i = 0; i < spheres.size(); i++)
		{
			if (((PxRigidDynamic*)spheres[i])->getGlobalPose().p.y < m_physx->FLOOR_LEVEL || targetHit)
			{
				m_physx->removeActor(spheres[i]);
				spheres.clear();
				smallStart = std::chrono::steady_clock::now();
				if (targetHit)
				{
					m_physx->clearContact();
					score++;
				}
				
			}

		}

		if (spheres.size() == 0)
		{
			smallNow = std::chrono::steady_clock::now();
			long long mi = std::chrono::duration_cast<std::chrono::milliseconds>(smallNow - smallStart).count();
			if (mi > 500)
			{			
				targetHit = false;
				XMFLOAT3 pos = m_Camera->GetPosition();
				PxVec3 cameraPosition = PxVec3(pos.x, pos.y, pos.z);
				cameraPosition.y += 2 * rand() / RAND_MAX + 2;
				cameraPosition.x += 2 * rand() / RAND_MAX;
				cameraPosition.z /= 2;

				PxRigidDynamic* sphere = m_physx->createSphere(SPHERE_RAD, PxVec3(0, 1.6, 2), PxQuat::createIdentity());

				sphere->setLinearVelocity(cameraPosition);


				spheres.push_back(sphere);
				count++;
			}
		}

		if (count > 30)
		{
			count--;
			StopCalibrate(false);
		}
	}
	else if (gameMode == 2)
	{
		end = std::chrono::steady_clock::now();
		long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (mils>1000)
		{
			start = end;
			countDownTimer--;
		}
		if (countDownTimer == 0)
		{
			for (int i = 0; i<targets.size(); i++)
				m_physx->removeActor(targets[i]);

			for (int i = 0; i<boxes.size(); i++)
				m_physx->removeActor(boxes[i]);

			for (int i = 0; i<obstacles.size(); i++)
				m_physx->removeActor(obstacles[i]);

			boxes.clear();
			targets.clear();
			obstacles.clear();
			m_physx->clearTargetStatus();

			StopCalibrate(false);
		}

		if (m_physx->getTargetStatus())
		{
			smallStart = std::chrono::steady_clock::now();
			targetHit = true;
			m_physx->clearTargetStatus();

		}
		
		if (targetHit)
		{
			smallNow = std::chrono::steady_clock::now();
			long long mi = std::chrono::duration_cast<std::chrono::milliseconds>(smallNow - smallStart).count();
			if(mi >500)
			{
				for (int i = 0; i<targets.size(); i++)
					m_physx->removeActor(targets[i]);

				for (int i = 0; i<boxes.size(); i++)
					m_physx->removeActor(boxes[i]);

				for (int i = 0; i<obstacles.size(); i++)
					m_physx->removeActor(obstacles[i]);

				score++;
				boxes.clear();
				targets.clear();
				obstacles.clear();

				targetHit = false;
			}

		}


		if (boxes.size() == 0 && targets.size() == 0)
		{

			PxVec3 boxPosition;
			PxVec3 targetPostion;
			boxPosition.y = m_physx->FLOOR_LEVEL+0.25;
			boxPosition.x = 3 * rand() / RAND_MAX - 1.5;
			boxPosition.z = -3* rand() / RAND_MAX;
			float distance;
			do
			{
				targetPostion.y = m_physx->FLOOR_LEVEL+0.05;
				targetPostion.x = 2 * rand() / RAND_MAX - 1;
				targetPostion.z = -2 * rand() / RAND_MAX -1;
				distance = (targetPostion.x - boxPosition.x)*(targetPostion.x - boxPosition.x) + (targetPostion.z - boxPosition.z)*(targetPostion.z - boxPosition.z);
			} while (distance < 2);

			PxRigidDynamic* box = m_physx->createBox(BOX_SIZE, boxPosition, PxQuat::createIdentity());
			PxRigidActor* target = m_physx->createTarget(TARGET_SIZE, targetPostion, PxQuat::createIdentity());

			PxRigidActor* obs1 = m_physx->createBarrier(BARRIER_SIZE_X, PxVec3(targetPostion.x, targetPostion.y+BARRIER_SIZE_X.y, targetPostion.z+TARGET_SIZE.z+BARRIER_SIZE_X.z), PxQuat::createIdentity());
			PxRigidActor* obs3 = m_physx->createBarrier(BARRIER_SIZE_X, PxVec3(targetPostion.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z - TARGET_SIZE.z- BARRIER_SIZE_X.z), PxQuat::createIdentity());
			PxRigidActor* obs2 = m_physx->createBarrier(BARRIER_SIZE_Z, PxVec3(targetPostion.x + TARGET_SIZE.x + BARRIER_SIZE_Z.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z ), PxQuat::createIdentity());
			PxRigidActor* obs4 = m_physx->createBarrier(BARRIER_SIZE_Z, PxVec3(targetPostion.x - TARGET_SIZE.x - BARRIER_SIZE_Z.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z ), PxQuat::createIdentity());

			obstacles.push_back(obs1);
			obstacles.push_back(obs2);
			obstacles.push_back(obs3);
			obstacles.push_back(obs4);
			boxes.push_back(box);
			targets.push_back(target);
		}


	}
	else if (gameMode == 3)
	{
		end = std::chrono::steady_clock::now();
		long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (mils>1000)
		{
			start = end;
			countDownTimer--;
		}
		if (countDownTimer == 0)
		{
			StopCalibrate(false);
		}
	}
}

void GraphicsClass::RenderText(bool Head_Error, Point3f* head_pos)
{
	spriteBatch->Begin();
	std::vector<handActor> handactor = m_Leap->getHandActor();

	if (Head_Error)
	{
		m_font->DrawString(spriteBatch.get(), L"Multiple Red Detected!", XMFLOAT2(100, 100), Colors::Red);
	}
	if(head_pos !=nullptr)
	{
		std::wstring s = L"X: " + std::to_wstring(head_pos->x) + L"Y: " + std::to_wstring(head_pos->y) + L"Z: " + std::to_wstring(head_pos->z);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 100), Colors::Red);
	}
	if (calibrateMode)
	{
		if (handactor.size() > 0)
		{
			if (handactor[0].isExtended[0] && handactor[0].isExtended[1] && handactor[0].isExtended[2] && handactor[0].isExtended[3] && handactor[0].isExtended[4] && !FULL_SCREEN)
			{
				fingertipDetected = m_Tracker->fingerTipDetection(handactor[0].fingerTipPosition);
			}

			std::wstring s = L"hand size: " + std::to_wstring(handlength) /*+ L"T:" + std::to_wstring((int)handactor[0].isExtended[0])
				+ L"I:" + std::to_wstring((int)handactor[0].isExtended[1]) + L"M:" + std::to_wstring((int)handactor[0].isExtended[2])
				+ L"R:" + std::to_wstring((int)handactor[0].isExtended[3]) + L"P:" + std::to_wstring((int)handactor[0].isExtended[4]) */
				+ L"FingerTip Detected:" + std::to_wstring(fingertipDetected);
			m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::Red);
		}


	}


	if (gameMode == 0)
	{
		if (lastGameMode == 1)
		{
			std::wstring s = L"Score : " + std::to_wstring(score) + L" / " + std::to_wstring(count);
			m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);
		}
		else if (lastGameMode == 2)
		{
			std::wstring s = L"Timer : " + std::to_wstring(countDownTimer) + L" Score " + std::to_wstring(score);
			m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);
		}
		else if (lastGameMode == 3)
		{
			std::wstring s = L"Timer : " + std::to_wstring(countDownTimer);
			m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);
		}
	}

	else if (gameMode == 1)
	{
		if (countDownTimer > 0)
		{
			XMFLOAT2 m_fontPos;
			m_fontPos.x = m_screenWidth / 2.0f;
			m_fontPos.y = m_screenHeight / 2.0f;

			std::wstring s = std::to_wstring(countDownTimer);

			DirectX::SimpleMath::Vector2 origin = m_font->MeasureString(s.c_str()) / 2.f;

			m_font_bold->DrawString(spriteBatch.get(), s.c_str(),
				m_fontPos, Colors::LightGreen, 0.f);

		}
		if (countDownTimer == 0)
		{
			XMFLOAT2 m_fontPos;
			m_fontPos.x = m_screenWidth / 2.0f;
			m_fontPos.y = m_screenHeight / 2.0f;

			std::wstring s = L"GO !";

			DirectX::SimpleMath::Vector2 origin = m_font_bold->MeasureString(s.c_str()) / 2.f;

			m_font_bold->DrawString(spriteBatch.get(), s.c_str(),
				m_fontPos, Colors::LightGreen, 0.f);
		}
		std::wstring s = L"Score : " + std::to_wstring(score) + L" / " + std::to_wstring(count);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);
	}

	else if (gameMode == 2)
	{
		std::wstring s = L"Timer : " + std::to_wstring(countDownTimer) + L" Score " + std::to_wstring(score);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);

	}
	else if (gameMode == 3)
	{
		std::wstring s = L"Timer : " + std::to_wstring(countDownTimer);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);

	}


	spriteBatch->End();

	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();
}

bool GraphicsClass::RenderSceneToTexture()
{
	XMMATRIX worldMatrix, lightViewMatrix, lightProjectionMatrix, translateMatrix;
	float posX, posY, posZ;
	bool result;
	

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	//Generate the view matrix of the light.

		// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	//Render all the objects in the scene using the depth shader and the light view and projection matrices.


	// Render the cube model with the depth shader.
	//m_CubeModel->Render(m_Direct3D->GetDeviceContext());
	//result = m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_CubeModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightProjectionMatrix);
	//if (!result)
	//{
	//	return false;
	//}	

	XMMATRIX trans2 = XMMatrixTranslation(0, m_physx->FLOOR_LEVEL - 0.25, 0.0f);
	XMMATRIX s = XMMatrixScaling(5, 0.5, 8);

	m_Shape->Render(m_Direct3D->GetDevice());

	m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
		m_Shape->GetBoxVertexOffset(), s*trans2, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

	RenderHand(0);

	RenderActor(0);




	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderBlackAndWhiteShadows()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix, translateMatrix;
	XMMATRIX lightViewMatrix, lightProjectionMatrix;
	float posX, posY, posZ;
	bool result;


	// Set the render target to be the render to texture.
	m_BlackWhiteRenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	//m_Camera->Render();

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);


	XMMATRIX trans2 = XMMatrixTranslation(0, m_physx->FLOOR_LEVEL - 0.25, 0.0f);
	XMMATRIX s = XMMatrixScaling(5, 0.5, 8);

	m_Shape->Render(m_Direct3D->GetDevice());

	m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
		m_Shape->GetBoxVertexOffset(), s*trans2,viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(),m_Light->GetPositions());

	RenderHand(1);

	RenderActor(1);

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::DownSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;


	// Set the render target to be the render to texture.
	m_DownSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_DownSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(XMFLOAT3(0.0f, 1.5f, 0.0f));

	// Get the world and view matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetBaseViewMatrix(baseViewMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions being that it is smaller.
	m_DownSampleTexure->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the small ortho window using the texture shader and the render to texture of the scene as the texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_BlackWhiteRenderTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderHorizontalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	float screenSizeX;
	bool result;


	// Store the screen width in a float that will be used in the horizontal blur shader.
	screenSizeX = (float)(SHADOWMAP_WIDTH / 2);

	// Set the render target to be the render to texture.
	m_HorizontalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_HorizontalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(XMFLOAT3(0, 0, 0));

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_HorizontalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the small ortho window using the horizontal blur shader and the down sampled render to texture resource.
	result = m_HorizontalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_DownSampleTexure->GetShaderResourceView(), screenSizeX);
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderVerticalBlurToTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	float screenSizeY;
	bool result;


	// Store the screen height in a float that will be used in the vertical blur shader.
	screenSizeY = (float)(SHADOWMAP_HEIGHT / 2);

	// Set the render target to be the render to texture.
	m_VerticalBlurTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_VerticalBlurTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(XMFLOAT3(0,0,0));

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_VerticalBlurTexture->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the small ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_SmallWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the small ortho window using the vertical blur shader and the horizontal blurred render to texture resource.
	result = m_VerticalBlurShader->Render(m_Direct3D->GetDeviceContext(), m_SmallWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_HorizontalBlurTexture->GetShaderResourceView(), screenSizeY);
	if (!result)
	{
		return false;
	}


	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::UpSampleTexture()
{
	XMMATRIX worldMatrix, baseViewMatrix, orthoMatrix;
	bool result;


	// Set the render target to be the render to texture.
	m_UpSampleTexure->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	m_UpSampleTexure->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(XMFLOAT3(0, 0, 0));

	// Get the world and view matrices from the camera and d3d objects.
	m_Camera->GetBaseViewMatrix(baseViewMatrix);
	m_Direct3D->GetWorldMatrix(worldMatrix);

	// Get the ortho matrix from the render to texture since texture has different dimensions.
	m_UpSampleTexure->GetOrthoMatrix(orthoMatrix);

	// Turn off the Z buffer to begin all 2D rendering.
	m_Direct3D->TurnZBufferOff();

	// Put the full screen ortho window vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_FullScreenWindow->Render(m_Direct3D->GetDeviceContext());

	// Render the full screen ortho window using the texture shader and the small sized final blurred render to texture resource.
	result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
		m_VerticalBlurTexture->GetShaderResourceView());
	if (!result)
	{
		return false;
	}

	// Turn the Z buffer back on now that all 2D rendering has completed.
	m_Direct3D->TurnZBufferOn();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

void GraphicsClass::WriteFile()
{

	ofstream fout;
	fout.open("zdepth.txt");
	if (fout.is_open())
	{
		fout << handlength << " " << globalZ << endl;
		Mat affltok = m_Tracker->getAffineTransformLtoK();
		for (int i = 0; i < 3; i++)
			fout << affltok.at<double>(i, 0) << " " << affltok.at<double>(i, 1) << " " << affltok.at<double>(i, 2) << " " << affltok.at<double>(i, 3) << endl;

		Mat affktol = m_Tracker->getAffineTransformKtoL();
		for (int i = 0; i < 3; i++)
			fout << affktol.at<double>(i, 0) << " " << affktol.at<double>(i, 1) << " " << affktol.at<double>(i, 2) << " " << affktol.at<double>(i, 3) << endl;
		
		fout.close();
	}

}

void GraphicsClass::ReadFile()
{
	ifstream fin;
	fin.open("zdepth.txt");
	if (fin.is_open())
	{
		fin >> handlength >> globalZ;
		Mat affltok = cv::Mat(3, 4, CV_64F);
		Mat affktol = cv::Mat(3, 4, CV_64F);
		for (int i = 0; i < 3; i++)
			fin >> affltok.at<double>(i, 0) >> affltok.at<double>(i, 1) >> affltok.at<double>(i, 2) >> affltok.at<double>(i, 3);
		for (int i = 0; i < 3; i++)
			fin >> affktol.at<double>(i, 0) >> affktol.at<double>(i, 1) >> affktol.at<double>(i, 2) >> affktol.at<double>(i, 3);


		m_Tracker->setAffineLtoK(affltok);
		m_Tracker->setAffineKtoL(affktol);
		fin.close();
	}

	


}