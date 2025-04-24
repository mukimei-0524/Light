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

    // 更新処理
    void Update(float elapsedTime) override;

    // 描画処理
    void Render(const RenderContext& rc, ModelShader* shader) override;

    // ターゲットの位置を設定
    void SetPosition(const DirectX::XMFLOAT3& pos);

    // ターゲットの位置を取得
    const DirectX::XMFLOAT3& GetPosition() const { return position; }

private:
    Model* model = nullptr;
};