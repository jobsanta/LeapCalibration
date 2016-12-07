#include <Kinect.h>


class KinectClass
{
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;



public:
	KinectClass();
	~KinectClass();


	IKinectSensor* m_pKinectSensor;
	ICoordinateMapper* m_pCoordinateMapper;
	
	// to prevent drawing until we have data for both streams
	bool m_bDepthReceived;
	bool m_bColorReceived;
	bool m_bNearMode;
	bool m_bPaused;
	bool firstRun;

	IMultiSourceFrameReader* m_pMultiSourceFrameReader;

	// For   mapping depth to color
	RGBQUAD* m_pColorRGBX;
	RGBQUAD* m_pOutputRGBX;
	RGBQUAD* m_pBackgroundRGBX;

	HRESULT Initialize();
	HRESULT Process();
	void	Shutdown();

private:
	HRESULT CreateFirstConnected();

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