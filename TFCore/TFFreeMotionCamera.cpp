#include "TFFreeMotionCamera.h"

namespace TFCore
{

	TFFreeMotionCamera::TFFreeMotionCamera()
		:m_vPosition(0.0f, 0.0f, -5.0f, 1.0f),
		 m_vForward(0.0f, 0.0f, 1.0f, 0.0f),
		 m_vUp(0.0f, 1.0f, 0.0f, 0.0f),
		 m_vSide(1.0f, 0.0f, 0.0f, 0.0f),
		 CAMERA_ROTATION_BUFFER_YAW(500.0f),
		 CAMERA_MOVEMENT_BUFFER(0.003f)
	{
	}


	TFFreeMotionCamera::~TFFreeMotionCamera()
	{
	}

	void TFFreeMotionCamera::MoveRight(float a_fDelta)
	{
		// Move the position along the side axis
		XMVECTOR _vPos  = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		_vPos += _vSide * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveLeft(float a_fDelta)
	{
		// Move the position along the side axis
		XMVECTOR _vPos  = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		_vPos -= _vSide * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveForward(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vFor = XMLoadFloat4(&m_vForward);

		_vPos += _vFor * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveBack(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vFor = XMLoadFloat4(&m_vForward);

		_vPos -= _vFor * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	XMMATRIX TFFreeMotionCamera::GetView()
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vFor = XMLoadFloat4(&m_vForward);
		XMVECTOR _vAt  = _vPos + _vFor;
		XMVECTOR _vUp  = XMLoadFloat4(&m_vUp);

		// Normalize the vectors
		XMVector4Normalize(_vPos);
		XMVector4Normalize(_vAt);
		XMVector4Normalize(_vUp);

		return XMMatrixLookAtLH(_vPos, _vAt, _vUp);
	}

	void TFFreeMotionCamera::RotateCameraYaw(float a_fDeltaTime, float a_fDeltaDistance)
	{
		XMVECTOR _vUp   = XMLoadFloat4(&m_vUp);
		XMVECTOR _vFor  = XMLoadFloat4(&m_vForward);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		XMMATRIX _matRotation = XMMatrixRotationAxis(_vUp, a_fDeltaDistance / CAMERA_ROTATION_BUFFER_YAW);

		// In a yaw rotation, update the forward and side vectors of the camera
		_vFor  = XMVector4Transform(_vFor, _matRotation);
		_vSide = XMVector4Transform(_vSide, _matRotation);

		// Normalize the vectors
		XMVector4Normalize(_vFor);
		XMVector4Normalize(_vSide);

		// Update member vectors that were adjusted
		XMStoreFloat4(&m_vForward, _vFor);
		XMStoreFloat4(&m_vSide, _vSide);
	}

	void RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance)
	{

	}

}
