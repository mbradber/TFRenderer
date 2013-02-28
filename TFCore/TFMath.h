#pragma once

#include "TFTypes.h"

#define TF_PIDIV2 XM_PIDIV2

inline tfMatrix TFGetMatrixIdentity()
{
	return XMMatrixIdentity();
}

inline tfMatrix TFMatrixPerspectiveLH(float a_fFovAngleY, float a_fAspectRatio, float a_fNearZ, float a_fFarZ)
{
	return XMMatrixPerspectiveFovLH(a_fFovAngleY, a_fAspectRatio, a_fNearZ, a_fFarZ);
}

inline tfVector TFVectorSet(float x, float y, float z, float w)
{
	return XMVectorSet(x, y, z, w);
}

inline tfMatrix TFMatrixLookAtLH(tfVector a_eye, tfVector a_at, tfVector a_up)
{
	return XMMatrixLookAtLH(a_eye, a_at, a_up);
}

inline tfMatrix TFMatrixTranspose(CXMMATRIX a_matSubject)
{
	return XMMatrixTranspose(a_matSubject);
}



