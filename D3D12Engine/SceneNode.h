#pragma once
#include <DirectXMath.h>
#include <vector>
using namespace std;
using namespace DirectX;
class SceneNode
{
private:
	std::vector<SceneNode*> m_children;
public:
	SceneNode(XMFLOAT3 Position, XMFLOAT3 Scale = XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3 Rotation = XMFLOAT3(1.0f, 1.0f, 1.0f));
	SceneNode();
	~SceneNode();

	XMFLOAT3 Position;
	XMFLOAT3 Rotation;
	XMFLOAT3 Scale;
};

