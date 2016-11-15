////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


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
	if(!result)
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


	// Create the camera object.
	m_Camera = new CameraClass;
	if (!m_Camera)
	{
		return false;
	}

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 1.5f, -8.0f);

	m_Light = new LightClass;
	if (!m_Light)
	{
		return false;
	}
	m_Light->SetDirection(0.0f, 0.0f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f,1.0f);
	m_Light->SetAmbientColor(0.5f, 0.5f, 0.5f, 1.0f);

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

	m_spheres = new ModelClass;
	if (!m_boxes)
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

	m_TextureShader = new TextureShaderClass;
	if (!m_LightShader)
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

	m_cylinder = GeometricPrimitive::CreateCylinder(m_Direct3D->GetDeviceContext(),1.0f,1.0f,32Ui64,false);
	m_palm = GeometricPrimitive::CreateBox(m_Direct3D->GetDeviceContext(), XMFLOAT3(1.0f, 1.0f, 1.0f),false);


	globalZ = 0;
	globalFOV = 26.6;

	StopCalibrate(false);
	
	return true;
}

void GraphicsClass::StopCalibrate(bool writeMode)
{
	for (int i = 0; i < 5; i++)
	{
		PxRigidActor* box = m_physx->createBox(PxVec3(0.25, 0.25, 0.25), PxVec3(0, i, -3), PxQuat::createIdentity());
		boxes.push_back(box);
	}
	for (int i = 0; i < 5; i++)
	{
		PxRigidActor* sphere = m_physx->createSphere(0.25, PxVec3(i - 3.0f, 0, -3), PxQuat::createIdentity());
		spheres.push_back(sphere);
	}
	calibrateMode = false;
}

void GraphicsClass::StartCalibrate()
{

}

void GraphicsClass::StartCalibrateOtho()
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
		boxes.clear();
		spheres.clear();
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

		m_physx->moveWall(globalZ);
	}
}

void GraphicsClass::RenderActor()
{
	for (int i = 0; i < boxes.size(); i++)
	{
		RenderBox(boxes[i], 0.25, 0.25, 0.25f);
	}
	for (int i = 0; i < spheres.size(); i++)
	{
		RenderSphere(spheres[i], 0.25);
	}
}

void GraphicsClass::RenderHand()
{
	mHandlist = m_Leap->getHandActor();
	for (int i = 0; i< mHandlist.size(); i++)
	{
		for (int j = 0; j < 5; j++)
			for (int k = 0; k < 3; k++)
				RenderCylinder(mHandlist[i].finger_actor[j][k], mHandlist[i].halfHeight[j][k]*2.0, mHandlist[i].fingerWidth[j][k]);

		PxVec3 dimension = mHandlist[i].palmDimension;
		RenderPalm(mHandlist[i].palm, dimension.x, dimension.y, dimension.z);
	}
}

void GraphicsClass::RenderBox(PxRigidActor* box, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width, height, depth);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_boxes->Render(m_Direct3D->GetDevice());

		// Render the model using the color shader.
		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_boxes->GetIndexCount(), mat, viewMatrix, projectionMatrix, m_boxes->GetTexture(),
			m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition());
	}
}

void GraphicsClass::RenderPalm(PxRigidActor* box, float width, float height, float depth)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	PxU32 nShapes = box->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	box->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *box);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(width, height, depth);
		XMMATRIX r = XMMatrixRotationX(90 * XM_PI / 180);
		mat = s*r*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_palm->Draw(mat, viewMatrix, projectionMatrix);
	}
}

void GraphicsClass::RenderSphere(PxRigidActor* sphere, float radius)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	PxU32 nShapes = sphere->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	sphere->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *sphere);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX s = XMMatrixScaling(radius, radius, radius);
		mat = s*mat;

		// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
		m_spheres->Render(m_Direct3D->GetDevice());

		// Render the model using the color shader.
		m_LightShader->Render(m_Direct3D->GetDeviceContext(), m_spheres->GetIndexCount(), mat, viewMatrix, projectionMatrix, m_spheres->GetTexture(),
			m_Light->GetDirection(), m_Light->GetDiffuseColor(), m_Light->GetAmbientColor(), m_Camera->GetPosition());
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

void GraphicsClass::RenderCylinder(PxRigidActor* cylinder, float height, float radius)
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	PxU32 nShapes = cylinder->getNbShapes();
	PxShape** shapes = new PxShape*[nShapes];

	cylinder->getShapes(shapes, nShapes);
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);


	while (nShapes--)
	{

		PxTransform pT = PxShapeExt::getGlobalPose(*shapes[nShapes], *cylinder);
		XMMATRIX mat = PxtoXMMatrix(pT);
		XMMATRIX r = XMMatrixRotationZ(90 * XM_PI / 180);
		XMMATRIX s = XMMatrixScaling(radius, height, radius);
		mat = s*r*mat;

		m_cylinder->Draw(mat, viewMatrix, projectionMatrix);

	}
}

void GraphicsClass::RenderTerrian()
{
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	XMMATRIX trans = XMMatrixTranslation(-3.0f, 0.0f, -4.0f+globalZ);
	XMMATRIX back = XMMatrixTranslation(-3.0f, 0.0f, 0.0f+globalZ);
	XMMATRIX rotate = XMMatrixRotationX(-XM_PI / 2);
	XMMATRIX slant = XMMatrixRotationX(187*XM_PI / 180);
	//XMMATRIX mat = rotat


	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(),m_Terrain->GetIndexCount(), rotate*back, viewMatrix, projectionMatrix, m_Camera->GetPosition());

	m_Terrain->Render(m_Direct3D->GetDeviceContext());

	m_ColorShader->Render(m_Direct3D->GetDeviceContext(), m_Terrain->GetIndexCount(), slant*back, viewMatrix, projectionMatrix, m_Camera->GetPosition());

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
	// Release the color shader object.
	if (m_ColorShader)
	{
		m_ColorShader->Shutdown();
		delete m_ColorShader;
		m_ColorShader = 0;
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

	return;
}


bool GraphicsClass::Frame()
{

	bool result;


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
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMFLOAT4X4 proj;
	bool result;




	// Clear the buffers to begin the scene.
	m_Direct3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	m_3dvision->Render(m_Direct3D->GetDevice(), m_Direct3D->GetDeviceContext(),8.0+globalZ);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render(XMFLOAT3(0.0f,1.5f,0.0f));

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Direct3D->GetWorldMatrix(worldMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_Direct3D->GetProjectionMatrix(projectionMatrix);
	
	XMStoreFloat4x4(&proj, projectionMatrix);
	
	m_Leap->processFrame(0, 1.5, -8.0, globalZ, proj._11, proj._22);


	m_physx->Render();
	
	RenderTerrian();
	if(m_RenderHand)
	RenderHand();
	RenderActor();

	// Present the rendered scene to the screen.
	m_Direct3D->EndScene();

	return true;
}