#include "HomingMaker.h"
#include <DirectXMath.h>

HomingTargetMarker::HomingTargetMarker(ProjectileManager* manager):Projectile(manager)
{
	model = new Model("Data/Model/Maker/Maker.mdl");

	scale = { 1.5f,1.0f,1.5f };

	//‰ñ“]‚³‚¹‚½‚­‚È‚¢ê‡‚ÍdirectionŒÅ’è
	direction = { 0.0f,0.0f,1.0f };

	UpdateTransform();
}

HomingTargetMarker::~HomingTargetMarker()
{
	delete model;
}

void HomingTargetMarker::Update(float elapsedTime)
{
	UpdateTransform();
}

void HomingTargetMarker::Render(const RenderContext& rc, ModelShader* shader)
{
	shader->Draw(rc, model);
}

void HomingTargetMarker::SetPosition(const DirectX::XMFLOAT3& pos)
{
	this->position = pos;
	UpdateTransform();
}
