////////////////////////////////////////////////////////////////////////////////
// Filename: inputclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _INPUTCLASS_H_
#define _INPUTCLASS_H_

///////////////////////////////
// PRE-PROCESSING DIRECTIVES //
///////////////////////////////
#define DIRECTINPUT_VERSION 0x0800

/////////////
// LINKING //
/////////////
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")

//////////////
// INCLUDES //
//////////////
#include <dinput.h>

////////////////////////////////////////////////////////////////////////////////
// Class name: InputClass
////////////////////////////////////////////////////////////////////////////////
class InputClass
{
public:
	InputClass();
	InputClass(const InputClass&);
	~InputClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

	bool IsCPressed();
	bool IsPPressed();
	bool IsBPressed();
	bool IsQPressed();
	bool IsOPressed();
	bool IsEscapePressed();
	bool IsUpPressed();
	bool IsDownPressed();
	bool IsPageDownPressed();
	bool IsNumpad1Pressed();
	bool IsNumpad2Pressed();
	bool IsNumpad3Pressed();
	bool IsNumpad4Pressed();
	bool IsNumpad6Pressed();
	bool IsNumpad7Pressed();
	bool IsNumpad8Pressed();
	bool IsNumpad9Pressed();
	bool IsaltPressed();
	bool IsPageUpPressed();
	bool IsIPressed();
	bool IsMPressed();
	bool IsLessPressed();
	bool IsGPressed();

	bool IsOnePressed();
	bool IsTwoPressed();
	bool IsThreePressed();
	bool IsFourPressed();
	bool IsFivePressed();
	bool IsSixPressed();

	bool IsSpacePressed();

	bool IsLeftPressed();
	bool IsRightPressed();
	bool IsLeftMouseButtonDown();
	void GetMouseLocation(int&, int&);

private:
	bool ReadKeyboard();
	bool ReadMouse();
	void ProcessInput();

private:
	IDirectInput8* m_directInput;
	IDirectInputDevice8* m_keyboard;
	IDirectInputDevice8* m_mouse;

	unsigned char m_keyboardState[256];
	DIMOUSESTATE m_mouseState;

	int m_screenWidth, m_screenHeight;
	int m_mouseX, m_mouseY;

	WORD pressKey = 0;
};

#endif