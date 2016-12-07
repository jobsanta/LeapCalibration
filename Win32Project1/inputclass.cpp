////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "inputclass.h"


InputClass::InputClass()
{
	m_directInput = 0;
	m_keyboard = 0;
	m_mouse = 0;
}


InputClass::InputClass(const InputClass& other)
{
}


InputClass::~InputClass()
{
}


bool InputClass::Initialize(HINSTANCE hinstance, HWND hwnd, int screenWidth, int screenHeight)
{
	HRESULT result;


	// Store the screen size which will be used for positioning the mouse cursor.
	m_screenWidth = screenWidth;
	m_screenHeight = screenHeight;

	// Initialize the location of the mouse on the screen.
	m_mouseX = 0;
	m_mouseY = 0;

	// Initialize the main direct input interface.
	result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&m_directInput, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the keyboard.
	result = m_directInput->CreateDevice(GUID_SysKeyboard, &m_keyboard, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
	result = m_keyboard->SetDataFormat(&c_dfDIKeyboard);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the keyboard to not share with other programs.
	result = m_keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Now acquire the keyboard.
	result = m_keyboard->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	// Initialize the direct input interface for the mouse.
	result = m_directInput->CreateDevice(GUID_SysMouse, &m_mouse, NULL);
	if(FAILED(result))
	{
		return false;
	}

	// Set the data format for the mouse using the pre-defined mouse data format.
	result = m_mouse->SetDataFormat(&c_dfDIMouse);
	if(FAILED(result))
	{
		return false;
	}

	// Set the cooperative level of the mouse to share with other programs.
	result = m_mouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	if(FAILED(result))
	{
		return false;
	}

	// Acquire the mouse.
	result = m_mouse->Acquire();
	if(FAILED(result))
	{
		return false;
	}

	return true;
}


void InputClass::Shutdown()
{
	// Release the mouse.
	if(m_mouse)
	{
		m_mouse->Unacquire();
		m_mouse->Release();
		m_mouse = 0;
	}

	// Release the keyboard.
	if(m_keyboard)
	{
		m_keyboard->Unacquire();
		m_keyboard->Release();
		m_keyboard = 0;
	}

	// Release the main interface to direct input.
	if(m_directInput)
	{
		m_directInput->Release();
		m_directInput = 0;
	}

	return;
}


bool InputClass::Frame()
{
	bool result;


	// Read the current state of the keyboard.
	result = ReadKeyboard();
	if(!result)
	{
		return false;
	}

	// Read the current state of the mouse.
	result = ReadMouse();
	if(!result)
	{
		return false;
	}

	// Process the changes in the mouse and keyboard.
	ProcessInput();

	return true;
}


bool InputClass::ReadKeyboard()
{
	HRESULT result;


	// Read the keyboard device.
	result = m_keyboard->GetDeviceState(sizeof(m_keyboardState), (LPVOID)&m_keyboardState);
	if(FAILED(result))
	{
		// If the keyboard lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_keyboard->Acquire();
		}
		else
		{
			return false;
		}
	}
		
	return true;
}


bool InputClass::ReadMouse()
{
	HRESULT result;


	// Read the mouse device.
	result = m_mouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&m_mouseState);
	if(FAILED(result))
	{
		// If the mouse lost focus or was not acquired then try to get control back.
		if((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
		{
			m_mouse->Acquire();
		}
		else
		{
			return false;
		}
	}

	return true;
}


void InputClass::ProcessInput()
{
	// Update the location of the mouse cursor based on the change of the mouse location during the frame.
	m_mouseX += m_mouseState.lX;
	m_mouseY += m_mouseState.lY;

	// Ensure the mouse location doesn't exceed the screen width or height.
	if(m_mouseX < 0)  { m_mouseX = 0; }
	if(m_mouseY < 0)  { m_mouseY = 0; }
	
	if(m_mouseX > m_screenWidth)  { m_mouseX = m_screenWidth; }
	if(m_mouseY > m_screenHeight) { m_mouseY = m_screenHeight; }
	
	return;
}


bool InputClass::IsEscapePressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if((!(m_keyboardState[DIK_ESCAPE] & 0x80)) && (DIK_ESCAPE == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_ESCAPE] & 0x80)
		pressKey = DIK_ESCAPE;

	return false;
}


bool InputClass::IsCPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_C] & 0x80)) && (DIK_C == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_C] & 0x80)
		pressKey = DIK_C;

	return false;
}


bool InputClass::IsPPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_P] & 0x80)) && (DIK_P == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_P] & 0x80)
		pressKey = DIK_P;

	return false;
}


bool InputClass::IsBPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_B] & 0x80)) && (DIK_B == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_B] & 0x80)
		pressKey = DIK_B;

	return false;
}

bool InputClass::IsQPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_Q] & 0x80)) && (DIK_Q == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_Q] & 0x80)
		pressKey = DIK_Q;

	return false;
}

bool InputClass::IsOPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_O] & 0x80)) && (DIK_O == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_O] & 0x80)
		pressKey = DIK_O;

	return false;
}


bool InputClass::IsUpPressed()
{

	if (m_keyboardState[DIK_UP] & 0x80)
		return true;

	return false;

}


bool InputClass::IsDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.

	if (m_keyboardState[DIK_DOWN] & 0x80)
		return true;

	return false;
}

bool InputClass::IsLeftPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.

	if (m_keyboardState[DIK_LEFT] & 0x80)
		return true;

	return false;
}


bool InputClass::IsRightPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.

	if (m_keyboardState[DIK_RIGHT] & 0x80)
		return true;

	return false;
}

bool InputClass::IsPageUpPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_PGUP] & 0x80)) && (DIK_PGUP == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_PGUP] & 0x80)
		pressKey = DIK_PGUP;

	return false;
}
bool InputClass::IsPageDownPressed()
{
	// Do a bitwise and on the keyboard state to check if the escape key is currently being pressed.
	if ((!(m_keyboardState[DIK_PGDN] & 0x80)) && (DIK_PGDN == pressKey))
	{
		pressKey = 0;
		return true;
	}
	if (m_keyboardState[DIK_PGDN] & 0x80)
		pressKey = DIK_PGDN;

	return false;
}


void InputClass::GetMouseLocation(int& mouseX, int& mouseY)
{
	mouseX = m_mouseX;
	mouseY = m_mouseY;
	return;
}