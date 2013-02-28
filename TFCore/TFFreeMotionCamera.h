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

		void MoveRight(float a_fDelta);
		void MoveLeft(float a_fDelta);
		void RotateCameraYaw(float a_fDeltaTime, float a_fDeltaDistance);
		void RotateCameraPitch(float a_fDeltaTime, float a_fDeltaDistance);
		XMMATRIX GetView();

	private:
		XMFLOAT4 m_vEye;
		XMFLOAT4 m_vAt;
		XMFLOAT4 m_vUp;

		const float CAMERA_ROTATION_BUFFER_YAW;
	};
}

