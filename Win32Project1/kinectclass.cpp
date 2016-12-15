#include "kinectclass.h"

KinectClass::KinectClass()
{
	m_colorToDepthDivisor = cColorWidth / cDepthWidth;

	m_pCoordinateMapper = NULL;
	m_pKinectSensor = NULL;
	m_pMultiSourceFrameReader = NULL;

	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	m_pDepthBuffer = new UINT16[cDepthWidth*cDepthHeight];

	m_pDepthCoordinates = new DepthSpacePoint[cColorWidth * cColorHeight];

}

KinectClass::~KinectClass()
{

}

HRESULT KinectClass::Initialize()
{
	HRESULT hr;
	hr = CreateFirstConnected();


	return hr;
}

HRESULT KinectClass::CreateFirstConnected()
{
	HRESULT hr;

	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED(hr))
	{
		return hr;
	}

	if (m_pKinectSensor)
	{
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->Open();
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth |
				FrameSourceTypes::FrameSourceTypes_Color,
				&m_pMultiSourceFrameReader);
		}
	}

	return hr;
}

HRESULT KinectClass::Process()
{
	if (!m_pMultiSourceFrameReader)
	{
		return E_FAIL;
	}
	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IDepthFrame*       pDepthFrame = NULL;
	IColorFrame*       pColorFrame = NULL;


	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
	// Get Depth
	if (SUCCEEDED(hr))
	{
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}

		SafeRelease(pDepthFrameReference);
	}

	// Get Color
	if (SUCCEEDED(hr))
	{
		IColorFrameReference* pColorFrameReference = NULL;

		hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}

		SafeRelease(pColorFrameReference);
	}

	// If all succeeded combine frame
	// If all succeeded combine frame
	if (SUCCEEDED(hr))
	{
		INT64                 nDepthTime = 0;
		IFrameDescription*    pDepthFrameDescription = NULL;
		//int                   nDepthWidth = 0;
		//int                   nDepthHeight = 0;
		//USHORT	              nMaxDistance = 0;
		//USHORT                nMinDistance = 0;
		UINT                  nDepthBufferSize = 0;
		UINT16                *pDepthBuffer = NULL;

		IFrameDescription* pColorFrameDescription = NULL;
		//int                nColorWidth = 0;
		//int                nColorHeight = 0;
		ColorImageFormat   imageFormat = ColorImageFormat_None;
		UINT               nColorBufferSize = 0;
		RGBQUAD            *pColorBuffer = NULL;



		pDepthFrame->get_RelativeTime(&nDepthTime);
		pDepthFrame->get_DepthMaxReliableDistance(&m_pMaxDistance);
		pDepthFrame->get_DepthMinReliableDistance(&m_pMinDistance);
		hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);

		if (SUCCEEDED(hr))
		{
			pDepthFrameDescription->get_Width(&m_pDepthWidth);
			pDepthFrameDescription->get_Height(&m_pDepthHeight);
			nDepthBufferSize = cDepthWidth*cDepthHeight;
			hr = pDepthFrame->CopyFrameDataToArray(nDepthBufferSize, m_pDepthBuffer);
		}

		// get color frame data
		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}
		if (SUCCEEDED(hr))
		{
			pColorFrameDescription->get_Width(&m_pColorWidth);
			pColorFrameDescription->get_Height(&m_pColorHeight);
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}


		//Copy color data to the heap
		if (SUCCEEDED(hr))
		{
			//if (imageFormat == ColorImageFormat_Bgra)
			//{
			//	hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
			//}
			if (m_pColorRGBX)
			{
				pColorBuffer = m_pColorRGBX;
				nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(m_pColorRGBX), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}


	/*	if (SUCCEEDED(hr))
		{
			 ProcessFrame(nDepthTime, pDepthBuffer, nDepthWidth, nDepthHeight,
				pColorBuffer, nColorWidth, nColorHeight,
				nMinDistance, nMaxDistance);
		}*/

		SafeRelease(pDepthFrameDescription);
		SafeRelease(pColorFrameDescription);
	}

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pMultiSourceFrame);

	return hr;
}

int KinectClass::getDepthWidth()
{
	return m_pDepthWidth;
}
int KinectClass::getDepthHeight()
{
	return m_pDepthHeight;
}
int KinectClass::getColorWidth()
{
	return m_pColorWidth;
}
int KinectClass::getColorHeight()
{
	return m_pColorHeight;
}
UINT16* KinectClass::getDepthBuffer()
{
	return m_pDepthBuffer;
}
RGBQUAD* KinectClass::getColorBuffer()
{
	return m_pColorRGBX;
}

USHORT KinectClass::getMinDistance()
{
	return m_pMinDistance;
}
USHORT KinectClass::getMaxDistance()
{
	return m_pMaxDistance;
}

ICoordinateMapper* KinectClass::getCoordinateMapper()
{
	return m_pCoordinateMapper;
}


void KinectClass::Shutdown()
{
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}



	SafeRelease(m_pKinectSensor);

	SafeRelease(m_pMultiSourceFrameReader);


}


void KinectClass::ProcessFrame(INT64 nTime,
	UINT16* pDepthBuffer, int nDepthWidth, int nDepthHeight,
	RGBQUAD* pColorBuffer, int nColorWidth, int nColorHeight,
	USHORT nMinDistance, USHORT nMaxDistance)
{
	if (m_pCoordinateMapper && m_pDepthCoordinates  &&
		pDepthBuffer && (nDepthWidth == cDepthWidth) && (nDepthHeight == cDepthHeight) &&
		pColorBuffer && (nColorWidth == cColorWidth) && (nColorHeight == cColorHeight))
	{
		//HRESULT hr = m_Tracker->Process(nDepthWidth, nDepthHeight, nColorWidth, nColorHeight, nMinDistance, nMaxDistance, pDepthBuffer, pColorBuffer, m_pCoordinateMapper);
		//
		//if(SUCCEEDED(hr))
		//m_Tracker->goggleDetection();
	}
}