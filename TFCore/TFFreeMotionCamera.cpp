#include "TFFreeMotionCamera.h"
#include "TFInput.h"

namespace TFCore
{

	TFFreeMotionCamera::TFFreeMotionCamera()
		:m_vPosition(0.0f, 100.0f, 0.0f, 1.0f),
		 m_vForward(0.0f, -0.4f, 1.0f, 0.0f),
		 m_vUp(0.0f, 1.0f, 0.0f, 0.0f),
		 m_vSide(1.0f, 0.0f, 0.0f, 0.0f),
		 CAMERA_ROTATION_BUFFER_YAW(500.0f),
		 CAMERA_MOVEMENT_BUFFER(15.0f)
	{
		// normalize forward vector
		XMVECTOR _vForward = XMLoadFloat4(&m_vForward);
		XMVector4Normalize(_vForward);
		XMStoreFloat4(&m_vForward, _vForward);
	}


	TFFreeMotionCamera::~TFFreeMotionCamera()
	{
	}

	void TFFreeMotionCamera::Update(float a_fDelta)
	{
		// Camera updates from keyboard (directional)
		if(TFInput::Instance()->IsRightPressed())
		{
			MoveRight(a_fDelta);
		}
		if(TFInput::Instance()->IsLeftPressed())   
		{
			MoveLeft(a_fDelta);
		}
		if(TFInput::Instance()->IsForwardPressed())
		{
			MoveForward(a_fDelta);
		}
		if(TFInput::Instance()->IsBackPressed())
		{
			MoveBack(a_fDelta);
		}
		if(TFInput::Instance()->IsUpPressed())
		{
			MoveUp(a_fDelta);
		}
		if(TFInput::Instance()->IsDownPressed())
		{
			MoveDown(a_fDelta);
		}

		// Camera updates from mouse (rotations)
		RotateCameraYaw(a_fDelta, static_cast<float>(TFInput::Instance()->GetMouseDeltaX()));
		RotateCameraPitch(a_fDelta, static_cast<float>(TFInput::Instance()->GetMouseDeltaY()));
	}

	void TFFreeMotionCamera::MoveRight(float a_fDelta)
	{
		// Move the position along the side axis
		XMVECTOR _vPos  = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		_vPos += _vSide * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveLeft(float a_fDelta)
	{
		// Move the position along the side axis
		XMVECTOR _vPos  = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		_vPos -= _vSide * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveForward(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vFor = XMLoadFloat4(&m_vForward);

		_vPos += _vFor * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveBack(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vFor = XMLoadFloat4(&m_vForward);

		_vPos -= _vFor * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveUp(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vUp = XMLoadFloat4(&m_vUp);

		_vPos += _vUp * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	void TFFreeMotionCamera::MoveDown(float a_fDelta)
	{
		XMVECTOR _vPos = XMLoadFloat4(&m_vPosition);
		XMVECTOR _vUp = XMLoadFloat4(&m_vUp);

		_vPos -= _vUp * a_fDelta * CAMERA_MOVEMENT_BUFFER;

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

	XMFLOAT3 TFFreeMotionCamera::GetPosition() const
	{
		XMFLOAT3 _pos;
		_pos.x = m_vPosition.x;
		_pos.y = m_vPosition.y;
		_pos.z = m_vPosition.z;

		return _pos;
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

	void TFFreeMotionCamera::RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance)
	{
		XMVECTOR _vUp   = XMLoadFloat4(&m_vUp);
		XMVECTOR _vFor  = XMLoadFloat4(&m_vForward);
		XMVECTOR _vSide = XMLoadFloat4(&m_vSide);

		XMMATRIX _matRotation = XMMatrixRotationAxis(_vSide, a_fDeltaDistance / CAMERA_ROTATION_BUFFER_YAW);

		_vFor = XMVector4Transform(_vFor, _matRotation);

		XMVector4Normalize(_vFor);
		XMStoreFloat4(&m_vForward, _vFor);
	}

}
