#include "trackerclass.h"

static const float height = 0.7f; // 70 cm
static const float degree = 60.0;
static const float cos_deg = 0.68199836006;
static const float sin_deg = 0.73135370161;

TrackerClass::TrackerClass()
{
	depthToCamera_points = new CameraSpacePoint[cDepthWidth*cDepthHeight];
	colorToCamera_points = new CameraSpacePoint[cColorWidth*cColorHeight];
	depthToColor_points = new ColorSpacePoint[cDepthHeight*cDepthWidth];
	colorToDepth_points = new DepthSpacePoint[cColorWidth*cColorHeight];

	previousPoints = Point3f(0, 0, 0);

}

TrackerClass::~TrackerClass()
{

}

HRESULT TrackerClass::Process(int depthWidth, int depthHeight, int colorWidth, int colorHeight,
	USHORT minDistance, USHORT maxDistance, UINT16* depthBuffer, RGBQUAD* colorBuffer, ICoordinateMapper* coodinateMapper)
{

	nDepthWidth = depthWidth;
	nDepthHeight = depthHeight;
	nColorWidth = colorWidth;
	nColorHeight = colorHeight;

	nMinDistance = minDistance;
	nMaxDistance = maxDistance;

	pDepthBuffer = depthBuffer;
	pColorBuffer = colorBuffer;
	m_pCoordinateMapper = coodinateMapper;
	HRESULT hr = coordinateMapping(depthBuffer);

	return hr;
}


Point3f* TrackerClass::goggleDetection()
{
	Point3f* worldPoint = new Point3f;
	float factor = 4.0;
	Mat color_img(nColorHeight, nColorWidth, CV_8UC4, reinterpret_cast<void*>(pColorBuffer));
	Mat  color_resize, color_hsv;
	resize(color_img, color_resize, Size(480, 270));
	cvtColor(color_resize, color_hsv, CV_BGR2HSV);

	inRange(color_hsv, Scalar(0, 140, 140), Scalar(10, 255, 255), img_WristMask);
	
	medianBlur(img_WristMask, imgMask, 5);


	int dilateSize = 5;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * dilateSize + 1, 2 * dilateSize + 1),
		Point(dilateSize, dilateSize));
	dilate(imgMask, imgMask, element);

	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Point2f> result;


	float contour_area;
	Mat temp_img;
	imgMask.copyTo(temp_img);

	findContours(temp_img, contours, hierarchy, CV_RETR_CCOMP,
	CV_CHAIN_APPROX_SIMPLE);


	vector<vector<Point> > contours_poly(contours.size());
	vector<RotatedRect> boundRect(contours.size());
	// Find indices of contours whose area is less than `threshold`


	if (contours.size() != 1)
		return NULL;
	
		for (size_t i = 0; i < contours.size(); ++i) {
			Moments mu = moments(contours[i], false);
			Point2f mc = Point2f(factor*mu.m10 / mu.m00, factor*mu.m01 / mu.m00);

			if (mc.x > 0 && mc.x < 1980 && mc.y > 0 && mc.y < 1080)
			{
				result.push_back(mc);

				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = minAreaRect(Mat(contours_poly[i]));
				Point2f rect_points[4]; boundRect[i].points(rect_points);
				for (int j = 0; j < 4; j++)
					line(imgMask, rect_points[j], rect_points[(j + 1) % 4], Scalar(128));
			}
		}


	//imshow("WristMask", imgMask);
	Point2i rect_points = boundRect[0].center*factor;
	if (rect_points.y > 0 && rect_points.y < 1080 && rect_points.x >0 && rect_points.x < 1920)
	{
		int colorX = static_cast<int>(rect_points.x + 0.5f);
		int colorY = static_cast<int>(rect_points.y + 0.5f);
		long colorIndex = (long)(colorY*cColorWidth + colorX);
		CameraSpacePoint cam_point = colorToCamera_points[colorIndex];
		*worldPoint = cameraToWorldSpace(cam_point);


		//float mapColorX = colorToDepth_points[rect_points.y*nColorWidth + rect_points.x].X;
		//float mapDepthY = colorToDepth_points[rect_points.y*nColorWidth + rect_points.x].Y;
		//int depthX = (int)mapDepthX + 0.5f;
		//int depthY = (int)mapDepthY + 0.5f;
		//if (depthX > 0 && depthX < nDepthWidth && depthY > 0 && depthY < nDepthHeight)
		//{

		//	*worldPoint = cameraToWorldSpace(depthToCamera_points[(int)(depthY *nDepthWidth + depthX)]);
		if (worldPoint->x > -3.0 && worldPoint->x < 3 && worldPoint->y > 0.0 && worldPoint->y  < 5 && worldPoint->z > -15 && worldPoint->z < -2)
		{
			return smoothPoint(worldPoint);
		}
		else
				return NULL;
		//}
		//else
		//	return NULL;
		
		//if (worldPoint->x > -3.5 &&worldPoint->x < 3.5   &&worldPoint->y > 0 && worldPoint->y <4.0&& worldPoint->z >-10 && worldPoint->z < -4)
		//	return worldPoint;
		//else
		//	return NULL;
	}
	else
	{
		return NULL;
	}
		




}

HRESULT TrackerClass::coordinateMapping(UINT16* pDepthBuffer)
{
	if (depthToCamera_points == NULL)
		depthToCamera_points = new CameraSpacePoint[nDepthHeight*nDepthWidth];

	if (depthToColor_points == NULL)
		depthToColor_points = new ColorSpacePoint[nDepthHeight*nDepthWidth];

	if (colorToDepth_points == NULL)
		colorToDepth_points = new DepthSpacePoint[nColorHeight* nColorWidth];

	if (colorToCamera_points == NULL)
		colorToCamera_points = new CameraSpacePoint[nColorHeight* nColorWidth];

	HRESULT hr;


	hr = m_pCoordinateMapper->MapDepthFrameToCameraSpace(nDepthHeight*nDepthWidth, pDepthBuffer, nDepthHeight*nDepthWidth, depthToCamera_points);
	if(SUCCEEDED(hr))
		hr = m_pCoordinateMapper->MapDepthFrameToColorSpace(nDepthWidth*nDepthHeight, pDepthBuffer, nDepthHeight*nDepthWidth, depthToColor_points);
	if (SUCCEEDED(hr))
	hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(nDepthHeight*nDepthWidth, pDepthBuffer, nColorHeight* nColorWidth, colorToDepth_points);
	if (SUCCEEDED(hr))
	hr = m_pCoordinateMapper->MapColorFrameToCameraSpace(nDepthHeight*nDepthWidth, pDepthBuffer, nColorHeight*nColorWidth, colorToCamera_points);

	return hr;
}

Point3f TrackerClass::cameraToWorldSpace(CameraSpacePoint camPoint)
{
	Point3f result;
	// convert from camera space - > world space
	result.x = camPoint.Y;
	result.y = -cos_deg*camPoint.X - sin_deg*camPoint.Z + height;
	result.z = sin_deg*camPoint.X - cos_deg*camPoint.Z;

	return result*10.0;
}


Point3f* TrackerClass::smoothPoint(Point3f* currentPoint)
{
	if (previousPoints == Point3f(0, 0, 0))
	{
		previousPoints = *currentPoint;
		return currentPoint;
	}
	
	Point3f diff = *currentPoint - previousPoints;
	*currentPoint = previousPoints + diff*0.1f;
	previousPoints = *currentPoint;
	return currentPoint;


}