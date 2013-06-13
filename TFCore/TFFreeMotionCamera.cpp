#include "TFFreeMotionCamera.h"
#include "TFInput.h"

namespace TFCore
{
	using namespace DirectX;

	/*** Ctor. Initializes camera position, camera rotation buffer, and camera movement buffer ***/
	TFFreeMotionCamera::TFFreeMotionCamera()
		:/*m_vPosition(-220.0f, 180.0f, -49.0f, 1.0f),*/
		//m_vPosition(0, 100, 0, 1.0f),
		 m_vPosition(-102.281f, 84.5717f, 94.1944f, 1.0f),
		 m_vForward(1.0f, 0.0f, 0.0f, 0.0f),
		 m_vUp(0.0f, 1.0f, 0.0f, 0.0f),
		 m_xmvUp(XMLoadFloat4(&m_vUp)),
		 m_vSide(0.0f, 0.0f, -1.0f, 0.0f),
		 CAMERA_ROTATION_BUFFER_YAW(500.0f),
		 CAMERA_MOVEMENT_BUFFER(20.0f)
	{
		// normalize forward vector
		tfVector _vForward = XMLoadFloat4(&m_vForward);
		XMVector4Normalize(_vForward);
		XMStoreFloat4(&m_vForward, _vForward);
	}

	/*** Dtor ***/
	TFFreeMotionCamera::~TFFreeMotionCamera()
	{
	}

	/*** Query input singleton to adjust the camera position/rotation ***/
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

	/*** Move camera along the side vector (Strafe) ***/
	void TFFreeMotionCamera::MoveRight(float a_fDelta)
	{
		// Move the position along the side axis
		tfVector _vPos  = XMLoadFloat4(&m_vPosition);
		tfVector _vSide = XMLoadFloat4(&m_vSide);

		_vPos += _vSide * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Move camera along the side vector (Strafe) ***/
	void TFFreeMotionCamera::MoveLeft(float a_fDelta)
	{
		// Move the position along the side axis
		tfVector _vPos  = XMLoadFloat4(&m_vPosition);
		tfVector _vSide = XMLoadFloat4(&m_vSide);

		_vPos -= _vSide * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Move camera along the forward vector ***/
	void TFFreeMotionCamera::MoveForward(float a_fDelta)
	{
		tfVector _vPos = XMLoadFloat4(&m_vPosition);
		tfVector _vFor = XMLoadFloat4(&m_vForward);

		_vPos += _vFor * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Move camera back along the forward vector ***/
	void TFFreeMotionCamera::MoveBack(float a_fDelta)
	{
		tfVector _vPos = XMLoadFloat4(&m_vPosition);
		tfVector _vFor = XMLoadFloat4(&m_vForward);

		_vPos -= _vFor * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Move camera along world up vector ***/
	void TFFreeMotionCamera::MoveUp(float a_fDelta)
	{
		tfVector _vPos = XMLoadFloat4(&m_vPosition);
		tfVector _vUp = XMLoadFloat4(&m_vUp);

		_vPos += _vUp * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Move camera down the world up vector ***/
	void TFFreeMotionCamera::MoveDown(float a_fDelta)
	{
		tfVector _vPos = XMLoadFloat4(&m_vPosition);
		tfVector _vUp = XMLoadFloat4(&m_vUp);

		_vPos -= _vUp * a_fDelta * CAMERA_MOVEMENT_BUFFER;

		XMStoreFloat4(&m_vPosition, _vPos);
	}

	/*** Build and return a view matrix that has a target in front of the camera ***/
	tfMatrix TFFreeMotionCamera::GetView()
	{
		tfVector _vPos = XMLoadFloat4(&m_vPosition);
		tfVector _vFor = XMLoadFloat4(&m_vForward);
		tfVector _vAt  = _vPos + _vFor;
		tfVector _vUp  = XMLoadFloat4(&m_vUp);

		// Normalize the vectors
		XMVector4Normalize(_vPos);
		XMVector4Normalize(_vAt);
		XMVector4Normalize(_vUp);

		return XMMatrixLookAtLH(_vPos, _vAt, _vUp);
	}

	/*** Query camera position ***/
	tfFloat3 TFFreeMotionCamera::GetPosition() const
	{
		tfFloat3 _pos;
		_pos.x = m_vPosition.x;
		_pos.y = m_vPosition.y;
		_pos.z = m_vPosition.z;

		return _pos;
	}

	/*** Return camera forward vector ***/
	tfVector TFFreeMotionCamera::GetForward() const
	{
		return XMLoadFloat4(&m_vForward);
	}

	/*** Based on horizontal mouse movement, yaw the camera by rotating the forward and side vector about
		 the world up vector ***/
	void TFFreeMotionCamera::RotateCameraYaw(float a_fDeltaTime, float a_fDeltaDistance)
	{
		tfVector _vUp   = XMLoadFloat4(&m_vUp);
		tfVector _vFor  = XMLoadFloat4(&m_vForward);
		tfVector _vSide = XMLoadFloat4(&m_vSide);

		tfMatrix _matRotation = XMMatrixRotationAxis(_vUp, a_fDeltaDistance / CAMERA_ROTATION_BUFFER_YAW);

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

	/*** Based on vertical mouse movement, pitch the camera by rotating the forward vector about the camera
		 side vector ***/
	void TFFreeMotionCamera::RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance)
	{
		tfVector _vUp   = XMLoadFloat4(&m_vUp);
		tfVector _vFor  = XMLoadFloat4(&m_vForward);
		tfVector _vSide = XMLoadFloat4(&m_vSide);

		tfMatrix _matRotation = XMMatrixRotationAxis(_vSide, a_fDeltaDistance / CAMERA_ROTATION_BUFFER_YAW);

		_vFor = XMVector4Transform(_vFor, _matRotation);

		XMVector4Normalize(_vFor);
		XMStoreFloat4(&m_vForward, _vFor);
	}

}
