#pragma once

#include <d3d11.h>
#include "TFTypes.h"

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
		tfMatrix GetView();
		tfFloat3 GetPosition() const;
		tfVector GetForward() const;
		inline tfVector& GetUpVector() { return m_xmvUp; }

	private:
		tfFloat4 m_vPosition;
		tfFloat4 m_vForward;
		tfFloat4 m_vUp;
		tfVector m_xmvUp;
		tfFloat4 m_vSide;

		const float CAMERA_ROTATION_BUFFER_YAW;
		const float CAMERA_MOVEMENT_BUFFER;
	};
}

