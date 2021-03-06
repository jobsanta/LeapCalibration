#include <Kinect.h>
#include "leapclass.h"
#include "opencv2/core/core.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

#include <fstream>
//#include <pcl/io/pcd_io.h>
//#include <pcl/point_types.h>
//#include <pcl/registration/icp.h>
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
	int fingerTipDetection(PxVec3 fingertipPosition[5]);

	float handMeasurement(PxVec3);
	void handVisualize(vector<handActor*> hand);

	void transformWorldToKinectDepthImage(Leap::Vector wrist, FLOAT * distDest);
	void estimateAffineTransform();
	void setAffineLtoK(Mat affine);
	void setAffineKtoL(Mat affine);
	Mat getAffineTransformLtoK();
	Mat getAffineTransformKtoL();

	void getPointCloudData(float*, bool mirror);
	void getColorPointCloudData(float*);

	void clear();

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

	FLOAT* depthDest;
	FLOAT* rgbDest;

	CameraSpacePoint* depthToCamera_points;
	CameraSpacePoint* colorToCamera_points;
	ColorSpacePoint* depthToColor_points;
	DepthSpacePoint* colorToDepth_points;

	ICoordinateMapper* m_pCoordinateMapper;

	Point3f previousPoints;

	HRESULT coordinateMapping(UINT16* pDepthBuffer);
	Point3f cameraToWorldSpace(CameraSpacePoint camPoint);
	CameraSpacePoint worldToCameraSpace(Point3f worldPoint);

	Point3f applyAffineTransform_KtoL(Point3f worldPoint);
	Point3f applyAffineTransform_LtoK(Point3f worldPoint);
	Point3f* smoothPoint(Point3f*);
	void drawFinger(std::vector<PxVec3> leapJointPosition, Mat depthImage);

	vector<Point3f> kinectPoints;
	vector<Point3f> leapPoints;

	HRESULT status;
	bool affine_set;

	cv::Mat aff_ltok;
	cv::Mat aff_ktol;
	cv::Mat wp;
	Point3f* precompute;

	float affktol[3][4];
	//cv::Mat icp_t;
};