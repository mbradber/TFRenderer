#include "TFFreeMotionCamera.h"

namespace TFCore
{

	TFFreeMotionCamera::TFFreeMotionCamera()
		:m_vEye(0.0f, 1.0f, -5.0f, 0.0f),
		 m_vAt(0.0f, 1.0f, 0.0f, 0.0f),
		 m_vUp(0.0f, 1.0f, 0.0f, 0.0f),
		 CAMERA_ROTATION_BUFFER_YAW(500.0f)
	{
	}


	TFFreeMotionCamera::~TFFreeMotionCamera()
	{
	}

	void TFFreeMotionCamera::MoveRight(float a_fDelta)
	{
		m_vAt.x += 0.001f;
		m_vEye.x += 0.001f;
	}

	void TFFreeMotionCamera::MoveLeft(float a_fDelta)
	{
		m_vAt.x -= 0.001f;
		m_vEye.x -= 0.001f;
	}

	XMMATRIX TFFreeMotionCamera::GetView()
	{
		return XMMatrixLookAtLH(XMLoadFloat4(&m_vEye), XMLoadFloat4(&m_vAt), XMLoadFloat4(&m_vUp));
	}

	void TFFreeMotionCamera::RotateCameraYaw(float a_fDeltaTime, float a_fDeltaDistance)
	{
		XMVECTOR _up = XMLoadFloat4(&m_vUp);
		XMVECTOR _eye = XMLoadFloat4(&m_vEye);

		XMMATRIX _mat = XMMatrixRotationAxis(_up, a_fDeltaDistance / CAMERA_ROTATION_BUFFER_YAW);
		_eye = XMVector4Transform(_eye, _mat);

		XMStoreFloat4(&m_vEye, _eye);
	}

	void RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance)
	{

	}

}
