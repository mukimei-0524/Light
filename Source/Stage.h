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

	// 行列更新処理
	void UpdateTransform();

	// インスタンス取得 
	static Stage& Instance();

private:
protected:
	Model* model = nullptr;
	DirectX::XMFLOAT3		position = { 0, 0, 0 };
	DirectX::XMFLOAT3		angle = { 0, 0, 0 };
	DirectX::XMFLOAT3		scale = { 1, 1, 1 };
	DirectX::XMFLOAT4X4		transform = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1
	};
};