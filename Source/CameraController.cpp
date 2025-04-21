#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include <windows.h>

// �X�V����
void CameraController::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    // ��ʒ����̍��W���擾
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;

    // �}�E�X�̌��݈ʒu���擾
    POINT mousePos;
    GetCursorPos(&mousePos);

    // �ړ��ʂ��v�Z
    float mouseDx = mousePos.x - centerX;
    float mouseDy = mousePos.y - centerY;

    // �J�����̉�]�X�V
	const float mouseSensitivity = 0.0015f; // �}�E�X���x
    angle.y += mouseDx * mouseSensitivity; // ����]
    angle.x += mouseDy * mouseSensitivity; // �c��]

    // X���̃J������]����
    angle.x = max(minAngleX, min(angle.x, maxAngleX));

    // �J�[�\���𒆉��ɖ߂�
    SetCursorPos(centerX, centerY);

    // �}�E�X�̌��݈ʒu���ʂɎ擾
    float currentMousePosX = mouse.GetPositionX();
    float currentMousePosY = mouse.GetPositionY();

    // �O�t���[���̈ʒu���X�V
    previousMousePosX = currentMousePosX;
    previousMousePosY = currentMousePosY;

    ShowCursor(FALSE); // ���t���[���J�[�\�����\���ɂ���(�O�̂���)

    // �J������]�p�x���X�V
    angle.y += mouseDx * mouseSensitivity; // ����]
    angle.x += mouseDy * mouseSensitivity; // �c��]�i�t�����ɂ��邱�ƂŎ��R�ȓ����Ɂj

    // X���̃J������]�𐧌�
    if (angle.x < minAngleX) {
        angle.x = minAngleX;
    }
    if (angle.x > maxAngleX) {
        angle.x = maxAngleX;
    }

    // �J������]�l����]�s��ɕϊ�
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, 0);

    // �L�����N�^�[�̈ʒu����ɃJ�����̎��_�ʒu��ݒ�
    DirectX::XMFLOAT3 eye = target;

    // ���_�������グ��
    eye.y += 1.0f;

    // ���������x�N�g�����v�Z
    DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(Transform.r[2]);
    DirectX::XMStoreFloat3(&front, Front);

    // ���_��O�Ɉړ�
    eye.x += front.x * 0.5f;
    eye.y += front.y * 0.5f;
    eye.z += front.z * 0.5f;

    // �J�����ݒ�
    Camera::Instance().SetLookAt(eye, { eye.x + front.x, eye.y + front.y, eye.z + front.z }, DirectX::XMFLOAT3(0, 1, 0));
}

// �f�o�b�O�pGUI�`��
void CameraController::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// �A���O��
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// �^�[�Q�b�g
		ImGui::InputFloat3("Target", &target.x);
		// ��]���x
		float rollSpeedDeg = DirectX::XMConvertToDegrees(rollSpeed);
		ImGui::InputFloat("RollSpeed", &rollSpeedDeg);
		rollSpeed = DirectX::XMConvertToRadians(rollSpeedDeg);
		// �����W
		ImGui::InputFloat("Range", &range);
		// �p�x����
		float minAngleDeg = DirectX::XMConvertToDegrees(minAngleX);
		float maxAngleDeg = DirectX::XMConvertToDegrees(maxAngleX);
		ImGui::InputFloat("MinAngleX", &minAngleDeg);
		ImGui::InputFloat("MaxAngleX", &maxAngleDeg);
		minAngleX = DirectX::XMConvertToRadians(minAngleDeg);
		maxAngleX = DirectX::XMConvertToRadians(maxAngleDeg);
	}
}