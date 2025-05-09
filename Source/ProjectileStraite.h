#pragma once

#include "Graphics/Model.h"
#include "Projectile.h"

// ���i�e��
class ProjectileStraite : public Projectile
{
public:
	ProjectileStraite(ProjectileManager* manager);
	~ProjectileStraite() override;

	// �X�V����
	void Update(float elapsedTime) override;

	// �`�揈��
	void Render(const RenderContext& rc, ModelShader* shader) override;

	// ����
	void Launch(const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);

private:
	Model*		model = nullptr;
	float		speed = 1.0f;
	float		lifeTimer = 10.0f;
};
