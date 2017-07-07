////////////////////////////////////////////////////////////////////////////////
// Filename: systemclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "systemclass.h"

SystemClass::SystemClass()
{
	m_Input = 0;
	m_Graphics = 0;
}

SystemClass::SystemClass(const SystemClass& other)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	int screenWidth, screenHeight;
	bool result;

	// Initialize the width and height of the screen to zero before sending the variables into the function.
	screenWidth = 0;
	screenHeight = 0;

	// Initialize the windows api.
	InitializeWindows(screenWidth, screenHeight);

	// Create the input object.  This object will be used to handle reading the keyboard input from the user.
	m_Input = new InputClass;
	if (!m_Input)
	{
		return false;
	}

	// Initialize the input object.
	result = m_Input->Initialize(m_hinstance, m_hwnd, screenWidth, screenHeight);
	if (!result)
	{
		MessageBox(m_hwnd, L"Could not initialize the input object.", L"Error", MB_OK);
		return false;
	}

	// Create the graphics object.  This object will handle rendering all the graphics for this application.
	m_Graphics = new GraphicsClass;
	if (!m_Graphics)
	{
		return false;
	}

	// Initialize the graphics object.
	result = m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
	if (!result)
	{
		return false;
	}

	return true;
}

void SystemClass::Shutdown()
{
	// Release the graphics object.
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = 0;
	}

	// Release the input object.
	if (m_Input)
	{
		m_Input->Shutdown();
		delete m_Input;
		m_Input = 0;
	}

	// Shutdown the window.
	ShutdownWindows();

	return;
}

void SystemClass::Run()
{
	MSG msg;
	bool done, result;

	// Initialize the message structure.
	ZeroMemory(&msg, sizeof(MSG));

	// Loop until there is a quit message from the window or the user.
	done = false;
	while (!done)
	{
		// Handle the windows messages.
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT)
		{
			done = true;
		}
		else
		{
			// Otherwise do the frame processing.
			result = Frame();
			if (!result)
			{
				done = true;
			}
		}
	}

	return;
}

bool SystemClass::Frame()
{
	bool result;
	int mouseX, mouseY;
	m_Input->Frame();
	m_Input->GetMouseLocation(mouseX, mouseY);


	// Check if the user pressed escape and wants to exit the application.
	if (m_Input->IsEscapePressed() == true)
	{
		return false;
	}

	if (m_Input->IsPPressed() == true)
	{
		m_Graphics->StartCalibrate();
	}

	if (m_Input->IsQPressed() == true)
	{
		//	m_Graphics->CalculateCalibration();
		m_Graphics->StopCalibrate(true);
	}

	if (m_Input->IsGPressed() == true)
	{
		m_Graphics->toggleGOGO();
	}

	if (m_Input->IsOnePressed() == true)
	{
		m_Graphics->StartGameMode(4);
	}

	if (m_Input->IsLessPressed() == true)
	{
		m_Graphics->m_RenderMirrorHand = !m_Graphics->m_RenderMirrorHand;
	}

	if (m_Input->IsTwoPressed() == true)
	{
		m_Graphics->StartGameMode(5);
	}

	if (m_Input->IsThreePressed() == true)
	{
		m_Graphics->StartGameMode(6);
	}

	if (m_Input->IsFourPressed() == true)
	{
		m_Graphics->setPointCloud();
	}

	if (m_Input->IsFivePressed() == true)
	{
		m_Graphics->StartGameMode(7);
	}
	if (m_Input->IsSixPressed() == true)
	{

		m_Graphics->StartGameMode(8);
	}

	//if (m_Input->IsCPressed() == true)
	//{
	//	m_Graphics->CaptureData();
	//}
	if (m_Input->IsBPressed() == true)
	{
		m_Graphics->CreateBox();
	}

	if (m_Input->IsOPressed() == true)
	{
		m_Graphics->CreateSphere();
	}

	if (m_Input->IsIPressed() == true)
	{
		m_Graphics->CreateCylinder();
	}

	if (m_Input->IsCPressed() == true)
	{
		m_Graphics->m_RenderHand = !m_Graphics->m_RenderHand;
	}

	if (m_Input->IsSpacePressed() == true)
	{
		m_Graphics->CaptureBoxPosition();
	}


	if (m_Input->IsMPressed() == true)
	{
		m_Graphics->ChangeMirrorHand();
	}

	if (m_Input->IsNumpad2Pressed() == true)
	{
		m_Graphics->ChangeZvalue(-0.025);
	}

	if (m_Input->IsNumpad8Pressed() == true)
	{
		m_Graphics->ChangeZvalue(0.025);
	}

	if (m_Input->IsNumpad1Pressed() == true)
	{
		m_Graphics->RotateActorZ(0.01);
	}

	if (m_Input->IsNumpad3Pressed() == true)
	{
		m_Graphics->RotateActorZ(-.01);
	}
	
	if (m_Input->IsNumpad7Pressed() == true)
	{
		m_Graphics->RotateActorX(0.01);
	}
	if (m_Input->IsNumpad9Pressed() == true)
	{
		m_Graphics->RotateActorX(-.01);
	}
	
	if (m_Input->IsNumpad4Pressed() == true)
	{
		m_Graphics->RotateActorY(0.01);
	}
	if (m_Input->IsNumpad6Pressed() == true)
	{
		m_Graphics->RotateActorY(-.01);
	}

	if (m_Input->IsPageDownPressed() == true)
	{
		m_Graphics->ChangeSize(-0.05);
	}

	if (m_Input->IsPageUpPressed() == true)
	{
		m_Graphics->ChangeSize(+0.05);
	}

	m_Graphics->setAltPressed(m_Input->IsaltPressed());
	// Do the frame processing for the graphics object.

	result = m_Graphics->Frame(mouseX, mouseY, m_Input->IsLeftMouseButtonDown());
	if (!result)
	{
		return false;
	}



	return true;
}

LRESULT CALLBACK SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	return DefWindowProc(hwnd, umsg, wparam, lparam);
}

void SystemClass::InitializeWindows(int& screenWidth, int& screenHeight)
{
	WNDCLASSEX wc;
	DEVMODE dmScreenSettings;
	int posX, posY;

	// Get an external pointer to this object.
	ApplicationHandle = this;

	// Get the instance of this application.
	m_hinstance = GetModuleHandle(NULL);

	// Give the application a name.
	m_applicationName = L"Engine";

	// Setup the windows class with default settings.
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = m_hinstance;
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = m_applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	// Determine the resolution of the clients desktop screen.
	screenWidth = GetSystemMetrics(SM_CXSCREEN);
	screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Setup the screen settings depending on whether it is running in full screen or in windowed mode.
	if (FULL_SCREEN)
	{
		// If full screen set the screen to maximum size of the users desktop and 32bit.
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);
		dmScreenSettings.dmPelsWidth = (unsigned long)screenWidth;
		dmScreenSettings.dmPelsHeight = (unsigned long)screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		// Change the display settings to full screen.
		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);

		// Set the position of the window to the top left corner.
		posX = posY = 0;
	}
	else
	{
		// If windowed then set it to 800x600 resolution.
		screenWidth = 1920;
		screenHeight = 1080;

		// Place the window in the middle of the screen.
		posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
		posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;
	}

	// Create the window with the screen settings and get the handle to it.
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_applicationName, m_applicationName,
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,
		posX, posY, screenWidth, screenHeight, NULL, NULL, m_hinstance, NULL);

	// Bring the window up on the screen and set it as main focus.
	ShowWindow(m_hwnd, SW_SHOW);
	SetForegroundWindow(m_hwnd);
	SetFocus(m_hwnd);

	// Hide the mouse cursor.
	ShowCursor(false);

	return;
}

void SystemClass::ShutdownWindows()
{
	// Show the mouse cursor.
	ShowCursor(true);

	// Fix the display settings if leaving full screen mode.
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// Remove the window.
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// Remove the application instance.
	UnregisterClass(m_applicationName, m_hinstance);
	m_hinstance = NULL;

	// Release the pointer to this class.
	ApplicationHandle = NULL;

	return;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
		// Check if the window is being destroyed.
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		return 0;
	}

	// Check if the window is being closed.
	case WM_CLOSE:
	{
		PostQuitMessage(0);
		return 0;
	}

	// All other messages pass to the message handler in the system class.
	default:
	{
		return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
	}
	}
}