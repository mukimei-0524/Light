#include "Stage.h"

static Stage* instance = nullptr;

// 行列更新処理
void Stage::UpdateTransform()
{
	// スケール行列を作成
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// 回転行列を作成
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// 位置行列を作成
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// ３つの行列を組み合わせ、ワールド行列を作成
	DirectX::XMMATRIX W = S * R * T;
	// 計算したワールド行列を取り出す
	DirectX::XMStoreFloat4x4(&transform, W);
}

// インスタンス取得 
Stage& Stage::Instance()
{
	
	return *instance;
}

// コンストラクタ
Stage::Stage()
{
	instance = this;
	// ステージモデルを読み込み
	model = new Model("Data/Model/ExampleStage/test_bord.mdl");
	scale.x = 20.0f;
}
Stage::Stage(const char* filename)
{
	instance = this;
	// ステージモデルを読み込み
	model = new Model(filename);
}

Stage::~Stage()
{
	// ステージモデルを破棄
	delete model;
}

// 更新処理
void Stage::Update(float elapsedTime)
{
	// オブジェクト行列を更新
	UpdateTransform();

	// モデル行列更新
	model->UpdateTransform(transform);
}

// レイキャスト 
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}

// 描画処理
void Stage::Render(const RenderContext& rc, ModelShader* shader)
{
	// シェーダーにモデルを描画してもらう
	shader->Draw(rc, model);
}