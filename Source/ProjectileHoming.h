#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"
#include "HomingMaker.h"

// �ǔ��e��
class ProjectileHoming : public Projectile
{
public:
	ProjectileHoming(ProjectileManager* manager);
	~ProjectileHoming() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
//	void Render(ID3D11DeviceContext* dc, ModelShader* shader) override;
	void Render(const RenderContext& rc, ModelShader* shader) override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& target);
private:
	Model*				model = nullptr;
	//DirectX::XMFLOAT3	target = { 0, 0, 0 };
	float				moveSpeed = 1.0f;
	float				turnSpeed = DirectX::XMConvertToRadians(90);
	float				lifeTimer = 60.0f;

	bool				beWalking = false;

	Model* Light_Line = nullptr;
};
