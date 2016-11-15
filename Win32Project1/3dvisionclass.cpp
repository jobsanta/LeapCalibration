#include "3dvisionclass.h"

Vision3DClass::Vision3DClass()
{

}

Vision3DClass::~Vision3DClass()
{

}



bool Vision3DClass::Initialize(ID3D11Device* g_D3D11Device)
{
	//Prepare NVAPI for use in this application
	NvAPI_Status status;
	status = NvAPI_Initialize();
	if (status != NVAPI_OK)
	{
		NvAPI_ShortString errorMessage;
		NvAPI_GetErrorMessage(status, errorMessage);
		MessageBoxA(NULL, errorMessage, "Unable to initialize NVAPI", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		return false;
	}
	else
	{
		// Check the Stereo availability
		NvU8 isStereoEnabled;
		status = NvAPI_Stereo_IsEnabled(&isStereoEnabled);

		// Stereo status report an error
		if (status != NVAPI_OK)
		{
			// GeForce Stereoscopic 3D driver is not installed on the system
			MessageBoxA(NULL, "Stereo is not available\nMake sure the stereo driver is installed correctly", "Stereo not available", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			return false;
		}
		// Stereo is available but not enabled, let's enable it
		else if (NVAPI_OK == status && !isStereoEnabled)
		{
			MessageBoxA(NULL, "Stereo is available but not enabled\nLet's enable it", "Stereo not enabled", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			status = NvAPI_Stereo_Enable();
		}

		NvAPI_Stereo_CreateConfigurationProfileRegistryKey(NVAPI_STEREO_DEFAULT_REGISTRY_PROFILE);
	}

	status = NvAPI_Stereo_CreateHandleFromIUnknown(g_D3D11Device, &g_StereoHandle);
	if (NVAPI_OK != status)
	{
		MessageBoxA(NULL, "Couldn't create the StereoHandle", "NvAPI_Stereo_CreateHandleFromIUnknown failed", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		return false;
	}

	if (NVAPI_OK != NvAPI_Stereo_GetEyeSeparation(g_StereoHandle, &g_EyeSeparation))
	{
		MessageBoxA(NULL, "Couldn't get the hardware eye separation", "NvAPI_Stereo_GetEyeSeparation failed", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
		return false;
	}

	return true;
}

void Vision3DClass::Render(ID3D11Device* pd3dDevice, ID3D11DeviceContext* pd3dDeviceContext, float cameraDistance)
{
	
			float sep, conv;
			if (NVAPI_OK != NvAPI_Stereo_SetConvergence(g_StereoHandle, cameraDistance))
			{
				MessageBoxA(NULL, "Couldn't set the convergence", "NvAPI_Stereo_SetConvergence failed", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			}
			if (NVAPI_OK != NvAPI_Stereo_GetSeparation(g_StereoHandle, &sep))
			{
				MessageBoxA(NULL, "Couldn't get the separation", "NvAPI_Stereo_GetSeparation failed", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
			}
			if (NVAPI_OK != NvAPI_Stereo_GetConvergence(g_StereoHandle, &conv))
			{
				MessageBoxA(NULL, "Couldn't get the convergence", "NvAPI_Stereo_GetConvergence failed", MB_OK | MB_SETFOREGROUND | MB_TOPMOST);
	
			}
			if (sep * 0.01 != g_Separation || conv != g_Convergence)
			{
				g_Separation = sep * 0.01;
				g_Convergence = conv;
				//g_pStereoSeparation->SetFloat(g_Separation);
				//g_pStereoConvergence->SetFloat(g_Convergence);
	
				g_StereoParamD3D11.updateStereoParamsMap(pd3dDevice, pd3dDeviceContext, g_EyeSeparation, g_Separation, g_Convergence);
			}
}

void Vision3DClass::Shutdown()
{
	g_StereoParamD3D11.destroyGraphics();
}