////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _LIGHTCLASS_H_
#define _LIGHTCLASS_H_

//////////////
// INCLUDES //
//////////////
#include <DirectXMath.h>

using namespace DirectX;

////////////////////////////////////////////////////////////////////////////////
// Class name: LightClass
////////////////////////////////////////////////////////////////////////////////
class LightClass
{
public:
	LightClass();
	LightClass(const LightClass&);
	~LightClass();

	void SetAmbientColor(float, float, float, float);
	void SetDiffuseColor(float, float, float, float);
	void SetSpecularColor(float, float, float, float);
	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);

	XMFLOAT4 GetAmbientColor();
	XMFLOAT4 GetDiffuseColor();
	XMFLOAT4 GetSpecularColor();
	XMFLOAT3 GetPositions();

	void GenerateViewMatrix();
	void GenerateMirrorMatrix();
	void GenerateProjectionMatrix(float, float);

	void GetViewMatrix(XMMATRIX&);
	void GetMirrorMatrix(XMMATRIX & viewMatrix);
	void GetProjectionMatrix(XMMATRIX&);

private:
	XMFLOAT4 m_ambientColor;
	XMFLOAT4 m_diffuseColor;
	XMFLOAT4 m_specularColor;
	XMFLOAT3 m_direction;

	XMFLOAT3 m_position;
	XMFLOAT3 m_lookAt;
	XMMATRIX m_viewMatrix;
	XMMATRIX m_mirrorMatrix;
	XMMATRIX m_projectionMatrix;
};

#endif