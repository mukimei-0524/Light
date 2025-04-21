#include "Stage.h"

static Stage* instance = nullptr;

// �s��X�V����
void Stage::UpdateTransform()
{
	// �X�P�[���s����쐬
	DirectX::XMMATRIX S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
	// ��]�s����쐬
	DirectX::XMMATRIX R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
	// �ʒu�s����쐬
	DirectX::XMMATRIX T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
	// �R�̍s���g�ݍ��킹�A���[���h�s����쐬
	DirectX::XMMATRIX W = S * R * T;
	// �v�Z�������[���h�s������o��
	DirectX::XMStoreFloat4x4(&transform, W);
}

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
	model = new Model("Data/Model/ExampleStage/test_bord.mdl");
	scale.x = 20.0f;
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
	// �I�u�W�F�N�g�s����X�V
	UpdateTransform();

	// ���f���s��X�V
	model->UpdateTransform(transform);
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