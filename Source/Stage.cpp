#include "Stage.h"

static Stage* instance = nullptr;

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
	model = new Model("Data/Model/ExampleStage/test_stage.mdl");
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
	// 今は特にやることはない
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