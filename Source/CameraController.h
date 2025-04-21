#pragma once

#include <DirectXMath.h>

// �J�����R���g���[���[
class CameraController
{
public:
	CameraController() {}
	~CameraController() {}

	// �X�V����
	void Update(float elapsedTime);

	// �^�[�Q�b�g�ʒu�ݒ�
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// �f�o�b�O�pGUI�`��
	void DrawDebugGUI();

	// front�̃Q�b�^�[
	DirectX::XMFLOAT3		GetFront() const { return front; }

	DirectX::XMFLOAT3		angle = { 0, 0, 0 };
private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };
	DirectX::XMFLOAT3		front;
	float previousMousePosX = 0.0f; // X���̑O��̃}�E�X���W
	float previousMousePosY = 0.0f; // Y���̑O��̃}�E�X���W
	float					rollSpeed = DirectX::XMConvertToRadians(90);
	float					range = 10.0f;
	float					maxAngleX = DirectX::XMConvertToRadians(45);
	float					minAngleX = DirectX::XMConvertToRadians(-45);
};
