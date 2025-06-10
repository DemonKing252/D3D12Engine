#pragma once
#include <DirectXMath.h>
using namespace DirectX;
class Camera
{
public:
	void SetLens(XMFLOAT3 CameraPos, XMFLOAT3 Focus = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 Up = XMFLOAT3(0.0f, 1.0f, 0.0f));
	void SetFOV(float FieldOfView);
	void SetNearZ(float NearZ);
	void SetFarZ(float FarZ);
	void SetAspectRatio(float AspectRatio);

	XMFLOAT3 GetEyePos() const;
	XMMATRIX GetViewProj() const;

	Camera(float FieldOfView, float Near, float Far, float AspectRatio);
	Camera();
	~Camera();
private:
	void UpdateViewProj();

	XMFLOAT3 m_fEye;
	XMFLOAT3 m_fUp;
	XMFLOAT3 m_fFocus;

	XMMATRIX ViewProj;
	
	float m_fAspectRatio;
	float m_fNearZ;
	float m_fFarZ;
	float m_fFOV;

};

