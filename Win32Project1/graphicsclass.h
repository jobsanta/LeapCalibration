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
#include "rendertextureclass.h"
#include "depthshaderclass.h"
#include "colorshaderclass.h"
#include "terrainclass.h"
#include "textclass.h"
#include "leapclass.h"
#include "bitmapclass.h"
#include "textureshaderclass.h"
#include "shapeclass.h"
#include "pointcloudshaderclass.h"

#include "kinectclass.h"
#include "trackerclass.h"

#include "GeometricPrimitive.h"
#include "CommonStates.h"

#include "shadowshaderclass.h"
#include "orthowindowclass.h"
#include "horizontalblurshaderclass.h"
#include "verticalblurshaderclass.h"

#include <SpriteFont.h>
#include <Model.h>
#include <Effects.h>
#include <SimpleMath.h>

#include <Eigen/Eigen>
#include <Eigen/SVD>
#include <Eigen/Geometry>
#include "groundshaderclass.h"

#include <chrono>
/////////////
// GLOBALS //
/////////////

using namespace Eigen;

const std::string logfile = "Pa19";

const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = false;
const bool SHADOW_ENABLED = true;

const bool CORRECT_PERPECTIVE = true;
const float SCREEN_DEPTH = 50.0f;
const float SCREEN_NEAR = 1.0f;

const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

const PxVec3 BOX_SIZE(0.25, 0.25, 0.25);
const PxReal SPHERE_RAD = 0.35;
const PxVec3 BROAD_SIZE(1.0, 0.2, 0.75);
const PxVec3 TARGET_SIZE(0.4, 0.001, 0.4);
const PxVec3 BARRIER_SIZE_X(0.5, 0.2, 0.05);
const PxVec3 BARRIER_SIZE_Z(0.05, 0.2, 0.5);

const XMFLOAT4 TARGETCOLORHIT(0, 1.0f, 0.0, 1.0f);
const XMFLOAT4 TARGETCOLOR(0, 1.0f, 0.5, 1.0f);

const XMFLOAT4 HANDCOLORHIT(0, 1.0f, 0.0f, 0.5f);
const XMFLOAT4 HANDCOLOR(0, 1.0, 0.5, 1.0f);
const XMFLOAT3 LEFT(-3.0, 0.0, 0.0);
const XMFLOAT3 RIGHT(3.0, 0.0, 0.0);
const XMFLOAT3 TOP(-3.0, 3.4, 0.0);

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

	void ReadPath();

	void setPointCloud();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(int mousex, int mousey, bool LeftMouseDown);

	void StopCalibrate(bool);
	void StartCalibrate();
	void StartGameMode(int);
	void setAltPressed(bool);
	void ChangeZvalue(float);
	void RotateActorZ(float);
	void RotateActorX(float);
	void RotateActorY(float);
	void ChangeSize(float);
	void ChangeMirrorHand();
	void CreateBox();
	void Createfolder();
	void CreateSphere();
	void CreateCylinder();
	bool m_RenderHand;
	bool m_RenderMirrorHand;
	PxRigidDynamic* TestIntersection(int mouseX, int mouseY);
	void toggleGOGO();
	void CaptureBoxPosition();
private:
	bool Render();
	void RenderActor(int mode);
	void RenderTexActor(int mode);
	void RenderHand(int mode);
	void RenderText(bool, Point3f*);
	void RenderTextureBox(int mode, PxRigidActor* box,
		ID3D11ShaderResourceView* texture, float width, float height, float depth);
	void RenderTexture(int mode, PxTransform transform, ID3D11ShaderResourceView * texture, float width, float height, float depth);
	void RenderTargetBox(int mode, XMMATRIX transform, ID3D11ShaderResourceView * texture, float width, float height, float depth);
	void RenderAxis(XMMATRIX transform);

	void RenderColorBox(int mode, PxRigidActor* box,
		XMFLOAT4 color, float width, float height, float depth);
	void RenderPalm(int mode, PxRigidActor* box, float alpha, float width, float height, float depth);
	void RenderSphere(int mode, PxRigidActor* sphere, float radius);
	void RenderCylinder(int mode, PxRigidActor* cylinder, float alpha, float height, float radius);
	void RenderDebugSphere(int mode, PxVec3, float radius, XMFLOAT4 color);
	void RenderColorSphere(int mode, PxRigidActor* sphere, float radius, XMFLOAT4 color);
	void RenderTerrian(int rendermode, int gamemode = 0);
	void RenderPointCloud();
	//void RenderPillar(PxVec3 endPoint);
	XMMATRIX PxtoXMMatrix(PxTransform input);
	bool RenderSceneToTexture();
	bool RenderSceneToTexture2();
	bool RenderSceneToTexture3();
	bool RenderBlackAndWhiteShadows();
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();

	bool UpSampleTexture();
	void GameFrame();

	void WriteFile();
	void ReadFile();



private:
	bool POINT_CLOUD_ENABLED;
	PxVec3 pts3[NUM_POINT];
	PxVec3 unpts3[NUM_POINT];
	bool calibrateMode;
	int gameMode;
	bool altpressed;
	int handMode;
	int lastGameMode;

	D3DClass* m_Direct3D;
	CameraClass* m_Camera;
	ModelClass* m_boxes;
	ModelClass* m_spheres;
	ModelClass* m_ground;
	Vision3DClass* m_3dvision;
	PhysxClass* m_physx;
	LightClass* m_Light;
	LightShaderClass* m_LightShader;
	ColorShaderClass* m_ColorShader;
	TextureShaderClass* m_TextureShader;
	TextureClass* m_Target_Texture;
	TextureClass* m_folder_texture[19];
	TerrainClass* m_Terrain;
	TextClass* m_Text;
	LeapClass* m_Leap;
	BitmapClass* m_Bitmap;
	ShapeClass* m_Shape;
	KinectClass* m_Kinect;
	TrackerClass* m_Tracker;
	RenderTextureClass* m_RenderTexture, *m_RenderTexture2, *m_RenderTexture3;
	RenderTextureClass *m_BlackWhiteRenderTexture, *m_DownSampleTexure;
	RenderTextureClass *m_HorizontalBlurTexture, *m_VerticalBlurTexture, *m_UpSampleTexure;
	DepthShaderClass* m_DepthShader;
	GroundShaderClass* m_groundShader;
	ShadowShaderClass* m_ShadowShader;
	OrthoWindowClass *m_SmallWindow, *m_FullScreenWindow;
	HorizontalBlurShaderClass* m_HorizontalBlurShader;
	VerticalBlurShaderClass* m_VerticalBlurShader;
	PointCloudShaderClass* m_PointCloudShader;

	XMMATRIX zoffset_matrix;

	vector<PxRigidActor*> boxes;
	vector<PxRigidActor*> spheres;
	vector<PxRigidActor*> cylinders;
	vector<PxRigidActor*> folders;
	vector<PxRigidActor*> targets;
	vector<PxRigidActor*> obstacles;

	vector<PxJoint*> joints;

	std::unique_ptr<DirectX::SpriteBatch> spriteBatch;
	std::unique_ptr<DirectX::SpriteFont> m_font;
	std::unique_ptr<DirectX::SpriteFont> m_font_bold;

	vector<handActor*> mHandlist;
	vector<handActor*> mMirrorHandlist;

	PxTransform actorstartPos;
	PxTransform handStartPos;

	int captureIndex;
	int m_screenHeight;
	int m_screenWidth;
	float globalZ;
	float globalFOV;
	PxVec3 calibratePosition;
	float globalFactor;
	float handlength;
	int score;
	int count;
	int countDownTimer;
	int countFrame;
	int frameCalibrated;
	int fingertipDetected;
	bool targetHit;

	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;
	std::chrono::steady_clock::time_point smallStart;
	std::chrono::steady_clock::time_point smallNow;
	std::chrono::steady_clock::time_point switchZoneStart;
	std::chrono::steady_clock::time_point switchZoneNow;

	DirectX::SimpleMath::Matrix m_world;
	DirectX::SimpleMath::Matrix m_view;
	DirectX::SimpleMath::Matrix m_proj;

	std::unique_ptr<DirectX::CommonStates> m_states;
	std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
	std::unique_ptr<DirectX::Model> m_model;

	XMFLOAT4 targetColor;
	XMFLOAT4 handColor;
	XMMATRIX floor_trans;
	XMMATRIX floor_scale;

	FLOAT* rgbDest;
	FLOAT* depthDest;
	FLOAT* distDest;

	void checkObjectPos(PxRigidActor* actor);



	void ViewUnProject(int xi, int yi, float depth, PxVec3 & v);

	void ViewProject(int xi, int yi, float zi, PxVec3 & v);

	Material mhandMaterial;
	Material mboxMaterial;
	Material msphereMaterial;
	Material mfloorMaterial;
	Material medgeMaterial;
	Material mtextureMaterial;
	Material mtextMaterial;


	int mouseX;
	int mouseY;
	XMFLOAT3 attenuate;
	float lightRange;
	bool gogoMode;
	

	PxRigidDynamic* pickActor;
	PxTransform actorStartPosition;
	PxTransform actorOffset;

	PxTransform boxEndPosition[50];
	PxTransform logPosition[5];
	int logCount;
	long long logTime[5];
	int randomOffset;

	PxRigidStatic* pollActor;
	PxRigidStatic* pollActor2;
	bool showContent;


};

#endif