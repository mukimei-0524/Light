#pragma once

#include <DirectXMath.h>

// カメラコントローラー
class CameraController
{
public:
	CameraController() {}
	~CameraController() {}

	// 更新処理
	void Update(float elapsedTime);

	// ターゲット位置設定
	void SetTarget(const DirectX::XMFLOAT3& target) { this->target = target; }

	// デバッグ用GUI描画
	void DrawDebugGUI();

	// frontのゲッター
	DirectX::XMFLOAT3		GetFront() const { return front; }

	DirectX::XMFLOAT3		angle = { 0, 0, 0 };
private:
	DirectX::XMFLOAT3		target = { 0, 0, 0 };
	DirectX::XMFLOAT3		front;
	float previousMousePosX = 0.0f; // X軸の前回のマウス座標
	float previousMousePosY = 0.0f; // Y軸の前回のマウス座標
	float					rollSpeed = DirectX::XMConvertToRadians(90);
	float					range = 10.0f;
	float					maxAngleX = DirectX::XMConvertToRadians(45);
	float					minAngleX = DirectX::XMConvertToRadians(-45);
};
