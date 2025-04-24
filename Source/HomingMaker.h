#pragma once

#include <DirectXMath.h>
#include "Graphics/Model.h"
#include "Graphics/Shader.h"
#include "Projectile.h"

class HomingTargetMarker:public Projectile
{
public:
    HomingTargetMarker(ProjectileManager* manager);
    ~HomingTargetMarker() override;

    // �X�V����
    void Update(float elapsedTime) override;

    // �`�揈��
    void Render(const RenderContext& rc, ModelShader* shader) override;

    // �^�[�Q�b�g�̈ʒu��ݒ�
    void SetPosition(const DirectX::XMFLOAT3& pos);

    // �^�[�Q�b�g�̈ʒu���擾
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

private:
    Model* model = nullptr;
};