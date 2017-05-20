#include "trackerclass.h"

static const float height = 0.90f; // 70 cm
static const float degree = 57.0f;
static const float sin_deg = 0.819152044f;
static const float cos_deg = 0.573576436f;

TrackerClass::TrackerClass()
{
	depthToCamera_points = new CameraSpacePoint[cDepthWidth*cDepthHeight];
	colorToCamera_points = new CameraSpacePoint[cColorWidth*cColorHeight];
	depthToColor_points = new ColorSpacePoint[cDepthHeight*cDepthWidth];
	colorToDepth_points = new DepthSpacePoint[cColorWidth*cColorHeight];



	previousPoints = Point3f(0, 0, 0);

	aff_ltok = cv::Mat::eye(3, 4,  CV_64F);
	aff_ktol = cv::Mat::eye(3, 4, CV_64F);
	wp = Mat(4, 1, CV_64F);
	affine_set = false;

}

TrackerClass::~TrackerClass()
{

}

void TrackerClass::getPointCloudData(float* dest)
{
	Point3f point;

	float* fdest = (float*)dest;
	for (int i = 0; i < nDepthHeight*nDepthWidth; i++) {

		if (affine_set)
			point = applyAffineTransform_KtoL(cameraToWorldSpace(depthToCamera_points[i]));
		else
			point = cameraToWorldSpace(depthToCamera_points[i]);

		*fdest++ = point.x;
		*fdest++ = point.y;
		*fdest++ = -point.z;
	}


}

void TrackerClass::getColorPointCloudData(float* dest)
{
	// Write color array for vertices
	float* fdest = (float*)dest;
	for (int i = 0; i < nDepthWidth*nDepthHeight; i++) {
		ColorSpacePoint p = depthToColor_points[i];
		// Check if color pixel coordinates are in bounds
		if (p.X < 0 || p.Y < 0 || p.X > nColorWidth || p.Y > nColorHeight) {
			*fdest++ = 0;
			*fdest++ = 0;
			*fdest++ = 0;
			*fdest++ = 1;
		}
		else {
			int idx = (int)p.X + nColorWidth*(int)p.Y;
			*fdest++ = pColorBuffer[idx].rgbRed / 255.0;
			*fdest++ = pColorBuffer[idx].rgbGreen / 255.0;
			*fdest++ = pColorBuffer[idx].rgbBlue / 255.0;
			*fdest++ = 1.0;
		}
	}
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
	
	
	
	status = coordinateMapping(depthBuffer);

	return status;
}


float TrackerClass::handMeasurement(PxVec3 wristPosition)
{
	if (wristPosition == PxVec3(0,0,0))
		return -1.0f;

	if (!SUCCEEDED(status))
		return -1.0f;

	int kernelSize = 100;

	Point3f wristWorldPos = Point3f(wristPosition.x, wristPosition.y, wristPosition.z);
	CameraSpacePoint wristCamPos;
	if(affine_set)
		wristCamPos = worldToCameraSpace(applyAffineTransform_LtoK(wristWorldPos));
	else
		wristCamPos = worldToCameraSpace(wristWorldPos);

	DepthSpacePoint wristDepthPos;
	m_pCoordinateMapper->MapCameraPointToDepthSpace(wristCamPos, &wristDepthPos);
	Mat depthMap(nDepthHeight, nDepthWidth, CV_16UC1, reinterpret_cast<void*>(pDepthBuffer));
	Mat handMap(nDepthHeight, nDepthWidth, CV_16UC1,cv::Scalar(0));

	ushort wristDepth = depthMap.at<ushort>((int)(wristDepthPos.Y+0.5f), (int)(wristDepthPos.X + 0.5f));
	Point2d topPosition = Point2d(0,0);
	ushort topDepth = 0;
	if (wristDepthPos.Y - kernelSize < 0 || wristDepthPos.Y + kernelSize > nDepthHeight ||
		wristDepthPos.X - kernelSize < 0 || wristDepthPos.X + kernelSize > nDepthWidth)
		return -1.0f;


	for (int j = (int)(wristDepthPos.Y+0.5f) - kernelSize ; j < (int)(wristDepthPos.Y+0.5f) + kernelSize; j++)
	{
		ushort* p = handMap.ptr<ushort>(j);
		ushort* q = depthMap.ptr<ushort>(j);
		for (int i = (int)(wristDepthPos.X+0.5f) - kernelSize; i <  (int)(wristDepthPos.X+0.5f) + kernelSize; i++)
		{
			// convert from camera space - > world space
		

	
			if (q[i] < wristDepth+50 && q[i] != 0)
			{
				p[i] = q[i];
				if (i > topPosition.x)
				{
					topPosition.x = i;
					topPosition.y = j;
					topDepth = p[i];
				}
			}
		}
	}

	if(topPosition.x >= (int)(wristDepthPos.X+0.5)+kernelSize || abs(topDepth - wristDepth) < 25 || handMap.at<ushort>((int)(wristDepthPos.Y + 0.5), (int)(wristDepthPos.X + 0.5) + kernelSize)>0)
		return -1.0f;

	float distance = float(topPosition.x - wristDepthPos.X);
	float radian = (XM_PI * distance / 7.31427f) / 180;

	float handsize = sqrt( topDepth*topDepth + wristDepth*wristDepth - 2 * topDepth*wristDepth*cos(radian));



	Mat debugImg(nDepthHeight, nDepthWidth, CV_8UC1);
	handMap.convertTo(debugImg, CV_8UC1, 255.0 / (nMaxDistance - nMinDistance));
	circle(debugImg, Point(static_cast<int>(wristDepthPos.X), static_cast<int>(wristDepthPos.Y)), 2, Scalar(255));
	imshow("Debug", debugImg);

	return handsize-15;

}

Point3f* TrackerClass::goggleDetection()
{
	if (!SUCCEEDED(status))
		return NULL;

	Point3f* worldPoint = new Point3f;
	float factor = 4.0;
	Mat color_img(nColorHeight, nColorWidth, CV_8UC4, reinterpret_cast<void*>(pColorBuffer));
	Mat  color_resize, color_hsv;
	Mat img_WristMask, img_WristMask2, imgMask;
	resize(color_img, color_resize, Size(480, 270));
	cvtColor(color_resize, color_hsv, CV_BGR2HSV);

	inRange(color_hsv, Scalar(0, 140, 140), Scalar(10, 255, 255), img_WristMask);
	
	medianBlur(img_WristMask, imgMask, 5);


	int dilateSize = 5;
	Mat element = getStructuringElement(MORPH_RECT,
		Size(2 * dilateSize + 1, 2 * dilateSize + 1),
		Point(dilateSize, dilateSize));
	dilate(imgMask, imgMask, element);

	//imshow("WristMask", imgMask);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Point2f> result;

	Mat temp_img;
	imgMask.copyTo(temp_img);

	findContours(temp_img, contours, hierarchy, CV_RETR_CCOMP,
	CV_CHAIN_APPROX_SIMPLE);


	vector<vector<Point> > contours_poly(contours.size());
	vector<RotatedRect> boundRect(contours.size());
	// Find indices of contours whose area is less than `threshold`


	if (contours.size() != 1)
	{
		*worldPoint = Point3f(0, 0, 0);
		return worldPoint;
	}
		
	
		for (size_t i = 0; i < contours.size(); ++i) {
			Moments mu = moments(contours[i], false);
			Point2f mc = Point2f(float(factor*mu.m10 / mu.m00), float(factor*mu.m01 / mu.m00));

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


	
	Point2i rect_points = boundRect[0].center*factor;
	if (rect_points.y > 0 && rect_points.y < 1080 && rect_points.x >0 && rect_points.x < 1920)
	{
		int colorX = static_cast<int>(rect_points.x + 0.5f);
		int colorY = static_cast<int>(rect_points.y + 0.5f);
		long colorIndex = (long)(colorY*cColorWidth + colorX);
		CameraSpacePoint cam_point = colorToCamera_points[colorIndex];
		if (affine_set)
			*worldPoint = applyAffineTransform_KtoL(cameraToWorldSpace(cam_point));
		else
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

int TrackerClass::fingerTipDetection(PxVec3 fingertipPosition[5])
{
	if (!SUCCEEDED(status))
		return (int)leapPoints.size();

	Mat color_img(nColorHeight, nColorWidth, CV_8UC4, reinterpret_cast<void*>(pColorBuffer));
	Mat img_WristMask, img_WristMask2, imgMask;;
	Mat  color_resize, color_hsv;
	resize(color_img, color_resize, Size(480, 270));
	cvtColor(color_resize, color_hsv, CV_BGR2HSV);
	float factor = 4;
	inRange(color_hsv, Scalar(25, 100, 100), Scalar(35, 255, 255), img_WristMask);

	medianBlur(img_WristMask, imgMask, 3);


	//int dilateSize = 5;
	//Mat element = getStructuringElement(MORPH_RECT,
	//	Size(2 * dilateSize + 1, 2 * dilateSize + 1),
	//	Point(dilateSize, dilateSize));
	//dilate(imgMask, imgMask, element);

	//imshow("WristMask", imgMask);
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	vector<Point2f> result;

	Mat temp_img;
	imgMask.copyTo(temp_img);

	findContours(temp_img, contours, hierarchy, CV_RETR_CCOMP,
		CV_CHAIN_APPROX_SIMPLE);
	vector<vector<Point> > contours_poly(contours.size());
	vector<RotatedRect> boundRect(contours.size());
	Point3f cam_point[5];

	if (contours.size() == 5)
	{
		for (size_t i = 0; i < contours.size(); ++i) 
		{
			Moments mu = moments(contours[i], false);
			Point2f mc = Point2f(float(factor*mu.m10 / mu.m00), float(factor*mu.m01 / mu.m00));

			if (mc.x > 0 && mc.x < 1920 && mc.y > 0 && mc.y < 1080)
			{
				result.push_back(mc);

				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = minAreaRect(Mat(contours_poly[i]));
				Point2f rect_points[4]; boundRect[i].points(rect_points);
				for (int j = 0; j < 4; j++)
					line(imgMask, rect_points[j], rect_points[(j + 1) % 4], Scalar(128));


				Point2i cen_point = boundRect[i].center*factor;
				
				if (cen_point.y > 0 && cen_point.y < 1080 && cen_point.x >0 && cen_point.x < 1920)
				{
					int colorX = static_cast<int>(cen_point.x + 0.5f);
					int colorY = static_cast<int>(cen_point.y + 0.5f);
					long colorIndex = (long)(colorY*cColorWidth + colorX);
					cam_point[i] = cameraToWorldSpace( colorToCamera_points[colorIndex]);
				}
			
			}
		}


		double distance = 0;
		if (leapPoints.size() > 0)
		{
			for (int i = 0; i < 5; i++)
			{
				Point3f lpos = leapPoints[leapPoints.size() - 5 + i];
				Point3f fpos = Point3f(fingertipPosition[i].x / 100.0f, fingertipPosition[i].y / 100.0f - 1.0f, -fingertipPosition[i].z / 100.0f - 2.6f);
				Point3f d = fpos - lpos;
				distance += sqrt(d.dot(d));
			}
		}
		else
			distance = 2;

		if (distance >= 2)
		{
			for (int i = 0; i < 5; i++)
			{
				kinectPoints.push_back(cam_point[i]);
				leapPoints.push_back(Point3f(fingertipPosition[i].x / 100.0f, fingertipPosition[i].y / 100.0f - 1.0f, -fingertipPosition[i].z / 100.0f - 2.6f));
			//	Point3f(fingertipPosition[i].x/1000, fingertipPosition[i].y/1000, -fingertipPosition[i].z/1000));
			}
		}
	}



	imshow("Mask", imgMask);

	return (int)leapPoints.size();
}

void TrackerClass::estimateAffineTransform()
{
	std::vector<uchar> inliers;
	if (leapPoints.size() > 100)
	{
		int ret = cv::estimateAffine3D(leapPoints, kinectPoints, aff_ltok, inliers);
		ret = cv::estimateAffine3D(kinectPoints, leapPoints, aff_ktol, inliers);
		//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_in(new pcl::PointCloud<pcl::PointXYZ>);
		//pcl::PointCloud<pcl::PointXYZ>::Ptr cloud_out(new pcl::PointCloud<pcl::PointXYZ>);

		//// Fill in the CloudIn data
		//cloud_in->width = leapPoints.size();
		//cloud_in->height = 1;
		//cloud_in->is_dense = false;
		//cloud_in->points.resize(cloud_in->width * cloud_in->height);
		//for (size_t i = 0; i < cloud_in->points.size(); i++)
		//{
		//	cloud_in->points[i].x = leapPoints[i].x;
		//	cloud_in->points[i].y = leapPoints[i].y;
		//	cloud_in->points[i].z = leapPoints[i].z;
		//}
		//*cloud_out = *cloud_in;
		//for (size_t i = 0; i < cloud_in->points.size(); i++)
		//{
		//	cloud_out->points[i].x = kinectPoints[i].x;
		//	cloud_out->points[i].y = kinectPoints[i].y;
		//	cloud_out->points[i].z = kinectPoints[i].z;

		//}

		//pcl::IterativeClosestPoint<pcl::PointXYZ, pcl::PointXYZ> icp;
		//icp.setInputCloud(cloud_in);
		//icp.setInputTarget(cloud_out);
		//pcl::PointCloud<pcl::PointXYZ> Final;
		//icp.align(Final);

		//Matrix4f m = icp.getFinalTransformation();

		//for (int i = 0; i < 4; i++)
		//{
		//	for (int j = 0; j < 4; j++)
		//	{
		//		icp_t.at<double>(i, j) = m(i, j);
		//	}
		//}

		affine_set = true;
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				affktol[i][j] = aff_ktol.at<double>(i, j);
			}
		}
	}



	leapPoints.clear();
	kinectPoints.clear();
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


	result.z -= 0.1f;
	return result*10.0;
}

CameraSpacePoint TrackerClass::worldToCameraSpace(Point3f worldPoint)
{


	CameraSpacePoint camPoint;
	worldPoint = worldPoint/10.0f;
	worldPoint.z += 0.1f;


	camPoint.X = -cos_deg*(worldPoint.y - height) + sin_deg*worldPoint.z;
	camPoint.Y = worldPoint.x;
	camPoint.Z = -sin_deg*(worldPoint.y - height) - cos_deg*worldPoint.z;
	return camPoint;
}

Point3f TrackerClass::applyAffineTransform_KtoL(Point3f worldPoint)
{
	//wp.at<double>(0, 0) = worldPoint.x;
	//wp.at<double>(1, 0) = worldPoint.y;
	//wp.at<double>(2, 0) = worldPoint.z;
	//wp.at<double>(3, 0) = 1;
	//wp.data[0] = worldPoint.x;
	//wp.data[1] = worldPoint.y;
	//wp.data[2] = worldPoint.z;
	//wp.data[3] = 1;
	//Mat kp = aff_ktol*wp;

	Point3f result;
	result.x = affktol[0][0]*worldPoint.x+ affktol[0][1] * worldPoint.y+ affktol[0][2] * worldPoint.z+ affktol[0][3];
	result.y = affktol[1][0] * worldPoint.x + affktol[1][1] * worldPoint.y + affktol[1][2] * worldPoint.z + affktol[1][3];
	result.z = affktol[2][0] * worldPoint.x + affktol[2][1] * worldPoint.y + affktol[2][2] * worldPoint.z + affktol[2][3];


	return result;
}

Point3f TrackerClass::applyAffineTransform_LtoK(Point3f worldPoint)
{
	Mat wp = Mat(4, 1, CV_64F);
	wp.at<double>(0, 0) = worldPoint.x;
	wp.at<double>(1, 0) = worldPoint.y;
	wp.at<double>(2, 0) = worldPoint.z;
	wp.at<double>(3, 0) = 1;
	Mat kp = aff_ltok*wp;

	Point3f result;
	result.x = (float)kp.at<double>(0, 0);
	result.y = (float)kp.at<double>(1, 0);
	result.z = (float)kp.at<double>(2, 0);

	return result;
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

void TrackerClass::handVisualize(vector<handActor> hand)
{
	if (!SUCCEEDED(status))
		return;

	Mat depthMap(nDepthHeight, nDepthWidth, CV_16UC1, reinterpret_cast<void*>(pDepthBuffer));
	Mat depthImg(nDepthHeight, nDepthWidth, CV_8UC1);
	depthMap.convertTo(depthImg, CV_8UC1, 255.0 / (nMaxDistance - nMinDistance));
	
	for (int i = 0; i < hand.size(); i++)
	{
		if (hand[i].leapJointPosition != nullptr)
			drawFinger(hand[i].leapJointPosition, depthImg);
	}

	imshow("Debug Depth", depthImg);
}

void TrackerClass::drawFinger(PxVec3* leapJointPosition, Mat depthImg)
{
	CameraSpacePoint kinectJointPosition[20];
	for(int i = 0; i <5 ;i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if (!affine_set)
			{
				kinectJointPosition[i * 4 + j] = worldToCameraSpace(
					Point3f(leapJointPosition[i * 4 + j].x / 100.0f, leapJointPosition[i * 4 + j].y / 100.0f - 1.0f, -leapJointPosition[i * 4 + j].z / 100.0f - 2.6f));
			}
			else
			{
				Point3f cam = applyAffineTransform_LtoK(
					Point3f(leapJointPosition[i * 4 + j].x / 100.0f, leapJointPosition[i * 4 + j].y / 100.0f - 1.0f, -leapJointPosition[i * 4 + j].z / 100.0f - 2.6f));
				kinectJointPosition[i * 4 + j] = worldToCameraSpace(cam);
			}

		}
	}

	DepthSpacePoint depthJointPosition[20];

	m_pCoordinateMapper->MapCameraPointsToDepthSpace(20, kinectJointPosition, 20, depthJointPosition);


	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			if(depthJointPosition[i * 4 + j].X >= 0 && depthJointPosition[i * 4 + j].X < nDepthWidth &&
				depthJointPosition[i * 4 + j].Y >= 0 && depthJointPosition[i *4 + j].Y < nDepthHeight)
			circle(depthImg, Point((int)depthJointPosition[i*4+j].X, (int)depthJointPosition[i * 4 + j].Y), 2, Scalar(128+j*32));
		}
	}
}

void TrackerClass::setAffineLtoK(Mat transform)
{
	aff_ltok = transform;
	affine_set = true;
}

void TrackerClass::setAffineKtoL(Mat transform)
{
	aff_ktol = transform;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			affktol[i][j] = aff_ktol.at<double>(i, j);
		}
	}


	affine_set = true;
}

Mat TrackerClass::getAffineTransformLtoK()
{
	return aff_ltok;
}

Mat TrackerClass::getAffineTransformKtoL()
{
	return aff_ktol;
}
//Mat TrackerClass::getICPTransform()
//{
//	return icp_t;
//}