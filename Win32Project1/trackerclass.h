#include <Kinect.h>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

using namespace std;
using namespace cv;

class TrackerClass
{
	static const int        cDepthWidth = 512;
	static const int        cDepthHeight = 424;
	static const int        cColorWidth = 1920;
	static const int        cColorHeight = 1080;

public:
	TrackerClass();
	~TrackerClass();

	HRESULT Process(int depthWidth, int depthHeight, int colorWidth, int colorHeight,
		USHORT nMinDistance, USHORT nMaxDistance, UINT16* depthBuffer, RGBQUAD* colorBuffer, ICoordinateMapper* coodinateMapper);
	Point3f* goggleDetection();
private:
	bool dataRecived;


	int nDepthWidth;
	int nDepthHeight;
	int nColorWidth;
	int nColorHeight;
	UINT16* pDepthBuffer;
	RGBQUAD* pColorBuffer;

	USHORT nMinDistance;
	USHORT nMaxDistance;


	CameraSpacePoint* depthToCamera_points;
	CameraSpacePoint* colorToCamera_points;
	ColorSpacePoint* depthToColor_points;
	DepthSpacePoint* colorToDepth_points;

	ICoordinateMapper* m_pCoordinateMapper;
	Mat img_WristMask, img_WristMask2, imgMask;;
	Point3f previousPoints;

	HRESULT coordinateMapping(UINT16* pDepthBuffer);
	Point3f cameraToWorldSpace(CameraSpacePoint camPoint);
	Point3f* smoothPoint(Point3f*);

};