////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"

//using namespace SimpleMath;

GraphicsClass::GraphicsClass()
{

	showContent = false;
	POINT_CLOUD_ENABLED = false;
	m_Direct3D = 0;
	m_Camera = 0;
	m_RemoteCamera = 0;
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
	m_RenderMirrorHand = true;
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
	m_PointCloudShader = 0;
	m_UpSampleTexure = 0;
	m_FullScreenWindow = 0;
	m_RenderTexture2 = 0;

	handlength = 150;
	handMode = 0;

	rgbDest = new FLOAT[512 * 424 * 4];
	depthDest = new FLOAT[512 * 424 * 3];
	distDest = new FLOAT[512 * 424 * 3];

	for (int i = 0; i < 512 * 424 * 3; i++)
	{
			distDest[i] = 0;
	}


	mhandMaterial.Ambient = XMFLOAT4(0.0f, 0.7, 0.3, 1.0);
	mhandMaterial.Diffuse = XMFLOAT4(0.0f, 0.7, 0.3, 1.0);
	mhandMaterial.Specular = XMFLOAT4(0.5, 0.5, 0.5, 16.0);

	mrHandMaterial.Ambient = XMFLOAT4(0.0f, 0.3, 0.7, 1.0);
	mrHandMaterial.Diffuse = XMFLOAT4(0.0f, 0.3, 0.7, 1.0);
	mrHandMaterial.Specular = XMFLOAT4(0.5, 0.5, 0.5, 16.0);

	mboxMaterial.Ambient = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mboxMaterial.Diffuse = XMFLOAT4(0.137f, 0.42f, 0.556f, 1.0f);
	mboxMaterial.Specular = XMFLOAT4(1.0f, 1.0f, 1.0f, 96.0f);


	msphereMaterial.Ambient = XMFLOAT4(0.7, 0.4, 0.46f, 1.0f);
	msphereMaterial.Diffuse = XMFLOAT4(0.7, 0.4, 0.46f, 1.0f);
	msphereMaterial.Specular = XMFLOAT4(0.8f, 0.8f, 0.8f, 96.0f);

	mfloorMaterial.Ambient = XMFLOAT4(0.8, 0.8, 0.8, 1.0f);
	mfloorMaterial.Diffuse = XMFLOAT4(0.8, 0.8, 0.8, 0.5);
	mfloorMaterial.Specular = XMFLOAT4(1.0, 1.0, 1.0, 96.0f);

	medgeMaterial.Ambient = XMFLOAT4(0.5, 0.25, 0.25, 1.0f);
	medgeMaterial.Diffuse = XMFLOAT4(0.5, 0.25, 0.25, 1.0f);
	medgeMaterial.Specular = XMFLOAT4(1.0, 1.0, 1.0, 96.0f);


	mtextureMaterial.Ambient = XMFLOAT4(0.75, 0.75, 0.75, 1.0f);
	mtextureMaterial.Diffuse = XMFLOAT4(0.75, 0.75, 0.75, 1.0f);
	mtextureMaterial.Specular = XMFLOAT4(1.0, 1.0, 1.0, 96.0f);

	mtextMaterial.Ambient = XMFLOAT4(1.0, 1.0, 1.0, 1.0f);
	mtextMaterial.Diffuse = XMFLOAT4(1.0, 1.0, 1.0, 0.5f);
	mtextMaterial.Specular = XMFLOAT4(1.0, 1.0, 1.0, 96.0f);


	attenuate = XMFLOAT3(0, 0.05, 0);
	lightRange = 40;


	gogoMode = false;

	pickActor = NULL;
	globe = NULL;

}

GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}

GraphicsClass::~GraphicsClass()
{
}

void GraphicsClass::ReadPath()
{
	ifstream fin;
	fin.open("path.txt");
	if (fin.is_open())
	{
		for (int i = 0; i < 50;i++)
		{
			float x, y, z, xa, ya, za, theta;
			fin >> x >> y >> z >> xa >> ya >> za >> theta;
			PxVec3 u = PxVec3(xa, ya, za);
			u = u.getNormalized();

			boxEndPosition[i] = PxTransform(PxVec3(x, y, z), PxQuat(theta, u));
		}

		fin.close();

	}
}

void GraphicsClass::setPointCloud()
{
	POINT_CLOUD_ENABLED = !POINT_CLOUD_ENABLED;
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


	// Create the camera object.
	m_RemoteCamera = new CameraClass;
	if (!m_RemoteCamera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_RemoteCamera->SetPosition(0.0f, 0.0f, -10.0f);
	m_RemoteCamera->RenderBaseViewMatrix();


	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	m_Light->SetPosition(0.0f, 0.0f, 1.0f);
	m_Light->SetDiffuseColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light->SetSpecularColor(0.7f, 0.7f, 0.7f, 1.0f);
	m_Light->SetAmbientColor(0.15, 0.15, 0.15f, 1.0f);
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

	m_RenderTexture2 = new RenderTextureClass;
	if (!m_RenderTexture2)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture2->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the render to texture object.", L"Error", MB_OK);
		return false;
	}

	m_RenderTexture3 = new RenderTextureClass;
	if (!m_RenderTexture3)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture3->Initialize(m_Direct3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
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
	result = m_boxes->Initialize(m_Direct3D->GetDevice(), "../Win32Project1/data/cube.txt", L"../Win32Project1/data/WoodCrate01.dds");
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

	for (int i = 0; i < 19; i++)
	{
		m_folder_texture[i] = new TextureClass;
		if (!m_folder_texture[i])
		{
			return false;
		}
		std::wstring file = L"../Win32Project1/data/folder" + std::to_wstring(i) + L".dds";
		// Initialize the model object.
		result = m_folder_texture[i]->Initialize(m_Direct3D->GetDevice(), file.c_str() );
		if (!result)
		{
			MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
			return false;
		}
	}

	m_content_texture[0] = new TextureClass;
	if (!m_content_texture[0])
	{
		return false;
	}
	std::wstring file = L"../Win32Project1/data/3D.DDs";
	// Initialize the model object.
	result = m_content_texture[0]->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}

	m_maptarget_texture = new TextureClass;
	if (!m_content_texture[0])
	{
		return false;
	}
	file = L"../Win32Project1/data/target.DDs";
	// Initialize the model object.
	result = m_maptarget_texture->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}


	m_content_texture[1] = new TextureClass;
	if (!m_content_texture[1])
	{
		return false;
	}
	file = L"../Win32Project1/data/folder17.DDs";
	// Initialize the model object.
	result = m_content_texture[1]->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}


	m_content_texture[2] = new TextureClass;
	if (!m_content_texture[2])
	{
		return false;
	}
	file = L"../Win32Project1/data/photo.DDs";
	// Initialize the model object.
	result = m_content_texture[2]->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}




	m_earth_texture = new TextureClass;
	if (!m_earth_texture)
	{
		return false;
	}
	file = L"../Win32Project1/data/earthmap1k.DDs";
	// Initialize the model object.
	result = m_earth_texture->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}

	m_map_texture[0] = new TextureClass;
	if (!m_earth_texture)
	{
		return false;
	}
	file = L"../Win32Project1/data/map3.DDs";
	// Initialize the model object.
	result = m_map_texture[0]->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}

	m_map_texture[1] = new TextureClass;
	if (!m_earth_texture)
	{
		return false;
	}
	file = L"../Win32Project1/data/map2.DDs";
	// Initialize the model object.
	result = m_map_texture[1]->Initialize(m_Direct3D->GetDevice(), file.c_str());
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the target texture object.", L"Error", MB_OK);
		return false;
	}


	m_space_texture = new TextureClass;
	if (!m_space_texture)
	{
		return false;
	}
	file = L"../Win32Project1/data/space.DDs";
	// Initialize the model object.
	result = m_space_texture->Initialize(m_Direct3D->GetDevice(), file.c_str());
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
	if (!result)
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

	if (NETWORK_ENABLED)
	{
		m_network = new NetworkClass();
		if (!m_network)
		{
			return false;
		}
		m_network->EstrablishConnection();
	}


	//m_flex = new FlexClass;
	//if (!m_flex)
	//{
	//	return false;
	//}
	//m_flex->initialize();



	m_Text = new TextClass;
	if (!m_Text)
	{
		return false;
	}
	XMMATRIX baseView;
	m_Camera->SetPosition(0.0f, 0.0f, -1.0f);
	m_Camera->Render(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_Camera->GetViewMatrix(baseView);
	result = m_Text->Initialize(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), hwnd, m_screenWidth, m_screenHeight, baseView);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize the text object.", L"Error", MB_OK);
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.5f, -8.0f);
	m_RemoteCamera->SetPosition(0.0f, 1.5f, -8.0f);

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

	//m_states = std::make_unique<CommonStates>(m_Direct3D->GetDevice());

	//m_fxFactory = std::make_unique<EffectFactory>(m_Direct3D->GetDevice());

	//m_model = Model::CreateFromSDKMESH(m_Direct3D->GetDevice(), L"teapot.sdkmesh", *m_fxFactory);

	//m_world = SimpleMath::Matrix::Identity;
	//

	//m_view = SimpleMath::Matrix::CreateLookAt(SimpleMath::Vector3(2.f, 2.f, 2.f),
	//	SimpleMath::Vector3::Zero, SimpleMath::Vector3::UnitY);
	//m_proj = SimpleMath::Matrix::CreatePerspectiveFieldOfView(XM_PI / 4.f,
	//	float(1920) / float(1080), 0.1f, 10.f);

	globalZ = 0;
	globalFactor = 1;
	globalFOV = 21.239;

	ReadFile();
	StopCalibrate(false);

	floor_trans = XMMatrixTranslation(0, m_physx->floor_height, -0.0f);
	floor_scale = XMMatrixScaling(10, 2, 10);

	m_PointCloudShader = new PointCloudShaderClass;
	if (!m_PointCloudShader)
	{
		return false;
	}

	result = m_PointCloudShader->Initialize(m_Direct3D->GetDevice(), hwnd, 512, 424);
	if (!result)
	{
		MessageBox(hwnd, L"Could not initialize point cloud object.", L"Error", MB_OK);
		return false;
	}

	RenderSceneToTexture3();
	ReadPath();
	m_Leap->setNoLoseGrip(false);

		

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
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Direct3D->GetOrthoMatrix(othoMatrix);

	XMStoreFloat4x4(&proj, projectionMatrix);
	XMStoreFloat4x4(&view, viewMatrix);

	memset(depthDest, 0, 512 * 424 * 3);


	if (NETWORK_ENABLED) {
		std::string fr = m_network->Read();
		FrameMsg framemsg;
		framemsg.ParseFromString(fr);
		std::string rframe = framemsg.frame();
		float l, t;
		l = 5.65;
		t = 3.0f;
		if (framemsg.headpos_x() != 0 && framemsg.headpos_y() != 0 && framemsg.headpos_z() != 0)
		{
			m_RemoteCamera->SetPosition(framemsg.headpos_x(), framemsg.headpos_y(), framemsg.headpos_z());
		}
		XMFLOAT3 rleft = XMFLOAT3(-l / 2.0f, 0.0, 0.0);
		XMFLOAT3 rright = XMFLOAT3(l / 2.0f, 0.0, 0.0);
		XMFLOAT3 rtop = XMFLOAT3(-l / 2.0, t, 0.0);
		XMFLOAT3 left = XMFLOAT3(rleft.x - m_RemoteCamera->GetPosition().x, rleft.y - m_RemoteCamera->GetPosition().y, rleft.z - m_RemoteCamera->GetPosition().z);
		XMFLOAT3 right = XMFLOAT3(rright.x - m_RemoteCamera->GetPosition().x, rright.y - m_RemoteCamera->GetPosition().y, rright.z - m_RemoteCamera->GetPosition().z);
		XMFLOAT3 top = XMFLOAT3(rtop.x - m_RemoteCamera->GetPosition().x, rtop.y - m_RemoteCamera->GetPosition().y, rtop.z - m_RemoteCamera->GetPosition().z);
		XMMATRIX rproj = m_Direct3D->ChangeHeadRemotePosition(XMFLOAT3(0, 0, 0), left, right, top, SCREEN_NEAR, SCREEN_DEPTH);


		XMMATRIX rview = m_RemoteCamera->GetRemoteViewMatrix(XMFLOAT3(m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, -m_RemoteCamera->GetPosition().z));
		XMStoreFloat4x4(&proj, rproj);
		//XMStoreFloat4x4(&view, viewMatrix);
		XMStoreFloat4x4(&view, rview);


		m_Leap->setoffset(1.5, 1.8, 5.65, 3.0, PxVec3(m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z));
		m_Leap->processRemoteFrame(m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z, 0, view, proj, handlength, rframe);


	}

	m_Leap->setoffset(1.0, 2.9, 6.0, 3.4, PxVec3());
	m_Leap->processFrame(m_Camera->GetPosition().x, m_Camera->GetPosition().y, m_Camera->GetPosition().z, 0, view, proj, handlength);
	mHandlist = m_Leap->getHandActor();






	if (SUCCEEDED(hr))
	{
		hr = m_Tracker->Process(m_Kinect->getDepthWidth(), m_Kinect->getDepthHeight(), m_Kinect->getColorWidth(), m_Kinect->getColorHeight(),
			m_Kinect->getMinDistance(), m_Kinect->getMaxDistance(), m_Kinect->getDepthBuffer(), m_Kinect->getColorBuffer(), m_Kinect->getCoordinateMapper());

		if (SUCCEEDED(hr))
		{
			head_pos = m_Tracker->goggleDetection();
			if (POINT_CLOUD_ENABLED)
			{
				//for (int i = 0; i < mHandlist.size();i++)
				//{
				//	m_Tracker->transformWorldToKinectDepthImage(mHandlist[i]->wristPosition,distDest);
				//}

				m_Tracker->getPointCloudData(depthDest, true);
				m_Tracker->getColorPointCloudData(rgbDest);

				m_PointCloudShader->UpdateSubResource(m_Direct3D->GetDeviceContext(), depthDest, rgbDest, distDest);
			}

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
				//XMFLOAT3 eyepos(head_pos->x, head_pos->y, head_pos->z);
				XMFLOAT3 left = XMFLOAT3(LEFT.x - head_pos->x, LEFT.y - head_pos->y, LEFT.z - head_pos->z);
				XMFLOAT3 right = XMFLOAT3(RIGHT.x - head_pos->x, RIGHT.y - head_pos->y, RIGHT.z - head_pos->z);
				XMFLOAT3 top = XMFLOAT3(TOP.x - head_pos->x, TOP.y - head_pos->y, TOP.z - head_pos->z);
				m_Direct3D->ChangeHeadPosition(XMFLOAT3(0, 0, 0), left, right, top, SCREEN_NEAR, SCREEN_DEPTH);
			}

			if (gameMode == 0 || gameMode == 1)
				m_physx->moveFrontWall(head_pos->z);
		}
	}

	float cameraDistance = m_Camera->GetPosition().x*m_Camera->GetPosition().x + (m_Camera->GetPosition().y - 1.7)*(m_Camera->GetPosition().y - 1.7) +
		(m_Camera->GetPosition().z)*(m_Camera->GetPosition().z);
	cameraDistance = sqrt(cameraDistance);

	m_3dvision->Render(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(), cameraDistance);

	// Generate the view matrix based on the camera's position.
	if (CORRECT_PERPECTIVE)
		m_Camera->Render(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, -m_Camera->GetPosition().z));
	else
		m_Camera->Render(XMFLOAT3(0, 1.5, 0));

	// Get the world, view, and projection matrices from the camera and d3d objects.
	//std::vector<int> tobedeleted;
	//for (int i = 0; i < m_Leap->deleteBoxes.size(); i++)
	//{
	//	for (int j = 0; j < boxes.size(); j++)
	//	{
	//		if (m_Leap->deleteBoxes[i] == boxes[j])
	//			tobedeleted.push_back(j);
	//	}
	//}
	//int size = tobedeleted.size();
	//for (int i = 0; i < size; i++)
	//{
	//	m_physx->removeActor(boxes[i]);
	//	boxes.erase(boxes.begin() + (int)tobedeleted.back());
	//	tobedeleted.pop_back();
	//}
	//tobedeleted.clear();

	//m_physx->setHandActor(m_Leap->getHandActor());

	map<int, PxRigidDynamic*> contacts = m_physx->getActiveContact();
	if (contacts.size() > 0)
	{
		map<int, PxVec3> forces = m_Leap->computeForce(contacts);
		m_physx->applyForce(forces);
		if (gameMode == 1)
			targetHit = true;
	}

	m_physx->Render();

	if (NETWORK_ENABLED)
	{
		SceneMsg scnmsg;
		EncodedMessage(&scnmsg);
		std::string scn_str;
		scnmsg.set_objects_count(20);
		scnmsg.set_headpos_x(m_Camera->GetPosition().x);
		scnmsg.set_headpos_y(m_Camera->GetPosition().y);
		scnmsg.set_headpos_z(-5.0 - m_Camera->GetPosition().z);
		scnmsg.SerializeToString(&scn_str);

		m_network->Reply(scn_str, scnmsg.ByteSize());
	}


	////Add fingertip
	//m_flex->preupdate();
	//if (gameMode)
	//{
	//	GameFrame();
	//	if (targetHit)
	//	{
	//		if (gameMode == 1)
	//		{
	//			handColor = HANDCOLORHIT;
	//		}
	//		else if (gameMode == 2)
	//		{
	//			targetColor = TARGETCOLORHIT;
	//		}
	//	}
	//	else
	//	{
	//		if (gameMode == 1)
	//		{
	//			handColor = HANDCOLOR;
	//		}
	//		else if (gameMode == 2)
	//		{
	//			targetColor = TARGETCOLOR;
	//		}
	//	}
	//}

	result = RenderSceneToTexture();
	if (!result)
	{
		return false;
	}
	result = RenderSceneToTexture2();
	if (!result)
	{
		return false;
	}
	if (SHADOW_ENABLED)
	{
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
	}

	// Finally up sample the final blurred render to texture that can now be used in the soft shadow shader.
	result = UpSampleTexture();
	if (!result)
	{
		return false;
	}

	if (CORRECT_PERPECTIVE)
		m_Camera->Render(XMFLOAT3(m_Camera->GetPosition().x, m_Camera->GetPosition().y, -m_Camera->GetPosition().z));
	else
		m_Camera->Render(XMFLOAT3(0, 1.5, 0));

	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.5, 0.5, 0.5, 1.0f);

	m_Direct3D->TurnOnAlphaBlending();

	m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(),
		m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(),
		lightRange, attenuate);

	RenderActor(3);
	RenderHand(3);
	RenderTerrian(3);

	RenderRemoteHead(3, m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z);

	#pragma region old
		//for (int i = 0; i < m_flex->particle.size(); i++)
		//{
		//	PxTransform p = PxTransform(PxVec3(m_flex->particle[i].x, m_flex->particle[i].y, m_flex->particle[i].z));

		//	RenderColorSphere(3, p, 0.01, true, XMFLOAT4(1, 0, 0, 1.0));
		//}

		//m_LightShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(),
		//	m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(),
		//	lightRange, attenuate);
		//RenderTexActor(3);

		//if (gameMode == 4 || gameMode == 5 || gameMode == 6)
		//{
		//	m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_RenderTexture3->GetShaderResourceView(),
		//		m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(),
		//		lightRange, attenuate);
		//	XMMATRIX mat = PxtoXMMatrix(boxEndPosition[randomOffset + logCount]);

		//	RenderTargetBox(0, mat, m_RenderTexture3->GetShaderResourceView(), BOX_SIZE.x, BOX_SIZE.y, BOX_SIZE.z);

		//}

	#pragma endregion

	m_Direct3D->TurnOffAlphaBlending();

	//RenderText(Head_Error, head_pos);
	

	if (POINT_CLOUD_ENABLED)
		RenderPointCloud();
	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}


void GraphicsClass::StopCalibrate(bool writeMode)
{
	for (int i = 0; i < spheres.size(); i++)
		m_physx->removeActor(spheres[i]);

	for (int i = 0; i < cylinders.size(); i++)
		m_physx->removeActor(cylinders[i]);

	for (int i = 0; i < targets.size(); i++)
		m_physx->removeActor(targets[i]);

	for (int i = 0; i < folders.size(); i++)
		m_physx->removeActor(folders[i]);

	if (gameMode != 4 || gameMode != 5 || gameMode != 6)
	for (int i = 0; i < boxes.size(); i++)
		m_physx->removeActor(boxes[i]);

	for (int i = 0; i < obstacles.size(); i++)
		m_physx->removeActor(obstacles[i]);

	spheres.clear();
	if (gameMode != 4 || gameMode != 5 || gameMode != 6)
	boxes.clear();
	targets.clear();
	obstacles.clear();
	cylinders.clear();
	folders.clear();

	if (gameMode == 7)
	{
		m_physx->removeActor(pollActor);
		m_physx->removeActor(pollActor2);

	}
	if (gameMode == 8)
	{
		m_physx->removeActor(pollActor);
		m_physx->removeActor(pollActor2);

		for (int i = 0; i < contents.size(); i++)
			deleteContent(contents[i]);

		for (int i = 0; i < joints.size(); i++)
			if (joints[i] != NULL)
			{
				joints[i]->release();
				joints[i] = NULL;
			}

		joints.clear();
		contents.clear();

	}

	
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

	if (fingertipDetected > 50)
	{
		m_Tracker->estimateAffineTransform();
		fingertipDetected = 0;
	}
	else
	{
		fingertipDetected = 0;
	}

	m_physx->moveBackWall(5);
	lastGameMode = gameMode;
	gameMode = 0;
	expMode = 0;
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
			m_physx->moveBackWall(5.0f);
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
		else if (gameMode == 4)
		{
			logCount = 0;
			gogoMode = false;
			m_Leap->setHandMode(gogoMode);
			
			randomOffset = (rand() % 10)*5;
			PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0,0,-2),PxQuat::createIdentity());
			boxes.push_back(box);
		}
		else if (gameMode == 5)
		{
			logCount = 0;
			gogoMode = true;
			m_Leap->setHandMode(gogoMode);
			m_Leap->setNoLoseGrip(true);
			randomOffset = (rand() % 10) * 5;
			PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0, 0, -2), PxQuat::createIdentity());
			((PxRigidDynamic*)box)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			boxes.push_back(box);
		}
		else if (gameMode == 6)
		{
			logCount = 0;
			gogoMode = false;
			m_Leap->setHandMode(gogoMode);
			m_Leap->setNoLoseGrip(true);
			randomOffset = (rand() % 10) * 5;
			PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0, 0, -2), PxQuat::createIdentity());
			((PxRigidDynamic*)box)->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
			boxes.push_back(box);
		}
		else if (gameMode == 7)
		{
			PxMaterial* mMaterial = m_physx->getPhysicsSDK()->createMaterial(1.0, 1.0, 0.5);
			pollActor = m_physx->getPhysicsSDK()->createRigidStatic(PxTransform(PxVec3(-1.0, 1.26, -2.0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			PxShape* shape = pollActor->createShape(PxCapsuleGeometry(0.05, 0.26), *mMaterial);
			m_physx->getScene()->addActor(*pollActor);

			pollActor2 = m_physx->getPhysicsSDK()->createRigidStatic(PxTransform(PxVec3(1.0, 1.26, 2.0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			shape = pollActor2->createShape(PxCapsuleGeometry(0.05, 0.26), *mMaterial);
			m_physx->getScene()->addActor(*pollActor2);

			Createfolder();
		}
		else if (gameMode == 8)
		{
			PxMaterial* mMaterial = m_physx->getPhysicsSDK()->createMaterial(1.0, 1.0, 0.5);
			pollActor = m_physx->getPhysicsSDK()->createRigidStatic(PxTransform(PxVec3(-2, 2, 1.25), PxQuat(PxHalfPi, PxVec3(0, 1,0 ))));
			PxShape* shape = pollActor->createShape(PxCapsuleGeometry(0.02, 1.25), *mMaterial);
			m_physx->getScene()->addActor(*pollActor);

			pollActor2 = m_physx->getPhysicsSDK()->createRigidStatic(PxTransform(PxVec3(2, 2, 1.25), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
			shape = pollActor2->createShape(PxCapsuleGeometry(0.02, 1.25), *mMaterial);
			m_physx->getScene()->addActor(*pollActor2);

		//	PxRigidActor* endbox = m_physx->createBox(PxVec3(0.25, 0.25, 0.05), PxVec3(0,0,0), PxQuat::createIdentity());
		//	PxD6Joint* j = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(1.25, 0, 0)), endbox, PxTransform(PxVec3(-0.26, 0.25, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
		////	j = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(1.25, 0, 0)), endbox, PxTransform(PxVec3(-0.26, -0.25, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
		//	folders.push_back(endbox);
		//	joints.push_back(j);


			for (int i = 0; i < 4; i++)
			{
				PxVec3 pose = PxVec3(-2, 3, 2+i/5.0f);
				PxRigidActor* box = m_physx->createBox(PxVec3(0.3, 0.3, 0.15), pose, PxQuat::createIdentity());
				PxD6Joint* j = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(0, 0, 0)), box, PxTransform(PxVec3(-0.3, 0.3, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
				j->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

				joints.push_back(j);
				folders.push_back(box);

			}

			for (int i = 0; i < 4; i++)
			{
				PxVec3 pose = PxVec3(2, 3, 4-i/10.0f);
				PxRigidActor* box = m_physx->createBox(PxVec3(0.3, 0.3, 0.15), pose, PxQuat::createIdentity());
				PxD6Joint* j = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor2, PxTransform(PxVec3(0, 0, 0)), box, PxTransform(PxVec3(0.3, 0.3, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
				j->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

				joints.push_back(j);
				folders.push_back(box);

			}

			//pose = PxVec3(0, 3, -1);
			//box = m_physx->createBox(PxVec3(0.25, 0.25, 0.05), pose, PxQuat::createIdentity());
			//j = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor2, PxTransform(PxVec3(0, 0, 0)), box, PxTransform(PxVec3(-0.35, 0, 0) , PxQuat(-PxHalfPi, PxVec3(0, 1, 0))));
			//folders.push_back(box);

			//Createfolder();
		}
		else if (gameMode == 9)
		{
			mapAlpha = 1.0f;
			picAlpha = 0.0f;
			countstart = false;
			showContent = false;
			pollActor = m_physx->getPhysicsSDK()->createRigidStatic(PxTransform(PxVec3(0, 2.5, 3)));
			PxShape* shape = pollActor->createShape(PxSphereGeometry(0.5), *m_physx->mMaterial);
			shape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
			
			globe = m_physx->createSphere(1.0, PxVec3(0, 2.5, 3), PxQuat::createIdentity());

			//globe->setMassSpaceInertiaTensor(PxVec3(100, 100, 100));

			PxD6Joint* joint = PxD6JointCreate(*(m_physx->getPhysicsSDK()), pollActor, PxTransform(PxVec3(0, 0, 0)), globe, PxTransform(PxVec3(0, 0, 0)));
			joint->setMotion(PxD6Axis::eTWIST, PxD6Motion::eFREE);
			joint->setMotion(PxD6Axis::eSWING1, PxD6Motion::eFREE);
			joint->setMotion(PxD6Axis::eSWING2, PxD6Motion::eFREE);
			m_physx->getScene()->addActor(*pollActor);
			m_physx->getScene()->addActor(*globe);
					
			createPin(PxVec3(1, 2.5, -2));

		}
		start = std::chrono::steady_clock::now();
		smallStart = std::chrono::steady_clock::now();
	}
}

void GraphicsClass::StartExperimentMode(int mode)
{
	//Set correct mode
	gameMode = 0;
	expMode = mode;

	//Clear all the actor
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

	//Start Countdown timer, in case we need to use it.
	start = std::chrono::steady_clock::now();
	smallStart = std::chrono::steady_clock::now();

	//Initialize based on experiment mode
	if (expMode == 1)
	{
		//Todo 
		//Create different box
		for (int i = 0; i < 7; i++)
		{
			PxRigidActor* box = m_physx->createBox(BOX_SIZE, PxVec3(0, i + 2, -2), PxQuat::createIdentity());
			boxes.push_back(box);
		}
	}
}

void GraphicsClass::ExperimentFrame()
{

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

void GraphicsClass::RotateActorZ(float value)
{
	PxQuat q = PxQuat(value, PxVec3(0, 0, 1));
	actorOffset.q = q*actorOffset.q;
}

void GraphicsClass::RotateActorX(float value)
{
	PxQuat q = PxQuat(value, PxVec3(1, 0, 0));
	actorOffset.q = q*actorOffset.q;
}

void GraphicsClass::RotateActorY(float value)
{
	PxQuat q = PxQuat(value, PxVec3(0, 1, 0));
	actorOffset.q = q*actorOffset.q;
}

void GraphicsClass::ChangeZvalue(float value)
{
		actorOffset.p.z += value;
		//m_physx->moveBackWall(globalZ);
}

void GraphicsClass::ChangeSize(float value)
{
	if (calibrateMode)
	{
		std::vector<handActor*> handactor = m_Leap->getHandActor();
		if (handactor.size() == 1)
		{
			PxVec3 wristPos = m_Leap->getWristPosition(handactor[0]->id);
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

void GraphicsClass::ChangeMirrorHand()
{
	handMode++;
	handMode = handMode % 3;
	m_Leap->switchMirrorHand(handMode);
}

void GraphicsClass::RenderActor(int mode)
{
	for (int i = 0; i < boxes.size(); i++)
	{
		checkObjectPos(boxes[i]);
		//RenderTextureBox(mode, boxes[i], m_boxes->GetTexture(), BOX_SIZE.x, BOX_SIZE.y, BOX_SIZE.z);
		RenderColorBox(mode, boxes[i], XMFLOAT4(0, 1.0, 1.0, 1.0), BOX_SIZE.x, BOX_SIZE.y, BOX_SIZE.z);
	}
	for (int i = 0; i < spheres.size(); i++)
	{
		checkObjectPos(spheres[i]);
		RenderColorSphere(mode, spheres[i], SPHERE_RAD,true, XMFLOAT4(0.7, 0.4, 0.46f, 1.0f));
		//RenderTextureSphere(mode, spheres[i], m_earth_texture->GetTexture(), SPHERE_RAD);
	}

	for (int i = 0; i < cylinders.size(); i++)
	{
		checkObjectPos(cylinders[i]);
		RenderCylinder(mode, cylinders[i], 1.0f, SPHERE_RAD * 4, SPHERE_RAD * 2,mboxMaterial);
	}

	for (int i = 0; i < pins.size(); i++)
	{
		PxTransform pt = pins[i]->getGlobalPose();
		pt.q = PxQuat::createIdentity();
		pt.p.y -= 0.1;
		RenderColorBox(mode, pt, XMFLOAT4(0, 1.0, 1.0, 1.0), 0.01,0.15,0.01);
		pt.p.y += 0.2;
		RenderColorSphere(mode, pt, 0.1, true, XMFLOAT4(1.0,0.0,0.0,1.0));
	}


	for (int i = 0; i < targets.size(); i++)
	{
		RenderTextureBox(mode, targets[i], m_Target_Texture->GetTexture(), TARGET_SIZE.x, TARGET_SIZE.y, TARGET_SIZE.z);
	}

	for (int i = 0; i < obstacles.size(); i += 2)
	{
		RenderColorBox(mode, obstacles[i], targetColor, BARRIER_SIZE_X.x, BARRIER_SIZE_X.y, BARRIER_SIZE_X.z);
		RenderColorBox(mode, obstacles[i + 1], targetColor, BARRIER_SIZE_Z.x, BARRIER_SIZE_Z.y, BARRIER_SIZE_Z.z);
	}

	if (gameMode == 7)
	{
		RenderCylinder(mode, pollActor, 1.0, 0.6, 0.04, mboxMaterial);
		RenderCylinder(mode, pollActor2, 1.0f, 0.6, 0.04, mboxMaterial);
	}
	else if (gameMode == 8)
	{
		//RenderCylinder(mode, pollActor, 1.0, 2.5, 0.04);
		//RenderCylinder(mode, pollActor2, 1.0f, 2.5, 0.04);
	}

}

void GraphicsClass::RenderRemoteHead(int mode, float posx, float posy, float posz)
{
	PxTransform pt(PxVec3(posx, posy,-posz-5.0f));
	RenderColorSphere(mode, pt, 0.3, true, XMFLOAT4(1.0f,1.0f,1.0f,1.0f));

}

void GraphicsClass::RenderTexActor(int mode)
{
	if (gameMode == 7)
	{
		int offset = 5;
		RenderTexture(mode, PxTransform(PxVec3(-1, 1.1, -1),PxQuat(PxHalfPi,PxVec3(1,0,0))), m_folder_texture[1]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		RenderTexture(mode, PxTransform(PxVec3(-2, 1.1, -2), PxQuat(PxHalfPi, PxVec3(1, 0, 0))), m_folder_texture[2]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		RenderTexture(mode, PxTransform(PxVec3(-2, 1.1, -1), PxQuat(PxHalfPi, PxVec3(1, 0, 0))), m_folder_texture[3]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		RenderTexture(mode, PxTransform(PxVec3(2, 1.1, 1), PxQuat(PxHalfPi, PxVec3(1, 0, 0))), m_folder_texture[4]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		for (int i = 0; i < folders.size(); i++)
		{
			RenderTextureBox(mode, folders[i], m_folder_texture[i + offset]->GetTexture(), 0.25, 0.2, 0.01);
		}
	}
	if (gameMode == 8)
	{
		int offset = 4;
		//RenderTexture(mode, PxTransform(PxVec3(-1, 2, 2)), m_folder_texture[1]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		//RenderTexture(mode, PxTransform(PxVec3(-2, 3, 2)), m_folder_texture[2]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		//RenderTexture(mode, PxTransform(PxVec3(-2, 2, 2)), m_folder_texture[3]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		//RenderTexture(mode, PxTransform(PxVec3(1, 3, 2)), m_folder_texture[1]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		//RenderTexture(mode, PxTransform(PxVec3(2, 2, 2)), m_folder_texture[2]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		//RenderTexture(mode, PxTransform(PxVec3(2, 3, 2)), m_folder_texture[3]->GetTexture(),1.0, 0.25, 0.2, 0.01);
		for (int i = 0; i < folders.size(); i++)
		{
				RenderTextureBox(mode, folders[i], m_folder_texture[i + offset]->GetTexture(), 0.3, 0.3, 0.025);
		}
		RenderTexture(mode, PxTransform(PxVec3(0, 1, -2), PxQuat(PxPi / 2, PxVec3(1, 0, 0))), m_folder_texture[18]->GetTexture(),1.0, 2, 1, 0.01);

		for (int i = 0; i < contents.size(); i++)
		{
			if (contents[i]->rendering)
			{
				PxTransform pt = contents[i]->board->getGlobalPose();

				RenderTexture(mode, pt, m_content_texture[i%2]->GetTexture(), -pt.p.z - 0.9, 1.3*contents[i]->scale, 0.75*contents[i]->scale, 0.01);

			}

		}
	}
	if (gameMode == 9)
	{
		RenderTexture(mode, PxTransform(PxVec3(0, 2, 4)), m_space_texture->GetTexture(), 1.0, 6, 4, 0.1);
		RenderTextureSphere(mode, globe, m_earth_texture->GetTexture(), 1.0);		
		RenderTexture(mode, PxTransform(PxVec3(0, 2.5, 2)), m_maptarget_texture->GetTexture(), 1.0, 0.1, 0.1, 0.01);
		if(showContent)
			RenderTexture(mode, PxTransform(PxVec3(0, 2, 1.25)), m_content_texture[2]->GetTexture(), picAlpha, 5, 2, 0.1);

		RenderTextureTerrain(mode, PxTransform(PxVec3(0, 0.99, 0)), m_map_texture[0]->GetTexture(), 1-mapAlpha, 6, 4, 0.05);
		RenderTextureTerrain(mode, PxTransform(PxVec3(0, 1, 0)), m_map_texture[1]->GetTexture(), mapAlpha, 6, 4, 0.1);
		
	
	}
}

void GraphicsClass::RenderHand(int mode)
{
	mHandlist = m_Leap->getHandActor();
	mRHandlist = m_Leap->getRHandActor();
	//if(mHandlist.size() != 0.0 && !FULL_SCREEN )
	//m_Tracker->handVisualize(mHandlist);

	for (int i = 0; i < mHandlist.size(); i++)
	{
		if (mHandlist[i]->isInMirror)
		{
			for (int j = 0; j < 5; j++)
			{

				
			RenderColorSphere(mode,mHandlist[i]->finger_tip_actor[j], mHandlist[i]->fingerWidth[j][3]/2.0,true, mhandMaterial.Diffuse);

				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector(mHandlist[i]->leapJointPosition2[j * 4 + k].x, mHandlist[i]->leapJointPosition2[j * 4 + k].y, mHandlist[i]->leapJointPosition2[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				//RenderColorSphere(mode, mHandlist[i]->finger_tip_achor[j], mHandlist[i]->fingerWidth[j][3]/2, handColor);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
						RenderCylinder(mode, mHandlist[i]->finger_actor[j][k], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], mhandMaterial);

					RenderPalm(mode, mHandlist[i]->finger_actor[j][1], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][1] * 2.0, mHandlist[i]->fingerWidth[j][1], mHandlist[i]->fingerWidth[j][1] / 2, mhandMaterial);
				}
				else
				{
					for (int k = 1; k < 4; k++)
						RenderCylinder(mode, mHandlist[i]->finger_actor[j][k], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], mhandMaterial);

					RenderPalm(mode, mHandlist[i]->finger_actor[j][0], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][0] * 2.0, mHandlist[i]->fingerWidth[j][0], mHandlist[i]->fingerWidth[j][0] / 2, mhandMaterial);
				}
			}
		}
	}
	for (int i = 0; i < mHandlist.size(); i++)
	{
		if (!mHandlist[i]->isInMirror)
		{
			for (int j = 0; j < 5; j++)
			{
				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector( mHandlist[i]->leapJointPosition[j * 4 + k].x, mHandlist[i]->leapJointPosition[j * 4 + k].y, mHandlist[i]->leapJointPosition[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector(mHandlist[i]->leapJointPosition2[j * 4 + k].x, mHandlist[i]->leapJointPosition2[j * 4 + k].y, mHandlist[i]->leapJointPosition2[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				RenderColorSphere(mode, mHandlist[i]->finger_tip_actor[j], mHandlist[i]->fingerWidth[j][3]/2.0,true, mhandMaterial.Diffuse);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
						RenderCylinder(mode, mHandlist[i]->finger_actor[j][k], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], mhandMaterial);

					RenderPalm(mode, mHandlist[i]->finger_actor[j][1], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][1] * 2.0, mHandlist[i]->fingerWidth[j][1], mHandlist[i]->fingerWidth[j][1] / 2, mhandMaterial);
				}
				else
				{
					for (int k = 1; k < 4; k++)
						RenderCylinder(mode, mHandlist[i]->finger_actor[j][k], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], mhandMaterial);

					RenderPalm(mode, mHandlist[i]->finger_actor[j][0], mHandlist[i]->alphaValue, mHandlist[i]->halfHeight[j][0] * 2.0, mHandlist[i]->fingerWidth[j][0], mHandlist[i]->fingerWidth[j][0] / 2, mhandMaterial);
				}
			}
		}
	}


	for (int i = 0; i < mRHandlist.size(); i++)
	{
		if (mRHandlist[i]->isInMirror)
		{
			for (int j = 0; j < 5; j++)
			{


				RenderColorSphere(mode, mRHandlist[i]->finger_tip_actor[j], mRHandlist[i]->fingerWidth[j][3] / 2.0, true, mrHandMaterial.Diffuse);

				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector(mRHandlist[i]->leapJointPosition2[j * 4 + k].x, mRHandlist[i]->leapJointPosition2[j * 4 + k].y, mRHandlist[i]->leapJointPosition2[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				//RenderColorSphere(mode, mRHandlist[i]->finger_tip_achor[j], mRHandlist[i]->fingerWidth[j][3]/2, handColor);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
						RenderCylinder(mode, mRHandlist[i]->finger_actor[j][k], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], mrHandMaterial);

					RenderPalm(mode, mRHandlist[i]->finger_actor[j][1], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][1] * 2.0, mRHandlist[i]->fingerWidth[j][1], mRHandlist[i]->fingerWidth[j][1] / 2, mrHandMaterial);
				}
				else
				{
					for (int k = 1; k < 4; k++)
						RenderCylinder(mode, mRHandlist[i]->finger_actor[j][k], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], mrHandMaterial);

					RenderPalm(mode, mRHandlist[i]->finger_actor[j][0], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][0] * 2.0, mRHandlist[i]->fingerWidth[j][0], mRHandlist[i]->fingerWidth[j][0] / 2, mrHandMaterial);
				}
			}
		}
	}
	for (int i = 0; i < mRHandlist.size(); i++)
	{
		if (!mRHandlist[i]->isInMirror)
		{
			for (int j = 0; j < 5; j++)
			{
				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector( mRHandlist[i]->leapJointPosition[j * 4 + k].x, mRHandlist[i]->leapJointPosition[j * 4 + k].y, mRHandlist[i]->leapJointPosition[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				//for (int k = 0; k < 5; k++)
				//{
				//	Leap::Vector px = Leap::Vector(mRHandlist[i]->leapJointPosition2[j * 4 + k].x, mRHandlist[i]->leapJointPosition2[j * 4 + k].y, mRHandlist[i]->leapJointPosition2[j * 4 + k].z);
				//	RenderDebugSphere(mode, m_Leap->leapToWorld(px), 0.05, handColor);
				//}

				RenderColorSphere(mode, mRHandlist[i]->finger_tip_actor[j], mRHandlist[i]->fingerWidth[j][3] / 2.0, true, mrHandMaterial.Diffuse);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
						RenderCylinder(mode, mRHandlist[i]->finger_actor[j][k], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], mrHandMaterial);

					RenderPalm(mode, mRHandlist[i]->finger_actor[j][1], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][1] * 2.0, mRHandlist[i]->fingerWidth[j][1], mRHandlist[i]->fingerWidth[j][1] / 2, mrHandMaterial);
				}
				else
				{
					for (int k = 1; k < 4; k++)
						RenderCylinder(mode, mRHandlist[i]->finger_actor[j][k], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], mrHandMaterial);

					RenderPalm(mode, mRHandlist[i]->finger_actor[j][0], mRHandlist[i]->alphaValue, mRHandlist[i]->halfHeight[j][0] * 2.0, mRHandlist[i]->fingerWidth[j][0], mRHandlist[i]->fingerWidth[j][0] / 2, mrHandMaterial);
				}
			}
		}
	}
}

void GraphicsClass::RenderTextureBox(int mode, PxRigidActor* box,
	ID3D11ShaderResourceView* texture, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	while (nShapes--)
	{
		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
		mat = s*mat;

		m_Shape->Render(m_Direct3D->GetDevice());

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 1)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);
			Material tempMat = mtextureMaterial;
			if (box->userData != NULL)
			{
				float* st = reinterpret_cast<float*>(box->userData);
				float s = st[0];
				tempMat.Diffuse = XMFLOAT4(tempMat.Diffuse.x , tempMat.Diffuse.y, tempMat.Diffuse.z+s, tempMat.Diffuse.w);
			}

			m_LightShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat,texture, m_Camera->GetPosition());

			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset());
		}
	}
}

void GraphicsClass::RenderTextureSphere(int mode, PxRigidActor* box,
	ID3D11ShaderResourceView* texture, float radius)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	while (nShapes--)
	{
		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(radius*2,radius*2,radius*2);
		mat = s*mat;

		m_Shape->Render(m_Direct3D->GetDevice());

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 1)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(),  m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);
			Material tempMat = mtextureMaterial;
			//if (box->userData != NULL)
			//{
			//	float* st = reinterpret_cast<float*>(box->userData);
			//	float s = st[0];
			//	tempMat.Diffuse = XMFLOAT4(tempMat.Diffuse.x, tempMat.Diffuse.y, tempMat.Diffuse.z + s, tempMat.Diffuse.w);
			//}

			m_LightShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, texture, m_Camera->GetPosition());

			m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset());
		}
	}
}

void GraphicsClass::RenderTextureTerrain(int mode, PxTransform pt,
	ID3D11ShaderResourceView* texture, float aplha, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	XMMATRIX mat = PxtoXMMatrix(pt);
	//XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
	//mat = mat;

	m_Shape->Render(m_Direct3D->GetDevice());

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


	if (mode == 0)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 1)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 2)
	{
		m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
			lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
			lightViewMatrix2, lightProjectionMatrix
			, m_RenderTexture2->GetShaderResourceView());
	}
	else
	{
		Material text = mtextMaterial;
		text.Diffuse.w = aplha;

		m_LightShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, text, texture, m_Camera->GetPosition());

		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset());

	}
}

void GraphicsClass::RenderTexture(int mode, PxTransform transform,
	ID3D11ShaderResourceView* texture,float aplha, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);
		
	XMMATRIX mat = PxtoXMMatrix(transform);
	XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
	mat = s*mat;

	m_Shape->Render(m_Direct3D->GetDevice());

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


	if (mode == 0)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 1)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 2)
	{
		m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
			lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
			lightViewMatrix2, lightProjectionMatrix
			, m_RenderTexture2->GetShaderResourceView());
	}
	else
	{
		Material text = mtextMaterial;
		text.Diffuse.w = aplha;

		m_LightShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, text, texture, m_Camera->GetPosition());

		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset());
		
	}
}

void GraphicsClass::RenderTargetBox(int mode, XMMATRIX transform, ID3D11ShaderResourceView* texture, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

		XMMATRIX mat;
		XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
		mat = s*transform;

		Material tempMat;
		tempMat = mboxMaterial;
		tempMat.Diffuse.z = 1.0f;
		tempMat.Diffuse.w = 0.5f;

		m_Shape->Render(m_Direct3D->GetDevice());


		m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, m_Camera->GetPosition());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset());
		

}

void GraphicsClass::RenderAxis(XMMATRIX transform)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	XMMATRIX xmat = XMMatrixScaling(0.3, 0.01, 0.01)*XMMatrixTranslation(0.15,0,0)*transform;
	XMMATRIX ymat = XMMatrixScaling(0.01, 0.3, 0.01)*XMMatrixTranslation(0.0, 0.15, 0)*transform;
	XMMATRIX zmat = XMMatrixScaling(0.01, 0.01, 0.3)*XMMatrixTranslation(0, 0, 0.15)*transform;
	Material tempMatx = mboxMaterial;
	tempMatx.Diffuse.x = 1.0f;
	tempMatx.Diffuse.y = 0.0f;
	tempMatx.Diffuse.z = 0.0f;

	Material tempMaty = mboxMaterial;
	tempMaty.Diffuse.x = 0.0f;
	tempMaty.Diffuse.y = 1.0f;
	tempMaty.Diffuse.z = 0.0f;
	Material tempMatz = mboxMaterial;
	tempMatz.Diffuse.x = 0.0f;
	tempMatz.Diffuse.y = 0.0f;
	tempMatz.Diffuse.z = 1.0f;


	m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), xmat, viewMatrix, projectionMatrix, tempMatx, m_Camera->GetPosition());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
		m_Shape->GetBoxVertexOffset());

	m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), ymat, viewMatrix, projectionMatrix, tempMaty, m_Camera->GetPosition());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
		m_Shape->GetBoxVertexOffset());

	m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), zmat, viewMatrix, projectionMatrix, tempMatz, m_Camera->GetPosition());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
		m_Shape->GetBoxVertexOffset());


}

void GraphicsClass::CaptureBoxPosition()
{
	if (gameMode == 4 || gameMode == 5 || gameMode == 6)
	{
		end = std::chrono::steady_clock::now();
		long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		logPosition[logCount] = boxes[0]->getGlobalPose();
		logTime[logCount] = mils;
		logCount++;

		if (pickActor != NULL)
		{
			pickActor = NULL;
		}

		
		start = std::chrono::steady_clock::now();	

		if (logCount >= 5)
		{

			ofstream fout;
			std::string file = logfile;
			if (gameMode == 4)
				file = file + "_mouse.txt";
			else if (gameMode == 5)
				file = file + "_gogo.txt";
			else if (gameMode == 6)
				file = file + "_mirror.txt";
			fout.open(file);
			if (fout.is_open())
			{
				for (int i = 0; i < 5; i++)
				{
					fout << randomOffset+i << " " << logTime[i] << " "
						<< logPosition[i].p.x << " "
						<< logPosition[i].p.y << " "
						<< logPosition[i].p.z << " "
						<< logPosition[i].q.x << " "
						<< logPosition[i].q.y << " "
						<< logPosition[i].q.z << " "
						<< logPosition[i].q.w << endl;
				}
				fout.close();
			}
			gameMode = 0;
			m_Leap->setNoLoseGrip(false);
			((PxRigidDynamic*)boxes[0])->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			//StopCalibrate(false);


		}
	}
}


void GraphicsClass::RenderColorBox(int mode, PxRigidActor* box,
	XMFLOAT4 color, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	while (nShapes--)
	{
		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
		mat = s*mat;

		m_Shape->Render(m_Direct3D->GetDevice());

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 1)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);
			Material tempMat = mboxMaterial;
			if (box->userData != NULL)
			{
				float* st = reinterpret_cast<float*>(box->userData);
				float s = st[0];
				tempMat.Diffuse = XMFLOAT4(tempMat.Diffuse.x + (s), tempMat.Diffuse.y, tempMat.Diffuse.z, tempMat.Diffuse.w);
			}

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, m_Camera->GetPosition());

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset());
		}
	}

	delete[] shapes;
}

void GraphicsClass::RenderColorBox(int mode, PxTransform pT,
	XMFLOAT4 color, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);


	XMMATRIX mat = PxtoXMMatrix(pT);
	XMMATRIX s = XMMatrixScaling(width * 2, height * 2, depth * 2);
	mat = s*mat;

	m_Shape->Render(m_Direct3D->GetDevice());

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.


	if (mode == 0)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 1)
	{
		m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
	}
	else if (mode == 2)
	{
		m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
			lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
			lightViewMatrix2, lightProjectionMatrix
			, m_RenderTexture2->GetShaderResourceView());
	}
	else
	{
		//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);
		Material tempMat = mboxMaterial;

		m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, m_Camera->GetPosition());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
			m_Shape->GetBoxVertexOffset());
	}
	

}


void GraphicsClass::RenderPalm(int mode, PxRigidActor* box, float alpha, float width, float height, float depth, Material mMaterial)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);
	Material temp = mMaterial;
	temp.Diffuse.w = alpha;


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
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix,
				lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, temp, m_Camera->GetPosition());

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(),
				m_Shape->GetBoxVertexOffset());
		}
	}

	delete[] shapes;
}

void GraphicsClass::RenderColorSphere(int mode, PxRigidActor* sphere, float radius, bool customedColor = false, XMFLOAT4 color = XMFLOAT4(0,0,0,0))
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;

	PxU32 nShapes = sphere->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];
	sphere->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);

	while (nShapes--)
	{
		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *sphere);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(radius * 2, radius * 2, radius * 2);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Shape->Render(m_Direct3D->GetDevice());

		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 1)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			Material tempMat = msphereMaterial;
			if (customedColor)
			{
				tempMat.Diffuse = color;
			}
			if (sphere->userData != NULL)
			{
				float* st = reinterpret_cast<float*>(sphere->userData);
				float s = st[0];
				tempMat.Diffuse = XMFLOAT4(tempMat.Diffuse.x + (s), tempMat.Diffuse.y, tempMat.Diffuse.z, tempMat.Diffuse.w);
			}
			//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, m_Camera->GetPosition());

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset());
		}
	}
}

void GraphicsClass::RenderColorSphere(int mode, PxTransform pt, float radius,bool customedColor = false, XMFLOAT4 color = XMFLOAT4(0, 0, 0, 0))
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;

	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);


		XMMATRIX mat = PxtoXMMatrix(pt);
		XMMATRIX s = XMMatrixScaling(radius * 2, radius * 2, radius * 2);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_Shape->Render(m_Direct3D->GetDevice());

		if (mode == 0)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 1)
		{
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
				m_Shape->GetSphereVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
		Material tempMat = msphereMaterial;
		if (customedColor)
			tempMat.Diffuse = color;

		//m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);

		m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, tempMat, m_Camera->GetPosition());

		m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetSphereIndexCount(), m_Shape->GetSphereIndexOffset(),
			m_Shape->GetSphereVertexOffset());
	}
	
}

void GraphicsClass::CreateBox()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0]->palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.25), pose, PxQuat::createIdentity());
		boxes.push_back(box);
	}
	else
	{
		PxVec3 pose = PxVec3(0, 3, -1);
		PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.25), pose, PxQuat::createIdentity());
		boxes.push_back(box);
	}
}

void GraphicsClass::Createfolder()
{

	PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.1), PxVec3(1,2,-1), PxQuat::createIdentity());
	PxRevoluteJoint* j = NULL;
	folders.push_back(box);
	joints.push_back(j);

	box = m_physx->createBox(PxVec3(0.25, 0.25, 0.1), PxVec3(1, 2, -1), PxQuat::createIdentity());
	folders.push_back(box);
	joints.push_back(j);
	for (int i = 0; i < 4; i++)
	{
		PxVec3 pose = PxVec3(0, 3, -1);
		PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.1), pose, PxQuat::createIdentity());
		PxRevoluteJoint* j = PxRevoluteJointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(0, 0, 0)), box, PxTransform(PxVec3(-0.45, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			folders.push_back(box);
			joints.push_back(j);

		pose = PxVec3(0, 3, -1);
		box = m_physx->createBox(PxVec3(0.25, 0.25, 0.1), pose, PxQuat::createIdentity());
		j = PxRevoluteJointCreate(*m_physx->getPhysicsSDK(), pollActor2, PxTransform(PxVec3(0, 0, 0)), box, PxTransform(PxVec3(-0.45, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
			folders.push_back(box);
			joints.push_back(j);
	}

	
}

void GraphicsClass::CreateSphere()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0]->palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* sphere = m_physx->createSphere(SPHERE_RAD, pose, PxQuat::createIdentity());
		spheres.push_back(sphere);
	}
	else
	{
		PxVec3 pose = PxVec3(0, 3, -1);
		PxRigidActor* sphere = m_physx->createSphere(SPHERE_RAD, pose, PxQuat::createIdentity());
		spheres.push_back(sphere);
	}
}

void GraphicsClass::CreateCylinder()
{
	mHandlist = m_Leap->getHandActor();
	if (mHandlist.size() > 0)
	{
		PxVec3 pose = mHandlist[0]->palm->getGlobalPose().p;
		pose = PxVec3(pose.x, pose.y + 2, pose.z);
		PxRigidActor* sphere = m_physx->createCapsule(SPHERE_RAD, SPHERE_RAD, pose, PxQuat::createIdentity());
		cylinders.push_back(sphere);
	}
}

void GraphicsClass::RenderCylinder(int mode, PxRigidActor* cylinder, float alpha, float height, float radius, Material mMaterial)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	PxU32 nShapes = cylinder->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	cylinder->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);
	Material temp = mMaterial;
	temp.Diffuse.w = alpha;

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
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset(), mat, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (mode == 2)
		{
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset(), mat, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			//	m_ColorShader->SetShaderPerFrame(m_Direct3D->GetDeviceContext(), m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetSpecularColor(), lightRange, attenuate);

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), mat, viewMatrix, projectionMatrix, temp, m_Camera->GetPosition());

			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetCylinderIndexCount(), m_Shape->GetCylinderIndexOffset(),
				m_Shape->GetCylinderVertexOffset());
		}
	}

	delete[] shapes;
}

void GraphicsClass::RenderTerrian(int rendermode, int gamemode)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMMATRIX lightViewMatrix, lightViewMatrix2, lightProjectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetMirrorMatrix(lightViewMatrix2);
	m_Light->GetProjectionMatrix(lightProjectionMatrix);

	XMMATRIX edge = XMMatrixScaling(0.1, 8, 0.05);
	XMMATRIX edgeTrans = XMMatrixTranslation(-3, 0, 0);
	XMMATRIX edgeTrans2 = XMMatrixTranslation(3, 0, 0);
	XMMATRIX edgeTrans3 = XMMatrixTranslation(0, 1, 0);
	XMMATRIX edge2 = XMMatrixScaling(6, 2 * m_physx->floor_height + 0.1, 0.05);
	XMMATRIX trans = XMMatrixTranslation(-3, m_physx->floor_height, -4.0f);
	XMMATRIX back = XMMatrixTranslation(-3, m_physx->floor_height, -0.0);
	XMMATRIX rotate = XMMatrixRotationX(-XM_PI / 2);
	XMMATRIX slant = XMMatrixRotationX(187 * XM_PI / 180);
	//XMMATRIX mat = rotat
	if (gameMode != 9)
	{
		if (rendermode == 0)
		{
			m_Shape->Render(m_Direct3D->GetDevice());
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(),
				floor_trans, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans2, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge2, lightViewMatrix, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (rendermode == 1)
		{
			m_Shape->Render(m_Direct3D->GetDevice());
			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(),
				floor_trans, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans2, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());

			m_DepthShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge2, lightViewMatrix2, lightProjectionMatrix, m_Camera->GetPosition());
		}
		else if (rendermode == 2)
		{
			m_Shape->Render(m_Direct3D->GetDevice());
			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset(),
				floor_trans, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());

			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());

			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge*edgeTrans2, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());

			m_ShadowShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(),
				edge2, viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_RenderTexture->GetShaderResourceView(), m_Light->GetPositions(),
				lightViewMatrix2, lightProjectionMatrix
				, m_RenderTexture2->GetShaderResourceView());
		}
		else
		{
			m_Shape->Render(m_Direct3D->GetDevice());
			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), floor_trans, viewMatrix, projectionMatrix, mfloorMaterial, m_Camera->GetPosition());
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetGridIndexCount(), m_Shape->GetGridIndexOffset(), m_Shape->GetGridVertexOffset());

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), edge*edgeTrans, viewMatrix, projectionMatrix, medgeMaterial, m_Camera->GetPosition());
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset());

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), edge*edgeTrans2, viewMatrix, projectionMatrix, medgeMaterial, m_Camera->GetPosition());
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset());

			m_ColorShader->SetShaderPerObject(m_Direct3D->GetDeviceContext(), edge2, viewMatrix, projectionMatrix, medgeMaterial, m_Camera->GetPosition());
			m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset());
		}
	}
	

	//m_groundShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(), edge*edgeTrans,
	//	viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_UpSampleTexure->GetShaderResourceView(),
	//	m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	//m_groundShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(), edge*edgeTrans2,
	//	viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_UpSampleTexure->GetShaderResourceView(),
	//	m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	//m_groundShader->Render(m_Direct3D->GetDeviceContext(), m_Shape->GetBoxIndexCount(), m_Shape->GetBoxIndexOffset(), m_Shape->GetBoxVertexOffset(), edge2,
	//	viewMatrix, projectionMatrix, lightViewMatrix, lightProjectionMatrix, m_UpSampleTexure->GetShaderResourceView(),
	//	m_Light->GetPositions(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());

	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, rotate*back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
	//	m_Camera->GetPosition(), globalZ);

	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
	//	m_Camera->GetPosition(), globalZ);

	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, trans, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
	//	m_Camera->GetPosition(), globalZ);

//else if (mode == 1)
//{
//	//trans = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -4.0f);

//	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

//	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, rotate*back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(),
//	//	m_Camera->GetPosition(), globalZ);

//	//back = XMMatrixTranslation(-3, m_physx->FLOOR_LEVEL, -0.5);

//	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

//	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, back, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
//	//	m_Camera->GetPosition(), globalZ);

//	//m_Terrain->Render(m_Direct3D->GetDeviceContext());

//	//m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), 0, 0, trans, viewMatrix, projectionMatrix, m_UpSampleTexure->GetShaderResourceView(), m_Light->GetPositions(), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
//	//	m_Camera->GetPosition(), globalZ);

//}
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

	//if (m_flex)
	//{
	//	m_flex->shutdown();
	//	delete m_flex;
	//	m_flex = 0;
	//}

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
	if (m_RenderTexture2)
	{
		m_RenderTexture2->Shutdown();
		delete m_RenderTexture2;
		m_RenderTexture2 = 0;
	}

	if (m_RenderTexture3)
	{
		m_RenderTexture3->Shutdown();
		delete m_RenderTexture3;
		m_RenderTexture3 = 0;
	}

	if (m_network)
	{
		m_network->Shutdown();
		delete m_network;
		m_network = 0;
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

	if (m_PointCloudShader)
	{
		m_PointCloudShader->Shutdown();
		delete m_PointCloudShader;
		m_PointCloudShader = 0;
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
	if (m_RemoteCamera)
	{
		delete m_RemoteCamera;
		m_RemoteCamera = 0;
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

bool GraphicsClass::Frame(int x, int y, bool leftMouse)
{
	bool result;
	mouseX = x;
	mouseY = y;

	// Update the position of the light.
	m_Light->SetPosition(3, 7.0f, -8);

	if (leftMouse && gameMode != 5 && gameMode != 6)
	{
		if (pickActor == NULL)
		{
			pickActor = TestIntersection(mouseX, mouseY);
			if (pickActor != NULL)
			{
				pickActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, true);
				actorStartPosition = pickActor->getGlobalPose();
				actorOffset.p = PxVec3(0, 0, 0);
				actorOffset.q = PxQuat::createIdentity();
			}
		}
		else
		{
			PxTransform t = actorStartPosition;
			PxVec3 p;
			ViewProject(t.p.x, t.p.y, t.p.z+actorOffset.p.z, p);
			ViewUnProject(x, y, p.z, t.p);
			t.q = actorOffset.q*t.q;
			pickActor->setKinematicTarget(t);

		}

	}
	else
	{
		if (pickActor != NULL)
		{
			pickActor->setRigidBodyFlag(PxRigidBodyFlag::eKINEMATIC, false);
			pickActor = NULL;
		}
	}
	// Update the position of the light.
	// Render the graphics scene.
	result = Render();
	if (!result)
	{
		return false;
	}

	return true;
}

void GraphicsClass::setAltPressed(bool alt)
{
	altpressed = alt;
}

void GraphicsClass::GameFrame()
{
	if (gameMode == 1)
	{
		end = std::chrono::steady_clock::now();
		long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
		if (mils > 1000)
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
			if (((PxRigidDynamic*)spheres[i])->getGlobalPose().p.y < m_physx->floor_height || targetHit)
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
		if (mils > 1000)
		{
			start = end;
			countDownTimer--;
		}
		if (countDownTimer == 0)
		{
			for (int i = 0; i < targets.size(); i++)
				m_physx->removeActor(targets[i]);

			for (int i = 0; i < boxes.size(); i++)
				m_physx->removeActor(boxes[i]);

			for (int i = 0; i < obstacles.size(); i++)
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
			if (mi > 500)
			{
				for (int i = 0; i < targets.size(); i++)
					m_physx->removeActor(targets[i]);

				for (int i = 0; i < boxes.size(); i++)
					m_physx->removeActor(boxes[i]);

				for (int i = 0; i < obstacles.size(); i++)
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
			boxPosition.y = m_physx->floor_height + 0.25;
			boxPosition.x = 3 * rand() / RAND_MAX - 1.5;
			boxPosition.z = -3 * rand() / RAND_MAX;
			float distance;
			do
			{
				targetPostion.y = m_physx->floor_height + 0.05;
				targetPostion.x = 2 * rand() / RAND_MAX - 1;
				targetPostion.z = -2 * rand() / RAND_MAX - 1;
				distance = (targetPostion.x - boxPosition.x)*(targetPostion.x - boxPosition.x) + (targetPostion.z - boxPosition.z)*(targetPostion.z - boxPosition.z);
			} while (distance < 2);

			PxRigidDynamic* box = m_physx->createBox(BOX_SIZE, boxPosition, PxQuat::createIdentity());
			PxRigidActor* target = m_physx->createTarget(TARGET_SIZE, targetPostion, PxQuat::createIdentity());

			PxRigidActor* obs1 = m_physx->createBarrier(BARRIER_SIZE_X, PxVec3(targetPostion.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z + TARGET_SIZE.z + BARRIER_SIZE_X.z), PxQuat::createIdentity());
			PxRigidActor* obs3 = m_physx->createBarrier(BARRIER_SIZE_X, PxVec3(targetPostion.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z - TARGET_SIZE.z - BARRIER_SIZE_X.z), PxQuat::createIdentity());
			PxRigidActor* obs2 = m_physx->createBarrier(BARRIER_SIZE_Z, PxVec3(targetPostion.x + TARGET_SIZE.x + BARRIER_SIZE_Z.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z), PxQuat::createIdentity());
			PxRigidActor* obs4 = m_physx->createBarrier(BARRIER_SIZE_Z, PxVec3(targetPostion.x - TARGET_SIZE.x - BARRIER_SIZE_Z.x, targetPostion.y + BARRIER_SIZE_X.y, targetPostion.z), PxQuat::createIdentity());

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
		if (mils > 1000)
		{
			start = end;
			countDownTimer--;
		}
		if (countDownTimer == 0)
		{
			StopCalibrate(false);
		}
	}
	else if (gameMode == 5 || gameMode ==6)
	{
		if (mHandlist.size() > 0)
		{
			PxTransform t = mHandlist[0]->handTransform;
			PxTransform h = t;

			if (actorstartPos.isValid() == false)
			{
				actorstartPos.p = t.p;
				actorstartPos.q = PxQuat::createIdentity();
				PxVec3 roffset = h.q.rotate(PxVec3(0, -0.5, 0));
				PxTransform f = PxTransform(actorstartPos.p + roffset, actorstartPos.q);
				((PxRigidDynamic*)boxes[0])->setKinematicTarget(f);
			}
			if (handStartPos.isValid() == false )
			{
				handStartPos = mHandlist[0]->handTransform;
			}
			if (altpressed)
			{
				PxTransform test = ((PxRigidDynamic*)boxes[0])->getGlobalPose();
				actorstartPos.q = test.q;
				handStartPos.q = h.q;
			}
			else
			{
				PxVec3 roffset = h.q.rotate(PxVec3(0, -0.5, 0));
				PxQuat diff = handStartPos.q *h.q.getConjugate();
				PxTransform f = PxTransform(actorstartPos.p + (h.p - handStartPos.p) + roffset, diff.getConjugate()* actorstartPos.q);
				((PxRigidDynamic*)boxes[0])->setKinematicTarget(f);
			}
		}
		else
		{
			actorstartPos = PxTransform();
			handStartPos = PxTransform();
		}
	


		
	}
	else if (gameMode == 7)
	{
		for (int i = 0; i < folders.size(); i++)
		{
			if (folders[i]->userData != NULL)
			{
				if (joints[i] != NULL)
				{
					joints[i]->release();
					joints[i] = NULL;
				}

			}

			if (joints[i] == NULL)
			{
				PxTransform t = folders[i]->getGlobalPose();
				PxVec3 tp = t.p;
				if (tp.x < -0 && tp.x > -2.5 && tp.z > -2.5 && tp.z < 0)
				{
					joints[i] = PxRevoluteJointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(0, 0, 0)), folders[i], PxTransform(PxVec3(-0.45, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));
				}
				if (tp.x > -0 && tp.x < 2.5 && tp.z < 2.5 && tp.z > 0)
				{
					joints[i] = PxRevoluteJointCreate(*m_physx->getPhysicsSDK(), pollActor2, PxTransform(PxVec3(0, 0, 0)), folders[i], PxTransform(PxVec3(-0.45, 0, 0), PxQuat(PxHalfPi, PxVec3(0, 0, 1))));

				}
			}

		}
	}
	else if ( gameMode == 8)
	{
		for (int i = 0; i < folders.size(); i++)
		{
			if (folders[i]->userData != NULL)
			{
				if (joints[i]!=NULL)
				{
					joints[i]->release();
					joints[i] = NULL;
				}

			}

			if(joints[i] == NULL)
			{
				PxTransform t = folders[i]->getGlobalPose();
				PxVec3 tp = t.p;
				if (tp.x < -1 && tp.x > -2.5 && tp.z > 1 && tp.z < 3.5)
				{
					joints[i] = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor, PxTransform(PxVec3(tp.z, 0, 0)), folders[i], PxTransform(PxVec3(-0.3, 0.3, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
					((PxD6Joint*)joints[i])->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

				}
				else if (tp.x > 1 && tp.x < 2.5 && tp.z > 1 && tp.z < 3.5)
				{
					joints[i] = PxD6JointCreate(*m_physx->getPhysicsSDK(), pollActor2, PxTransform(PxVec3(tp.z, 0, 0)), folders[i], PxTransform(PxVec3(0.3, 0.3, 0), PxQuat(PxHalfPi, PxVec3(0, 1, 0))));
					((PxD6Joint*)joints[i])->setMotion(PxD6Axis::eX, PxD6Motion::eLIMITED);

				}
				else if (tp.x > -2 && tp.x < 2 && tp.y <1.5 && tp.z > -3.5 && tp.z < -1)
				{
					bool exist = false;
					for (int m = 0; m < contents.size(); m++)
					{
						if (i == contents[m]->associateFolder)
						{
							exist = true;

							contents[m]->rendering = true;
							PxShape* shapes;
							contents[m]->board->getShapes(&shapes, 1);
							shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, true);
							PxVec3 t = tp;
							t.z = t.z-0.2;
							updateContent(contents[m], t);
							if (mHandlist.size() > 0 && contents[m]->zoomStart == false)
							{
								PxTransform thump = mHandlist[0]->finger_tip_actor[0]->getGlobalPose();
								PxTransform index = mHandlist[0]->finger_tip_actor[1]->getGlobalPose();
								float pinch = mHandlist[0]->pinchDistance;

								if (abs(thump.p.x - tp.x) < 0.5 && abs(thump.p.z - tp.z) < 0.5 &&
									abs(index.p.x - tp.x) < 0.5 && abs(index.p.z - tp.z) < 0.5 && thump.p.y>2.0f&& pinch > 0.5)
								{
									contents[m]->pinchStart = pinch;
									contents[m]->zoomStart = true;
								}
							}
							else if (mHandlist.size() > 0 && contents[m]->zoomStart)
							{
								float pinch = mHandlist[0]->pinchDistance;
								contents[m]->scale += contents[m]->pinchStart - pinch;
								PxTransform thump = mHandlist[0]->finger_tip_actor[0]->getGlobalPose();
								PxTransform index = mHandlist[0]->finger_tip_actor[1]->getGlobalPose();

								if (contents[m]->scale > 1.5)
									contents[m]->scale = 1.5;
								else if (contents[m]->scale < 0.5)
									contents[m]->scale = 0.5;

								if (abs(thump.p.x - tp.x) > 0.5 || abs(thump.p.z - tp.z) < 0.5 ||
									abs(index.p.x - tp.x) > 0.5 || abs(index.p.z - tp.z) < 0.5 ||thump.p.y<2.0f)
								{
									contents[m]->zoomStart = false;
								}

							}
						}
							
					}
					if (!exist)
					{
						PxVec3 t = tp;
						t.z = t.z - 0.2;
						createContent(i, tp);
					}
						
				}
				else
				{
					for (int m = 0; m < contents.size(); m++)
					{

						if (i == contents[m]->associateFolder)
						{
							contents[m]->rendering = false;
							PxShape* shapes;
							contents[m]->board->getShapes(&shapes, 1);
							shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
						}
					}
				}
			


			}

		}
	}
	else if (gameMode == 9)
	{
		if (globe != NULL)
		{
			bool userTouch = false;
			PxVec3 g = globe->getAngularVelocity();
			if (g.x != 0 || g.y != 0 || g.z != 0)
			{
				PxVec3 gm = g *0.8;
				globe->setAngularVelocity(gm);
				userTouch = true;
			}
			if (userTouch)
			{
				if (g.magnitude() < 0.1)
				{
					start = std::chrono::steady_clock::now();
					countstart = true;
				}

			}
			else if (countstart)
			{
				end = std::chrono::steady_clock::now();
				long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
				if (mils < 10000000)
				{
					mapAlpha = mapAlpha - (mils / 10000000.0);
				}
				else
					mapAlpha = 0.0f;
			}

		}

		PxTransform p = pins.back()->getGlobalPose();
		if (p.p.x < 0.75 || p.p.x > 1.25 || p.p.y < 2.25 || p.p.z <-2.25 || p.p.z > -1.75)
		{
			createPin(PxVec3(1, 2.5, -2));
		}



		for (int i = 0; i < pins.size(); i++)
		{
			PxTransform p = pins[i]->getGlobalPose();
			p.q = PxQuat::createIdentity();
			((PxRigidDynamic*)pins[i])->setLinearDamping(0);
			((PxRigidDynamic*)pins[i])->setLinearVelocity(PxVec3(0, -0, 0));
			pins[i]->setGlobalPose(p);

			if (p.p.y < 1.26)
			{
				if(!showContent)
					start = std::chrono::steady_clock::now();

				showContent = true;
				
			}


		}



		if (showContent)
		{
			end = std::chrono::steady_clock::now();
			long long mils = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			if (mils < 100000)
			{
				picAlpha = picAlpha + (mils / 100000.0);
			}
			else
				mils = 100000.0;

		}
	}
}

void GraphicsClass::createContent(int folderNo, PxVec3 Position)
{
	content* newcontent = new content;
	newcontent->associateFolder = folderNo;
	PxVec3 pos = PxVec3(Position.x, 2.5, Position.z);
	newcontent->core = PxCreateDynamic(*m_physx->getPhysicsSDK(), PxTransform(pos), PxCapsuleGeometry(0.1, 0.26), *m_physx->mMaterial, 1.0);
	newcontent->board = m_physx->createBox(PxVec3(0.26, 0.25, 0.05), pos, PxQuat::createIdentity());
	newcontent->board->setMass(100.0f);
	newcontent->board->setMassSpaceInertiaTensor(PxVec3(110, 0, 0));
	newcontent->core->setRigidDynamicFlag(PxRigidBodyFlag::eKINEMATIC, true);
	PxShape* shapes;
	newcontent->core->getShapes(&shapes, 1);
	shapes->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);

	newcontent->scale = 1.0f;
	newcontent->zoomStart = false;
	newcontent->rendering = true;


	PxRevoluteJoint* joint = PxRevoluteJointCreate(*m_physx->getPhysicsSDK(), newcontent->core, PxTransform(PxVec3(0, 0, 0)), newcontent->board, PxTransform(PxVec3(0, 0, 0)));
	joint->setLimit(PxJointAngularLimitPair(0, PxPi / 3, 0.01f));
	joint->setRevoluteJointFlag(PxRevoluteJointFlag::eLIMIT_ENABLED, true);
	m_physx->getScene()->addActor(*newcontent->core);
	m_physx->getScene()->addActor(*newcontent->board);
	contents.push_back(newcontent);
}

void GraphicsClass::deleteContent(content* con)
{
	if (con != NULL)
	{
		m_physx->removeActor(con->core);
		m_physx->removeActor(con->board);

	}
}

void GraphicsClass::createPin(PxVec3 Position)
{
	PxRigidDynamic*box = m_physx->createBox(PxVec3(0.1, 0.25, 0.1), Position, PxQuat::createIdentity());
	//box->set
	//box->setMassSpaceInertiaTensor(PxVec3(0, 0, 0));
	box->setRigidDynamicFlag(PxRigidBodyFlag::eKINEMATIC, true);
	pins.push_back(box);
	m_physx->getScene()->addActor(*box);
}

void GraphicsClass::updateContent(content* c, PxVec3 Position)
{
	PxVec3 pos = PxVec3(Position.x, 2, Position.z);
	c->core->setKinematicTarget(PxTransform(pos));
}



void GraphicsClass::RenderPointCloud()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);

	m_PointCloudShader->Render(m_Direct3D->GetDeviceContext(), worldMatrix, viewMatrix, projectionMatrix, m_Camera->GetPosition());
}

void GraphicsClass::RenderText(bool Head_Error, Point3f* head_pos)
{
	spriteBatch->Begin();
	std::vector<handActor*> handactor = m_Leap->getHandActor();

		std::wstring mouse = L"+";
		m_font->DrawString(spriteBatch.get(), mouse.c_str(), XMFLOAT2(mouseX, mouseY), Colors::Red);
	

	if (Head_Error)
	{
		m_font->DrawString(spriteBatch.get(), L"Multiple Red Detected!", XMFLOAT2(100, 50), Colors::Red);
	}
	if (head_pos != nullptr)
	{
		std::wstring s = L"X: " + std::to_wstring(head_pos->x) + L"Y: " + std::to_wstring(head_pos->y) + L"Z: " + std::to_wstring(head_pos->z);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 50), Colors::Red);
	}
	if (!gogoMode )
	{
		std::wstring s = L"Go go: false";
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 100), Colors::Green);
	}
	else if (gogoMode)
	{
		std::wstring s = L"Go go: true";
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 100), Colors::Green);
	}


	if (calibrateMode)
	{
		if (handactor.size() > 0)
		{
			if (handactor[0]->isExtended[1] && handactor[0]->isExtended[2] && handactor[0]->isExtended[3] && handactor[0]->isExtended[4] && handactor[0]->isExtended[0] && !FULL_SCREEN)
			{
				fingertipDetected = m_Tracker->fingerTipDetection(handactor[0]->fingerTipPosition);
			}

			std::wstring s = L"hand size: " + std::to_wstring(handlength) /*+ L"T:" + std::to_wstring((int)handactor[0]->isExtended[0])
																		  + L"I:" + std::to_wstring((int)handactor[0]->isExtended[1]) + L"M:" + std::to_wstring((int)handactor[0]->isExtended[2])
																		  + L"R:" + std::to_wstring((int)handactor[0]->isExtended[3]) + L"P:" + std::to_wstring((int)handactor[0]->isExtended[4]) */
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
	/*	if (countDownTimer > 0)
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
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);*/
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
	else if (gameMode == 4 || gameMode == 5 || gameMode == 6)
	{
		std::wstring s = L"Box No. : " + std::to_wstring(logCount+1);
		m_font->DrawString(spriteBatch.get(), s.c_str(), XMFLOAT2(100, 200), Colors::GreenYellow);
	}

	spriteBatch->End();

	m_Direct3D->TurnOffAlphaBlending();
	m_Direct3D->TurnZBufferOn();
}

bool GraphicsClass::RenderSceneToTexture()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	if (SHADOW_ENABLED)
		m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	else
		m_RenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);
	//Generate the view matrix of the light.

	if (SHADOW_ENABLED)
	{
		// Generate the light view matrix based on the light's position.
		m_Light->GenerateViewMatrix();

		
		RenderActor(0);
		RenderHand(0);
		RenderTerrian(0);
		RenderTexActor(0);
		RenderRemoteHead(0, m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z);
	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderSceneToTexture2()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture2->SetRenderTarget(m_Direct3D->GetDeviceContext());

	// Clear the render to texture.
	if (SHADOW_ENABLED)
		m_RenderTexture2->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
	else
		m_RenderTexture2->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);
	//Generate the view matrix of the light.

	if (SHADOW_ENABLED)
	{
		// Generate the light view matrix based on the light's position.
		m_Light->GenerateMirrorMatrix();
		
		RenderTerrian(1);
		RenderActor(1);
		RenderHand(1);
		RenderTexActor(1);
		RenderRemoteHead(1, m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z);

	}

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_Direct3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_Direct3D->ResetViewport();

	return true;
}

bool GraphicsClass::RenderSceneToTexture3()
{
	bool result;

	// Set the render target to be the render to texture.
	m_RenderTexture3->SetRenderTarget(m_Direct3D->GetDeviceContext());


	m_RenderTexture3->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);


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
	if (SHADOW_ENABLED)
	{
		m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);
		// Generate the light view matrix based on the light's position.
		m_Light->GenerateViewMatrix();

		// Get the world, view, and projection matrices from the camera and d3d objects.
		m_Camera->GetViewMatrix(viewMatrix);
		m_Direct3D->GetWorldMatrix(worldMatrix);
		m_Direct3D->GetProjectionMatrix(projectionMatrix);

		// Get the light's view and projection matrices from the light object.
		m_Light->GetViewMatrix(lightViewMatrix);
		m_Light->GetProjectionMatrix(lightProjectionMatrix);

		RenderTerrian(2);

		RenderHand(2);

		RenderActor(2);
		RenderTexActor(2);
		RenderRemoteHead(2, m_RemoteCamera->GetPosition().x, m_RemoteCamera->GetPosition().y, m_RemoteCamera->GetPosition().z);
	}
	else
		m_BlackWhiteRenderTexture->ClearRenderTarget(m_Direct3D->GetDeviceContext(), 1.0f, 1.0f, 1.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	//m_Camera->Render();

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
	m_Camera->Render(XMFLOAT3(0.0f, 0.0f, 0.0f));

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
	m_Camera->Render(XMFLOAT3(0, 0, 0));

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
	if (SHADOW_ENABLED)
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_VerticalBlurTexture->GetShaderResourceView());
	else
		result = m_TextureShader->Render(m_Direct3D->GetDeviceContext(), m_FullScreenWindow->GetIndexCount(), worldMatrix, baseViewMatrix, orthoMatrix,
			m_RenderTexture->GetShaderResourceView());
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

void GraphicsClass::toggleGOGO()
{
	gogoMode = !gogoMode;
	m_Leap->setHandMode(gogoMode);
}

void GraphicsClass::checkObjectPos(PxRigidActor* actor)
{
	if (((PxRigidDynamic*)actor)->isSleeping())
	{
		PxTransform t = actor->getGlobalPose();
		if (t.p.z > -1 && t.p.z <= 0)
		{
			((PxRigidDynamic*)actor)->setLinearVelocity(PxVec3(0, 2, -3));
		}
		else if (t.p.z < 1 && t.p.z > 0)
		{
			((PxRigidDynamic*)actor)->setLinearVelocity(PxVec3(0, 2, 3));
		}
	}
}

PxRigidDynamic* GraphicsClass::TestIntersection(int mouseX, int mouseY)
{

	PxVec3 origin;
	PxVec3 dir;
	PxReal maxDistance = 30;
	PxRaycastBuffer hit;
	ViewUnProject(mouseX, mouseY, 0.0f, origin);
	ViewUnProject(mouseX, mouseY, 1.0f, dir);
	dir = dir - origin;
	dir = dir.getNormalized();


	PxQueryFilterData filterData = PxQueryFilterData(PxQueryFlag::eDYNAMIC);
	filterData.data.word0 = FilterGroup::eBox;

	PxRigidDynamic* hitactor;
	bool status = m_physx->getScene()->raycast(origin, dir, maxDistance, hit, PxHitFlags(PxHitFlag::eDEFAULT), filterData);
	if (status)
	{
		return (PxRigidDynamic*)hit.block.actor;

	}
	return NULL;

}

void GraphicsClass::ViewUnProject(int xi, int yi, float depth, PxVec3 &v)
{

	//yi = m_screenHeight - yi - 1;
	XMMATRIX projectionMatrix, viewMatrix,worldMatrix;
	worldMatrix = XMMatrixIdentity();
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	XMVECTOR xv = XMLoadFloat3(&XMFLOAT3(xi,yi,depth));
	
	XMVECTOR vi = XMVector3Unproject(xv, 0, 0, m_screenWidth, m_screenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, worldMatrix);

	XMFLOAT3 vs;
	XMStoreFloat3(&vs, vi);

	v.x = vs.x;
	v.y = vs.y;
	v.z = vs.z;
}

void GraphicsClass::ViewProject(int xi, int yi, float zi, PxVec3 &v)
{

	//yi = m_screenHeight - yi - 1;
	XMMATRIX projectionMatrix, viewMatrix, worldMatrix;
	worldMatrix = XMMatrixIdentity();
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	XMVECTOR xv = XMLoadFloat3(&XMFLOAT3(xi, yi, zi));

	XMVECTOR vi = XMVector3Project(xv, 0, 0, m_screenWidth, m_screenHeight, 0.0f, 1.0f, projectionMatrix, viewMatrix, worldMatrix);

	XMFLOAT3 vs;
	XMStoreFloat3(&vs, vi);

	v.x = vs.x;
	v.y = vs.y;
	v.z = vs.z;
}

void GraphicsClass::UpdateFlexFingerTip(vector<handActor*> handlist)
{
	m_flex->ClearShapes();
	for (int i = 0; i < handlist.size(); i++)
	{
		for (int j = 0; j < 5; j++)
		{
			m_flex->AddSphere(handlist[i]->fingerWidth[j][3]/4.0f, Vec3(handlist[i]->fingerTipWorldPosition[j].x, handlist[i]->fingerTipWorldPosition[j].y, handlist[i]->fingerTipWorldPosition[j].z), QuatFromAxisAngle(Vec3(0, 1, 0), 0));
			

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
					{
						PxTransform p = mHandlist[i]->finger_actor[j][k]->getGlobalPose();
						Vec3 pos = Vec3(p.p.x, p.p.y, p.p.z);
						float angle;
						PxVec3 axis;
						p.q.toRadiansAndUnitAxis(angle, axis);
						Quat q = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

						PxTransform prev = mHandlist[i]->previousTransform[j][k];

						Vec3 prevpos = Vec3(prev.p.x, prev.p.y, prev.p.z);
						prev.q.toRadiansAndUnitAxis(angle, axis);
						Quat prevq = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

						float radius = handlist[i]->fingerWidth[j][k] / 4.0f;
						float halfHeight = handlist[i]->halfHeight[j][k]/2.0f;
						m_flex->AddCapsule(radius, halfHeight, pos, q,pos,q);
					}

					PxTransform p = mHandlist[i]->finger_actor[j][1]->getGlobalPose();
					Vec3 pos = Vec3(p.p.x, p.p.y, p.p.z);
					float angle;
					PxVec3 axis;
					p.q.toRadiansAndUnitAxis(angle, axis);
					Quat q = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

					PxTransform prev = mHandlist[i]->previousTransform[j][1];

					Vec3 prevpos = Vec3(prev.p.x, prev.p.y, prev.p.z);
					prev.q.toRadiansAndUnitAxis(angle, axis);
					Quat prevq = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

					float radius = handlist[i]->fingerWidth[j][1] / 4.0f;
					float halfHeight = handlist[i]->halfHeight[j][1]/2.0f;
					m_flex->AddBox(Vec3(halfHeight, radius, radius / 2), pos, q, pos, q);
					
				}
				else
				{
					for (int k = 1; k < 4; k++)
					{
						PxTransform p = mHandlist[i]->finger_actor[j][k]->getGlobalPose();
						Vec3 pos = Vec3(p.p.x, p.p.y, p.p.z);
						float angle;
						PxVec3 axis;
						p.q.toRadiansAndUnitAxis(angle, axis);
						Quat q = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);


						PxTransform prev = mHandlist[i]->previousTransform[j][k];

						Vec3 prevpos = Vec3(prev.p.x, prev.p.y, prev.p.z);
						prev.q.toRadiansAndUnitAxis(angle, axis);
						Quat prevq = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

						float radius = handlist[i]->fingerWidth[j][k] / 4.0f;
						float halfHeight = handlist[i]->halfHeight[j][k] / 2.0f;
						m_flex->AddCapsule(radius, halfHeight, pos, q, pos, q);
					}

					PxTransform p = mHandlist[i]->finger_actor[j][0]->getGlobalPose();
					Vec3 pos = Vec3(p.p.x, p.p.y, p.p.z);
					float angle;
					PxVec3 axis;
					p.q.toRadiansAndUnitAxis(angle, axis);
					Quat q = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);

					PxTransform prev = mHandlist[i]->previousTransform[j][0];

					Vec3 prevpos = Vec3(prev.p.x, prev.p.y, prev.p.z);
					prev.q.toRadiansAndUnitAxis(angle, axis);
					Quat prevq = QuatFromAxisAngle(Vec3(axis.x, axis.y, axis.z), angle);


					float radius = handlist[i]->fingerWidth[j][0] / 4.0f;
					float halfHeight = handlist[i]->halfHeight[j][0]/2.0f;
					m_flex->AddBox(Vec3(halfHeight, radius, radius / 2), pos, q,pos, q);
				}
		}
	}
}

void GraphicsClass::EncodedMessage(SceneMsg* scnmsg)
{
	mHandlist = m_Leap->getHandActor();


	for (int i = 0; i < mHandlist.size(); i++)
	{
			for (int j = 0; j < 5; j++)
			{
				VObject* objects = scnmsg->add_scene_objects();
				PxToVObject(objects, mHandlist[i]->finger_tip_actor[j], VObject::VObjectType::VObject_VObjectType_rFinger_tip, mHandlist[i]->fingerWidth[j][3] / 2.0, 0.0f,0.0f, mrHandMaterial.Diffuse);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
					{
						VObject* objects = scnmsg->add_scene_objects();
						PxToVObject(objects, mHandlist[i]->finger_actor[j][k], VObject::VObjectType::VObject_VObjectType_rFinger, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], 0.0f, mrHandMaterial.Diffuse);
					}
						
					VObject* objects = scnmsg->add_scene_objects();
					PxToVObject(objects, mHandlist[i]->finger_actor[j][1], VObject::VObjectType::VObject_VObjectType_rPalm, mHandlist[i]->halfHeight[j][1] * 2.0, mHandlist[i]->fingerWidth[j][1], mHandlist[i]->fingerWidth[j][1] / 2, mrHandMaterial.Diffuse);
				}
				else
				{
					for (int k = 1; k < 4; k++)
					{
						VObject* objects = scnmsg->add_scene_objects();
						PxToVObject(objects, mHandlist[i]->finger_actor[j][k], VObject::VObjectType::VObject_VObjectType_rFinger, mHandlist[i]->halfHeight[j][k] * 2.0, mHandlist[i]->fingerWidth[j][k], 0.0f, mrHandMaterial.Diffuse);
					}

					VObject* objects = scnmsg->add_scene_objects();
					PxToVObject(objects, mHandlist[i]->finger_actor[j][0], VObject::VObjectType::VObject_VObjectType_rPalm, mHandlist[i]->halfHeight[j][0] * 2.0, mHandlist[i]->fingerWidth[j][0], mHandlist[i]->fingerWidth[j][0] / 2, mrHandMaterial.Diffuse);
				}
			}
		
	}


	for (int i = 0; i < mRHandlist.size(); i++)
	{
		if (!mRHandlist[i]->toBeDelete)
		{
			for (int j = 0; j < 5; j++)
			{
				VObject* objects = scnmsg->add_scene_objects();
				PxToVObject(objects, mRHandlist[i]->finger_tip_actor[j], VObject::VObjectType::VObject_VObjectType_Finger_tip, mRHandlist[i]->fingerWidth[j][3] / 2.0, 0.0f, 0.0f, mhandMaterial.Diffuse);

				if (j == 0)
				{
					for (int k = 2; k < 4; k++)
					{
						VObject* objects = scnmsg->add_scene_objects();
						PxToVObject(objects, mRHandlist[i]->finger_actor[j][k], VObject::VObjectType::VObject_VObjectType_Finger, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], 0.0f, mhandMaterial.Diffuse);
					}

					VObject* objects = scnmsg->add_scene_objects();
					PxToVObject(objects, mRHandlist[i]->finger_actor[j][1], VObject::VObjectType::VObject_VObjectType_Palm, mRHandlist[i]->halfHeight[j][1] * 2.0, mRHandlist[i]->fingerWidth[j][1], mRHandlist[i]->fingerWidth[j][1] / 2, mhandMaterial.Diffuse);
				}
				else
				{
					for (int k = 1; k < 4; k++)
					{
						VObject* objects = scnmsg->add_scene_objects();
						PxToVObject(objects, mRHandlist[i]->finger_actor[j][k], VObject::VObjectType::VObject_VObjectType_Finger, mRHandlist[i]->halfHeight[j][k] * 2.0, mRHandlist[i]->fingerWidth[j][k], 0.0f, mhandMaterial.Diffuse);
					}

					VObject* objects = scnmsg->add_scene_objects();
					PxToVObject(objects, mRHandlist[i]->finger_actor[j][0], VObject::VObjectType::VObject_VObjectType_Palm, mRHandlist[i]->halfHeight[j][0] * 2.0, mRHandlist[i]->fingerWidth[j][0], mRHandlist[i]->fingerWidth[j][0] / 2, mhandMaterial.Diffuse);
				}
			}
		}


	}

	for (int i = 0; i < boxes.size(); i++)
	{
		VObject* objects = scnmsg->add_scene_objects();
		PxToVObject(objects, boxes[i], VObject::VObjectType::VObject_VObjectType_Box, BOX_SIZE.x, BOX_SIZE.y, BOX_SIZE.z, mboxMaterial.Diffuse);

	}

	for (int i = 0; i < spheres.size(); i++)
	{
		VObject* objects = scnmsg->add_scene_objects();
		PxToVObject(objects, spheres[i], VObject::VObjectType::VObject_VObjectType_Box, SPHERE_RAD, BOX_SIZE.y, BOX_SIZE.z, msphereMaterial.Diffuse);

	}
}

void GraphicsClass::PxToVObject(VObject* object, PxRigidActor* actor, VObject::VObjectType type, float size_x, float size_y, float size_z, XMFLOAT4 color)
{
	if (actor != NULL)
	{
		PxTransform transform = actor->getGlobalPose();
		PxVec3 p = transform.p;
		PxQuat q = transform.q;

		object->set_type(type);
		object->set_size_x(size_x);
		object->set_size_y(size_y);
		object->set_size_z(size_z);
		object->set_p_x(-p.x);
		object->set_p_y(p.y);
		object->set_p_z(-5.0-(p.z));
		object->set_q_w(q.x);
		object->set_q_x(q.w);
		object->set_q_y(-q.z);
		object->set_q_z(-q.y);


		object->set_color_w(color.w);
		object->set_color_x(color.x);
		object->set_color_y(color.y);
		object->set_color_z(color.z);
	}


}