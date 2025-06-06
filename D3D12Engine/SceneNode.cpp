#include "SceneNode.h"

SceneNode::SceneNode(XMFLOAT3 Position, XMFLOAT3 Scale, XMFLOAT3 Rotation)
{
	this->Position = Position;
	this->Scale = Scale;
	this->Rotation = Rotation;
}

SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}
