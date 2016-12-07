#include "kinectclass.h"

KinectClass::KinectClass()
{
	m_pCoordinateMapper = NULL;
	m_pKinectSensor = NULL;
	m_pMultiSourceFrameReader = NULL;
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
	if (SUCCEEDED(hr))
	{
		INT64                 nDepthTime = 0;
		IFrameDescription*    pDepthFrameDescription = NULL;
		int                   nDepthWidth = 0;
		int                   nDepthHeight = 0;
		USHORT	              nMaxDistance = 0;
		USHORT                nMinDistance = 0;
		UINT                  nDepthBufferSize = 0;
		UINT16                *pDepthBuffer = NULL;

		IFrameDescription* pColorFrameDescription = NULL;
		int                nColorWidth = 0;
		int                nColorHeight = 0;
		ColorImageFormat   imageFormat = ColorImageFormat_None;
		UINT               nColorBufferSize = 0;
		RGBQUAD            *pColorBuffer = NULL;



		pDepthFrame->get_RelativeTime(&nDepthTime);
		pDepthFrame->get_DepthMaxReliableDistance(&nMaxDistance);
		pDepthFrame->get_DepthMinReliableDistance(&nMinDistance);
		hr = pDepthFrame->get_FrameDescription(&pDepthFrameDescription);

		if (SUCCEEDED(hr))
		{
			pDepthFrameDescription->get_Width(&nDepthWidth);
			pDepthFrameDescription->get_Height(&nDepthHeight);
			hr = pDepthFrame->AccessUnderlyingBuffer(&nDepthBufferSize, &pDepthBuffer);
		}

		// get color frame data
		if (SUCCEEDED(hr))
		{
			hr = pColorFrame->get_FrameDescription(&pColorFrameDescription);
		}
		if (SUCCEEDED(hr))
		{
			pColorFrameDescription->get_Width(&nColorWidth);
			pColorFrameDescription->get_Height(&nColorHeight);
			hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
		}


		//Copy color data to the heap
		if (SUCCEEDED(hr))
		{
			if (imageFormat == ColorImageFormat_Bgra)
			{
				hr = pColorFrame->AccessRawUnderlyingBuffer(&nColorBufferSize, reinterpret_cast<BYTE**>(&pColorBuffer));
			}
			else if (m_pColorRGBX)
			{
				pColorBuffer = m_pColorRGBX;
				nColorBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
				hr = pColorFrame->CopyConvertedFrameDataToArray(nColorBufferSize, reinterpret_cast<BYTE*>(pColorBuffer), ColorImageFormat_Bgra);
			}
			else
			{
				hr = E_FAIL;
			}
		}


		if (SUCCEEDED(hr))
		{
			//Process frame here
		}

		SafeRelease(pDepthFrameDescription);
		SafeRelease(pColorFrameDescription);
	}

	SafeRelease(pDepthFrame);
	SafeRelease(pColorFrame);
	SafeRelease(pMultiSourceFrame);
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
