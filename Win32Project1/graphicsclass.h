#pragma once
////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_

///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"
#include "3dvisionclass.h"
#include "physxclass.h"
#include "lightshaderclass.h"
#include "lightclass.h"
#include "colorshaderclass.h"
#include "terrainclass.h"
#include "textclass.h"
#include "leapclass.h"
#include "bitmapclass.h"
#include "textureshaderclass.h"
#include "shapeclass.h"

#include "GeometricPrimitive.h"
#include "CommonStates.h"

#include <Eigen/Eigen>
#include <Eigen/SVD>
#include <Eigen/Geometry>
/////////////
// GLOBALS //
/////////////

using namespace Eigen;

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 0.1f;


const int NUM_POINT = 32;

////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	void StopCalibrate(bool);
	void StartCalibrate();
	void StartCalibrateOtho();
	void ChangeZvalue(float);
	void ChangeFOV(float);
	void ChangeSize(float);
	void CreateBox();
	void CreateSphere();
	bool m_RenderHand;


private:
	bool Render();
	void RenderActor();
	void RenderHand();
	void RenderBox(PxRigidActor* box, float width, float height, float depth);
	void RenderPalm(PxRigidActor* box, float width, float height, float depth);
	void RenderSphere(PxRigidActor* sphere, float radius);
	void RenderCylinder(PxRigidActor* cylinder,float height, float radius);
	void RenderTerrian();
	//void RenderPillar(PxVec3 endPoint);
	XMMATRIX PxtoXMMatrix(PxTransform input);

	

	//void WriteFile();
	//void ReadFile();






private:

	PxVec3 pts3[NUM_POINT];
	PxVec3 unpts3[NUM_POINT];
	bool calibrateMode;
	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_boxes;
	ModelClass* m_pillars;
	ModelClass* m_spheres;
	Vision3DClass* m_3dvision;
	PhysxClass* m_physx;
	LightClass* m_Light;
	LightShaderClass* m_LightShader;
	ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;
	TerrainClass* m_Terrain;
	TextClass* m_Text;
	LeapClass* m_Leap;
	BitmapClass* m_Bitmap;
	ShapeClass* m_Shape;

	XMMATRIX zoffset_matrix;


	vector<PxRigidActor*> boxes;
	vector<PxRigidActor*> spheres;

	std::unique_ptr<GeometricPrimitive> m_cylinder;
	std::unique_ptr<GeometricPrimitive> m_palm;

	vector<handActor> mHandlist;

	int captureIndex;
	int m_screenHeight;
	int m_screenWidth;
	float globalZ;
	float globalFOV;
	PxVec3 calibratePosition;
	float globalFactor;


};

#endif