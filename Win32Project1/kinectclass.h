#include <Kinect.h>
//#include "trackerclass.h"

#ifndef _KINECTCLASS_H_
#define _KINECTCLASS_H_




class KinectClass
{
public:

	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;
	

	KinectClass();
	~KinectClass();

	int getDepthWidth();
	int getDepthHeight();
	int getColorWidth();
	int getColorHeight();
	UINT16* getDepthBuffer();
	RGBQUAD* getColorBuffer();

	USHORT getMinDistance();
	USHORT getMaxDistance();

	ICoordinateMapper* getCoordinateMapper();

	HRESULT Initialize();
	HRESULT Process();
	void	Shutdown();

private:

	IKinectSensor* m_pKinectSensor;
	ICoordinateMapper* m_pCoordinateMapper;
	DepthSpacePoint*   m_pDepthCoordinates;

	IMultiSourceFrameReader* m_pMultiSourceFrameReader;

	// For   mapping depth to color
	RGBQUAD* m_pColorRGBX;
	UINT16* m_pDepthBuffer;



	// to prevent drawing until we have data for both streams
	bool m_bDepthReceived;
	bool m_bColorReceived;
	bool m_bNearMode;
	bool m_bPaused;
	bool firstRun;

	int                m_pDepthWidth = 0;
	int                m_pDepthHeight = 0;
	USHORT	           m_pMaxDistance = 0;
	USHORT             m_pMinDistance = 0;
	int                m_pColorWidth = 0;
	int                m_pColorHeight = 0;






	HRESULT CreateFirstConnected();
	LONG m_colorToDepthDivisor;
	void ProcessFrame(INT64 nTime,
		UINT16* pDepthBuffer, int nDepthWidth, int nDepthHeight,
		RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight,
		USHORT nMinDistance, USHORT nMaxDistance);
	

};

// Safe release for interfaces
template<class Interface>
inline void SafeRelease(Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease->Release();
		pInterfaceToRelease = NULL;
	}
}

#endif