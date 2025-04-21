#include <imgui.h>
#include "CameraController.h"
#include "Camera.h"
#include "Input/Input.h"
#include <windows.h>

// 更新処理
void CameraController::Update(float elapsedTime)
{
    Mouse& mouse = Input::Instance().GetMouse();

    // 画面中央の座標を取得
    RECT rect;
    GetClientRect(GetActiveWindow(), &rect);
    int centerX = rect.right / 2;
    int centerY = rect.bottom / 2;

    // マウスの現在位置を取得
    POINT mousePos;
    GetCursorPos(&mousePos);

    // 移動量を計算
    float mouseDx = mousePos.x - centerX;
    float mouseDy = mousePos.y - centerY;

    // カメラの回転更新
	const float mouseSensitivity = 0.0015f; // マウス感度
    angle.y += mouseDx * mouseSensitivity; // 横回転
    angle.x += mouseDy * mouseSensitivity; // 縦回転

    // X軸のカメラ回転制限
    angle.x = max(minAngleX, min(angle.x, maxAngleX));

    // カーソルを中央に戻す
    SetCursorPos(centerX, centerY);

    // マウスの現在位置を個別に取得
    float currentMousePosX = mouse.GetPositionX();
    float currentMousePosY = mouse.GetPositionY();

    // 前フレームの位置を更新
    previousMousePosX = currentMousePosX;
    previousMousePosY = currentMousePosY;

    ShowCursor(FALSE); // 毎フレームカーソルを非表示にする(念のため)

    // カメラ回転角度を更新
    angle.y += mouseDx * mouseSensitivity; // 横回転
    angle.x += mouseDy * mouseSensitivity; // 縦回転（逆向きにすることで自然な動きに）

    // X軸のカメラ回転を制限
    if (angle.x < minAngleX) {
        angle.x = minAngleX;
    }
    if (angle.x > maxAngleX) {
        angle.x = maxAngleX;
    }

    // カメラ回転値を回転行列に変換
    DirectX::XMMATRIX Transform = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, 0);

    // キャラクターの位置を基準にカメラの視点位置を設定
    DirectX::XMFLOAT3 eye = target;

    // 視点を少し上げる
    eye.y += 1.0f;

    // 視線方向ベクトルを計算
    DirectX::XMVECTOR Front = DirectX::XMVector3Normalize(Transform.r[2]);
    DirectX::XMStoreFloat3(&front, Front);

    // 視点を前に移動
    eye.x += front.x * 0.5f;
    eye.y += front.y * 0.5f;
    eye.z += front.z * 0.5f;

    // カメラ設定
    Camera::Instance().SetLookAt(eye, { eye.x + front.x, eye.y + front.y, eye.z + front.z }, DirectX::XMFLOAT3(0, 1, 0));
}

// デバッグ用GUI描画
void CameraController::DrawDebugGUI()
{
	if (ImGui::CollapsingHeader("CameraController", ImGuiTreeNodeFlags_DefaultOpen))
	{
		// アングル
		DirectX::XMFLOAT3 a;
		a.x = DirectX::XMConvertToDegrees(angle.x);
		a.y = DirectX::XMConvertToDegrees(angle.y);
		a.z = DirectX::XMConvertToDegrees(angle.z);
		ImGui::InputFloat3("Angle", &a.x);
		angle.x = DirectX::XMConvertToRadians(a.x);
		angle.y = DirectX::XMConvertToRadians(a.y);
		angle.z = DirectX::XMConvertToRadians(a.z);
		// ターゲット
		ImGui::InputFloat3("Target", &target.x);
		// 回転速度
		float rollSpeedDeg = DirectX::XMConvertToDegrees(rollSpeed);
		ImGui::InputFloat("RollSpeed", &rollSpeedDeg);
		rollSpeed = DirectX::XMConvertToRadians(rollSpeedDeg);
		// レンジ
		ImGui::InputFloat("Range", &range);
		// 角度制限
		float minAngleDeg = DirectX::XMConvertToDegrees(minAngleX);
		float maxAngleDeg = DirectX::XMConvertToDegrees(maxAngleX);
		ImGui::InputFloat("MinAngleX", &minAngleDeg);
		ImGui::InputFloat("MaxAngleX", &maxAngleDeg);
		minAngleX = DirectX::XMConvertToRadians(minAngleDeg);
		maxAngleX = DirectX::XMConvertToRadians(maxAngleDeg);
	}
}