#pragma once

#include <d3d11.h>
#include <xnamath.h>

namespace TFCore
{
	class TFFreeMotionCamera
	{
	public:
		TFFreeMotionCamera();
		~TFFreeMotionCamera();

		void Update(float a_fDelta);
		void MoveRight(float a_fDelta);
		void MoveLeft(float a_fDelta);
		void MoveForward(float a_fDelta);
		void MoveBack(float a_fDelta);
		void MoveUp(float a_fDelta);
		void MoveDown(float a_fDelta);
		void RotateCameraYaw(float a_fDeltaTime, float a_fDeltaDistance);
		void RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance);
		XMMATRIX GetView();
		XMFLOAT3 GetPosition() const;
		XMVECTOR GetForward() const;
		inline XMVECTOR& GetUpVector() { return m_xmvUp; }

	private:
		XMFLOAT4 m_vPosition;
		XMFLOAT4 m_vForward;
		XMFLOAT4 m_vUp;
		XMVECTOR m_xmvUp;
		XMFLOAT4 m_vSide;

		const float CAMERA_ROTATION_BUFFER_YAW;
		const float CAMERA_MOVEMENT_BUFFER;
	};
}

