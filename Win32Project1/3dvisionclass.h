#include <Windows.h>
#include "d3dclass.h"
//#define __d3d11_h__

#include "nvapi.h"
#include "nvapi_lite_stereo.h"
#include "nvStereo.h"

class Vision3DClass
{
public:
	Vision3DClass();
	~Vision3DClass();

	bool Initialize(ID3D11Device* g_D3D11Device);
	void Render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float cameraDistance);
	void Shutdown();

private:
	StereoHandle                 g_StereoHandle;
	float                        g_EyeSeparation;
	float                        g_Separation;
	float                        g_Convergence;
	float						 g_CameraDistance;
	nv::StereoParametersD3D11	 g_StereoParamD3D11;
};