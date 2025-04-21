#include "Stage.h"

static Stage* instance = nullptr;

// �C���X�^���X�擾 
Stage& Stage::Instance()
{
	
	return *instance;
}

// �R���X�g���N�^
Stage::Stage()
{
	instance = this;
	// �X�e�[�W���f����ǂݍ���
	model = new Model("Data/Model/ExampleStage/test_stage.mdl");
}
Stage::Stage(const char* filename)
{
	instance = this;
	// �X�e�[�W���f����ǂݍ���
	model = new Model(filename);
}

Stage::~Stage()
{
	// �X�e�[�W���f����j��
	delete model;
}

// �X�V����
void Stage::Update(float elapsedTime)
{
	// ���͓��ɂ�邱�Ƃ͂Ȃ�
}

// ���C�L���X�g 
bool Stage::RayCast(const DirectX::XMFLOAT3& start, const DirectX::XMFLOAT3& end, HitResult& hit)
{
	return Collision::IntersectRayVsModel(start, end, model, hit);
}



// �`�揈��
void Stage::Render(const RenderContext& rc, ModelShader* shader)
{
	// �V�F�[�_�[�Ƀ��f����`�悵�Ă��炤
	shader->Draw(rc, model);
}