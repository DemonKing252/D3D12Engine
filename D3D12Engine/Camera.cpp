#include "Camera.h"

void Camera::SetLens(XMFLOAT3 CameraPos, XMFLOAT3 Focus, XMFLOAT3 Up)
{
	this->m_fEye = CameraPos;
	this->m_fFocus = Focus;
	this->m_fUp = Up;

	this->UpdateViewProj();
}

void Camera::SetFOV(float FieldOfView)
{
	this->m_fFOV = FieldOfView;
	this->UpdateViewProj();
}

void Camera::SetNearZ(float NearZ)
{
	this->m_fNearZ = NearZ;
	this->UpdateViewProj();
}

void Camera::SetFarZ(float FarZ)
{
	this->m_fFarZ = FarZ;
	this->UpdateViewProj();
}

void Camera::SetAspectRatio(float AspectRatio)
{
	this->m_fAspectRatio = AspectRatio;
	this->UpdateViewProj();
}

XMFLOAT3 Camera::GetEyePos() const
{
	return m_fEye;
}

XMMATRIX Camera::GetViewProj() const
{
	return ViewProj;
}

Camera::Camera(float FieldOfView, float Near, float Far, float AspectRatio)
	: m_fEye(XMFLOAT3(0.0f, 0.0f, -1.0f)), m_fFocus(XMFLOAT3(0.0f, 0.0f, 0.0f)), m_fUp(XMFLOAT3(0.0f, 1.0f, 0.0f)), m_fNearZ(Near), m_fFarZ(Far), m_fFOV(FieldOfView), m_fAspectRatio(AspectRatio)
{
	this->UpdateViewProj();
}

Camera::Camera()
	: m_fEye(XMFLOAT3(0.0f, 0.0f, 0.0f)), m_fFocus(XMFLOAT3(0.0f, 0.0f, 0.0f)), m_fUp(XMFLOAT3(0.0f, 0.0f, 0.0f)), m_fNearZ(0.0f), m_fFarZ(0.0f), m_fFOV(0.0f)
{
}

Camera::~Camera()
{
}

void Camera::UpdateViewProj()
{
	XMMATRIX Proj = XMMatrixPerspectiveFovLH(m_fFOV, m_fAspectRatio, m_fNearZ, m_fFarZ);
	
	XMVECTOR EyeV = XMLoadFloat3(&m_fEye);
	XMVECTOR FocusV = XMLoadFloat3(&m_fFocus);
	XMVECTOR UpV = XMLoadFloat3(&m_fUp);
	
	XMMATRIX View = XMMatrixLookAtLH(EyeV, FocusV, UpV);

	ViewProj = XMMatrixTranspose(View * Proj);
}
