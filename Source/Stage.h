#pragma once

#include "Graphics/Shader.h"
#include "Graphics/Model.h"
#include "Collision.h"

// ステージ
class Stage
{
public:
	Stage();
	Stage(const char* filename);
	virtual ~Stage();

	// 更新処理
	virtual void Update(float elapsedTime);

	// 描画処理
	void Render(const RenderContext& rc, ModelShader* shader);

	// レイキャスト 
	bool RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit);

	// インスタンス取得 
	static Stage& Instance();

private:
protected:
	Model* model = nullptr;
};